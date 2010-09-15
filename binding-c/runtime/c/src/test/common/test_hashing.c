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
 * test_hashing.c 
 * test hashing of various complex items and associated memory management 
 */

#include "etch_runtime.h"
#include "etch_binary_tdi.h"  /* must be included second */
#include "etch_binary_tdo.h"
#include "etch_connection.h"
#include "etch_id_name.h"
#include "etch_field.h"
#include "etch_type.h"
#include "etch_log.h"
#include "etch_mem.h"

#include <stdio.h>
#include "CUnit.h"

#define IS_DEBUG_CONSOLE FALSE

// extern types
extern apr_pool_t* g_etch_main_pool;

// intern types
static int g_is_automated_test;

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

/* - - - - - - - - - - - - - - - - - - - - - - 
 * individual setup and teardown
 * - - - - - - - - - - - - - - - - - - - - - -
 */

static int this_test_setup()
{
    int result = -1;

    do {
        result = 0;

    } while(0);

    return result;
}

static int this_test_teardown()
{    

    return 0;
}

/**
 * destroy_content_id_name()
 * passed a content pointer out of the hashtable, interprets pointer as etch_id_name,
 * frees the name string, and frees the etch_id_name shell.
 */
static int destroy_content_id_name(void* content)
{
    etch_id_name*  idname = (etch_id_name*) content;
    return idname? etch_object_destroy(idname): -1;
    return 0;
}


/**
 * destroy_content_field()
 */
static int destroy_content_field(void* content)
{
    etch_field* field = (etch_field*) content;
    return field? etch_object_destroy(field): -1;
}


/**
 * destroy_content_type()
 */
static int destroy_content_type(void* content)
{
    etch_type* type = (etch_type*) content;
    return type? etch_object_destroy(type): -1;
}


/**
 * test_idname_as_key
 * tests that we can use etch_id_name (and etch_field and etch_type, since they
 * are etch_id_name typedefs), as hashkeys, and subsequently acccess the original
 * objects, individually clean up the keys, and ask hashtable to clean up the values.
 */
