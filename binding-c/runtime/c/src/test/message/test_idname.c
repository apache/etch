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
#include "etch_runtime.h"
#include "etch_id_name.h"

#include <stdio.h>
#include "CUnit.h"
#include <wchar.h>

#define IS_DEBUG_CONSOLE FALSE

// extern types
extern apr_pool_t* g_etch_main_pool;


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


/**
 * test_id_name
 */
static void test_id_name(void)
{
    int result = 0;
    etch_id_name *id_name1 = NULL, *id_name2 = NULL;

    const wchar_t* nametext1 = L"abracadabra";

    const wchar_t* nametext2 = L"gilgamesh";

#ifdef ETCH_DEBUGALLOC
    alloc_a  = etch_showmem(0, FALSE);
#endif
    id_name1 = new_id_name(NULL);
    CU_ASSERT_PTR_NULL(id_name1);

#ifdef ETCH_DEBUGALLOC
    alloc_b  = etch_showmem(0, FALSE);
    CU_ASSERT_EQUAL(alloc_a, alloc_b); 
#endif

    id_name1 = new_id_name(nametext1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(id_name1);
    CU_ASSERT_TRUE(is_good_id_name(id_name1));

    id_name2 = new_id_name(nametext1);
    CU_ASSERT_TRUE(is_equal_id_names(id_name1, id_name2));
    result = memcmp(id_name1->name, id_name2->name, id_name1->namebytelen);
    CU_ASSERT_EQUAL(result,0);

    destroy_id_name(id_name1);
    destroy_id_name(id_name2);

#ifdef ETCH_DEBUGALLOC
    alloc_a = etch_showmem(0, FALSE);
    CU_ASSERT_EQUAL(alloc_a, alloc_b); 
#endif

    id_name1 = new_id_name(nametext1);
    id_name2 = new_id_name(nametext2);

    CU_ASSERT_FALSE(is_equal_id_names(id_name1, id_name2));

    destroy_id_name(id_name1);
    destroy_id_name(id_name2);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_id_name_hashfunc
 * Unit test id_name_hashfunc
 */
static void test_id_name_hashfunc(void)
{
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
CU_pSuite test_etch_idname_suite()
{    
    CU_pSuite pSuite = CU_add_suite("suite_id_name", init_suite, clean_suite);

    CU_add_test(pSuite, "test etch_id_name constructors/destructors", test_id_name);  
    CU_add_test(pSuite, "test etch_id_name hashing", test_id_name_hashfunc);  

    return pSuite;
}
