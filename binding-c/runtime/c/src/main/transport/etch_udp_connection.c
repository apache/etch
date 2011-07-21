/* $Id$ 
 * 
 * Licensed to the Apache Software Foundation (ASF) under one or more 
 * contributor license agreements. See the NOTICE file distributed with  
 * this work for additional information regarding copyright ownership. 
 * The ASF licenses this file to you under the Apache License, Version  
 * 2.0 (the "License"); you may not use this file except in compliance  
 * with the License. You may obtain a copy of the License at 
 * 
 * http://www.apache.org/licenses/LICENSE-2.0 
 * 
 * Unless required by applicable law or agreed to in writing, software 
 * distributed under the License is distributed on an "AS IS" BASIS, 
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and 
 * limitations under the License. 
 */ 

/*
 * etch_udpconxn.c
 * udp connection class
 */

#include "etch_thread.h"
#include "etch_udp_connection.h"
#include "etch_encoding.h"
#include "etch_flexbuffer.h"
#include "etch_log.h"
#include "etch_objecttypes.h"
#include "etch_connection_event.h"
#include "etch_inet_who.h"

static const char* LOG_CATEGORY = "etch_udp_connection";

// extern types
extern apr_pool_t*         g_etch_main_pool;
extern apr_thread_mutex_t* g_etch_main_pool_mutex;

/*
 * is_good_tcp_params()
 */
int is_good_udp_params(etch_url* url, void* resources, etch_rawsocket* socket)
{
    return is_good_conn_params(url, resources, socket);
}

/**
 * etch_udpconx_set_socket_options()
 */
int etch_udpconx_set_socket_options(void* data)
{
   etch_udp_connection *c = (etch_udp_connection*)data;
   int arc = 0, ecount = 0;
   etch_connection_event_handler eventx;
   etch_rawsocket* socket = c? c->cx.socket: NULL;
	if (!socket) return -1;
      eventx = c->cx.on_event;
      
   if (0 != (arc = apr_socket_opt_set(socket, APR_SO_REUSEADDR, c->is_reuseaddr)))
      ecount += eventx(c, ETCH_CONXEVT_SOCKOPTERR, arc, "reuseaddr");

#if defined(APR_SO_BROADCAST)
   if (0 != (arc = apr_socket_opt_set(socket, APR_SO_BROADCAST, c->is_broadcast)))
      ecount += eventx(c, ETCH_CONXEVT_SOCKOPTERR, arc, "broadcast");
#else
   if (c->is_broadcast)
      ETCH_LOG(LOG_CATEGORY, ETCH_LOG_WARN, "Your APR version doesn't support setting SO_BROADCAST flag.\n");
#endif

   return ecount == 0 ? 0: -1;
}


/**
 * etch_udpclient_on_data()
 * udp socket received data handler.  
 * @param cx the connection object.
 * @param unused parameter not currently used.
 * @param length number of bytes in the supplied data buffer.
 * @param data the data as received via the socket wrapped in a flexbuffer.
 * caller retains this memory.
 * @remarks todo: if this remains the same as etch_udpsvr_on_data, replace both 
 * methods with a etch_udpconx_on_data() containing the same code.
 */
int etch_udpclient_on_data (void* thisData, const int unused, int length, void* bufferData)
{
    etch_connection* cx = (etch_connection*)thisData;
    etch_flexbuffer* data = (etch_flexbuffer*)bufferData;
    int result = 0;
    i_sessionpacket* session = NULL;
    etch_inet_who *whofrom = NULL;
    apr_sockaddr_t *sockaddr = NULL;
    etch_udp_connection* udpx = cx? (etch_udp_connection*) cx->owner: NULL;

    ETCH_ASSERT(is_etch_udpconnection(udpx));
    ETCH_ASSERT(is_etch_flexbuffer(data));
    
    session = udpx->session;
    whofrom = new_inet_who(udpx->remote_addr);
                                           
    /* send the data up the chain to be packetized. note that udpx->session->thisx
     * is the owner of the i_sessionpacket* session, which is the next higher layer 
     * of the transport stack, which is ordinarily the packetizer.  
     */
    if (-1 == (result = session->session_packet(session->thisx, whofrom, data)))
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "current %d bytes on connxn %d rejected\n", length, cx->conxid);
    return result;
}

/*
 * new_udp_connection()
 * etch_udp_connection udp client constructor
 */
