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
 * test_structvalue.c -- test etch_structvalue object
 */

#include "etch_runtime.h"
#include "etch_connection.h"
#include "etch_encoding.h"
#include "etch_log.h"
#include "etch_structval.h"
#include "etch_nativearray.h"
#include "etch_objecttypes.h"
#include "etch_exception.h"

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
    //this_teardown();
    etch_runtime_shutdown();
    return 0;
}

/* - - - - - - - - - - - - - - - - - - - - - - 
 * individual setup and teardown
 * - - - - - - - - - - - - - - - - - - - - - -
 */

static etch_type *mt1, *mt2;

static etch_field *mf_bool_d0_1;   
static etch_field *mf_bool_d0_2;   
static etch_field *mf_bool_d1_1;   
static etch_field *mf_bool_d1_2;  

static etch_field *mf_int32_d0_1;  
static etch_field *mf_int32_d0_2;  
static etch_field *mf_int32_d1_1;  
static etch_field *mf_int32_d1_2;  

static etch_field *mf_str_d0_1;   
static etch_field *mf_str_d0_2;   
static etch_field *mf_str_d1_1;  
static etch_field *mf_str_d1_2;  

static etch_hashtable* testdata;  

static int g_which_exception_test;

#if(0)
#define OBJTYPE_FAKETDI_IMPL ETCHTYPEB_INSTANCEDATA
#define CLASSID_FAKETDI_IMPL 0xf0
#define OBJTYPE_FAKETDO_IMPL ETCHTYPEB_INSTANCEDATA
#define CLASSID_FAKETDO_IMPL 0xf1
#define OBJTYPE_FAKETDI_VTABLE 0x88
#define OBJTYPE_FAKETDO_VTABLE 0x89
#define CLASSID_FAKETDI 0xf6
#define CLASSID_FAKETDO 0xf7
#endif

#define EXCPTEST_UNCHECKED_STATICTEXT 1
#define EXCPTEST_CHECKED_COPYTEXT     2
#define EXCPTEST_CHECKED_STATICTEXT   3


#if(0)

/**
 * fake_tdi_impl
 * instance data for a TDI implementation
 */
typedef struct fake_tdi_impl
{
    etch_object object;

    etch_type* tdi_type;
    byte started, done, ended, is_owned_struct;
    etch_structvalue* xstruct;
    etch_iterator iterator;  

} fake_tdi_impl;


/**
 * fake_tdo_impl
 * instance data for a TDO implementation
 */
typedef struct fake_tdo_impl
{
    etch_object object;

    byte started, ended, closed;
    etch_structvalue* xstruct;
    etch_hashtable* fakeout; /* fake output stream */  

} fake_tdo_impl;


/**
 * fake_tdi_impl_destroy_handler
 * memory cleanup handler for fake_tdi_impl
 */
static int destroy_fake_tdi_impl(fake_tdi_impl* impl)
{
    etch_destructor destroy = NULL;
    int result = verify_object((etch_object*)impl, OBJTYPE_FAKETDI_IMPL, CLASSID_FAKETDI_IMPL, NULL);
    if (result == -1) return -1; /* object passed was not expected object */

    /* type is a refrence, it does  not belong to the tdi. struct is created  
     * in the tdi, but ownership is assumed to transfer to the caller when  
     * structvalue_read returns. if this is not the case then is_owned_struct 
     * must have been set elsewhere
     */
    if (impl->is_owned_struct) 
    {   /* not the default case, see comment above */
        CU_ASSERT_PTR_NOT_NULL_FATAL(impl->xstruct);  
        etch_object_destroy(impl->xstruct);  
    }
 
    etch_free(impl); 
   
    return 0;
}


/**
 * fake_tdo_impl_destroy_handler
 * memory cleanup handler for fake_tdo_impl
 */
static int destroy_fake_tdo_impl(fake_tdo_impl* impl)
{
    etch_destructor destroy = NULL;
    int result = verify_object((etch_object*)impl, OBJTYPE_FAKETDO_IMPL, CLASSID_FAKETDO_IMPL, NULL);
    if (result == -1) return -1; /* object passed was not expected object */

    /* destroy the fake output receptor, in this case a hashtable. we don't want the
     * hashtable to destroy its content because its content is references to someone
     * else's content, in this case the host struct value. 
    */
    destroy_hashtable(impl->fakeout, FALSE, FALSE); 

    /* we do not destroy the struct value, this is merely a reference */ 
    /* impl->((etch_object*)xstruct)->vtab->destroy(impl->xstruct); */

    etch_free(impl);    
    return 0;
}


