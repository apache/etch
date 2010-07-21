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
 * etch_cache.h -- etch caching
 */

#ifndef _ETCH_CACHE_H_
#define _ETCH_CACHE_H_

#include "etch.h"
#include "etch_runtime.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*etch_cache_freehook_func)(void*, void*);

// etch_cache type
typedef struct etch_hashtable etch_cache_t;

/**
 * create a new cache instance.
 */
etch_status_t etch_cache_create();

/**
 * set freehook for cache values.
 */
etch_status_t etch_cache_set_freehook(etch_cache_freehook_func freehook);

/**
 * locate cached object with specified key, returning it or NULL
 * @param typ specified key
 * @param out   TODO: doc
 * @return      TODO: doc
 */
void* etch_cache_find(const unsigned int typ, void** out);

/**
 * Remove specified object from cache given integer key.
 * @param typ specified key
 * @return pointer to cached object, which is not freed here.
 */
void* etch_cache_del(const unsigned int typ);

/**
 * Add specified object to cache given integer key.
 * @param typ specified key
 * @return return 0 or -1.
 */
int etch_cache_add(const unsigned int typ, void* data);

/**
 * Add specified object to cache with existence test optional.
 * @param typ   TODO: doc
 * @param data      TODO: doc
 * @param is_check  TODO: doc
 * @return inserted item hash, or zero.
 */
int etch_cache_insert(const unsigned int typ, void* data, const int is_check);


/**
 * clear the cache and destroy the cache object
 * @return      TODO: doc
 */
int etch_cache_destroy();


#ifdef __cplusplus
}
#endif

#endif /* _ETCH_CACHE_H_ */
