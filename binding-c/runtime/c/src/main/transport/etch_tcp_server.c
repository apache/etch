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
 * etch_connection.c
 * connection client and server classes - tcp, udp
 */

#include "etch_thread.h"
#include "etch_tcp_server.h"
#include "etch_encoding.h"
#include "etch_flexbuffer.h"
#include "etch_session_message.h"
#include "etch_objecttypes.h"
#include "etch_log.h"
#include "etch_mem.h"
#include "etch_runtime.h"
#include "etch_config.h"

static const char* LOG_CATEGORY = "etch_tcpserver";

// extern types
extern apr_pool_t*         g_etch_main_pool;
extern apr_thread_mutex_t* g_etch_main_pool_mutex;

// TODO: module wide
// - etch_tcpserver_static_create
// - etch_tcpserver_static_destroy
// - synchronize tcp servicer id
// - synchronize tco connection id
// - separating extern and static functions inside module


char* ETCHTCPS = "TCPS";

#define ETCH_CONNECTION_DEFLINGERTIME 30
#define ETCH_CONNECTION_DEFNODELAY  TRUE
#define ETCH_CONNECTION_DEFRETRYDELAYMS  1000

unsigned next_etch_tcpserver_id();
unsigned next_etch_connection_id();

etch_tcp_connection* new_accepted_tcp_connection
    (char* host, const int port, etch_rawsocket*);

int  etch_tcpsvr_dummy_connection(etch_tcp_connection*);
int  etch_deftcplistener_on_event(etch_tcp_server*, etch_tcp_connection*, const int, int, void*);

/* transport interface */
int etch_tcpsvr_transport_control(void*, etch_event*, etch_object*);
int etch_tcpsvr_transport_notify (void*, etch_event*);
etch_object* etch_tcpsvr_transport_query (void*, etch_query*);
i_session* etch_tcpsvr_get_session(void*);

/* session listener interface stubs */
etch_object* etch_tcpsvr_stub_session_query (void*, etch_query*);


#if(0)

 TCPSERVER
 |  Socket, hostIP, port, delay, isKeepalive, isNoDelay
 |  buffersize, isAutoflush, trafficclass
 |  InputStream, OutputStream
 |  stop0(); openSocket(); setupSocket(); readSocket();
 |  close(); send(); flush(); shutdownInput(); shutdownOutput();
 |  remoteAddress(); fireData(); transportData();
 |
  - CONNECTION<SESSIONLISTENER>
 |  |  sessionListener session;
 |  |      sessionAccepted(socket);
 |  |      SESSION
 |  |         sessionQuery(); sessionControl(); sessionNotify(); 
 |  |       
 |  |  Monitor status;
 |  |  Connection(); started(); stopped(); exception();
 |  |  run0(); localAddress(); translateHost();
 |  |  openSocket(); setupSocket(); readSocket(); close();
 |  |  transportQuery(); transportControl(); transportNotify();
 |  |  fireUp(); fireDown(); 
 |  |  waitUp(); waitDown();
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
  - TRANSPORT<SESSIONLISTENER>
    |  SessionListener getSession(); setSession(SessionListener);
    |  transportQuery(); transportControl(); transportNotify();

#endif



/* - - - - - - - - - - - - - - - - - 
 * constructors / destructors
 * - - - - - - - - - - - - - - - - - 
 */


/**
 * etch_tcpsvr_on_data()
 * tcp socket received data handler.  
 * @param cx the connection object.
 * @param uu parameter currently unused.
 * @param length number of bytes in the supplied data buffer.
 * @param data the data as received via the socket wrapped in a flexbuffer.
 * caller retains this memory.
 */
int etch_tcpsvr_on_data (void* connectionData, const int uu, int length, void* bufferData)
{
    etch_connection* cx = (etch_connection*)connectionData;
    etch_flexbuffer* data = (etch_flexbuffer*)bufferData;
    int result = 0;
    i_sessiondata* session = NULL;
    etch_tcp_connection* tcpx = cx? (etch_tcp_connection*) cx->owner: NULL;
    ETCH_ASSERT(is_etch_tcpconnection(tcpx));
    ETCH_ASSERT(is_etch_flexbuffer(data));
    ETCH_ASSERT(is_etch_sessiondata(tcpx->session));
    session = tcpx->session;
                                           
    /* send the data up the chain to be packetized. note that tcpx->session->thisx
     * is the owner of the i_sessiondata* session, which is the next higher layer 
     * of the transport stack, which is ordinarily the packetizer. 
     */
    if (-1 == (result = session->session_data (session->thisx, NULL, data)))
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_WARN, "%d bytes via connxn %d discarded\n", 
                 length, cx->conxid);
    return result;
}


/*
 * new_accepted_tcp_connection()
 * etch_tcp_connection constructor for use with an accepted socket
 * receive thread exit must delete this object
 */