/**
 * new_fake_tdi_impl()
 * constructor for TDI implementation instance data
 */
static fake_tdi_impl* new_fake_tdi_impl(etch_type* static_type)
{
   fake_tdi_impl* data = (fake_tdi_impl*) 
        new_object(sizeof(fake_tdi_impl), ETCHTYPEB_INSTANCEDATA, CLASSID_FAKETDI_IMPL);

   /* assign type - this is a reference, it is not our memory */
    data->tdi_type = static_type;       

    /* set destructor */
    data->destroy = destroy_fake_tdi_impl;
    return data;
}


/**
 * new_fake_tdo_impl()
 * constructor for TDO implementation instance data
 */
static fake_tdo_impl* new_fake_tdo_impl(etch_structvalue* sv)
{
    fake_tdo_impl* data = (fake_tdo_impl*) 
         new_object(sizeof(fake_tdo_impl), ETCHTYPEB_INSTANCEDATA, CLASSID_FAKETDO_IMPL);

    data->xstruct  = sv;

    /* set destructor */
    data->destroy = destroy_fake_tdo_impl;    
    return data;
}


enum objtyp ETCHTYPE_VTABLE_FAKETDI = 0xf0;
enum objtyp ETCHTYPE_VTABLE_FAKETDO = 0xf1;


/**
 * faketdi_start_struct() overrides tdi_start_struct()
 */
static etch_structvalue* faketdi_start_struct(tagged_data_input* tdi) 
{
    int result = 0;
    fake_tdi_impl* data = NULL;
    etch_structvalue* newstructval = NULL;
    const int READONLY = TRUE, DEFSIZE = 0, DEFDELTA = 0;
    
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi->impl);

    data = (fake_tdi_impl*) tdi->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDI_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = etch_object_destroy(NULL); /* ensure we can call into instance data destructor */
    CU_ASSERT_EQUAL(result,-1);

    CU_ASSERT_EQUAL(data->started,FALSE);
    CU_ASSERT_EQUAL(data->done,FALSE);
    CU_ASSERT_EQUAL(data->ended,FALSE);

    data->started = TRUE;

    /* create a struct to receive the elements read. a struct owns all its memory
     * so we give it a copy of the tdi's type. ownership of the struct is assumed
     * to be passed to the caller when this function returns. if instead it was
     * desired that the tdi own the struct, data.is_owned_struct would have to
     * be set subsequently elsewhere. 
     * note that sv no longer owns type, so we no longer clone the type
     */
    newstructval = new_structvalue (data->tdi_type, 0);  

    data->xstruct = newstructval;
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->xstruct);

    /* establish iterator on the fake input data */
    set_iterator(&data->iterator, testdata, &testdata->iterable);

    /* while an empty dataset is valid in the general case, 
     * this test assumes that input is non-empty */
    result = data->iterator.vtab->has_next(&data->iterator); 
    CU_ASSERT_EQUAL(result,TRUE);

    return data->xstruct;
}


/**
 * faketdo_start_struct() overrides tdo_start_struct()
 */
static int faketdo_start_struct(tagged_data_output* tdo, etch_structvalue* structval)  
{
    int result = 0;
    fake_tdo_impl* data = NULL;
  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo->impl);

    data = (fake_tdo_impl*) tdo->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDO_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);
    
    result = data->destroy(NULL); /* ensure we can call instance data destructor */
    CU_ASSERT_EQUAL(result,-1);

    CU_ASSERT_EQUAL(data->started,FALSE);
    CU_ASSERT_EQUAL(data->ended,FALSE);

    CU_ASSERT_EQUAL_FATAL(structval, data->xstruct);
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->fakeout); 

    data->started = TRUE;    
    return 0;
}



/**
 * faketdo_write_struct_element() overrides tdo_write_struct_element()
 */
