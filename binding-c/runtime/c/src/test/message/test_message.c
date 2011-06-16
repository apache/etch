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
 * test_message.c -- test etch_message object
 */
#include "etch_runtime.h"
#include "etch_connection.h"
#include "etch_encoding.h"
#include "etch_thread.h"
#include "etch_log.h"
#include "etch_message.h"
#include "etch_objecttypes.h"
#include "etch_exception.h"
#include "etch_cache.h"
#include "etch_mem.h"

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

#if(0)
#define OBJTYPE_FAKETDI_IMPL ETCHTYPEB_INSTANCEDATA
#define CLASSID_FAKETDI_IMPL 0xf0
#define OBJTYPE_FAKETDO_IMPL ETCHTYPEB_INSTANCEDATA
#define CLASSID_FAKETDO_IMPL   0xf1
#define OBJTYPE_FAKETDI_VTABLE 0x88
#define OBJTYPE_FAKETDO_VTABLE 0x89
#define CLASSID_FAKETDI 0xf6
#define CLASSID_FAKETDO 0xf7
#endif

#define OBJTYPE_FAKEVF_IMPL ETCHTYPEB_INSTANCEDATA
#define CLASSID_FAKEVF_IMPL    0xf2
#define OBJTYPE_FAKEVF_VTABLE  0x90
#define OBJTYPE_FAKEVF_IMPL ETCHTYPEB_INSTANCEDATA
#define OBJTYPE_FAKEVF ETCHTYPEB_VALUEFACTORY
#define CLASSID_FAKEVF  0xf5

#define EXCPTEST_UNCHECKED_STATICTEXT 1
#define EXCPTEST_CHECKED_COPYTEXT     2
#define EXCPTEST_CHECKED_STATICTEXT   3


typedef struct testitems
{
    etch_type*  mt1;
    etch_type*  mt2;
    etch_type*  rmt;
    etch_field* mf1;
    etch_field* mf2;
    etch_field* mf3;
    etch_field* mf4;

    etch_string* s1;
    etch_string* s2;
    etch_int32*  n1;
    etch_int32*  n2;
    etch_int64*  l1;
    etch_int64*  l2;

    etch_field *mf_bool_d0_1;   
    etch_field *mf_bool_d0_2;   
    etch_field *mf_bool_d1_1;   
    etch_field *mf_bool_d1_2;  

    etch_field *mf_int32_d0_1;  
    etch_field *mf_int32_d0_2;  
    etch_field *mf_int32_d1_1;  
    etch_field *mf_int32_d1_2;  

    etch_field *mf_str_d0_1;   
    etch_field *mf_str_d0_2;   
    etch_field *mf_str_d1_1;  
    etch_field *mf_str_d1_2;  

    etch_field *mf_message_id;
    etch_field *mf_in_reply_to; 

    etch_hashtable* items;

} testitems;

enum objtyp {
	ETCHTYPE_VTABLE_FAKETDI = 0xfa,
	ETCHTYPE_VTABLE_FAKETDO = 0xfb,
	ETCHTYPE_VTABLE_FAKEVF  = 0xfc
};


/* = = = = = = = = = = = = = = =
 * VALUE FACTORY IMPLEMENTATION
 * = = = = = = = = = = = = = = =
 */

/**
 * fake_vf_impl
 * instance data for a value factory implementation
 */
typedef struct fake_vf_impl
{
    etch_object object;

    etch_field* mf_message_id;
    etch_field* mf_in_reply_to;

} fake_vf_impl;


/**
 * destroy_fake_vf_impl
 * destructor for fake_vf_impl
 */
static int destroy_fake_vf_impl(void* data)
{
  fake_vf_impl* impl = (fake_vf_impl*)data;
    int result = verify_object((etch_object*)impl, OBJTYPE_FAKEVF_IMPL, CLASSID_FAKEVF_IMPL, 0);
    if (result == -1) return -1; /* object passed was not expected object */

    CU_ASSERT_PTR_NOT_NULL_FATAL(impl->mf_message_id);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(impl->mf_in_reply_to); 

    etch_object_destroy(impl->mf_message_id);
    etch_object_destroy(impl->mf_in_reply_to);

    etch_free(impl); 
    return 0;
}


/**
 * new_fake_vf_impl()
 * constructor for TDI implementation instance data
 */
static fake_vf_impl* new_fake_vf_impl(testitems* items)
{
    fake_vf_impl* data = etch_malloc(sizeof(fake_vf_impl), ETCHTYPEB_INSTANCEDATA);
    memset(data, 0, sizeof(fake_vf_impl));
    ((etch_object*)data)->obj_type = OBJTYPE_FAKEVF_IMPL;
    ((etch_object*)data)->class_id = CLASSID_FAKEVF_IMPL;

    data->mf_message_id  = (etch_field*)etch_object_clone_func(items->mf_message_id);
    data->mf_in_reply_to = (etch_field*)etch_object_clone_func(items->mf_in_reply_to);
 
    ((etch_object*)data)->destroy = destroy_fake_vf_impl;
    return data;
}