static void test_idname_as_key_hashclean_values(void)
{
    etch_field*   my_field  = NULL;
    etch_type*    my_type   = NULL;
    etch_id_name* my_idname = NULL;

    const int id_name_size = sizeof(etch_id_name);
    const int field_size   = sizeof(etch_field);
    const int type_size    = sizeof(etch_type);

    etch_hashtable* myhashtab = NULL;  
    etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket; 

    wchar_t* name1 = L"name number one";  
    wchar_t* name2 = L"nombre numero dos";
    wchar_t* name3 = L"le troisieme nom";

    const size_t numElements1 = wcslen(name1);
    const size_t numElements2 = wcslen(name2);
    const size_t numElements3 = wcslen(name3);

    const size_t numBytes1 = sizeof(wchar_t) * numElements1;
    const size_t numBytes2 = sizeof(wchar_t) * numElements2;
    const size_t numBytes3 = sizeof(wchar_t) * numElements3;

    void* value1 = NULL, *value2 = NULL, *value3 = NULL;

    int result = 0, result1 = 0, result2 = 0, result3 = 0;
    unsigned bytes_allocated = 0;

    value1 = etch_malloc(numBytes1 + sizeof(wchar_t), 0);
    value2 = etch_malloc(numBytes2 + sizeof(wchar_t), 0);
    value3 = etch_malloc(numBytes3 + sizeof(wchar_t), 0);

    wcscpy(value1, name1);
    wcscpy(value2, name2);
    wcscpy(value3, name3);

    myhashtab = new_hashtable(16);  
    myhashtab->is_readonly_keys   = TRUE;  /* keys will NOT be freed on destroy */
    myhashtab->is_readonly_values = FALSE; /* values WILL be freed on destroy */
    myhashtab->is_tracked_memory  = TRUE;  /* hashtable will use etch_free */
   
    my_idname = new_id_name(name1);
    my_field  = new_field(name2);
    my_type   = new_type(name3);  

    CU_ASSERT_PTR_NOT_NULL_FATAL(my_idname);
    CU_ASSERT_PTR_NOT_NULL_FATAL(my_field);
    CU_ASSERT_PTR_NOT_NULL_FATAL(my_type);

    /* ensure constructors populated the hash key */
    CU_ASSERT_NOT_EQUAL_FATAL(((etch_object*)my_idname)->get_hashkey(my_idname),0);
    CU_ASSERT_NOT_EQUAL_FATAL(((etch_object*)my_field)->get_hashkey(my_field),0);
    CU_ASSERT_NOT_EQUAL_FATAL(((etch_object*)my_type)->get_hashkey(my_type),0);

    /* ensure constructors populated the wire id */
    CU_ASSERT_NOT_EQUAL_FATAL(my_idname->id,0);
    CU_ASSERT_NOT_EQUAL_FATAL(my_field->id,0);
    CU_ASSERT_NOT_EQUAL_FATAL(my_type->id,0);

    result = ((struct i_hashtable*)((etch_object*)myhashtab)->vtab)->inserth(myhashtab->realtable, my_idname, value1, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = ((struct i_hashtable*)((etch_object*)myhashtab)->vtab)->inserth(myhashtab->realtable, my_field, value2, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = ((struct i_hashtable*)((etch_object*)myhashtab)->vtab)->inserth(myhashtab->realtable, my_type, value3, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = ((struct i_hashtable*)((etch_object*)myhashtab)->vtab)->findh(myhashtab->realtable, ((etch_object*)my_idname)->get_hashkey(my_idname), myhashtab, &myentry);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_PTR_NOT_NULL(myentry->key);
    CU_ASSERT_PTR_NOT_NULL(myentry->value);
    result = destroy_content_id_name(myentry->key);
    CU_ASSERT_EQUAL(result,0);

    result = ((struct i_hashtable*)((etch_object*)myhashtab)->vtab)->findh(myhashtab->realtable, ((etch_object*)my_field)->hashkey, myhashtab, &myentry);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_PTR_NOT_NULL(myentry->key);
    CU_ASSERT_PTR_NOT_NULL(myentry->value);
    result = destroy_content_field(myentry->key);
    CU_ASSERT_EQUAL(result,0);

    result = ((struct i_hashtable*)((etch_object*)myhashtab)->vtab)->findh(myhashtab->realtable, ((etch_object*)my_type)->hashkey, myhashtab, &myentry);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_PTR_NOT_NULL(myentry->key);
    CU_ASSERT_PTR_NOT_NULL(myentry->value);
    result = destroy_content_type(myentry->key);
    CU_ASSERT_EQUAL(result,0);

    /** above we looked up each item and freed memory for the id_name, field, and type keys,
     * but not for their values. when we created the map we specified is_readonly_keys, so the
     * map destructor will free memory for the value content, but not for the key content. */
    etch_object_destroy(myhashtab);

}


/**
 * test_idname_as_key
 * tests that we can use etch_id_name (and etch_field and etch_type, since they
 * are etch_id_name typedefs), as hashkeys, and subsequently acccess the original
 * objects and clean up both the keys and values.
 */
static void test_idname_as_key_hashclean_none(void)
{
    etch_field*   my_field  = NULL;
    etch_type*    my_type   = NULL;
    etch_id_name* my_idname = NULL;

    const int id_name_size = sizeof(etch_id_name);
    const int field_size   = sizeof(etch_field);
    const int type_size    = sizeof(etch_type);

    etch_hashtable* myhashtab = NULL;  
    etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket; 

    wchar_t* name1 = L"name number one";  
    wchar_t* name2 = L"nombre numero dos";
    wchar_t* name3 = L"le troisieme nom";

    const size_t numElements1 = wcslen(name1);
    const size_t numElements2 = wcslen(name2);
    const size_t numElements3 = wcslen(name3);

    const size_t numBytes1 = sizeof(wchar_t) * numElements1;
    const size_t numBytes2 = sizeof(wchar_t) * numElements2;
    const size_t numBytes3 = sizeof(wchar_t) * numElements3;
    void* value1 = NULL, *value2 = NULL, *value3 = NULL;

    size_t actlen1 = 0, actlen2 = 0, actlen3 = 0;
    int result = 0, result1 = 0, result2 = 0, result3 = 0;
    unsigned bytes_allocated = 0;

    value1 = etch_malloc(numBytes1 + 2, 0);   
    value2 = etch_malloc(numBytes2 + 2, 0);  
    value3 = etch_malloc(numBytes3 + 2, 0);  
    
//	wcscpy_s(value1, numElements1+1, name1);   
//    wcscpy_s(value2, numElements2+1, name2);   
//    wcscpy_s(value3, numElements3+1, name3);   

	wcscpy(value1, name1);   
    wcscpy(value2, name2);   
    wcscpy(value3, name3);   


    actlen1 = wcslen(name1);  actlen2 = wcslen(name2); actlen3 = wcslen(name3);

    myhashtab = new_hashtable(16);  
    myhashtab->is_readonly_keys   = TRUE; /* keys will be freed on destroy  */
    myhashtab->is_readonly_values = TRUE; /* values will be freed on destroy */
    myhashtab->is_tracked_memory  = TRUE; /* hashtable will etch_free content */

    /* here we are creating id_name, field, and type objects using names allocated
     * on out local stack. therefore the cleanup parameters must be set to not free 
     * memory for the name part, or we will crash. note that name is part of the 
     * key structs, and is also the value of the hashed key/value pair, however 
     * we etch_malloc'ed memory for the values and copied our stack strings into it,
     * so we will want to free the value memory. we could ask the hashtable to do
     * so but we'll do it manually here instead.
     */
    my_idname = new_id_name(name1);
    my_field  = new_field(name2);
    my_type   = new_type(name3);

    CU_ASSERT_PTR_NOT_NULL_FATAL(my_idname);
    CU_ASSERT_PTR_NOT_NULL_FATAL(my_field);
    CU_ASSERT_PTR_NOT_NULL_FATAL(my_type);

   /* ensure constructors populated the hash key */
    CU_ASSERT_NOT_EQUAL_FATAL(((etch_object*)my_idname)->hashkey,0);
    CU_ASSERT_NOT_EQUAL_FATAL(((etch_object*)my_field)->hashkey,0);
    CU_ASSERT_NOT_EQUAL_FATAL(((etch_object*)my_type)->hashkey,0);

    /* ensure constructors populated the id (hash of name) */
    CU_ASSERT_NOT_EQUAL_FATAL(my_idname->id,0);
    CU_ASSERT_NOT_EQUAL_FATAL(my_field->id,0);
    CU_ASSERT_NOT_EQUAL_FATAL(my_type->id,0);

    result = ((struct i_hashtable*)((etch_object*)myhashtab)->vtab)->inserth(myhashtab->realtable, my_idname, value1, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = ((struct i_hashtable*)((etch_object*)myhashtab)->vtab)->inserth(myhashtab->realtable, my_field, value2, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = ((struct i_hashtable*)((etch_object*)myhashtab)->vtab)->inserth(myhashtab->realtable, my_type, value3, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = ((struct i_hashtable*)((etch_object*)myhashtab)->vtab)->findh(myhashtab->realtable, ((etch_object*)my_idname)->hashkey, myhashtab, &myentry);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_PTR_NOT_NULL(myentry->key);
    CU_ASSERT_PTR_NOT_NULL(myentry->value);
    result = destroy_content_id_name(myentry->key);  
    CU_ASSERT_EQUAL(result,0);  
    etch_free(myentry->value); 

    result = ((struct i_hashtable*)((etch_object*)myhashtab)->vtab)->findh(myhashtab->realtable, ((etch_object*)my_field)->hashkey, myhashtab, &myentry);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_PTR_NOT_NULL(myentry->key);
    CU_ASSERT_PTR_NOT_NULL(myentry->value);
    result = destroy_content_field(myentry->key);  
    CU_ASSERT_EQUAL(result,0); 
    etch_free(myentry->value);  

    result = ((struct i_hashtable*)((etch_object*)myhashtab)->vtab)->findh(myhashtab->realtable, ((etch_object*)my_type)->hashkey, myhashtab, &myentry);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_PTR_NOT_NULL(myentry->key);
    CU_ASSERT_PTR_NOT_NULL(myentry->value);
    result = destroy_content_type(myentry->key);  
    CU_ASSERT_EQUAL(result,0); 
    etch_free(myentry->value);  

    etch_object_destroy(myhashtab);  /* hashtable was asked to not free any content */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}

//int wmain( int argc, wchar_t* argv[], wchar_t* envp[])
CU_pSuite test_etch_hashing_suite()
{
    CU_pSuite ps = CU_add_suite("suite_hashing", init_suite, clean_suite);

    CU_add_test(ps, "test hash id_name etc - auto-cleanup values", test_idname_as_key_hashclean_values);
    CU_add_test(ps, "test hash id_name etc - no auto-cleanup", test_idname_as_key_hashclean_none);

    return ps;
}
