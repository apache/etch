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
 * etch_stub.h
 * a translation of the java binding StubBase
 */

#ifndef ETCH_STUB_H
#define ETCH_STUB_H

#include "etch_object.h"
#include "etch_thread.h"
#include "etch_transport.h"
#include "etch_session_message.h"
#include "etch_message.h"
#include "etch_default_value_factory.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCH_STUBTYPE_NONE   0
#define ETCH_STUBTYPE_CLIENT 1
#define ETCH_STUBTYPE_SERVER 2

/*
 * etch_stub
 * stub base quasi interface
 */
typedef struct etch_stub
{
    etch_object object;
  
    etch_object* obj;         /* concrete client or server object, not owned */
    etch_object* stubobj;     /* stub xxxx_either_stub* container, not owned */
    i_objsession* impl_callbacks; /* impl's session callbacks, not owned */
    i_delivery_service* delivery_service;     /* not owned */

    /* - - - - - - - - - -
     * i_sessionmessage
     * - - - - - - - - - - 
     */
    i_session*           isession;         /* owned by isessionmsg */
    i_sessionmessage*    isessionmsg;      /* owned */
    etch_session_message session_message; 
    etch_session_control session_control;
    etch_session_notify  session_notify;
    etch_session_query   session_query;

    /* TODO lose threadpools and delivery params, they are here */
    etch_server_factory* params;           /* not owned */

    unsigned char stub_type;
    unsigned char is_implobj_owned;

    /* session interface notes:
     * the stub base itself implements i_sessionmessage and its associated
     * i_session. the client or server implementor implements, or not, the 
     * objession interface methods, which are the callbacks to which the 
     * i_session calls will be forwarded, if present, to the implementation. 
     * when we construct a stub, we copy the implementor's objsession* to 
     * the stub's impl_callbacks member for convenience. so, the presence 
     * of objession callbacks indicates the desire to have exceptions, etc.
     * forwarded to the implementation.
     */
} etch_stub;


/*
 * etchstub_runparams
 * parameters to a stub helper thread.
 */
typedef struct etchstub_runparams
{
    unsigned int        sig;    /* signature */
    etch_stub*          stub;   /* stub's this pointer */
    opaque_stubhelper   run;    /* stub helper function */
    void*               server; /* i_xxx_server impl mask */
    i_delivery_service* ds;
    etch_who*           who;
    etch_message*       msg;

} etchstub_runparams;

#define ETCH_STUBPARAMS_SIGNATURE 0xe1d2c3b4


etch_stub* new_stub (void* thisx, unsigned char stubtype, 
   i_delivery_service*, etch_threadpool* qp, etch_threadpool* fp);

void* new_clientstub_init (void* implobj, const int bytelen, etch_object_destructor userdtor, 
    i_delivery_service*, etch_threadpool* qp, etch_threadpool* fp, void*);  

void* new_serverstub_init (void* implobj, const int bytelen, etch_object_destructor userdtor, 
    i_delivery_service*, etch_threadpool* qp, etch_threadpool* fp, void*);  

void* new_stubimpl_init (void* implobj, const int objsize, 
      const unsigned char stubtype, etch_object_destructor userdtor,
      i_delivery_service* ids, etch_threadpool* qp, etch_threadpool* fp, 
      void* params);  

int etchstub_validate_args (etch_stub*, i_delivery_service*, etch_message*, 
    void* iclient, default_value_factory**, void** vfimpl, void** clientimpl);

int etchstub_send_reply (etch_stub*, i_delivery_service*, 
    etch_who*, etch_message*, etch_object*, boolean );

int is_etch_stub(void*);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_STUB_H */