static int faketdo_write_struct_element(tagged_data_output* tdo, etch_field* key, etch_object* val)  
{
    int result = 0;
    fake_tdo_impl*  data  = NULL;
    etch_hashtable* fakeout = NULL;
  	etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket;
  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo);  /* validate parameters */
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo->impl);
    CU_ASSERT_PTR_NOT_NULL_FATAL(val);

    result = is_good_field(key);
    CU_ASSERT_EQUAL_FATAL(result,TRUE);

    data = (fake_tdo_impl*) tdo->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDO_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    fakeout = data->fakeout;
    CU_ASSERT_PTR_NOT_NULL_FATAL(fakeout);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(fakeout->realtable);  

    CU_ASSERT_EQUAL(data->started,TRUE);
    CU_ASSERT_EQUAL(data->ended, FALSE);
    CU_ASSERT_EQUAL(data->closed,FALSE);

    /* do the write to the fake output */ 
    result = ((etch_object*)fakeout)->vtab->insert(fakeout->realtable, key, HASHSIZE_FIELD, val, 0, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    /* verify the write by accessing the item just written */
    result = ((etch_object*)fakeout)->vtab->find(fakeout->realtable, key, HASHSIZE_FIELD, 0, &myentry);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_EQUAL_FATAL(myentry->key, (void*)key);
    CU_ASSERT_EQUAL_FATAL(myentry->value, val);
    
	return 0;
}

 
/**
 * faketdi_read_struct_element() overrides tdi_read_struct_element()
 */
static int faketdi_read_struct_element(tagged_data_input* tdi, etch_struct_element* out_se)   
{
    int result = 0;
    fake_tdi_impl* data = NULL;
    etch_iterator* iterator = NULL;
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi->impl);
    CU_ASSERT_PTR_NOT_NULL_FATAL(out_se);

    data = (fake_tdi_impl*) tdi->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDI_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);
  
    CU_ASSERT_EQUAL(data->started,TRUE);
    CU_ASSERT_EQUAL(data->done,FALSE);
    CU_ASSERT_EQUAL(data->ended,FALSE);
    iterator = &data->iterator;

	if (iterator->has_next(iterator)) 
	{   
        CU_ASSERT_EQUAL_FATAL(result,0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(iterator->current_key);
        CU_ASSERT_PTR_NOT_NULL_FATAL(iterator->current_value);

    	out_se->key   = (etch_field*) iterator->current_key;
        out_se->value = (etch_object*)    iterator->current_value;

        iterator->next(iterator);
		return TRUE;
	}
	
	data->done = TRUE;
	return FALSE;
}


/**
 * faketdi_end_struct() overrides tdi_end_struct()
 */
static int faketdi_end_struct(tagged_data_input* tdi, etch_structvalue* sv)   
{
    int result = 0;
    fake_tdi_impl* data = NULL;
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi->impl);

    data = (fake_tdi_impl*) tdi->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDI_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->xstruct); 
 
    CU_ASSERT_EQUAL(data->started,TRUE);
    CU_ASSERT_EQUAL(data->done,TRUE);
    CU_ASSERT_EQUAL(data->ended,FALSE);
    CU_ASSERT_EQUAL(data->xstruct,sv);
			
	data->ended = TRUE;
    return 0;
}


/**
 * faketdo_end_struct() overrides tdo_end_struct()
 */
static int faketdo_end_struct(tagged_data_output* tdo, struct etch_structvalue* sv) 
{
    int result = 0;
    fake_tdo_impl* data = NULL;
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo->impl);

    data = (fake_tdo_impl*) tdo->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDO_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->xstruct); 
 
    CU_ASSERT_EQUAL(data->started,TRUE);
    CU_ASSERT_EQUAL(data->ended,FALSE);
    CU_ASSERT_EQUAL(data->closed,FALSE);
    CU_ASSERT_EQUAL(data->xstruct, sv);
			
	data->ended = TRUE;
    return 0;
}


/**
 * faketdi_close() ala java test
 * also tested here is that we can call base methods on a derived object. we invoke 
 * the TDI destructor here, which is an overide of the etchobject destructor.
 * the TDI destructor walks the vtable chain to its parent, and invokes the parent
 * destructor to destroy etchobject content such as any exception, and finally
 * the object itself.
 */
