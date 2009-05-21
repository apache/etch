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
 * test_cache.c
 * test the runtime object cache
 * we can swap out cache back ends and this test should work the same regardless
 */
#include "apr_time.h" /* some apr must be included first */
#include "etchthread.h"
#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etch_global.h"
#include "etch_arraylist.h"

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


/**
 * This subtest instantiates various etch objects which cache some part of 
 * themselves, and destroys the objects. At each step the test verifies that
 * the cache contains the expected number of entries, e.g. if I create 
 * multiple hashtables I should only have cached one hashtable vtable.
 */
void test_multiple_items(void)
{
    int cache_start_count = 0, cache_current_count;
    int result1 = 0, result2 = 0, result3 = 0;
    etch_hashtable* myhashtab1 = NULL;     
    etch_hashtable* myhashtab2 = NULL; 
    etch_hashtable* myhashtab3 = NULL;     
    etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket;

    wchar_t* wstr1 = L"abracadabra"; 
    wchar_t* wstr2 = L"gilgamesh";
    wchar_t* wstr3 = L"antidisestablishmentarianism";

    const size_t numElements1 = wcslen(wstr1);
    const size_t numElements2 = wcslen(wstr2);
    const size_t numElements3 = wcslen(wstr3);

    const size_t numBytes1 = sizeof(wchar_t) * numElements1;
    const size_t numBytes2 = sizeof(wchar_t) * numElements2;
    const size_t numBytes3 = sizeof(wchar_t) * numElements3;

    size_t actlen1 = 0, actlen2 = 0, actlen3 = 0;
    wchar_t *key1 = NULL, *key2 = NULL, *key3 = NULL;

    key1 = malloc(numBytes1 + 2);   
    key2 = malloc(numBytes2 + 2); 
    key3 = malloc(numBytes3 + 2); 
    CU_ASSERT_PTR_NOT_NULL_FATAL(key1);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(key2); 
    CU_ASSERT_PTR_NOT_NULL_FATAL(key3);

    /* create one hashtable first, so in case we are tracking memory, we ensure that 
     * the hashtable code module paths will already be cached. */
    myhashtab1 = new_hashtable(16);  
 
    cache_start_count = cache_count();
        
    result1 = wcscpy_s(key1, numElements1+1, wstr1);  /* wcscpy_s param 2 must be */
    result2 = wcscpy_s(key2, numElements2+1, wstr2);  /* number of characters + 1 */
    result3 = wcscpy_s(key3, numElements3+1, wstr3);  
    actlen1 = wcslen(key1);  actlen2 = wcslen(key2);  actlen3 = wcslen(key3);

    myhashtab2 = new_hashtable(16);    
    myhashtab3 = new_hashtable(16);   

    /* we should not have cached any more hashtable vtables */
    cache_current_count = cache_count();
    CU_ASSERT_EQUAL(cache_current_count,cache_start_count); 

    myhashtab1->vtab->insert(myhashtab1->realtable, key1, (int)numBytes1, NULL,0,0,0);            
    myhashtab2->vtab->insert(myhashtab2->realtable, key2, (int)numBytes2, NULL,0,0,0);
    myhashtab2->vtab->insert(myhashtab3->realtable, key3, (int)numBytes3, NULL,0,0,0);

    /* TODO instantiate some other object here which uses the cache */

    destroy_hashtable(myhashtab1, TRUE, TRUE);
    destroy_hashtable(myhashtab2, TRUE, TRUE);
    destroy_hashtable(myhashtab3, TRUE, TRUE);
    /* note that key1 and key2 are now dangling pointers since we asked the
     * hashtable to free keys and values memory
     */
    
   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
   memtable_clear();  /* start fresh for next test */   
}


/**
 * test_intkeys()
 * tests caching using integer keys as we might do for etchobjects such as vtables
 */