/*  
 * value factory virtual function overrides
 */

/**
 * fvf_get_message_id() -- valuefactory.get_message_id() implementation.
 * @return non-disposable reference to id object, or null if not found.
 */
static etch_int64* fvf_get_message_id (void* vfData, etch_message* msg)
{
    etch_value_factory* vf = (etch_value_factory*)vfData;
    etch_int64* id = NULL;
    fake_vf_impl* data = (fake_vf_impl*) vf->impl;

    id = (etch_int64*) etch_message_get(msg, data->mf_message_id);

    /* return (etch_int64*) verifyx((etch_object*) id, 0, 
       CLASSID_PRIMITIVE_INT64, EXCPTYPE_INTERNALERR); */

    return id;
}


/**
 * fvf_set_message_id() -- valuefactory.set_message_id() implementation.
 */
static int fvf_set_message_id (void* vfData, etch_message* msg, etch_int64* id)
{
    etch_value_factory* vf = (etch_value_factory*)vfData;
    fake_vf_impl* data  = (fake_vf_impl*) vf->impl;
    etch_field* keycopy = (etch_field*)etch_object_clone_func(data->mf_message_id);

    const int result = etch_message_put(msg, keycopy, (etch_object*) id);
    return result;
}


/**
 * fvf_get_in_reply_to() -- valuefactory.get_in_reply_to() implementation.
 */
static etch_int64* fvf_get_in_reply_to (void* vfData, etch_message* msg)
{
    etch_value_factory* vf = (etch_value_factory*)vfData;
    etch_int64* id = NULL;
    fake_vf_impl* data = (fake_vf_impl*) vf->impl;

    id = (etch_int64*) etch_message_get(msg, data->mf_in_reply_to);

    return id;
}


/**
 * fvf_set_in_reply_to() -- valuefactory.set_message_id() implementation.
 */
static int fvf_set_in_reply_to (void* vfData, etch_message* msg, etch_int64* id)
{
    etch_value_factory* vf = (etch_value_factory*)vfData;
    fake_vf_impl* data  = (fake_vf_impl*) vf->impl;
    etch_field* keycopy = clone_field(data->mf_in_reply_to);

    /* FYI this copy of the key is put to etch_message* sent message, and gets   
     * freed in msg.destroy(), message owns memory other than vf and type 
     */
    return etch_message_put(msg, keycopy, (etch_object*) id);
}

/*  
 * value factory constructors/destructors
 */


#if 0
/**
 * fakevf_close() ala java test
 */
static void fakevf_close(etch_value_factory* vf)  
{
    etch_object_destroy(vf);  
}
#endif

#define CLASSID_FAKE_VALUEFACTORY 200

/**
 * new_fake_value_factory()
 * constructor for value factory implementation  
 */
static etch_value_factory* new_fake_value_factory(testitems* items)  
{
    etch_value_factory* fakevf = NULL;
    i_value_factory* vtab = NULL;
    const int VTABSIZE = sizeof(i_value_factory);
    const unsigned short CLASS_ID = CLASSID_FAKE_VALUEFACTORY;
   
    fakevf = new_value_factory(0);
   
    vtab = etch_cache_find(get_vtable_cachehkey(CLASS_ID), 0);

    if(!vtab)  
    {    
        vtab = new_vtable(((etch_object*)fakevf)->vtab, VTABSIZE, CLASS_ID);

        /* override four i_value_factory methods */
        vtab->get_message_id  = fvf_get_message_id;
        vtab->set_message_id  = fvf_set_message_id;
        vtab->get_in_reply_to = fvf_get_in_reply_to;
        vtab->set_in_reply_to = fvf_set_in_reply_to;      

        ((etch_object*)vtab)->vtab = (vtabmask*)((etch_object*)fakevf)->vtab;  /* chain vtabs */
        etch_cache_insert(((etch_object*)vtab)->hashkey, vtab, 0);  
    } 

    CU_ASSERT_EQUAL_FATAL(((etch_object*)vtab)->class_id, CLASS_ID);

    ((etch_object*)fakevf)->vtab = (vtabmask*)vtab;  /* set override vtab */

    fakevf->impl = (etch_object*) new_fake_vf_impl(items); /* create vf instance data */

    return fakevf;
}


#if(0)

/* = = = = = = = = = =    
 * TDI IMPLEMENTATION
 * = = = = = = = = = =     
 */