static void faketdi_close(tagged_data_input* tdi)  
{
    tdi->destroy(tdi);  
}


/**
 * faketdo_close() ala java test
 */
static void faketdo_close(tagged_data_output* tdo)  
{
    tdo->destroy(tdo);  
}


/**
 * new_fake_tdi()
 * constructor for TDI implementation  
 */
static tagged_data_input* new_fake_tdi(etch_type* static_type)  
{
    tagged_data_input* faketdi = NULL;
    i_tagged_data_input* vtab  = NULL;
    const unsigned short CLASS_ID = CLASSID_FAKETDI;
    CU_ASSERT_EQUAL_FATAL(is_good_type(static_type),TRUE);
   
    faketdi = new_tagged_data_input();

    vtab = cache_find(get_vtable_cachehkey(CLASS_ID), 0);

    if(!vtab)  
    {    
        vtab = new_vtable(((etch_object*)faketdi)->vtab, sizeof(i_tagged_data_input), CLASS_ID);

        /* override three i_tagged_data_input methods */
        vtab->start_struct = faketdi_start_struct;  
        vtab->end_struct   = faketdi_end_struct;    
        vtab->read_struct_element = faketdi_read_struct_element;

        ((etch_object*)vtab)->vtab = faketdi->vtab;      /* chain parent vtab to override vtab */
        cache_insert(vtab->hashkey, vtab, FALSE);
    } 

    CU_ASSERT_EQUAL_FATAL(((etch_object*)vtab)->class_id, CLASS_ID);

    ((etch_object*)faketdi)->vtab = vtab;  /* set override vtab */

    faketdi->impl = (etch_object*) 
        new_fake_tdi_impl(static_type); /* create TDI instance data */

    switch(g_which_exception_test)
    {   case EXCPTEST_UNCHECKED_STATICTEXT: 
             etch_throw((etch_object*) faketdi, EXCPTYPE_NULLPTR, NULL, 0);  
             break;   
        case EXCPTEST_CHECKED_COPYTEXT:   
             etch_throw((etch_object*) faketdi, EXCPTYPE_CHECKED_BOGUS, L"copied text", ETCHEXCP_COPYTEXT | ETCHEXCP_FREETEXT);  
             break; 
        case EXCPTEST_CHECKED_STATICTEXT:   
             etch_throw((etch_object*) faketdi, EXCPTYPE_CHECKED_BOGUS, local_excp_text, ETCHEXCP_STATICTEXT);  
             break;       
    }

    return faketdi;
}



/**
 * new_fake_tdo()
 * constructor for TDO implementation  
 */
static tagged_data_output* new_fake_tdo(etch_structvalue* sv)  
{
    tagged_data_output* faketdo = NULL;
    i_tagged_data_output*  vtab = NULL;
    fake_tdo_impl* impl = NULL;
    const unsigned short CLASS_ID = CLASSID_FAKETDO;
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv);

    faketdo = new_tagged_data_output();     

    vtab = cache_find(get_vtable_cachehkey(CLASS_ID), 0);

    if(!vtab)  
    {   
        vtab = new_vtable(((etch_object*)faketdo)->vtab, sizeof(i_tagged_data_output), CLASS_ID);

        /* override three i_tagged_data_output methods */
        vtab->start_struct = faketdo_start_struct;
        vtab->end_struct   = faketdo_end_struct;
        vtab->write_struct_element = faketdo_write_struct_element;

        ((etch_object*)vtab)->vtab = faketdo->vtab;  /* chain parent vtab to override vtab */
    
        cache_insert(vtab->hashkey, vtab, FALSE);
    } 

    CU_ASSERT_EQUAL_FATAL(((etch_object*)vtab)->class_id, CLASS_ID);

    ((etch_object*)faketdo)->vtab = vtab;         /* set override vtab */

    impl = new_fake_tdo_impl(sv); /* set TDO instance data */
    CU_ASSERT_PTR_NOT_NULL_FATAL(impl); 
                  
    impl->fakeout = new_hashtable(16); 
    CU_ASSERT_PTR_NOT_NULL_FATAL(impl->fakeout);
    impl->fakeout->is_readonly_keys  = impl->fakeout->is_readonly_values = FALSE;
    impl->fakeout->is_tracked_memory = TRUE;
    impl->fakeout->content_type = ETCHHASHTABLE_CONTENT_OBJECT;

    faketdo->impl = (etch_object*) impl;

    return faketdo;
}

