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
 * etch_transport.h -- common transport code
 */

#ifndef ETCHTRANSPORT_H
#define ETCHTRANSPORT_H

#include "etch_message.h"
#include "etch_tcp_connection.h"
#include "etch_resources.h"
#include "etch_arraylist.h"
#include "etch_mailbox_manager.h"
#include "etch_session_listener.h"
#include "etch_wait.h"
#include "etch_url.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*etch_delivsvc_begincall)(void* thisx, etch_message*, void** out);  /* i_mailbox** out */
typedef int (*etch_delvisvc_endcall)  (void* thisx, i_mailbox*, etch_type* response_type, void** out); /* etch_object** out */

typedef void* (*new_client_impl_funcptr)(void* argstruct);  /* not used */
typedef void* (*main_client_create_func)(void* factory_thisx, void* server);

typedef void* (*helper_listener_create_func)(void* serverargs, void* sessionargs);
typedef int   (*helper_resources_init_func)(void* argstruct);
typedef void* (*main_server_create_func)(void* factory_thisx, void* client);

/*
 * i_delivery_service
 * the implementor of delivery service will implement i_sessionmessage and
 * i_session, and copy pointers to the implementations to this interface.
 */
typedef struct i_delivery_service
{
    etch_object object;

    etch_delivsvc_begincall begin_call;
    etch_delvisvc_endcall   end_call;
    
    i_sessionmessage*   session;   /* not owned */
    i_transportmessage* transport; /* not owned */
    i_sessionmessage*   ism;       /* not owned */
    i_transportmessage* itm;       /* not owned */

    void* thisx;
    etch_mutex* rwlock; /* not owned */
    unsigned char is_session_owned;
    unsigned char is_transport_owned;

} i_delivery_service;


/*
 * etch_tcp_delivery_service
 * java binding does not have one of these, it constructs and returns a  
 * DeliveryService in TcpTransportFactory.newTransport(). we will instead 
 * invoke new_etch_transport(uri, resources), returning an i_delivery_service. 
 */
typedef struct etch_tcp_delivery_service  
{
    etch_object object;

    // ***** TODO lose i_deliveryservice instantiation and instead mask 
    // ***** all delivery service with i_deliveryservice. *************
    i_delivery_service*     ids;
    etch_delivsvc_begincall begin_call;
    etch_delvisvc_endcall   end_call;

    /* i_transportmessage - owns the tcp delivery service */
    i_transportmessage*     transport;   /* owned by transportx */ 
    i_mailbox_manager*      transportx;  /* owned */ 

    /* external implementation of i_session_message */
    i_sessionmessage*       session;     /* not owned */

    /* delivery service implementation of i_session_message */
    i_sessionmessage*       sessionmsg;  /* owned */
    etch_session_message    session_message;
    etch_session_control    session_control;
    etch_session_notify     session_notify;
    etch_session_query      session_query;

    /* delivery service implementation of i_transport_message */
    /* owns the tcp delivery service */
    i_transportmessage*     transportmsg;   /* owned */ 
    etch_transport_message  transport_message;
    etch_transport_control  transport_control;  
    etch_transport_notify   transport_notify;   
    etch_transport_query    transport_query;   
    etch_transport_get_session  get_session;   
    etch_transport_set_session  set_session;  

    /* private component implementations */
    etch_resources* resources;    /* not owned */
    etch_tcp_connection* tcpconx; /* not owned on server - destroyed on listener exit */
    etch_mutex* rwlock; /* not owned */
    etch_wait_t* wait_up;   /* owned by tcpconx */
    etch_wait_t* wait_down; /* owned by tcpconx */
    void* packetizer;  /* owned */
    void* messagizer;  /* owned */
    void* mailboxmgr;  /* owned */

    unsigned char is_tcpconx_owned;
    unsigned char unused[3];

} etch_tcp_delivery_service;

/* - - - - - - - - - - - - - - - - - - - -
 * factory parameter bundle
 * - - - - - - - - - - - - - - - - - - - -
 */

/**
 * etch_session
 * server's per-client parameters.
 * this object wraps all of a server's per-session instance data.
 */
typedef struct etch_session
{
    etch_object object;

    void* thisx;             /* host etch_factory_params* */

    unsigned   session_id;   /* session key: same as cx.conxid */
    /* fyi a session's receive thread is available at cx.thread */
    etch_connection* cx;     /* session client accepted connection */ 
    i_delivery_service* ds;  /* session delivery service */
    void*      client;       /* xxxx_remote_client* */
    void*      server;       /* i_xxxx_server */
    void*      server_stub;  /* xxxx_server_stub* */
    etch_object*   conximpl;     /* accepted conx impl obj e.g. tcp_connection* */

    /* note that mainlistener.thisx is the server object e.g. etch_tcp_server 
     * and that serverobject.session is this i_sessionlistener */
    i_sessionlistener* mainlistener; /* accept listener */ 

} etch_session;