/**
 * fake_tdi_impl
 * instance data for a TDI implementation
 */
typedef struct fake_tdi_impl
{
    etch_object object;

    etch_type*    tdi_type;
    etch_message* xmessage;
    etch_iterator iterator;  
    etch_value_factory* vf;
    testitems*    testdata;
    byte started, done, ended, is_owned_message;

} fake_tdi_impl;



/**
 * destroy_fake_tdi_impl
 * memory cleanup handler for fake_tdi_impl
 */
static int destroy_fake_tdi_impl(void* data)
{
  fake_tdi_impl* impl = (fake_tdi_impl*)data;
    etch_destructor destroy = NULL;
    int result = verify_object((etch_object*)impl, OBJTYPE_FAKETDI_IMPL, CLASSID_FAKETDI_IMPL, 0);
    if (result == -1) return -1; /* object passed was not expected object */

    /* type is a reference, it does not belong to the tdi. message is created  
     * in the tdi, but ownership is assumed to transfer to the caller when  
     * message_read returns. if this is not the case then is_owned_message 
     * must have been set elsewhere.
     */
    if (impl->is_owned_message) 
    {   /* not the default case, see comment above */
        CU_ASSERT_PTR_NOT_NULL_FATAL(impl->xmessage);  
        etch_object_destroy(impl->xmessage);  
    }
 
    etch_free(impl); 
    return 0;
}


/**
 * new_fake_tdi_impl()
 * constructor for TDI implementation instance data
 */
static fake_tdi_impl* new_fake_tdi_impl(etch_value_factory* vf, etch_type* static_type, testitems* testdata)
{
    fake_tdi_impl* data = (fake_tdi_impl*) 
        new_object(sizeof(fake_tdi_impl), ETCHTYPEB_INSTANCEDATA, CLASSID_FAKETDI_IMPL);

    /* value factory and type are references, memory not owned by tdi */
    data->tdi_type = static_type;     
    data->vf = vf;  
    data->testdata = testdata;

    data->destroy = destroy_fake_tdi_impl;        
    return data;
}


/**
 * faketdi_start_message() overrides tdi_start_message()
 */
static etch_message* faketdi_start_message(tagged_data_input* tdi) 
{
    int result = 0;
    fake_tdi_impl* tdidata = NULL;
    etch_message* newmessage = NULL;
    etch_value_factory* vf = NULL;
    const int READONLY = TRUE, DEFSIZE = 0, DEFDELTA = 0;
    
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi->impl);

    tdidata = (fake_tdi_impl*) tdi->impl; /* validate instance data */
    result  = verify_object((etch_object*)tdidata, OBJTYPE_FAKETDI_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdidata->vf);

    etch_object_destroy(NULL); /* ensure we can call into instance destructor */
    CU_ASSERT_EQUAL(result,-1);

    CU_ASSERT_EQUAL(tdidata->started,FALSE);
    CU_ASSERT_EQUAL(tdidata->done,FALSE);
    CU_ASSERT_EQUAL(tdidata->ended,FALSE);

    tdidata->started = TRUE;

    /* create a message (struct) to receive the elements read. a message owns all  
     * its memory except the value factory and type (note that the vf might need 
     * to be refcounted for that reason, to ensure that if the service shuts down 
     * while a message is in the pipeline, a vf is not destroyed prior to a message
     * which references it). the type is passed through to the underlying struct. 
     * a struct owns all its memory except type, so it is assigned a reference to 
     * the tdi's type, which is an object global to the service. ownership of the 
     * struct, and by extension the messsage, memory, is assumed to be passed to 
     * the caller when this function returns. if instead it was desired that the 
     * tdi own the message and hence the struct, an indicator of such and an 
     * associated message dtor call, would need to be coded in the tdi and tdi 
     * destructor implementation.
     */
    newmessage = new_message (tdidata->tdi_type, 0, tdidata->vf);  

    tdidata->xmessage = newmessage;
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdidata->xmessage);
    
    set_iterator(&tdidata->iterator, /* establish iterator on the input data */
        tdidata->testdata->items, &tdidata->testdata->items->iterable);

    /* while an empty dataset is valid in the general case, 
     * this test assumes that input is non-empty */
    result = tdidata->iterator.has_next(&tdidata->iterator); 
    CU_ASSERT_EQUAL(result,TRUE);

    return newmessage;
}

 
/**
 * faketdi_read_struct_element() overrides tdi_read_struct_element()
 */
