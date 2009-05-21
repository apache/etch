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
 * xmpl_perf_remote_client.h 
 */

#ifndef PERF_REMOTE_CLIENT_H
#define PERF_REMOTE_CLIENT_H

#include "xmpl_perf_remote.h"
#include "xmpl_perf_client.h"
#include "etch_transport.h"

unsigned short CLASSID_PERFREMOTECLIENT;


/**
 * perf_remote_client
 */
typedef struct perf_remote_client
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

    i_perf_client* perf_client_base;     /* owned */
    perf_remote*   perf_remote_base;     /* owned */
    etch_server_factory* server_factory; /* owned */
    default_value_factory* vf;   /* owned by base */
    int session_id;

    /* toward-client virtuals go here */
    /* no toward-client items are declared by this service */

    /* asyncs go here if any toward-client messages defined */

} perf_remote_client;



/* constructor */
perf_remote_client* new_remote_client (void*, etch_session*, etch_value_factory*);


#endif /* PERF_REMOTE_CLIENT_H */

