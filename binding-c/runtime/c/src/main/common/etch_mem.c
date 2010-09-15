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
 * etchmem.c -- heap memory allocate and free.
 * the c binding wraps the heap allocator in order to track allocations. 
 * we supply the etch_malloc macro which, when ETCH_DEBUGALLOC is defined, 
 * will accept module name and code line number, along with object type and
 * allocation byte length, in order to track allocations and frees, and thus 
 * enable identification of memory leaks. 
 */

#include "etch_mem.h"
#include "etch_log.h"

static mallocFunc g_malloc = NULL;
static freeFunc   g_free = NULL;
static reallocFunc g_realloc = NULL;

void* _etch_malloc(size_t size, char* file, int line) {
    void* p = NULL;
    
    if (g_malloc == NULL)
        p = malloc(size);
    else
        p = g_malloc(size);
        
    //printf("malloc Mem: %p, File: %s, Line: %d\n", p, file, line);
    //fflush(stdout);
    return p;
}

void _etch_free(void* mem, char* file, int line) {
    //printf("free Mem: %p, File: %s, Line: %d\n", mem, file, line);
    //fflush(stdout);
    if (g_free == NULL)
        free(mem);
    else
        g_free(mem);
}

void* _etch_realloc(void* p, size_t size, char* file, int line) {
    void* res = NULL;

    if (g_realloc == NULL)
        res = realloc(p, size);
    else
        res = g_realloc(p, size);

    return res;        
}

void etch_set_mallocator(mallocFunc myMallocFunc, freeFunc myFreeFunc, reallocFunc myReallocFunc)
{
    g_malloc = myMallocFunc;
    g_free = myFreeFunc;
    g_realloc = myReallocFunc;
}
