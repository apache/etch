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
 * etch_mutex.h -- thread mutex implementation
 * currently wraps APR mutex. 
 */

#ifndef _ETCH_MUTEX_H_
#define _ETCH_MUTEX_H_

#include "etch_errno.h"
#include "etch_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCH_MUTEX_NESTED   0x1
#define ETCH_MUTEX_UNNESTED 0x2

/**
 * etch mutex type
 */
typedef struct etch_mutex_t etch_mutex_t;

typedef etch_mutex_t etch_mutex;
typedef int (*etch_mutex_hookproc)(int action, etch_mutex* mutex);

/**
 * create a new etch mutex instance.
 * @param address where the new created mutex will be stored.
 * @param flags mutex type ETCH_MUTEX_NESTED or ETCH_MUTEX_UNNESTED.
 * @param pool memory pool where the memory will be allocated from.
 * @return status
 */
etch_status_t etch_mutex_create(etch_mutex_t** mutex, unsigned int flags, etch_pool_t* pool);

/**
 * acquire the lock for the given mutex.
 * @param mutex
 * @return status
 */
etch_status_t etch_mutex_lock(etch_mutex_t* mutex);

/**
 * try to acquire the lock for the given mutex.
 * @param mutex
 * @return status
 */
etch_status_t etch_mutex_trylock(etch_mutex_t* mutex);

/**
 * release the lock for the given mutex.
 * @param mutex
 * @return status
 */
etch_status_t etch_mutex_unlock(etch_mutex_t* mutex);

/**
 * destroy the given mutex.
 * @param mutex
 */
etch_status_t etch_mutex_destroy(etch_mutex_t* mutex);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_MUTEX_H */