void test_intkeys(void)
{
    int i, startsize, size;
    const int STARTKEY = 0, ENDKEY = 512, KEYCOUNT = ENDKEY - STARTKEY;
    char* teststring = "it works!";
    char* item = etch_malloc(sizeof(teststring),0);
    memcpy(item, teststring, sizeof(teststring)); 

    startsize = cache_count();

    for(i = STARTKEY; i < ENDKEY; i++)
        cache_add(i, item);

    size = cache_count();
    CU_ASSERT_EQUAL(size, KEYCOUNT + startsize);

    for(i = STARTKEY; i < ENDKEY; i++)
        CU_ASSERT_PTR_NOT_NULL(cache_find(i, 0));

    for(i = STARTKEY; i < ENDKEY; i++)
        CU_ASSERT_PTR_NOT_NULL(cache_del(i));

    size = cache_count();
    CU_ASSERT_EQUAL(size, startsize);

    etch_free(item);

    g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */  
}


/**
 * test_intkeys()
 * tests caching using string keys with no values, as we might do for source  
 * file paths in the debug allocator  
 */
void test_pathkeys(void)
{
    char* path1 = "..\\..\\foo\\bar\\file1.dat";
    char* path2 = "..\\..\\foo\\bar\\file2.dat";
    char* path3 = "c:\\the\\quick\\brown\\fox\\jumped\\over\\the\\lazy\\dog\\file3.dat";
    unsigned hash1 = 0, hash2 = 0, hash3 = 0;
    char* namefound = NULL;
    etch_hashitem  hashbucket; 
    etch_hashitem* thisitem = &hashbucket;
    int result = 0; 
    int len1 = (int)strlen(path1), len2 = (int)strlen(path2), len3 = (int)strlen(path3);
 
    hash1 = cache_insertx (path1, NULL, FALSE);
    hash2 = cache_insertx (path2, NULL, FALSE);
    hash3 = cache_insertx (path3, NULL, FALSE);

    memset(thisitem, 0, sizeof(etch_hashitem));
    cache_findx(path1, &thisitem);
    CU_ASSERT_PTR_NOT_NULL(thisitem->key);
    CU_ASSERT_EQUAL(hash1, thisitem->hash);
    result = strncmp(path1, thisitem->key, len1);
    CU_ASSERT_EQUAL(result,0);

    memset(thisitem, 0, sizeof(etch_hashitem));
    cache_findx(path2, &thisitem);
    CU_ASSERT_PTR_NOT_NULL(thisitem->key);
    CU_ASSERT_EQUAL(hash2, thisitem->hash);
    result = strncmp(path2, thisitem->key, len2);
    CU_ASSERT_EQUAL(result,0);

    memset(thisitem, 0, sizeof(etch_hashitem));
    cache_findx(path3, &thisitem);
    CU_ASSERT_PTR_NOT_NULL(thisitem->key);
    CU_ASSERT_EQUAL(hash3, thisitem->hash);
    result = strncmp(path3, thisitem->key, len3);
    CU_ASSERT_EQUAL(result,0);

    memset(thisitem, 0, sizeof(etch_hashitem));
    cache_find_by_hash(hash1, &thisitem);
    CU_ASSERT_PTR_NOT_NULL(thisitem->key);
    result = strncmp(path1, thisitem->key, len1);
    CU_ASSERT_EQUAL(result,0);

    memset(thisitem, 0, sizeof(etch_hashitem));
    cache_find_by_hash(hash2, &thisitem);
    CU_ASSERT_PTR_NOT_NULL(thisitem->key);
    result = strncmp(path2, thisitem->key, len2);
    CU_ASSERT_EQUAL(result,0);

    memset(thisitem, 0, sizeof(etch_hashitem));
    cache_find_by_hash(hash3, &thisitem);
    CU_ASSERT_PTR_NOT_NULL(thisitem->key);
    result = strncmp(path3, thisitem->key, len3);
    CU_ASSERT_EQUAL(result,0);
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
    CU_set_output_filename("../test_cache");
    pSuite = CU_add_suite("suite_cache", init_suite, clean_suite);

    CU_add_test(pSuite, "test path strings as keys", test_pathkeys);
    CU_add_test(pSuite, "multiple of same object test", test_multiple_items); 
    CU_add_test(pSuite, "integer cache key test", test_intkeys);

    if (g_is_automated_test)    
        CU_automated_run_tests();    
    else
    {
        CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    }

    if (!g_is_automated_test) { printf("any key ..."); while(!c) c = _getch();  wprintf(L"\n"); }     
    CU_cleanup_registry();
    return CU_get_error(); 
}

