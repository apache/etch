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
#include "etch_runtime.h"
#include "etch_arraylist.h"
#include "etch_hash.h"
#include "etch_mem.h"

#include <stdio.h>
#include "CUnit.h"
#include <wchar.h>

#define IS_DEBUG_CONSOLE FALSE

// extern types
extern apr_pool_t* g_etch_main_pool;

/* - - - - - - - - - - - - - - 
 * unit test infrastructure
 * - - - - - - - - - - - - - -
 */

static int init_suite(void)
{
    etch_status_t etch_status = ETCH_SUCCESS;

    etch_status = etch_runtime_initialize(NULL);
    if(etch_status != NULL) {
        // error
    }
    return 0;
}

static int clean_suite(void)
{
    //this_teardown();
    etch_runtime_shutdown();
    return 0;
}

static etch_arraylist* testlist;
static etch_hashtable* testhash;

/* 
 * load_listdata_int()
 * load testlist array with some etch_int32 objects
 */
static int load_listdata_int()
{
    int i = 0, numitems = 4;
    etch_int32* newobj = NULL;
    int ints[4] = { 1, 2, 3, 4 };

    for(; i < numitems; i++)
    {
        newobj = new_int32(ints[i]);
        etch_arraylist_add(testlist, newobj);
    }

    return numitems;
}


/* 
 * new_listdata()
 * create testlist array and load it up with data objects
 */
static int new_listdata(const int datatype)
{
    int count = 0;
    testlist = new_etch_arraylist(0,0);  
    testlist->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
    count = load_listdata_int(); 
    return count;
}


/* 
 * destroy_listdata()
 * destroy testlist array and content
 */
static void destroy_listdata()
{
    etch_arraylist_destroy(testlist, TRUE);
}


/* 
 * load_hashdata_string()
 * load testhash hashtable with some etch_string objects
 */
static int load_hashdata_string()
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
        newobj = new_stringw(strings[i]); 

        key  = etch_malloc(bytelen, 0);
        memcpy(key, strings[i], bytelen);

        result = ((struct i_hashtable*)((etch_object*)testhash)->vtab)->insert(testhash->realtable, 
                 key, (int)bytelen, newobj, 0, NULL, NULL);
    }

    return numitems;
}


/* 
 * new_hashdata()
 * create testhash hashtable and load it up with data objects
 */
static int new_hashdata(const int datatype)
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
static void destroy_hashdata()
{
    destroy_hashtable(testhash, TRUE, TRUE);
}


/* 
 * test_iterator_over_arraylist
 */
static void test_iterator_over_arraylist(void)
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

    etch_object_destroy(iterator);
    destroy_listdata();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test_iterator_over_hashtable
 */
static void test_iterator_over_hashtable(void)
{
    etch_iterator* iterator = NULL; 
    int result = 0, thiscount = 0, testcount = 0;
    testcount = new_hashdata(1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(testhash);
    CU_ASSERT_NOT_EQUAL(((struct i_hashtable*)((etch_object*)testhash)->vtab)->count(testhash->realtable,0,0),0);
#ifdef ETCH_DEBUGALLOC
    startbytes = etch_showmem(0, FALSE); /* note testdata bytes */
#endif

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

    etch_object_destroy(iterator);
    destroy_hashdata();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   g_bytes_allocated -= startbytes;
   CU_ASSERT_TRUE(g_bytes_allocated <= 0);  
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test_stack_allocated_iterator
 * iterator as automatic variable
 */
static void test_stack_allocated_iterator(void)
{
    etch_iterator iterator;
    struct i_iterable* vtab = NULL;
    int testcount = 0, thiscount = 0;
    testcount = new_listdata(0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(testlist);
    CU_ASSERT_NOT_EQUAL(testlist->count, 0);
    

    set_iterator(&iterator, testlist, &testlist->iterable);

    CU_ASSERT_EQUAL_FATAL(iterator.ordinal,1);
    vtab = (struct i_iterable*)iterator.object.vtab;
    while(vtab->has_next(&iterator))
          thiscount += (vtab->next(&iterator) == 0);  
        
    CU_ASSERT_EQUAL(thiscount, testcount-1);

    destroy_listdata();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test_reset_iterator
 * test reinitalizing and reusing iterator
 */
static void test_reset_iterator(void)
{
    etch_iterator iterator;
    struct i_iterable* vtab = NULL;
    int testcount = 0, thiscount1 = 0, thiscount2 = 0;

    testcount = new_listdata(0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(testlist);
    CU_ASSERT_NOT_EQUAL(testlist->count, 0);

    set_iterator(&iterator, testlist, &testlist->iterable);

    CU_ASSERT_EQUAL(iterator.ordinal,1);
    vtab = (struct i_iterable*)iterator.object.vtab;
    while(vtab->has_next(&iterator))
          thiscount1 += (vtab->next(&iterator) == 0);  
        
    CU_ASSERT_EQUAL(thiscount1, testcount-1);

    set_iterator(&iterator, testlist, &testlist->iterable);
    vtab = (struct i_iterable*)iterator.object.vtab;
    CU_ASSERT_EQUAL(iterator.ordinal,1);

    while(vtab->has_next(&iterator))
          thiscount2 += (vtab->next(&iterator) == 0); 
 
    CU_ASSERT_EQUAL(thiscount2, thiscount1);    

    destroy_listdata();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * main   
 */
//int wmain( int argc, wchar_t* argv[], wchar_t* envp[])
CU_pSuite test_etch_iterator_suite()
{    
    CU_pSuite pSuite = CU_add_suite("suite_iterator", init_suite, clean_suite);

    CU_add_test(pSuite, "test forward iterator over arraylist",   test_iterator_over_arraylist);
    CU_add_test(pSuite, "test forward iterator over hashtable",   test_iterator_over_hashtable);
    CU_add_test(pSuite, "test stack allocated iterator",   test_stack_allocated_iterator);
    CU_add_test(pSuite, "test reset iterator",   test_reset_iterator);

    return pSuite;
}
