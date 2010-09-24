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
 * test_field.c 
 * tests the C implementation of the etch_field object.
 */
#include "etch_runtime.h"
#include "etch_field.h"

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


/**
 * test_field
 */
static void test_field(void)
{
    int alloc_a = 0, alloc_b = 0, result = 0;
    etch_field *field1 = NULL, *field2 = NULL;

    const wchar_t* nametext1 = L"abracadabra";

    const wchar_t* nametext2 = L"gilgamesh";

#ifdef ETCH_DEBUGALLOC
    alloc_a  = etch_showmem(0, FALSE);
#endif
    field1 = new_field(NULL);
    
#ifdef ETCH_DEBUGALLOC
   alloc_b  = etch_showmem(0, FALSE);
#endif

    CU_ASSERT_PTR_NULL(field1);
    CU_ASSERT_EQUAL(alloc_a, alloc_b); 

    field1 = new_field(nametext1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(field1);
    CU_ASSERT_TRUE(is_good_field(field1));

    field2 = new_field(nametext1);
    CU_ASSERT_TRUE(is_equal_fields(field1, field2));
    result = memcmp(field1->name, field2->name, field1->namebytelen);
    CU_ASSERT_EQUAL(result,0);

    destroy_field(field1);
    destroy_field(field2);

#ifdef ETCH_DEBUGALLOC
    alloc_a = etch_showmem(0, FALSE);
    CU_ASSERT_EQUAL(alloc_a, alloc_b); 
#endif

    field1 = new_field(nametext1);
    field2 = new_field(nametext2);

    CU_ASSERT_FALSE(is_equal_fields(field1, field2));

    destroy_field(field1);
    destroy_field(field2);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_inheritance
 * test that etch_field's inheritance list is as expected
 */
static void test_inheritance(void)
{
    int  ndx = 0, parentcount = 0;
    etchparentinfo* parentinfo = NULL;
    const wchar_t* nametext1 = L"abracadabra";
    
    etch_field *field1 = new_field(nametext1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(field1);

    while(1)
    { parentinfo = get_next_etch_parent((etch_object*)field1, ndx++);
      if (NULL == parentinfo) break;
      parentcount++; 
      #if IS_DEBUG_CONSOLE 
      printf("parent %d type %d class %d\n", parentcount, ((etch_object*)parentinfo)->obj_type, ((etch_object*)parentinfo)->class_id);
      #endif
    }  

    CU_ASSERT_EQUAL(parentcount, 2); /* field derives from both id_name and object */

    destroy_field(field1);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_field_hashfunc
 * Unit test field_hashfunc
 */
static void test_field_hashfunc(void)
{
    unsigned hash1 = 0, hash2 = 0;
    etch_field *field1 = NULL, *field2 = NULL;

    const wchar_t* nametext1 = L"abracadabra";
    const size_t   numelts1  = wcslen(nametext1);
    const size_t   numbytes1 = sizeof(wchar_t) * ( numelts1 + 1 );

    const wchar_t* nametext2 = L"gilgamesh";
    const size_t   numelts2  = wcslen(nametext2);
    const size_t   numbytes2 = sizeof(wchar_t) * ( numelts2 + 1 );
 
    hash1 = compute_field_id_from_widename(nametext1);
    hash2 = compute_field_id_from_widename(nametext2);

    CU_ASSERT_NOT_EQUAL(hash1, hash2);

    field1 = new_field(nametext1);
    field2 = new_field(nametext2);
    CU_ASSERT_EQUAL(field1->namebytelen, numbytes1);
    CU_ASSERT_EQUAL(field2->namebytelen, numbytes2);
    CU_ASSERT_EQUAL(field1->id, hash1);
    CU_ASSERT_EQUAL(field2->id, hash2);

    destroy_field(field1);
    destroy_field(field2);

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
CU_pSuite test_etch_field_suite()
//int main(int argc, char** argv[])
{    
    CU_pSuite pSuite = CU_add_suite("etch_field tests", init_suite, clean_suite);

    CU_add_test(pSuite, "test etch_field constructors/destructors", test_field);  
    CU_add_test(pSuite, "test etch_field hashing", test_field_hashfunc); 
    CU_add_test(pSuite, "test etch_field inheritance", test_inheritance);   

    return pSuite;
}
