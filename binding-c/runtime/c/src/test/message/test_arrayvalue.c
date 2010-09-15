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
 * test_arrayvalue.c -- test etch_arrayvalue object
 */
#include "etch_runtime.h"
#include "etch_tagdata_inp.h"
#include "etch_arrayval.h"
#include "etch_exception.h"
#include "etch_nativearray.h"
#include "etch_objecttypes.h"
#include "etch_mem.h"
#include <wchar.h>

#include <stdio.h>
#include "CUnit.h"

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
    etch_runtime_shutdown();
    return 0;
}

#define OBJTYPE_FAKETDI_IMPL ETCHTYPEB_INSTANCEDATA
#define CLASSID_FAKETDI_IMPL 0xf0
#define OBJTYPE_FAKETDO_IMPL ETCHTYPEB_INSTANCEDATA
#define CLASSID_FAKETDO_IMPL 0xf1
#define OBJTYPE_FAKETDI_VTABLE 0x88
#define OBJTYPE_FAKETDO_VTABLE 0x89

#define EXCPTEST_UNCHECKED_STATICTEXT 1
#define EXCPTEST_CHECKED_COPYTEXT     2
#define EXCPTEST_CHECKED_STATICTEXT   3

#define IS_DEBUG_CONSOLE FALSE  /* TRUE to display test diagnostics */

static etch_arraylist* testdata;
static int g_which_exception_test;


#if(0)

/**
 * fake_tdi_impl
 * instance data for a TDI implementation
 */
typedef struct fake_tdi_impl
{
    etch_object object;

    int index;
    byte started, done, ended;
    etch_arrayvalue* arrray;

} fake_tdi_impl;


/**
 * fake_tdo_impl
 * instance data for a TDO implementation
 */
typedef struct fake_tdo_impl
{
    etch_object object;

    byte started, ended;
    etch_arraylist* list;   
    etch_arrayvalue* arrray;

} fake_tdo_impl;


/**
 * destroy_fake_tdi_impl
 * memory cleanup handler for fake_tdi_impl
 */
static int destroy_fake_tdi_impl(fake_tdi_impl* impl)
{
    int result = verify_object((etch_object*)impl, OBJTYPE_FAKETDI_IMPL, CLASSID_FAKETDI_IMPL, NULL);
    if (result == -1) return -1; /* object passed was not expected object */

    etch_object_destroy(impl->arrray); 

    etch_free(impl);    
    return 0;
}


/**
 * destroy_fake_tdo_impl
 * memory cleanup handler for fake_tdo_impl
 */
static int destroy_fake_tdo_impl(fake_tdo_impl* impl)
{
    etch_destructor destroy = NULL;
    int result = verify_object((etch_object*)impl, OBJTYPE_FAKETDO_IMPL, CLASSID_FAKETDO_IMPL, NULL);
    if (result == -1) return -1; /* object passed was not expected object */

    impl->list->is_readonly = TRUE;
    etch_object_destroy(impl->list);     /* destroy list, but not content */

    impl->arrray->list->is_readonly = FALSE;
    etch_object_destroy(impl->arrray); /* destroy arravalue and content */

    etch_free(impl);    
    return 0;
}


/**
 * new_fake_tdi_impl()
 * constructor for TDI implementation instance data
 */
static fake_tdi_impl* new_fake_tdi_impl()
{
    fake_tdi_impl* data = (fake_tdi_impl*) new_object
        (sizeof(fake_tdi_impl), ETCHTYPEB_INSTANCEDATA, CLASSID_FAKETDI_IMPL);

    data->destroy = destroy_fake_tdi_impl;
    
    return data;
}


/**
 * new_fake_tdo_impl()
 * constructor for TDO implementation instance data
 */
static fake_tdo_impl* new_fake_tdo_impl()
{
    fake_tdo_impl* data = (fake_tdo_impl*) new_object
        (sizeof(fake_tdo_impl), ETCHTYPEB_INSTANCEDATA, CLASSID_FAKETDO_IMPL);
 
    data->destroy = destroy_fake_tdo_impl;
    
    return data;
}


static enum etch_classid CLASSID_VTABLE_FAKETDI = CLASSID_DYNAMIC_START + 0x0;  
static enum etch_classid CLASSID_VTABLE_FAKETDO = CLASSID_DYNAMIC_START + 0x1;


/**
 * faketdi_start_array() overrides tdi_start_array()
 * Starts reading an array from the stream.
 * @return the array that we are reading.
 * @throws IOException if there is a problem with the stream.
 */
static etch_arrayvalue* faketdi_start_array(tagged_data_input* tdi)
{
    int result = 0;
    fake_tdi_impl* data = NULL;
    etch_arrayvalue* newarrayval = NULL;
    const int DIM_ZERO = 0, READONLY = TRUE, DEFSIZE = 0, DEFDELTA = 0;
    byte fake_typecode = 0;
    etch_type* fake_structtype = NULL;
    
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi->impl);

    data = (fake_tdi_impl*) tdi->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDI_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = data->destroy(NULL); /* ensure we can call instance data destructor */
    CU_ASSERT_EQUAL(result,-1);

    CU_ASSERT_EQUAL(data->started,FALSE);
    CU_ASSERT_EQUAL(data->done,FALSE);
    CU_ASSERT_EQUAL(data->ended,FALSE);

    data->started = TRUE;
    data->index = 0;

    /* create instance array */
    newarrayval = new_arrayvalue 
        (fake_typecode, fake_structtype, DIM_ZERO, DEFSIZE, DEFDELTA, READONLY); 

    data->arrray = newarrayval;
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->arrray);

    return data->arrray;
}


/**
 * faketdo_start_array() overrides tdo_start_array()
 */
static etch_arrayvalue* faketdo_start_array(tagged_data_output* tdo, etch_arrayvalue* thisp)
{
    int result = 0;
    fake_tdo_impl* data = NULL;
  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo->impl);

    data = (fake_tdo_impl*) tdo->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDO_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = data->destroy(NULL); /* ensure we can call into instance data destructor */
    CU_ASSERT_EQUAL(result,-1);

    CU_ASSERT_EQUAL(data->started,FALSE);
    CU_ASSERT_EQUAL(data->ended,FALSE);

    data->arrray = thisp;
    data->list = new_arrayvalue_arraylist();
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->arrray);
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->list);

    data->started = TRUE;
    return data->arrray;
}


/**
 * faketdi_read_array_element() overrides tdi_read_array_element()
 */
static int faketdi_read_array_element(tagged_data_input* tdi, ETCH_ARRAY_ELEMENT** out_ae)   
{
    int result = 0;
    fake_tdi_impl* data = NULL;
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi->impl);

    data = (fake_tdi_impl*) tdi->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDI_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    CU_ASSERT_EQUAL(data->started,TRUE);
    CU_ASSERT_EQUAL(data->done,FALSE);
    CU_ASSERT_EQUAL(data->ended,FALSE);

	if (data->index < (int)testdata->count)
	{   
		*out_ae = arraylist_get(testdata, data->index++);
		return TRUE;
	}
	
	data->done = TRUE;
	return FALSE;
}


/**
 * faketdo_write_array_element() overrides tdi_read_array_element()
 */
static int faketdo_write_array_element(tagged_data_output* tdo, etch_object* newitem)   
{
    int result = 0;
    fake_tdo_impl* data = NULL;
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo->impl);

    data = (fake_tdo_impl*) tdo->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDO_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->list);  

    CU_ASSERT_EQUAL(data->started,TRUE);
    CU_ASSERT_EQUAL(data->ended,FALSE);
   
	result = arraylist_add(data->list, newitem);
	return 0;
}