etch_tcp_connection* new_accepted_tcp_connection(char* host, const int port, etch_rawsocket* accepted_socket)
{
    int result = 0;
    etch_tcp_connection* newcon = NULL;
    if (!host || !accepted_socket) return NULL;

    newcon = (etch_tcp_connection*) new_object (sizeof(etch_tcp_connection), ETCHTYPEB_CONNECTION, CLASSID_TCP_CONNECTION);
    ((etch_object*)newcon)->destroy = destroy_etch_tcp_connection;  

    /* 1/1/09 need to verify this is correct - unit test failed since it expected 
     * tcpconx.session to be non-null - while new_tcp_connection() initialized 
     * these interfaces, the new_accepted_connection() did not - watch this spot 
     * in case it turns out that the transport and session interfaces now plugged  
     * in here are incorrect. fyi unit test now passes but must re-vet runtime.
     */
    if (0 != init_etch_tcpconx_interfaces (newcon)) return NULL; /* 1/1/09 */  

    etch_init_connection (&newcon->cx, accepted_socket, newcon);

    newcon->cx.socket = accepted_socket;
    newcon->cx.hostname = etch_malloc(strlen(host)+1, ETCHTYPEB_BYTES);
    strcpy(newcon->cx.hostname, host);
    newcon->cx.port = port;       
    newcon->is_nodelay = ETCH_CONNECTION_DEFNODELAY;
    newcon->linger = ETCH_CONNECTION_DEFLINGERTIME;
    newcon->cx.on_event = etch_tcpconx_on_event;  /* connection state handler */
    #ifndef IS_ETCH_NO_SESSIONDATA                /* dead-end data for testing */
    newcon->cx.on_data  = etch_tcpsvr_on_data;    /* received data handler */
    #endif

    if (0 == result)
        newcon->cx.on_event (newcon, ETCH_CONXEVT_CREATED, (int) (size_t) accepted_socket, 0);
    else 
    {   newcon->cx.on_event (newcon, ETCH_CONXEVT_CREATERR, 0, 0);
        destroy_etch_tcp_connection(newcon);
        newcon = NULL;
    } 
      
    return newcon;
}


/*
 * destroy_etch_tcp_server()
 * etch_tcp_server destructor
 */
int destroy_etch_tcp_server (void* data)
{
    etch_tcp_server* svr = (etch_tcp_server*)data;
    int result = 0, serverid = 0;
    if (NULL == svr) return -1;
    serverid = svr->listener_id; 
    svr->on_event(svr, 0, ETCH_CONXEVT_DESTROYING, 0, 0);               
    
    etch_tcpsvr_close(svr);  /* close if open */

    if (!is_etchobj_static_content(svr))
        destroy_etch_tcp_connection(svr->cxlisten);

    if (svr->session && svr->is_session_owned)
       ((etch_object*)svr->session)->destroy (svr->session);

    if (svr->itransport)
        etch_free(svr->itransport);

    if (svr->threadpool && svr->is_threadpool_owned)
        etch_object_destroy(svr->threadpool);

    etch_object_destroy(svr->thread);
    svr->thread = NULL;

    etch_mutex_destroy(svr->client_connections_mutex);
    svr->client_connections_mutex = NULL;

    etch_linked_list_destroy(svr->client_connections);
    svr->client_connections = NULL;

    svr->on_event(svr, 0, ETCH_CONXEVT_DESTROYED, 0, 0);

    result = destroy_objectex((etch_object*)svr);
    return result;
}


/**
 * etch_tcpsvr_set_session()
 * i_transport::set_session() override
 * @param session an i_sessionlistener*. caller owns this object.
 */
void etch_tcpsvr_set_session(void* data, void* sessionData)
{
    etch_tcp_server* thisx = (etch_tcp_server*)data;
    i_sessionlistener* session = (i_sessionlistener*)sessionData;
    ETCH_ASSERT(is_etch_tcpserver(thisx));
    ETCH_ASSERT(is_etch_sessionlxr(session));

    thisx->is_session_owned = FALSE;  /* internal caller will re-set */

    if (thisx->session)  /* replacing? */
    {   ETCH_ASSERT(is_etch_sessionlxr(thisx->session));
        etch_object_destroy(thisx->session);
    }

    thisx->session  = session;
    thisx->isession = session->isession;
    thisx->session_control = session->session_control;
    thisx->session_notify  = session->session_notify;
    thisx->session_query   = session->session_query;
    thisx->on_session_accepted = session->session_accepted;    
}

int etch_tcpsvr_stub_session_control (void* obj, etch_event* evt, etch_object* v)
{
    return -1;
}

int etch_tcpsvr_stub_session_notify  (void* obj, etch_event* event)
{
    return -1;
}

/*
 * etch_tcpsvr_stub_on_session_accepted()
 * i_sessionlistener::session_accepted default implementation
 * @param thisx
 * @param socket presumably an etch_socket wrapper. caller relinquishes.
 */
int etch_tcpsvr_stub_on_session_accepted(void* thisx, void* socket)
{

    return -1;
} 


/**
 * new_tcp_server()
 * etch_tcp_server constructor
 */
