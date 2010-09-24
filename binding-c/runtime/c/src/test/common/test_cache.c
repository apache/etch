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
#include "etch_runtime.h"
#include "etch_cache.h"
#include "etch_arraylist.h"
#include "etch_hash.h"
#include "etch_mem.h"

#include <stdio.h>
#include "CUnit.h"


#define IS_DEBUG_CONSOLE FALSE


/* - - - - - - - - - - - - - - 
 * unit test infrastructure
 * - - - - - - - - - - - - - -
 */


static int init_suite(void)
{
    etch_status_t etch_status = ETCH_SUCCESS;

    return etch_runtime_initialize(NULL);
}

static int clean_suite(void)
{
    etch_runtime_shutdown();
    return 0;
}


static int test_cache_freehook_func(void* key, void* value)
{
    //etch_free(key);
    //if(value) {
    //    etch_free(value);
    //}
    return 0;
}

/**
 * This subtest instantiates various etch objects which cache some part of 
 * themselves, and destroys the objects. At each step the test verifies that
 * the cache contains the expected number of entries, e.g. if I create 
 * multiple hashtables I should only have cached one hashtable vtable.
 */
static void test_multiple_items(void)
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

    // set freehook for cache
    etch_cache_set_freehook(test_cache_freehook_func);

    key1 = etch_malloc(numBytes1 + sizeof(wchar_t), 0);
    key2 = etch_malloc(numBytes2 + sizeof(wchar_t), 0);
    key3 = etch_malloc(numBytes3 + sizeof(wchar_t), 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key2);
    CU_ASSERT_PTR_NOT_NULL_FATAL(key3);

    /* create one hashtable first, so in case we are tracking memory, we ensure that 
     * the hashtable code module paths will already be cached. */
    myhashtab1 = new_hashtable(16);
 
    cache_start_count = etch_cache_count();
        
#if defined(WIN32) && !defined(_WIN32_WCE)
	result1 = wcscpy_s(key1, numElements1+1, wstr1);  /* wcscpy_s param 2 must be */
    result2 = wcscpy_s(key2, numElements2+1, wstr2);  /* number of characters + 1 */
    result3 = wcscpy_s(key3, numElements3+1, wstr3);  
#elif defined(_WIN32_WCE)
	wcsncpy(key1, wstr1, numElements1);
	wcsncpy(key2, wstr2, numElements2);
	wcsncpy(key3, wstr3, numElements3);
#else
    result1 = wcscpy(key1, wstr1);
    result2 = wcscpy(key2, wstr2);
    result3 = wcscpy(key3, wstr3);  
#endif
    actlen1 = wcslen(key1);
    actlen2 = wcslen(key2);
    actlen3 = wcslen(key3);

    myhashtab2 = new_hashtable(16);
    myhashtab3 = new_hashtable(16);

    /* we should not have cached any more hashtable vtables */
    cache_current_count = etch_cache_count();
    CU_ASSERT_EQUAL(cache_current_count, cache_start_count);

    ((struct i_hashtable*)((etch_object*)myhashtab1)->vtab)->insert(myhashtab1->realtable, key1, (int)numBytes1, NULL,0,0,0);
    ((struct i_hashtable*)((etch_object*)myhashtab2)->vtab)->insert(myhashtab2->realtable, key2, (int)numBytes2, NULL,0,0,0);
    ((struct i_hashtable*)((etch_object*)myhashtab2)->vtab)->insert(myhashtab3->realtable, key3, (int)numBytes3, NULL,0,0,0);

    /* TODO instantiate some other object here which uses the cache */

    destroy_hashtable(myhashtab1, TRUE, TRUE);
    destroy_hashtable(myhashtab2, TRUE, TRUE);
    destroy_hashtable(myhashtab3, TRUE, TRUE);
    /* note that key1 and key2 are now dangling pointers since we asked the
     * hashtable to free keys and values memory
     */
    etch_free(key1);
    etch_free(key2);
    etch_free(key3);

}


/**
 * test_intkeys()
 * tests caching using integer keys as we might do for etchobjects such as vtables
 */
