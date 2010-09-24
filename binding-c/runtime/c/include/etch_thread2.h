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
 * etch_thread.h -- thread implementation 
 */

#ifndef _ETCH_THREAD_H_
#define _ETCH_THREAD_H

#include "etch_errno.h"
#include "etch_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCH_THREAD_CREATE_JOINABLE 0
#define ETCH_THREAD_CREATE_DETACHED 1

typedef struct etch_thread_attr_t etch_thread_attr_t;
typedef struct etch_thread_t etch_thread_t;

typedef void (*etch_thread_func)(etch_thread_t* thread, void* data);

/**
 * create a new thread attr instance.
 * @param attr
 * @return status
 */
etch_status_t etch_thread_attr_create(etch_thread_attr_t** attr);

/**
 * set thread detach state.
 * @param attr
 * @param state ETCH_THREAD_CREATE_JOINABLE or ETCH_THREAD_CREATE_DETACHED
 * @return status
 */
etch_status_t etch_thread_attr_set_detachstate(etch_thread_attr_t* attr, int state);

/**
 * get thread detach state.
 * @param attr
 * @param state ETCH_THREAD_CREATE_JOINABLE or ETCH_THREAD_CREATE_DETACHED
 * @return status
 */
etch_status_t etch_thread_attr_get_detachstate(etch_thread_attr_t* attr, int* state);

/**
 * destroy thread attr instance.
 * @param attr
 * @return status
 */
etch_status_t etch_thread_attr_destroy(etch_thread_attr_t* attr);


/**
 * create a new thread.
 * @param thread
 * @param attr
 * @param thread_proc
 * @param data
 * @return status
 */
etch_status_t etch_thread_create(etch_thread_t** thread, const etch_thread_attr_t* attr, etch_thread_func thread_proc, void* data);

/**
 * wait thread proc has finished. 
 * @param thread
 * @param status return code of thread exit
 * @return status
 */
etch_status_t etch_thread_join(etch_thread_t* thread, etch_status_t* status);

/**
 * yield current thread.
 * @param status
 */
etch_status_t etch_thread_yield();

/**
 * thread exit, return status
 * @param thread
 * @param status thread exit status
 * @return status
 */
etch_status_t etch_thread_exit(etch_thread_t* thread, etch_status_t status);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ETCH_THREAD_H */
