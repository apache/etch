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
 * xmpl_perf_remote.h
 * perf remote.
 * combines java bindings's RemotePerf, Perf, and RemoteBase.
 */

#ifndef PERF_REMOTE_H
#define PERF_REMOTE_H

#include "xmpl_perf.h"
#include "etch_remote.h"
#include "etch_transport.h"
unsigned short CLASSID_REMOTE_PERF;


/**
 * perf_remote
 */
typedef struct perf_remote
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

    i_perf*  iperf;             /* possibly owned */
    i_delivery_service*  dsvc;  /* not owned */
    etch_value_factory*  vf;    /* not owned */
    unsigned char  remote_type; /* client or server */
    unsigned char  is_service_interface_owned;
    unsigned short unused;      /* alignment */

    etch_message* (*new_message) (void*, etch_type*);
    int   (*send)   (void*, etch_message*);
    void* (*sendex) (void*, etch_message*);
    etch_delivsvc_begincall begin_call;  /* i_mailbox** out */
    etch_delvisvc_endcall   end_call;    /* objmask** out */

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

    /* - - - - - - - - - - -
     * service virtuals
     * - - - - - - - - - - -
     */
    perf_add      add;
    perf_sum      sum;
    perf_report   report;
    perf_dist     dist;
    perf_add2     add2;
    perf_report2  report2;
    perf_point*   point;

    /* - - - - - - - - - - -
     * private instance data
     * - - - - - - - - - - -
     */    

} perf_remote;


perf_remote* new_perf_remote (void*, i_delivery_service*, etch_value_factory*, i_perf*);


#endif /* PERF_REMOTE_H */

