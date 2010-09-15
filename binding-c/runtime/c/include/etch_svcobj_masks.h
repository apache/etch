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
 * etch_svcobj_masks.h
 * masks over generated objects
 */

#ifndef ETCHSVCetch_objectS_H
#define ETCHSVCetch_objectS_H

#include "etch_transport.h"
#include "etch_stub.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * i_xxx_server
 * server base interface
 * not instantiated, mask over a server base.
 */
typedef struct i_xxxx_server
{
    etch_object object;

    etch_object* thisx;  /* xxxx_server_impl* */
    etch_object* ixxxx;  /* service interface */
    int session_id;
    unsigned char is_service_interface_owned;
    unsigned char unused[3];

    /* - - - - - - - - - - -
     * objsession
     * - - - - - - - - - - -
     */
    i_objsession* iobjsession;
    etch_session_control _session_control;
    etch_session_notify  _session_notify;
    etch_session_query   _session_query;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * methods and data specific to the service follow in the implementation
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     */

} i_xxxx_server;



/**
 * i_xxxx_client
 * xxxx client base interface
 * this object is not instantiated, it is merely a mask over any
 * i_xxxx_client object, where xxxx is the etch service
 */
typedef struct i_xxxx_client
{
    etch_object object;

    etch_object* thisx;  /* xxxx_client_impl* */
    etch_object* ixxxx;  /* service interface */ 
    int server_id; 

    /* - - - - - - - - - - -
     * objsession
     * - - - - - - - - - - -
     */
    i_objsession* iobjsession;
    etch_session_control _session_control;
    etch_session_notify  _session_notify;
    etch_session_query   _session_query;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * methods and data specific to the service follow in the implementation
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     */

} i_xxxx_client;



/**
 * i_xxxx_either
 * mask over xxxx client base or xxxx_server_base
 */
typedef struct i_xxxx_either
{
    etch_object object;

    etch_object* thisx;  /* xxxx_yyyy_impl* */
    etch_object* ixxxx;  /* service interface */
    int owner_id;

    /* - - - - - - - - - - -
     * objsession
     * - - - - - - - - - - -
     */
    i_objsession* iobjsession;
    etch_session_control _session_control;
    etch_session_notify  _session_notify;
    etch_session_query   _session_query;

} i_xxxx_either;


/**
 * xxxx_remote
 * not instantiated, mask over any xxxx_remote object,
 * where xxxx is the service name
 */
typedef struct xxxx_remote
{
    etch_object object;

    etch_object*  ixxxx;           /* possibly owned */
    i_delivery_service*  dsvc; /* not owned */
    etch_value_factory*  vf;   /* not owned */
    unsigned char  remote_type; /* client or server */
    unsigned char  is_service_interface_owned;
    unsigned short unused;      /* alignment */

    etch_message* (*new_message) (void*, etch_type*);
    int   (*send)   (void*, etch_message*);
    void* (*sendex) (void*, etch_message*);
    etch_delivsvc_begincall begin_call;  /* i_mailbox** out */
    etch_delvisvc_endcall   end_call;    /* etch_object** out */

    int (*start_waitup)  (void*, const int waitms);
    int (*stop_waitdown) (void*, const int waitms);

    /* - - - - - - - - - - - - -
     * transport functionality
     * - - - - - - - - - - - - -
     */
    etch_transport_control transport_control;  
    etch_transport_notify  transport_notify;   
    etch_transport_query   transport_query;   
    etch_transport_get_session get_session;  
    etch_transport_set_session set_session;  

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * methods and data specific to the service follow in the implementation
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     */

} xxxx_remote;

typedef xxxx_remote etch_remote;



/**
 * xxxx_remote_client
 * this object is not instantiated, it is merely a mask over any
 * xxxx_remote_client object, where xxxx is the etch service
 */