/**
 * faketdi_end_array() overrides tdi_end_array()
 * Ends an array that we are reading.
 * @param array the array that we read.
 * @throws IOException if there is a problem with the stream.
 */
static int faketdi_end_array(tagged_data_input* tdi, etch_arrayvalue* av)
{
    int result = 0;
    fake_tdi_impl* data = NULL;
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi->impl);

    data = (fake_tdi_impl*) tdi->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDI_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->arrray); 
 
    CU_ASSERT_EQUAL(data->started,TRUE);
    CU_ASSERT_EQUAL(data->done,TRUE);
    CU_ASSERT_EQUAL(data->ended,FALSE);
    CU_ASSERT_EQUAL(data->arrray,av);
			
	data->ended = TRUE;
    return 0;
}


/**
 * faketdo_end_array() overrides tdo_end_array()
 */
static int faketdo_end_array(tagged_data_output* tdo, etch_arrayvalue* av)
{
    int result = 0;
    fake_tdo_impl* data = NULL;
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo->impl);

    data = (fake_tdo_impl*) tdo->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDO_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->list); 
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->arrray); 
 
    CU_ASSERT_EQUAL(data->started,TRUE);
    CU_ASSERT_EQUAL(data->ended,FALSE);
    CU_ASSERT_EQUAL(data->arrray,av);
			
	data->ended = TRUE;
    return 0;
}


/**
 * faketdi_close() port from java test
 * also tested here is that we can call base methods on a derived object. we invoke 
 * the TDI destructor here, which is an overide of the etchobject destructor.
 * the TDI destructor walks the vtable chain to its parent, and invokes the parent
 * destructor to destroy etchobject content such as any exception, and finally
 * the object itself.
 */
static void faketdi_close(tagged_data_input* tdi)  
{
    etch_object_destroy(tdi); /* destroy object */
}


/**
 * faketdo_close() port from java test
 */
static void faketdo_close(tagged_data_output* tdo)  
{
    etch_object_destroy(tdo); /* destroy object */
}


/**
 * new_fake_tdi()
 * constructor for TDI implementation  
 */
static tagged_data_input* new_fake_tdi()
{
    tagged_data_input* faketdi = NULL;
    i_tagged_data_input*  vtab = NULL;
    const unsigned short CLASS_ID = CLASSID_VTABLE_FAKETDI;

    faketdi = new_tagged_data_input();
   
    vtab = cache_find(get_vtable_cachehkey(CLASS_ID), 0);

    if(!vtab)  
    {   
        vtab = new_vtable(NULL, sizeof(i_tagged_data_input), CLASS_ID);
   
        /* override three i_tagged_data_input methods */
        vtab->start_array = faketdi_start_array;
        vtab->end_array   = faketdi_end_array;
        vtab->read_array_element = faketdi_read_array_element;

        ((etch_object*)vtab)->vtab = faketdi->vtab; /* chain parent vtable to override vtab */

        cache_insert(vtab->hashkey, vtab, FALSE);
    } 

    CU_ASSERT_EQUAL_FATAL(((etch_object*)vtab)->class_id, CLASSID_VTABLE_FAKETDI);

    ((etch_object*)faketdi)->vtab = vtab;  /* set override vtab */

    faketdi->impl = (etch_object*) new_fake_tdi_impl(); /* instantiate tdi instance data */

    switch(g_which_exception_test)
    {   case EXCPTEST_UNCHECKED_STATICTEXT: 
             etch_throw((etch_object*)faketdi, EXCPTYPE_NULLPTR, NULL, 0);  
             break;   
        case EXCPTEST_CHECKED_COPYTEXT:   
             etch_throw((etch_object*)faketdi, EXCPTYPE_CHECKED_BOGUS, L"copied text", ETCHEXCP_COPYTEXT | ETCHEXCP_FREETEXT);  
             break; 
        case EXCPTEST_CHECKED_STATICTEXT:   
             etch_throw((etch_object*)faketdi, EXCPTYPE_CHECKED_BOGUS, local_excp_text, ETCHEXCP_STATICTEXT);  
             break;       
    }

    return faketdi;
}


/**
 * new_fake_tdo()
 * constructor for TDO implementation  
 */
static tagged_data_output* new_fake_tdo()
{
    tagged_data_output* faketdo = NULL;
    i_tagged_data_output*  vtab = NULL;
    const unsigned short CLASS_ID = CLASSID_VTABLE_FAKETDO;

    faketdo = new_tagged_data_output();
     
    vtab = cache_find(get_vtable_cachehkey(CLASS_ID), 0);

    if(!vtab)  
    {   vtab = new_vtable(NULL, sizeof(i_tagged_data_input), CLASS_ID);
     
        /* override three i_tagged_data_output methods */
        vtab->start_array = faketdo_start_array;
        vtab->end_array   = faketdo_end_array;
        vtab->write_array_element = faketdo_write_array_element;

        ((etch_object*)vtab)->vtab = faketdo->vtab; /* chain parent vtab to override vtab */
    
        cache_insert(vtab->hashkey, vtab, FALSE);
    } 

    CU_ASSERT_EQUAL_FATAL(((etch_object*)vtab)->class_id, CLASSID_VTABLE_FAKETDO);

    ((etch_object*)faketdo)->vtab = vtab; /* set override vtab */

    faketdo->impl = (etch_object*) new_fake_tdo_impl(); /* instantiate tdo instance data */

    return faketdo;
}

#endif /* if(0) */


/* 
 * load_testdata_string()
 * load testdata array with some ETCH_STRING objects
 */
static int load_testdata_string()
{
    int i = 0, numitems = 4;
    etch_string* newobj = NULL;
    wchar_t* str0 = L"now ", *str1 = L"is  ", *str2 = L"the ", *str3 = L"time";
    wchar_t* strings[4] = { str0, str1, str2, str3 };

    for(; i < numitems; i++)
    {
        /* new_etch_string copies parameter string */        
        newobj = new_stringw(strings[i]);  
        etch_arraylist_add(testdata, newobj);
    }

    return numitems;
}


/* 
 * load_testdata_int()
 * load testdata array with some ETCH_INT objects
 */
static int load_testdata_int()
{
    int i = 0, numitems = 4;
    etch_int32* newobj = NULL;
    int ints[4] = { 1, 2, 3, 4 };

    for(; i < numitems; i++)
    {
        newobj = new_int32(ints[i]);
        etch_arraylist_add(testdata, newobj);
    }

    return numitems;
}


/* 
 * new_testdata()
 * create testdata array and load it up with data objects
 */
static int new_testdata(const int datatype)
{
    int count = 0;
    g_which_exception_test = 0;
    testdata = new_etch_arraylist(0,0);  
    testdata->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
 
    switch(datatype)
    { case 1: count = load_testdata_int();    break;
      case 2: count = load_testdata_string(); break;
      default: return -1;
    }
  
    return count;
}


/* 
 * destroy_testdata()
 * destroy testdata array and content
 */
static void destroy_testdata()
{
    etch_arraylist_destroy(testdata, TRUE);
}


#if 0
/* 
 * compare_lists()
 * compares testdata list with the arrayvalue's list
 * returns boolean indicating equal or not
 */
static int compare_lists(etch_arrayvalue* av)
{
    int testcount = 0, i = 0, eqcount = 0;
    CU_ASSERT_PTR_NOT_NULL_FATAL(av);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(av->list);

    testcount = av->list->count; 
    CU_ASSERT_EQUAL(testcount, testdata->count);
    if (testcount != testdata->count) return FALSE;

    for(; i < (const int) testcount; i++)     
        if (etch_arraylist_get(testdata, i) == etch_arraylist_get(av->list, i))
            eqcount++;  

    CU_ASSERT_EQUAL(testcount, eqcount);     
    return testcount == eqcount;
}
#endif