#endif /* #if(0) */


/* 
 * load_testdata_string()
 * load testdata map with some ETCH_STRING objects
 */
static int load_testdata_string()   
{
    int i = 0, numitems = 4, result = 0;
    etch_field* newkey  = NULL;
    etch_string* newobj = NULL;
    wchar_t* str0 = L"now ", *str1 = L"is  ", *str2 = L"the ", *str3 = L"time";
    wchar_t* strings[4] = { str0, str1, str2, str3 };

    for(; i < numitems; i++)
    {
        newkey  = new_field(strings[i]);      /* testdata map CAN free keys */
        newobj  = new_stringw(strings[i]);
        result  = ((struct i_hashtable*)((etch_object*)testdata)->vtab)->inserth(testdata->realtable, newkey, newobj, 0, 0);
    }

    return numitems;
}


/* 
 * load_testdata_int()
 * load testdata array with some ETCH_INT objects
 */
static int load_testdata_int()   
{
    const int numitems = 4;
    int i = 0, result = 0;
    etch_field* newkey = NULL;
    etch_int32* newobj = NULL;
    wchar_t* str0 = L"fld1", *str1 = L"fld2", *str2 = L"fld3", *str3 = L"fld4";
    wchar_t* keys[4] = { str0, str1, str2, str3 };
    int ints[4] = { 1, 2, 3, 4 };

    for(; i < numitems; i++)
    {
        newkey = new_field(keys[i]);
        newobj = new_int32(ints[i]);  /* testdata table can free both keys and values */
        result = ((struct i_hashtable*)((etch_object*)testdata)->vtab)->inserth(testdata->realtable, newkey, newobj, 0, 0);
    }

    return numitems;
}


/* 
 * testdata_clear_handler()
 * memory callback on testdata clear
 */
static int testdata_clear_handler (void* keyData, void* valueData)  
{
    etch_object_destroy((etch_object*)keyData);
    etch_object_destroy((etch_object*)valueData);
    return TRUE;
}


/* 
 * new_testdata()
 * create testdata map and load it up with data objects
 */