static int faketdi_read_struct_element(tagged_data_input* tdi, etch_struct_element* out_se)   
{
    int result = 0;
    fake_tdi_impl* data = NULL;
    etch_iterator* iterator = NULL;
    etch_destructor destroy = NULL;    
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
 * faketdi_end_message() overrides tdi_end_struct()
 */
static etch_message* faketdi_end_message(tagged_data_input* tdi, etch_message* msg) 
{
    int result = 0;
    fake_tdi_impl* data = NULL;
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi->impl);

    data = (fake_tdi_impl*) tdi->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDI_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_EQUAL_FATAL(data->xmessage, msg); 

    CU_ASSERT_EQUAL(data->started,TRUE);
    CU_ASSERT_EQUAL(data->done,TRUE);
    CU_ASSERT_EQUAL(data->ended,FALSE);
    			
	data->ended = TRUE;
    return msg;
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
    etch_object_destroy(tdi);  
}


/**
 * new_fake_tdi()
 * constructor for TDI implementation  
 */
static tagged_data_input* new_fake_tdi(etch_type* static_type, etch_value_factory* vfobj, testitems* data)  
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
        vtab->start_message       = faketdi_start_message;  
        vtab->end_message         = faketdi_end_message;    
        vtab->read_struct_element = faketdi_read_struct_element;

        ((etch_object*)vtab)->vtab = faketdi->vtab;      /* chain parent vtab to override vtab */
        cache_insert(vtab->hashkey, vtab, FALSE);
    } 

    CU_ASSERT_EQUAL_FATAL(((etch_object*)vtab)->class_id, CLASS_ID);

    ((etch_object*)faketdi)->vtab = vtab;  /* set override vtab */

    faketdi->impl = (etch_object*) 
        new_fake_tdi_impl(vfobj, static_type, data); /* create TDI instance data */

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


/* = = = = = = = = = = 
 * TDO IMPLEMENTATION
 * = = = = = = = = = = 
 */

/**
 * fake_tdo_impl
 * instance data for a TDO implementation
 */
typedef struct fake_tdo_impl
{
    etch_object object;

    byte started, ended, closed;
    etch_message* xmessage;  /* reference */
    etch_hashtable* fakeout; /* fake output sink */  

} fake_tdo_impl;


/**
 * destroy_fake_tdo_impl
 * memory cleanup handler for fake_tdo_impl
 * we do not destroy the message and underlying struct, this is a reference. 
 */
static int destroy_fake_tdo_impl(void* data)
{
  fake_tdo_impl* impl = (fake_tdo_impl*)data;
    etch_destructor destroy = NULL;
    int result = verify_object((etch_object*)impl, OBJTYPE_FAKETDO_IMPL, CLASSID_FAKETDO_IMPL, NULL);
    if (result == -1) return -1; /* object passed was not expected object */

    /* destroy the fake output sink, in this case a map. we do not want the map to
     * destroy its content because the message owns that content, and we did not
     * in this case give the output sink copies of that content. 
    */
    etch_object_destroy(impl->fakeout); 

    etch_free(impl);    
    return 0;
}


/**
 * new_fake_tdo_impl()
 * constructor for TDO implementation instance data
 */
static fake_tdo_impl* new_fake_tdo_impl(etch_message* msg)
{
    fake_tdo_impl* data = (fake_tdo_impl*) 
        new_object(sizeof(fake_tdo_impl), ETCHTYPEB_INSTANCEDATA, CLASSID_FAKETDO_IMPL);

    data->xmessage = msg; /* reference, not owned */

    data->destroy = destroy_fake_tdo_impl;    
    return data;
}


/**
 * faketdo_start_message() overrides tdo_start_message()
 */
static etch_message* faketdo_start_message(tagged_data_output* tdo, etch_message* msg)  
{
    int result = 0;
    fake_tdo_impl* data = NULL;
    etch_destructor destructor = NULL;
  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo->impl);

    data = (fake_tdo_impl*) tdo->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDO_IMPL, 0, (void**) &destructor);
    CU_ASSERT_EQUAL_FATAL(result,0);
    
    etch_object_destroy(NULL); /* ensure we can call instance destructor */
    CU_ASSERT_EQUAL(result,-1);

    CU_ASSERT_EQUAL(data->started,FALSE);
    CU_ASSERT_EQUAL(data->ended,FALSE);

    CU_ASSERT_EQUAL_FATAL(msg, data->xmessage);
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->fakeout); 

    data->started = TRUE;    
    return msg;
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
 * faketdo_end_message() overrides tdo_end_message()
 */
static etch_message* faketdo_end_message(tagged_data_output* tdo, etch_message* msg)
{
    int result = 0;
    fake_tdo_impl* data = NULL;
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdo->impl);

    data = (fake_tdo_impl*)  tdo->impl; /* validate instance data */
    result = verify_object((etch_object*)data, OBJTYPE_FAKETDO_IMPL, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(data->xmessage); 
 
    CU_ASSERT_EQUAL(data->started,TRUE);
    CU_ASSERT_EQUAL(data->ended,FALSE);
    CU_ASSERT_EQUAL(data->closed,FALSE);
    CU_ASSERT_EQUAL(data->xmessage, msg);
			
	data->ended = TRUE;
    return msg;
}


