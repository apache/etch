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
#include "etchhash.h"


#define ETCH_DEBUGALLOC

/*
 * map etch_malloc() to the standard allocator or the debug allocator, 
 * depending on whether ETCH_DEBUGALLOC is defined. likewise etch_free().
 */
#ifdef  ETCH_DEBUGALLOC

void* debug_malloc(size_t nbytes, const short objtype, char* file, const short line);
void* debug_realloc(void* p, size_t nbytes, const short objtype, char* file, const short line);
int   debug_free(void* p, char* file, const short line); /* __FILE__ is not unicode */ 
int   debug_showmem(const int is_freeitem, const int is_console);
void  debug_dumpmem();

#define etch_malloc(n,o) debug_malloc(n,o,__FILE__,(const short) __LINE__)
#define etch_realloc(p,n,o) debug_realloc(p,n,o,__FILE__,(const short) __LINE__)
#define etch_free(n) debug_free(n,__FILE__,(const short) __LINE__)
#define etch_showmem(f,c) debug_showmem(f,c) 
#define etch_dumpmem() debug_dumpmem()

#else   /* ETCH_DEBUGALLOC */

#define etch_malloc(n,o) malloc(n)
#define etch_realloc(p,n,o) realloc(p,n)
#define etch_free(n) free(n)
#define etch_showmem(f,c)  
#define etch_dumpmem()

#endif  /* ETCH_DEBUGALLOC */



#ifdef ETCH_DEBUGALLOC

#define DEFETCHHEAPTABLESIZE 1024 /* initial size of heap tracking table */

/* compile time memory debugging switches */
#define IS_TRACKING_ETCHHEAP 1    /* 1 to watch alloc address or memory */
#define IS_USING_ALLOC_ID    1    /* 1 to store an ID # with each alloc */
#define IS_USING_DEBUGBREAK  1    /* 1 to break at alloc# etch_watch_id */


/*
 * memrec is a record of an individual memory allocation
 */
typedef struct memrec
{
  size_t   size;     /* allocated bytes */
  short    objtype;  /* type of allocation */
  short    line;     /* source line number */
  unsigned filehash; /* hash of filepath */
  #if IS_USING_ALLOC_ID
  unsigned id;      /* allocation serial number */
  #endif
} memrec;


/*
 * memtable is the hash table used for heap allocation tracking.
 */
etch_hashtable* memtable;
int  is_memtable_instance;  /* indicator to not etch_malloc() the memtable */
size_t etchheap_currbytes;  /* total bytes currently etch_malloc() 'ed */
size_t etchheap_maxbytes;   /* high water mark - number of bytes */
unsigned curr_alloc_id;     /* current memory allocation serial number */
unsigned etchheap_count;    /* current number of allocations */
unsigned etchheap_hiwater;  /* high water mark - number of allocations */

size_t etch_watch_addr;     /* a memory address to watch */
unsigned etch_watch_id;     /* an allocation # to watch */


#endif /* ETCH_DEBUGALLOC */

#endif /* #ifndef ETCHMEM_H*/ 