etch_udp_connection* new_udp_connection(etch_url* url, void* resources, etch_rawsocket* socket)
{
    int result = -1, item = 0;
    etch_udp_connection* newcon = NULL;    
    if (!is_good_udp_params (url, resources, socket)) return NULL;

    newcon = (etch_udp_connection*)new_object (sizeof(etch_udp_connection), ETCHTYPEB_CONNECTION, CLASSID_UDP_CONNECTION);
    ((etch_object*)newcon)->destroy = destroy_etch_udp_connection;

    newcon->is_server = resources == NULL;
    newcon->remote_addr = NULL;
    do  /* populate connection's transport and session interfaces */
    {   if (-1 == (result = init_etch_udpconx_interfaces(newcon))) break;

        if (-1 == (result = etch_init_connection (&newcon->cx, socket, newcon))) break;

        newcon->cx.set_socket_options = etch_udpconx_set_socket_options;
        newcon->cx.on_event = etch_def_connection_on_event;  /* connection state handler */

        if (socket)
            newcon->cx.socket = socket;
        else
        {   
            // TODO: pool
            etch_encoding_transcode_wchar(&newcon->cx.hostname, ETCH_ENCODING_UTF8, url->host, NULL);
            newcon->cx.port = url->port;
        }
        
        /* set term default values - values for any terms not set here are zero */

        /* set any terms which may have been supplied with URL */
        etchurl_get_boolean_term(url, ETCH_CONNECTION_REUSE_PORT,  &newcon->is_reuseaddr);
        etchurl_get_boolean_term(url, ETCH_CONNECTION_BROADCAST,  &newcon->is_broadcast);

        result = 0;

    } while(0);

    newcon->cx.on_data = etch_udpclient_on_data;
    newcon->cx.on_event(newcon, result? ETCH_CONXEVT_CREATERR: ETCH_CONXEVT_CREATED, 0, 0);

    if (-1 == result)
    {   destroy_etch_udp_connection(newcon);
        return NULL;
    }       
    else return newcon;
}

/**
 * etch_client_get_session
 * i_transport::get_session implementation
 */
i_session* etch_udpclient_get_session (void* data)
{
    etch_udp_connection* thisx = (etch_udp_connection*)data;
    ETCH_ASSERT(is_etch_udpconnection(thisx));
    return (i_session*)thisx->session;
}

/**
 * etch_udpclient_set_session()
 * i_transport::set_session() override
 * @param session an i_sessionpacket*. caller retains this object.
 */
void etch_udpclient_set_session (void* data, void* newsession)
{
    etch_udp_connection* thisx = (etch_udp_connection*)data;
    ETCH_ASSERT(is_etch_udpconnection(thisx));
    ETCH_ASSERT(is_etch_sessionpacket(newsession));
    if (thisx->is_session_owned){
        etch_object_destroy(thisx->session);
        thisx->session = NULL;
    }
    thisx->is_session_owned = FALSE;
    thisx->session = (i_sessionpacket*)newsession;
}

/**
 * etch_udpconx_transport_control()
 * connection::i_transport::transport_control override.
 * this is the base connection class' implementation of i_transport.
 * this is java binding's Connection.transportControl(), and serves as the
 * Transport part of the java UdpConnection TransportData. 
 * while udp connection does implement i_transportdata, udp connection's
 * implementation of i_transport comes from its inheritance of connection,
 * and its implementation of TransportData. since we do not separately implement
 * the connection class, the i_transport methods are implemented here.
 * @param control the event, sender relinquishes.
 * @param value control value, sender relinquishes.
 */
