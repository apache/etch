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
 * xmpl_perf.h
 * perf service data
 */

#ifndef PERF_H
#define PERF_H

#include "etchobj.h"

unsigned short CLASSID_PERF_SERVICE_INTERFACE;
unsigned short CLASSID_PERF_POINT;

typedef struct perf_point perf_point;

typedef etch_int32* (*perf_add)     (void* thisx, etch_int32*, etch_int32*);
typedef etch_int32* (*perf_sum)     (void* thisx, etch_arraytype*);
typedef void*       (*perf_report)  (void* thisx, etch_int32*, etch_string*);
typedef perf_point* (*perf_dist)    (void* thisx, perf_point*, perf_point*);
typedef etch_date*  (*perf_add2)    (void* thisx, etch_date*, etch_int64*); 
typedef void*       (*perf_report2) (void* thisx, etch_date*, etch_int32*, etch_string*);

typedef struct i_mailbox i_mailbox;

typedef i_mailbox*  (*perf_async_begin_add)     (void* thisx, etch_int32*, etch_int32*);
typedef i_mailbox*  (*perf_async_begin_sum)     (void* thisx, etch_arraytype*);
typedef i_mailbox*  (*perf_async_begin_report)  (void* thisx, etch_int32*, etch_string*);
typedef i_mailbox*  (*perf_async_begin_dist)    (void* thisx, perf_point*, perf_point*);
typedef i_mailbox*  (*perf_async_begin_add2)    (void* thisx, etch_date*, etch_int64*); 
typedef i_mailbox*  (*perf_async_begin_report2) (void* thisx, etch_date*, etch_int32*, etch_string*);

typedef etch_int32* (*perf_async_end_add)     (void* thisx, i_mailbox*);
typedef etch_int32* (*perf_async_end_sum)     (void* thisx, i_mailbox*);
typedef void*       (*perf_async_end_report)  (void* thisx, i_mailbox*);
typedef perf_point* (*perf_async_end_dist)    (void* thisx, i_mailbox*);
typedef etch_date*  (*perf_async_end_add2)    (void* thisx, i_mailbox*); 
typedef void*       (*perf_async_end_report2) (void* thisx, i_mailbox*);


/**
 * perf_point 
 * perf service value object "point" 
 */
typedef struct perf_point
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

    int x;
    int y;

} perf_point;


perf_point* new_perf_point (const int x, const int y);
int is_perf_point(void* obj); 


/**
 * i_perf
 * perf service interface
 */
typedef struct i_perf
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

} i_perf;


i_perf* new_perf_service_interface();


#endif /* PERF_H */

