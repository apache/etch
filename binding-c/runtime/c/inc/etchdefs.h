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
 * etchdefs.h -- type and constant definitions etc
 */

#ifndef ETCHDEFS_H
#define ETCHDEFS_H
#include <io.h>

typedef long long int64;
typedef unsigned long long uint64;
typedef unsigned char byte;
typedef unsigned char boolean;
#undef  NULL
#define NULL  0
#undef  FALSE
#define FALSE 0
#undef  TRUE
#define TRUE  1
#undef  BOOLEAN
#define BOOLEAN int
#undef  BYTE


#ifdef  IS_WINDOWS_ETCH
#define MAXPATHSIZE 260  /* value of MAX_PATH in windef.h */
#define etch_deletefile _unlink
#else
#define MAXPATHSIZE 260  /* TODO max pathsize for linux etc */
#define etch_deletefile unlink
#endif

#ifndef _TCHAR
#define _TCHAR wchar_t
#endif

#define MAX_ETCHHASHKEY 128 /* arbitrary max byte length of a hashtable key */

#define INITIAL_CACHE_SIZE_ITEMS 32  /* global cache starts out capacity 32 */

#define ETCH_ASSERT(condition) assert(condition)

#define ETCH_ASYNCMODE_NONE   0  /* implies operation is synchronous */
#define ETCH_ASYNCMODE_QUEUED 1  /* operation is queued to a thread pool */
#define ETCH_ASYNCMODE_FREE   2  /* operation is executed in a new thread */

#define ETCH_DEFSIZE     0
#define ETCH_INFWAIT     0
#define ETCH_NOWAIT    (-1)
#define ETCH_TIMEOUT     1
#define ETCH_INTERRUPTED 2

#define ETCH_THREADPOOLTYPE_FREE   0
#define ETCH_THREADPOOLTYPE_QUEUED 1

#define ETCH_TRANSPORT_FORMAT_BINARY 0
#define ETCH_TRANSPORT_FORMAT_XML    1

#define ETCH_ENCODING_ASCII 1
#define ETCH_ENCODING_UTF8  2
#define ETCH_ENCODING_UTF16 3
#define ETCH_ENCODING_DEFAULT ETCH_ENCODING_UTF8
#define IS_ETCH_ENCODING_8BIT(n) (n == ETCH_ENCODING_UTF8 || n == ETCH_ENCODING_ASCII)

#define ETCHMAKECLASS(t,c) ((t << 16) | c)
#define ETCHGETCLASS(n,t,c) do{ c=(short)(n & 0xffff); t=(short)(n>>16); }while(0);

#define ETCH_SHUTDOWNSIGNAL "$ETCHQUIT"

#define ETCH_DEBUG 
#ifdef  ETCH_DEBUG
int g_debugtest;
#endif 



#endif /* #ifndef ETCHDEFS_H */ 