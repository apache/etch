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
 * test_allocator.c -- test the debug allocator 
 */
#include "apr_time.h" /* some apr must be included first */
#include "etchthread.h"
#include <tchar.h>
#include <stdio.h>
#include <conio.h>
#include "etchobj.h"
#include "etch_global.h"

#include "cunit.h"
#include "basic.h"
#include "automated.h"

int apr_setup(void);
int apr_teardown(void);
int this_setup();
int this_teardown();
apr_pool_t* g_apr_mempool;
const char* pooltag = "etchpool";


/* - - - - - - - - - - - - - - 
 * unit test infrastructure
 * - - - - - - - - - - - - - -
 */

int init_suite(void)
{
    apr_setup();
    etch_runtime_init(TRUE);
    return this_setup();
}

int clean_suite(void)
{
    this_teardown();
    etch_runtime_cleanup(0,0); /* free memtable and cache etc */
    apr_teardown();
    return 0;
}

int g_is_automated_test, g_bytes_allocated;

#define IS_DEBUG_CONSOLE FALSE

/*
 * apr_setup()
 * establish apache portable runtime environment
 */
int apr_setup(void)
{
    int result = apr_initialize();
    if (result == 0)
    {   result = etch_apr_init();
        g_apr_mempool = etch_apr_mempool;
    }
    if (g_apr_mempool)
        apr_pool_tag(g_apr_mempool, pooltag);
    else result = -1;
    return result;
}

/*
 * apr_teardown()
 * free apache portable runtime environment
 */
int apr_teardown(void)
{
    if (g_apr_mempool)
        apr_pool_destroy(g_apr_mempool);
    g_apr_mempool = NULL;
    apr_terminate();
    return 0;
}

int this_setup()
{
    etch_apr_mempool = g_apr_mempool;
    return 0;
}

int this_teardown()
{    
    return 0;
}


#define PAYLOADSIG 0xf00f00
#define NUMITEMS 3

typedef struct payload
{
    int count;
    int signature;
} payload;


/* 
 * This subtest test our ability to build a hashtable each key of which is a
 * pointer to that key's value, in other words, the hashtable stores a void**
 * as they key. This essentially models a debug allocator. 
 */
void malloc_test(void)
{
    /* These are what is stored in the hash table for each hash, two pointers.
       To clarify, *pointers* are what is copied into the hashtable, not values.
       So it follows that if I want to use a pointer as a hash key, what I must
       store in the hashtable as the key is a pointer to that pointer.
    */
    char **pkey = 0; void *pval = 0; 
     
    payload** allocations[NUMITEMS];
    int i, testresult = 0, count = 0;
    etch_hashtable* myhash;
    etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket;

    is_memtable_instance = TRUE;
    myhash = new_hashtable(16);  /* create hash table */
    is_memtable_instance = FALSE;
    CU_ASSERT_PTR_NOT_NULL_FATAL(myhash); 

    /* insert NUMITEMS allocations into the tracking table
     */
    for(i = 0; i < NUMITEMS; i++)
    {
        int      result = 0;
        void*    pkey = 0;
        payload* pval = 0;
       
        pkey = malloc(sizeof(void*));        /* allocate the key*    */        
        pval = malloc(sizeof(payload));      /* allocate the value   */
        pval->count = i; pval->signature = PAYLOADSIG; /* debug info */
        memcpy(pkey, &pval, sizeof(void*));  /* copy value* into key */
        allocations[i] = pkey  ;             /* save for iterating   */

        result = myhash->vtab->insert(myhash->realtable, pkey, sizeof(void*), pval, sizeof(payload), 0, 0);
        CU_ASSERT_EQUAL(result,0);
    } 

    /* ensure there are NUMITEMS entries in the tracking table*/
    count = myhash->vtab->count(myhash->realtable, 0, 0);
    CU_ASSERT_EQUAL_FATAL(count,NUMITEMS);

    for(i = 0; i < NUMITEMS; i++)
    {
        int       result = 0;
        payload*  pval = 0;
        payload** pkey = (payload**) &allocations[i]; 

        result = myhash->vtab->find(myhash->realtable, *pkey, sizeof(void*), NULL, &myentry); 
        CU_ASSERT_EQUAL(result,0);

        pval = (payload*)myentry->value;
        CU_ASSERT_EQUAL(pval->signature, PAYLOADSIG);
        CU_ASSERT_EQUAL(pval->count, i);
    } 

    /* 
     * Clear the hashtable, while doing so ask hashtable to free memory at the 
     * and value pointers. Since we have individually allocated both keys (4 bytes
     * each, each a void**), and values (sizeof(payload) bytes each), we can do so. 
     * If we do not crash, this mini-test succeeds.
     * CUnit note: there is no easy way to test if this succeeds, since the hashtable
     * no longer exists if successful, and an attempt to reference it would elicit 
     * an exception.
     */
    myhash->vtab->clear(myhash->realtable, TRUE, TRUE, 0, 0);

    g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */ 
}


/* 
 * This subtest tests our implemented debug allocator. 
 */