etch_tcp_server* new_tcp_server(etch_url* url, etch_threadpool* mp, etch_threadpool* sp, etch_resources* resxmap, i_sessionlistener* insession)
{
    etch_tcp_server* svr = NULL;
    
    // check parameters
    if (NULL == url)
    {
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "Invalid argument url is null\n");
        return NULL;
    }

    svr = (etch_tcp_server*) new_object(sizeof(etch_tcp_server), ETCHTYPEB_TCPSERVER, CLASSID_TCP_LISTENER);
    svr->listener_id = next_etch_tcpserver_id();

    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "creating tcp server %d ...\n", svr->listener_id);

    ((etch_object*)svr)->destroy     = destroy_etch_tcp_server;
    ((etch_object*)svr)->clone       = clone_null;
    svr->on_event    = etch_deftcplistener_on_event;
    svr->on_data     = etch_defconx_on_data;
    svr->resxmap     = resxmap; /* not owned, can be null */
    svr->threadpool  = mp;  /* currently always passed in */
    svr->subpool     = sp;  /* currently always passed in */
    svr->url         = url;
    svr->thread      = NULL;

    etch_linked_list_create(&svr->client_connections, 0);

    etch_mutex_create(&svr->client_connections_mutex , ETCH_MUTEX_NESTED, NULL);

    /* - - - - - - - - - - - - - - -
     * transport (i_transport) 
     * - - - - - - - - - - - - - - -
     */
    svr->itransport = new_transport_interface(svr,  
        (etch_transport_control) etch_tcpsvr_transport_control, 
        (etch_transport_notify)  etch_tcpsvr_transport_notify, 
        (etch_transport_query)   etch_tcpsvr_transport_query);

    svr->transport_control = etch_tcpsvr_transport_control;
    svr->transport_notify  = etch_tcpsvr_transport_notify;
    svr->transport_query   = etch_tcpsvr_transport_query;

    svr->get_session = etch_tcpsvr_get_session;
    svr->set_session = etch_tcpsvr_set_session;

          
    /* - - - - - - - - - - - - - - -
     * session (i_sessionlistener)       
     * - - - - - - - - - - - - - - -
     */

    if (insession)
    {
        svr->set_session(svr, insession);
        svr->is_session_owned = FALSE; 
    }
    else
    {   i_session* isession = new_session_interface(svr, 
            etch_tcpsvr_stub_session_control, 
            etch_tcpsvr_stub_session_notify, 
            etch_tcpsvr_stub_session_query);

        i_sessionlistener* newsession = new_sessionlistener_interface(svr, 
            etch_tcpsvr_stub_on_session_accepted,
            isession);

        svr->set_session(svr, newsession);
        svr->is_session_owned = TRUE; 
    }  


    /* - - - - - - - - - - - - - - -
     * etch_tcp_server continued 
     * - - - - - - - - - - - - - - -
     */

    /* create the listener's tcp connection */
    svr->cxlisten = new_tcp_connection (url, NULL, NULL);

    if (svr->cxlisten)
    {
        svr->cxlisten->cx.listener = (etch_object*) svr;
        svr->on_event(svr, 0, ETCH_CONXEVT_CREATED, 0, 0);
    }
    else
    {   svr->on_event(svr, 0, ETCH_CONXEVT_CREATERR, 0, 0);
        destroy_etch_tcp_server(svr);
        svr = NULL;
    }

    if (svr)
        ETCH_LOG(ETCHTCPS, ETCH_LOG_DEBUG, "tcp server %d created\n", svr->listener_id);
    else
        ETCH_LOG(ETCHTCPS, ETCH_LOG_ERROR, "could not create tcp server\n");

    return svr;
}

/* - - - - - - - - - - - - - - - - - 
 * tcp server methods
 * - - - - - - - - - - - - - - - - - 
 */

/**
 * etch_tcpsvr_open()
 * open the server accept listener socket.
 */
int etch_tcpsvr_open (etch_tcp_server *svr, const int is_reconnect)
{
	int result = -1, is_new_socket = 0, arc = 0, attempt = 0;  
    etch_tcp_connection* tcpx = NULL;
    etch_connection* cx = NULL;
    ETCH_ASSERT(is_etch_tcpserver(svr));
    tcpx = svr->cxlisten;
    cx   = &tcpx->cx;
    if (svr->state != ETCH_TCPSERVER_STATE_CLOSED) return 0;
    svr->on_event(svr, tcpx, ETCH_CONXEVT_OPENING, 0, 0); 
    is_new_socket = cx->socket == NULL;  

    if (is_reconnect)
        if (!cx->socket || !cx->hostname || !*cx->hostname || !cx->delay) 
            return svr->on_event(svr, tcpx, ETCH_CONXEVT_OPENERR, 0, 0);  
    do 
    {  if (0 != (arc = apr_sockaddr_info_get(&cx->sockdata, cx->hostname, 
            ETCH_DEFAULT_SOCKET_FAMILY, cx->port, 0, cx->aprpool)))
        {   svr->on_event(svr, tcpx, ETCH_CONXEVT_OPENERR, 4, (void*)(size_t)arc);  
            break;
        }

        if (is_new_socket) /* not reconnecting, create a socket */
        {
            if (0 != (arc = new_tcpsocket (&cx->socket,cx->aprpool)))
            {   svr->on_event(svr, tcpx, ETCH_CONXEVT_OPENERR, 3, (void*)(size_t)arc);  
                break;
            }

            /* set socket options here: NONBLOCK, TIMEOUT */ 
        }

        apr_socket_opt_set(cx->socket, APR_SO_REUSEADDR, 1);

        while(attempt++ < ETCH_CONNECTION_DEFRETRYATTEMPTS+1)   
        {
            if (is_reconnect || attempt > 0)
                etch_sleep(cx->delay);

            if (0 != (arc = apr_socket_bind(cx->socket, cx->sockdata)))
            {   svr->on_event(svr, tcpx, ETCH_CONXEVT_OPENERR, 5, (void*)(size_t)arc);   
                continue;
            }

            // get backlog item
            etchurl_get_integer_term(svr->url, ETCH_TCPLISTENER_BACKLOG, &svr->backlog);

            if (0 == (arc = apr_socket_listen(cx->socket, svr->backlog)))
            {   cx->is_started = TRUE;
                svr->on_event(svr, tcpx, ETCH_CONXEVT_LISTENED, cx->conxid, 0);
                break;
            } 
     
            svr->on_event(svr, tcpx, ETCH_CONXEVT_OPENERR, 6, (void*)(size_t)arc);  
        }         

    } while(0);
    
    if (cx->is_started) 
    {   result = 0;  /* stopped means no longer closed but not yet started */
        svr->state = ETCH_TCPSERVER_STATE_STOPPED;  
    }
    else 
    if (is_new_socket)
        cx->socket = NULL;

    svr->on_event(svr, tcpx, 
        result? ETCH_CONXEVT_OPENERR: ETCH_CONXEVT_OPENED, 0, 0);

	return result;  
}


