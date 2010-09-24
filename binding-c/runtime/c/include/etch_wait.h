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
 * etch_wait.h -- wait implementation
 */

#ifndef _ETCH_WAIT_H_
#define _ETCH_WAIT_H_

#include "etch_errno.h"
#include "etch_mem.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct etch_wait_t etch_wait_t;

/**
 * create a new etch wait instance. 
 * @param waiter address where the new created mutex will be stored.
 * @param pool memory pool, if pool is null the global pool will be used
 * @return status
 */
etch_status_t etch_wait_create(etch_wait_t** waiter, etch_pool_t* pool);

/**
 * set autoreset mode
 * @param waiter instance.
 * @param autorest mode 0 = autoreset off, 1 = autoreset on. If autoreset is
 *        on, the signal state will automaticly reset after a signal was send to
 *        the sleeping threads.
 * @return status
 */


/**
 * wait until signaled.
 * @param waiter instance
 * @return status
 */
etch_status_t etch_wait_wait(etch_wait_t* waiter, int64 cond_value);

/**
 * wait until signaled and new value.
 * @param waiter instance
 * @param timeout in ms
 * @return status
 */
etch_status_t etch_wait_wait_and_set(etch_wait_t* waiter, int64 cond_value, int64 new_cond_value);

/**
 * wait timed until signaled.
 * @param waiter instance
 * @param timeout in ms
 * @return status
 */
etch_status_t etch_wait_timedwait(etch_wait_t* waiter, int64 cond_value, uint32 timeout);

/**
 * wait timed until signaled and new value.
 * @param waiter instance
 * @param timeout in ms
 * @return status
 */
etch_status_t etch_wait_timedwait_and_set(etch_wait_t* waiter, int64 cond_value, uint32 timeout, int64 new_cond_value);


/**
 * set etch_wait in signaled state, all waiting thread will
 * be signaled
 * @param waiter instance
 * @return status
 */
etch_status_t etch_wait_set(etch_wait_t* waiter, int64 cond_value);

/**
 * set condition variable without signaling waiting threads
 * be signaled
 * @param waiter instance
 * @return status
 */
etch_status_t etch_wait_reset(etch_wait_t* waiter, int64 cond_value);

/**
 * destroy the given mutex.
 * @param mutex
 */
etch_status_t etch_wait_destroy(etch_wait_t* waiter);

#ifdef __cplusplus
}
#endif

#endif /* _ETCH_WAIT_H_ */