int etch_udpconx_transport_control (void* data, etch_event* control, etch_object* value)
{
    etch_udp_connection* thisx = (etch_udp_connection*)data;
    etch_connection* cx = NULL;
    int result = 0, timeoutms = 0;
    const int objclass  = control? ((etch_object*)control)->class_id: 0;
    const int is_client = is_etch_int32(value)? ((etch_int32*)value)->value: NULL;
    ETCH_ASSERT(is_etch_udpconnection(thisx));
    cx = &thisx->cx;
    

    switch(objclass)
    {  
       case CLASSID_CONTROL_START:

            result = etch_udpconx_start (thisx);  

            if (0 == result)
                result = etch_udpclient_start_listener (thisx);  
            break; 
           
       case CLASSID_CONTROL_START_WAITUP: 
 
            /* open the connection, and wait for completion. caller blocks by virtue
             * of the fact that this is of course a function call, not a message handler.
             * timeout is communicated to caller via result code 1 = ETCH_TIMEOUT. 
             * it is not clear why wait up is implemented here. since a udp server
             * implements transport interface itself, a server will never invoke this
             * implementation. on the other hand, the requester of a client connection  
             * and the socket itself are the same thread, wait up therefore being
             * meaningless since the socket open is known to be complete prior to
             * invoking wait up.
             */
            if (0 == (result = etch_udpconx_open (thisx, ETCH_CONX_NOT_RECONNECTING)))
            {   timeoutms = value? ((etch_int32*) value)->value: 0;         
                result = etchconx_wait_up (cx, timeoutms);
            }
            break;  

       case CLASSID_CONTROL_STOP:  

            if (is_client)
                result = etch_udpclient_stop_listener (thisx);  
            else
                result = etch_udpconx_close (thisx);
            break;

       case CLASSID_CONTROL_STOP_WAITDOWN: 
 
            /* see comments above at CLASSID_CONTROL_START_WAITUP */
            if (0 == (result = etch_udpconx_close (thisx)))
            {   timeoutms = value? ((etch_int32*) value)->value: 0;
                result = etchconx_wait_down (cx, timeoutms);
            }
            break;
    }

    etch_object_destroy(control);
    etch_object_destroy(value);
    return result;
}

/**
 * etch_udpconx_transport_notify()
 * i_transport::transport_notify override.
 * @param evt, caller relinquishes.
 */
int etch_udpconx_transport_notify (void* data, etch_event* evt)
{
    etch_udp_connection* thisx = (etch_udp_connection*)data;
    ETCH_ASSERT(is_etch_udpconnection(thisx));
    etch_object_destroy(evt);
    return 0;  /* nothing to do */
}



/**
 * etch_udpconx_transport_query()
 * i_transport::transport_query override.
 * @param query, caller relinquishes.
 */
etch_object* etch_udpconx_transport_query (void* data, etch_query* query) 
{
    etch_udp_connection* thisx = (etch_udp_connection*)data;
    int result = 0;
    etch_object*  resultobj = NULL;
    etch_connection* cx = NULL;
    const int timeoutms = query? query->value: 0;
    const int objclass  = query? ((etch_object*)query)->class_id: 0;
    ETCH_ASSERT(is_etch_udpconnection(thisx));
    cx = &thisx->cx;

    switch(objclass)
    {  
       case CLASSID_QUERY_LOCALADDR:
            /* TODO return wrapped local address */
            break; 
           
       case CLASSID_QUERY_REMOTEADDR:  
            /* TODO return wrapped remote address */
            break;  

       case CLASSID_QUERY_WAITUP:   
            result = etchconx_wait_up(cx, timeoutms);
            break;

       case CLASSID_QUERY_WAITDOWN:  
            result = etchconx_wait_down(cx, timeoutms);
            break;
    }

    etch_object_destroy(query);
    return resultobj;
}


/*
 * etch_udpclient_sendex()
 * send data with specified timeout
 */
int etch_udpclient_sendex (etch_udp_connection *conx, etch_who* whoto, unsigned char* buf, 
    const size_t totallen, const int timeout_ms, int* rc)
{
    int arc = 0, is_eod = 0;
    int64 existing_timeout_us = 0;
    apr_size_t datalen = 0, totalsent = 0, remaining = totallen;
    etch_connection *cx  = conx? &conx->cx:  NULL;
    etch_rawsocket* socket = cx? cx->socket: NULL;
	if (!socket) return -1;
    cx->on_event(conx, ETCH_CONXEVT_SENDING, 0, 0);

    if (timeout_ms) 
    {   apr_socket_timeout_get(cx->socket, &existing_timeout_us);
        apr_socket_timeout_set(cx->socket, timeout_ms * 1000);
    }

    /* note socket currently blocking write with no timeout */

    while(cx->is_started && !is_eod && remaining > 0)
    {
        datalen = totallen;
       
        if (is_etch_inet_who(whoto))
           is_eod = (APR_EOF == (arc = apr_socket_sendto(socket,
              inet_who_sockaddr((etch_inet_who*)whoto), 0, (char*)(buf + totalsent), &datalen)));
        else
           is_eod = (APR_EOF == (arc = apr_socket_send(socket, (char*)(buf + totalsent), &datalen)));

        totalsent += datalen; remaining -= datalen;

        if (arc != 0 && !is_eod)
        {   cx->on_event(conx, ETCH_CONXEVT_SENDERR, arc, 0);  
            break;
        }

        cx->on_event(conx, ETCH_CONXEVT_SENT, is_eod, (char*)datalen);
    }

    if (timeout_ms) /* restore socket timeout property */
        apr_socket_timeout_set(cx->socket, existing_timeout_us);

    cx->on_event(conx, ETCH_CONXEVT_SENDEND, (int)totalsent, 0); 
    if (rc) *rc = arc;
    return remaining > 0? -1: 0;
}