#if(0)

/* 
 * run_read_test
 * create an array value to read current test data
 * and verify that content matches test data
 */
static void run_read_test()
{
    int result = 0;
    tagged_data_input* tdi = NULL;
    etch_arrayvalue* av = NULL;

    tdi = new_fake_tdi();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);
    CU_ASSERT_FALSE_FATAL(is_exception(tdi));
    CU_ASSERT_EQUAL_FATAL(get_result(tdi),0);

    /* create array value, reading from test values */
    av = arrayvalue_read(tdi);

    result = compare_lists(av);
    CU_ASSERT_EQUAL(result, TRUE);

    /* clean up TDI */
    faketdi_close(tdi);

    /* destroy testdata list and content */
    testdata->destroy(testdata);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/* 
 * run_write_test
 * create an array value to write current test data
 * and verify that content matches test data
 */
static void run_write_test()
{
    int result = 0, i = 0;
    etch_arrayvalue* arrayval = NULL;
    tagged_data_output* tdo = NULL;
    const int DIM_ZERO = 0, NOT_READONLY = FALSE, DEFSIZE = 0, DEFDELTA = 0;
    etch_type* fake_structtype = NULL;
    byte  fake_typecode = 0;
    void* thisitem = NULL;
                                         
    arrayval = new_arrayvalue /* create array value instance */
        (fake_typecode, fake_structtype, DIM_ZERO, DEFSIZE, DEFDELTA, NOT_READONLY);

    CU_ASSERT_PTR_NOT_NULL_FATAL(arrayval);
    CU_ASSERT_FALSE_FATAL(is_exception(arrayval));
    CU_ASSERT_EQUAL_FATAL(get_result(arrayval),0);

    for(; i < (const int) testdata->count; i++)
    {     
        thisitem = arraylist_get(testdata, i);
        result = arrayvalue_add(arrayval, (ETCH_ARRAY_ELEMENT*) thisitem);
        CU_ASSERT_EQUAL(result,0);
    }

    tdo = new_fake_tdo();

    result = arrayvalue_write(arrayval, tdo);
    CU_ASSERT_EQUAL(result,0);

    faketdo_close(tdo);

    /* destroy testdata list but not content */
    arraylist_destroy(testdata, FALSE);  

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}

#endif /* #if(0) */


/* 
 * run_exception_test
 *  
 */
static void run_exception_test(const int whichtest)
{
    /* global marker asks components to throw exceptions */
    g_which_exception_test = whichtest; 

    #if(0)
    /* create a bogus TDI inheriting from tagged data input */
    tdi = new_fake_tdi();
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);

    do 
    {   switch(whichtest)
        {   case EXCPTEST_UNCHECKED_STATICTEXT:
            case EXCPTEST_CHECKED_COPYTEXT:
            case EXCPTEST_CHECKED_STATICTEXT:
            {   CU_ASSERT_TRUE_FATAL(is_exception(tdi));
                #if IS_DEBUG_CONSOLE 
                wprintf(L"\ncaught %s exception\n", tdi->result->exception->excptext);          
                #endif   
                break;          
            }
        }
        if (is_exception(tdi)) break;

        /* create array value, reading from test values */
        av = arrayvalue_read(tdi);
        /* if av is NULL we would throw an exception here */

        result = compare_lists(av);
        CU_ASSERT_EQUAL(result, TRUE); 

    } while(0);  

    /* clean up TDI */
    faketdi_close(tdi);

    #endif

    /* destroy testdata list and content */
    etch_object_destroy(testdata);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


#if(0)

/* 
 * read_test_integer
 */
static void read_test_integer(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(1)) > 0)
        run_read_test(); 
}


/* 
 * write_test_integer
 */
static void write_test_integer(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(1)) > 0)
        run_write_test(); 
}


/* 
 * read_test_string
 */
static void read_test_string(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(2)) > 0)
        run_read_test(); 
}


/* 
 * write_test_string
 */
static void write_test_string(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(2)) > 0)
        run_write_test(); 
}

#endif /* #if(0) */


/* 
 * exception_test_1
 */
static void exception_test_1(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(1)) > 0)
        run_exception_test(EXCPTEST_UNCHECKED_STATICTEXT); 
}


/* 
 * exception_test_2
 */
static void exception_test_2(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(1)) > 0)
        run_exception_test(EXCPTEST_CHECKED_COPYTEXT); 
}


/* 
 * exception_test_3
 */
static void exception_test_3(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(1)) > 0)
        run_exception_test(EXCPTEST_CHECKED_STATICTEXT); 
}


/* 
 * test_iterator_over_arraylist
 */
