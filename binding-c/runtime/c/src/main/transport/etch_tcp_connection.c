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
 * etch_tcpconxn.c
 * tcp connection class
 */

#include "etch_thread.h"
#include "etch_tcp_connection.h"
#include "etch_encoding.h"
#include "etch_flexbuffer.h"
#include "etch_log.h"
#include "etch_objecttypes.h"

static const char* LOG_CATEGORY = "etch_tcp_connection";

// extern types
extern apr_pool_t*         g_etch_main_pool;
extern apr_thread_mutex_t* g_etch_main_pool_mutex;

int   etch_tcpconx_closex(etch_tcp_connection*, const int, const int);
i_session* etch_tcpclient_get_session      (void*); 


//extern char* ETCH_CONNECTION_LOGID;
#define ETCH_SHUTDOWNSIGNALSIZE (sizeof(ETCH_SHUTDOWNSIGNAL)-1)

unsigned connection_id_farm;

#if(0)

 TCPCONNECTION
 |  Socket, hostIP, port, delay, isKeepalive, isNoDelay
 |  buffersize, isAutoflush, trafficclass
 |  InputStream, OutputStream
 |  stop0(); openSocket(); setupSocket(); readSocket();
 |  close(); send(); flush(); shutdownInput(); shutdownOutput();
 |  remoteAddress(); fireData(); transportData();
 |
  - CONNECTION<SESSIONDATA>
 |  |  Monitor status;
 |  |  Connection(); started(); stopped(); exception();
 |  |  run0(); localAddress(); translateHost();
 |  |  openSocket(); setupSocket(); readSocket(); close();
 |  |  transportQuery(); transportControl(); transportNotify();
 |  |  fireUp(); fireDown(); 
 |  |  void* getSession(); setSession(void*); waitUp(); waitDown();
 |  |  
 |   - SESSION
 |  |     sessionQuery(); sessionControl(); sessionNotify();
 |  |
 |   - RUNNER
 |  |  |  Thread thread;
 |  |  |  RunnerHandler handler;
 |  |  |  start0()
 |  |  |  stop0()
 |  |  |  run()
 |  |  |  run0()
 |  |  |  fireStarted()
 |  |  |  fireStopped()
 |  |  |  fireException()
 |  |   - ABSTRACTSTARTABLE
 |  |
 |   - TRANSPORT<SESSIONDATA>
 |  |     transportQuery(); transportControl(); transportNotify();
 |  |
 |   - RUNNERHANDLER interface
 |        started(); stopped(); exception();
 | 
  - TRANSPORTDATA 
    |  int transportData(to, buffer);
    |  int headerSize;
     - TRANSPORT
          transportQuery(); transportControl(); transportNotify();
#endif




/**
 * etch_tcpconx_set_socket_options()
 */
int etch_tcpconx_set_socket_options(void* data)
{
    etch_tcp_connection *c = (etch_tcp_connection*)data;
    int arc = 0, ecount = 0;
    etch_connection_event_handler eventx;
    etch_rawsocket* socket = c? c->cx.socket: NULL;
	if (!socket) return -1;
    eventx = c->cx.on_event;
      
    /*
     * APR_SO_DEBUG      -  turn on debugging information 
     * APR_SO_KEEPALIVE  -  keep connections active
     * APR_SO_LINGER     -  lingers on close if data is present
     * APR_SO_NONBLOCK   -  turns blocking on/off for socket
     *   when this option is enabled, use the APR_STATUS_IS_EAGAIN() macro  
     *   to determine if a send or receive function could not transfer data 
     *   without blocking.                                  
     * APR_SO_REUSEADDR  -  the rules used in validating addresses
     *   supplied to bind should allow reuse of local addresses.
     * APR_SO_SNDBUF     -  set the send buffer size
     * APR_SO_RCVBUF     -  set the receive buffer size
     */

    if (0 != (arc = apr_socket_opt_set(socket, APR_SO_KEEPALIVE, c->is_keepalive)))
        ecount += eventx(c, ETCH_CONXEVT_SOCKOPTERR, arc, "keepalive");   
 
    if (0 != (arc = apr_socket_opt_set(socket, APR_SO_LINGER, c->linger)))
        ecount += eventx(c, ETCH_CONXEVT_SOCKOPTERR, arc, "linger");   

    if (0 != (arc = apr_socket_opt_set(socket, APR_TCP_NODELAY, c->is_nodelay)))
        ecount += eventx(c, ETCH_CONXEVT_SOCKOPTERR, arc, "nodelay");   

    /* 
    if (0 != (arc = apr_socket_opt_set(socket, APR_SO_NONBLOCK, FALSE)))
        ecount += eventx(c, ETCH_CONXEVT_SOCKOPTERR, arc, "do not block");
     */

    /* timeout < 0 = block, 0 = never block, > 0 = block until timeout
    if (0 != (arc = apr_socket_timeout_set(socket, -1)))
        ecount +=  eventx(c, ETCH_CONXEVT_SOCKOPTERR, arc, "socket timeout");   
    */

    return ecount == 0? 0: -1;
}