/*
 * etch_udpclient_send()
 */
int etch_udpclient_send (etch_udp_connection *conx, etch_who* whoto, unsigned char* buf, const size_t totallen, int* rc)
{
    return etch_udpclient_sendex(conx, whoto, buf, totallen, 0, rc);
}


/*
 * etch_udpconx_transport_packet()
 * etch_udp_connection::i_transportpacket::transport_packet
 * @param whoto caller retains
 * @param fbuf caller retains 
 */
int etch_udpconx_transport_packet (void* data, etch_who* whoto, etch_flexbuffer* fbuf)
{
    etch_udp_connection* thisx = (etch_udp_connection*)data;
    int result = 0, apr_rc = 0;
    ETCH_ASSERT(is_etch_udpconnection(thisx));

    result = etch_udpclient_send (thisx, whoto, fbuf->buf, fbuf->datalen, &apr_rc);
    
    etch_flexbuf_reset(fbuf);
    return result;
} 

/*
 * init_etch_udpcon_interfaces()
 * populate transport and placeholder session interfaces to udp connection.
 */
int init_etch_udpconx_interfaces (etch_udp_connection* udpx)
{
    i_transport* itransport = NULL;
    ETCH_ASSERT(is_etch_udpconnection(udpx));
    if (udpx->itp) return 0;  /* already initialized */

    itransport = new_transport_interface_ex (udpx, 
        etch_udpconx_transport_control, 
        etch_udpconx_transport_notify, 
        etch_udpconx_transport_query,
        etch_udpclient_get_session,
        etch_udpclient_set_session);

    udpx->itp = new_transportpkt_interface(udpx, 
					     (etch_transport_packet)etch_udpconx_transport_packet, itransport);  /* itp now owns itransport */
    udpx->itp->header_size = 0;

    /* establish placeholder session interface which is expected   
     * to be replaced by the connection host (e.g. messagizer) */
    udpx->session = new_sessionpkt_interface (udpx, NULL, NULL);
    udpx->is_session_owned = TRUE;

    return 0;
}


/**
 * etch_udpconx_start()
 * start means open. generally we would come through here with an accepted socket,
 * in which case it is currently marked already open and we will return success. 
 * @return 0 success, -1 failure.
 */
int etch_udpconx_start (etch_udp_connection *conx)
{
    etch_connection* cx = conx? &conx->cx: NULL;
    ETCH_ASSERT(cx);
    cx->on_event (conx, ETCH_CONXEVT_STARTING, 0, 0);  
    if (cx->is_started) return 0;

    return etch_udpconx_open (conx, ETCH_CONX_NOT_RECONNECTING);
}


/**
 * etch_udpconx_open()
 * open connection to server based on host name/port set at construction.
 * @note we have omitted reconnect logic for now, pending logic to detect 
 * listen socket down and initiate reconnect.
 * @return 0 success, -1 failure (already open, hostname or socket error, etc)
 */
