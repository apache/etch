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
#include "etch_runtime.h"
#include "etch_validator.h"
#include "etch_tagged_data.h"
#include "etch_nativearray.h"
#include "etch_structval.h"
#include "etch_objecttypes.h"

#include <stdio.h>
#include "CUnit.h"
#include "Basic.h"
#include "Automated.h"

#define IS_DEBUG_CONSOLE FALSE

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
    etch_runtime_shutdown();
    return 0;
}

static etch_type*    testtype;
static etch_boolean* objbool_false;
static etch_boolean* objbool_true;
static etch_byte*    objbyte_one;
static etch_int16*   objshort_two;
static etch_int32*   objint_three;
static etch_int64*   objlong_four;
static etch_float*   objfloat_five;
static etch_double*  objdouble_six;
static etch_string*  objstring_abc;
static etch_object*  objobject_null;
static etch_nativearray* arraydim1;
static etch_nativearray* arraydim2;
static etch_nativearray* arraydim3;


static void create_testobjects()
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
    objstring_abc  = new_stringw(L"abc");
    objobject_null = (etch_object*) new_primitive(sizeof(etch_object), CLASSID_NONE);
    arraydim1  = new_etch_nativearray(CLASSID_ARRAY_BOOL, 1, 1, 4, 0, 0);
    arraydim2  = new_etch_nativearray(CLASSID_ARRAY_BOOL, 1, 2, 4, 3, 0);
    arraydim3  = new_etch_nativearray(CLASSID_ARRAY_BOOL, 1, 3, 4, 3, 2);
}


static void destroy_testobjects()
{
    etch_object_destroy(testtype); testtype = NULL;
    etch_object_destroy(objbool_false); objbool_false = NULL;
    etch_object_destroy(objbool_true);  objbool_true  = NULL;
    etch_object_destroy(objbyte_one);   objbyte_one   = NULL;
    etch_object_destroy(objshort_two);  objshort_two  = NULL;
    etch_object_destroy(objint_three);  objint_three  = NULL;
    etch_object_destroy(objlong_four);  objlong_four  = NULL;
    etch_object_destroy(objfloat_five); objfloat_five = NULL;
    etch_object_destroy(objdouble_six); objdouble_six = NULL;
    etch_object_destroy(objstring_abc); objstring_abc = NULL;
    etch_object_destroy(objobject_null);objobject_null= NULL;
    etch_object_destroy(arraydim1);  arraydim1 = NULL;
    etch_object_destroy(arraydim2);  arraydim2 = NULL;
    etch_object_destroy(arraydim3);  arraydim3 = NULL;
}


/**
 * do_boolean_test()
 * common test template for the boolean validator tests
 */
static void do_boolean_test(int dims, char* name, byte in_typecode, 
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
    etch_object_destroy(vtor); /* test that vtor is cached */
    CU_ASSERT_EQUAL(result, -1);  /* and cannot be destroyed */ 
}


/**
 * test_boolean_validator()
 */
static void test_boolean_validator(void)  
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

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/*
 * do_recursive_test()
 * run a validate on an anonymous object. if the object is a nativearray, 
 * this method is invoked recursively until the array elements are validated.
 * the caller relinquishes ownership of valobj to this method.
 */
static void do_recursive_test(etch_validator* vtor, const int dims, byte* typecodes, etch_object* valobj)
{
    int  result = 0;
    signed char expected_typecode = 0, validated_typecode = 0;
    //++debug_count;
    //if (debug_count == 0x28)
    //    result = 0; // debugger target    

    result = vtor->validate(vtor, (etch_object*) valobj);
    CU_ASSERT_EQUAL(result, 0);

    expected_typecode = typecodes[dims];
    result = vtor->check_value(vtor, (etch_object*) valobj, (unsigned char*)&validated_typecode);
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
            etch_object* subobj = etch_nativearray_get_element(a, i); 

            CU_ASSERT_PTR_NOT_NULL_FATAL(subobj);
             
            do_recursive_test(element_vtor, dims-1, typecodes, subobj);

        } 

        etch_object_destroy(valobj);
    }
    
}


/*
 * test_array_boolean()
 * test validator on arrays of boolean using the recursive test
 */
