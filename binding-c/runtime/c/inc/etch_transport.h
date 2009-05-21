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
#include "etch_tcpconxn.h"
#include "etch_resources.h"
#include "etch_arraylist.h"
#include "etch_mailboxmgr.h"
#include "etch_sessionlisten.h"
#include "etch_url.h"

typedef int (*etch_delivsvc_begincall)(void* thisx, 
    etch_message*, void** out);  /* i_mailbox** out */

typedef int (*etch_delvisvc_endcall)  (void* thisx, 
    i_mailbox*, etch_type* response_type, void** out); /* objmask** out */

typedef void* (*main_new_server_funcptr) (void* serverargs, void* sessionargs);
typedef void* (*new_client_impl_funcptr) (void* argstruct);  /* not used */

typedef void* (*helper_new_server_funcptr) (void* serverargs, void* sessionargs);
typedef void* (*new_client_funcptr) (void* argstruct);
typedef int   (*get_xxxx_resources_funcptr)(void* argstruct);


/*
 * i_delivery_service
 * the implementor of delivery service will implement i_sessionmessage and
 * i_session, and copy pointers to the implementations to this interface.
 */
typedef struct i_delivery_service
{
    unsigned int    hashkey;  
    unsigned short  obj_type; 
    unsigned short  class_id;
    struct objmask* vtab;  
    int  (*destroy)(void*);
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;
    struct objmask* parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;

    etch_delivsvc_begincall begin_call;
    etch_delvisvc_endcall   end_call;
    
    i_sessionmessage*   session;   /* not owned */
    i_transportmessage* transport; /* not owned */
    i_sessionmessage*   ism;       /* not owned */
    i_transportmessage* itm;       /* not owned */

    void* thisx;
    etchmutex* rwlock; /* not owned */
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
    unsigned int    hashkey;  
    unsigned short  obj_type; 
    unsigned short  class_id;
    struct objmask* vtab;  
    int  (*destroy)(void*);
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;
    struct objmask* parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;

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
    etchmutex* rwlock; /* not owned */
    etchwait* waiter;  /* owned by tcpconx */
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
    unsigned int    hashkey;  
    unsigned short  obj_type; 
    unsigned short  class_id;
    struct objmask* vtab;  
    int  (*destroy)(void*);
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;
    struct objmask* parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;

    void* thisx;             /* host etch_factory_params* */

    unsigned   session_id;   /* session key: same as cx.conxid */
    /* fyi a session's receive thread is available at cx.thread */
    etch_connection* cx;     /* session client accepted connection */ 
    i_delivery_service* ds;  /* session delivery service */
    void*      client;       /* xxxx_remote_client* */
    void*      server;       /* i_xxxx_server */
    void*      server_stub;  /* xxxx_server_stub* */
    objmask*   conximpl;     /* accepted conx impl obj e.g. tcp_connection* */

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
    unsigned int    hashkey;  
    unsigned short  obj_type; 
    unsigned short  class_id;
    struct objmask* vtab;  
    int  (*destroy)(void*);
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;
    struct objmask* parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;

    void* thisx;        /* params owner TODO populate this */

    wchar_t*            in_uri;
    etch_resources*     in_resx;
    etch_value_factory* in_valufact;
    etch_threadpool*    mainpool;
    etch_threadpool*    subpool;
    etch_threadpool*    qpool;
    etch_threadpool*    fpool;
    etchmutex*          mblock;
    objmask*            session;  
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
    unsigned int    hashkey;  
    unsigned short  obj_type; 
    unsigned short  class_id;
    struct objmask* vtab;  
    int  (*destroy)(void*);
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;
    struct objmask* parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;

    void* thisx;        /* params owner TODO populate this */

    wchar_t*            in_uri;
    etch_resources*     in_resx;
    etch_value_factory* in_valufact;
    etch_threadpool*    mainpool;   /* main listener thread manager*/
    etch_threadpool*    subpool;
    etch_threadpool*    qpool;      /* session thread manager */
    etch_threadpool*    fpool;      /* session thread manager */
    etchmutex*          mblock;     /* utility lock */
    objmask*            session;    /* i_session_listener* */ 
    i_session*          isession;   /* listener's i_session */  
    etch_arraylist*     clientlist; /* create session in new_helper_accepted_server() */  

    /* pointer to new_helper_accepted_server() in xxxx_helper */
    helper_new_server_funcptr helper_new_accepted;

    /* pointer to new_xxxx_server() in [main] */
    main_new_server_funcptr   main_new_server; 

} etch_server_factory;


/**
 * etch_client_factory
 * masked by etch_factory_params
 */
typedef struct etch_client_factory
{
    unsigned int    hashkey;  
    unsigned short  obj_type; 
    unsigned short  class_id;
    struct objmask* vtab;  
    int  (*destroy)(void*);
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;
    struct objmask* parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;

    void* thisx;        /* params owner todo populate this */

    wchar_t*            in_uri;
    etch_resources*     in_resx;
    etch_value_factory* in_valufact;
    etch_threadpool*    mainpool;
    etch_threadpool*    subpool;
    etch_threadpool*    qpool;
    etch_threadpool*    fpool;
    etchmutex*          rwlock;  /* mailbox read/write lock */
    objmask*   session; /* unused? */ 
    i_session* isession;/* unused? */  
    etch_arraylist*     per_server;  /* unused? */

    new_client_funcptr new_client;
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

etch_client_factory* new_client_factory (objmask* session, 
    i_session* isession, new_client_funcptr new_client_func); 

etch_server_factory* new_server_factory (objmask* session, i_session* isession,  
    helper_new_server_funcptr new_server_callback, main_new_server_funcptr new_server_impl_callback); 

i_delivery_service* new_delivery_service_interface(i_sessionmessage*, i_transportmessage*);  
i_delivery_service* new_etch_transport  (wchar_t*  uri, etch_factory_params*, void* conximpl);
i_delivery_service* new_etch_transport_a(etch_url* url, etch_factory_params*, void* conximpl);

etch_session* remove_etch_session (etch_server_factory*, const int session_id);
int  get_etch_session (etch_server_factory*, const int session_id, etch_session** out);
int  transport_teardown_client_sessions (i_sessionlistener*);
int  transport_session_count (i_sessionlistener*);
int  transport_thread_id (i_sessionlistener*);

i_sessionlistener*  new_etch_listener (wchar_t* uri, etch_resources*, 
    helper_new_server_funcptr, 
    main_new_server_funcptr,
    get_xxxx_resources_funcptr);  


#endif /* #ifndef ETCHTRANSPORT_H */