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
 * xmpl_perf_remote_server.h 
 * generated remote procedure calls.
 */

#ifndef PERF_REMOTE_SERVER_H
#define PERF_REMOTE_SERVER_H

#include "xmpl_perf_remote.h"
#include "xmpl_perf_server.h"
#include "xmpl_perf_valufact.h"
#include "etch_transport.h"      

unsigned short CLASSID_PERFREMOTESERVER;


/**
 * perf_remote_server
 */
typedef struct perf_remote_server
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

    i_perf_server* server_base;  /* owned */
    perf_remote*   remote_base;  /* owned */
    etch_client_factory* client_factory; /* owned */
    perf_value_factory*  vf;     /* owned by base */
    int server_id;

    perf_add       add;
    perf_sum       sum;
    perf_report    report;
    perf_dist      dist;
    perf_add2      add2;
    perf_report2   report2;

    perf_point*    point;
               
    /* private, generally. since unit tests invoke async begin and end,
     * we must expose them either as virtuals or as external references.
     */           
    perf_async_begin_add    async_begin_add; 
    perf_async_begin_sum    async_begin_sum;
    perf_async_begin_dist   async_begin_dist;
    perf_async_begin_add2   async_begin_add2;
     
    perf_async_end_add      async_end_add;
    perf_async_end_sum      async_end_sum;
    perf_async_end_dist     async_end_dist;
    perf_async_end_add2     async_end_add2;

} perf_remote_server;


/* constructor */
perf_remote_server* new_perf_remote_server (void*, i_delivery_service*, etch_value_factory*);

int perf_remote_dispose_mailbox(perf_remote_server*, i_mailbox**);

/* convenience methods to override client's session interface methods */
etch_stub* perf_remote_get_stubbase (perf_remote_server*);
etch_session_notify  perf_remote_set_session_notify( perf_remote_server*, etch_session_notify);
etch_session_control perf_remote_set_session_control(perf_remote_server*, etch_session_control);
etch_session_query   perf_remote_set_session_query  (perf_remote_server*, etch_session_query);


#endif /* PERF_REMOTE_SERVER_H */