/**
 * etch_tcpclient_on_data()
 * tcp socket received data handler.  
 * @param cx the connection object.
 * @param unused parameter not currently used.
 * @param length number of bytes in the supplied data buffer.
 * @param data the data as received via the socket wrapped in a flexbuffer.
 * caller retains this memory.
 * @remarks todo: if this remains the same as etch_tcpsvr_on_data, replace both 
 * methods with a etch_tcpconx_on_data() containing the same code.
 */
int etch_tcpclient_on_data (void* thisData, const int unused, int length, void* bufferData)
{
    etch_connection* cx = (etch_connection*)thisData;
    etch_flexbuffer* data = (etch_flexbuffer*)bufferData;
    int result = 0;
    i_sessiondata* session = NULL;
    etch_tcp_connection* tcpx = cx? (etch_tcp_connection*) cx->owner: NULL;
    ETCH_ASSERT(is_etch_tcpconnection(tcpx));
    ETCH_ASSERT(is_etch_flexbuffer(data));
    session = tcpx->session;
                                           
    /* send the data up the chain to be packetized. note that tcpx->session->thisx
     * is the owner of the i_sessiondata* session, which is the next higher layer 
     * of the transport stack, which is ordinarily the packetizer.  
     */
    if (-1 == (result = session->session_data (session->thisx, NULL, data)))
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "current %d bytes on connxn %d rejected\n", length, cx->conxid);
    return result;
}

/*
 * new_tcp_connection()
 * etch_tcp_connection tcp client constructor
 */
