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
 * etchmutex.h -- thread mutex implementation
 * currently wraps APR mutex. 
 */

#ifndef ETCHMUTEX_H
#define ETCHMUTEX_H

#include "etchobj.h"

typedef struct etchmutex etchmutex;
typedef struct etchwait  etchwait;
#define ETCHMUTEX_NESTED TRUE
#define ETCHMUTEX_UNNESTED FALSE


/** signature of a mutex operation */
typedef int (*etchmutex_operation)  (struct etchmutex*); 

/** signatures of a wait operation */
typedef int (*etchwait_operation)(etchwait*);
typedef int (*etchwait_timedsig) (etchwait*, int);
typedef int (*etchwait_condition)(etchwait*, int64*, int64);
typedef int (*etchwait_timedcond)(etchwait*, int64*, int64, int);
typedef int (*etchwait_setcond)  (etchwait*, int64); 


/** 
 *  etchmutex: mutex object
 */
struct etchmutex
{
    unsigned int    hashkey;
    unsigned short  obj_type; 
    unsigned short  class_id;
    struct i_iterable* vtab;
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

    void* impl; 
    int   lockcount;

    etchmutex_operation acquire;
    etchmutex_operation tryacquire;
    etchmutex_operation release;
};

typedef struct etchmutex etchmutex;

etchmutex* new_mutex(void* params, const int is_nestable);
int destroy_etchmutex(etchmutex*);

int etchmutex_hookproc(int action, void* mutex);


/** 
 * etchwait: condition variable object with associated mutex.
 * similar to the java binding's monitor except that a monitored variable state 
 * is not tracked automatically -- instead the etchwait object's set() method 
 * must be invoked to change the monitored variable's value. waiting threads
 * can always be unblocked with signal(). if set() is used, waiters will be 
 * unblocked if the value being set() is equal to the value specified at 
 * invocation of waitequal() or timed_waitequal().
 */
typedef struct etchwait
{
    unsigned int    hashkey;
    unsigned short  obj_type; 
    unsigned short  class_id;
    struct i_iterable* vtab;
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

    void*  impl;          /* wait implementation - owned */
    int    timeoutms;     /* optional timeout: 0 forever */
    int64* cond_var;      /* optional condition variable */
    int64  cond_waitfor;  /* optional value to wait for */
    int    threadcount;   /* count of threads waiting */
    etchmutex* mutex;     /* associated mutex - owned */

    /** block until signal */
    etchwait_operation  wait;

    /** block until signal or timeout */
    etchwait_timedsig   timed_wait;

    /** block until condition variable equal */
    etchwait_condition  waitequal;

    /** block until condition equal or timeout */
    etchwait_timedcond  timed_waitequal;

    /** unblock waiter */
    etchwait_operation  signal;

    /** unblock all waiters */
    etchwait_operation  broadcast;

    /** set monitored value and signal if condition met */
    etchwait_setcond    set; 

} etchwait;


etchwait* new_wait(void* params);
int destroy_etchwait (etchwait*);
int etchmutex_release(etchmutex*);
int etchmutex_acquire(etchmutex*);  
int etchmutex_tryacquire(etchmutex*);


#endif /* #ifndef ETCHMUTEX_H */