/*
 * etch_tcpsvr_close()
 * close server socket
 */
int etch_tcpsvr_close (etch_tcp_server* lxr)
{
    int  result = 0;
    etch_connection* cx = NULL;
    ETCH_ASSERT(is_etch_tcpserver(lxr));

    if (lxr->state < ETCH_TCPSERVER_STATE_STOPPED)
        return -1;  

    if (lxr->state > ETCH_TCPSERVER_STATE_STOPPED) 
        result = etch_tcpsvr_stop(lxr);  /* SVR BREAK 007 */

    if (lxr->state != ETCH_TCPSERVER_STATE_STOPPED) 
        return -1;

    cx = &lxr->cxlisten->cx;
    lxr->state = ETCH_TCPSERVER_STATE_CLOSING;
    lxr->on_event(lxr, 0, ETCH_CONXEVT_CLOSING, 0, 0); 

    result = etch_tcpconx_close(lxr->cxlisten, 0); /* close listen socket */

    lxr->state = ETCH_TCPSERVER_STATE_CLOSED;
    lxr->on_event(lxr, 0, result? ETCH_CONXEVT_CLOSERR: ETCH_CONXEVT_CLOSED, 0, 0);

    if (cx->wait_down) 
    {
        /* if another thread is blocking on this condition variable, we set   
         * the condition to DOWN, which is presumably what the other thread  
         * will unblock on. 
         */
        etch_wait_set(cx->wait_down, ETCH_CONXEVT_DOWN);
    }

    return result; 
}


/**
 * etch_tcp_listener_client_connection_proc
 * tcp receive thread procedure for accepted session connections.
 * this is the client session lifetime pump. it runs in its own thread, which
 * exists until the peer socket is closed, the session requests closure of the
 * session or server, or a socket error is detected.
 */
