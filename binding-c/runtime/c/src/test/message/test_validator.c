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
 * test_validator.c
 * test etch_validator
 */

#include "apr_time.h" /* some apr must be included first */
#include "etchthread.h"
#include "etch_validator.h"  /* must be included second */

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etch_tagdata.h"
#include "etch_global.h"
#include "etchlog.h"
#include "etch_structval.h"


int apr_setup(void);
int apr_teardown(void);
int this_setup();
int this_teardown();
apr_pool_t* g_apr_mempool;
const char* pooltag = "etchpool";


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

etch_type*    testtype;
etch_boolean* objbool_false;
etch_boolean* objbool_true;
etch_byte*    objbyte_one;
etch_int16*   objshort_two;
etch_int32*   objint_three;
etch_int64*   objlong_four;
etch_float*   objfloat_five;
etch_double*  objdouble_six;
etch_string*  objstring_abc;
etch_object*  objobject_null;
etch_nativearray* arraydim1;
etch_nativearray* arraydim2;
etch_nativearray* arraydim3;


void create_testobjects()
{
    testtype = new_type(L"abc");
    objbool_false  = new_boolean(FALSE);
    objbool_true   = new_boolean(TRUE);
    objbyte_one    = new_byte(1);
    objshort_two   = new_int16(2);
    objint_three   = new_int32(3);
    objlong_four   = new_int64(4);
    objfloat_five  = new_float(5.0);
    objdouble_six  = new_double(6.0);
    objstring_abc  = new_string(L"abc", ETCH_ENCODING_UTF16);
    objobject_null = (etch_object*) new_primitive(sizeof(etch_object), CLASSID_NONE);
    arraydim1  = new_nativearray(CLASSID_ARRAY_BOOL, 1, 1, 4, 0, 0);
    arraydim2  = new_nativearray(CLASSID_ARRAY_BOOL, 1, 2, 4, 3, 0);
    arraydim3  = new_nativearray(CLASSID_ARRAY_BOOL, 1, 3, 4, 3, 2);
}


void destroy_testobjects()
{
    testtype->destroy(testtype); testtype = NULL;
    objbool_false ->destroy(objbool_false); objbool_false = NULL;
    objbool_true  ->destroy(objbool_true);  objbool_true  = NULL;
    objbyte_one   ->destroy(objbyte_one);   objbyte_one   = NULL;
    objshort_two  ->destroy(objshort_two);  objshort_two  = NULL;
    objint_three  ->destroy(objint_three);  objint_three  = NULL;
    objlong_four  ->destroy(objlong_four);  objlong_four  = NULL;
    objfloat_five ->destroy(objfloat_five); objfloat_five = NULL;
    objdouble_six ->destroy(objdouble_six); objdouble_six = NULL;
    objstring_abc ->destroy(objstring_abc); objstring_abc = NULL;
    objobject_null->destroy(objobject_null);objobject_null= NULL;
    arraydim1->destroy(arraydim1);  arraydim1 = NULL;
    arraydim2->destroy(arraydim2);  arraydim2 = NULL;
    arraydim3->destroy(arraydim3);  arraydim3 = NULL;
}


/**
 * do_boolean_test()
 * common test template for the boolean validator tests
 */
void do_boolean_test(int dims, char* name, byte in_typecode, 
    unsigned short validated_class_id, etch_object* objgood, etch_object* objbad)
{
    int result = 0; 
    byte out_typecode = 0;
    etch_validator* vtor = etchvtor_boolean_get(dims);

    CU_ASSERT_PTR_NOT_NULL_FATAL(vtor);
    CU_ASSERT_EQUAL(vtor->numdimensions, dims); 
    CU_ASSERT_EQUAL(validated_class_id, vtor->expected_class_id); 
    CU_ASSERT_STRING_EQUAL(name, vtor->description);

    result = vtor->validate(vtor, objgood);
    CU_ASSERT_EQUAL(result,0);

    result = vtor->validate(vtor, objbad);
    CU_ASSERT_EQUAL(result,-1);
    if (result != -1)
        result = 1;

    result = vtor->check_value(vtor, objgood, &out_typecode);
    CU_ASSERT_EQUAL(result, 0);   
    CU_ASSERT_EQUAL(in_typecode, out_typecode);   

    result = vtor->check_value(vtor, objbad, &out_typecode);
    CU_ASSERT_EQUAL(result, -1);  

    CU_ASSERT_EQUAL(vtor->is_cached, TRUE);
    result = vtor->destroy(vtor); /* test that vtor is cached */
    CU_ASSERT_EQUAL(result, -1);  /* and cannot be destroyed */ 
}


