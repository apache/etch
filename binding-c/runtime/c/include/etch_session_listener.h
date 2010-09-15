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
 * etch_sessionmsg.h
 * i_sessionmessage interface
 * interface used to deliver messages to the session from the transport
 */
#ifndef ETCHISESSIONLISTEN_H
#define ETCHISESSIONLISTEN_H

#if(0)

 SESSIONLISTENER
 |  int sessionAccepted(socket)  
  - SESSION
       sessionQuery(); sessionControl(); sessionNotify();
 
#endif

#include "etch_sessionint.h"
#include "etch_transportint.h"
#include "etch_resources.h"

#ifdef __cplusplus
extern "C" {
#endif


/* session accepted interface */                          // 11/16
typedef int (*etch_session_accepted) (void* thisx, void* acceptedconx);

/* this is the signature that must be called from accept */ 
/* this function is defined in servermain */
typedef void* (*funcptr_new_impl_server) (void* thisx, void* remote_client);
struct i_sessionlistener;

typedef int (*etchlistener_waitexit) (struct i_sessionlistener*);

/**
 * i_server_factory
 * server factory interface
 */
typedef struct i_server_factory
{
    etch_object object;

    void* thisx;  /* object which is the interface implementor */
    funcptr_new_impl_server new_server;  /* server constructor */

    etch_object*   session;  /* not owned */  
    i_session* isession; /* not owned */
    etch_session_control session_control;
    etch_session_notify  session_notify;
    etch_session_query   session_query;

} i_server_factory;


/**
 * i_sessionlistener
 * sessionlistener interface
 */
typedef struct i_sessionlistener
{
    etch_object object;

    void* thisx; /* object which is the interface implementor */

    /* session interface */
    void* session;           /* not owned */
    i_session* isession;     /* may be owned or not */
    etch_session_control   session_control;
    etch_session_notify    session_notify;
    etch_session_query     session_query;

   /**
     * session_accepted()
     * delivers a socket to the session from the listener
     * @param socket caller retains
     * @return 0 success, -1 failure
     */
    etch_session_accepted session_accepted;

    /* transport interface */
    void* transport;             /* not owned */
    i_transport* itransport;     /* may be owned or not */
    /* these virtuals are proxied up to this level since [main] 
     * does not see the specific transport header e.g. tcpserver */
    etch_transport_control transport_control;
    etch_transport_notify  transport_notify;
    etch_transport_query   transport_query;
    etch_transport_set_session set_session; 
    etch_transport_get_session get_session; 

    void* url;                  /* owned */
    void* server_params;        /* owned */ 
    etch_resources* resources;  /* owned */  
    etchlistener_waitexit wait_exit;  /* listener thread exit wait */

    unsigned char is_session_owned;
    unsigned char is_transport_owned;
    unsigned char is_resources_owned;
    unsigned char unused;

} i_sessionlistener;


i_sessionlistener* new_sessionlistener_interface (void* thisx, etch_session_accepted, i_session*); 



#ifdef __cplusplus
}
#endif

#endif /* ETCHISESSIONLISTEN_H */