static void etch_tcp_listener_client_connection_proc(void* data)
{ 
    etch_thread_params* params = (etch_thread_params*)data;
    unsigned char* buf = NULL;
    int result = 0, arc = 0;
    etch_flexbuffer* fbuf = NULL;   
    const int thread_id = params->etch_thread_id;
    int is_shutdown_request = FALSE;
    int is_other_end_closed = FALSE, is_this_end_closed = FALSE;
    etch_tcp_connection* accx = (etch_tcp_connection*) params->data;
    etch_connection* cx  = &accx->cx;
    etch_tcp_server* svr =(etch_tcp_server*) cx->listener;  
    const int session_id = cx->conxid;
    const int blen = cx->bufsize? cx->bufsize: ETCH_CONX_DEFAULT_BUFSIZE;
    
    ETCH_ASSERT(is_etch_tcpconnection(accx));
    cx->on_event = etch_tcpconx_on_event; 

 
    svr->on_event(svr, accx, ETCH_CONXEVT_RCVPUMP_START, 0, 0);
    fbuf = new_flexbuffer(blen);   
    buf  = etch_flexbuf_get_buffer(fbuf);   
  
    /* we are using blocking sockets. fyi if we switch to non-blocking mode
     * in the future, the APR_STATUS_IS_EAGAIN(rc) macro is the test we want
     * to make on the read or write result to determine if the socket was not
     * ready to read or write - that macro tests multiple apr result codes.
     */

    accx->session->session_notify (accx->session->thisx, new_etch_event(0, ETCHEVT_SESSION_UP));

    /* receive pump -- blocking read */
    while (svr->state == ETCH_TCPSERVER_STATE_STARTED && cx->is_started) {
        memset(buf, 0, blen); /* nice for debugging but otherwise unnecessary */
        svr->on_event(svr, accx, ETCH_CONXEVT_RCVPUMP_RECEIVING, thread_id, 0);

        /* receive data from tcp socket into buffer owned by flexbuffer.
         * note that if this receive were to stop blocking, for example if the  
         * peer went down without it being detected here, we would see serious 
         * slowdown begin here due to unfettered looping of the listener proc. 
         */
        result = etch_tcpclient_receive (accx, buf, blen, &arc);  /* block */

        /* if result is >= 0, it is the number of bytes received */

        if (svr->state != ETCH_TCPSERVER_STATE_STARTED || !cx->is_started){
            //in this case the svr stops the conx
            params->is_own_data = 0;
            break; /* if server stopped, exit */
        }

        if (APR_OTHER_END_CLOSED == arc || ETCH_OTHER_END_CLOSED == result)
        {   is_other_end_closed = TRUE;  /* if peer down, exit */
            break;
        }

        if (APR_THIS_END_CLOSED == arc)
        {   is_this_end_closed = TRUE;  /* if this end down, exit */
            break;
        }

        if (ETCH_SHUTDOWN_NOTIFIED == result)    
        {   is_shutdown_request = TRUE;  /* client sent server shutdown request */
            svr->is_started = FALSE;     /* (as opposed to client stop request) */
            break;
        }

        if (result < 0) /* if socket error, exit */
        {   svr->on_event(svr, svr->cxlisten, ETCH_CONXEVT_RCVPUMP_ERR, arc, 0);
            break;
        }  

        etch_flexbuf_set_length(fbuf, result);   
        etch_flexbuf_set_index (fbuf, 0);        

        if (result > 0) /* forward (result) bytes to data handler for packetization */ 
            result = cx->on_data (cx, 0, result, fbuf);  
    }

    /* at this point the client receive loop has exited, most likely due to 
     * socket closed on either end, but could also be due to a socket error.
     */

    etch_mutex_lock(svr->client_connections_mutex);
    svr->connections--; // TODO: connection count can also be accessed by client_connection list
    etch_linked_list_remove(svr->client_connections, accx);
    etch_mutex_unlock(svr->client_connections_mutex);

    if (is_this_end_closed || is_other_end_closed || is_shutdown_request) result = 0;
    svr->on_event(svr, accx, ETCH_CONXEVT_RCVPUMP_STOP, result, (void*)(size_t)thread_id);

    accx->session->session_notify (accx->session->thisx, new_etch_event(0, ETCHEVT_SESSION_DOWN));

    if (is_shutdown_request)
    {   /* if this receive caught a server shutdown request from a client, do not destroy
         * this session, but rather message back to the main listener to shut itself down.
         * the main thread is blocking on the main listener thread, and will iterate and 
         * destroy all the server's sessions once the listener thread unblocks.
         */
        i_sessionlistener* mainlistener = svr->session;

        const int resultx = mainlistener->transport_control (mainlistener->thisx, new_etch_event(CLASSID_CONTROL_STOP_WAITDOWN, ETCH_INFWAIT), NULL);

        /* we will not return here until after main listener has been destroyed */
        result = resultx;    /* this extra local is simply a breakpoint target */
    }
    else
    {   /* this code is executed when a session is terminates normally, either due to 
         * peer connection closing, or client stop request. this code is not executed
         * if the client is forced down from the main thread.
         */
        ETCH_LOG(ETCHTCPS, ETCH_LOG_DEBUG, "session %d begin teardown ...\n", session_id);

        /* tear down client session not including accx */
        etch_object_destroy(cx->session);
        cx->session = NULL;

        /* destroy client connection object */
        etch_object_destroy(accx);
        accx = NULL;

        ETCH_LOG(ETCHTCPS, ETCH_LOG_DEBUG, "session %d destroyed\n", session_id);
    }

    etch_object_destroy(fbuf);  /* we now exit the client session receive thread */
    ETCH_LOG(ETCHTCPS, ETCH_LOG_INFO, "client session %d ends\n", session_id);
    ETCH_LOG(ETCHTCPS, ETCH_LOG_DEBUG, "session %d thread exit ...\n", session_id);
}




/**
 * etch_tcpsvr_acceptproc()
 * accept pump
 */
