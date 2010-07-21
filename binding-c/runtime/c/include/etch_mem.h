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
 * etchmem.h -- heap memory allocate and free.
 * The c binding wraps the heap allocator in order to track allocations. we supply 
 * the etch_malloc macro which, when ETCH_DEBUGALLOC is defined,  will accept module 
 * name and code line number, along with object type and allocation size, in order  
 * to track heap allocations and frees, and to subsequently report memory leaks. 
 */

#ifndef ETCHMEM_H
#define ETCHMEM_H

#include "etch.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef apr_pool_t etch_pool_t;

#define etch_malloc(n,o) _etch_malloc(n, __FILE__, __LINE__)
#define etch_realloc(p,n,o) _etch_realloc(p,n,__FILE__, __LINE__)
#define etch_free(n) _etch_free(n, __FILE__, __LINE__)
#define etch_showmem(f,c)
#define etch_dumpmem()

void* _etch_malloc(size_t size, char* file, int line);
void* _etch_realloc(void*, size_t size, char* file, int line);
void _etch_free(void* mem, char* file, int line);

/* For own malloc/free hook */
typedef void*(*mallocFunc)(size_t);
typedef void(*freeFunc)(void*);
typedef void*(*reallocFunc)(void*,size_t);

void etch_set_mallocator(mallocFunc myMallocFunc, freeFunc myFreeFunc, reallocFunc myReallocFunc);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHMEM_H*/ 