static void test_array_boolean(void)  
{
    const int numdimensions = 2, dim0count = 3, dim1count = 2;
    boolean x1[2][3] = { { FALSE,FALSE,FALSE,}, { FALSE,FALSE,FALSE,}, };
    boolean x2[2][3] = { { TRUE, TRUE, TRUE, }, { TRUE, TRUE, TRUE, }, };
    byte typecodz[3] = { ETCH_XTRNL_TYPECODE_BOOLEAN_FALSE, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };
    etch_nativearray* a1 = NULL, *a2 = NULL;

    /* test validate boolean array x1/a1 */
    etch_validator* vtor = etchvtor_boolean_get(numdimensions);

	
    a1 = new_etch_nativearray_from(&x1, CLASSID_ARRAY_BOOL,  
        sizeof(boolean), numdimensions, dim0count, dim1count, 0);  
	a1->is_content_owned = FALSE;
    /* fyi we relinquish ownership of array a1 here */
    do_recursive_test(vtor, numdimensions, typecodz, (etch_object*) a1);

    /* test validate boolean array x2/a2 */
    typecodz[0] = ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE; 
    vtor = etchvtor_boolean_get(numdimensions); 

    a2 = new_etch_nativearray_from(&x2, CLASSID_ARRAY_BOOL,  
        sizeof(boolean), numdimensions, dim0count, dim1count, 0);  

	a2->is_content_owned = FALSE;
    /* fyi we relinquish ownership of array a2 here */
    do_recursive_test(vtor, numdimensions, typecodz, (etch_object*) a2);

    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/*
 * test_array_byte()
 * test validator on arrays of byte using the recursive test
 */
static void test_array_byte(void)  
{
    const int numdimensions = 2, dim0count = 3, dim1count = 2;
    byte x1[2][3] = { { 'a',  'b',  'c', }, { 'd',  'e',  'f',}, };
    byte x2[2][3] = { { ETCHTYPE_MIN_BYTE,0xff,0},{ ETCHTYPE_MAX_BYTE,0xff,0, }, };
    byte typecodz[3] = { ETCH_XTRNL_TYPECODE_BYTE, 
        ETCH_XTRNL_TYPECODE_BYTES, ETCH_XTRNL_TYPECODE_ARRAY };
    etch_nativearray* a1 = NULL, *a2 = NULL;

    /* test validate byte array x1/a1 */
    etch_validator* vtor = etchvtor_byte_get(numdimensions);

    a1 = new_etch_nativearray_from(&x1, CLASSID_ARRAY_BYTE,  
        sizeof(byte), numdimensions, dim0count, dim1count, 0);  

    /* fyi we relinquish ownership of array a1 here */
    do_recursive_test(vtor, numdimensions, typecodz, (etch_object*) a1);

    /* test validate byte array x2/a2 */
    vtor = etchvtor_byte_get(numdimensions); 

    a2 = new_etch_nativearray_from(&x2, CLASSID_ARRAY_BYTE,  
        sizeof(byte), numdimensions, dim0count, dim1count, 0);  

    /* fyi we relinquish ownership of array a2 here */
    do_recursive_test(vtor, numdimensions, typecodz, (etch_object*) a2);

    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/*
 * test_array_int16()
 * test validator on arrays of short using the recursive test
 */
static void test_array_int16(void)  
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
    a = new_etch_nativearray_from(&x1, CLASSID_ARRAY_INT16,  
         sizeof(short), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate short array x2/tc1 (1-byte shorts) */
    a = new_etch_nativearray_from(&x2, CLASSID_ARRAY_INT16,  
           sizeof(short), numdimensions, dim0count, dim1count, dim2count);  
 
    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


   /* test validate short array x3/tc1 (1-byte shorts) */
    a = new_etch_nativearray_from(&x3, CLASSID_ARRAY_INT16,  
           sizeof(short), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate short array x4/tc1 (1-byte shorts) */
    a = new_etch_nativearray_from(&x4, CLASSID_ARRAY_INT16,  
           sizeof(short), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate short array x5/tc1 (1-byte shorts) */
    a = new_etch_nativearray_from(&x5, CLASSID_ARRAY_INT16,  
           sizeof(short), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate short array x6/tc2 (2-byte shorts) */
    a = new_etch_nativearray_from(&x6, CLASSID_ARRAY_INT16,  
           sizeof(short), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (etch_object*) a);


    /* test validate short array x7/tc2 (2-byte shorts) */
    a = new_etch_nativearray_from(&x7, CLASSID_ARRAY_INT16,  
           sizeof(short), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (etch_object*) a);


    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/*
 * test_array_int32()
 * test validator on arrays of int using the recursive test
 */
static void test_array_int32(void)  
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

    byte typecodz1[4] = { ETCH_XTRNL_TYPECODE_BYTE, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };

    byte typecodz2[4] = { ETCH_XTRNL_TYPECODE_SHORT, 
        ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY, ETCH_XTRNL_TYPECODE_ARRAY };


    /* test validate int array x1/tc1 (1-byte ints) */
    etch_validator* vtor = etchvtor_int32_get(numdimensions);  /* allocates object */

    a = new_etch_nativearray_from(&x1, CLASSID_ARRAY_INT32,  
         sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate int array x2/tc1 (1-byte ints) */
    vtor = etchvtor_int32_get(numdimensions);  /* gets cached object */

    a = new_etch_nativearray_from(&x2, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  
 
    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate int array x3/tc1 (1-byte ints) */
    a = new_etch_nativearray_from(&x3, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate int array x4/tc1 (1-byte ints) */
    a = new_etch_nativearray_from(&x4, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate int array x5/tc1 (1-byte ints) */
    a = new_etch_nativearray_from(&x5, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate int array x6/tc2 (2-byte ints) */
    a = new_etch_nativearray_from(&x6, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (etch_object*) a);


    /* test validate int array x7/tc2 (2-byte ints) */
    a = new_etch_nativearray_from(&x7, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (etch_object*) a);


    /* test validate int array x8/tc4 (4-byte ints) */
    a = new_etch_nativearray_from(&x6, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (etch_object*) a);


    /* test validate int array x9/tc4 (4-byte ints) */
    a = new_etch_nativearray_from(&x7, CLASSID_ARRAY_INT32,  
           sizeof(int), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (etch_object*) a);


    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/*
 * test_array_int64()
 * test validator on arrays of long long using the recursive test
 */
static void test_array_int64(void)  
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

    a = new_etch_nativearray_from(&x1, CLASSID_ARRAY_INT64,  
         sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate int64 array x2/tc1 (1-byte longs) */
    vtor = etchvtor_int64_get(numdimensions);  /* gets cached object */

    a = new_etch_nativearray_from(&x2, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  
 
    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate int64 array x3/tc1 (1-byte longs) */
    a = new_etch_nativearray_from(&x3, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate int64 array x4/tc1 (1-byte longs) */
    a = new_etch_nativearray_from(&x4, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate int64 array x5/tc1 (1-byte longs) */
    a = new_etch_nativearray_from(&x5, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate int64 array x6/tc2 (2-byte longs) */
    a = new_etch_nativearray_from(&x6, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (etch_object*) a);


    /* test validate int64 array x7/tc2 (2-byte longs) */
    a = new_etch_nativearray_from(&x7, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (etch_object*) a);


    /* test validate int64 array x8/tc4 (4-byte longs) */
    a = new_etch_nativearray_from(&x8, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz4, (etch_object*) a);


    /* test validate int64 array x9/tc4 (4-byte longs) */
    a = new_etch_nativearray_from(&x9, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz4, (etch_object*) a);


    /* test validate int64 array x10/tc8 (8-byte longs) */
    a = new_etch_nativearray_from(&x10, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz8, (etch_object*) a);


    /* test validate int64 array x11/tc8 (8-byte longs) */
    a = new_etch_nativearray_from(&x11, CLASSID_ARRAY_INT64,  
           sizeof(int64), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz8, (etch_object*) a);


    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/*
 * test_array_float()
 * test validator on arrays of float using the recursive test
 */
static void test_array_float(void)  
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
    a = new_etch_nativearray_from(&x1, CLASSID_ARRAY_FLOAT,  
         sizeof(float), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate float array x2/tc1 (1-byte floats) */
    a = new_etch_nativearray_from(&x2, CLASSID_ARRAY_FLOAT,  
           sizeof(float), numdimensions, dim0count, dim1count, dim2count);  
 
    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


   /* test validate float array x3/tc1 (1-byte floats) */
    a = new_etch_nativearray_from(&x3, CLASSID_ARRAY_FLOAT,  
           sizeof(float), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate float array x4/tc1 (1-byte floats) */
    a = new_etch_nativearray_from(&x4, CLASSID_ARRAY_FLOAT,  
           sizeof(float), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate float array x5/tc1 (1-byte floats) */
    a = new_etch_nativearray_from(&x5, CLASSID_ARRAY_FLOAT,  
           sizeof(float), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/*
 * test_array_double()
 * test validator on arrays of double using the recursive test
 */
static void test_array_double(void)  
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
    a = new_etch_nativearray_from(&x1, CLASSID_ARRAY_DOUBLE,  
         sizeof(double), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate double array x2/tc1 (1-byte doubles) */
    a = new_etch_nativearray_from(&x2, CLASSID_ARRAY_DOUBLE,  
           sizeof(double), numdimensions, dim0count, dim1count, dim2count);  
 
    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


   /* test validate double array x3/tc1 (1-byte doubles) */
    a = new_etch_nativearray_from(&x3, CLASSID_ARRAY_DOUBLE,  
           sizeof(double), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate double array x4/tc1 (1-byte doubles) */
    a = new_etch_nativearray_from(&x4, CLASSID_ARRAY_DOUBLE,  
           sizeof(double), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate double array x5/tc1 (1-byte doubles) */
    a = new_etch_nativearray_from(&x5, CLASSID_ARRAY_DOUBLE,  
           sizeof(double), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/*
 * test_array_string()
 * test validator on arrays of etch_string* using the recursive test
 */
static void test_array_string(void)  
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
        x1[i][j][k] = new_stringw(emptystring);
        x2[i][j][k] = new_stringw(nonemptystring);
    }

    /* test validate etch_string* array x1/tc1 (empty string) */
    a = new_etch_nativearray_from(&x1, CLASSID_ARRAY_STRING,  
         sizeof(etch_string*), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz1, (etch_object*) a);


    /* test validate etch_string* array x2/tc2 (nonempty string) */
    a = new_etch_nativearray_from(&x2, CLASSID_ARRAY_STRING,  
         sizeof(etch_string*), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz2, (etch_object*) a);


    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/*
 * test_array_object()
 * test validator on arrays of etch_object* using the recursive test
 */
static void test_array_object(void)  
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
        obj = (etch_object*)new_int32(sequence);
        x1[i][j][k] = obj;

        obj = (etch_object*)&sequence;
        x2[i][j][k] = obj;
    }

    /* test validate etch_object* array x1/tc (object owning object content) */
    a = new_etch_nativearray_from(&x1, CLASSID_ARRAY_OBJECT,  
         sizeof(etch_object*), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz, (etch_object*) a);


    /* test validate etch_object* array x2/tc (object not owning its content) */
    a = new_etch_nativearray_from(&x2, CLASSID_ARRAY_OBJECT,  
         sizeof(etch_object*), numdimensions, dim0count, dim1count, dim2count);  

    do_recursive_test(vtor, numdimensions, typecodz, (etch_object*) a);


    /* done */
    etchvtor_clear_cache();  /* destroy cached validator */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}



/**
 * do_struct_element_vtor_test()
 * common test template for the structvalue element_validator tests
 */
static void do_struct_element_vtor_test(int testdims, char* name, unsigned short expected_class_id, etch_type* thistype)
{
    etch_validator* vtor = etchvtor_struct_get(thistype, testdims);

    CU_ASSERT_PTR_NOT_NULL_FATAL(vtor); 
    CU_ASSERT_EQUAL(vtor->numdimensions, testdims); 
    CU_ASSERT_EQUAL(vtor->expected_class_id, expected_class_id); 
    CU_ASSERT_STRING_EQUAL(vtor->description, name); 
    CU_ASSERT_EQUAL(is_equal_types(vtor->struct_type, thistype), TRUE);

    /* validator destructor is benign if validator is marked cached
     * however a struct validator is not cached */
    etch_object_destroy(vtor);
}


/**
 * test_struct_element_validator()
 */
static void test_struct_element_validator(void)  
{
    etch_type* typeabc = new_type(L"abc");

    do_struct_element_vtor_test(0, "struct_abc[0]", CLASSID_STRUCTVALUE,  typeabc);
    do_struct_element_vtor_test(1, "struct_abc[1]", CLASSID_ARRAY_OBJECT, typeabc);
    do_struct_element_vtor_test(2, "struct_abc[2]", CLASSID_ARRAY_OBJECT, typeabc);
    do_struct_element_vtor_test(3, "struct_abc[3]", CLASSID_ARRAY_OBJECT, typeabc);

    etch_object_destroy(typeabc);
    etchvtor_clear_cache();    /* not needed since struct validator not cached  */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * do_struct_goodvalue_test()
 */
static void do_struct_goodvalue_test(int testdims, const byte expected_typecode, 
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
        etch_object_destroy(validated_value);       

    /* validator destructor is benign if validator is marked cached
     * however a struct validator is not cached */
    etch_object_destroy(vtor);      
}



/**
 * do_struct_badvalue_test()
 */
static void do_struct_badvalue_test(int testdims, etch_object* value, etch_type* thistype)
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
        etch_object_destroy(validated_value);       

    /* validator destructor is benign if validator is marked cached
     * however a struct validator is not cached */
    etch_object_destroy(vtor);      
}


/**
 * test_struct_good_values()
 */
static void test_struct_good_values(void)  
{
    etch_type* typeabc = new_type(L"abc");
    etch_nativearray* array_of_struct = NULL;
    const int NUMDIMS0 = 0, NUMDIMS1 = 1, NUMDIMS2 = 2, DIMSIZE4 = 4;

    etch_structvalue* sv = new_structvalue(typeabc, 0);
    do_struct_goodvalue_test(NUMDIMS0, ETCH_XTRNL_TYPECODE_CUSTOM, (etch_object*) sv, typeabc, FALSE);

    /* following tests create an empty array of structvalue objects */
    array_of_struct = new_etch_nativearray_of(((etch_object*)sv)->obj_type, ((etch_object*)sv)->class_id, NUMDIMS1, DIMSIZE4, 0, 0);
    do_struct_goodvalue_test(NUMDIMS1, ETCH_XTRNL_TYPECODE_ARRAY, (etch_object*) array_of_struct, typeabc, FALSE);
    etch_object_destroy(array_of_struct);

    array_of_struct = new_etch_nativearray_of(((etch_object*)sv)->obj_type, ((etch_object*)sv)->class_id, NUMDIMS2, DIMSIZE4, DIMSIZE4, 0);
    do_struct_goodvalue_test(NUMDIMS2, ETCH_XTRNL_TYPECODE_ARRAY, (etch_object*) array_of_struct, typeabc, FALSE);
    etch_object_destroy(array_of_struct);

    etch_object_destroy(sv);          
    etch_object_destroy(typeabc); /* in practice this would have been a static type */
    etchvtor_clear_cache();    /* not needed since struct validator not cached  */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_struct_bad_values()  
 */
static void test_struct_bad_values(void)  
{
    etch_type* type_expected = new_type(L"abc");
    etch_type* type_unexpect = new_type(L"def");
    etch_nativearray* array_of_struct = NULL;
    const int NUMDIMS0 = 0, NUMDIMS1 = 1, NUMDIMS2 = 2, NUMDIMS3 = 3, DIMSIZE4 = 4;

    etch_structvalue* sv = new_structvalue(type_expected, 0);
    etch_structvalue* svbogus = new_structvalue(type_unexpect, 0);
    etch_int32*  intobj  = new_int32(1);
    etch_object* objobj  = (etch_object*)intobj;

    do_struct_badvalue_test(0, (etch_object*) intobj, type_expected);
    do_struct_badvalue_test(0, objobj,  type_expected);
    do_struct_badvalue_test(0, (etch_object*) svbogus, type_expected);
    do_struct_badvalue_test(0, (etch_object*) sv, type_unexpect);

    do_struct_badvalue_test(1, (etch_object*) intobj, type_expected);
    do_struct_badvalue_test(1, objobj, type_expected);
    do_struct_badvalue_test(1, (etch_object*) svbogus, type_expected);
    do_struct_badvalue_test(1, (etch_object*) sv, type_unexpect);

    /* following tests create an empty array of structvalue objects */
    array_of_struct = new_etch_nativearray_of(((etch_object*)sv)->obj_type, ((etch_object*)sv)->class_id, NUMDIMS1, DIMSIZE4, 0, 0);
    do_struct_badvalue_test(NUMDIMS0, (etch_object*) array_of_struct, type_expected);
    etch_object_destroy(array_of_struct);

    array_of_struct = new_etch_nativearray_of(((etch_object*)sv)->obj_type, ((etch_object*)sv)->class_id, NUMDIMS2, DIMSIZE4, DIMSIZE4, 0);
    do_struct_badvalue_test(NUMDIMS1, (etch_object*) array_of_struct, type_expected);
    do_struct_badvalue_test(NUMDIMS0, (etch_object*) array_of_struct, type_expected);
    etch_object_destroy(array_of_struct);

    array_of_struct = new_etch_nativearray_of(((etch_object*)sv)->obj_type, ((etch_object*)sv)->class_id, NUMDIMS3, DIMSIZE4, DIMSIZE4, DIMSIZE4);
    do_struct_badvalue_test(NUMDIMS2, (etch_object*) array_of_struct, type_expected);
    do_struct_badvalue_test(NUMDIMS1, (etch_object*) array_of_struct, type_expected);
    do_struct_badvalue_test(NUMDIMS0, (etch_object*) array_of_struct, type_expected);
    etch_object_destroy(array_of_struct);
  
    etch_object_destroy(objobj);
    etch_object_destroy(sv);   
    etch_object_destroy(svbogus);       
    etch_object_destroy(type_expected); /* in real world these are static types */
    etch_object_destroy(type_unexpect);  
    etchvtor_clear_cache();  /* however struct validator is not cached */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_combo_validator_1()
 */
static void test_combo_validator_1(void)  
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
    etch_object_destroy(vtor3);

    /* one cached validator */
    vtor1  = NULL;
    vtor2  = etchvtor_boolean_get(0);
    vtor3  = new_combo_validator(vtor1, vtor2);
    result = vtor3->validate(vtor3, (etch_object*) objbool_true);
    CU_ASSERT_EQUAL(result,0);
    etch_object_destroy(vtor3);

    /* two cached validators */
    vtor1  = etchvtor_boolean_get(0);
    vtor2  = etchvtor_int32_get(0);
    vtor3  = new_combo_validator(vtor1, vtor2);
    result = vtor3->validate(vtor3, (etch_object*) objint_three);
    CU_ASSERT_EQUAL(result,0);
    etch_object_destroy(vtor3);

    destroy_testobjects(); 
    etchvtor_clear_cache();  /* destroy cached validators */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_combo_validator_2()
 */
static void test_combo_validator_2(void)  
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
    etch_object_destroy(vtor5);

    /* chain 2 of cached validators and combos */
    vtor1  = etchvtor_boolean_get(0);
    vtor2  = etchvtor_boolean_get(0);
    vtor3  = new_combo_validator(vtor1, vtor2);
    vtor4  = etchvtor_boolean_get(0);
    vtor5  = new_combo_validator(vtor3, vtor4);
    result = vtor5->validate(vtor5, (etch_object*) objbool_true);
    CU_ASSERT_EQUAL(result,0);
    etch_object_destroy(vtor5);

    destroy_testobjects(); 
    etchvtor_clear_cache();  /* destroy cached validators */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_combo_validator_3()
 */
static void test_combo_validator_3(void)  
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
    etch_object_destroy(vtor5);

    /* chain 2 of cached and non-cached validators and combos */
    vtor1  = etchvtor_int32_get(0);
    vtor2  = etchvtor_boolean_get(0);
    vtor3  = new_combo_validator(vtor1, vtor2);
    vtor4  = etchvtor_struct_get(testtype, 0);  
    vtor5  = new_combo_validator(vtor3, vtor4);
    result = vtor5->validate(vtor5, (etch_object*) objint_three);
    CU_ASSERT_EQUAL(result, 0);
    etch_object_destroy(vtor5);

    destroy_testobjects(); 
    etchvtor_clear_cache();  /* destroy cached validators */

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
CU_pSuite test_etch_validator_suite()
{
    CU_pSuite ps = CU_add_suite("validator test suite", init_suite, clean_suite);

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

    return ps;
}