static void etch_tcp_listener_accept_proc(void* data)
{
    etch_thread_params* params = (etch_thread_params*)data;
    int result = 0, arc = 0;
    etch_tcp_server* listener = (etch_tcp_server*) params->data; 
    const int thread_id  = params->etch_thread_id;
    etch_tcp_connection* newx = 0;  
    etch_tcp_connection* tcpx = listener->cxlisten;
    etch_connection* cx = &tcpx->cx;
    etch_rawsocket* listensock = cx->socket;
    etch_rawsocket* newsock = NULL;
    etch_thread* newthread  = NULL;
    apr_pool_t* temp_pool  = NULL;
    int max_number_of_connections = 0;
    etch_config_t* config;

    ETCH_ASSERT(is_etch_tcpserver(listener));

    while (listener->is_started) {
        listener->on_event(listener, tcpx, ETCH_CONXEVT_ACCEPTING, 0, 0);

        /* each accepted connection gets its own apr subpool, which is freed when 
         * the connection is destroyed. this accounts for apr memory specific to  
         * the connection which is not explicitly freed, such as apr_socket_t */
        ETCH_ASSERT(g_etch_main_pool != NULL);

        apr_pool_create(&temp_pool, NULL); 
        //printf("4 creating apr pool %p\n",temp_pool);

        /* socket block here for a client connection request */
        if (0 != (arc = apr_socket_accept (&newsock, listensock, temp_pool))) {
            listener->on_event(listener, tcpx, ETCH_CONXEVT_ACCEPTERR, 0,(void*)(size_t)arc); 
            /* if server shutdown, no error */
            if (listener->is_started) {
                result = -1; 
            }
            /* TODO: catch other conditions in which nonzero return not error */
            break;
        }
        
        if (!listener->is_started) {
            /* Even when we disconnect, close the newly created socket. So we don't have to wait
               for the timeout. */
            if (newsock != NULL)
                apr_socket_close(newsock);
            break; /* server shutdown */
        }
        ETCH_LOG(ETCHTCPS, ETCH_LOG_DEBUG, "connect request for socket %x\n", (size_t)newsock);
	
        etch_runtime_get_config(&config);
        ETCH_ASSERT(config);
        etch_config_get_property_int(config, "etch.maxconnections", &max_number_of_connections);
        if(max_number_of_connections == 0) {
            max_number_of_connections = 40;
        }

        if(listener->connections >= max_number_of_connections){
            ETCH_LOG(ETCHTCPS, ETCH_LOG_DEBUG, "max number of connections (%d) reached, not accepting more connection requests\n", listener->connections);
            apr_socket_close(newsock);
            continue;
        }

        /* create the new accepted connection object. note that this object 
         * will be freed when its listener thread exits. SVR BREAK 004 
         */
        newx = new_accepted_tcp_connection (cx->hostname, cx->port, newsock);

        newx->cx.listener   = (etch_object*)listener;
        newx->cx.is_started = TRUE;
        
        listener->on_event (listener, newx, ETCH_CONXEVT_ACCEPTED, 0, 0);

        /* 1/4/09 permit socket data handler to be overridden, e.g. by a unit test */
        if (listener->on_data && listener->on_data != etch_defconx_on_data)
            newx->cx.on_data = listener->on_data;

        /* TODO use the sessionlistener interface to call back to the accepted handler.
         * we temporarily plugged it in directly to the tcp server object in new_etch_listener().
         */
        /* e.g. transport.tcpxfact_session_accepted */
        if (listener->on_session_accepted) {
            listener->on_session_accepted (listener->session, newx);   /* SVR BREAK 005 */
        }

        /* on return from on_session_accepted, the connection cx.session is a reference
         * to the etch_session* client session data object. the thread we start below
         * will call the destructor on this object when it exits, in order to destroy  
         * the client session objects such as delivery service, stub, etc.
         */

        etch_mutex_lock(listener->client_connections_mutex);
        listener->connections++;
        etch_linked_list_add(listener->client_connections, newx);
        etch_mutex_unlock(listener->client_connections_mutex);

        /* run a read listener for the client connection, on a thread from the client pool,
         * via etch_threadpool_run_freethread, etch_thread_start, etch_tcpserver_listenerproc.
         * SVR BREAK 006  
         */
        if (NULL == ( newx->cx.thread = newthread = listener->threadpool->run (listener->subpool, etch_tcp_listener_client_connection_proc, newx))) 
        {
            listener->on_event (listener, newx, ETCH_CONXEVT_STARTERR, 1, 0);
            etch_object_destroy(newx); 
            newx = NULL;  /* todo newx should cx.session.destroy() */
            result = -1;
            break;
        }

        /* the new client session is now executing on its own thread. 
         * loop back to continue listening for client connection requests. 
         */

    }

    if(0 == result) {
         listener->on_event(listener, tcpx, ETCH_CONXEVT_ACCEPTPUMPEXIT, thread_id, 0);
    } else {
        listener->on_event(listener, tcpx, ETCH_CONXEVT_ACCEPTPUMPEXITERR, 0, 0);
    }
}



/**
 * etch_tcpsvr_start()
 * start accepting connections
 */
