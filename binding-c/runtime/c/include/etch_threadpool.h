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
 * etch_threadpool.h -- etch thread pool 
 */

#ifndef _ETCH_THREAD_POOL_H_
#define _ETCH_THREAD_POOL_H_

#ifdef __cplusplus
extern "C" {
#endif

#include "etch_errno.h"

#define ETCH_THREADPOOL_TYPE_FREE 0x01
#define ETCH_THREADPOOL_TYPE_QUEUED 0x02

typedef struct etch_threadpool_t etch_threadpool_t;

etch_status_t etch_threadpool_create(etch_threadpool_t** threadpool, uint8 type, const uint16 init_size);
etch_status_t etch_threadpool_remove(etch_threadpool_t* threadpool, const uint16 thread_id);
etch_status_t etch_threadpool_join(etch_threadpool_t* threadpool);
etch_status_t etch_threadpool_destroy(etch_threadpool_t* threadpool);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef _ETCH_THREAD_POOL_H_ */