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

#include "apr_time.h"
#include "apr_network_io.h"
#include "etchthread.h"
#include "etch_tcpserver.h"
#include "etch_encoding.h"
#include "etchflexbuf.h"
#include "etch_global.h"
#include "etch_sessionmsg.h"
#include "etchlog.h"

char* ETCHTCPS = "TCPS";

#define ETCH_CONNECTION_DEFLINGERTIME 30
#define ETCH_CONNECTION_DEFNODELAY  TRUE
#define ETCH_CONNECTION_DEFRETRYDELAYMS  1000

unsigned next_etch_tcpserver_id();
unsigned next_etch_connection_id();

etch_tcp_connection* new_accepted_tcp_connection
    (char* host, const int port, etch_rawsocket*);

int  etch_tcpsvr_acceptproc(etch_threadparams*);
int  etch_tcpsvr_dummy_connection(etch_tcp_connection*);
int  etch_tcpsvr_on_data (etch_connection*, const int, int, etch_flexbuffer*);
int  etch_deftcplistener_on_event(etch_tcp_server*, etch_tcp_connection*, 
     const int, int, void*);

/* transport interface */
int etch_tcpsvr_transport_control(etch_tcp_server*, etch_event*, objmask*);
int etch_tcpsvr_transport_notify (etch_tcp_server*, etch_event*);
objmask* etch_tcpsvr_transport_query (etch_tcp_server*, objmask*);
i_session* etch_tcpsvr_get_session(etch_tcp_server*);
void etch_tcpsvr_set_session(etch_tcp_server*, i_sessionlistener*);

/* session listener interface stubs */
int etch_tcpsvr_stub_on_session_accepted(etch_tcp_server*, void*);
int etch_tcpsvr_stub_session_control (void*, void*, void*);
int etch_tcpsvr_stub_session_notify  (void*, void*);
void* etch_tcpsvr_stub_session_query (void*, void*);


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

/*
 * new_accepted_tcp_connection()
 * etch_tcp_connection constructor for use with an accepted socket
 * receive thread exit must delete this object
 */
etch_tcp_connection* new_accepted_tcp_connection
    (char* host, const int port, etch_rawsocket* accepted_socket)
{
    int result = 0;
    etch_tcp_connection* newcon = NULL;
    if (!host || !accepted_socket) return NULL;

    newcon = (etch_tcp_connection*) new_object (sizeof(etch_tcp_connection), 
        ETCHTYPEB_CONNECTION, CLASSID_TCP_CONNECTION);

    newcon->destroy = destroy_etch_tcp_connection;  

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


/**
 * new_tcp_server()
 * etch_tcp_server constructor
 */
etch_tcp_server* new_tcp_server (etch_url* url, etch_threadpool* mp, etch_threadpool* sp, 
    etch_resources* resxmap, i_sessionlistener* insession)
{
    etch_tcp_server* svr;    
    if (NULL == url) return NULL;

    svr = (etch_tcp_server*) new_object
       (sizeof(etch_tcp_server), ETCHTYPEB_TCPSERVER, CLASSID_TCP_LISTENER);

    svr->listener_id = next_etch_tcpserver_id();

    etchlog (ETCHTCPS, ETCHLOG_DEBUG, "creating tcp server %d ...\n", svr->listener_id);

    svr->destroy     = destroy_etch_tcp_server;  
    svr->clone       = clone_null;
    svr->on_event    = etch_deftcplistener_on_event;
    svr->on_data     = etch_defconx_on_data;
    svr->resxmap     = resxmap; /* not owned, can be null */
    svr->threadpool  = mp;  /* currently always passed in */
    svr->subpool     = sp;  /* currently always passed in */
    svr->url         = url;


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
            etch_tcpsvr_stub_on_session_accepted, isession);    

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
        etchlog (ETCHTCPS, ETCHLOG_DEBUG, "tcp server %d created\n", svr->listener_id);
    else
        etchlog (ETCHTCPS, ETCHLOG_ERROR, "could not create tcp server\n");

    return svr;
}