void etch_malloc_test(void)
{
    /* These are what is stored in the hash table for each hash, two pointers.
       To clarify, *pointers* are what is copied into the hashtable, not values.
       So it follows that if I want to use a pointer as a hash key, what I must
       store in the hashtable as the key is a pointer to that pointer.
    */
     
    payload* allocations[NUMITEMS];
    int i = 0, testresult = 0, count = 0;
    etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket;

    /* insert NUMITEMS allocations into the tracking table
     */
    for(i = 0; i < NUMITEMS; i++)
    {
        void*    pmem = 0;
        payload* pval = 0;

        pval = etch_malloc(sizeof(payload), 0); /* allocate value, get key */ 
        CU_ASSERT_PTR_NOT_NULL_FATAL(pval); 

        pval->count = i; pval->signature = PAYLOADSIG; /* debug info */
        allocations[i] = pval  ;   /* save for iterating */
    } 

    /* ensure there are NUMITEMS entries in the tracking table */
    CU_ASSERT_PTR_NOT_NULL_FATAL(memtable); 
    count = memtable->vtab->count(memtable->realtable, 0, 0);  
    CU_ASSERT_EQUAL_FATAL(count, NUMITEMS);   
 
    for(i = 0; i < NUMITEMS; i++)
    {
        int n, result;
        payload* pval = (payload*) allocations[i]; 
        n = pval->count;

        result = etch_free(pval);        
        CU_ASSERT_EQUAL(result, 0);
    } 

    /* ensure there are zero entries in the tracking table */
    CU_ASSERT_PTR_NOT_NULL_FATAL(memtable);
    CU_ASSERT_EQUAL_FATAL(memtable->vtab->count(memtable->realtable, 0, 0), 0);

    g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */  
}


/* 
 * This subtest tests that the debug allocator reports un-freed memory as expected.
 */
void negative_etch_malloc_test(void)
{
    struct 
    {   double d;
    } EIGHTBYTES;
    struct
    {   char x[496];
        int  n;
    } FIVEHUNDREDBYTES;

    void *p1=0, *p2=0;
    int   bytes_allocated = 0;
    memtable = NULL; /* ensure not dangling from a prior test */

    p1 = etch_malloc(sizeof(EIGHTBYTES), 0); 
    p2 = etch_malloc(sizeof(FIVEHUNDREDBYTES), 0);  

    bytes_allocated = etch_showmem(0,0); /* get leaks - don't free them */
    CU_ASSERT_EQUAL(bytes_allocated, sizeof(EIGHTBYTES) + sizeof(FIVEHUNDREDBYTES));
     
    etch_free(p2);
    bytes_allocated = etch_showmem(0,0); 
    CU_ASSERT_EQUAL(bytes_allocated, sizeof(EIGHTBYTES));

    etch_free(p1);

    g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */ 
}


/* 
 * This subtest tests that the debug allocator reports un-freed memory as expected.
 */
void etch_realloc_test(void)
{
    struct 
    {   double d;
    } EIGHTBYTES;
    struct
    {   char x[496];
        int  n;
    } FIVEHUNDREDBYTES;

    void *p1=0, *p2=0;
    int   bytes_allocated = 0;    

    /* same as malloc */
    p1 = etch_realloc(NULL, sizeof(EIGHTBYTES), 0);
    CU_ASSERT_PTR_NOT_NULL(p1);
    bytes_allocated = etch_showmem(0,0); /* get leaks - don't free them */
    CU_ASSERT_EQUAL(bytes_allocated, sizeof(EIGHTBYTES));

    /* same as free */
    p2 = etch_realloc(p1, 0, 0);
    bytes_allocated = etch_showmem(0,0);  
    CU_ASSERT_EQUAL(bytes_allocated, 0);

    /* now expand the memory */
    p1 = etch_realloc(NULL, sizeof(EIGHTBYTES), 0);
    CU_ASSERT_PTR_NOT_NULL(p1);
    bytes_allocated = etch_showmem(0,0);  
    CU_ASSERT_EQUAL(bytes_allocated, sizeof(EIGHTBYTES));
    *((long *) p1) = 1234;

    p2 = etch_realloc(p1, sizeof(FIVEHUNDREDBYTES), 0);

    CU_ASSERT_PTR_NOT_NULL(p2);
    bytes_allocated = etch_showmem(0,0);  
    CU_ASSERT_EQUAL(bytes_allocated, sizeof(FIVEHUNDREDBYTES));
    CU_ASSERT( (*( (long *) p2) ) == 1234 );

    etch_free(p2);


    /* now shrink memory */
    p1 = etch_realloc(NULL, sizeof(FIVEHUNDREDBYTES), 0);
    CU_ASSERT_PTR_NOT_NULL(p1);
    bytes_allocated = etch_showmem(0,0);  
    CU_ASSERT_EQUAL(bytes_allocated, sizeof(FIVEHUNDREDBYTES));
    *((long *) p1) = 1234;

    p2 = etch_realloc(p1, sizeof(EIGHTBYTES), 0);

    CU_ASSERT_PTR_NOT_NULL(p2);
    bytes_allocated = etch_showmem(0,0);  
    CU_ASSERT_EQUAL(bytes_allocated, sizeof(EIGHTBYTES));
    CU_ASSERT( (*( (long *) p2) ) == 1234 );

    etch_free(p2);

    g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */  
}


/**
 * main   
 */
int _tmain(int argc, _TCHAR* argv[])
{
    char c=0;
    CU_pSuite pSuite = NULL;
    g_is_automated_test = argc > 1 && 0 != wcscmp(argv[1], L"-a");
    if (CUE_SUCCESS != CU_initialize_registry()) return CU_get_error();

    CU_set_output_filename("../test_allocator");
    pSuite = CU_add_suite("suite_allocator", init_suite, clean_suite);

    CU_add_test(pSuite, "hashtable pointer key test", malloc_test);
    CU_add_test(pSuite, "debug realloc test", etch_realloc_test);  
    CU_add_test(pSuite, "debug allocator test", etch_malloc_test);
    CU_add_test(pSuite, "negative debug allocator test", negative_etch_malloc_test); 

    if (g_is_automated_test)    
        CU_automated_run_tests();    
    else
    {   CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    }
     
    if (!g_is_automated_test) { printf("any key ..."); while(!c) c = _getch();  wprintf(L"\n"); }     
    CU_cleanup_registry();
    return CU_get_error(); 
}