static int new_testdata(const int datatype)   
{
    int count = 0;
    #if IS_DEBUG_CONSOLE
    etch_iterator iterator;
    #endif
    g_which_exception_test = 0;
    testdata = new_hashtable(0); 
    testdata->is_readonly_keys   = FALSE;
    testdata->is_readonly_values = FALSE; 
    testdata->is_tracked_memory  = TRUE;
    testdata->content_type = ETCHHASHTABLE_CONTENT_OBJECT;
    testdata->freehook = testdata_clear_handler;
 
    switch(datatype)
    { case 1: count = load_testdata_int();    break;
      case 2: count = load_testdata_string(); break;
      default: return -1;
    }

    #if IS_DEBUG_CONSOLE
    printf("\n");
    #pragma warning (disable:4313)
    set_iterator(&iterator, testdata, &testdata->iterable);

    while(iterator.vtab->has_next(&iterator))
    {
        printf("testdata %08x %08x\n", iterator.current_key, iterator.current_value);
        iterator.vtab->next(&iterator);
    }
    printf("\n");
    #endif

    mt1 = new_type(L"one"); 
    mt2 = new_type(L"two");

    mf_bool_d0_1  = new_field(L"f1");
    mf_bool_d0_2  = new_field(L"f2");
    mf_bool_d1_1  = new_field(L"f3");
    mf_bool_d1_2  = new_field(L"f4");

    mf_int32_d0_1 = new_field(L"f5");
    mf_int32_d0_2 = new_field(L"f6");
    mf_int32_d1_1 = new_field(L"f7");
    mf_int32_d1_2 = new_field(L"f8");

    mf_str_d0_1   = new_field(L"f9");
    mf_str_d0_2   = new_field(L"fa");
    mf_str_d1_1   = new_field(L"fb");
    mf_str_d1_2   = new_field(L"fc");

    etchtype_put_validator(mt1, (etch_field*)etch_object_clone_func(mf_bool_d0_1), (etch_object*) etchvtor_boolean_get(0));
    etchtype_put_validator(mt1, (etch_field*)etch_object_clone_func(mf_bool_d0_2), (etch_object*) etchvtor_boolean_get(0));

    etchtype_put_validator(mt1, (etch_field*)etch_object_clone_func(mf_bool_d1_1), (etch_object*) etchvtor_boolean_get(1));
    etchtype_put_validator(mt1, (etch_field*)etch_object_clone_func(mf_bool_d1_2), (etch_object*) etchvtor_boolean_get(1));

    etchtype_put_validator(mt1, (etch_field*)etch_object_clone_func(mf_int32_d0_1), (etch_object*) etchvtor_int32_get(0));
    etchtype_put_validator(mt1, (etch_field*)etch_object_clone_func(mf_int32_d0_2), (etch_object*) etchvtor_int32_get(0));

    etchtype_put_validator(mt1, (etch_field*)etch_object_clone_func(mf_int32_d1_1), (etch_object*) etchvtor_int32_get(1));
    etchtype_put_validator(mt1, (etch_field*)etch_object_clone_func(mf_int32_d1_2), (etch_object*) etchvtor_int32_get(1));

    etchtype_put_validator(mt1, (etch_field*)etch_object_clone_func(mf_str_d0_1), (etch_object*) etchvtor_string_get(0));
    etchtype_put_validator(mt1, (etch_field*)etch_object_clone_func(mf_str_d0_2), (etch_object*) etchvtor_string_get(0));

    etchtype_put_validator(mt1, (etch_field*)etch_object_clone_func(mf_str_d1_1), (etch_object*) etchvtor_string_get(1));
    etchtype_put_validator(mt1, (etch_field*)etch_object_clone_func(mf_str_d1_2), (etch_object*) etchvtor_string_get(1));
 
    return count;
}

/* 
 * destroy_testdata()
 * destroy testdata map and content
 */
static void destroy_testdata()  
{
    destroy_hashtable(testdata, TRUE, TRUE);

    etch_object_destroy(mt1);  
    etch_object_destroy(mt2);  

    destroy_field(mf_bool_d0_1);
    destroy_field(mf_bool_d0_2);
    destroy_field(mf_bool_d1_1);
    destroy_field(mf_bool_d1_2);
    destroy_field(mf_int32_d0_1);
    destroy_field(mf_int32_d0_2);
    destroy_field(mf_int32_d1_1);
    destroy_field(mf_int32_d1_2);
    destroy_field(mf_str_d0_1);
    destroy_field(mf_str_d0_2);
    destroy_field(mf_str_d1_1);
    destroy_field(mf_str_d1_2);

    etchvtor_clear_cache(); /* free all cached validators */
}

#if 0

/* 
 * compare_lists()
 * compares testdata content with specified struct content.
 * returns boolean indicating equal or not.
 */
static int compare_lists(etch_structvalue* svx)  
{
    int thiscount = 0, testcount = 0, result = 0;
    etch_iterator  iterator;
  	etch_hashitem  hashbucket;
    etch_hashitem* sventry = &hashbucket;

    CU_ASSERT_PTR_NOT_NULL_FATAL(svx);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(svx->items);

    thiscount = ((struct i_hashtable*)((etch_object*)svx->items)->vtab)->count(svx->items->realtable,0,0); 
    testcount = ((struct i_hashtable*)((etch_object*)testdata)->vtab)->count(testdata->realtable,0,0); 
    CU_ASSERT_EQUAL(testcount, thiscount);
    if (testcount != thiscount) return FALSE;

    set_iterator(&iterator, testdata, &testdata->iterable);

    while(iterator.has_next(&iterator) && result == 0)
    {
        result = ((struct i_hashtable*)((etch_object*)svx->items)->vtab)->findh
	  (svx->items->realtable, ((etch_object*)iterator.current_key)->hashkey, testdata, (void**)&sventry);

        CU_ASSERT_EQUAL(iterator.current_value, sventry->value);

        iterator.next(&iterator);
    }

    return TRUE;
}


