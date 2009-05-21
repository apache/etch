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
 * test_id_name.c 
 * tests the C implementation of the etch_id_name object.
 */
#include "apr_time.h" /* some apr must be included first */
#include "etchthread.h"
#include <stdio.h>
#include <conio.h>
#include "cunit.h"
#include "basic.h"
#include "automated.h"
#include "etch_id_name.h"
#include "etch_global.h"


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
 * test_id_name
 */
void test_id_name(void)
{
    int alloc_a = 0, alloc_b = 0, result = 0;
    etch_id_name *id_name1 = NULL, *id_name2 = NULL;

    const wchar_t* nametext1 = L"abracadabra";
    const size_t   numelts1  = wcslen(nametext1);
    const size_t   numbytes1 = sizeof(wchar_t) * numelts1;

    const wchar_t* nametext2 = L"gilgamesh";
    const size_t   numelts2  = wcslen(nametext2);
    const size_t   numbytes2 = sizeof(wchar_t) * numelts2;
    
    alloc_a  = etch_showmem(0, FALSE);
    id_name1 = new_id_name(NULL);
    alloc_b  = etch_showmem(0, FALSE);
    CU_ASSERT_PTR_NULL(id_name1);
    CU_ASSERT_EQUAL(alloc_a, alloc_b); 

    id_name1 = new_id_name(nametext1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(id_name1);
    CU_ASSERT_TRUE(is_good_id_name(id_name1));

    id_name2 = new_id_name(nametext1);
    CU_ASSERT_TRUE(is_equal_id_names(id_name1, id_name2));
    result = memcmp(id_name1->name, id_name2->name, id_name1->namebytelen);
    CU_ASSERT_EQUAL(result,0);

    destroy_id_name(id_name1);
    destroy_id_name(id_name2);

    alloc_a = etch_showmem(0, FALSE);
    CU_ASSERT_EQUAL(alloc_a, alloc_b); 

    id_name1 = new_id_name(nametext1);
    id_name2 = new_id_name(nametext2);

    CU_ASSERT_FALSE(is_equal_id_names(id_name1, id_name2));

    destroy_id_name(id_name1);
    destroy_id_name(id_name2);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_id_name_hashfunc
 * Unit test id_name_hashfunc
 */
void test_id_name_hashfunc(void)
{
    int alloc_a = 0, alloc_b = 0, result = 0;
    unsigned hash1 = 0, hash2 = 0;
    etch_id_name *id_name1 = NULL, *id_name2 = NULL;

    const wchar_t* nametext1 = L"abracadabra";
    const size_t   numelts1  = wcslen(nametext1);
    const size_t   numbytes1 = sizeof(wchar_t) * ( numelts1 + 1 );

    const wchar_t* nametext2 = L"gilgamesh";
    const size_t   numelts2  = wcslen(nametext2);
    const size_t   numbytes2 = sizeof(wchar_t) * ( numelts2 + 1 );
 
    hash1 = compute_id_name_id_from_widename(nametext1);
    hash2 = compute_id_name_id_from_widename(nametext2);

    CU_ASSERT_NOT_EQUAL(hash1, hash2);

    id_name1 = new_id_name(nametext1);
    id_name2 = new_id_name(nametext2);
    CU_ASSERT_EQUAL(id_name1->namebytelen, numbytes1);
    CU_ASSERT_EQUAL(id_name2->namebytelen, numbytes2);
    CU_ASSERT_EQUAL(id_name1->id, hash1);
    CU_ASSERT_EQUAL(id_name2->id, hash2);

    destroy_id_name(id_name1);
    destroy_id_name(id_name2);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
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
    pSuite = CU_add_suite("suite_id_name", init_suite, clean_suite);
    CU_set_output_filename("../test_id_name");

    CU_add_test(pSuite, "test etch_id_name constructors/destructors", test_id_name);  
    CU_add_test(pSuite, "test etch_id_name hashing", test_id_name_hashfunc);  

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