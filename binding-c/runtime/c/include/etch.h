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

#ifndef _ETCH_H_
#define _ETCH_H_

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <stdarg.h>
#include <ctype.h>
#include <assert.h>

#include <apr_pools.h>
#include <apr_atomic.h>
#include <apr_errno.h>
#include <apr_general.h>
#include <apr_thread_mutex.h>
#include <apr_thread_cond.h>
#include <apr_portable.h>
#include <apr_strings.h>
#include <apr_time.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * macros
 */
#define ETCH_ASSERT(condition) assert(condition)
#define ETCH_ASSERT_EXIT(condition) assert(condition)

//TODO: check which component use this defines, change functions
// to use etch_status_t
#define ETCH_DEFSIZE     0
#define ETCH_INFWAIT     0
#define ETCH_NOWAIT    (-1)
#define ETCH_TIMEOUT     1
#define ETCH_INTERRUPTED 0

//TODO: check this stuff, check etch_nativearray encoding (QNX, Windows)
#define ETCH_ENCODING_ASCII 0
#define ETCH_ENCODING_UTF8  1
#define ETCH_ENCODING_UTF16 2
#define ETCH_ENCODING_UTF32 3
#define ETCH_ENCODING_UCS2  4
#define ETCH_ENCODING_UCS4  5
#define ETCH_ENCODING_UNICODE 6

#define ETCH_ENCODING_DEFAULT ETCH_ENCODING_UTF8
#define IS_ETCH_ENCODING_8BIT(n) (n == ETCH_ENCODING_UTF8 || n == ETCH_ENCODING_ASCII)

//TODO: check this sutff, etch_thread
#define ETCH_THREADPOOLTYPE_FREE   0
#define ETCH_THREADPOOLTYPE_QUEUED 1

// TODO: check this stuff
#define ETCHMAKECLASS(t,c) ((t << 16) | c)
#define ETCHGETCLASS(n,t,c) do{ c=(short)(n & 0xffff); t=(short)(n>>16); }while(0);

//TODO: check this stuff,etch_stub
#define ETCH_ASYNCMODE_NONE   0  /* implies operation is synchronous */
#define ETCH_ASYNCMODE_QUEUED 1  /* operation is queued to a thread pool */
#define ETCH_ASYNCMODE_FREE   2  /* operation is executed in a new thread */

//TODO: check, etch_tcpconxn
#define ETCH_SHUTDOWNSIGNAL "$ETCHQUIT"

/**
 * common defines
 */
#undef  NULL
#define NULL  0

#undef  FALSE
#define FALSE 0

#undef  TRUE
#define TRUE  1

/**
 * typedefs of basic datatypes
 */
typedef char                int8;
typedef unsigned char       uint8;
typedef short int           int16;
typedef unsigned short int  uint16;
typedef int                 int32;
typedef unsigned int        uint32;
typedef long long           int64;
typedef unsigned long long  uint64;
typedef unsigned char       byte;
typedef unsigned char       boolean;

uint32 etchhash(const void* key, const int keylen, const unsigned priorhash);

typedef int (*etch_comparator) (void* myobj, void* otherobj);

#define IS_ETCH_TRANSPORT_LITTLEENDIAN FALSE
#define ETCHCONFIGDEF_EXAMPLE_STRING_MAXLEN 15 


#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_H */
