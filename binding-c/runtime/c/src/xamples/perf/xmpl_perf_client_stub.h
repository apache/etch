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
 * xmpl_perf_client_stub.h
 * perf client stub
 */

#ifndef PERF_CLIENT_STUB_H
#define PERF_CLIENT_STUB_H

#include "etch_stub.h"
unsigned short CLASSID_STUBCLIENT_PERF;

#define is_perf_client_stub(p) ( p && ((objmask*)p)->class_id == CLASSID_STUBCLIENT_PERF )


/**
 * perf_client_stub
 */
typedef struct perf_client_stub
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
    int  server_id;
    int (*destroyex) (void*);  /* user memory destructor */

    /* - - - - - - - - - - - - - - - - -
     * client_directed service virtuals
     * - - - - - - - - - - - - - - - - -
     */  

    /* no client-directed items defined */   

    /* - - - - - - - - - - - - - - - - -
     * user-allocated instance data
     * - - - - - - - - - - - - - - - - -
     */   
    char* my_example_obj;  /* example data */

} perf_client_stub;


perf_client_stub* new_perf_client_stub (etch_client_factory*);    
int destroy_perf_client_stub (perf_client_stub*);


#endif /* PERF_CLIENT_STUB_H */