int etch_udpconx_open(etch_udp_connection *conx, const int is_reconnect)
{
	int result = -1, arc = 0, attempt = 0, is_already_open = TRUE;
    apr_status_t        apr_status;
	apr_interval_time_t apr_timeout;
    etch_connection* cx = &conx->cx;
    etch_connection_event_handler eventx = cx->on_event;
    eventx(conx, ETCH_CONXEVT_OPENING, 0, 0);   

    do 
    {   if (cx->is_started) break;
        is_already_open = FALSE;

        apr_thread_mutex_lock(g_etch_main_pool_mutex);
        if (conx->remote_addr == NULL) {
           conx->remote_addr = (apr_sockaddr_t *)apr_pcalloc(g_etch_main_pool, sizeof(apr_sockaddr_t));
           conx->remote_addr->pool = g_etch_main_pool;
        }
        arc = apr_sockaddr_info_get(&cx->sockdata, cx->hostname, ETCH_DEFAULT_SOCKET_FAMILY, cx->port, 0, g_etch_main_pool);
        apr_thread_mutex_unlock(g_etch_main_pool_mutex);
        if (0 != arc) {
            eventx(conx, ETCH_CONXEVT_OPENERR, 4, (void*)(size_t)arc);  
            break;
        }

        if (!cx->socket)
        {
            if (0 != (arc = new_udpsocket (&cx->socket, cx->aprpool)))
            {   eventx(conx, ETCH_CONXEVT_OPENERR, 3, (void*)(size_t)arc);  
                break;
            }

            /* set socket options here: NONBLOCK, TIMEOUT */ 
            cx->set_socket_options(conx);
        }
        
		apr_status = apr_socket_timeout_get(cx->socket, &apr_timeout);
		if(apr_status != APR_SUCCESS){
			char buffer[1024];
			apr_strerror(apr_status, buffer, sizeof(buffer));
			ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "could not get socket options: %s\n", buffer);
		}
		
		apr_status = apr_socket_timeout_set(cx->socket, 5 * APR_USEC_PER_SEC);
		if(apr_status != APR_SUCCESS){
			char buffer[1024];
			apr_strerror(apr_status, buffer, sizeof(buffer));
			ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "could not set socket options: %s\n", buffer);
		}

      if (conx->is_server) {
         apr_status = apr_socket_bind (cx->socket, cx->sockdata);
         if(apr_status != APR_SUCCESS){
            char buffer[1024];
            apr_strerror(apr_status, buffer, sizeof(buffer));
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "could not bind server: %s\n", buffer);
         } else {
            cx->is_started = TRUE;
         }
      } else 
        while(attempt++ < ETCH_CONNECTION_DEFRETRYATTEMPTS+1)   
        {   /* possibly todo: configure number of retry attempts */
            /* open socket */
			
            apr_status = apr_socket_connect (cx->socket, cx->sockdata);
            if(apr_status != APR_SUCCESS){
                char buffer[1024];
                apr_strerror(apr_status, buffer, sizeof(buffer));
                ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "could not connect to server reson: %s\n", buffer);
            } else {
                cx->is_started = TRUE;
                break;
            }

            cx->on_event(conx, ETCH_CONXEVT_OPENERR, 2, (void*)(size_t)arc); 
            etch_sleep(ETCH_CONNECTION_DEFRETRYDELAYMS);
	    }

		apr_status = apr_socket_timeout_set(cx->socket, apr_timeout);
		if(apr_status != APR_SUCCESS){
			char buffer[1024];
			apr_strerror(apr_status, buffer, sizeof(buffer));
			ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "could not set socket options: %s\n", buffer);
		}

    } while(0);

    if (cx->is_started && !is_already_open) result = 0;
    eventx(conx, result? ETCH_CONXEVT_OPENERR: ETCH_CONXEVT_OPENED, 0, 0);

    if (cx->wait_up) {

        /* java binding inserts a flow of control discontinuity here by assigning
         * the responsibility for notifying the session of the new connection state
         * to the separate thread it calls the todo manager (see jave fireUp()), 
         * the purpose being to avoid a race condition by delaying the state change 
         * until after return from this method. we may eventually have to implement 
         * something like this, but as of this writing we are not doing so. the fact
         * that we change state and then unblock as two separate actions here may
         * well preclude the need for such a discontinuity here.
         */

        /* notify session that connection is up (see comment above) */
        /* fyi: conx->session->thisx is packetizer */
        if (0 == result) {
            conx->session->session_notify (conx->session->thisx, new_etch_event(0, ETCHEVT_SESSION_UP));
        }

        /* unblock the thread waiting for connection to come up */
        /* todo permit unblock with something other than UP, when result != 0 */
        etch_wait_set(cx->wait_up, ETCH_CONXEVT_UP);
    }

	return result;  
}

/*
 * etch_udpcconx_closex()
 * close udp connection
 * @param is_dtor true only if this call is from the udp connection destructor.
 * @return 0 success, -1 failure.
 */