static void test_iterator_over_arraylist(void)
{
    etch_iterator* iterator = NULL; 
    int testcount = 0;
    struct i_iterable* vtab = NULL;

    new_testdata(1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(testdata);
    CU_ASSERT_NOT_EQUAL(testdata->count, 0);

    iterator = new_iterator(testdata, &testdata->iterable);

    CU_ASSERT_PTR_NOT_NULL_FATAL(iterator);
    CU_ASSERT_EQUAL_FATAL(iterator->ordinal,1);
    testcount = 1;
    
    vtab = (struct i_iterable*)((etch_object*)iterator)->vtab;
    
    while(vtab->has_next(iterator))
          testcount += vtab->next(iterator) == 0;  
        
    CU_ASSERT_EQUAL(testcount, testdata->count);

    destroy_iterator(iterator);
    destroy_testdata();
}


/* 
 * test_create_from_1
 * create arrayvalue from single-dimensioned byte nativearray and verify
 */
static void test_create_from_1(void)
{
    etch_arrayvalue* av = NULL;
    etch_byte* thisitem = NULL;
    int   i = 0, result = 0;
    byte  fake_typecode = 0xff;
    char  x[4] = {'a','b','c','d'};
    const int numdimensions = 1, itemcount = 4;

    etch_nativearray* a = new_etch_nativearray 
        (CLASSID_ARRAY_BYTE, sizeof(byte), numdimensions, itemcount, 0, 0);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 

    a->content_obj_type = ETCHTYPEB_BYTE; /* array content is type byte */
    a->content_class_id = CLASSID_NONE;   /* array content is unwrapped (default) */

    for(i = 0; i < itemcount; i++) /* populate native array from test data */
        CU_ASSERT_EQUAL(0, result = a->put1(a, &x[i], i)); 

    /* populate arrayvalue from native array */
    av = new_arrayvalue_from(a, fake_typecode, NULL, itemcount, 0, FALSE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(av);
    CU_ASSERT_EQUAL(arrayvalue_count(av), itemcount);

    /* assert that arrayvalue content matches test data */
    for(i=0; i < itemcount; i++)
    {     
        thisitem = arrayvalue_get(av, i);
        CU_ASSERT_PTR_NOT_NULL_FATAL(thisitem); 
        CU_ASSERT_EQUAL(((etch_object*)thisitem)->obj_type, ETCHTYPEB_PRIMITIVE);
        CU_ASSERT_EQUAL(((etch_object*)thisitem)->class_id, CLASSID_PRIMITIVE_BYTE);
        CU_ASSERT_EQUAL(thisitem->value, x[i]);
    }

    etch_object_destroy(av);  /* destroy array value and content */
}


/* 
 * test_create_from_2
 * create arrayvalue from single-dimensioned int nativearray and verify
 */
static void test_create_from_2(void)
{
    etch_arrayvalue* av  = NULL;
    etch_int32* thisitem = NULL;
    int   i = 0, result  = 0;
    byte  fake_typecode  = 0xff;
    int   n[4] = {10,11,12,13};
    const int numdimensions = 1, itemcount = 4;

    etch_nativearray* a = new_etch_nativearray 
        (CLASSID_ARRAY_INT32, sizeof(int), numdimensions, itemcount, 0, 0);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 

    a->content_obj_type = ETCHTYPEB_INT32; /* array content is type int */
    a->content_class_id = CLASSID_NONE;   /* array content is unwrapped (default) */

    for(i = 0; i < itemcount; i++) /* populate native array from test data */
        CU_ASSERT_EQUAL(0, result = a->put1(a, &n[i], i));
 
    /* populate arrayvalue from native array */
    av = new_arrayvalue_from(a, fake_typecode, NULL, itemcount, 0, FALSE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(av);
    CU_ASSERT_EQUAL(arrayvalue_count(av), itemcount);

    /* assert that arrayvalue content matches test data */
    for(i=0; i < itemcount; i++)
    {     
        thisitem = arrayvalue_get(av, i);
        CU_ASSERT_PTR_NOT_NULL_FATAL(thisitem); 
        CU_ASSERT_EQUAL(((etch_object*)thisitem)->obj_type, ETCHTYPEB_PRIMITIVE);
        CU_ASSERT_EQUAL(((etch_object*)thisitem)->class_id, CLASSID_PRIMITIVE_INT32);
        CU_ASSERT_EQUAL(thisitem->value, n[i]);
    }

    etch_object_destroy(av);  /* destroy array value and content */
}


/* 
 * test_create_from_3
 * create arrayvalue from single-dimensioned long long nativearray and verify
 */
static void test_create_from_3(void)
{
    etch_arrayvalue* av  = NULL;
    etch_int64* thisitem = NULL;
    int   i = 0, result  = 0;
    byte  fake_typecode  = 0xff;
    int64 n[5] = {-2, -1, 0 , 1, 2};
    const int numdimensions = 1, itemcount = 5;

    etch_nativearray* a = new_etch_nativearray 
        (CLASSID_ARRAY_INT64, sizeof(int64), numdimensions, itemcount, 0, 0);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 

    a->content_obj_type = ETCHTYPEB_INT64; /* array content is type int64 */
    a->content_class_id = CLASSID_NONE;   /* array content is unwrapped (default) */

    for(i = 0; i < itemcount; i++) /* populate native array from test data */
        CU_ASSERT_EQUAL(0, result = a->put1(a, &n[i], i));
 
    /* populate arrayvalue from native array */
    av = new_arrayvalue_from(a, fake_typecode, NULL, itemcount, 0, FALSE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(av);
    CU_ASSERT_EQUAL(arrayvalue_count(av), itemcount);

    /* assert that arrayvalue content matches test data */
    for(i=0; i < itemcount; i++)
    {     
        thisitem = arrayvalue_get(av, i);
        CU_ASSERT_PTR_NOT_NULL_FATAL(thisitem); 
        CU_ASSERT_EQUAL(((etch_object*)thisitem)->obj_type, ETCHTYPEB_PRIMITIVE);
        CU_ASSERT_EQUAL(((etch_object*)thisitem)->class_id, CLASSID_PRIMITIVE_INT64);
        CU_ASSERT_EQUAL(thisitem->value, n[i]);
    }

    etch_object_destroy(av);  /* destroy array value and content */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test_create_from_4
 * create arrayvalue from single-dimensioned double nativearray and verify
 */
static void test_create_from_4(void)
{
    etch_arrayvalue* av   = NULL;
    etch_double* thisitem = NULL;
    int    i = 0, result  = 0;
    byte   fake_typecode  = 0xff;
    double f[3] = {-1000.0, 3.14149, 65536.0};
    const int numdimensions = 1, itemcount = 3;

    etch_nativearray* a = new_etch_nativearray 
        (CLASSID_ARRAY_DOUBLE, sizeof(int64), numdimensions, itemcount, 0, 0);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 

    a->content_obj_type = ETCHTYPEB_IEEE64; /* array content is type double */
    a->content_class_id = CLASSID_NONE;   /* array content is unwrapped (default) */

    for(i = 0; i < itemcount; i++) /* populate native array from test data */
        CU_ASSERT_EQUAL(0, result = a->put1(a, &f[i], i));
 
    /* populate arrayvalue from native array */
    av = new_arrayvalue_from(a, fake_typecode, NULL, itemcount, 0, FALSE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(av);
    CU_ASSERT_EQUAL(arrayvalue_count(av), itemcount);

    /* assert that arrayvalue content matches test data */
    for(i=0; i < itemcount; i++)
    {     
        thisitem = arrayvalue_get(av, i);
        CU_ASSERT_PTR_NOT_NULL_FATAL(thisitem); 
        CU_ASSERT_EQUAL(((etch_object*)thisitem)->obj_type, ETCHTYPEB_PRIMITIVE);
        CU_ASSERT_EQUAL(((etch_object*)thisitem)->class_id, CLASSID_PRIMITIVE_DOUBLE);
        CU_ASSERT_DOUBLE_EQUAL(thisitem->value, f[i], 0.01);
    }

    etch_object_destroy(av);  /* destroy array value and content */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test_create_from_5
 * create arrayvalue from single-dimensioned string nativearray and verify
 */
static void test_create_from_5(void)
{
    etch_arrayvalue* av   = NULL;
    etch_string* thisitem = NULL;
    int   i = 0, result   = 0;
    byte  fake_typecode   = 0xff;
    wchar_t* c[3] = {L"hey", L"it", L"works!"};
    //char* c[3] = {"hey", "it", "works!"};
    const int numdimensions = 1, itemcount = 3;

    etch_nativearray* a = new_etch_nativearray 
        (CLASSID_ARRAY_STRING, sizeof(void*), numdimensions, itemcount, 0, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 

    a->content_obj_type = ETCHTYPEB_STRING; /* array content is type string */
    a->content_class_id = CLASSID_NONE;    /* array content is unwrapped (default) */
    a->is_content_owned = FALSE;

    for(i = 0; i < itemcount; i++) /* populate native array from test data */
        CU_ASSERT_EQUAL(0, result = a->put1(a, &c[i], i));
 
    /* populate arrayvalue from native array */
    printf("tot1\n");
    av = new_arrayvalue_from(a, fake_typecode, NULL, itemcount, 0, FALSE);
    printf("tot2\n");
    CU_ASSERT_PTR_NOT_NULL_FATAL(av);
    CU_ASSERT_EQUAL(arrayvalue_count(av), itemcount);

    /* assert that arrayvalue content matches test data */
    for(i=0; i < itemcount; i++)
    {     
        thisitem = arrayvalue_get(av, i);
        CU_ASSERT_PTR_NOT_NULL_FATAL(thisitem); 
        CU_ASSERT_EQUAL(((etch_object*)thisitem)->obj_type, ETCHTYPEB_PRIMITIVE);
        CU_ASSERT_EQUAL(((etch_object*)thisitem)->class_id, CLASSID_STRING);
        CU_ASSERT_NSTRING_EQUAL(thisitem->v.value, c[i], wcslen(c[i]));
        //CU_ASSERT_NSTRING_EQUAL(thisitem->v.value, c[i], strlen(c[i]));
    }

    etch_object_destroy(av);  /* destroy array value and content */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test_create_from_10
 * create arrayvalue from two-dimensioned byte nativearray and verify
 */
static void test_create_from_10(void)
{
    etch_arrayvalue* av = NULL;
    etch_arrayvalue* subav = NULL;
    etch_byte*       thisitem = NULL;
    int   i = 0, j = 0, result = 0;
    byte  fake_typecode = 0xff;
    char  x[2][4] = {  {'a','b','c','d',},  {'e', 'f', 'g', 'h',},  };
    const int numdimensions = 2, dim0count = 4, dim1count = 2;

    etch_nativearray* a = new_etch_nativearray 
        (CLASSID_ARRAY_BYTE, sizeof(byte), numdimensions, dim0count, dim1count, 0);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 

    a->content_obj_type = ETCHTYPEB_BYTE; /* array content is type byte */
    a->content_class_id = CLASSID_NONE;   /* array content is unwrapped (default) */

    for(i = 0; i < dim1count; i++)
        for(j = 0; j < dim0count; j++) /* populate native array from test data */
            CU_ASSERT_EQUAL(0, result = a->put2(a, &x[i][j], i, j)); 

    /* populate arrayvalue from native array. we tell the arrayvalue to take  
     * ownership of the native array. result is a 2-dimensional arrayvalue,
     * which is represented as an arrayvalue of arrayvalues
     */
    av = new_arrayvalue_from(a, fake_typecode, NULL, 32, 0, FALSE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(av);
    CU_ASSERT_EQUAL(av->dim, numdimensions); 
    CU_ASSERT_EQUAL(arrayvalue_count(av), dim1count); 

    /* assert that arrayvalue content matches test data */
    for(i = 0; i < dim1count; i++)
    {
        subav = arrayvalue_get(av, i);
        CU_ASSERT_PTR_NOT_NULL_FATAL(subav); 
        CU_ASSERT_EQUAL(((etch_object*)subav)->obj_type, ETCHTYPEB_ARRAYVAL);
        CU_ASSERT_EQUAL(arrayvalue_count(subav), dim0count);

        for(j = 0; j < dim0count; j++)
        {     
            byte expected_value = x[i][j];
            thisitem = arrayvalue_get(subav, j);
            CU_ASSERT_PTR_NOT_NULL_FATAL(thisitem); 
            CU_ASSERT_EQUAL(((etch_object*)thisitem)->obj_type, ETCHTYPEB_PRIMITIVE);
            CU_ASSERT_EQUAL(((etch_object*)thisitem)->class_id, CLASSID_PRIMITIVE_BYTE);
            CU_ASSERT_EQUAL(thisitem->value, expected_value);
        }
    }

    /* destroy array value and content. memory includes the sub-arrayvalues,
     * the native array from above, and the sub-native arrays */
    etch_object_destroy(av);  

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test_create_from_11
 * create arrayvalue from two-dimensioned int16 nativearray and verify
 */
static void test_create_from_11(void)
{
    etch_arrayvalue* av = NULL;
    etch_arrayvalue* subav = NULL;
    etch_int16*      thisitem = NULL;
    int   i = 0, j = 0, result = 0;
    byte  fake_typecode = 0xff;
    short n[2][4] = { { 100,101,102,103, },  { 0xeffc,0xeffd,0xeffe,0xefff },  };
    const int numdimensions = 2, dim0count = 4, dim1count = 2;

    etch_nativearray* a = new_etch_nativearray 
        (CLASSID_ARRAY_INT16, sizeof(short), numdimensions, dim0count, dim1count, 0);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 

    a->content_obj_type = ETCHTYPEB_INT16; /* array content is type short int */
    a->content_class_id = CLASSID_NONE;   /* array content is unwrapped (default) */

    for(i = 0; i < dim1count; i++)
        for(j = 0; j < dim0count; j++) /* populate native array from test data */
            CU_ASSERT_EQUAL(0, result = a->put2(a, &n[i][j], i, j)); 

    /* populate arrayvalue from native array. we tell the arrayvalue to take  
     * ownership of the native array. result is a 2-dimensional arrayvalue,
     * which is represented as an arrayvalue of arrayvalues
     */
    av = new_arrayvalue_from(a, fake_typecode, NULL, 32, 0, FALSE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(av);
    CU_ASSERT_EQUAL(av->dim, numdimensions); 
    CU_ASSERT_EQUAL(arrayvalue_count(av), dim1count); 

    /* assert that arrayvalue content matches test data */
    for(i = 0; i < dim1count; i++)
    {
        subav = arrayvalue_get(av, i);
        CU_ASSERT_PTR_NOT_NULL_FATAL(subav); 
        CU_ASSERT_EQUAL(((etch_object*)subav)->obj_type, ETCHTYPEB_ARRAYVAL);
        CU_ASSERT_EQUAL(arrayvalue_count(subav), dim0count);

        for(j = 0; j < dim0count; j++)
        {     
            short expected_value = n[i][j];
            thisitem = arrayvalue_get(subav, j);
            CU_ASSERT_PTR_NOT_NULL_FATAL(thisitem); 
            CU_ASSERT_EQUAL(((etch_object*)thisitem)->obj_type, ETCHTYPEB_PRIMITIVE);
            CU_ASSERT_EQUAL(((etch_object*)thisitem)->class_id, CLASSID_PRIMITIVE_INT16);
            CU_ASSERT_EQUAL(thisitem->value, expected_value);
        }
    }

    /* destroy array value and content. memory includes the sub-arrayvalues,
     * the native array from above, and the sub-native arrays */
    etch_object_destroy(av);  

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test_create_from_12
 * create arrayvalue from two-dimensioned string nativearray and verify
 */
static void test_create_from_12(void)
{
    etch_arrayvalue* av = NULL;
    etch_arrayvalue* subav = NULL;
    etch_string*     thisitem = NULL;
    const int DEFSIZE=0, DEFDELTA=0; 
    int   i = 0, j = 0, result = 0;
    byte  fake_typecode = 0xff;

    wchar_t* x[6][4] 
      = { { L"I think that I", L"shall never see,", L"A poem as lovely", L"as a tree.", }, 
          { L"She knows not what", L"the curse may be,", L"And so she weaveth", L"steadily,", }, 
          { L"And little other", L"care hath she,", L"The Lady of Shalott", L".", }, 
          { L"And therefore I have sailed", L"the seas, and come", L"To the holy city", L"of Byzantium.", },
          { L"Once out of nature", L"I shall never take", L"My bodily form", L" from any natural thing,", }, 
          { L"But such a form", L"as Grecian goldsmiths make", L"Of hammered gold ", L"and gold enameling", }, 
       };

    const int numdimensions = 2, dim0count = 4, dim1count = 6;

    #if(0) /* create empty native array and populate it item by item */

    etch_nativearray* a = new_etch_nativearray (CLASSID_ARRAY_STRING, 
        sizeof(void*), numdimensions, dim0count, dim1count, 0);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 

    a->content_obj_type = ETCHTYPEB_STRING; /* array content is type string */
    a->content_class_id = CLASSID_NONE;   /* array content is unwrapped (default) */

    for(i = 0; i < dim1count; i++)
    for(j = 0; j < dim0count; j++)  /* populate native array from test data */
        CU_ASSERT_EQUAL(0, result = a->put2(a, &x[i][j], i, j)); 

    #else /* create native array from static array */

    etch_nativearray* a = new_etch_nativearray_from(&x, CLASSID_ARRAY_STRING, 
        sizeof(void*), numdimensions, dim0count, dim1count, 0);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 

    a->content_obj_type = ETCHTYPEB_STRING; /* array content is type string */
    a->content_class_id = CLASSID_NONE;     /* array content is unwrapped (default) */

    #endif 

    for(j = 0; j < dim0count; j++)       
    {   wchar_t* thisx = 0, *thatx = x[i][j];          
        result = a->get2(a, &thisx, i, j); 
        CU_ASSERT_EQUAL(result, 0); 
        CU_ASSERT_PTR_NOT_NULL_FATAL(thisx); 
        result = wcscmp(thisx, thatx);
        CU_ASSERT_EQUAL(result,0); 
    }   

    /* populate arrayvalue from native array. we tell the arrayvalue to take  
     * ownership of the native array. result is a 2-dimensional arrayvalue,
     * which is represented as an arrayvalue of arrayvalues
     */
    av = new_arrayvalue_from(a, fake_typecode, NULL, DEFSIZE, DEFDELTA, FALSE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(av);
    CU_ASSERT_EQUAL(av->dim, numdimensions); 
    CU_ASSERT_EQUAL(result = arrayvalue_count(av), dim1count); 

    /* assert that arrayvalue content matches test data */
    for(i = 0; i < dim1count; i++)
    {
        subav = arrayvalue_get(av, i);
        CU_ASSERT_PTR_NOT_NULL_FATAL(subav); 
        CU_ASSERT_EQUAL(((etch_object*)subav)->obj_type, ETCHTYPEB_ARRAYVAL);
        CU_ASSERT_EQUAL(result = arrayvalue_count(subav), dim0count);

        for(j = 0; j < dim0count; j++)
        {     
            wchar_t* expected_value = x[i][j];
            thisitem = arrayvalue_get(subav, j);
            CU_ASSERT_PTR_NOT_NULL_FATAL(thisitem); 
            CU_ASSERT_EQUAL(((etch_object*)thisitem)->obj_type, ETCHTYPEB_PRIMITIVE);
            CU_ASSERT_EQUAL(((etch_object*)thisitem)->class_id, CLASSID_STRING);
            result = wcscmp(expected_value, thisitem->v.value);
            CU_ASSERT_EQUAL(result, 0);
        }
    }

    /* destroy array value and content. memory includes the sub-arrayvalues,
     * the native array from above, and the sub-native arrays */
    etch_object_destroy(av);  

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test_create_from_13
 * create arrayvalue from two-dimensioned object nativearray and verify
 */
static void test_create_from_13(void)
{
    etch_nativearray* a = NULL;
    etch_arrayvalue* av = NULL;
    etch_arrayvalue* subav = NULL;
    etch_object* thisitem = NULL;
    const int DEFSIZE=0, DEFDELTA=0; 
    int   i = 0, j = 0, result = 0, count = 0;
    byte  fake_typecode = 0xff;

    etch_object* x[2][3] = { { NULL, NULL, NULL, }, { NULL, NULL, NULL, }, };

    const int numdimensions = 2, dim0count = 3, dim1count = 2;

    for(i = 0; i < dim1count; i++)  /* populate array of objects */
    for(j = 0; j < dim0count; j++)
    {   etch_object* thisobj = new_object(sizeof(etch_object), ETCHTYPEB_ETCHOBJECT,CLASSID_NONE);
        thisobj->hashkey = ++count;     /* usurp hashkey for this test */
        x[i][j] = thisobj;
    }

    /* create an etch_nativearray from the above array of objects. 
     * native array owns its data and so since the data are etch objects
     * the native array will destroy them at such time as arrayvalue
     * destroys the native array, the objects likewise destroying their
     * own content (that content being the etch_malloc'ed memory, above).
     */
    a = new_etch_nativearray_from(&x, CLASSID_ARRAY_OBJECT, 
        sizeof(void*), numdimensions, dim0count, dim1count, 0);  

    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 

    a->content_obj_type = ETCHTYPEB_ETCHOBJECT; /* array content is type object */
    a->content_class_id = CLASSID_OBJECT;       /* array content is wrapped */

    for(i = 0; i < dim1count; i++)   /* assert that native array content */
    for(j = 0; j < dim0count; j++)   /* when read back, is as expected */    
    {   etch_object* thisx = 0, *thatx = x[i][j];          
        result = a->get2(a, &thisx, i, j); 
        CU_ASSERT_EQUAL(result, 0); 
        CU_ASSERT_PTR_NOT_NULL_FATAL(thisx); 
        CU_ASSERT_PTR_EQUAL_FATAL(thisx, thatx);  
    }   

    /* populate arrayvalue from native array. we tell the arrayvalue to take  
     * ownership of the native array. result is a 2-dimensional arrayvalue,
     * which is represented as an arrayvalue of arrayvalues
     */
    av = new_arrayvalue_from(a, fake_typecode, NULL, DEFSIZE, DEFDELTA, FALSE);

    CU_ASSERT_PTR_NOT_NULL_FATAL(av);
    CU_ASSERT_EQUAL(av->dim, numdimensions); 
    CU_ASSERT_EQUAL(result = arrayvalue_count(av), dim1count); 

    /* assert that arrayvalue content matches test data */
    for(i = 0; i < dim1count; i++)
    {
        subav = arrayvalue_get(av, i);
        CU_ASSERT_PTR_NOT_NULL_FATAL(subav); 
        CU_ASSERT_EQUAL(((etch_object*)subav)->obj_type, ETCHTYPEB_ARRAYVAL);
        CU_ASSERT_EQUAL(result = arrayvalue_count(subav), dim0count);

        for(j = 0; j < dim0count; j++)
        {     
            etch_object* expected_object = x[i][j];
            thisitem = arrayvalue_get(subav, j);
            CU_ASSERT_PTR_NOT_NULL_FATAL(thisitem); 
            CU_ASSERT_EQUAL_FATAL(((etch_object*)thisitem)->obj_type, ETCHTYPEB_ETCHOBJECT);
            // TODO this is CLASSID_ANY (zero) not CLASSID_OBJECT - verify that this is as expected
            // CU_ASSERT_EQUAL_FATAL(((etch_object*)thisitem)->class_id, CLASSID_OBJECT);
            CU_ASSERT_EQUAL(thisitem->hashkey, expected_object->hashkey);
        }
    }

    /* destroy array value and content. memory freed includes sub-arrayvalues,
     * the native array and test objects from above, and the sub-arrays */
    etch_object_destroy(av);  

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test_create_from_21
 * create arrayvalue from three-dimensioned byte nativearray and verify
 */
static void test_create_from_21(void)
{
    etch_arrayvalue* av = NULL;
    etch_arrayvalue* subav1 = NULL;
    etch_arrayvalue* subav2 = NULL;
    etch_byte* thisitem = NULL;
    int   i = 0, j = 0, k = 0, result = 0;
    const int DEFSIZE=0, DEFDELTA=0; 
    byte  fake_typecode = 0xff;

    char x[2][3][4] 
      = { { {'a','b','c','d',},             
            {'e','f','g','h',}, 
            {'i','j','k','l',}, 
          },
          { {'m','n','o','p',},             
            {'q','r','s','t',}, 
            {'u','v','w','x',}, 
          },
        };

    const int numdimensions = 3, dim0count = 4, dim1count = 3, dim2count = 2;

    #if(0)
    etch_nativearray* a = new_etch_nativearray   /* construct empty native array */
        (CLASSID_ARRAY_BYTE, sizeof(byte), numdimensions, dim0count, dim1count, 0);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 

    a->content_obj_type = ETCHTYPEB_BYTE; /* array content is type byte */
    a->content_class_id = CLASSID_NONE;   /* array content is unwrapped (default) */

    for(i = 0; i < dim2count; i++) /* populate native array iteratively */
    for(j = 0; j < dim1count; j++) 
    for(k = 0; k < dim0count; k++) 
        CU_ASSERT_EQUAL(0, result = a->put3(a, &x[i][j][k], i, j, k)); 
    #else
    /* populate native arrary from static test data array */

    etch_nativearray* a = new_etch_nativearray_from(&x, CLASSID_ARRAY_BYTE, 
        sizeof(byte), numdimensions, dim0count, dim1count, dim2count);  

    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 
    a->content_obj_type = ETCHTYPEB_BYTE; /* array content is type byte */
    a->content_class_id = CLASSID_NONE;   /* array content is unwrapped */

    #endif

    /* populate arrayvalue from native array. we tell the arrayvalue to take  
     * ownership of the native array. result is a 3-dimensional arrayvalue,
     * which is represented as an arrayvalue of arrayvalues of arrayvalues.
     */
    av = new_arrayvalue_from(a, fake_typecode, NULL, DEFSIZE, DEFDELTA, FALSE);

    CU_ASSERT_PTR_NOT_NULL_FATAL(av);
    CU_ASSERT_EQUAL(av->dim, numdimensions); 
    CU_ASSERT_EQUAL(arrayvalue_count(av), dim2count); 

    /* assert that arrayvalue content matches test data */
    for(i = 0; i < dim2count; i++)
    {
        subav1 = arrayvalue_get(av, i);
        CU_ASSERT_PTR_NOT_NULL_FATAL(subav1); 
        CU_ASSERT_EQUAL(((etch_object*)subav1)->obj_type, ETCHTYPEB_ARRAYVAL);
        CU_ASSERT_EQUAL(result = arrayvalue_count(subav1), dim1count);

        for(j = 0; j < dim1count; j++)
        {     
            subav2 = arrayvalue_get(subav1, j);
            CU_ASSERT_PTR_NOT_NULL_FATAL(subav2); 
            CU_ASSERT_EQUAL(((etch_object*)subav2)->obj_type, ETCHTYPEB_ARRAYVAL);
            CU_ASSERT_EQUAL(result = arrayvalue_count(subav2), dim0count);

            for(k = 0; k < dim0count; k++)
            {     
                byte expected_value = x[i][j][k];
                thisitem = arrayvalue_get(subav2, k);
                CU_ASSERT_PTR_NOT_NULL_FATAL(thisitem); 
                CU_ASSERT_EQUAL(((etch_object*)thisitem)->obj_type, ETCHTYPEB_PRIMITIVE);
                CU_ASSERT_EQUAL(((etch_object*)thisitem)->class_id, CLASSID_PRIMITIVE_BYTE);
                CU_ASSERT_EQUAL(((etch_byte*)thisitem)->value, expected_value);
            }
        }
    }

    /* destroy array value and content. memory freed includes the native array 
     * constructed above, the arrayvalue, its sub-arrayvalues and their sub-
     * native arrays, and any disposable native array content. 
     */ 
    etch_object_destroy(av);  

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test_arrayvalue_to_nativearray_10
 * create create native byte[] array from arrayvalue
 */
static void test_arrayvalue_to_nativearray_10(void)
{
    etch_arrayvalue* av = NULL;
    int   i = 0, result = 0;
    byte  fake_typecode = 0xff;
    char  x[4] = {'a','b','c','d'};
    const int numdimensions = 1, itemcount = 4;
    etch_nativearray* old_nativearray_dangling_ref = NULL;

    etch_nativearray* a = new_etch_nativearray(CLASSID_ARRAY_BYTE, sizeof(byte), numdimensions, itemcount, 0, 0);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 

    a->content_obj_type  = ETCHTYPEB_BYTE; /* array content is type byte */
    a->content_class_id  = CLASSID_NONE;   /* array content is unwrapped (default) */

    for(i = 0; i < itemcount; i++) /* populate native array from test data */
        CU_ASSERT_EQUAL(0, result = a->put1(a, &x[i], i)); 

    /* populate arrayvalue from native array */
    av = new_arrayvalue_from(a, fake_typecode, NULL, itemcount, 0, FALSE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(av);
    CU_ASSERT_EQUAL(arrayvalue_count(av), itemcount);

    for(i=0; i < itemcount; i++)
    {     
        etch_byte* thisitem = arrayvalue_get(av, i);
        CU_ASSERT_PTR_NOT_NULL_FATAL(thisitem); 
        CU_ASSERT_EQUAL(thisitem->value, x[i]);
    }

    /* this will be a dangling pointer after we arrayvalue_to_nativearray() */
    old_nativearray_dangling_ref = av->natarray;

    /* convert new arrayvalue back to a nativearray. the embedded nativearray 
     * we created above, is destroyed in the process, and replaced with a new
     * nativearray, whose content should of course be identical to the old. 
     */
    result = arrayvalue_to_nativearray(av); 
    CU_ASSERT_PTR_NOT_NULL_FATAL (av->natarray); 
    CU_ASSERT_PTR_NOT_EQUAL_FATAL(av->natarray, old_nativearray_dangling_ref); 
    CU_ASSERT_EQUAL_FATAL(result,itemcount);  /* returns items inserted count */

    /* get items one at a time from the arrayvalue's new native array,
     * and compare to expected value from the test data above.
     */
    for(i = 0; i < itemcount; i++)  
    {
        byte c2=0, c1 = x[i];
        result = av->natarray->get1(av->natarray, &c2, i);
        CU_ASSERT_EQUAL(result,0);
        CU_ASSERT_EQUAL(c1, c2); 
    }

    etch_object_destroy(av);  /* destroy array value and content */
}


/* 
 * test_arrayvalue_to_nativearray_20
 * create create native string[][] array from arrayvalue
 */
static void test_arrayvalue_to_nativearray_20(void)
{
    etch_arrayvalue* av = NULL;
    etch_arrayvalue* subav = NULL;
    etch_string*     thisitem = NULL;
    byte  fake_typecode = 0xff;
    const int numdimensions = 2, dim0count = 4, dim1count = 6;
    const int DEFSIZE=0, DEFDELTA=0; 
    int   i = 0, j = 0, result = 0;
    etch_nativearray* old_nativearray_dangling_ref = NULL;

    wchar_t* x[6][4] 
      = { { L"I think that I", L"shall never see,", L"A poem as lovely", L"as a tree.", }, 
          { L"She knows not what", L"the curse may be,", L"And so she weaveth", L"steadily,", }, 
          { L"And little other", L"care hath she,", L"The Lady of Shalott", L".", }, 
          { L"And therefore I have sailed", L"the seas, and come", L"To the holy city", L"of Byzantium.", },
          { L"Once out of nature", L"I shall never take", L"My bodily form", L" from any natural thing,", }, 
          { L"But such a form", L"as Grecian goldsmiths make", L"Of hammered gold ", L"and gold enameling", }, 
       };

    etch_nativearray* a = new_etch_nativearray_from(&x, CLASSID_ARRAY_STRING, 
        sizeof(void*), numdimensions, dim0count, dim1count, 0);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 
    a->content_obj_type = ETCHTYPEB_STRING; /* content is type string */
    a->content_class_id = CLASSID_UNWRAPPED;     /* content is unwrapped (default) */

    /* populate arrayvalue from native array as in earlier test */
    av = new_arrayvalue_from(a, fake_typecode, NULL, DEFSIZE, DEFDELTA, FALSE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(av);

    /* this will be a dangling pointer after we arrayvalue_to_nativearray() */
    old_nativearray_dangling_ref = av->natarray;

    /* convert new arrayvalue back to a nativearray. the embedded nativearray 
     * we created above, is destroyed in the process, and replaced with a new
     * nativearray, whose content should of course be identical to the old. 
     */
    result = arrayvalue_to_nativearray(av); 
    CU_ASSERT_PTR_NOT_NULL_FATAL (av->natarray); 
    CU_ASSERT_PTR_NOT_EQUAL_FATAL(av->natarray, old_nativearray_dangling_ref); 
    CU_ASSERT_EQUAL_FATAL(result, dim0count * dim1count);   
  
    /* get items one at a time from the arrayvalue's new native array,
     * and compare to expected value from the test data above.
     */
    for(i = 0; i < dim1count; i++)
    {
        subav = arrayvalue_get(av, i);
        CU_ASSERT_PTR_NOT_NULL_FATAL(subav); 
        CU_ASSERT_EQUAL(((etch_object*)subav)->obj_type, ETCHTYPEB_ARRAYVAL);
        CU_ASSERT_EQUAL(result = arrayvalue_count(subav), dim0count);

        for(j = 0; j < dim0count; j++)
        {     
            wchar_t* expected_value = x[i][j];
            thisitem = arrayvalue_get(subav, j);
            CU_ASSERT_PTR_NOT_NULL_FATAL(thisitem); 
            CU_ASSERT_EQUAL(((etch_object*)thisitem)->obj_type, ETCHTYPEB_PRIMITIVE);
            CU_ASSERT_EQUAL(((etch_object*)thisitem)->class_id, CLASSID_STRING);
            result = wcscmp(expected_value, thisitem->v.value);
            CU_ASSERT_EQUAL(result, 0);
        }
    }

    etch_object_destroy(av);  /* destroy array value and content */

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test_arrayvalue_to_nativearray_30
 * create create native byte[][][] array from arrayvalue
 */
static void test_arrayvalue_to_nativearray_30(void)
{
    etch_arrayvalue* av = NULL;
    etch_arrayvalue* subav1 = NULL;
    etch_arrayvalue* subav2 = NULL;
    etch_byte* thisitem = NULL;
    byte  fake_typecode = 0xff;
    int   i = 0, j = 0, k = 0, result = 0;
    const int DEFSIZE=0, DEFDELTA=0; 
    const int numdimensions = 3, dim0count = 4, dim1count = 3, dim2count = 2;
    etch_nativearray* old_nativearray_dangling_ref = NULL;

    char x[2][3][4] 
      = { { {'a','b','c','d',},             
            {'e','f','g','h',}, 
            {'i','j','k','l',}, 
          },
          { {'m','n','o','p',},             
            {'q','r','s','t',}, 
            {'u','v','w','x',}, 
          },
        };

    etch_nativearray* a = new_etch_nativearray_from(&x, CLASSID_ARRAY_BYTE, 
        sizeof(byte), numdimensions, dim0count, dim1count, dim2count);  

    CU_ASSERT_PTR_NOT_NULL_FATAL(a); 
    a->content_obj_type = ETCHTYPEB_BYTE; /* content is type byte */
    a->content_class_id = CLASSID_NONE;   /* content is unwrapped */

    /* populate arrayvalue from native array. we tell the arrayvalue to take  
     * ownership of the native array. result is a 3-dimensional arrayvalue,
     * which is represented as an arrayvalue of arrayvalues of arrayvalues.
     */
    av = new_arrayvalue_from(a, fake_typecode, NULL, DEFSIZE, DEFDELTA, FALSE);

    CU_ASSERT_PTR_NOT_NULL_FATAL(av);
 
    /* this will be a dangling pointer after we arrayvalue_to_nativearray() */
    old_nativearray_dangling_ref = av->natarray;

    /* convert new arrayvalue back to a nativearray. the embedded nativearray 
     * we created above, is destroyed in the process, and replaced with a new
     * nativearray, whose content should of course be identical to the old. 
     */
    result = arrayvalue_to_nativearray(av); 
    CU_ASSERT_PTR_NOT_NULL_FATAL (av->natarray); 
    CU_ASSERT_PTR_NOT_EQUAL_FATAL(av->natarray, old_nativearray_dangling_ref); 
    CU_ASSERT_EQUAL_FATAL(result, dim0count * dim1count * dim2count);   
  
    /* get items one at a time from the arrayvalue's new native array,
     * and compare to expected value from the test data above.
     */
    for(i = 0; i < dim2count; i++)
    {
        subav1 = arrayvalue_get(av, i);
        CU_ASSERT_PTR_NOT_NULL_FATAL(subav1); 
        CU_ASSERT_EQUAL(((etch_object*)subav1)->obj_type, ETCHTYPEB_ARRAYVAL);
        CU_ASSERT_EQUAL(result = arrayvalue_count(subav1), dim1count);

        for(j = 0; j < dim1count; j++)
        {     
            subav2 = arrayvalue_get(subav1, j);
            CU_ASSERT_PTR_NOT_NULL_FATAL(subav2); 
            CU_ASSERT_EQUAL(((etch_object*)subav2)->obj_type, ETCHTYPEB_ARRAYVAL);
            CU_ASSERT_EQUAL(result = arrayvalue_count(subav2), dim0count);

            for(k = 0; k < dim0count; k++)
            {     
                byte expected_value = x[i][j][k];
                thisitem = arrayvalue_get(subav2, k);
                CU_ASSERT_PTR_NOT_NULL_FATAL(thisitem); 
                CU_ASSERT_EQUAL(((etch_object*)thisitem)->obj_type, ETCHTYPEB_PRIMITIVE);
                CU_ASSERT_EQUAL(((etch_object*)thisitem)->class_id, CLASSID_PRIMITIVE_BYTE);
                CU_ASSERT_EQUAL(((etch_byte*)thisitem)->value, expected_value);
            }
        }
    }

    etch_object_destroy(av);  /* destroy array value and content */

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
CU_pSuite test_etch_arrayvalue_suite()
{    
    CU_pSuite pSuite = CU_add_suite("suite_arrayvalue", init_suite, clean_suite);

    CU_add_test(pSuite, "test iterator over arraylist",   test_iterator_over_arraylist);
    #if(0)
    CU_add_test(pSuite, "test array value read ints",     read_test_integer);
    CU_add_test(pSuite, "test array value read strings",  read_test_string);
    CU_add_test(pSuite, "test array value write ints",    write_test_integer);
    CU_add_test(pSuite, "test array value write strings", write_test_string);
    #endif
    CU_add_test(pSuite, "test predefined exception",      exception_test_1);
    CU_add_test(pSuite, "test copy text exception",       exception_test_2);
    CU_add_test(pSuite, "test global text exception",     exception_test_3);
    CU_add_test(pSuite, "test create from byte[]",        test_create_from_1);
    CU_add_test(pSuite, "test create from int[]",         test_create_from_2);
    CU_add_test(pSuite, "test create from int64[]",       test_create_from_3);
    CU_add_test(pSuite, "test create from double[]",      test_create_from_4);
    CU_add_test(pSuite, "test create from string[]",      test_create_from_5);
    CU_add_test(pSuite, "test create from byte[][]",      test_create_from_10);
    CU_add_test(pSuite, "test create from short[][]",     test_create_from_11);
    CU_add_test(pSuite, "test create from string[][]",    test_create_from_12);
    CU_add_test(pSuite, "test create from object[][]",    test_create_from_13);
    CU_add_test(pSuite, "test create from byte[][][]",    test_create_from_21);
    CU_add_test(pSuite, "test arrayvalue to byte[]",      test_arrayvalue_to_nativearray_10);
    CU_add_test(pSuite, "test arrayvalue to string[][]",  test_arrayvalue_to_nativearray_20);
    CU_add_test(pSuite, "test arrayvalue to byte[][][]",  test_arrayvalue_to_nativearray_30);

    return pSuite; 
}