/* 
 * hashtable_clear_handler()
 * override callback from hashtable during clear()
 */
static int hashtable_clear_handler (void* key, void* value)  
{
    /* prior to calling clear() on any hashtable htab, set: 
     *    htab.callback = hashtable_clear_handler; 
     * and the hashtable will call back here for each item in the table,
     * replacing any such handler installed. return FALSE to have the hashtable 
     * proceed as it would with a handler. save off the existing handler and call 
     * it, to proceed as if you had not replaced the handler. return TRUE to  
     * indicate you handled the free and the hashtable should not attempt to free 
     * memory for key and/or value, if in fact it would have done so otherwise. 
     * note that structvalue always sets such a callback on its backing store
     * in order to free key and value allocations.
     */
    return FALSE;
}

/* 
 * hashtable_do_nada_handler()
 * override callback from hashtable during clear()
 * this callback does nothing and returns TRUE, so no content memory is freed.
 */
static int hashtable_do_nada_handler (void* key, void* value)  
{
    return TRUE;
}
#endif

/* 
 * run_iterator_test
 * test struct value iterator
 */
static void run_iterator_test(void)
{
    int result = 0, count = 0;
    etch_iterator iterator;
    etch_int32* int_obj_1 = NULL;
    etch_int32* int_obj_2 = NULL;
    etch_nativearray* int_array_1 = NULL;
    etch_nativearray* int_array_2 = NULL;
    etch_structvalue* sv = NULL;
    etch_field* fldkey   = NULL;

    new_testdata(1); /* instantiate types and fields we use here */

    /* memory for field keys and object values is given to the struct to manage. 
     * once a key and value are "put" to the struct, we give up responsibility
     * for freeing them. therefore when we put an object to the struct, we clone 
     * a type to use as the key. note that when we assign the struct a type, 
     * however, we do not relinquish ownership of the memory for that etch_type.
     * this is because types are global to the service in the binding, 
     * and are freed only at service shutdown.
     */
 
    int_obj_1 = new_int32(123);  
    int_obj_2 = new_int32(234);  

    int_array_1 = new_etch_nativearray(CLASSID_ARRAY_INT32, sizeof(int), 1, 3, 0, 0);
    int_array_2 = new_etch_nativearray(CLASSID_ARRAY_INT32, sizeof(int), 1, 5, 0, 0);

    sv = new_structvalue(mt1, 0);   

    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 
    result = is_etch_exception(sv);
    CU_ASSERT_EQUAL(result, FALSE); 
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv->items);

    /* if structvalue has ETCH_STRUCTVAL_VALIDATE_ON_PUT compiled,
     * these structvalue.put()s will be validated. */     

    fldkey = clone_field(mf_int32_d0_1);
    result = structvalue_put(sv, fldkey, (etch_object*) int_obj_1);
    CU_ASSERT_EQUAL(result, 0);

    set_iterator(&iterator, sv->items, &sv->items->iterable);
    CU_ASSERT_EQUAL(iterator.has_next(&iterator), TRUE);

    fldkey = clone_field(mf_int32_d0_2); 
    result = structvalue_put(sv, fldkey, (etch_object*) int_obj_2);
    CU_ASSERT_EQUAL(result, 0);

    set_iterator(&iterator, sv->items, &sv->items->iterable);
    CU_ASSERT_TRUE(iterator.has_next(&iterator));

    iterator.next(&iterator);
    CU_ASSERT_TRUE(iterator.has_next(&iterator));

    iterator.next(&iterator);
    CU_ASSERT_FALSE(iterator.has_next(&iterator));

    fldkey = clone_field(mf_int32_d1_1); 
    result = structvalue_put(sv, fldkey, (etch_object*) int_array_1);
    CU_ASSERT_EQUAL(result, 0);

    fldkey = clone_field(mf_int32_d1_2); 
    result = structvalue_put(sv, fldkey, (etch_object*) int_array_2);
    CU_ASSERT_EQUAL(result, 0);

    set_iterator(&iterator, sv->items, &sv->items->iterable);

    while(iterator.has_next(&iterator))
    { count++;
      iterator.next(&iterator);
    }
    CU_ASSERT_EQUAL(count,4);


    /* free memory for struct and its instance data and content */
    etch_object_destroy(sv); 
    destroy_testdata();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * run_exception_test
 *  
 */
