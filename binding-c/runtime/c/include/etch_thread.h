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
 * etchpool.h -- threads and thread pool 
 */

#ifndef ETCHPOOL_H
#define ETCHPOOL_H

#include "apr_thread_proc.h"
#include "apr_atomic.h"

#include "etch_object.h"
#include "etch_mutex.h"
#include "etch_wait.h"
#include "etch_arraylist.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCH_THREADSTATE_INITIAL    0
#define ETCH_THREADSTATE_STARTED    4
#define ETCH_THREADSTATE_STOPPING   7
#define ETCH_THREADSTATE_STOPPED    8
#define ETCH_THREADSTATE_DEFUNCT    9

#define ETCH_DEFAULT_POOLTHREADS    4
#define ETCH_THREAD_PARAMS_SIGNATURE 0xcafef00d

//TODO: check global + do doc
// move to c file
// synchronize thread_id_farm var
extern unsigned thread_id_farm;


extern etch_mutex* loglock;


unsigned next_etch_threadid();
int etch_apr_init();
struct etch_threadpool;
struct etch_thread;
int etch_init_static_mutex(etch_mutex*, const int is_nestable);

struct etch_threadpool;

/** signature of a thread procedure */
typedef void (*etch_threadproc) (void*); 

/** signature of a thread procedure callback */
typedef int (*etch_thread_callback) (void*); 

/** interface to run a pool thread */
typedef struct etch_thread* (*etch_run) (struct etch_threadpool*, etch_threadproc, void*); 

/**
 * etch_apr_threaddata
 * thread parameters specific to the apache portable runtime. 
 * this object memory is always managed by the caller, regardless of
 * thread params is_xxxx configuration.
 */
typedef struct etch_apr_threaddata
{
    apr_pool_t*       mempool;
    apr_thread_t*     thread;
    apr_threadattr_t* threadattr;  
    unsigned char     threadstate;
    unsigned char     is_dedicated_mempool;

} etch_apr_threaddata;


/**
 * etch_thread_params
 * parameters to any thread start. 
 * an internal threadproc invokes the user threadproc, 
 * so we supply this object to the internal threadproc as data.
 */
typedef struct etch_thread_params
{
    unsigned int signature;           /* thread params sig */
    int etch_thread_id;               /* our sequential ID */
    etch_threadproc threadproc;       /* user thread proc */ 
    etch_thread_callback on_start;    /* thread start hook */ 
    etch_thread_callback on_exit;     /* thread exit hook */ 
    etch_apr_threaddata* libdata;     /* thread library data*/
    struct etch_threadpool* etchpool; /* thread pool if any */
    struct etch_thread* etchobj;      /* etch_thread object */ 
    etch_wait_t* waitobj;             /* post-start waiter */  
    void*  data;                      /* actual thread data */
    unsigned char is_own_data;        /* does thread own data */
    unsigned char is_data_etchobject; /* is destroy() in play */
    unsigned char threadstate;        /* current start state */
    unsigned char unused;
    etch_mutex_hookproc  synchook;
    etch_mutex_t*   synclock;

} etch_thread_params;


/** 
 *  etch_thread: thread object
 */
typedef struct etch_thread
{
    etch_object object;
    
    unsigned char   is_joined;

    int (*start) (void*);
    int (*stop)  (void*);

    etch_thread_params   params;
    etch_apr_threaddata  aprdata;
    etch_wait_t*         waiter;
    int64                startCond;
    etch_mutex_t*   objlock;

} etch_thread;


/** 
 *  etch_threadpool: thread pool object
 */
typedef struct etch_threadpool
{
    etch_object object;

    etch_run run;

    etch_mutex* pool_lock;

    /* to do: embed underlying queued threadpool here, such as APR pool */

    etch_arraylist* threadlist;
    etch_mutex* threadlist_lock; 
    unsigned int    threadpool_id;  
    int (*count) (); /* implementation should return threadlist.count */

    unsigned char   pooltype;      /* queued or free */
    unsigned char   is_defunct;    /* is pool forcibly destroying threads */
    unsigned char   is_iterating;  /* is pool currently being iterated */
    unsigned char   is_free_data;  /* will thread free its data memory */
    unsigned char   is_free_threads;     /* is poolthread memory freed */
    unsigned char   is_data_etchobject;  /* can thread call destroy() on data */
    unsigned char   is_manual_start;     /* is explicit thread.start() required */
    unsigned char   unused;      
    
} etch_threadpool;


/* 
 * global free thread pool
 */
extern etch_threadpool* global_free_threadpool;  
etch_threadpool* etch_glopool();
int etch_glopool_exit();

extern unsigned short etch_threadpool_id_farm;

#define ETCH_HAS_QUEUED_THREADPOOL (0)


etch_threadpool* new_threadpool(const unsigned pooltype, const int initsize);
etch_thread* new_thread (etch_threadproc, void*);
etch_thread* threadpool_remove_entry (etch_threadpool*, const int thread_id);
etch_thread* threadpool_thread (etch_threadpool*, const int i);
int    threadpool_waitfor_all(etch_threadpool*, const int is_cancel);
int    threadpool_remove (etch_threadpool*, const int id);
int    destroy_threadpool(void*);

void   etch_init_threadparams (etch_thread_params*);
int    etch_createthread (etch_thread_params*);
int    etch_thread_join  (etch_thread_params*);
int    etch_thread_cancel(etch_thread*);
int    etch_thread_current_id();
int    etch_join(etch_thread*);
void   etch_thread_yield ();
int    etch_on_threadstart(void*);
int    etch_on_threadexit (void*);
size_t etch_get_threadid  (void* threadstruct);
void   etch_sleep(const int ms);


#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHPOOL_H */