/**
 * faketdo_close() ala java test
 */
static void faketdo_close(tagged_data_output* tdo)  
{
    etch_object_destroy(tdo); 
}


/**
 * new_fake_tdo()
 * constructor for TDO implementation  
 */
static tagged_data_output* new_fake_tdo(etch_message* msg)  
{
    tagged_data_output* faketdo = NULL;
    i_tagged_data_output*  vtab = NULL;
    fake_tdo_impl* impl = NULL;
    const unsigned short CLASS_ID = CLASSID_FAKETDO;
    CU_ASSERT_PTR_NOT_NULL_FATAL(msg);

    faketdo = new_tagged_data_output();     

    vtab = cache_find(get_vtable_cachehkey(CLASS_ID), 0);

    if(!vtab)  
    {   
        vtab = new_vtable(((etch_object*)faketdo)->vtab, sizeof(i_tagged_data_output), CLASS_ID);

        /* override three i_tagged_data_output methods */
        vtab->start_message        = faketdo_start_message;
        vtab->end_message          = faketdo_end_message;
        vtab->write_struct_element = faketdo_write_struct_element;

        ((etch_object*)vtab)->vtab = faketdo->vtab;       /* chain parent vtab */
    
        cache_insert(vtab->hashkey, vtab, FALSE);
    } 

    CU_ASSERT_EQUAL_FATAL(((etch_object*)vtab)->class_id, CLASS_ID);

    ((etch_object*)faketdo)->vtab = vtab;  /* set override vtab */

    impl = new_fake_tdo_impl(msg);  /* construct TDO instance data */
    CU_ASSERT_PTR_NOT_NULL_FATAL(impl); 
            
    impl->fakeout = new_hashtable(16); /* instantiate fake output target */
    CU_ASSERT_PTR_NOT_NULL_FATAL(impl->fakeout);
    impl->fakeout->content_type = ETCHHASHTABLE_CONTENT_OBJECT;

    faketdo->impl = (etch_object*) impl;

    return faketdo; 
}

#endif /* #if(0) */


/* = = = = = = = = = = 
 * TEST DATA  
 * = = = = = = = = = = 
 */

/* 
 * testdata_clear_handler()
 * memory callback on testdata clear
 */
static int testdata_clear_handler (void* key, void* value)  
{
    return TRUE; /* indicate free handled */
}

/* 
 * new_testitems()
 * instantiate and return types and fields used in tests
 */