static void run_exception_test(const int whichtest)
{
    etch_type* static_type = NULL;
    static_type = new_type(L"type1");

    #if(0)

    /* create a bogus TDI inheriting from tagged data input */
    tdi = new_fake_tdi(static_type);
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);

    switch(whichtest)
    {   case EXCPTEST_UNCHECKED_STATICTEXT:
        case EXCPTEST_CHECKED_COPYTEXT:
        case EXCPTEST_CHECKED_STATICTEXT:
        {   CU_ASSERT_TRUE_FATAL(is_exception(tdi));
            #if IS_DEBUG_CONSOLE 
            wprintf(L"\ncaught %s exception on tdi\n", tdiobj->result->exception->excptext);          
            #endif   
        }
    }

    sv = structvalue_read(NULL); /* pass NULL for TDI */
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv);
    CU_ASSERT_TRUE(is_exception(tdi));
    excp = get_exception(sv);
    CU_ASSERT_EQUAL(excp->excptype, EXCPTYPE_ILLEGALARG);
    #if IS_DEBUG_CONSOLE 
    wprintf(L"\ncaught %s exception on sv\n", svobj->result->exception->excptext);          
    #endif   

    /* free TDI */
    faketdi_close(tdi);

    /* free struct, it is just a shell with no content other than the exception */
    etch_object_destroy(sv);

    #endif

    /* destroy the type allocated above.
     * in most tests we would NOT do this since the struct would own it after we
     * created the struct. however we passed a null tdi parameter above to the 
     * method which would have created the struct. the type which would have
     * been assigned to the struct was in the tdi that we did not pass. The tdi
     * will not destroy its type. however note that in the real world this 
     * situation would not occur. all types would be global, and we will always
     * create a copy of the type for the struct. see faketdi_start_struct() for
     * an example, note that it clones the type for the new struct.  
     */
    etch_object_destroy(static_type);

    /* destroy testdata list and content */
    destroy_testdata();

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
    int testcount = 0, thiscount = 0;
    struct i_iterable* vtab = NULL;
    new_testdata(1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(testdata);
    testcount = ((struct i_hashtable*)((etch_object*)testdata)->vtab)->count(testdata->realtable, 0, 0);
    CU_ASSERT_NOT_EQUAL(testcount, 0);

    iterator = new_iterator(testdata, &testdata->iterable);

    CU_ASSERT_PTR_NOT_NULL_FATAL(iterator);
    CU_ASSERT_EQUAL_FATAL(iterator->ordinal,1);
    thiscount = 1;
    vtab = (i_iterable*)((etch_object*)iterator)->vtab;
    while(vtab->has_next(iterator))
          thiscount += (vtab->next(iterator) == 0);  
        
    CU_ASSERT_EQUAL(testcount, thiscount);

    destroy_iterator(iterator);
    destroy_testdata();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


#if 0
/* 
 * exception_test_1
 */
static void exception_test_1(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(1)) > 0)
        run_exception_test(EXCPTEST_UNCHECKED_STATICTEXT); 
}
#endif

/* 
 * exception_test_2
 */
static void exception_test_2(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(1)) > 0)
        run_exception_test(EXCPTEST_CHECKED_COPYTEXT); 
}

#if 0

/* 
 * exception_test_3
 */
static void exception_test_3(void)
{
    int itemcount = 0;
    if ((itemcount = new_testdata(1)) > 0)
        run_exception_test(EXCPTEST_CHECKED_STATICTEXT); 
}

#endif


/**
 * main   
 */
//int wmain( int argc, wchar_t* argv[], wchar_t* envp[])
CU_pSuite test_etch_structvalue_suite()
{    
    CU_pSuite pSuite = CU_add_suite("suite_structvalue", init_suite, clean_suite);

    CU_add_test(pSuite, "test iterator over hashtable", test_iterator_over_hashtable);
    CU_add_test(pSuite, "test structvalue iterator", run_iterator_test);
    CU_add_test(pSuite, "test sv exceptions", exception_test_2);

    return pSuite;
}
