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
 * etch_udp_server.c
 */

#include "etch_thread.h"
#include "etch_udp_server.h"
#include "etch_encoding.h"
#include "etch_flexbuffer.h"
#include "etch_session_message.h"
#include "etch_objecttypes.h"
#include "etch_log.h"
#include "etch_mem.h"
#include "etch_runtime.h"
#include "etch_config.h"
#include "etch_connection_event.h"

static const char* LOG_CATEGORY = "etch_udpserver";

// extern types
extern apr_pool_t*         g_etch_main_pool;
extern apr_thread_mutex_t* g_etch_main_pool_mutex;

char* ETCHUDPS = "UDPS";

unsigned next_etch_udpserver_id();

/* transport interface */
int etch_udpsvr_transport_control(void*, etch_event*, etch_object*);
int etch_udpsvr_transport_notify (void*, etch_event*);
etch_object* etch_udpsvr_transport_query (void*, etch_query*);
i_session* etch_udpsvr_get_session(void*);

/* - - - - - - - - - - - - - - - - - 
 * constructors / destructors
 * - - - - - - - - - - - - - - - - - 
 */


/*
 * destroy_etch_udp_server()
 * etch_udp_server destructor
 */
int destroy_etch_udp_server (void* data)
{
    etch_udp_server* svr = (etch_udp_server*)data;
    if (NULL == svr) return -1;
    svr->on_event((etch_server *)svr, 0, ETCH_CONXEVT_DESTROYING, 0, 0);               
    
    etch_udpsvr_close(svr);  /* close if open */

    if (!is_etchobj_static_content(svr))
        destroy_etch_udp_connection(svr->cxlisten);

    return destroy_etch_server((etch_server *)svr);
}

/**
 * new_udp_server()
 * etch_udp_server constructor
 */
etch_udp_server* new_udp_server(etch_url* url, etch_threadpool* mp, etch_threadpool* sp, etch_resources* resxmap, i_sessionlistener* insession)
{
    etch_udp_server* svr = NULL;
    
    // check parameters
    if (NULL == url)
    {
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "Invalid argument url is null\n");
        return NULL;
    }

    svr = (etch_udp_server*) new_object(sizeof(etch_udp_server), ETCHTYPEB_UDPSERVER, CLASSID_UDP_LISTENER);
    svr->listener_id = next_etch_udpserver_id();

    ETCH_LOG(LOG_CATEGORY, ETCH_LOG_DEBUG, "creating udp server %d ...\n", svr->listener_id);

    ((etch_object*)svr)->destroy     = destroy_etch_udp_server;
    ((etch_object*)svr)->clone       = clone_null;
    svr->on_event    = etch_def_listener_on_event;
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
        (etch_transport_control) etch_udpsvr_transport_control, 
        (etch_transport_notify)  etch_udpsvr_transport_notify, 
        (etch_transport_query)   etch_udpsvr_transport_query);

    svr->transport_control = etch_udpsvr_transport_control;
    svr->transport_notify  = etch_udpsvr_transport_notify;
    svr->transport_query   = etch_udpsvr_transport_query;

    svr->get_session = etch_udpsvr_get_session;
    svr->set_session = etch_server_set_session;

          
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
            etch_server_stub_session_control, 
            etch_server_stub_session_notify, 
            etch_server_stub_session_query);

        i_sessionlistener* newsession = new_sessionlistener_interface(svr, 
            etch_server_stub_on_session_accepted,
            isession);

        svr->set_session(svr, newsession);
        svr->is_session_owned = TRUE; 
    }  


    /* - - - - - - - - - - - - - - -
     * etch_udp_server continued 
     * - - - - - - - - - - - - - - -
     */

    /* create the listener's udp connection */
    svr->cxlisten = new_udp_connection (url, NULL, NULL);

    if (svr->cxlisten)
    {
        svr->cxlisten->cx.listener = (etch_object*) svr;
        svr->on_event((etch_server *)svr, 0, ETCH_CONXEVT_CREATED, 0, 0);
    }
    else
    {   svr->on_event((etch_server *)svr, 0, ETCH_CONXEVT_CREATERR, 0, 0);
        destroy_etch_udp_server(svr);
        svr = NULL;
    }

    if (svr)
        ETCH_LOG(ETCHUDPS, ETCH_LOG_DEBUG, "udp server %d created\n", svr->listener_id);
    else
        ETCH_LOG(ETCHUDPS, ETCH_LOG_ERROR, "could not create udp server\n");

    return svr;
}

/* - - - - - - - - - - - - - - - - - 
 * udp server methods
 * - - - - - - - - - - - - - - - - - 
 */

/**
 * etch_udpsvr_open()
 * open the server accept listener socket.
 */
int etch_udpsvr_open (etch_udp_server *svr, const int is_reconnect)
{
	int result = 0, is_new_socket = 0, arc = 0, attempt = 0;  

   svr->on_session_accepted (svr->session, svr->cxlisten);
   svr->state = ETCH_UDPSERVER_STATE_STARTED;  

	return result;  
}


/*
 * etch_udpsvr_close()
 * close server socket
 */