int etch_udpconx_closex(etch_udp_connection* conx, const int is_dtor)
{
    etch_status_t status = ETCH_SUCCESS;
    int result = 0, arc = 0, is_locked = 0, is_teardown = 0, is_logged = 0;
    etch_connection* cx = 0;
    if (NULL == conx) return -1;
    cx = &conx->cx;
    is_teardown = is_dtor && !cx->is_started;

    do {

        if (is_teardown) 
            break;
        cx->on_event(conx, ETCH_CONXEVT_CLOSING, 0, 0); 
     
        if (cx->socket) 
        {         
            if (cx->is_closing || !cx->is_started) 
            {   cx->on_event(conx, ETCH_CONXEVT_CLOSERR, 1, 0);
                is_logged = TRUE;
            }
        }

        status = etch_mutex_trylock(cx->mutex);
        if(status != ETCH_SUCCESS) {
            cx->on_event(conx, ETCH_CONXEVT_CLOSERR, 2, 0);
            is_logged = TRUE;
            break;
        }

        is_locked = TRUE;
        cx->is_closing = TRUE; 
        cx->is_started = FALSE;

        if (NULL != cx->socket) {
           apr_socket_shutdown(cx->socket, APR_SHUTDOWN_READWRITE);
           if (0 != (arc = apr_socket_close(cx->socket))) {
             cx->on_event(conx, ETCH_CONXEVT_CLOSERR, 3, (void*)(size_t)arc);
             result = -1;
           }
        }

        // close receiver thread
        if (conx->rcvlxr != NULL) {
           etch_join(conx->rcvlxr->thread);
        }

        cx->socket = NULL;
        cx->is_closing = FALSE;

        /* anything else we may need to do on close here */

    } while(0);

    if (is_locked) {
        etch_mutex_unlock(cx->mutex);
    }

    if (!is_teardown && !is_logged)
         cx->on_event(conx, result? ETCH_CONXEVT_CLOSERR: ETCH_CONXEVT_CLOSED, 0, 0);

    if (cx->wait_down) {
        /* if another thread is blocking on this condition variable,
         * we set the condition to DOWN, and UNBLOCK all waiters. */
        etch_wait_set(cx->wait_down, ETCH_CONXEVT_DOWN);
    }

    return result; 
}

/*
 * etch_udpcconx_close()
 * close udp connection
 */
int etch_udpconx_close(etch_udp_connection* conx)
{
    return etch_udpconx_closex(conx, FALSE);
}

/*
 * destroy_etch_udp_connection()
 * etch_udp_connection destructor
 */
int destroy_etch_udp_connection(void* thisx)
{
    etch_udp_connection* udpx = (etch_udp_connection*)thisx;
    if (NULL == udpx) return -1;
    udpx->cx.on_event(udpx, ETCH_CONXEVT_DESTROYING, 0, 0);
               
    etch_udpconx_closex (udpx, TRUE);  /* close if open */

    if (!is_etchobj_static_content(udpx)) {
        
        /* free listener if any */
        etch_object_destroy(udpx->rcvlxr);
        udpx->rcvlxr = NULL;

        /* free mem owned by udpx */
        etch_destroy_connection (&udpx->cx);

        /* free session interface */
        if (udpx->is_session_owned) {
            etch_object_destroy(udpx->session);
            udpx->session = NULL;
        }

        /* free transport interface */
        etch_object_destroy(udpx->itp);
        udpx->itp = NULL;
    }

    udpx->cx.on_event(udpx, ETCH_CONXEVT_DESTROYED, 0, 0);
    return destroy_objectex((etch_object*)udpx);
}


/*
 * new_udpsocket()
 */
int new_udpsocket (apr_socket_t** outsock, apr_pool_t* mempool)
{
    int rv = 0;
    apr_thread_mutex_lock(g_etch_main_pool_mutex);
    rv = apr_socket_create (outsock, APR_INET, SOCK_DGRAM, APR_PROTO_UDP, g_etch_main_pool);
    apr_thread_mutex_unlock(g_etch_main_pool_mutex);
    
    return rv;
} 





/*
 * etch_udpclient_receive()
 * receive data on socket
 * returns length received or -1
 */
int etch_udpclient_receive (etch_udp_connection *udpx, unsigned char* buf, const size_t buflen, int* rc)
{
    return etch_udpclient_receivex (udpx, buf, buflen, 0, rc);
}


/*
 * etch_udpclient_receivex()
 * receive data on socket with specified timeout, into specified character buffer.
 * @return number of bytes received on success; otherwise -2 (ETCH_OTHER_END_CLOSED)
 * if peer closed, or -1 if error. 
 */