static testitems* new_testitems()
{
    testitems* items = etch_malloc(sizeof(struct testitems), 0);
    items->mt1 = new_type(L"mt1");
    items->mt2 = new_type(L"mt2");
    items->rmt = new_type(L"rmt");
    items->mf1 = new_field(L"x");
    items->mf2 = new_field(L"y");
    items->mf3 = new_field(L"s1");
    items->mf4 = new_field(L"s2");

    items->s1 = new_stringw(L"string1");
    items->s2 = new_stringw(L"string2");
    items->n1 = new_int32(1);
    items->n2 = new_int32(2);
    items->l1 = new_int64(12345);
    items->l2 = new_int64(54321);

    items->mf_bool_d0_1  = new_field(L"f1");
    items->mf_bool_d0_2  = new_field(L"f2");
    items->mf_bool_d1_1  = new_field(L"f3");
    items->mf_bool_d1_2  = new_field(L"f4");

    items->mf_int32_d0_1 = new_field(L"f5");
    items->mf_int32_d0_2 = new_field(L"f6");
    items->mf_int32_d1_1 = new_field(L"f7");
    items->mf_int32_d1_2 = new_field(L"f8");

    items->mf_str_d0_1   = new_field(L"f9");
    items->mf_str_d0_2   = new_field(L"fa");
    items->mf_str_d1_1   = new_field(L"fb");
    items->mf_str_d1_2   = new_field(L"fc");

    items->mf_message_id  = new_field(L"_messageId"); 
    items->mf_in_reply_to = new_field(L"_inReplyTo"); 
 
    etchtype_put_validator(items->mt1, clone_field(items->mf_message_id), (etch_object*) etchvtor_int64_get(0));
    etchtype_put_validator(items->mt1, clone_field(items->mf1), (etch_object*) etchvtor_int32_get(0));
    etchtype_put_validator(items->mt1, clone_field(items->mf2), (etch_object*) etchvtor_int32_get(0));
    etchtype_put_validator(items->mt1, clone_field(items->mf3), (etch_object*) etchvtor_string_get(0));
    etchtype_put_validator(items->mt1, clone_field(items->mf4), (etch_object*) etchvtor_string_get(0));

    etchtype_put_validator(items->rmt, clone_field(items->mf_message_id), (etch_object*) etchvtor_int64_get(0));
    etchtype_put_validator(items->rmt, clone_field(items->mf_in_reply_to),(etch_object*) etchvtor_int64_get(0));

    #if(0)
    etchtype_put_validator(items->mt1, items->mf_bool_d0_1->clone(items->mf_bool_d0_1), (etch_object*) etchvtor_boolean_get(0));
    etchtype_put_validator(items->mt1, items->mf_bool_d0_2->clone(items->mf_bool_d0_2), (etch_object*) etchvtor_boolean_get(0));

    etchtype_put_validator(items->mt1, items->mf_bool_d1_1->clone(items->mf_bool_d1_1), (etch_object*) etchvtor_boolean_get(1));
    etchtype_put_validator(items->mt1, items->mf_bool_d1_2->clone(items->mf_bool_d1_2), (etch_object*) etchvtor_boolean_get(1));

    etchtype_put_validator(items->mt1, items->mf_int32_d0_1->clone(items->mf_int32_d0_1), (etch_object*) etchvtor_int32_get(0));
    etchtype_put_validator(items->mt1, items->mf_int32_d0_2->clone(items->mf_int32_d0_2), (etch_object*) etchvtor_int32_get(0));

    etchtype_put_validator(items->mt1, items->mf_int32_d1_1->clone(items->mf_int32_d1_1), (etch_object*) etchvtor_int32_get(1));
    etchtype_put_validator(items->mt1, items->mf_int32_d1_2->clone(items->mf_int32_d1_2), (etch_object*) etchvtor_int32_get(1));

    etchtype_put_validator(items->mt1, items->mf_str_d0_1->clone(items->mf_str_d0_1), (etch_object*) etchvtor_string_get(0));
    etchtype_put_validator(items->mt1, items->mf_str_d0_2->clone(items->mf_str_d0_2), (etch_object*) etchvtor_string_get(0));

    etchtype_put_validator(items->mt1, items->mf_str_d1_1->clone(items->mf_str_d1_1), (etch_object*) etchvtor_string_get(1));
    etchtype_put_validator(items->mt1, items->mf_str_d1_2->clone(items->mf_str_d1_2), (etch_object*) etchvtor_string_get(1));
    #endif

    items->items = new_hashtable(0);
    items->items->is_readonly_keys   = FALSE;
    items->items->is_readonly_values = FALSE; 
    items->items->is_tracked_memory  = TRUE;
    items->items->content_type = ETCHHASHTABLE_CONTENT_OBJECT;
    items->items->freehook = testdata_clear_handler;
    return items;
}

/* 
 * destroy_testitems()
 * free memory for test data
 */
static void destroy_testitems(testitems* items)
{
    etch_object_destroy(items->mt1);
    etch_object_destroy(items->mt2);
    etch_object_destroy(items->rmt);
    etch_object_destroy(items->mf1);
    etch_object_destroy(items->mf2); 
    etch_object_destroy(items->mf3);
    etch_object_destroy(items->mf4);

    etch_object_destroy(items->s1); 
    etch_object_destroy(items->s2); 
    etch_object_destroy(items->n1); 
    etch_object_destroy(items->n2); 
    etch_object_destroy(items->l1); 
    etch_object_destroy(items->l2); 

    etch_object_destroy (items->mf_message_id); 
    etch_object_destroy(items->mf_in_reply_to);

    etch_object_destroy(items->mf_bool_d0_1);   
    etch_object_destroy(items->mf_bool_d0_2);   
    etch_object_destroy(items->mf_bool_d1_1);  
    etch_object_destroy(items->mf_bool_d1_2);   

    etch_object_destroy(items->mf_int32_d0_1); 
    etch_object_destroy(items->mf_int32_d0_2);  
    etch_object_destroy(items->mf_int32_d1_1);  
    etch_object_destroy(items->mf_int32_d1_2);  

    etch_object_destroy(items->mf_str_d0_1);    
    etch_object_destroy(items->mf_str_d0_2);    
    etch_object_destroy(items->mf_str_d1_1);    
    etch_object_destroy(items->mf_str_d1_2);   

    etchvtor_clear_cache(); /* free all cached validators */

    destroy_hashtable(items->items, FALSE, FALSE);

    etch_free(items);   
}


#if 0
/* 
 * compare_lists()
 * compares specified message content with content of some other map.
 * returns boolean indicating equal or not.
 */
