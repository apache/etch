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
 * xmpl_perf_client.h
 * perf client interface.
 * combines java bindings's PerfServer and BasePerfServer
 */

#ifndef PERF_CLIENT_H
#define PERF_CLIENT_H

#include "xmpl_perf.h"
#include "etch_sessionint.h"

unsigned short CLASSID_PERFCLIENTBASE;
typedef struct perf_client_impl perf_client_impl;


/**
 * i_perf_client
 * perf client base interface
 */
typedef struct i_perf_client
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

    perf_client_impl* thisx;
    i_perf*  iperf;   /* owned */
    int server_id;

    /* - - - - - - - - - - -
     * objsession
     * - - - - - - - - - - -
     */
    i_objsession* iobjsession;
    etch_session_control _session_control;
    etch_session_notify  _session_notify;
    etch_session_query   _session_query;

    /* no client-directed items defined */

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

    /* - - - - - - - - - - -
     * service data
     * - - - - - - - - - - -
     */
    perf_point*   point;

} i_perf_client;


i_perf_client* new_perf_client_base ();


#endif /* PERF_CLIENT_H */