etch_tcp_connection* new_tcp_connection(etch_url* url, void* resources, etch_rawsocket* socket)
{
    int result = -1, item = 0;
    etch_tcp_connection* newcon = NULL;    
    if (!is_good_tcp_params (url, resources, socket)) return NULL;

    newcon = (etch_tcp_connection*)new_object (sizeof(etch_tcp_connection), ETCHTYPEB_CONNECTION, CLASSID_TCP_CONNECTION);
    ((etch_object*)newcon)->destroy = destroy_etch_tcp_connection;

    do  /* populate connection's transport and session interfaces */
    {   if (-1 == (result = init_etch_tcpconx_interfaces(newcon))) break;

        if (-1 == (result = etch_init_connection (&newcon->cx, socket, newcon))) break;

        newcon->cx.set_socket_options = etch_tcpconx_set_socket_options;
        newcon->cx.on_event = etch_tcpconx_on_event;  /* connection state handler */

        if (socket)
            newcon->cx.socket = socket;
        else
        {   
            // TODO: pool
            etch_encoding_transcode_wchar(&newcon->cx.hostname, ETCH_ENCODING_UTF8, url->host, NULL);
            newcon->cx.port = url->port;
            etchurl_get_integer_term (url, ETCH_CONNECTION_RECONDELAY, &newcon->cx.delay);
        }
        
        /* set term default values - values for any terms not set here are zero */
        newcon->is_nodelay = ETCH_CONNECTION_DEFNODELAY;
        newcon->linger     = ETCH_CONNECTION_DEFLINGERTIME;

        /* set any terms which may have been supplied with URL */
        etchurl_get_boolean_term(url, ETCH_CONNECTION_AUTOFLUSH,  &newcon->is_autoflush);
        etchurl_get_boolean_term(url, ETCH_CONNECTION_KEEPALIVE,  &newcon->is_keepalive);
        etchurl_get_boolean_term(url, ETCH_CONNECTION_NODELAY,    &newcon->is_nodelay);
        etchurl_get_integer_term(url, ETCH_CONNECTION_LINGERTIME, &newcon->linger);
        etchurl_get_integer_term(url, ETCH_CONNECTION_TRAFCLASS,  &newcon->traffic_class);
        etchurl_get_integer_term(url, ETCH_CONNECTION_BUFSIZE,    &item);
        if (item > 0) newcon->cx.bufsize = item;
        result = 0;

    } while(0);

    newcon->cx.on_data = etch_tcpclient_on_data;
    newcon->cx.on_event(newcon, result? ETCH_CONXEVT_CREATERR: ETCH_CONXEVT_CREATED, 0, 0);

    if (-1 == result)
    {   destroy_etch_tcp_connection(newcon);
        return NULL;
    }       
    else return newcon;
}

/**
 * etch_tcpclient_set_session()
 * i_transport::set_session() override
 * @param session an i_sessiondata*. caller retains this object.
 */
void etch_tcpclient_set_session (void* data, void* newsession)
{
    etch_tcp_connection* thisx = (etch_tcp_connection*)data;
    ETCH_ASSERT(is_etch_tcpconnection(thisx));
    ETCH_ASSERT(is_etch_sessiondata(newsession));
    if (thisx->is_session_owned){
        etch_object_destroy(thisx->session);
        thisx->session = NULL;
    }
    thisx->is_session_owned = FALSE;
    thisx->session = newsession;
}

/**
 * etch_tcpconx_transport_control()
 * connection::i_transport::transport_control override.
 * this is the base connection class' implementation of i_transport.
 * this is java binding's Connection.transportControl(), and serves as the
 * Transport part of the java TcpConnection TransportData. 
 * while tcp connection does implement i_transportdata, tcp connection's
 * implementation of i_transport comes from its inheritance of connection,
 * and its implementation of TransportData. since we do not separately implement
 * the connection class, the i_transport methods are implemented here.
 * @param control the event, sender relinquishes.
 * @param value control value, sender relinquishes.
 */
