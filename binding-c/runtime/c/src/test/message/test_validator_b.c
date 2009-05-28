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

/**
 * test_validator_b.c
 * more etch_validator tests
 */

#include "apr_time.h" /* some apr must be included first */
#include "etchthread.h"
#include "etch_validator.h"   
#include "etch_structval.h"

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etch_tagdata.h"
#include "etch_global.h"
#include "etchlog.h"


int apr_setup(void);
int apr_teardown(void);
int this_setup();
int this_teardown();
apr_pool_t* g_apr_mempool;
const char* pooltag = "etchpool";


int init_suite(void)
{
    apr_setup();
    etch_runtime_init();
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

#define IS_DEBUG_CONSOLE TRUE

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
 * do_struct_element_vtor_test()
 * common test template for the structvalue element_validator tests
 */
void do_struct_element_vtor_test(int testdims, char* name, unsigned short expected_class_id, etch_type* thistype)
{
    etch_validator* vtor = etchvtor_struct_get(thistype, testdims);

    CU_ASSERT_PTR_NOT_NULL_FATAL(vtor); 
    CU_ASSERT_EQUAL(vtor->numdimensions, testdims); 
    CU_ASSERT_EQUAL(vtor->expected_class_id, expected_class_id); 
    CU_ASSERT_STRING_EQUAL(vtor->description, name); 
    CU_ASSERT_EQUAL(is_equal_types(vtor->struct_type, thistype), TRUE);

    /* validator destructor is benign if validator is marked cached
     * however a struct validator is not cached */
    vtor->destroy(vtor);
}


/**
 * test_struct_element_validator()
 */
void test_struct_element_validator(void)  
{
    etch_type* typeabc = new_type(L"abc");

    do_struct_element_vtor_test(0, "struct_abc[0]", CLASSID_STRUCTVALUE,  typeabc);
    do_struct_element_vtor_test(1, "struct_abc[1]", CLASSID_ARRAY_OBJECT, typeabc);
    do_struct_element_vtor_test(2, "struct_abc[2]", CLASSID_ARRAY_OBJECT, typeabc);
    do_struct_element_vtor_test(3, "struct_abc[3]", CLASSID_ARRAY_OBJECT, typeabc);

    typeabc->destroy(typeabc);
    etchvtor_clear_cache();    /* not needed since struct validator not cached  */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/**
 * do_struct_goodvalue_test()
 */
void do_struct_goodvalue_test(int testdims, const byte expected_typecode, 
     etch_object* value, etch_type* thistype, const int is_expected_newobject)
{
    int result = 0;
    byte validated_typecode = 0;
    etch_object* validated_value = NULL;

    etch_validator* vtor = etchvtor_struct_get(thistype, testdims);
    CU_ASSERT_PTR_NOT_NULL_FATAL(vtor); 

    result = vtor->check_value(vtor, value, &validated_typecode);
    CU_ASSERT_EQUAL_FATAL(result,0); 
    CU_ASSERT_EQUAL(validated_typecode, expected_typecode); 

    result = vtor->validate(vtor, value);
    CU_ASSERT_EQUAL_FATAL(result,0); 

    validated_value = vtor->validate_value(vtor, value);
    CU_ASSERT_PTR_NOT_NULL_FATAL(validated_value); 

    if  (is_expected_newobject)
    {
         CU_ASSERT_PTR_NOT_EQUAL_FATAL(value, validated_value);
    }
    else CU_ASSERT_PTR_EQUAL_FATAL(value, validated_value);

    /* validated_value can return the same object as paassed, or a different
     * object. for example when we expect etch_byte but validate against an
     * etch_int32, we would pass an etch_int32 and get back an etch_byte.  
     * in the real world, we would replace the passed value with the validated
     * value; however in this test the caller is not prepared to handle this
     * situation, so we free the new object, and let caller free the original */
    if (validated_value != value)
        validated_value->destroy(validated_value);       

    /* validator destructor is benign if validator is marked cached
     * however a struct validator is not cached */
    vtor->destroy(vtor);      
}



/**
 * do_struct_badvalue_test()
 */
void do_struct_badvalue_test(int testdims, etch_object* value, etch_type* thistype)
{
    int result = 0;
    byte validated_typecode = 0;
    etch_object* validated_value = NULL;

    etch_validator* vtor = etchvtor_struct_get(thistype, testdims);
    CU_ASSERT_PTR_NOT_NULL_FATAL(vtor); 

    result = vtor->check_value(vtor, value, &validated_typecode);
    CU_ASSERT_EQUAL_FATAL(result,-1); 

    result = vtor->validate(vtor, value);
    CU_ASSERT_EQUAL_FATAL(result,-1); 

    validated_value = vtor->validate_value(vtor, value);
    CU_ASSERT_PTR_NULL_FATAL(validated_value); 

    if (validated_value && validated_value != value)
        validated_value->destroy(validated_value);       

    /* validator destructor is benign if validator is marked cached
     * however a struct validator is not cached */
    vtor->destroy(vtor);      
}


/**
 * test_struct_good_values()
 */
void test_struct_good_values(void)  
{
    etch_type* typeabc = new_type(L"abc");
    etch_nativearray* array_of_struct = NULL;
    const int NUMDIMS0 = 0, NUMDIMS1 = 1, NUMDIMS2 = 2, DIMSIZE4 = 4;

    etch_structvalue* sv = new_structvalue(typeabc);
    do_struct_goodvalue_test(NUMDIMS0, ETCH_XTRNL_TYPECODE_CUSTOM, (etch_object*) sv, typeabc, FALSE);

    /* following tests create an empty array of structvalue objects */
    array_of_struct = new_nativearray_of(sv->obj_type, sv->class_id, NUMDIMS1, DIMSIZE4, 0, 0);
    do_struct_goodvalue_test(NUMDIMS1, ETCH_XTRNL_TYPECODE_ARRAY, (etch_object*) array_of_struct, typeabc, FALSE);
    array_of_struct->destroy(array_of_struct);

    array_of_struct = new_nativearray_of(sv->obj_type, sv->class_id, NUMDIMS2, DIMSIZE4, DIMSIZE4, 0);
    do_struct_goodvalue_test(NUMDIMS2, ETCH_XTRNL_TYPECODE_ARRAY, (etch_object*) array_of_struct, typeabc, FALSE);
    array_of_struct->destroy(array_of_struct);

    sv->destroy(sv);          
    typeabc->destroy(typeabc); /* in practice this would have been a static type */
    etchvtor_clear_cache();    /* not needed since struct validator not cached  */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/**
 * test_struct_bad_values()  
 */
void test_struct_bad_values(void)  
{
    etch_type* type_expected = new_type(L"abc");
    etch_type* type_unexpect = new_type(L"def");
    etch_nativearray* array_of_struct = NULL;
    const int NUMDIMS0 = 0, NUMDIMS1 = 1, NUMDIMS2 = 2, NUMDIMS3 = 3, DIMSIZE4 = 4;

    etch_structvalue* sv = new_structvalue(type_expected);
    etch_structvalue* svbogus = new_structvalue(type_unexpect);
    etch_int32*  intobj  = new_int32(1);
    etch_object* objobj  = new_etch_object(CLASSID_OBJECT, intobj);
    objobj->is_value_object = TRUE; /* objobj will destroy intobj */

    do_struct_badvalue_test(0, (etch_object*) intobj, type_expected);
    do_struct_badvalue_test(0, objobj,  type_expected);
    do_struct_badvalue_test(0, (etch_object*) svbogus, type_expected);
    do_struct_badvalue_test(0, (etch_object*) sv, type_unexpect);

    do_struct_badvalue_test(1, (etch_object*) intobj, type_expected);
    do_struct_badvalue_test(1, objobj, type_expected);
    do_struct_badvalue_test(1, (etch_object*) svbogus, type_expected);
    do_struct_badvalue_test(1, (etch_object*) sv, type_unexpect);

    /* following tests create an empty array of structvalue objects */
    array_of_struct = new_nativearray_of(sv->obj_type, sv->class_id, NUMDIMS1, DIMSIZE4, 0, 0);
    do_struct_badvalue_test(NUMDIMS0, (etch_object*) array_of_struct, type_expected);
    array_of_struct->destroy(array_of_struct);

    array_of_struct = new_nativearray_of(sv->obj_type, sv->class_id, NUMDIMS2, DIMSIZE4, DIMSIZE4, 0);
    do_struct_badvalue_test(NUMDIMS1, (etch_object*) array_of_struct, type_expected);
    do_struct_badvalue_test(NUMDIMS0, (etch_object*) array_of_struct, type_expected);
    array_of_struct->destroy(array_of_struct);

    array_of_struct = new_nativearray_of(sv->obj_type, sv->class_id, NUMDIMS3, DIMSIZE4, DIMSIZE4, DIMSIZE4);
    do_struct_badvalue_test(NUMDIMS2, (etch_object*) array_of_struct, type_expected);
    do_struct_badvalue_test(NUMDIMS1, (etch_object*) array_of_struct, type_expected);
    do_struct_badvalue_test(NUMDIMS0, (etch_object*) array_of_struct, type_expected);
    array_of_struct->destroy(array_of_struct);
  
    objobj->destroy(objobj);
    sv->destroy(sv);   
    svbogus->destroy(svbogus);       
    type_expected->destroy(type_expected); /* in real world these are static types */
    type_unexpect->destroy(type_unexpect);  
    etchvtor_clear_cache();  /* however struct validator is not cached */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/**
 * main   
 */
int _tmain(int argc, _TCHAR* argv[])
{
    char c=0;
    CU_pSuite ps = NULL;
    g_is_automated_test = argc > 1 && 0 != wcscmp(argv[1], L"-a");
    if (0 != CU_initialize_registry()) return 0;
    CU_set_output_filename("../test_validator_b");
    ps = CU_add_suite("validator test suite b", init_suite, clean_suite);
    etch_watch_id = 1; 

    CU_add_test(ps, "test struct element validator", test_struct_element_validator);   
    CU_add_test(ps, "test struct good values", test_struct_good_values); 
    CU_add_test(ps, "test struct bad values",  test_struct_bad_values);     

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