int etch_udpsvr_close (etch_udp_server* lxr)
{
    int  result = 0;
    etch_connection* cx = NULL;
    ETCH_ASSERT(is_etch_udpserver(lxr));

    if (lxr->state < ETCH_UDPSERVER_STATE_STOPPED)
        return -1;  

    if (lxr->state > ETCH_UDPSERVER_STATE_STOPPED) 
        result = etch_udpsvr_stop(lxr);  /* SVR BREAK 007 */

    if (lxr->state != ETCH_UDPSERVER_STATE_STOPPED) 
        return -1;

    cx = &lxr->cxlisten->cx;
    lxr->state = ETCH_UDPSERVER_STATE_CLOSING;
    lxr->on_event((etch_server *)lxr, 0, ETCH_CONXEVT_CLOSING, 0, 0); 

    result = etch_udpconx_close(lxr->cxlisten); /* close listen socket */

    lxr->state = ETCH_UDPSERVER_STATE_CLOSED;
    lxr->on_event((etch_server *)lxr, 0, result? ETCH_CONXEVT_CLOSERR: ETCH_CONXEVT_CLOSED, 0, 0);

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
 * etch_udpsvr_start()
 * start accepting connections
 */
int etch_udpsvr_start (etch_udp_server* udpsvr)
{
   return 0;
}


/**
 * etch_udpsvr_stop()
 * stop accepting connections and shut down the accept listener.
 */
int etch_udpsvr_stop (etch_udp_server* server)
{
    int result = 0;
    etch_udp_connection* udpx = NULL;
    etch_udp_connection* client_con = NULL;
    int connection_count = 0;

    ETCH_ASSERT(is_etch_udpserver(server));
    udpx = server? server->cxlisten: NULL;
    if (!udpx) return -1;

    if (server->state <  ETCH_UDPSERVER_STATE_STARTED)  
        return server->on_event((etch_server *)server, (etch_transport_connection *)udpx, ETCH_CONXEVT_STOPERR, 0, 0);  
 
    server->is_started = FALSE; /* pump threads conditional */
    server->state = ETCH_UDPSERVER_STATE_STOPPING;
    server->on_event((etch_server *)server, (etch_transport_connection *)udpx, ETCH_CONXEVT_STOPPING, 0, 0);

    etch_mutex_lock(server->client_connections_mutex);
    connection_count = server->connections;
    ETCH_ASSERT(connection_count == etch_linked_list_count(server->client_connections));
    for(;connection_count > 0; connection_count--){
        etch_linked_list_get(server->client_connections, 0, (void**)&client_con);
        ETCH_ASSERT(client_con);
        result = etch_udpconx_close(client_con);
    }
    etch_mutex_unlock(server->client_connections_mutex);

    /* BLOCK here until all threads exited */
    result = threadpool_waitfor_all (server->threadpool, TRUE); 

    server->state = ETCH_UDPSERVER_STATE_STOPPED;
    server->on_event((etch_server *)server, (etch_transport_connection *)udpx, ETCH_CONXEVT_STOPPED, 0, 0);
    return result;
}


/**
 * next_etch_udpserver_id()
 * return a unique ID used to identify a server instance
 */
unsigned next_etch_udpserver_id()
{
    do { apr_atomic_inc32 ((volatile apr_uint32_t*) &udpserver_id_farm);
       } while(udpserver_id_farm == 0); 

    return udpserver_id_farm;
}


/* - - - - - - - - - - - - - - -
 * udpserver :: i_transport 
 * - - - - - - - - - - - - - - -
 */

/**
 * etch_udpsvr_get_session
 * i_transport::get_session implementation.
 * returns the i_session, whose thisx is a etch_session_listener*
 */
i_session* etch_udpsvr_get_session(void* data) 
{
    etch_udp_server* thisx = (etch_udp_server*)data;
    ETCH_ASSERT(is_etch_udpserver(thisx));
    return thisx->isession;
}




/**
 * etch_udpsvr_transport_control()
 * i_transport::transport_control override.
 * @param control event, sender relinquishes.
 * @param value control value, sender relinquishes.
 */
int etch_udpsvr_transport_control (void* data, etch_event* control, etch_object* value)
{
  etch_udp_server* thisx = (etch_udp_server*)data;
    int  result = 0;
    etch_connection* cx = NULL;
    const int timeoutms = value? ((etch_int32*) value)->value: 0;
    const int objclass  = control? ((etch_object*)control)->class_id: 0;
    ETCH_ASSERT(is_etch_udpserver(thisx));
    cx = &thisx->cxlisten->cx;

    switch(objclass)
    {  
       case CLASSID_CONTROL_START:

            if (0 == (result = etch_udpsvr_open(thisx, ETCH_CONX_NOT_RECONNECTING)))
                result = etch_udpsvr_start(thisx);

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
            if (0 == (result = etch_udpsvr_open(thisx, ETCH_CONX_NOT_RECONNECTING)))
                if (0 == (result = etch_udpsvr_start(thisx)))
                    result = etchconx_wait_up(cx, timeoutms);
            break;  

       case CLASSID_CONTROL_STOP:

            result = etch_udpsvr_close(thisx);
            break;

       case CLASSID_CONTROL_STOP_WAITDOWN:
            /* note all comments above at case CLASSID_CONTROL_START_WAITUP */
            //etchconx_init_waitstate(cx);

            if (0 == (result = etch_udpsvr_close(thisx)))
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
 * etch_udpsvr_transport_notify()
 * i_transport::transport_notify override.
 * @param evt, caller relinquishes.
 */
int etch_udpsvr_transport_notify (void* data, etch_event* evt)
{        
    etch_udp_server* thisx = (etch_udp_server*)data;
    ETCH_ASSERT(is_etch_udpserver(thisx)); 
    etch_object_destroy(evt);
    return 0;  /* nothing to do */
}


/**
 * etch_udpsvr_transport_query()
 * i_transport::transport_query override.
 * @param query, caller relinquishes.
 */
etch_object* etch_udpsvr_transport_query (void* data, etch_query* query) 
{          
    etch_udp_server* thisx = (etch_udp_server*)data;
    ETCH_ASSERT(is_etch_udpserver(thisx));
    /* todo is this right? */
    return thisx->itransport->transport_query(thisx->itransport, query);
}