static void test_intkeys(void)
{
    int i, startsize, size;
    const int STARTKEY = 0, ENDKEY = 512, KEYCOUNT = ENDKEY - STARTKEY;
    char* teststring = "it works!";
    char* item = etch_malloc(sizeof(teststring),0);
    memcpy(item, teststring, sizeof(teststring)); 

    // set freehook for cache
    etch_cache_set_freehook(test_cache_freehook_func);

    startsize = etch_cache_count();

    for(i = STARTKEY; i < ENDKEY; i++)
        etch_cache_add(i, item);

    size = etch_cache_count();
    CU_ASSERT_EQUAL(size, KEYCOUNT + startsize);

    for(i = STARTKEY; i < ENDKEY; i++)
        CU_ASSERT_PTR_NOT_NULL(etch_cache_find(i, 0));

    for(i = STARTKEY; i < ENDKEY; i++)
        CU_ASSERT_PTR_NOT_NULL(etch_cache_del(i));

    size = etch_cache_count();
    CU_ASSERT_EQUAL(size, startsize);

    etch_free(item);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_intkeys()
 * tests caching using string keys with no values, as we might do for source  
 * file paths in the debug allocator  
 */
static void test_pathkeys(void)
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
 
    // set freehook for cache
    etch_cache_set_freehook(test_cache_freehook_func);

    hash1 = etch_cache_insertx (path1, NULL, FALSE);
    hash2 = etch_cache_insertx (path2, NULL, FALSE);
    hash3 = etch_cache_insertx (path3, NULL, FALSE);

    memset(thisitem, 0, sizeof(etch_hashitem));
    etch_cache_findx(path1, &thisitem);
    CU_ASSERT_PTR_NOT_NULL(thisitem->key);
    CU_ASSERT_EQUAL(hash1, thisitem->hash);
    result = strncmp(path1, thisitem->key, len1);
    CU_ASSERT_EQUAL(result,0);

    memset(thisitem, 0, sizeof(etch_hashitem));
    etch_cache_findx(path2, &thisitem);
    CU_ASSERT_PTR_NOT_NULL(thisitem->key);
    CU_ASSERT_EQUAL(hash2, thisitem->hash);
    result = strncmp(path2, thisitem->key, len2);
    CU_ASSERT_EQUAL(result,0);

    memset(thisitem, 0, sizeof(etch_hashitem));
    etch_cache_findx(path3, &thisitem);
    CU_ASSERT_PTR_NOT_NULL(thisitem->key);
    CU_ASSERT_EQUAL(hash3, thisitem->hash);
    result = strncmp(path3, thisitem->key, len3);
    CU_ASSERT_EQUAL(result,0);

    memset(thisitem, 0, sizeof(etch_hashitem));
    etch_cache_find_by_hash(hash1, &thisitem);
    CU_ASSERT_PTR_NOT_NULL(thisitem->key);
    result = strncmp(path1, thisitem->key, len1);
    CU_ASSERT_EQUAL(result,0);

    memset(thisitem, 0, sizeof(etch_hashitem));
    etch_cache_find_by_hash(hash2, &thisitem);
    CU_ASSERT_PTR_NOT_NULL(thisitem->key);
    result = strncmp(path2, thisitem->key, len2);
    CU_ASSERT_EQUAL(result,0);

    memset(thisitem, 0, sizeof(etch_hashitem));
    etch_cache_find_by_hash(hash3, &thisitem);
    CU_ASSERT_PTR_NOT_NULL(thisitem->key);
    result = strncmp(path3, thisitem->key, len3);
    CU_ASSERT_EQUAL(result,0);
}

CU_pSuite test_cache_suite()
{
    CU_pSuite pSuite = CU_add_suite("etch_cache", init_suite, clean_suite);

    CU_add_test(pSuite, "test path strings as keys", test_pathkeys);
    CU_add_test(pSuite, "multiple of same object test", test_multiple_items); 
    CU_add_test(pSuite, "integer cache key test", test_intkeys);
    return pSuite;
}
