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
 * xmpl_perf_server.h
 * perf service interface.
 * combines java bindings's PerfServer and BasePerfServer
 */

#ifndef PERF_SERVER_H
#define PERF_SERVER_H

#include "xmpl_perf.h"
#include "etch_sessionint.h"
static int perf_server_id_farm;

unsigned short CLASSID_EXESERVERBASE_PERF;

typedef struct perf_server_impl perf_server_impl;

/* these constants are here only for purposes of testing the throwing
 * of remote etch exceptions by the perf  server implementation. they
 * will not be part of generated code.
 */
#define FAKEBADDATA_INT32 ETCHTYPE_MIN_INT32
#define FAKEBADDATA_INT64 ETCHTYPE_MIN_INT64
#define FAKEBADDATA_STRING L"baddata"
#define FAKEBADDATA_DATE  ((time_t) 0x1000)


/**
 * i_perf_server
 * perf server base interface
 */
typedef struct i_perf_server
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

    perf_server_impl* thisx;
    i_perf* iperf;
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

    perf_add      async_add;
    perf_sum      async_sum;
    perf_report   async_report;
    perf_dist     async_dist;
    perf_add2     async_add2;
    perf_report2  async_report2;

    /* - - - - - - - - - - -
     * service data
     * - - - - - - - - - - -
     */
    perf_point*   point;

    /* - - - - - - - - - - -
     * private instance data
     * - - - - - - - - - - -
     */
    int server_id;

} i_perf_server;


i_perf_server* new_perf_server_base (void* implobj, i_perf*);
i_perf_server* new_perf_remote_server_base (void* implobj, i_perf*);
int destroy_perf_server_base (i_perf_server*);


#endif /* PERF_SERVER_H */