typedef struct xxxx_remote_client
{
    etch_object object;

    i_xxxx_client* xxxx_client_base;  /* owned */
    xxxx_remote*   xxxx_remote_base;  /* owned */
    etch_server_factory* server_factory; /* owned */
    default_value_factory*  vf;       /* owned by base */
    int session_id;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * methods and data specific to the service follow in the implementation
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     */

} xxxx_remote_client;



/**
 * xxxx_remote_server
 */
typedef struct xxxx_remote_server
{
    etch_object object;

    i_xxxx_server* server_base;  /* owned */
    xxxx_remote*   remote_base;  /* owned */
    etch_client_factory* client_factory; /* owned */
    default_value_factory*  vf;  /* owned by base */
    int server_id;
 
    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * methods and data specific to the service follow in the implementation
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     */

} xxxx_remote_server;


/**
 * xxxx_remote_either
 */
typedef struct xxxx_remote_either
{
    etch_object object;

    i_xxxx_either* either_base;    /* owned */
    xxxx_remote*   remote_base;    /* owned */
    void*          either_factory; /* owned */
    default_value_factory*  vf;    /* owned by base */
    int owner_id;

} xxxx_remote_either;


/**
 * xxxx_either_stub
 * mask over xxxx_client_stub or xxxx_server_stub
 */
typedef struct xxxx_either_stub
{
    etch_object object;

    etch_stub*      stub_base;
    int             owner_id;  /* e.g. session_id */   

    int (*destroyex)(void*);   /* user memory destructor */

} xxxx_either_stub;

/**
 * xxxx_server_impl
 * this object is not instantiated, it is merely a mask over any
 * xxxx_server_impl object, where xxxx is the etch service
 */
typedef struct xxxx_server_impl
{
    etch_object object;
    i_xxxx_server*      server_base; /* owned */
    etch_object*            ixxxx;       /* not owned */
    xxxx_remote_client* client;      /* not owned */

    int (*destroyex) (void*);  /* user memory destructor */

    /* - - - - - - - - - - - -
     * objsession
     * - - - - - - - - - - - -
     */
    i_objsession* iobjsession;  /* owned by base */
    /* note that iobjsession->thisx is set to this xxxx_server_impl* */
    etch_session_control _session_control;
    etch_session_notify  _session_notify;
    etch_session_query   _session_query;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * methods and data specific to the service follow in the implementation
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     */

} xxxx_server_impl;

/**
 * xxxx_client_impl
 * this object is not instantiated, it is merely a mask over any
 * xxxx_client_impl object, where xxxx is the etch service
 */
typedef struct xxxx_client_impl
{
    etch_object object;

    i_xxxx_client*      client_base;  /* owned */
    etch_object*            ixxxx;        /* not owned */
    xxxx_remote_server* server;       /* not owned */

    int (*destroyex) (void*);  /* user memory destructor */

    /* - - - - - - - - - - - -
     * objsession
     * - - - - - - - - - - - -
     */
    i_objsession* iobjsession;  /* owned by base */
    /* note that iobjsession->thisx is set to this perf_client_impl* */
    etch_session_control _session_control;
    etch_session_notify  _session_notify;
    etch_session_query   _session_query;

    /* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     * methods and data specific to the service follow in the implementation
     * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - -
     */

} xxxx_client_impl;


/**
 * xxxx_either_impl
 * mask over any xxxx_client_impl or xxxx_server_impl
 */
typedef struct xxxx_either_impl
{
    etch_object object;

    i_xxxx_either*  either_base;  /* owned */
    etch_object*        ixxxx;        /* not owned */
    xxxx_remote_either* either;   /* not owned */

    int (*destroyex) (void*);  /* user memory destructor */

    /* - - - - - - - - - - - -
     * objsession
     * - - - - - - - - - - - -
     */
    i_objsession* iobjsession;  /* owned by base */
    /* note that iobjsession->thisx is set to this perf_client_impl* */
    etch_session_control _session_control;
    etch_session_notify  _session_notify;
    etch_session_query   _session_query;

} xxxx_either_impl;

#ifdef __cplusplus
}
#endif

#endif /* ETCHSVCetch_objectS_H */