static int compare_lists(etch_message* msg, etch_hashtable* otherdata)  
{
    int thiscount = 0, fakecount = 0, result = 0;
    etch_structvalue* svx = NULL;
    etch_iterator  iterator;
  	etch_hashitem  hashbucket;
    etch_hashitem* sventry = &hashbucket;

    svx = msg->sv;
    CU_ASSERT_PTR_NOT_NULL_FATAL(svx);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(svx->items);

    thiscount = ((struct i_hashtable*)((etch_object*)svx->items)->vtab)->count(svx->items->realtable,0,0); 
    fakecount = ((struct i_hashtable*)((etch_object*)otherdata)->vtab)->count(otherdata->realtable,0,0); 
    CU_ASSERT_EQUAL(fakecount, thiscount);
    if (fakecount != thiscount) return FALSE;

    set_iterator(&iterator, otherdata, &otherdata->iterable);

    while(((struct i_iterable*)iterator.object.vtab)->has_next(&iterator) && result == 0)
    {
        result = ((struct i_hashtable*)((etch_object*)svx->items)->vtab)->findh
            (svx->items->realtable, ((etch_object*)iterator.current_key)->hashkey, 
	     otherdata, (void**)&sventry);
        CU_ASSERT_EQUAL(iterator.current_value, sventry->value);

        ((struct i_iterable*)iterator.object.vtab)->next(&iterator);
    }

    return TRUE;
}

#endif

#if 0
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
     * note that structs always set such a callback on their backing store
     * in order to free key and value allocations.
     */
    return FALSE;
}

#endif
/* = = = = = = = = = = 
 * TESTS   
 * = = = = = = = = = = 
 */

/* 
 * run_iterator_test()
 * iterate over test collection, inserting pairs to specified message.
 * iterate over message, veriying that content matches test collection.
 */
static int run_iterator_test(etch_message* msg, testitems* data)
{
    etch_iterator* iterator  = NULL;
    int msgcount = 0, testcount = 0, result = 0;

    iterator = new_iterator(data->items, &data->items->iterable);

    CU_ASSERT_PTR_NOT_NULL_FATAL(iterator);
    CU_ASSERT_EQUAL(iterator->ordinal,1);

    while(iterator->has_next(iterator))
    {
        testcount++;

        result = etch_message_put(msg, iterator->current_key, iterator->current_value);

        CU_ASSERT_EQUAL(result,0);
        
        iterator->next(iterator);
    }
    
    set_iterator(iterator, msg->sv->items, &msg->sv->items->iterable);
    CU_ASSERT_EQUAL(iterator->ordinal,1);

    while(iterator->has_next(iterator))
    {
        msgcount++;
        iterator->next(iterator);
    }

    CU_ASSERT_EQUAL(testcount, msgcount);
    etch_object_destroy(iterator);
    return msgcount;
}


/* 
 * iterator_test()
 */
static void iterator_test(void)
{
    int result = 0;
    testitems* data = NULL;
    etch_hashtable* map = NULL;
    etch_message*   msg = NULL;
    etch_value_factory* vf = NULL; 

    etch_string *sval1 = NULL, *sval2 = NULL;
    etch_int32  *ival1 = NULL, *ival2 = NULL;
    etch_field  *skey1 = NULL, *skey2 = NULL;
    etch_field  *ikey1 = NULL, *ikey2 = NULL;

    data = new_testitems();
    map  = data->items;

    /* a message owns all its memory except the value factory and type, so we'll
     * make copies of all the testdata keys and values to be inserted into it */
    ikey1 = (etch_field*)etch_object_clone_func(data->mf1); 
    ikey2 = (etch_field*)etch_object_clone_func(data->mf2);
    skey1 = (etch_field*)etch_object_clone_func(data->mf3); 
    skey2 = (etch_field*)etch_object_clone_func(data->mf4);

    sval1 = (etch_string*)etch_object_clone_func(data->s1);  
    sval2 = (etch_string*)etch_object_clone_func(data->s2);
    ival1 = (etch_int32*)etch_object_clone_func(data->n1);  
    ival2 = (etch_int32*)etch_object_clone_func(data->n2);

    /* insert cloned fields and values to the test input collection */
    ((struct i_hashtable*)((etch_object*)map)->vtab)->inserth(map->realtable, ikey1, ival1, map, 0);
    ((struct i_hashtable*)((etch_object*)map)->vtab)->inserth(map->realtable, ikey2, ival2, map, 0);
    ((struct i_hashtable*)((etch_object*)map)->vtab)->inserth(map->realtable, skey1, sval1, map, 0);
    ((struct i_hashtable*)((etch_object*)map)->vtab)->inserth(map->realtable, skey2, sval2, map, 0);
   
    vf = new_fake_value_factory(data);

    CU_ASSERT_PTR_NOT_NULL_FATAL(vf);
    CU_ASSERT_FALSE_FATAL(is_etch_exception(vf));

    /* a message owns its memory except vf and type, so we give it global type */
    msg = new_message (data->mt1, 0, vf);  

    CU_ASSERT_PTR_NOT_NULL_FATAL(msg);
    CU_ASSERT_FALSE_FATAL(is_etch_exception(msg));
    
    result = run_iterator_test(msg, data);
    CU_ASSERT_EQUAL(result,4);  /* 4 items in message */

    etch_object_destroy(msg);
    etch_object_destroy(vf); 
  
    destroy_testitems(data);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}

