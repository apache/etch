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

#include "etchobj.h"
#include "etchthread.h"
#include "etch_transport.h"
#include "etch_sessionmsg.h"
#include "etch_svcobj_masks.h"
#include "etch_message.h"
#include "etch_defvalufact.h"

#define ETCH_REMOTETYPE_NONE   0
#define ETCH_REMOTETYPE_CLIENT 1
#define ETCH_REMOTETYPE_SERVER 2

char* ETCHREMO;

xxxx_remote* new_etch_remote_base (void* thisx, const int objsize, 
    const unsigned short class_id, i_delivery_service*, 
    etch_value_factory*, objmask*);

etch_message* etchremote_new_message(xxxx_remote*, etch_type*);

int      etchremote_send(xxxx_remote*, etch_message*);
void*    etchremote_sendex(xxxx_remote*, etch_message*);
int      etchremote_begincall(xxxx_remote*, etch_message*, void**);
int      etchremote_endcall  (xxxx_remote*, i_mailbox*, etch_type*, void**);
int      etchremote_transport_control(xxxx_remote*, etch_event*, etch_int32*);
int      etchremote_transport_notify (xxxx_remote*, etch_event*);
void     etchremote_set_session (xxxx_remote*, void* session); 
void*    etchremote_get_session (xxxx_remote*);
objmask* etchremote_transport_query (xxxx_remote*, objmask*); 
int      etchremote_start_waitup  (xxxx_remote*, const int);
int      etchremote_stop_waitdown (xxxx_remote*, const int);

int is_etch_remote(void* x);


#endif /* #ifndef ETCH_REMOTE_H */