/**
 * test_boolean_validator()
 */
void test_boolean_validator(void)  
{
    char* strbool0 = "bool[0]"; 
    create_testobjects();

	do_boolean_test(0, "bool[0]", ETCH_XTRNL_TYPECODE_BOOLEAN_FALSE, 
        CLASSID_PRIMITIVE_BOOL, (etch_object*) objbool_false, (etch_object*) objbyte_one);

	do_boolean_test(0, "bool[0]", ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE, 
        CLASSID_PRIMITIVE_BOOL, (etch_object*) objbool_true, (etch_object*) objshort_two);

	do_boolean_test(0, "bool[0]", ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE, 
        CLASSID_PRIMITIVE_BOOL, (etch_object*) objbool_true, (etch_object*) objint_three);

	do_boolean_test(0, strbool0, ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE, 
        CLASSID_PRIMITIVE_BOOL, (etch_object*) objbool_true, (etch_object*) objlong_four);

	do_boolean_test(0, strbool0, ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE, 
        CLASSID_PRIMITIVE_BOOL, (etch_object*) objbool_true, (etch_object*) objfloat_five);

	do_boolean_test(0, strbool0, ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE, 
        CLASSID_PRIMITIVE_BOOL, (etch_object*) objbool_true, (etch_object*) objdouble_six);

	do_boolean_test(0, strbool0, ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE, 
        CLASSID_PRIMITIVE_BOOL, (etch_object*) objbool_true, (etch_object*) objstring_abc);

	do_boolean_test(0, strbool0, ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE, 
        CLASSID_PRIMITIVE_BOOL, (etch_object*) objbool_true, objobject_null);

	do_boolean_test(1, "bool[1]", ETCH_XTRNL_TYPECODE_ARRAY,
        CLASSID_ARRAY_BOOL, (etch_object*) arraydim1, (etch_object*) objbool_true);

	do_boolean_test(2, "bool[2]", ETCH_XTRNL_TYPECODE_ARRAY,
        CLASSID_ARRAY_BOOL, (etch_object*) arraydim2, (etch_object*) arraydim1);

	do_boolean_test(3, "bool[3]", ETCH_XTRNL_TYPECODE_ARRAY,
        CLASSID_ARRAY_BOOL, (etch_object*) arraydim3, (etch_object*) arraydim2);

    destroy_testobjects(); 
    etchvtor_clear_cache();  /* destroy cached validators */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/*
 * do_recursive_test()
 * run a validate on an anonymous object. if the object is a nativearray, 
 * this method is invoked recursively until the array elements are validated.
 * the caller relinquishes ownership of valobj to this method.
 */
void do_recursive_test(etch_validator* vtor, const int dims, byte* typecodes, objmask* valobj)
{
    int  result = 0;
    static int debug_count;
    signed char expected_typecode = 0, validated_typecode = 0;
    //++debug_count;
    //if (debug_count == 0x28)
    //    result = 0; // debugger target    

    result = vtor->validate(vtor, (etch_object*) valobj);
    CU_ASSERT_EQUAL(result, 0);

    expected_typecode = typecodes[dims];
    result = vtor->check_value(vtor, (etch_object*) valobj, &validated_typecode);
    CU_ASSERT_EQUAL(result,0);
    if  (expected_typecode == validated_typecode)
         result = TRUE;  
    else result = FALSE;  // debugger target 
    CU_ASSERT_EQUAL(result, TRUE);

    if  (dims)
    {   /* if validating an array, iterate the array and recursively validate 
         * each array element, which could itself be an array. todo -- this
         * is coded for nativearray, we may need to generalize to arrayvalue
         */
        int itemcount = 0, i = 0;
        etch_nativearray* a = NULL;
        etch_validator* element_vtor = vtor->element_validator(vtor);
        CU_ASSERT_EQUAL_FATAL(is_etch_nativearray(valobj), TRUE);

        a = (etch_nativearray*) valobj; 
        itemcount = (int) a->dimension[dims-1];

        for(; i < (const int) itemcount; i++)
        {
            objmask* subobj = etch_nativearray_get_element(a, i); 

            CU_ASSERT_PTR_NOT_NULL_FATAL(subobj);
             
            do_recursive_test(element_vtor, dims-1, typecodes, subobj);

            if (is_etch_nativearray(subobj))
                subobj->destroy(subobj); 
        } 

        valobj->destroy(valobj);
    }
    else valobj->destroy(valobj); /* otherwise object was scalar */
}


/*
 * test_array_boolean()
 * test validator on arrays of boolean using the recursive test
 */
void test_array_boolean(void)  
{
    const int numdimensions = 2, dim0count = 3, dim1count = 2;
    boolean x1[2][3] = { { FALSE,FALSE,FALSE,}, { FALSE,FALSE,FALSE,}, };
    boolean x2[2][3] = { { TRUE, TRUE, TRUE, }, { TRUE, TRUE, TRUE, }, };
    byte typecodz[3] = { ETCH_XTRNL_TYPECODE_BOOLEAN_FALSE, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };
    etch_nativearray* a1 = NULL, *a2 = NULL;

    /* test validate boolean array x1/a1 */
    etch_validator* vtor = etchvtor_boolean_get(numdimensions);

    a1 = new_nativearray_from(&x1, CLASSID_ARRAY_BOOL,  
        sizeof(boolean), numdimensions, dim0count, dim1count, 0);  

    /* fyi we relinquish ownership of array a1 here */
    do_recursive_test(vtor, numdimensions, typecodz, (objmask*) a1);

    /* test validate boolean array x2/a2 */
    typecodz[0] = ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE; 
    vtor = etchvtor_boolean_get(numdimensions); 

    a2 = new_nativearray_from(&x2, CLASSID_ARRAY_BOOL,  
        sizeof(boolean), numdimensions, dim0count, dim1count, 0);  

    /* fyi we relinquish ownership of array a2 here */
    do_recursive_test(vtor, numdimensions, typecodz, (objmask*) a2);

    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/*
 * test_array_byte()
 * test validator on arrays of byte using the recursive test
 */
void test_array_byte(void)  
{
    const int numdimensions = 2, dim0count = 3, dim1count = 2;
    byte x1[2][3] = { { 'a',  'b',  'c', }, { 'd',  'e',  'f',}, };
    byte x2[2][3] = { { ETCHTYPE_MIN_BYTE,0xff,0},{ ETCHTYPE_MAX_BYTE,0xff,0, }, };
    byte typecodz[3] = { ETCH_XTRNL_TYPECODE_BYTE, 
        ETCH_XTRNL_TYPECODE_BYTES, ETCH_XTRNL_TYPECODE_ARRAY };
    etch_nativearray* a1 = NULL, *a2 = NULL;

    /* test validate byte array x1/a1 */
    etch_validator* vtor = etchvtor_byte_get(numdimensions);

    a1 = new_nativearray_from(&x1, CLASSID_ARRAY_BYTE,  
        sizeof(byte), numdimensions, dim0count, dim1count, 0);  

    /* fyi we relinquish ownership of array a1 here */
    do_recursive_test(vtor, numdimensions, typecodz, (objmask*) a1);

    /* test validate byte array x2/a2 */
    vtor = etchvtor_byte_get(numdimensions); 

    a2 = new_nativearray_from(&x2, CLASSID_ARRAY_BYTE,  
        sizeof(byte), numdimensions, dim0count, dim1count, 0);  

    /* fyi we relinquish ownership of array a2 here */
    do_recursive_test(vtor, numdimensions, typecodz, (objmask*) a2);

    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/*
 * test_array_int16()
 * test validator on arrays of short using the recursive test
 */
void test_array_int16(void)  
{
    const int numdimensions = 3, dim0count = 4, dim1count = 3, dim2count = 2;
    etch_nativearray* a = NULL;

    short x1[2][3][4] = 
      { { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
        { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
      };
    short x2[2][3][4] = 
      { { { 1,1,1,1, }, { 1,1,1,1, }, { 1,1,1,1, }, },
        { { 1,1,1,1, }, { 1,1,1,1, }, { 1,1,1,1, }, },
      };
    short x3[2][3][4] = 
      { { { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, },
        { { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, },
      };
    short x4[2][3][4] = 
      { { { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
          { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
          { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
        },
        { { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
          { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
          { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
        },
      };
    short x5[2][3][4] = 
      { { { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
          { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
          { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
        },
        { { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
          { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
          { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
        },
      };
    short x6[2][3][4] = 
      { { { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
          { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
          { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
        },
        { { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
          { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
          { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
        },
      };
    short x7[2][3][4] = 
      { { { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
          { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
          { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
        },
        { { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
          { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
          { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
        },
      };

    byte typecodz1[4] = { ETCH_XTRNL_TYPECODE_BYTE, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };

    byte typecodz2[4] = { ETCH_XTRNL_TYPECODE_SHORT, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };

    etch_validator* vtor = etchvtor_int16_get(numdimensions); /* vtor for all tests */

    /* test validate short array x1/tc1 (1-byte shorts) */
    a = new_nativearray_from(&x1, CLASSID_ARRAY_INT16,  
         sizeof(short), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate short array x2/tc1 (1-byte shorts) */
    a = new_nativearray_from(&x2, CLASSID_ARRAY_INT16,  
           sizeof(short), numdimensions, dim0count, dim1count, dim2count);  
 
    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


   /* test validate short array x3/tc1 (1-byte shorts) */
    a = new_nativearray_from(&x3, CLASSID_ARRAY_INT16,  
           sizeof(short), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate short array x4/tc1 (1-byte shorts) */
    a = new_nativearray_from(&x4, CLASSID_ARRAY_INT16,  
           sizeof(short), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate short array x5/tc1 (1-byte shorts) */
    a = new_nativearray_from(&x5, CLASSID_ARRAY_INT16,  
           sizeof(short), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate short array x6/tc2 (2-byte shorts) */
    a = new_nativearray_from(&x6, CLASSID_ARRAY_INT16,  
           sizeof(short), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (objmask*) a);


    /* test validate short array x7/tc2 (2-byte shorts) */
    a = new_nativearray_from(&x7, CLASSID_ARRAY_INT16,  
           sizeof(short), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (objmask*) a);


    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/*
 * test_array_int32()
 * test validator on arrays of int using the recursive test
 */
void test_array_int32(void)  
{
    const int numdimensions = 3, dim0count = 4, dim1count = 3, dim2count = 2;
    etch_nativearray* a = NULL;

    int x1[2][3][4] = 
      { { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
        { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
      };
    int x2[2][3][4] = 
      { { { 1,1,1,1, }, { 1,1,1,1, }, { 1,1,1,1, }, },
        { { 1,1,1,1, }, { 1,1,1,1, }, { 1,1,1,1, }, },
      };
    int x3[2][3][4] = 
      { { { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, },
        { { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, },
      };
    int x4[2][3][4] = 
      { { { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
          { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
          { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
        },
        { { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
          { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
          { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
        },
      };
    int x5[2][3][4] = 
      { { { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
          { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
          { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
        },
        { { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
          { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
          { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
        },
      };
    int x6[2][3][4] = 
      { { { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
          { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
          { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
        },
        { { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
          { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
          { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
        },
      };
    int x7[2][3][4] = 
      { { { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
          { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
          { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
        },
        { { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
          { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
          { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
        },
      };
    int x8[2][3][4] = 
      { { { ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32, }, 
          { ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32, }, 
          { ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32, }, 
        },
        { { ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32, }, 
          { ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32, }, 
          { ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32, }, 
        },
      };
    int x9[2][3][4] = 
      { { { ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32, }, 
          { ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32, }, 
          { ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32, }, 
        },
        { { ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32, }, 
          { ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32, }, 
          { ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32, }, 
        },
      };

    byte typecodz1[4] = { ETCH_XTRNL_TYPECODE_BYTE, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };

    byte typecodz2[4] = { ETCH_XTRNL_TYPECODE_SHORT, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };

    byte typecodz4[4] = { ETCH_XTRNL_TYPECODE_INT, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };

    /* test validate int array x1/tc1 (1-byte ints) */
    etch_validator* vtor = etchvtor_int32_get(numdimensions);  /* allocates object */

    a = new_nativearray_from(&x1, CLASSID_ARRAY_INT32,  
         sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate int array x2/tc1 (1-byte ints) */
    vtor = etchvtor_int32_get(numdimensions);  /* gets cached object */

    a = new_nativearray_from(&x2, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  
 
    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate int array x3/tc1 (1-byte ints) */
    a = new_nativearray_from(&x3, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate int array x4/tc1 (1-byte ints) */
    a = new_nativearray_from(&x4, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate int array x5/tc1 (1-byte ints) */
    a = new_nativearray_from(&x5, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate int array x6/tc2 (2-byte ints) */
    a = new_nativearray_from(&x6, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (objmask*) a);


    /* test validate int array x7/tc2 (2-byte ints) */
    a = new_nativearray_from(&x7, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (objmask*) a);


    /* test validate int array x8/tc4 (4-byte ints) */
    a = new_nativearray_from(&x6, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (objmask*) a);


    /* test validate int array x9/tc4 (4-byte ints) */
    a = new_nativearray_from(&x7, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (objmask*) a);


    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/*
 * test_array_int64()
 * test validator on arrays of long long using the recursive test
 */
void test_array_int64(void)  
{
    const int numdimensions = 3, dim0count = 4, dim1count = 3, dim2count = 2;
    etch_nativearray* a = NULL;

    int64 x1[2][3][4] = 
      { { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
        { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
      };
    int64 x2[2][3][4] = 
      { { { 1,1,1,1, }, { 1,1,1,1, }, { 1,1,1,1, }, },
        { { 1,1,1,1, }, { 1,1,1,1, }, { 1,1,1,1, }, },
      };
    int64 x3[2][3][4] = 
      { { { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, },
        { { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, },
      };
    int64 x4[2][3][4] = 
      { { { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
          { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
          { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
        },
        { { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
          { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
          { ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE,ETCHTYPE_MAX_BYTE, }, 
        },
      };
    int64 x5[2][3][4] = 
      { { { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
          { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
          { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
        },
        { { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
          { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
          { ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE,ETCHTYPE_MIN_BYTE, }, 
        },
      };
    int64 x6[2][3][4] = 
      { { { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
          { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
          { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
        },
        { { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
          { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
          { ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16,ETCHTYPE_MAX_INT16, }, 
        },
      };
    int64 x7[2][3][4] = 
      { { { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
          { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
          { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
        },
        { { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
          { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
          { ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16,ETCHTYPE_MIN_INT16, }, 
        },
      };
    int64 x8[2][3][4] = 
      { { { ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32, }, 
          { ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32, }, 
          { ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32, }, 
        },
        { { ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32, }, 
          { ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32, }, 
          { ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32,ETCHTYPE_MAX_INT32, }, 
        },
      };
    int64 x9[2][3][4] = 
      { { { ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32, }, 
          { ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32, }, 
          { ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32, }, 
        },
        { { ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32, }, 
          { ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32, }, 
          { ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32,ETCHTYPE_MIN_INT32, }, 
        },
      };
    int64 x10[2][3][4] = 
      { { { ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64, }, 
          { ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64, }, 
          { ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64, }, 
        },
        { { ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64, }, 
          { ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64, }, 
          { ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64,ETCHTYPE_MAX_INT64, }, 
        },
      };
    int64 x11[2][3][4] = 
      { { { ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64, }, 
          { ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64, }, 
          { ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64, }, 
        },
        { { ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64, }, 
          { ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64, }, 
          { ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64,ETCHTYPE_MIN_INT64, }, 
        },
      };

    byte typecodz1[4] = { ETCH_XTRNL_TYPECODE_BYTE, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };

    byte typecodz2[4] = { ETCH_XTRNL_TYPECODE_SHORT, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };

    byte typecodz4[4] = { ETCH_XTRNL_TYPECODE_INT, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };

    byte typecodz8[4] = { ETCH_XTRNL_TYPECODE_LONG, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };

    /* test validate int array x1/tc1 (1-byte ints) */
    etch_validator* vtor = etchvtor_int64_get(numdimensions);  /* allocates object */

    a = new_nativearray_from(&x1, CLASSID_ARRAY_INT64,  
         sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate int64 array x2/tc1 (1-byte longs) */
    vtor = etchvtor_int64_get(numdimensions);  /* gets cached object */

    a = new_nativearray_from(&x2, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  
 
    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate int64 array x3/tc1 (1-byte longs) */
    a = new_nativearray_from(&x3, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate int64 array x4/tc1 (1-byte longs) */
    a = new_nativearray_from(&x4, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate int64 array x5/tc1 (1-byte longs) */
    a = new_nativearray_from(&x5, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate int64 array x6/tc2 (2-byte longs) */
    a = new_nativearray_from(&x6, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (objmask*) a);


    /* test validate int64 array x7/tc2 (2-byte longs) */
    a = new_nativearray_from(&x7, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (objmask*) a);


    /* test validate int64 array x8/tc4 (4-byte longs) */
    a = new_nativearray_from(&x8, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz4, (objmask*) a);


    /* test validate int64 array x9/tc4 (4-byte longs) */
    a = new_nativearray_from(&x9, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz4, (objmask*) a);


    /* test validate int64 array x10/tc8 (8-byte longs) */
    a = new_nativearray_from(&x10, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz8, (objmask*) a);


    /* test validate int64 array x11/tc8 (8-byte longs) */
    a = new_nativearray_from(&x11, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz8, (objmask*) a);


    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/*
 * test_array_float()
 * test validator on arrays of float using the recursive test
 */
void test_array_float(void)  
{
    const int numdimensions = 3, dim0count = 4, dim1count = 3, dim2count = 2;
    etch_nativearray* a = NULL;

    float x1[2][3][4] = 
      { { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
        { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
      };
    float x2[2][3][4] = 
      { { { 1,1,1,1, }, { 1,1,1,1, }, { 1,1,1,1, }, },
        { { 1,1,1,1, }, { 1,1,1,1, }, { 1,1,1,1, }, },
      };
    float x3[2][3][4] = 
      { { { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, },
        { { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, },
      };
    float x4[2][3][4] = 
      { { { ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT, }, 
          { ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT, }, 
          { ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT, }, 
        },
        { { ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT, }, 
          { ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT, }, 
          { ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT,ETCHTYPE_MAX_FLOAT, }, 
        },
      };
    float x5[2][3][4] = 
      { { { ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT, }, 
          { ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT, }, 
          { ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT, }, 
        },
        { { ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT, }, 
          { ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT, }, 
          { ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT,ETCHTYPE_MIN_FLOAT, }, 
        },
      };

    byte typecodz1[4] = { ETCH_XTRNL_TYPECODE_FLOAT, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };

    etch_validator* vtor = etchvtor_float_get(numdimensions); /* vtor for all tests */

    /* test validate float array x1/tc1 (1-byte floats) */
    a = new_nativearray_from(&x1, CLASSID_ARRAY_FLOAT,  
         sizeof(float), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate float array x2/tc1 (1-byte floats) */
    a = new_nativearray_from(&x2, CLASSID_ARRAY_FLOAT,  
           sizeof(float), numdimensions, dim0count, dim1count, dim2count);  
 
    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


   /* test validate float array x3/tc1 (1-byte floats) */
    a = new_nativearray_from(&x3, CLASSID_ARRAY_FLOAT,  
           sizeof(float), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate float array x4/tc1 (1-byte floats) */
    a = new_nativearray_from(&x4, CLASSID_ARRAY_FLOAT,  
           sizeof(float), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate float array x5/tc1 (1-byte floats) */
    a = new_nativearray_from(&x5, CLASSID_ARRAY_FLOAT,  
           sizeof(float), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/*
 * test_array_double()
 * test validator on arrays of double using the recursive test
 */
void test_array_double(void)  
{
    const int numdimensions = 3, dim0count = 4, dim1count = 3, dim2count = 2;
    etch_nativearray* a = NULL;

    double x1[2][3][4] = 
      { { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
        { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
      };
    double x2[2][3][4] = 
      { { { 1,1,1,1, }, { 1,1,1,1, }, { 1,1,1,1, }, },
        { { 1,1,1,1, }, { 1,1,1,1, }, { 1,1,1,1, }, },
      };
    double x3[2][3][4] = 
      { { { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, },
        { { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, { -1,-1,-1,-1, }, },
      };
    double x4[2][3][4] = 
      { { { ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE, }, 
          { ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE, }, 
          { ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE, }, 
        },
        { { ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE, }, 
          { ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE, }, 
          { ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE,ETCHTYPE_MAX_DOUBLE, }, 
        },
      };
    double x5[2][3][4] = 
      { { { ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE, }, 
          { ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE, }, 
          { ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE, }, 
        },
        { { ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE, }, 
          { ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE, }, 
          { ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE,ETCHTYPE_MIN_DOUBLE, }, 
        },
      };

    byte typecodz1[4] = { ETCH_XTRNL_TYPECODE_DOUBLE, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };

    etch_validator* vtor = etchvtor_double_get(numdimensions); /* vtor for all tests */

    /* test validate double array x1/tc1 (1-byte doubles) */
    a = new_nativearray_from(&x1, CLASSID_ARRAY_DOUBLE,  
         sizeof(double), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate double array x2/tc1 (1-byte doubles) */
    a = new_nativearray_from(&x2, CLASSID_ARRAY_DOUBLE,  
           sizeof(double), numdimensions, dim0count, dim1count, dim2count);  
 
    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


   /* test validate double array x3/tc1 (1-byte doubles) */
    a = new_nativearray_from(&x3, CLASSID_ARRAY_DOUBLE,  
           sizeof(double), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate double array x4/tc1 (1-byte doubles) */
    a = new_nativearray_from(&x4, CLASSID_ARRAY_DOUBLE,  
           sizeof(double), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate double array x5/tc1 (1-byte doubles) */
    a = new_nativearray_from(&x5, CLASSID_ARRAY_DOUBLE,  
           sizeof(double), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/*
 * test_array_string()
 * test validator on arrays of etch_string* using the recursive test
 */
void test_array_string(void)  
{
    const int numdimensions = 3, dim0count = 4, dim1count = 3, dim2count = 2;
    etch_nativearray* a = NULL;
    int i = 0, j = 0, k = 0;
    const wchar_t* emptystring = L"";
    const wchar_t* nonemptystring = L"abc";

    etch_string* x1[2][3][4] = 
      { { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
        { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
      };

    etch_string* x2[2][3][4] = 
      { { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
        { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
      };

    byte typecodz1[4] = { ETCH_XTRNL_TYPECODE_EMPTY_STRING, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };

    byte typecodz2[4] = { ETCH_XTRNL_TYPECODE_STRING,
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };

    etch_validator* vtor = etchvtor_string_get(numdimensions); /* vtor for all tests */

    for(i=0; i < dim2count; i++) /* populate test data arrays */
    for(j=0; j < dim1count; j++)  
    for(k=0; k < dim0count; k++)  
    {
        x1[i][j][k] = new_string(emptystring, ETCH_ENCODING_UTF16);
        x2[i][j][k] = new_string(nonemptystring, ETCH_ENCODING_UTF16);
    }

    /* test validate etch_string* array x1/tc1 (empty string) */
    a = new_nativearray_from(&x1, CLASSID_ARRAY_STRING,  
         sizeof(etch_string*), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (objmask*) a);


    /* test validate etch_string* array x2/tc2 (nonempty string) */
    a = new_nativearray_from(&x2, CLASSID_ARRAY_STRING,  
         sizeof(etch_string*), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (objmask*) a);


    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/*
 * test_array_object()
 * test validator on arrays of etch_object* using the recursive test
 */
void test_array_object(void)  
{
    const int numdimensions = 3, dim0count = 4, dim1count = 3, dim2count = 2;
    etch_nativearray* a = NULL;
    etch_object* obj = NULL;
    int i = 0, j = 0, k = 0;
    int sequence = 0;

    etch_object* x1[2][3][4] = 
      { { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
        { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
      };

    etch_object* x2[2][3][4] = 
      { { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
        { { 0,0,0,0, }, { 0,0,0,0, }, { 0,0,0,0, }, },
      };

    byte typecodz[4] = { ETCH_XTRNL_TYPECODE_ANY, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY};

    etch_validator* vtor = etchvtor_object_get(numdimensions); /* vtor for all tests */

    for(i=0; i < dim2count; i++) /* populate test data arrays */
    for(j=0; j < dim1count; j++)  
    for(k=0; k < dim0count; k++)  
    {
        sequence = i*j*k; 
        obj = new_etch_object(CLASSID_OBJECT, new_int32(sequence));
        obj->is_value_owned  = TRUE;  /* object dtor will destroy content */
        obj->is_value_object = TRUE;
        x1[i][j][k] = obj;

        obj = new_etch_object(CLASSID_OBJECT, &sequence);
        obj->is_value_owned  = FALSE; /* object dtor will not destroy content */
        obj->is_value_object = FALSE;
        x2[i][j][k] = obj;
    }

    /* test validate etch_object* array x1/tc (object owning object content) */
    a = new_nativearray_from(&x1, CLASSID_ARRAY_OBJECT,  
         sizeof(etch_object*), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz, (objmask*) a);


    /* test validate etch_object* array x2/tc (object not owning its content) */
    a = new_nativearray_from(&x2, CLASSID_ARRAY_OBJECT,  
         sizeof(etch_object*), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz, (objmask*) a);


    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
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

    etch_structvalue* sv = new_structvalue(typeabc, 0);
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

    etch_structvalue* sv = new_structvalue(type_expected, 0);
    etch_structvalue* svbogus = new_structvalue(type_unexpect, 0);
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
 * test_combo_validator_1()
 */
void test_combo_validator_1(void)  
{
    int result = 0; 
    etch_validator *vtor1 = 0, *vtor2 = 0, *vtor3 = 0;
    create_testobjects();

    /* one cached validator */
    vtor1  = etchvtor_boolean_get(0);
    vtor2  = NULL;
    vtor3  = new_combo_validator(vtor1, vtor2);
    result = is_etch_combo_validator(vtor3);
    CU_ASSERT_EQUAL_FATAL(result, TRUE);
    result = vtor3->validate(vtor3, (etch_object*) objbool_true);
    CU_ASSERT_EQUAL(result,0);
    result = vtor3->destroy(vtor3);
    CU_ASSERT_EQUAL(result, 0);

    /* one cached validator */
    vtor1  = NULL;
    vtor2  = etchvtor_boolean_get(0);
    vtor3  = new_combo_validator(vtor1, vtor2);
    result = vtor3->validate(vtor3, (etch_object*) objbool_true);
    CU_ASSERT_EQUAL(result,0);
    result = vtor3->destroy(vtor3);
    CU_ASSERT_EQUAL(result, 0);

    /* two cached validators */
    vtor1  = etchvtor_boolean_get(0);
    vtor2  = etchvtor_int32_get(0);
    vtor3  = new_combo_validator(vtor1, vtor2);
    result = vtor3->validate(vtor3, (etch_object*) objint_three);
    CU_ASSERT_EQUAL(result,0);
    result = vtor3->destroy(vtor3);
    CU_ASSERT_EQUAL(result, 0);

    destroy_testobjects(); 
    etchvtor_clear_cache();  /* destroy cached validators */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/**
 * test_combo_validator_2()
 */
void test_combo_validator_2(void)  
{
    int result = 0; 
    etch_validator *vtor1 = 0, *vtor2 = 0, *vtor3 = 0, *vtor4 = 0, *vtor5 = 0;
    create_testobjects();

    /* chain 1 of cached validators and combos */
    vtor1  = etchvtor_boolean_get(0);
    vtor2  = etchvtor_boolean_get(0);
    vtor3  = new_combo_validator(vtor1, vtor2);
    vtor4  = etchvtor_boolean_get(0);
    vtor5  = new_combo_validator(vtor4, vtor3);
    result = vtor5->validate(vtor5, (etch_object*) objbool_true);
    CU_ASSERT_EQUAL(result,0);
    result = vtor5->destroy(vtor5);
    CU_ASSERT_EQUAL(result, 0);

    /* chain 2 of cached validators and combos */
    vtor1  = etchvtor_boolean_get(0);
    vtor2  = etchvtor_boolean_get(0);
    vtor3  = new_combo_validator(vtor1, vtor2);
    vtor4  = etchvtor_boolean_get(0);
    vtor5  = new_combo_validator(vtor3, vtor4);
    result = vtor5->validate(vtor5, (etch_object*) objbool_true);
    CU_ASSERT_EQUAL(result,0);
    result = vtor5->destroy(vtor5);
    CU_ASSERT_EQUAL(result, 0);

    destroy_testobjects(); 
    etchvtor_clear_cache();  /* destroy cached validators */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */
}


/**
 * test_combo_validator_3()
 */
void test_combo_validator_3(void)  
{
    int result = 0; 
    etch_validator *vtor1 = 0, *vtor2 = 0, *vtor3 = 0, *vtor4 = 0, *vtor5 = 0;
    create_testobjects();

    /* chain 1 of cached and non-cached validators and combos */
    vtor1  = etchvtor_boolean_get(0);
    vtor2  = etchvtor_int32_get(0);
    vtor3  = new_combo_validator(vtor1, vtor2);
    vtor4  = etchvtor_struct_get(testtype, 0);
    vtor5  = new_combo_validator(vtor4, vtor3);
    result = vtor5->validate(vtor5, (etch_object*) objint_three);
    CU_ASSERT_EQUAL(result, 0); 
    result = vtor5->destroy(vtor5);
    CU_ASSERT_EQUAL(result, 0);

    /* chain 2 of cached and non-cached validators and combos */
    vtor1  = etchvtor_int32_get(0);
    vtor2  = etchvtor_boolean_get(0);
    vtor3  = new_combo_validator(vtor1, vtor2);
    vtor4  = etchvtor_struct_get(testtype, 0);  
    vtor5  = new_combo_validator(vtor3, vtor4);
    result = vtor5->validate(vtor5, (etch_object*) objint_three);
    CU_ASSERT_EQUAL(result, 0);
    result = vtor5->destroy(vtor5);
    CU_ASSERT_EQUAL(result, 0);

    destroy_testobjects(); 
    etchvtor_clear_cache();  /* destroy cached validators */

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
    CU_set_output_filename("../test_validator");
    ps = CU_add_suite("validator test suite", init_suite, clean_suite);
    etch_watch_id = 0; 

    CU_add_test(ps, "test boolean validator", test_boolean_validator);
    CU_add_test(ps, "test validate boolean arrays", test_array_boolean);
    CU_add_test(ps, "test validate byte arrays", test_array_byte);
    CU_add_test(ps, "test validate int16 arrays", test_array_int16);
    CU_add_test(ps, "test validate int32 arrays", test_array_int32);
    CU_add_test(ps, "test validate int64 arrays", test_array_int64);
    CU_add_test(ps, "test validate float arrays", test_array_float);
    CU_add_test(ps, "test validate double arrays", test_array_double);
    CU_add_test(ps, "test validate etch_string* arrays", test_array_string);
    CU_add_test(ps, "test validate etch_object* arrays", test_array_object);
    CU_add_test(ps, "test struct element validator", test_struct_element_validator);   
    CU_add_test(ps, "test struct good values", test_struct_good_values); 
    CU_add_test(ps, "test struct bad values",  test_struct_bad_values);  
    CU_add_test(ps, "test combo validator 1",  test_combo_validator_1); 
    CU_add_test(ps, "test combo validator 2",  test_combo_validator_2); 
    CU_add_test(ps, "test combo validator 3",  test_combo_validator_3);         
    

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