int etch_tcpsvr_start (etch_tcp_server* tcpsvr)
{
	int result = 0;
    etch_tcp_connection* tcpx = tcpsvr->cxlisten;   
    etch_connection* cx = &tcpx->cx;

    if (tcpsvr->state != ETCH_TCPSERVER_STATE_STOPPED) 
        return tcpsvr->on_event(tcpsvr, tcpx, ETCH_CONXEVT_STARTERR, 0, 0);  

    /* the threadpool acts as the server's thread manager. it creates threads
     * on request and destroys them at thread exit. the main pool is always
     * present here in the runtime, but could be null for unit tests. */
    if (tcpsvr->threadpool == NULL) /* currently only null for unit tests */  
    {   tcpsvr->threadpool = new_threadpool (ETCH_THREADPOOLTYPE_FREE, 0); 
        tcpsvr->is_threadpool_owned = TRUE;  
    }

    /* data passed to threads will be either this object, or accepted connection
     * objects. configure thread manager to not free this data at thread exit. */
    tcpsvr->threadpool->is_free_data = FALSE;
    tcpsvr->threadpool->is_data_etchobject = TRUE;

    tcpsvr->state = ETCH_TCPSERVER_STATE_STARTED;
    tcpsvr->is_started = TRUE;
    tcpsvr->on_event(tcpsvr, tcpx, ETCH_CONXEVT_STARTING, 0, 0);  

    tcpsvr->thread = new_thread(etch_tcp_listener_accept_proc, tcpsvr);
    if(tcpsvr->thread == NULL) {
        // TODO: error handling thread could not be started
        tcpsvr->on_event (tcpsvr, tcpx, ETCH_CONXEVT_STARTERR, 1, 0);
        result = -1;
    }
    tcpsvr->thread->start(tcpsvr->thread);
    // TODO: error logging

    /* start the accept thread on the main thread manager. SVR BREAK 003 */
    // TODO: run this as default thread
    //if (NULL == tcpsvr->threadpool->run (tcpsvr->threadpool, etch_tcp_listerner_proc, tcpsvr))
    //{
    //    tcpsvr->on_event (tcpsvr, tcpx, ETCH_CONXEVT_STARTERR, 1, 0);
    //    result = -1;
    //}
     
    tcpsvr->on_event(tcpsvr, tcpx, result? ETCH_CONXEVT_STARTERR: ETCH_CONXEVT_STARTED, 0, 0);

    if (cx->wait_up) 
    {
        /* if another thread is blocking on this condition variable, we set the wait 
         * variable to UP, which is presumably what the waiting thread will unblock on. */
        //ETCH_ASSERT(is_etch_wait(cx->waiter));
        etch_wait_set(cx->wait_up, ETCH_CONXEVT_UP);
    }

    return result;
}


/**
 * etch_tcpsvr_stop()
 * stop accepting connections and shut down the accept listener.
 */
int etch_tcpsvr_stop (etch_tcp_server* server)
{
    int result = 0;
    etch_tcp_connection* tcpx = NULL;
    etch_tcp_connection* client_con = NULL;
    int connection_count = 0;

    ETCH_ASSERT(is_etch_tcpserver(server));
    tcpx = server? server->cxlisten: NULL;
    if (!tcpx) return -1;

    if (server->state <  ETCH_TCPSERVER_STATE_STARTED)  
        return server->on_event(server, tcpx, ETCH_CONXEVT_STOPERR, 0, 0);  
 
    server->is_started = FALSE; /* pump threads conditional */
    server->state = ETCH_TCPSERVER_STATE_STOPPING;
    server->on_event(server, tcpx, ETCH_CONXEVT_STOPPING, 0, 0);
    
    /* unblock the accept thread so it will recognize that it should exit */
    result = etch_tcpsvr_dummy_connection (tcpx);

    // wait to server thread finished
    etch_join(server->thread);

    //etch_sleep(ACCEPTWAITMS); /* pause here avoids accept thread hang */
    
    etch_mutex_lock(server->client_connections_mutex);
    connection_count = server->connections;
    ETCH_ASSERT(connection_count == etch_linked_list_count(server->client_connections));
    for(;connection_count > 0; connection_count--){
        etch_linked_list_get(server->client_connections, 0, (void*)&client_con);
        ETCH_ASSERT(client_con);
        result = etch_tcpconx_close(client_con,0);
    }
    etch_mutex_unlock(server->client_connections_mutex);

    /* BLOCK here until all threads exited */
    result = threadpool_waitfor_all (server->threadpool, TRUE); 

    server->state = ETCH_TCPSERVER_STATE_STOPPED;
    server->on_event(server, tcpx, ETCH_CONXEVT_STOPPED, 0, 0);
    return result;
}


/**
 * next_etch_tcpserver_id()
 * return a unique ID used to identify a server instance
 */
unsigned next_etch_tcpserver_id()
{
    do { apr_atomic_inc32 ((volatile apr_uint32_t*) &tcpserver_id_farm);
       } while(tcpserver_id_farm == 0); 

    return tcpserver_id_farm;
}


/**
 * etch_tcpsvr_dummy_connection()
 * create a dummy client connection so as to unblock the accept thread, 
 * in order that it can then recognize that it should exit, and do so.
 * note that an invalid server address such as 0.0.0.0 will cause the connection
 * attempt to fail, whereas it will not have caused the accept to fail. in such
 * a case, the accept thread is left hanging, and a subsequent crash on service
 * exit is likely. TODO both ends should validate IP addresses. etch_url does not.
 */
