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
 * etch_remote.h
 * remote base
 */

#ifndef ETCH_REMOTE_H
#define ETCH_REMOTE_H

#include "etch_object.h"
#include "etch_thread.h"
#include "etch_transport.h"
#include "etch_session_message.h"
#include "etch_svcobj_masks.h"
#include "etch_message.h"
#include "etch_default_value_factory.h"

#define ETCH_REMOTETYPE_NONE   0
#define ETCH_REMOTETYPE_CLIENT 1
#define ETCH_REMOTETYPE_SERVER 2

#ifdef __cplusplus
extern "C" {
#endif

xxxx_remote* new_etch_remote_base (void* thisx, const int objsize, 
    const unsigned short class_id, i_delivery_service*, 
    etch_value_factory*, etch_object*);

int etchremote_start_waitup (void* data, const int waitms);
int etchremote_stop_waitdown (void* data, const int waitms);
int etchremote_transport_control (void* data, etch_event* evt, etch_object* value); /* value could be an etch_int32 */
int etchremote_transport_notify  (void* data, etch_event* evt);
etch_object* etchremote_transport_query (void* data, etch_query* query);
etch_message* etchremote_new_message (void* data, etch_type* message_type);
int etchremote_send (void* data, etch_message* msg);
void* etchremote_sendex (void* data, etch_message* msg);
int etchremote_begincall (void* data, etch_message* msg, void** out);
int etchremote_endcall (void* data, i_mailbox* mbox, etch_type* response_type, void** out);

int is_etch_remote(void* x);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_REMOTE_H */
