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
 * xmpl_perf_server_stub.h
 * perf server stub
 */

#ifndef PERF_SERVER_STUB_H
#define PERF_SERVER_STUB_H

#include "etch_stub.h"
unsigned short CLASSID_STUBSERVER_PERF;


/**
 * perf_server_stub
 */
typedef struct perf_server_stub
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

    etch_stub* stub_base;
    int  session_id;           /* client session to which stub belongs */
    int (*destroyex) (void*);  /* user memory destructor */

    /* - - - - - - - - - - - - - - - - -
     * server_directed service virtuals
     * - - - - - - - - - - - - - - - - -
     */  

     /* no server-directed methods */

    /* - - - - - - - - - - - - - - - - -
     * service-specific allocations
     * - - - - - - - - - - - - - - - - -
     */   
    char* my_example_obj;  

} perf_server_stub;


perf_server_stub* new_perf_server_stub (etch_server_factory*, etch_session*);
int destroy_perf_server_stub (perf_server_stub*);


#endif /* PERF_SERVER_STUB_H */