/**
 * etch_tcpserver_listenerproc
 * tcp receive thread procedure for accepted session connections.
 * this is the client session lifetime pump. it runs in its own thread, which
 * exists until the peer socket is closed, the session requests closure of the
 * session or server, or a socket error is detected.
 */
void etch_tcpserver_listenerproc (etch_threadparams* params)
{ 
    char* buf = NULL;
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

    /* receive pump -- blocking read */
    while (svr->state == ETCH_TCPSERVER_STATE_STARTED && cx->is_started)
    {
        memset(buf, 0, blen); /* nice for debugging but otherwise unnecessary */
        svr->on_event(svr, accx, ETCH_CONXEVT_RCVPUMP_RECEIVING, thread_id, 0);

        /* receive data from tcp socket into buffer owned by flexbuffer.
         * note that if this receive were to stop blocking, for example if the  
         * peer went down without it being detected here, we would see serious 
         * slowdown begin here due to unfettered looping of the listener proc. 
         */
        result = etch_tcpclient_receive (accx, buf, blen, &arc);  /* block */

        /* if result is >= 0, it is the number of bytes received */

        if (svr->state != ETCH_TCPSERVER_STATE_STARTED || !cx->is_started)
            break; /* if server stopped, exit */

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
    svr->connections--;  /* todo lose this count in favor of session arraylist */
    if (is_this_end_closed || is_other_end_closed || is_shutdown_request) result = 0;
    svr->on_event(svr, accx, ETCH_CONXEVT_RCVPUMP_STOP, result, (void*)(size_t)thread_id);

  
    if (is_shutdown_request)
    {   /* if this receive caught a server shutdown request from a client, do not destroy
         * this session, but rather message back to the main listener to shut itself down.
         * the main thread is blocking on the main listener thread, and will iterate and 
         * destroy all the server's sessions once the listener thread unblocks.
         */
        i_sessionlistener* mainlistener = svr->session;

        const int resultx = mainlistener->transport_control (mainlistener->thisx, 
                  new_etch_event(CLASSID_CONTROL_STOP_WAITDOWN, ETCH_INFWAIT), NULL);

        /* we will not return here until after main listener has been destroyed */
        result = resultx;    /* this extra local is simply a breakpoint target */
    }
    else
    {   /* this code is executed when a session is terminates normally, either due to 
         * peer connection closing, or client stop request. this code is not executed
         * if the client is forced down from the main thread.
         */
        etchlog(ETCHTCPS, ETCHLOG_DEBUG, "session %d begin teardown ...\n", session_id);
        ETCHOBJ_DESTROY(cx->session); /* tear down client session not including accx */
        ETCHOBJ_DESTROY(accx);        /* destroy client connection object */
        etchlog(ETCHTCPS, ETCHLOG_DEBUG, "session %d destroyed\n", session_id);
        // etchlog(ETCHTCPS, ETCHLOG_DEBUG, "server %d resumes listening ...\n", svr->listener_id); 
    }

    fbuf->destroy(fbuf);  /* we now exit the client session receive thread */
    etchlog(ETCHTCPS, ETCHLOG_INFO, "client session %d ends\n", session_id);
    etchlog(ETCHTCPS, ETCHLOG_DEBUG, "session %d thread exit ...\n", session_id); 
}


/*
 * destroy_etch_tcp_server()
 * etch_tcp_server destructor
 */
int destroy_etch_tcp_server (etch_tcp_server* svr)
{
    int result = 0, serverid = 0;
    if (NULL == svr) return -1;
    svr->on_event(svr, 0, ETCH_CONXEVT_DESTRUCTOR, svr->refcount, 0);
    if (svr->refcount > 0 && --svr->refcount > 0) return -1; 
    serverid = svr->listener_id; 
    svr->on_event(svr, 0, ETCH_CONXEVT_DESTROYING, 0, 0);               
    
    etch_tcpsvr_close(svr);  /* close if open */

    if (!is_etchobj_static_content(svr))
        destroy_etch_tcp_connection(svr->cxlisten);

    if (svr->session && svr->is_session_owned)
       ((objmask*)svr->session)->destroy (svr->session);

    if (svr->itransport)
        etch_free(svr->itransport);

    if (svr->threadpool && svr->is_threadpool_owned)
        svr->threadpool->destroy(svr->threadpool);

    svr->on_event(svr, 0, ETCH_CONXEVT_DESTROYED, 0, 0);

    result = destroy_objectex((objmask*)svr);
    return result;
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

        while(attempt++ < ETCH_CONNECTION_DEFRETRYATTEMPTS+1)   
        {
            if (is_reconnect || attempt > 0)
                etch_sleep(cx->delay);

            if (0 != (arc = apr_socket_bind(cx->socket, cx->sockdata)))
            {   svr->on_event(svr, tcpx, ETCH_CONXEVT_OPENERR, 5, (void*)(size_t)arc);   
                continue;
            }

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

    if (cx->waiter) 
    {   /* if another thread is blocking on this condition variable, we set   
         * the condition to DOWN, which is presumably what the other thread  
         * will unblock on. 
         */
        ETCH_ASSERT(is_etch_wait(cx->waiter));
        cx->waiter->set(cx->waiter, ETCH_CONXEVT_DOWN);
    }

    return result; 
}


/**
 * etch_tcpsvr_acceptproc()
 * accept pump
 */
int etch_tcpsvr_acceptproc (etch_threadparams* params)
{
	int result = 0, arc = 0;
    etch_tcp_server* svr = (etch_tcp_server*) params->data; 
    const int thread_id  = params->etch_thread_id;
    etch_tcp_connection* newx = 0;  
    etch_tcp_connection* tcpx = svr->cxlisten;  
    etch_connection* cx = &tcpx->cx;
    etch_rawsocket* listensock = cx->socket;
    etch_rawsocket* newsock = NULL;
    apr_pool_t* newsubpool  = NULL;
    etch_thread* newthread  = NULL;
    ETCH_ASSERT(is_etch_tcpserver(svr));   

    /* each accepted connection gets its own apr subpool, which is freed when 
     * the connection is destroyed. this accounts for apr memory specific to  
     * the connection which is not explicitly freed, such as apr_socket_t */
    apr_pool_create(&newsubpool, etch_apr_mempool); 

    while (svr->is_started)
    {   
        svr->on_event(svr, tcpx, ETCH_CONXEVT_ACCEPTING, 0, 0); 

        /* BLOCK here for a client connection request */
        if (0 != (arc = apr_socket_accept (&newsock, listensock, newsubpool)))
        {   
            svr->on_event(svr, tcpx, ETCH_CONXEVT_ACCEPTERR, 0,(void*)(size_t)arc); 
            if (svr->is_started) result = -1; /* if server shutdown, no error */ 
            /* todo catch other conditions in which nonzero return not error */
            break;
        }

        newsubpool = NULL;
        if (!svr->is_started) break; /* server shutdown */  
        etchlog (ETCHTCPS, ETCHLOG_DEBUG, "connect request for socket %x\n", 
            (size_t) newsock);
	
        /* create the new accepted connection object. note that this object 
         * will be freed when its listener thread exits. SVR BREAK 004 
         */
        newx = new_accepted_tcp_connection (cx->hostname, cx->port, newsock);

        newx->cx.listener = (etch_object*) svr;
        newx->cx.is_started = TRUE;
        svr->on_event (svr, newx, ETCH_CONXEVT_ACCEPTED, 0, 0);

        /* 1/4/09 permit socket data handler to be overridden, e.g. by a unit test */
        if (svr->on_data && svr->on_data != etch_defconx_on_data)
            newx->cx.on_data = svr->on_data;

        /* TODO use the sessionlistener interface to call back to the accepted handler.
         * we temporarily plugged it in directly to the tcp server object in new_etch_listener().
         */
        if (svr->on_session_accepted)  /* e.g. transport.tcpxfact_session_accepted */
            svr->on_session_accepted (svr->session, newx);   /* SVR BREAK 005 */

        /* on return from on_session_accepted, the connection cx.session is a reference
         * to the etch_session* client session data object. the thread we start below
         * will call the destructor on this object when it exits, in order to destroy  
         * the client session objects such as delivery service, stub, etc.
         */

        /* run a read listener for the client connection, on a thread from the client pool,
         * via etch_threadpool_run_freethread, etch_thread_start, etch_tcpserver_listenerproc.
         * SVR BREAK 006  
         */
        if (NULL == (newthread = svr->threadpool->run (svr->subpool,
            etch_tcpserver_listenerproc, newx))) 
        {
            svr->on_event (svr, newx, ETCH_CONXEVT_STARTERR, 1, 0);
            newx->destroy(newx); newx = NULL;  /* todo newx should cx.session.destroy() */
            result = -1;
            break;
        }

        /* the new client session is now executing on its own thread. 
         * loop back to continue listening for client connection requests. 
         */
        svr->connections++;   
        newx->cx.thread = newthread; 
        /* transport can now access thread via serverfactory.connection */
        apr_pool_create (&newsubpool, etch_apr_mempool);   
    } 

    if (newsubpool) 
        apr_pool_destroy(newsubpool);  

    if  (0 == result)
         svr->on_event(svr, tcpx, ETCH_CONXEVT_ACCEPTPUMPEXIT, thread_id, 0); 
    else svr->on_event(svr, tcpx, ETCH_CONXEVT_ACCEPTPUMPEXITERR, 0, 0); 

    return result; 
}


/**
 * etch_tcpsvr_on_data()
 * tcp socket received data handler.  
 * @param cx the connection object.
 * @param uu parameter currently unused.
 * @param length number of bytes in the supplied data buffer.
 * @param data the data as received via the socket wrapped in a flexbuffer.
 * caller retains this memory.
 */
int etch_tcpsvr_on_data (etch_connection* cx, const int uu, int length, etch_flexbuffer* data)
{
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
        etchlog (ETCHCONX, ETCHLOG_ERROR, "%d bytes via connxn %d discarded\n", 
                 length, cx->conxid);
    return result;
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

    /* start the accept thread on the main thread manager. SVR BREAK 003 */
    if (NULL == tcpsvr->threadpool->run (tcpsvr->threadpool, etch_tcpsvr_acceptproc, tcpsvr))
    {
        tcpsvr->on_event (tcpsvr, tcpx, ETCH_CONXEVT_STARTERR, 1, 0);
        result = -1;
    }
     
    tcpsvr->on_event(tcpsvr, tcpx, result? ETCH_CONXEVT_STARTERR: ETCH_CONXEVT_STARTED, 0, 0);

    if (cx->waiter) 
    {   /* if another thread is blocking on this condition variable, we set the wait 
         * variable to UP, which is presumably what the waiting thread will unblock on. */
        ETCH_ASSERT(is_etch_wait(cx->waiter));
        cx->waiter->set(cx->waiter, ETCH_CONXEVT_UP);
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
    apr_socket_t* bogus = 0;
    const int ACCEPTWAITMS = 100;
    etch_tcp_connection* tcpx = NULL;
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

    etch_sleep(ACCEPTWAITMS); /* pause here avoids accept thread hang */

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
    apr_socket_t* dummy = 0;
    int  arc = 0, attempts = 0;
    etch_connection* cx = &tcpx->cx;
    const int MAXATTEMPTS = 8, DELAY_BETWEEN_ATTEMPTS_MS = 100;

    if (0 != (arc = new_tcpsocket(&dummy, cx->aprpool)))
        cx->on_event (tcpx, ETCH_CONXEVT_OPENERR, 3, (void*)(size_t)arc); 

    else while (1)
    {   
        if (0 == (arc = apr_socket_connect (dummy, cx->sockdata))) 
            break; 
    
        if (++attempts > MAXATTEMPTS)
        {   cx->on_event (tcpx, ETCH_CONXEVT_OPENERR, 2, (void*)(size_t)arc); 
            break;
        }
        else etch_sleep(DELAY_BETWEEN_ATTEMPTS_MS);
    }

    return arc? -1: 0;
}


/* - - - - - - - - - - - - - - - - - 
 * tcp server :: i_sessionlistener
 * - - - - - - - - - - - - - - - - - 
 */

/*
 * pointers to these funtions are copied from the i_sessionlistener implementation
 * at set_session() time. these stub implementations are provided as placeholders.
 */

/*
 * etch_tcpsvr_stub_on_session_accepted()
 * i_sessionlistener::session_accepted default implementation
 * @param thisx
 * @param socket presumably an etch_socket wrapper. caller relinquishes.
 */
int etch_tcpsvr_stub_on_session_accepted(etch_tcp_server* thisx, void* socket)
{
    if (is_etch_socket(socket))
       ((objmask*)socket)->destroy(socket);
 
    return -1;
} 


int etch_tcpsvr_stub_session_control (void* obj, void* evt, void* v)
{
    return -1;
}


int etch_tcpsvr_stub_session_notify  (void* obj, void* evt)
{
    return -1;
}


void* etch_tcpsvr_stub_session_query (void* obj, void* query) 
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
i_session* etch_tcpsvr_get_session(etch_tcp_server* thisx) 
{
    ETCH_ASSERT(is_etch_tcpserver(thisx));
    return thisx->isession;
}


/**
 * etch_tcpsvr_set_session()
 * i_transport::set_session() override
 * @param session an i_sessionlistener*. caller owns this object.
 */
void etch_tcpsvr_set_session(etch_tcp_server* thisx, i_sessionlistener* session)
{
    ETCH_ASSERT(is_etch_tcpserver(thisx));
    ETCH_ASSERT(is_etch_sessionlxr(session));

    thisx->is_session_owned = FALSE;  /* internal caller will re-set */

    if (thisx->session)  /* replacing? */
    {   ETCH_ASSERT(is_etch_sessionlxr(thisx->session));
        thisx->session->destroy(thisx->session);
    }

    thisx->session  = session;
    thisx->isession = session->isession;
    thisx->session_control = session->session_control;
    thisx->session_notify  = session->session_notify;
    thisx->session_query   = session->session_query;
    thisx->on_session_accepted = session->session_accepted;    
}


/**
 * etch_tcpsvr_transport_control()
 * i_transport::transport_control override.
 * @param control event, sender relinquishes.
 * @param value control value, sender relinquishes.
 */
int etch_tcpsvr_transport_control (etch_tcp_server* thisx, etch_event* control, objmask* value)
{
    int  result = 0;
    etch_connection* cx = NULL;
    const int timeoutms = value? ((etch_int32*) value)->value: 0;
    const int objclass  = control? control->class_id: 0;
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
             */
            etchconx_init_waitstate(cx);

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
            etchconx_init_waitstate(cx);

            if (0 == (result = etch_tcpsvr_close(thisx)))
                result = etchconx_wait_down(cx, timeoutms);

            break;
    }

    if (control) control->destroy(control);
    if (value)   value->destroy(value);
    return result;
}


/**
 * etch_tcpsvr_transport_notify()
 * i_transport::transport_notify override.
 * @param evt, caller relinquishes.
 */
int etch_tcpsvr_transport_notify (etch_tcp_server* thisx, etch_event* evt)
{        
    ETCH_ASSERT(is_etch_tcpserver(thisx)); 
    if (evt) evt->destroy(evt);
    return 0;  /* nothing to do */
}


/**
 * etch_tcpsvr_transport_query()
 * i_transport::transport_query override.
 * @param query, caller relinquishes.
 */
objmask* etch_tcpsvr_transport_query (etch_tcp_server* thisx, objmask* query) 
{          
    ETCH_ASSERT(is_etch_tcpserver(thisx));
    /* todo is this right? */
    return thisx->itransport->transport_query(thisx->itransport, query);
}