int etch_udpclient_receivex (etch_udp_connection *udpx, unsigned char* buf, const size_t buflen, const int timeout_ms, int* rc)
{
    int result = 0, arc = 0, is_eod = 0, eventid = 0;
    int64 existing_timeout_us = 0;
    apr_size_t datalen = 0; 
    etch_connection *cx = udpx? &udpx->cx: NULL;
    if (NULL == cx) return -1;
    cx->on_event(udpx, ETCH_CONXEVT_RECEIVING, 0, 0);

    if (timeout_ms) {
        apr_socket_timeout_get(cx->socket, &existing_timeout_us);
        apr_socket_timeout_set(cx->socket, timeout_ms * 1000);
    }

    datalen = buflen; /* BLOCK on receive data here */
    
    arc = apr_socket_recvfrom (udpx->remote_addr, cx->socket, 0, (char*)buf, &datalen);
    is_eod = arc == APR_EOF;

    if (arc && !is_eod) {
        switch(arc) {
            case APR_OTHER_END_CLOSED:
                eventid = ETCH_CONXEVT_PEERCLOSED; 
                result  = ETCH_OTHER_END_CLOSED; 
                break;
            case APR_THIS_END_CLOSED:
                eventid = ETCH_CONXEVT_CONXCLOSED; 
                result  = ETCH_THIS_END_CLOSED;
                break;
            default: 
                eventid = ETCH_CONXEVT_RECEIVERR;
                result = -1;
        }
        cx->on_event (udpx, eventid, arc, 0); 
        
    }
    else
    if (0 == datalen)
    {   cx->on_event(udpx, ETCH_CONXEVT_PEERCLOSED, 0, 0);
        result = ETCH_OTHER_END_CLOSED;
    }
    else /* check for signal to shut down server */
    if (datalen == ETCH_SHUTDOWNSIGNALSIZE 
     && 0 == memcmp(buf, ETCH_SHUTDOWNSIGNAL, ETCH_SHUTDOWNSIGNALSIZE))
    {   cx->on_event(udpx, ETCH_CONXEVT_SHUTDOWN, 0, 0);
        result = ETCH_SHUTDOWN_NOTIFIED;
    }
    else
    {   cx->on_event (udpx, ETCH_CONXEVT_RECEIVED, is_eod, (char*)datalen);   
        if (-1 != result) result = (int) datalen;  /* return bytecount */
    }

    if (timeout_ms) /* restore socket timeout property */
        apr_socket_timeout_set(cx->socket, existing_timeout_us);
     
    if (rc) *rc = arc;
    return result;
}


static etch_status_t etch_udp_client_cleanup(void* p)
{
    etch_status_t    rv     = ETCH_SUCCESS;
    etch_status_t    status = ETCH_SUCCESS;
    etch_udp_client* client = (etch_udp_client *)p;

    status = etch_object_destroy(client->thread);
    // TODO: check status
    client->thread = NULL;

    status = destroy_objectex((etch_object*)client);
    // TODO: check status

    rv = status;

    return rv;
}


/**
 * destroy_etch_udp_client()
 * udp client (udp connection read listener) destructor.
 */
int destroy_etch_udp_client(void* data)
{
    etch_udp_client* thisx = (etch_udp_client*)data;
    etch_status_t rv     = ETCH_SUCCESS;
    rv = etch_udp_client_cleanup(thisx);
    return rv;
}


/**
 * etch_udpclient_listenerproc()
 * udp socket receive thread procedure. 
 */
static void etch_udp_client_receiver_proc(void* data)
{ 
    etch_thread_params* params = (etch_thread_params*)data;
    int result = 0, arc = 0;
    etch_udp_connection* udpx = (etch_udp_connection*) params->data;
    etch_connection* cx = &udpx->cx;
    const int thread_id = params->etch_thread_id;
    const int blen = cx->bufsize? cx->bufsize: ETCH_CONX_DEFAULT_BUFSIZE;
    //params->data->threas = params->threadob
    etch_flexbuffer* fbuf = new_flexbuffer(blen);  
    cx->on_event(udpx, ETCH_CONXEVT_RCVPUMP_START, 0, 0); 

    while(cx->is_started)
    {
        etch_flexbuf_clear(fbuf);  /* for debugging otherwise unnecessary */
        cx->on_event(udpx, ETCH_CONXEVT_RCVPUMP_RECEIVING, thread_id, 0);

        /* receive data from udp socket into buffer owned by flexbuffer.
         * note that if this receive were to stop blocking, for example 
         * if the peer went down without it being detected here, we would
         * see unfettered looping of this listener procedure. BLOCK.
         */
        result = etch_udpclient_receive (udpx, fbuf->buf, blen, &arc);   

        if (!cx->is_started) break;  /* client shutdown */
        
        if (result < 0) {  
           // print error message, but ignore it and continue
           cx->on_event(udpx, ETCH_CONXEVT_RCVPUMP_ERR, arc, 0);
        }

        etch_flexbuffer_reset_to (fbuf, result); /* received (result) bytes */

        if (result > 0) /* forward (result) bytes to data handler for packetization */ 
            cx->on_data (cx, 0, result, fbuf);     
    }

    udpx->session->session_notify (udpx->session->thisx, new_etch_event(0, ETCHEVT_SESSION_DOWN));
    
    cx->on_event(udpx, ETCH_CONXEVT_RCVPUMP_STOP, result, (void*) (size_t) thread_id);
    etch_object_destroy(fbuf);  
    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "leaving listener thread ...\n");   
}

