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
 * test_iterator.c -- test etch_iterator over i_iterable classes
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
#include "etchhash.h"


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


etch_arraylist* testlist;
etch_hashtable* testhash;

/* 
 * load_listdata_int()
 * load testlist array with some etch_int32 objects
 */
int load_listdata_int()
{
    int i = 0, numitems = 4;
    etch_int32* newobj = NULL;
    int ints[4] = { 1, 2, 3, 4 };

    for(; i < numitems; i++)
    {
        newobj = new_int32(ints[i]);
        arraylist_add(testlist, newobj);
    }

    return numitems;
}


/* 
 * new_listdata()
 * create testlist array and load it up with data objects
 */
int new_listdata(const int datatype)
{
    int count = 0;
    testlist = new_arraylist(0,0);  
    testlist->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
    count = load_listdata_int(); 
    return count;
}


/* 
 * destroy_listdata()
 * destroy testlist array and content
 */
void destroy_listdata()
{
    arraylist_destroy(testlist, TRUE);
}


/* 
 * load_hashdata_string()
 * load testhash hashtable with some etch_string objects
 */
int load_hashdata_string()
{
    int i = 0, numitems = 4, result = 0;
    //wchar_t* testval = NULL;
    void* key = NULL;
    etch_string* newobj = NULL;
    wchar_t* str0 = L"now ", *str1 = L"is  ", *str2 = L"the ", *str3 = L"time";
    wchar_t* strings[4] = { str0, str1, str2, str3 };
    const size_t bytelen = (wcslen(str0) + 1) * sizeof(wchar_t);

    for(; i < numitems; i++)
    {
        //testval = etch_malloc(bytelen, 0);
        //memcpy(testval, strings[i], bytelen);
        newobj = new_string(strings[i], ETCH_ENCODING_UTF16); 

        key  = etch_malloc(bytelen, 0);
        memcpy(key, strings[i], bytelen);

        result = testhash->vtab->insert(testhash->realtable, 
                 key, (int)bytelen, newobj, 0, NULL, NULL);
    }

    return numitems;
}


/* 
 * new_hashdata()
 * create testhash hashtable and load it up with data objects
 */
int new_hashdata(const int datatype)
{
    int count = 0;
    testhash = new_hashtable(16);  
    count = load_hashdata_string();
    return count;
}


/* 
 * destroy_hashdata()
 * destroy testhash hashtable and content
 */
void destroy_hashdata()
{
    destroy_hashtable(testhash, TRUE, TRUE);
}


/* 
 * test_iterator_over_arraylist
 */
void test_iterator_over_arraylist(void)
{
    etch_iterator* iterator = NULL; 
    int testcount = 0, thiscount = 0;
    testcount = new_listdata(0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(testlist);
    CU_ASSERT_NOT_EQUAL(testlist->count, 0);

    iterator = new_iterator(testlist, &testlist->iterable);

    CU_ASSERT_PTR_NOT_NULL_FATAL(iterator);
    CU_ASSERT_EQUAL_FATAL(iterator->ordinal,1);

    while(iterator->has_next(iterator))
          thiscount += (iterator->next(iterator) == 0);  
        
    CU_ASSERT_EQUAL(thiscount, testcount-1);

    iterator->destroy(iterator);
    destroy_listdata();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */ 
}


/* 
 * test_iterator_over_hashtable
 */
void test_iterator_over_hashtable(void)
{
    etch_iterator* iterator = NULL; 
    int result = 0, thiscount = 0, testcount = 0, startbytes = 0;
    testcount = new_hashdata(1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(testhash);
    CU_ASSERT_NOT_EQUAL(testhash->vtab->count(testhash->realtable,0,0),0);
    startbytes = etch_showmem(0, FALSE); /* note testdata bytes */

    iterator = new_iterator(testhash, &testhash->iterable);

    CU_ASSERT_PTR_NOT_NULL_FATAL(iterator);
    CU_ASSERT_PTR_NOT_NULL_FATAL(iterator->current_key);
    CU_ASSERT_EQUAL_FATAL(iterator->ordinal,1);
    thiscount = 1;

    while(iterator->has_next(iterator))
    {
        if (0 != (result = iterator->next(iterator))) break;  
         
        thiscount++;
        CU_ASSERT_PTR_NOT_NULL(iterator->current_key);
        CU_ASSERT_PTR_NOT_NULL(iterator->current_value);

        #if IS_DEBUG_CONSOLE       
        if  (iterator->current_value)
             wprintf(L"value is %s\n",(wchar_t*)iterator->current_value);
        else wprintf(L"value is null\n");           
        #endif       
    } 
        
    CU_ASSERT_EQUAL(testcount, thiscount);

    iterator->destroy(iterator);
    destroy_hashdata();

    /* we are etch_malloc'ing testdata but hashtable frees content, not etch_free */
    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    g_bytes_allocated -= startbytes;
    CU_ASSERT_TRUE(g_bytes_allocated <= 0);  
    memtable_clear();  /* start fresh for next test */ 
}


/* 
 * test_stack_allocated_iterator
 * iterator as automatic variable
 */
void test_stack_allocated_iterator(void)
{
    etch_iterator iterator;
    int testcount = 0, thiscount = 0;
    testcount = new_listdata(0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(testlist);
    CU_ASSERT_NOT_EQUAL(testlist->count, 0);

    set_iterator(&iterator, testlist, &testlist->iterable);

    CU_ASSERT_EQUAL_FATAL(iterator.ordinal,1);

    while(iterator.vtab->has_next(&iterator))
          thiscount += (iterator.vtab->next(&iterator) == 0);  
        
    CU_ASSERT_EQUAL(thiscount, testcount-1);

    destroy_listdata();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */ 
}


/* 
 * test_reset_iterator
 * test reinitalizing and reusing iterator
 */
void test_reset_iterator(void)
{
    etch_iterator iterator;
    int testcount = 0, thiscount1 = 0, thiscount2 = 0;
    testcount = new_listdata(0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(testlist);
    CU_ASSERT_NOT_EQUAL(testlist->count, 0);

    set_iterator(&iterator, testlist, &testlist->iterable);

    CU_ASSERT_EQUAL(iterator.ordinal,1);

    while(iterator.vtab->has_next(&iterator))
          thiscount1 += (iterator.vtab->next(&iterator) == 0);  
        
    CU_ASSERT_EQUAL(thiscount1, testcount-1);

    set_iterator(&iterator, testlist, &testlist->iterable);

    CU_ASSERT_EQUAL(iterator.ordinal,1);

    while(iterator.vtab->has_next(&iterator))
          thiscount2 += (iterator.vtab->next(&iterator) == 0); 
 
    CU_ASSERT_EQUAL(thiscount2, thiscount1);    

    destroy_listdata();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
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
    if (CUE_SUCCESS != CU_initialize_registry()) return 0;
    pSuite = CU_add_suite("suite_iterator", init_suite, clean_suite);
    CU_set_output_filename("../test_iterator");  

    CU_add_test(pSuite, "test forward iterator over arraylist",   test_iterator_over_arraylist);
    CU_add_test(pSuite, "test forward iterator over hashtable",   test_iterator_over_hashtable);
    CU_add_test(pSuite, "test stack allocated iterator",   test_stack_allocated_iterator);
    CU_add_test(pSuite, "test reset iterator",   test_reset_iterator);

    if (g_is_automated_test)    
        CU_automated_run_tests();    
    else
    {   CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    }

    if (!g_is_automated_test) { printf("any key ..."); while(!c) c = _getch(); wprintf(L"\n"); }     
    CU_cleanup_registry();
    return CU_get_error(); 
}

