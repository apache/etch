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
 * etch_transportint.h
 * transport interface
 */
#ifndef ETCHITRANSPORT_H
#define ETCHITRANSPORT_H

#include "etch_object.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * the memory ownership contracts for control, notify, and query, are:
 * caller relinquishes all arguments except of course the this pointer,
 * regardless of outcome. it follows that only simple and temporal
 * objects are passed as parameters.
 */
typedef int   (*etch_transport_control) (void* thisx, etch_event* eventx, etch_object*);
typedef int   (*etch_transport_notify)  (void* thisx, etch_event* eventx);
typedef etch_object* (*etch_transport_query)   (void* thisx, etch_query*);

struct i_sessionmessage;
typedef struct i_session* (*etch_transport_get_session) (void* thisx);
typedef void  (*etch_transport_set_session) (void* thisx, void*);


/**
 * i_transport
 * transport interface
 * not an etch object, ergo no destructor
 */
typedef struct i_transport
{
    etch_transport_control transport_control;
    etch_transport_notify  transport_notify;
    etch_transport_query   transport_query;

    etch_transport_get_session  get_session;
    etch_transport_set_session  set_session;

    void* thisx;

} i_transport;



/**
 * i_transport_mask
 * mask over any implemented transport interface
 */
typedef struct i_transport_mask
{
    etch_object object;

    void* thisx; /* object which is the interface implementor */

    /* transport interface */
    i_transport* itransport;
    etch_transport_control transport_control;
    etch_transport_notify  transport_notify;
    etch_transport_query   transport_query;
    etch_transport_get_session  get_session;
    etch_transport_set_session  set_session;

    void* main_transport_method;

} i_transport_mask;


i_transport* new_default_transport_interface();

i_transport* new_transport_interface(void* thisx, etch_transport_control, etch_transport_notify, etch_transport_query);

i_transport* new_transport_interface_ex(void* thisx, etch_transport_control, etch_transport_notify, etch_transport_query, etch_transport_get_session, etch_transport_set_session);

i_transport* clone_transport(void* thisx, const i_transport*) ;

#ifdef __cplusplus
}
#endif

#endif /* ETCHITRANSPORT_H */
