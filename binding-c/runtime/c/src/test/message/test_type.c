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
 * test_type.c 
 * test etch_type
 */

#include "etch_runtime.h"
#include "etch_type.h"
#include "etch_validator.h"
#include "etch_field.h"
#include "etch_objecttypes.h"
#include "etch_serializer.h"
#include "etch_arraylist.h"

#include <stdio.h>
#include "CUnit.h"
#include "Basic.h"
#include "Automated.h"
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

int this_setup()
{
    return 0;
}

/*
 * test_type
 */
static void test_type(void)
{
    int result = 0;
    etch_type *type1 = NULL, *type2 = NULL;

    const wchar_t* nametext1 = L"abracadabra";

    const wchar_t* nametext2 = L"gilgamesh";
    
#ifdef ETCH_DEBUGALLOC
    alloc_a = etch_showmem(0, FALSE);
#endif
    type1   = new_type(NULL);
    CU_ASSERT_PTR_NULL(type1);

#ifdef ETCH_DEBUGALLOC
    alloc_b = etch_showmem(0, FALSE);
    CU_ASSERT_EQUAL(alloc_a, alloc_b); 
#endif

    type1   = new_type(nametext1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(type1);
    CU_ASSERT_TRUE(is_good_type(type1));

    type2   = new_type(nametext1);
    CU_ASSERT_TRUE(is_equal_types(type1, type2));
    result = memcmp(type1->name, type2->name, type1->namebytelen);
    CU_ASSERT_EQUAL(result,0);

    etch_object_destroy(type1);
    etch_object_destroy(type2);

#ifdef ETCH_DEBUGALLOC
    alloc_a = etch_showmem(0, FALSE);
    CU_ASSERT_EQUAL(alloc_a, alloc_b); 
#endif

    type1 = new_type(nametext1);
    type2 = new_type(nametext2);

    CU_ASSERT_FALSE(is_equal_types(type1, type2));

    etch_object_destroy(type1);
    etch_object_destroy(type2);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_type_hashfunc
 * test wire hashing function
 */
static void test_type_hashfunc(void)
{
    unsigned hash1 = 0, hash2 = 0;
    etch_type *type1 = NULL, *type2 = NULL;

    const wchar_t* nametext1 = L"abracadabra";
    const size_t   numelts1  = wcslen(nametext1);
    const size_t   numbytes1 = sizeof(wchar_t) * ( numelts1 + 1 );

    const wchar_t* nametext2 = L"gilgamesh";
    const size_t   numelts2  = wcslen(nametext2);
    const size_t   numbytes2 = sizeof(wchar_t) * ( numelts2 + 1 );
 
    hash1 = compute_type_id_from_widename(nametext1);
    hash2 = compute_type_id_from_widename(nametext2);

    CU_ASSERT_NOT_EQUAL(hash1, hash2);

    type1 = new_type(nametext1);
    type2 = new_type(nametext2);
    CU_ASSERT_EQUAL(type1->namebytelen, numbytes1);
    CU_ASSERT_EQUAL(type2->namebytelen, numbytes2);
    CU_ASSERT_EQUAL(type1->id, hash1);
    CU_ASSERT_EQUAL(type2->id, hash2);

    etch_object_destroy(type1);
    etch_object_destroy(type2);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/*
 * function pointers used by test_getset()
 */
static int bogus_stubhelper (void* stub, void* delsvc, void* obj, void* whofrom, void* msg) { return 0; }



/**
 * test_getset
 * test mutators
 */
static void test_getset(void)
{
    unsigned int this_class  = 0, old_class = 0;
    unsigned char   cur_flag = 0, old_flag = 0;
    unsigned int cur_timeout = 0, old_timeout = 0;
    opaque_stubhelper cur_helper = NULL, old_helper = NULL;
    etch_serializer *cur_impexphelper, *old_impexphelper;
    etch_type *type1 = new_type(L"abracadabra");
    etch_type *type2 = new_type(L"gilgamesh"), *cur_type = NULL, *old_type  = NULL;
    etch_field *fld1 = new_field(L"field1"),  *cur_field = NULL, *old_field = NULL;
    etch_serializer* impxhelp1 = (etch_serializer*) new_object(sizeof(etch_object), ETCHTYPEB_ETCHOBJECT, CLASSID_NONE);
    const unsigned int TEST_CLASS = (ETCHTYPEB_PRIMITIVE << 16) | CLASSID_PRIMITIVE_INT32;

    /* component type */
    this_class = etchtype_get_component_type(type1);
    CU_ASSERT_EQUAL(this_class, 0);

    old_class = etchtype_set_component_type(type1, TEST_CLASS);
    CU_ASSERT_EQUAL(old_class, 0);

    this_class = etchtype_get_component_type(type1);
    CU_ASSERT_EQUAL(this_class, TEST_CLASS);

    old_class = etchtype_set_component_type(type1, 0);
    CU_ASSERT_EQUAL(old_class, TEST_CLASS);

    /* timeout */
    cur_timeout = etchtype_get_timeout(type1);
    CU_ASSERT_EQUAL(cur_timeout, 0);

    old_timeout = etchtype_set_timeout(type1, 1000);
    CU_ASSERT_EQUAL(old_timeout, 0);

    cur_timeout = etchtype_get_timeout(type1);
    CU_ASSERT_EQUAL(cur_timeout, 1000);

    old_timeout = etchtype_set_timeout(type1, 0);
    CU_ASSERT_EQUAL(old_timeout, 1000);

    /* result type */
    cur_type = etchtype_get_result_type(type1);
    CU_ASSERT_PTR_EQUAL(cur_type, NULL);

    old_type = etchtype_set_result_type(type1, type2);
    CU_ASSERT_PTR_EQUAL(old_type, NULL);

    cur_type = etchtype_get_result_type(type1);
    CU_ASSERT_PTR_EQUAL(cur_type, type2);

    old_type = etchtype_set_result_type(type1, NULL);
    CU_ASSERT_PTR_EQUAL(old_type, type2);

    /* super type */
    cur_type = etchtype_get_super_type(type1);
    CU_ASSERT_PTR_EQUAL(cur_type, NULL);

    old_type = etchtype_set_super_type(type1, type2);
    CU_ASSERT_PTR_EQUAL(old_type, NULL);

    cur_type = etchtype_get_super_type(type1);
    CU_ASSERT_PTR_EQUAL(cur_type, type2);

    old_type = etchtype_set_super_type(type1, NULL);
    CU_ASSERT_PTR_EQUAL(old_type, type2);

    /* response field */
    cur_field = etchtype_get_response_field(type1);
    CU_ASSERT_PTR_EQUAL(cur_field, NULL);

    old_field = etchtype_set_response_field(type1, type2);
    CU_ASSERT_PTR_EQUAL(old_field, NULL);

    cur_field = etchtype_get_response_field(type1);
    CU_ASSERT_PTR_EQUAL(cur_field, type2);

    old_field = etchtype_set_response_field(type1, NULL);
    CU_ASSERT_PTR_EQUAL(old_field, type2);

    /* run validators */
    cur_flag = etchtype_get_run_validators(type1);
    CU_ASSERT_EQUAL(cur_flag, FALSE);

    old_flag = etchtype_set_run_validators(type1, TRUE);
    CU_ASSERT_EQUAL(old_flag, FALSE);

    cur_flag = etchtype_get_run_validators(type1);
    CU_ASSERT_EQUAL(cur_flag, TRUE);

    old_flag = etchtype_set_run_validators(type1,FALSE);
    CU_ASSERT_EQUAL(old_flag, TRUE);

    /* stub helper */
    cur_helper = etchtype_get_type_stubhelper(type1);
    CU_ASSERT_PTR_EQUAL(cur_helper, NULL);

    old_helper = etchtype_set_type_stubhelper(type1, bogus_stubhelper);
    CU_ASSERT_PTR_EQUAL(old_helper, NULL);

    old_helper = etchtype_get_type_stubhelper(type1);
    CU_ASSERT_PTR_EQUAL(old_helper, bogus_stubhelper);

    old_helper = etchtype_set_type_stubhelper(type1, NULL);
    CU_ASSERT_PTR_EQUAL(old_helper, bogus_stubhelper);

    /* import/export helper */      
    cur_impexphelper = etchtype_get_impexphelper(type1);
    CU_ASSERT_PTR_EQUAL(cur_impexphelper, NULL);

    old_impexphelper = etchtype_set_impexphelper(type1, impxhelp1);
    CU_ASSERT_PTR_EQUAL(old_impexphelper, NULL);

    cur_impexphelper = etchtype_get_impexphelper(type1);
    CU_ASSERT_PTR_EQUAL(cur_impexphelper, impxhelp1);

    old_impexphelper = etchtype_set_impexphelper(type1, NULL);
    CU_ASSERT_PTR_EQUAL(old_impexphelper, impxhelp1);


    etch_object_destroy(fld1);
    etch_object_destroy(type2);
    etch_object_destroy(type1);
    etch_object_destroy(impxhelp1);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_assignable()
 * test is_assignable_from()
 */
static void test_assignable(void)
{
    int result = 0;
    const unsigned short TEST_SUPERCLASS_ID = 0xffff;
    etch_type* type1 = new_type(L"type1");
    etch_type* type2 = new_type(L"type2");
    ((etch_object*)type1)->class_id  = TEST_SUPERCLASS_ID;
    etchtype_set_super_type(type2, type1);

    result = etchtype_is_assignable_from(type1, type2);
    CU_ASSERT_EQUAL(result, TRUE); 

    result = etchtype_is_assignable_from(type2, type1);
    CU_ASSERT_EQUAL(result, FALSE); 

    result = etchtype_is_assignable_from(type2, NULL);
    CU_ASSERT_EQUAL(result, FALSE); 

    result = etchtype_is_assignable_from(NULL, type1);
    CU_ASSERT_EQUAL(result, FALSE); 

    etch_object_destroy(type1);
    etch_object_destroy(type2);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_validators_access
 * test put and get validators
 */
static void test_validators_access(void)
{
    etch_iterator iterator;
    etch_field* vtor1_key = new_field(L"vtor1");
    etch_field* vtor2_key = new_field(L"vtor2");
    etch_field* bogus_key = new_field(L"bogus");
    etch_type *type1 = new_type(L"abracadabra");
    etch_type *typev = new_type(L"struct"); /* struct validator type */
    etch_validator* vtor1 = etchvtor_int32_get(0);
    etch_validator* vtor2 = etchvtor_struct_get(typev, 0);
    etch_validator* vtor_return = NULL;

    int result = etchtype_put_validator(type1, (etch_field*)etch_object_clone_func(vtor1_key), (etch_object*) vtor1);
    CU_ASSERT_EQUAL(result, 0);

    vtor_return = (etch_validator*) etchtype_get_validator_by_name(type1, vtor1_key->name);
    CU_ASSERT_PTR_NOT_NULL(vtor_return);   
    CU_ASSERT_PTR_EQUAL(vtor_return, vtor1);

    vtor_return = (etch_validator*) etchtype_get_validator_by_id(type1, vtor1_key->id);
    CU_ASSERT_PTR_NOT_NULL(vtor_return);
    CU_ASSERT_PTR_EQUAL(vtor_return, vtor1);

    result = etchtype_put_validator(type1, (etch_field*)etch_object_clone_func(vtor2_key), (etch_object*) vtor2);
    CU_ASSERT_EQUAL(result, 0);

    result = etchtype_validators_count(type1);
    CU_ASSERT_EQUAL(result, 2);

    vtor_return = (etch_validator*) etchtype_get_validator_by_name(type1, vtor1_key->name);
    CU_ASSERT_PTR_NOT_NULL(vtor_return);
    CU_ASSERT_PTR_EQUAL(vtor_return, vtor1);

    vtor_return = (etch_validator*) etchtype_get_validator_by_id(type1, vtor1_key->id);
    CU_ASSERT_PTR_NOT_NULL(vtor_return);
    CU_ASSERT_PTR_EQUAL(vtor_return, vtor1);

    vtor_return = (etch_validator*) etchtype_get_validator_by_name(type1, vtor2_key->name);
    CU_ASSERT_PTR_NOT_NULL(vtor_return);
    CU_ASSERT_PTR_EQUAL(vtor_return, vtor2);

    vtor_return = (etch_validator*) etchtype_get_validator_by_id(type1, vtor2_key->id);
    CU_ASSERT_PTR_NOT_NULL(vtor_return);
    CU_ASSERT_PTR_EQUAL(vtor_return, vtor2);

    /* iterate over the validators map */
    result = etchtype_set_validators_iterator(type1, &iterator);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    while(iterator.has_next(&iterator))
    {   
        const etch_validator* this_vtor = (etch_validator*) iterator.current_value;

        if  (this_vtor == vtor1)
             result = TRUE;
        else
        if  (this_vtor == vtor2)
             result = TRUE; 
        else result = FALSE;  
             
        CU_ASSERT_EQUAL_FATAL(result, TRUE);

        iterator.next(&iterator);
    }

    /* etchtype_clear_validators() is superfluous here, destruction of the type
     * has the same result, however we may as well test it */
    result = etchtype_clear_validators(type1);
    CU_ASSERT_EQUAL(result, 2); /* result of clear is cleared count */

    result = etchtype_validators_count(type1);
    CU_ASSERT_EQUAL(result, 0);

    /* etchtype_put_validator() was given clones of these fields as validator 
     * keys, which were freed during etchtype_clear_validators. this is the model
     * to use in the binding, i.e. clone a static etch_field for put_validator().
     */
    etch_object_destroy(vtor1_key);
    etch_object_destroy(vtor2_key);
    etch_object_destroy(bogus_key);

    /* vtor1 will not have been freed during etchtype_clear_validators(), 
     * since it is a cached validator. likewise, calling its destructor  
     * would have no effect, etchtype_clear_validators() has already tried.
     * etchvtor_clear_cache() will free it and any other cached validators.
     * vtor2 will however have been freed during etchtype_clear_validators(),  
     * since a struct validator is not one of the cached validator types. 
     * in any case, we don't need to be concerned here about which validators
     * are cached and which are not, since etchtype_clear_validators() will
     * free the uncached ones, and etchvtor_clear_cache() the cached ones.
     */ 
    etch_object_destroy(typev);
    etch_object_destroy(type1);
    etchvtor_clear_cache(); /* destroy cached validators (vtor1 in this case) */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_fields_access
 */
static void test_fields_access(void)
{
    etch_iterator iterator;
    etch_field* field1 = new_field(L"field1");
    etch_field* field2 = new_field(L"field2");
    etch_field* field3 = new_field(L"field3");
    etch_field* fieldx = new_field(L"fieldx");
    etch_field* field_return = NULL;
    etch_field* field_clone  = NULL;
    etch_arraylist* list_return = NULL;
    etch_type *type1 = new_type(L"abracadabra");
    const int EXPECTED_FIELDS_COUNT = 3;
    int result = 0, newfield_id = 0;

    field_clone = (etch_field*)etch_object_clone_func(field1);
    field_return = etchtype_add_field(type1, field_clone);
    CU_ASSERT_PTR_NOT_NULL(field_return);
    CU_ASSERT_PTR_EQUAL(field_return, field_clone);

    /* ensure that trying to add a duplicate field returns the existing field.
     * note that the new clone is destroyed by etchtype_add_field when it 
     * determines that the supplied field can't be added */
    field_return = etchtype_add_field(type1, (etch_field*)etch_object_clone_func(field1));
    CU_ASSERT_PTR_NOT_NULL(field_return);
    CU_ASSERT_PTR_EQUAL(field_return, field_clone);

    /* add a couple more fields */
    field_clone = (etch_field*)etch_object_clone_func(field2);
    field_return = etchtype_add_field(type1, field_clone);
    CU_ASSERT_PTR_NOT_NULL(field_return);
    CU_ASSERT_PTR_EQUAL(field_return, field_clone);

    field_clone = (etch_field*)etch_object_clone_func(field3);
    field_return = etchtype_add_field(type1, field_clone);
    CU_ASSERT_PTR_NOT_NULL(field_return);
    CU_ASSERT_PTR_EQUAL(field_return, field_clone);

    result = etchtype_fields_count(type1);
    CU_ASSERT_EQUAL(result, EXPECTED_FIELDS_COUNT);

    /* test accessors */
    field_return = etchtype_get_field_by_name(type1, field1->name);
    CU_ASSERT_PTR_NOT_NULL(field_return);
    CU_ASSERT_EQUAL(is_equal_fields(field_return, field1), TRUE);

    field_return = etchtype_get_field_by_id(type1, field1->id);
    CU_ASSERT_PTR_NOT_NULL(field_return);
    CU_ASSERT_EQUAL(is_equal_fields(field_return, field1), TRUE);

    field_return = etchtype_get_field_by_name(type1, field2->name);
    CU_ASSERT_PTR_NOT_NULL(field_return);
    CU_ASSERT_EQUAL(is_equal_fields(field_return, field2), TRUE);

    field_return = etchtype_get_field_by_id(type1, field2->id);
    CU_ASSERT_PTR_NOT_NULL(field_return);
    CU_ASSERT_EQUAL(is_equal_fields(field_return, field2), TRUE);

    field_return = etchtype_get_field_by_name(type1, field3->name);
    CU_ASSERT_PTR_NOT_NULL(field_return);
    CU_ASSERT_EQUAL(is_equal_fields(field_return, field3), TRUE);

    field_return = etchtype_get_field_by_id(type1, field3->id);
    CU_ASSERT_PTR_NOT_NULL(field_return);
    CU_ASSERT_EQUAL(is_equal_fields(field_return, field3), TRUE);

    /* test that a "get" of an unrecognized field name, adds a new field
     * with that name. this is consistent with the java binding */
    field_return = etchtype_get_field_by_name(type1, fieldx->name);
    CU_ASSERT_PTR_NOT_NULL_FATAL(field_return);  
    result = etchtype_fields_count(type1);
    CU_ASSERT_EQUAL(result, EXPECTED_FIELDS_COUNT+1);
    newfield_id = field_return->id;

    /* iterate over the type's fieldmap */
    result = etchtype_set_fields_iterator(type1, &iterator);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    while(iterator.has_next(&iterator))
    {   
        const unsigned this_id = ((etch_field*)iterator.current_key)->id;

        if  (this_id == field1->id)
             result = TRUE;
        else
        if  (this_id == field2->id)
             result = TRUE; 
        else
        if  (this_id == field3->id)
             result = TRUE; 
        else
        if  (this_id == newfield_id)
             result = TRUE; 
        else result = FALSE; 
              
        CU_ASSERT_EQUAL(result, TRUE);

        iterator.next(&iterator);
    }

    /* get a list of all the type's fields. 
     * we own the returned list, which does not own its content */
    list_return = (etch_arraylist*) etchtype_get_fields(type1);

    CU_ASSERT_EQUAL_FATAL(is_etch_arraylist(list_return), TRUE);
    CU_ASSERT_EQUAL(list_return->count, EXPECTED_FIELDS_COUNT+1);

    /* iterate over the list of fields */
    set_iterator(&iterator, list_return, &list_return->iterable);

    while(iterator.has_next(&iterator))
    {   
        const unsigned this_id = ((etch_field*)iterator.current_value)->id;

        if  (this_id == field1->id)
             result = TRUE;
        else
        if  (this_id == field2->id)
             result = TRUE; 
        else
        if  (this_id == field3->id)
             result = TRUE; 
        else
        if  (this_id == newfield_id)
             result = TRUE; 
        else result = FALSE;  
          
        CU_ASSERT_EQUAL(result, TRUE);

        iterator.next(&iterator);
    }

    etch_object_destroy(list_return);
    etch_object_destroy(field1);
    etch_object_destroy(field2);
    etch_object_destroy(field3);
    etch_object_destroy(fieldx);
    etch_object_destroy(type1);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_validators_chained
 * test memory mangement of chained validators
 */
static void test_validators_chained(void)
{
    etch_field* vtor1_key = new_field(L"vtor1");
    etch_field* vtor2_key = new_field(L"vtor2");
    etch_type *structtype = new_type(L"struct"); /* struct validator type */
    etch_validator* vtor1 = etchvtor_int32_get(0);
    etch_validator* vtor2 = etchvtor_int16_get(0);
    etch_validator* vtor3 = etchvtor_byte_get(0);
    etch_validator* vtor4 = etchvtor_int32_get(0);
    etch_validator* vtor5 = etchvtor_struct_get(structtype, 0);
    etch_validator* vtor6 = etchvtor_struct_get(structtype, 0);
    etch_validator* vtor7 = etchvtor_struct_get(structtype, 0);
    etch_validator* vtor8 = etchvtor_string_get(0);
    etch_type *type1 = new_type(L"abracadabra");

    /* test 3 validators with the same key. when a validator is put to a type
     * and the type already has one or more validators with the specified key, 
     * the new validator is chained to the existing validator */
    int result = etchtype_put_validator(type1, (etch_field*)etch_object_clone_func(vtor1_key), (etch_object*) vtor1);
    CU_ASSERT_EQUAL(result, 0);

    result = etchtype_put_validator(type1, (etch_field*)etch_object_clone_func(vtor1_key), (etch_object*) vtor2);

    result = etchtype_put_validator(type1, (etch_field*)etch_object_clone_func(vtor1_key), (etch_object*) vtor3);

    /* etchtype_clear_validators is needed here, since we want to begin 
     * a new test with the type emptied of validators.
     */
    etchtype_clear_validators(type1); 

     /* test 3 validators with the same key, and 2 validators with another key.
      * note that the c binding convention is to always get a "fresh" validator 
      * when we add a validator to a type, even though the get will return the 
      * same object when the validator type we get is cached. */

     result = etchtype_put_validator(type1, (etch_field*)etch_object_clone_func(vtor1_key), (etch_object*) vtor4);
     result = etchtype_put_validator(type1, (etch_field*)etch_object_clone_func(vtor1_key), (etch_object*) vtor5);
     result = etchtype_put_validator(type1, (etch_field*)etch_object_clone_func(vtor1_key), (etch_object*) vtor6);

     result = etchtype_put_validator(type1, (etch_field*)etch_object_clone_func(vtor2_key), (etch_object*) vtor7);
     result = etchtype_put_validator(type1, (etch_field*)etch_object_clone_func(vtor2_key), (etch_object*) vtor8);

    /* etchtype_clear_validators is superfluous here, so we test not doing it. 
     * when we destroy type1 below, the destructor for the type's instance data 
     * object destroys the type's hashtables, which causes the hashtables to 
     * first clear their content, as etchtype_clear_validators() would do.
     */
    /* etchtype_clear_validators(type1); */

    /* etchtype_put_validator() was given clones of these fields as validator 
     * keys, which were freed during etchtype_clear_validators. this is the model
     * to use in the binding, i.e. clone a static etch_field for put_validator().
     */
    etch_object_destroy(vtor1_key);
    etch_object_destroy(vtor2_key);

    etch_object_destroy(structtype); 

    /* the destroy() of type1 clears the type's fields and validators maps,
     * which causes the fields, validator field keys, and non-cached validators,
     * to be destroyed.
     */
    etch_object_destroy(type1);
    etchvtor_clear_cache(); /* destroy any singleton cached validators */

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
CU_pSuite test_etch_type_suite()
{    
    CU_pSuite ps = CU_add_suite("type test suite", init_suite, clean_suite);

    CU_add_test(ps, "test etch_type constructors/destructors", test_type);  
    CU_add_test(ps, "test etch_type hashing", test_type_hashfunc); 
    CU_add_test(ps, "test getters/setters", test_getset);  
    CU_add_test(ps, "test assignability", test_assignable);  
    CU_add_test(ps, "test validators access", test_validators_access);  
    CU_add_test(ps, "test fields access", test_fields_access);  
    CU_add_test(ps, "test chained validators", test_validators_chained);

    return ps;
}