/**
 * new_udp_client()
 * udp client (udp connection read listener) constructor.
 * this class is an afterthought so it is backwards, the connection hosting
 * the client class. maybe we'll change it later to move to client and server
 * connection class symmetry. however this works. when a client connection
 * needs a read listener it hosts and owns one of these.
 * @param udpx the udp connection which is the client's receive listener.
 */
etch_udp_client* new_udp_client (etch_udp_connection* udpx)
{
    etch_udp_client* newclient = NULL;

    newclient = (etch_udp_client*)new_object(sizeof(etch_udp_client), ETCHTYPEB_UDPCLIENT, CLASSID_UDP_CLIENT);
    
    ((etch_object*)newclient)->destroy  = destroy_etch_udp_client;
    newclient->cxlisten = udpx;  /* client's receive listener is udpx */

    newclient->thread = new_thread(etch_udp_client_receiver_proc, udpx);
    if(newclient->thread == NULL) {
        udpx->cx.on_event (udpx, ETCH_CONXEVT_STARTERR, 1, 0);
        etch_object_destroy(newclient);
        newclient = NULL;
    }
    newclient->thread->start(newclient->thread);


    //status = etch_thread_start(newclient->thread);
    // TODO: check error


    /* the threadpool acts as the server's thread manager. it creates threads
     * on request and destroys them at thread exit. */
    //newclient->threadpool = new_threadpool (ETCH_THREADPOOLTYPE_FREE, 1);

    /* data passed to threads will be either this object, or udp connection
     * objects. here we configure thread mgr to not free these at thread exit */
    //newclient->threadpool->is_free_data = FALSE;
    //newclient->threadpool->is_data_etchobject = TRUE;
    //newclient->is_started = TRUE;

    /* start the receive thread on the local thread manager */
    //if (NULL == newclient->threadpool->run(newclient->threadpool, etch_udp_client_receiver_proc, udpx)) {
    //    udpx->cx.on_event (udpx, ETCH_CONXEVT_STARTERR, 1, 0);
    //    newclient->destroy(newclient);
    //    newclient = NULL;
    //}

    return newclient;
}


/**
 * etch_udpclient_start_listener
 * start a receive listener thread on the client connection
 */
int etch_udpclient_start_listener (etch_udp_connection *udpx)
{
    etch_connection *cx = udpx? &udpx->cx:  NULL;
	//if (NULL == cx || NULL != udpx->rcvlxr) return -1;
    if (NULL == cx)
        return -1;
    else if (NULL != udpx->rcvlxr)
    {
        etch_object_destroy(udpx->rcvlxr);
        udpx->rcvlxr = NULL;
    }
    
    udpx->rcvlxr = new_udp_client (udpx);

    return NULL == udpx->rcvlxr? -1: 0;
}


/**
 * etch_udpclient_stop_listener
 * stop the receive listener thread on the client connection
 */
int etch_udpclient_stop_listener (etch_udp_connection *udpx)
{
    int result = 0;
    etch_udp_client* udpclient = NULL;
    etch_udp_connection* clientconx = NULL;
    etch_connection *cx = udpx? &udpx->cx:  NULL;
	if (NULL == cx || NULL == udpx->rcvlxr) return -1;

    udpclient = udpx->rcvlxr;
    clientconx = udpclient->cxlisten;

    udpclient->is_started = FALSE;

    result = etch_udpconx_close (clientconx);

    // aprrc = apr_socket_send (clientconx->cx.socket, ETCH_SHUTDOWNSIGNAL, &datalen);

    // result = etch_udpclient_send (udpx, ETCH_SHUTDOWNSIGNAL, ETCH_SHUTDOWNSIGNALSIZE, &aprrc);
    
    return result;
    //return aprrc == 0? 0: -1;
}