int etch_tcpconx_transport_control (void* data, etch_event* control, etch_object* value)
{
    etch_tcp_connection* thisx = (etch_tcp_connection*)data;
    etch_connection* cx = NULL;
    int result = 0, timeoutms = 0;
    const int objclass  = control? ((etch_object*)control)->class_id: 0;
    const int is_client = is_etch_int32(value)? ((etch_int32*)value)->value: NULL;
    ETCH_ASSERT(is_etch_tcpconnection(thisx));
    cx = &thisx->cx;
    

    switch(objclass)
    {  
       case CLASSID_CONTROL_START:

            result = etch_tcpconx_start (thisx);  

            if (is_client && 0 == result)
                result = etch_tcpclient_start_listener (thisx);  
            break; 
           
       case CLASSID_CONTROL_START_WAITUP: 
 
            /* open the connection, and wait for completion. caller blocks by virtue
             * of the fact that this is of course a function call, not a message handler.
             * timeout is communicated to caller via result code 1 = ETCH_TIMEOUT. 
             * it is not clear why wait up is implemented here. since a tcp server
             * implements transport interface itself, a server will never invoke this
             * implementation. on the other hand, the requester of a client connection  
             * and the socket itself are the same thread, wait up therefore being
             * meaningless since the socket open is known to be complete prior to
             * invoking wait up.
             */
            if (0 == (result = etch_tcpconx_open (thisx, ETCH_CONX_NOT_RECONNECTING)))
            {   timeoutms = value? ((etch_int32*) value)->value: 0;         
                result = etchconx_wait_up (cx, timeoutms);
            }
            break;  

       case CLASSID_CONTROL_STOP:  

            if (is_client)
                result = etch_tcpclient_stop_listener (thisx);  
            else
                result = etch_tcpconx_close (thisx, ETCH_CONX_NO_LINGER);
            break;

       case CLASSID_CONTROL_STOP_WAITDOWN: 
 
            /* see comments above at CLASSID_CONTROL_START_WAITUP */
            if (0 == (result = etch_tcpconx_close (thisx, ETCH_CONX_NO_LINGER)))
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
 * etch_tcpconx_transport_notify()
 * i_transport::transport_notify override.
 * @param evt, caller relinquishes.
 */
int etch_tcpconx_transport_notify (void* data, etch_event* evt)
{
    etch_tcp_connection* thisx = (etch_tcp_connection*)data;
    ETCH_ASSERT(is_etch_tcpconnection(thisx));
    etch_object_destroy(evt);
    return 0;  /* nothing to do */
}



/**
 * etch_tcpconx_transport_query()
 * i_transport::transport_query override.
 * @param query, caller relinquishes.
 */
etch_object* etch_tcpconx_transport_query (void* data, etch_query* query) 
{
    etch_tcp_connection* thisx = (etch_tcp_connection*)data;
    int result = 0;
    etch_object*  resultobj = NULL;
    etch_connection* cx = NULL;
    const int timeoutms = query? query->value: 0;
    const int objclass  = query? ((etch_object*)query)->class_id: 0;
    ETCH_ASSERT(is_etch_tcpconnection(thisx));
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
 * etch_tcpclient_sendex()
 * send data with specified timeout
 */
int etch_tcpclient_sendex (etch_tcp_connection *conx, unsigned char* buf, 
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
 * etch_tcpclient_send()
 */
int etch_tcpclient_send (etch_tcp_connection *conx, unsigned char* buf, const size_t totallen, int* rc)
{
    return etch_tcpclient_sendex(conx, buf, totallen, 0, rc);
}


/*
 * etch_tcpconx_transport_data()
 * etch_tcp_connection::i_transportdata::transport_data
 * @param whoto caller retains
 * @param fbuf caller retains 
 */
int etch_tcpconx_transport_data (void* data, etch_who* whoto, etch_flexbuffer* fbuf)
{
    etch_tcp_connection* thisx = (etch_tcp_connection*)data;
    int result = 0, apr_rc = 0;
    ETCH_ASSERT(is_etch_tcpconnection(thisx));

    result = etch_tcpclient_send (thisx, fbuf->buf, fbuf->datalen, &apr_rc);
    
    etch_flexbuf_reset(fbuf);
    return result;
} 

/*
 * init_etch_tcpcon_interfaces()
 * populate transport and placeholder session interfaces to tcp connection.
 */
int init_etch_tcpconx_interfaces (etch_tcp_connection* tcpx)
{
    i_transport* itransport = NULL;
    ETCH_ASSERT(is_etch_tcpconnection(tcpx));
    if (tcpx->itd) return 0;  /* already initialized */

    itransport = new_transport_interface_ex (tcpx, 
        etch_tcpconx_transport_control, 
        etch_tcpconx_transport_notify, 
        etch_tcpconx_transport_query,
        etch_tcpclient_get_session,
        etch_tcpclient_set_session);

    tcpx->itd = new_transportdata_interface (tcpx, 
					     (void*)etch_tcpconx_transport_data, itransport);  /* itd now owns itransport */

    /* establish placeholder session interface which is expected   
     * to be replaced by the connection host (e.g. packetizer) */
    tcpx->session = new_sessiondata_interface (tcpx, NULL, NULL);
    tcpx->is_session_owned = TRUE;

    return 0;
}


/**
 * etch_tcpconx_start()
 * start means open. generally we would come through here with an accepted socket,
 * in which case it is currently marked already open and we will return success. 
 * @return 0 success, -1 failure.
 */
int etch_tcpconx_start (etch_tcp_connection *conx)
{
    etch_connection* cx = conx? &conx->cx: NULL;
    ETCH_ASSERT(cx);
    cx->on_event (conx, ETCH_CONXEVT_STARTING, 0, 0);  
    if (cx->is_started) return 0;

    return etch_tcpconx_open (conx, ETCH_CONX_NOT_RECONNECTING);
}


/**
 * etch_tcpconx_open()
 * open connection to server based on host name/port set at construction.
 * @note we have omitted reconnect logic for now, pending logic to detect 
 * listen socket down and initiate reconnect.
 * @return 0 success, -1 failure (already open, hostname or socket error, etc)
 */
int etch_tcpconx_open(etch_tcp_connection *conx, const int is_reconnect)
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
        arc = apr_sockaddr_info_get(&cx->sockdata, cx->hostname, ETCH_DEFAULT_SOCKET_FAMILY, cx->port, 0, g_etch_main_pool);
        apr_thread_mutex_unlock(g_etch_main_pool_mutex);
        if (0 != arc) {
            eventx(conx, ETCH_CONXEVT_OPENERR, 4, (void*)(size_t)arc);  
            break;
        }

        if (!cx->socket)
        {
            if (0 != (arc = new_tcpsocket (&cx->socket, cx->aprpool)))
            {   eventx(conx, ETCH_CONXEVT_OPENERR, 3, (void*)(size_t)arc);  
                break;
            }

            /* set socket options here: NONBLOCK, TIMEOUT */ 
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
 * etch_tcpcconx_close()
 * close tcp connection
 */
int etch_tcpconx_close(etch_tcp_connection* conx, const int is_linger)
{
    return etch_tcpconx_closex(conx, is_linger, FALSE);
}


/*
 * etch_tcpcconx_closex()
 * close tcp connection
 * @param is_linger whether to set the socket to linger.
 * @param is_dtor true only if this call is from the tcp connection destructor.
 * @return 0 success, -1 failure.
 */
int etch_tcpconx_closex(etch_tcp_connection* conx, const int is_linger, const int is_dtor)
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
        
        if (NULL != cx->socket && is_linger)
            apr_socket_opt_set(cx->socket, APR_SO_LINGER, conx->linger);

        if (NULL != cx->socket && 0 != (arc = apr_socket_shutdown(cx->socket,APR_SHUTDOWN_READWRITE))) {
            cx->on_event(conx, ETCH_CONXEVT_CLOSERR, 3, (void*)(size_t)arc);
            result = 0;
        }

        if (NULL != cx->socket && 0 != (arc = apr_socket_close(cx->socket))) {
            cx->on_event(conx, ETCH_CONXEVT_CLOSERR, 3, (void*)(size_t)arc);
            result = -1;
        }

        cx->socket = NULL;
        cx->is_closing = FALSE;

        /* anything else we may need to do on close here */

    } while(0);

    if (is_locked) {
        etch_mutex_unlock(cx->mutex);
    }

    // join thread on client receiver thread
    if(conx->rcvlxr != NULL) {
        etch_join(conx->rcvlxr->thread);
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
 * destroy_etch_tcp_connection()
 * etch_tcp_connection destructor
 */
int destroy_etch_tcp_connection(void* thisx)
{
    etch_tcp_connection* tcpx = (etch_tcp_connection*)thisx;
    if (NULL == tcpx) return -1;
    tcpx->cx.on_event(tcpx, ETCH_CONXEVT_DESTROYING, 0, 0);
               
    etch_tcpconx_closex (tcpx, FALSE, TRUE);  /* close if open */

    if (!is_etchobj_static_content(tcpx)) {
        
        /* free listener if any */
        etch_object_destroy(tcpx->rcvlxr);
        tcpx->rcvlxr = NULL;

        /* free mem owned by tcpx */
        etch_destroy_connection (&tcpx->cx);

        /* free session interface */
        if (tcpx->is_session_owned) {
            etch_object_destroy(tcpx->session);
            tcpx->session = NULL;
        }

        /* free transport interface */
        etch_object_destroy(tcpx->itd);
        tcpx->itd = NULL;
    }

    tcpx->cx.on_event(tcpx, ETCH_CONXEVT_DESTROYED, 0, 0);
    return destroy_objectex((etch_object*)tcpx);
}


/*
 * new_tcpsocket()
 */
int new_tcpsocket (apr_socket_t** outsock, apr_pool_t* mempool)
{
    int rv = 0;
    apr_thread_mutex_lock(g_etch_main_pool_mutex);
    rv = apr_socket_create (outsock, APR_INET, SOCK_STREAM, APR_PROTO_TCP, g_etch_main_pool);
    apr_thread_mutex_unlock(g_etch_main_pool_mutex);
    
    return rv;
} 





/*
 * etch_tcpclient_receive()
 * receive data on socket
 * returns length received or -1
 */
int etch_tcpclient_receive (etch_tcp_connection *tcpx, unsigned char* buf, const size_t buflen, int* rc)
{
    return etch_tcpclient_receivex (tcpx, buf, buflen, 0, rc);
}


/*
 * etch_tcpclient_receivex()
 * receive data on socket with specified timeout, into specified character buffer.
 * @return number of bytes received on success; otherwise -2 (ETCH_OTHER_END_CLOSED)
 * if peer closed, or -1 if error. 
 */
int etch_tcpclient_receivex (etch_tcp_connection *tcpx, unsigned char* buf, const size_t buflen, const int timeout_ms, int* rc)
{
    int result = 0, arc = 0, is_eod = 0, eventid = 0;
    int64 existing_timeout_us = 0;
    apr_size_t datalen = 0; 
    etch_connection *cx = tcpx? &tcpx->cx: NULL;
    if (NULL == cx) return -1;
    cx->on_event(tcpx, ETCH_CONXEVT_RECEIVING, 0, 0);

    if (timeout_ms) {
        apr_socket_timeout_get(cx->socket, &existing_timeout_us);
        apr_socket_timeout_set(cx->socket, timeout_ms * 1000);
    }

    datalen = buflen; /* BLOCK on receive data here */
    
    arc = apr_socket_recv (cx->socket, (char*)buf, &datalen);
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
        cx->on_event (tcpx, eventid, arc, 0); 
        
    }
    else
    if (0 == datalen)
    {   cx->on_event(tcpx, ETCH_CONXEVT_PEERCLOSED, 0, 0);
        result = ETCH_OTHER_END_CLOSED;
    }
    else /* check for signal to shut down server */
    if (datalen == ETCH_SHUTDOWNSIGNALSIZE 
     && 0 == memcmp(buf, ETCH_SHUTDOWNSIGNAL, ETCH_SHUTDOWNSIGNALSIZE))
    {   cx->on_event(tcpx, ETCH_CONXEVT_SHUTDOWN, 0, 0);
        result = ETCH_SHUTDOWN_NOTIFIED;
    }
    else
    {   cx->on_event (tcpx, ETCH_CONXEVT_RECEIVED, is_eod, (char*)datalen);   
        if (-1 != result) result = (int) datalen;  /* return bytecount */
    }

    if (timeout_ms) /* restore socket timeout property */
        apr_socket_timeout_set(cx->socket, existing_timeout_us);
     
    if (rc) *rc = arc;
    return result;
}


static etch_status_t etch_tcp_client_cleanup(void* p)
{
    etch_status_t    rv     = ETCH_SUCCESS;
    etch_status_t    status = ETCH_SUCCESS;
    etch_tcp_client* client = p;

    status = etch_object_destroy(client->thread);
    // TODO: check status
    client->thread = NULL;

    status = destroy_objectex((etch_object*)client);
    // TODO: check status

    rv = status;

    return rv;
}


/**
 * destroy_etch_tcp_client()
 * tcp client (tcp connection read listener) destructor.
 */
int destroy_etch_tcp_client(void* data)
{
    etch_tcp_client* thisx = (etch_tcp_client*)data;
    etch_status_t rv     = ETCH_SUCCESS;
    rv = etch_tcp_client_cleanup(thisx);
    return rv;
}


/**
 * etch_tcpclient_listenerproc()
 * tcp socket receive thread procedure. 
 */
static void etch_tcp_client_receiver_proc(void* data)
{ 
    etch_thread_params* params = (etch_thread_params*)data;
    int result = 0, arc = 0;
    etch_tcp_connection* tcpx = (etch_tcp_connection*) params->data;
    etch_connection* cx = &tcpx->cx;
    const int thread_id = params->etch_thread_id;
    const int blen = cx->bufsize? cx->bufsize: ETCH_CONX_DEFAULT_BUFSIZE;
    //params->data->threas = params->threadob
    etch_flexbuffer* fbuf = new_flexbuffer(blen);  
    cx->on_event(tcpx, ETCH_CONXEVT_RCVPUMP_START, 0, 0); 

    while(cx->is_started)
    {
        etch_flexbuf_clear(fbuf);  /* for debugging otherwise unnecessary */
        cx->on_event(tcpx, ETCH_CONXEVT_RCVPUMP_RECEIVING, thread_id, 0);

        /* receive data from tcp socket into buffer owned by flexbuffer.
         * note that if this receive were to stop blocking, for example 
         * if the peer went down without it being detected here, we would
         * see unfettered looping of this listener procedure. BLOCK.
         */
        result = etch_tcpclient_receive (tcpx, fbuf->buf, blen, &arc);   

        switch(result)
        {
            case ETCH_THIS_END_CLOSED: case ETCH_OTHER_END_CLOSED:
               /* a socket is down so close connection and exit thread */ 
               cx->is_started = FALSE; /* this is new: exit thread now */
               result = 0;   /* was break here but next line catches it*/
        }

        if (!cx->is_started) break;  /* client shutdown */
        
        if (result < 0)
        {   cx->on_event(tcpx, ETCH_CONXEVT_RCVPUMP_ERR, arc, 0);
            break;
        }

        etch_flexbuffer_reset_to (fbuf, result); /* received (result) bytes */

        if (result > 0) /* forward (result) bytes to data handler for packetization */ 
            cx->on_data (cx, 0, result, fbuf);     
    }

    tcpx->session->session_notify (tcpx->session->thisx, new_etch_event(0, ETCHEVT_SESSION_DOWN));
    
    cx->on_event(tcpx, ETCH_CONXEVT_RCVPUMP_STOP, result, (void*) (size_t) thread_id);
    etch_object_destroy(fbuf);  
    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "leaving listener thread ...\n");   
}

/**
 * new_tcp_client()
 * tcp client (tcp connection read listener) constructor.
 * this class is an afterthought so it is backwards, the connection hosting
 * the client class. maybe we'll change it later to move to client and server
 * connection class symmetry. however this works. when a client connection
 * needs a read listener it hosts and owns one of these.
 * @param tcpx the tcp connection which is the client's receive listener.
 */
etch_tcp_client* new_tcp_client (etch_tcp_connection* tcpx)
{
    etch_tcp_client* newclient = NULL;

    newclient = (etch_tcp_client*)new_object(sizeof(etch_tcp_client), ETCHTYPEB_TCPCLIENT, CLASSID_TCP_CLIENT);
    
    ((etch_object*)newclient)->destroy  = destroy_etch_tcp_client;
    newclient->cxlisten = tcpx;  /* client's receive listener is tcpx */

    newclient->thread = new_thread(etch_tcp_client_receiver_proc, tcpx);
    if(newclient->thread == NULL) {
        tcpx->cx.on_event (tcpx, ETCH_CONXEVT_STARTERR, 1, 0);
        etch_object_destroy(newclient);
        newclient = NULL;
    }
    newclient->thread->start(newclient->thread);


    //status = etch_thread_start(newclient->thread);
    // TODO: check error


    /* the threadpool acts as the server's thread manager. it creates threads
     * on request and destroys them at thread exit. */
    //newclient->threadpool = new_threadpool (ETCH_THREADPOOLTYPE_FREE, 1);

    /* data passed to threads will be either this object, or tcp connection
     * objects. here we configure thread mgr to not free these at thread exit */
    //newclient->threadpool->is_free_data = FALSE;
    //newclient->threadpool->is_data_etchobject = TRUE;
    //newclient->is_started = TRUE;

    /* start the receive thread on the local thread manager */
    //if (NULL == newclient->threadpool->run(newclient->threadpool, etch_tcp_client_receiver_proc, tcpx)) {
    //    tcpx->cx.on_event (tcpx, ETCH_CONXEVT_STARTERR, 1, 0);
    //    newclient->destroy(newclient);
    //    newclient = NULL;
    //}

    return newclient;
}


/**
 * etch_tcpclient_start_listener
 * start a receive listener thread on the client connection
 */
int etch_tcpclient_start_listener (etch_tcp_connection *tcpx)
{
    etch_connection *cx = tcpx? &tcpx->cx:  NULL;
	//if (NULL == cx || NULL != tcpx->rcvlxr) return -1;
    if (NULL == cx)
        return -1;
    else if (NULL != tcpx->rcvlxr)
    {
        etch_object_destroy(tcpx->rcvlxr);
        tcpx->rcvlxr = NULL;
    }
    
    tcpx->rcvlxr = new_tcp_client (tcpx);

    return NULL == tcpx->rcvlxr? -1: 0;
}


/**
 * etch_tcpclient_stop_listener
 * stop the receive listener thread on the client connection
 */
int etch_tcpclient_stop_listener (etch_tcp_connection *tcpx)
{
    int result = 0;
    etch_tcp_client* tcpclient = NULL;
    etch_tcp_connection* clientconx = NULL;
    etch_connection *cx = tcpx? &tcpx->cx:  NULL;
	if (NULL == cx || NULL == tcpx->rcvlxr) return -1;

    tcpclient = tcpx->rcvlxr;
    clientconx = tcpclient->cxlisten;

    tcpclient->is_started = FALSE;

    result = etch_tcpconx_close (clientconx, FALSE);

    // aprrc = apr_socket_send (clientconx->cx.socket, ETCH_SHUTDOWNSIGNAL, &datalen);

    // result = etch_tcpclient_send (tcpx, ETCH_SHUTDOWNSIGNAL, ETCH_SHUTDOWNSIGNALSIZE, &aprrc);
    
    return result;
    //return aprrc == 0? 0: -1;
}






/*
 * etch_tcpclient_stop()
 */
int etch_tcpclient_stop (etch_tcp_connection *conx)
{
    return etch_tcpconx_close(conx, 0);
}


/* - - - - - - - - - - - - - - - - - 
 * tcp client :: i_transportdata
 * - - - - - - - - - - - - - - - - - 
 */



/* - - - - - - - - - - - - - - -
 * tcpclient :: i_transport 
 * - - - - - - - - - - - - - - -
 */

/**
 * etch_tcpclient_get_session
 * i_transport::get_session implementation
 */
i_session* etch_tcpclient_get_session (void* data) 
{
    etch_tcp_connection* thisx = (etch_tcp_connection*)data;
    ETCH_ASSERT(is_etch_tcpconnection(thisx));
    return (i_session*)thisx->session;
}