/**
 * etch_factory_params
 * mask over server or client factory parameter bundle
 */
typedef struct etch_factory_params
{
    etch_object object;

    void* thisx;        /* params owner TODO populate this */

    wchar_t*            in_uri;
    etch_resources*     in_resx;
    etch_value_factory* in_valufact;
    etch_threadpool*    mainpool;
    etch_threadpool*    subpool;
    etch_threadpool*    qpool;
    etch_threadpool*    fpool;
    etch_mutex*          mblock;
    etch_object*            session;  
    i_session*          isession;  
    etch_arraylist*     per_connection_params;    

    /* factory constructors */
    void* helper_new_xxxx;
    void* main_new_xxxx;

} etch_factory_params;


/**
 * etch_server_factory
 * virtual server constructors and associated parameters.
 * masked by etch_factory_params
 */
typedef struct etch_server_factory
{
    etch_object object;

    void* thisx;        /* params owner TODO populate this */

    wchar_t*            in_uri;
    etch_resources*     in_resx;
    etch_value_factory* in_valufact;
    etch_threadpool*    mainpool;   /* main listener thread manager*/
    etch_threadpool*    subpool;
    etch_threadpool*    qpool;      /* session thread manager */
    etch_threadpool*    fpool;      /* session thread manager */
    etch_mutex*          mblock;     /* utility lock */
    etch_object*            session;    /* i_session_listener* */ 
    i_session*          isession;   /* listener's i_session */  
    etch_arraylist*     clientlist; /* create session in new_helper_accepted_server() */  

    /* pointer to xxxx_helper_listener_create() in xxxx_helper */
    helper_listener_create_func helper_new_listener;

    /* pointer to new_xxxx_server() in [main] */
    main_server_create_func main_new_server; 

} etch_server_factory;


/**
 * etch_client_factory
 * masked by etch_factory_params
 */
typedef struct etch_client_factory
{
    etch_object object;

    void* thisx;        /* params owner todo populate this */

    wchar_t*            in_uri;
    etch_resources*     in_resx;
    etch_value_factory* in_valufact;
    etch_threadpool*    mainpool;
    etch_threadpool*    subpool;
    etch_threadpool*    qpool;
    etch_threadpool*    fpool;
    etch_mutex*          rwlock;  /* mailbox read/write lock */
    etch_object*   session; /* unused? */ 
    i_session* isession;/* unused? */  
    etch_arraylist*     per_server;  /* unused? */

    main_client_create_func new_client;
    new_client_impl_funcptr new_client_impl;  /* unused */

    int   server_id;
    void* server;   /* xxxx_remote_server* use: is_etch_remote_server(x) */
    void* iclient;  /* i_xxxx_client use: is_etch_client_base(x) */
    void* stub;     /* xxxx_client_stub* */
    i_delivery_service* dsvc; /* use: is_etch_ideliverysvc(x) */

} etch_client_factory;


etch_tcp_delivery_service* new_tcp_delivery_service (etch_url*, etch_factory_params*, etch_tcp_connection*);
etch_tcp_delivery_service* get_etch_ds_impl (i_delivery_service*); 

etch_resources* etch_transport_resources_init(etch_resources*); 

etch_resources* get_etch_transport_resources(etch_resources*);

etch_client_factory* new_client_factory (etch_object* session, i_session* isession, main_client_create_func client_create_func);
etch_server_factory* new_server_factory (etch_object* session, i_session* isession, helper_listener_create_func helper_listener_create, main_server_create_func main_server_create);

i_delivery_service* new_delivery_service_interface(i_sessionmessage*, i_transportmessage*);  
i_delivery_service* new_etch_transport  (wchar_t*  uri, etch_factory_params*, void* conximpl);
i_delivery_service* new_etch_transport_a(etch_url* url, etch_factory_params*, void* conximpl);

etch_session* remove_etch_session (etch_server_factory*, const int session_id);
int  get_etch_session (etch_server_factory*, const int session_id, etch_session** out);
int  transport_teardown_client_sessions (i_sessionlistener*);
int  transport_session_count (i_sessionlistener*);
int  transport_thread_id (i_sessionlistener*);

i_sessionlistener* new_etch_listener (wchar_t* uri, etch_resources* resources, void* factory_thisx, helper_listener_create_func, main_server_create_func, helper_resources_init_func);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHTRANSPORT_H */