/* 
 * reply_test
 * test message.reply 
 */
static void reply_test(void)
{
    int result = 0;
    etch_message* msg = NULL; 
    etch_message* newmsg = NULL;
    etch_type* replytype = NULL;
    etch_value_factory* vf = NULL;
    etch_int64* id_original = NULL;
    etch_int64* id_replied_to = NULL;
    int64 val_origid = 0, val_replyid = 0;
    testitems* data = new_testitems();

    id_original = data->l1;

    vf = new_fake_value_factory(data);
    
    msg = new_message (data->mt1, 0, vf);  /* message owns neither arg */

    result = etch_message_set_id(msg, (etch_int64*)etch_object_clone_func(id_original));
    CU_ASSERT_EQUAL_FATAL(result,0);

    replytype = data->rmt;    /* again, a message does not own a type */ 

    newmsg = etch_message_reply (msg, replytype);

    CU_ASSERT_PTR_NOT_NULL_FATAL(newmsg);
    CU_ASSERT_FALSE_FATAL(is_etch_exception(newmsg));

    result = is_equal_types(data->rmt, newmsg->sv->struct_type);
    CU_ASSERT_TRUE(result);
    CU_ASSERT_EQUAL(vf, newmsg->vf);

    id_replied_to = etch_message_get_in_reply_to(newmsg);

    CU_ASSERT_PTR_NOT_NULL_FATAL(id_replied_to);
    CU_ASSERT_FALSE_FATAL(is_etch_exception(id_replied_to));

    val_origid  = id_original->value;
    val_replyid = id_replied_to->value;   
    CU_ASSERT_EQUAL(val_origid, val_replyid);

    etch_object_destroy(newmsg);
    etch_object_destroy(msg);  
    etch_object_destroy(vf);   
    destroy_testitems(data);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


#if 0
/* 
 * run_exception_test
 * execute an individual exception test 
 */
static void run_exception_test(const int whichtest)
{
    etch_type* static_type = NULL;
    etch_value_factory* vf = NULL;
    testitems* data = new_testitems();

    static_type = new_type(L"type1");
    vf = new_fake_value_factory(data);

    #if(0)
    /* create a bogus TDI inheriting from tagged data input */
    tdi = new_fake_tdi(static_type, vf, NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(tdi);

    switch(whichtest)
    {   case EXCPTEST_UNCHECKED_STATICTEXT:
        case EXCPTEST_CHECKED_COPYTEXT:
        case EXCPTEST_CHECKED_STATICTEXT:
        {   CU_ASSERT_TRUE_FATAL(is_exception(tdi));
            #if IS_DEBUG_CONSOLE 
            wprintf(L"\ncaught %s exception on tdi\n", tdi->result->exception->excptext);          
            #endif   
        }
    }

    msg = etch_message_read(NULL); /* pass NULL for TDI */
    CU_ASSERT_PTR_NOT_NULL_FATAL(msg);
    CU_ASSERT_TRUE(is_exception(msg));
    excp = get_exception(msg);
    CU_ASSERT_EQUAL(excp->excptype, EXCPTYPE_ILLEGALARG);
    #if IS_DEBUG_CONSOLE 
    wprintf(L"\ncaught %s exception on message\n", msg->result->exception->excptext);          
    #endif   

    /* free TDI */
    faketdi_close(tdi);

    /* free struct, it is just a shell with no content other than the exception */
    etch_object_destroy(msg);

    #endif

    etch_object_destroy(vf);

    destroy_testitems(data);
    etch_object_destroy(static_type);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


#endif



/**
 * main   
 */
//int _tmain(int argc, _TCHAR* argv[])
CU_pSuite test_etch_message_suite()
{    
    CU_pSuite pSuite = CU_add_suite("suite_message", init_suite, clean_suite);
    CU_add_test(pSuite, "test iterator over message", iterator_test);  
    CU_add_test(pSuite, "message reply test", reply_test); 

    return pSuite;
}