int etch_tcpsvr_dummy_connection (etch_tcp_connection* tcpx)
{
    apr_status_t apr_status = APR_SUCCESS;
    apr_socket_t*   dummy = 0;
    apr_sockaddr_t* addr  = 0;
    int attempts = 0;
    etch_connection* cx = &tcpx->cx;
    const int MAXATTEMPTS = 8, DELAY_BETWEEN_ATTEMPTS_MS = 100;

    apr_status = new_tcpsocket(&dummy, cx->aprpool);
    if(apr_status != APR_SUCCESS) {
        cx->on_event(tcpx, ETCH_CONXEVT_OPENERR, 3, (void*)(size_t)apr_status); 
    }
    else {
        apr_status = apr_sockaddr_info_get(&addr, "127.0.0.1", APR_UNSPEC, cx->sockdata->port, 0, cx->aprpool);
        if(apr_status != APR_SUCCESS) {
            ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "could not get addr info.");
        }

        while (1)
        {
            apr_status = apr_socket_connect (dummy, addr);
            if(apr_status == APR_SUCCESS)
                break;
            
            if (++attempts > MAXATTEMPTS)
            {
                cx->on_event (tcpx, ETCH_CONXEVT_OPENERR, 2, (void*)(size_t)apr_status); 
                break;
            }
            else
                etch_sleep(DELAY_BETWEEN_ATTEMPTS_MS);
        }
    }
    return apr_status ? -1 : 0;
}


/* - - - - - - - - - - - - - - - - - 
 * tcp server :: i_sessionlistener
 * - - - - - - - - - - - - - - - - - 
 */

/*
 * pointers to these funtions are copied from the i_sessionlistener implementation
 * at set_session() time. these stub implementations are provided as placeholders.
 */







etch_object* etch_tcpsvr_stub_session_query (void* obj, etch_query* query) 
{
    return NULL;
}


/* - - - - - - - - - - - - - - -
 * tcpserver :: i_transport 
 * - - - - - - - - - - - - - - -
 */

/**
 * etch_tcpsvr_get_session
 * i_transport::get_session implementation.
 * returns the i_session, whose thisx is a etch_session_listener*
 */
i_session* etch_tcpsvr_get_session(void* data) 
{
    etch_tcp_server* thisx = (etch_tcp_server*)data;
    ETCH_ASSERT(is_etch_tcpserver(thisx));
    return thisx->isession;
}




/**
 * etch_tcpsvr_transport_control()
 * i_transport::transport_control override.
 * @param control event, sender relinquishes.
 * @param value control value, sender relinquishes.
 */
int etch_tcpsvr_transport_control (void* data, etch_event* control, etch_object* value)
{
  etch_tcp_server* thisx = (etch_tcp_server*)data;
    int  result = 0;
    etch_connection* cx = NULL;
    const int timeoutms = value? ((etch_int32*) value)->value: 0;
    const int objclass  = control? ((etch_object*)control)->class_id: 0;
    ETCH_ASSERT(is_etch_tcpserver(thisx));
    cx = &thisx->cxlisten->cx;

    switch(objclass)
    {  
       case CLASSID_CONTROL_START:

            if (0 == (result = etch_tcpsvr_open(thisx, ETCH_CONX_NOT_RECONNECTING)))
                result = etch_tcpsvr_start(thisx);

            break; 
           
       case CLASSID_CONTROL_START_WAITUP:

            /* point to the condition variable on the waiter. this is a semikludge;
             * however we need to have a target for the up state before we do the 
             * waitup, since the connect will complete before we get around to waitup,
             * and it needs to be able to mark state as up. previously the state 
             * variable cond_var was not set until the wait_up was invoked. in the 
             * current design the cond_var is nulled out after a wait, in order to 
             * reset wait state to not waiting, so we need to ensure it is populated 
             * in advance of any need to set a wait condition to some state prior to 
             * actually waiting.
             * TODO: etch_wait was changed remove this
             */
            //etchconx_init_waitstate(cx);

            /* open the connection, and wait for completion. caller blocks here.
             * timeout is indicated via result code 1 = ETCH_TIMEOUT. SVR BREAK 002
             */
            if (0 == (result = etch_tcpsvr_open(thisx, ETCH_CONX_NOT_RECONNECTING)))
                if (0 == (result = etch_tcpsvr_start(thisx)))
                    result = etchconx_wait_up(cx, timeoutms);
            break;  

       case CLASSID_CONTROL_STOP:

            result = etch_tcpsvr_close(thisx);
            break;

       case CLASSID_CONTROL_STOP_WAITDOWN:
            /* note all comments above at case CLASSID_CONTROL_START_WAITUP */
            //etchconx_init_waitstate(cx);

            if (0 == (result = etch_tcpsvr_close(thisx)))
                result = etchconx_wait_down(cx, timeoutms);

            break;
    }

    if (control) 
        etch_object_destroy(control);
    if (value)   
        etch_object_destroy(value);
    return result;
}


/**
 * etch_tcpsvr_transport_notify()
 * i_transport::transport_notify override.
 * @param evt, caller relinquishes.
 */
int etch_tcpsvr_transport_notify (void* data, etch_event* evt)
{        
    etch_tcp_server* thisx = (etch_tcp_server*)data;
    ETCH_ASSERT(is_etch_tcpserver(thisx)); 
    etch_object_destroy(evt);
    return 0;  /* nothing to do */
}


/**
 * etch_tcpsvr_transport_query()
 * i_transport::transport_query override.
 * @param query, caller relinquishes.
 */
etch_object* etch_tcpsvr_transport_query (void* data, etch_query* query) 
{          
    etch_tcp_server* thisx = (etch_tcp_server*)data;
    ETCH_ASSERT(is_etch_tcpserver(thisx));
    /* todo is this right? */
    return thisx->itransport->transport_query(thisx->itransport, query);
}


