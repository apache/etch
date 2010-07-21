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
 * test_transport.c
 * test delivery service etc
 */
#include "etch_runtime.h"
#include "etch_transport.h"
#include "etch_thread.h"
#include "etch_default_value_factory.h"
#include "etch_plain_mailbox_manager.h"
#include "etch_objecttypes.h"
#include "etch_general.h"
#include "etch_map.h"
#include "etch_log.h"

#include <stdio.h>
#include "CUnit.h"

#define IS_DEBUG_CONSOLE TRUE


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


static default_value_factory* new_bogus_valuefactory();

/* - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * unit test support
 * - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

#define FAKEID_TYPE_ADD         1 
#define FAKEID_TYPE_ADD_RESULT  2 
#define FAKEID_FIELD_X          3 
#define FAKEID_FIELD_Y          4
#define FAKEID_FIELD_RESULT     5
#define THISTEST_WHO_VALUE 0x5151

static unsigned short CLASSID_MY_VF;
static unsigned short CLASSID_MY_VF_VTAB;
static unsigned short CLASSID_MY_VF_IMPL;
static unsigned short CLASSID_MY_IMPL_TP;
static unsigned short CLASSID_MY_IMPL_SM;

static etch_resources*         g_my_resources;
static default_value_factory*  g_my_vf;
static vf_idname_map*          g_my_typemap;
static class_to_type_map*      g_my_c2tmap;
static etch_who*               g_who;

/**
 * setup_this_test()
 * set up an individual unit test
 */
static int setup_this_test()
{
    CLASSID_MY_VF      = get_dynamic_classid();
    CLASSID_MY_VF_VTAB = get_dynamic_classid();
    CLASSID_MY_VF_IMPL = get_dynamic_classid();
    CLASSID_MY_IMPL_TP = get_dynamic_classid();
    CLASSID_MY_IMPL_SM = get_dynamic_classid();  

    g_my_vf = new_bogus_valuefactory();
    set_etchobj_static_all(g_my_vf);   /* so resources map will not destroy */ 
 
    /* get resources map populated with transport resources such as thread pools */
    g_my_resources = get_etch_transport_resources(NULL);
    etch_resources_add(g_my_resources, ETCH_RESXKEY_MSGIZER_VALUFACT, (etch_object*) g_my_vf); 
   
    g_who = new_who(new_int32(THISTEST_WHO_VALUE));

    #if(IS_DEBUG_CONSOLE)
    printf("\n");
    #endif
    return 0;
}


/**
 * teardown_this_test()
 * tear down an individual unit test
 */
static void teardown_this_test()
{ 
    etch_object_destroy(g_my_resources);

    if (g_my_vf) 
    {   /* we clear the set_etchobj_static_all() on the g_my_vf value factory  
         * and as a result we can then destroy it */
        clear_etchobj_static_all(g_my_vf);
        etch_object_destroy(g_my_vf);
    }

    etch_object_destroy(g_my_c2tmap);
    etch_object_destroy(g_my_typemap);
    
 
    etch_object_destroy(g_who);

    g_my_resources = NULL;
    g_my_vf = NULL;
    g_who = NULL;

    etchvf_free_builtins(); 
}


/* - - - - - - - - - -
 * value factory
 * - - - - - - - - - - 
 */

/**
 * my_valufactory_impl
 * value factory instance data object
 */
typedef struct my_valufactory_impl
{
    etch_object object;

    etch_type*      mt_add;
    etch_type*      mt_add_result;
    etch_field*     mf_x;
    etch_field*     mf_y;
    etch_field*     mf_result;

} my_valufactory_impl;


/**
 * destroy_my_valufactory_impl()
 * destructor for inheriting value factory instance data
 */
static int destroy_my_valufactory_impl(void* data)
{
    my_valufactory_impl* impl = (my_valufactory_impl*)data;
    if (NULL == impl) return -1;

    if (!is_etchobj_static_content(impl))
    {
 	    destroy_static_type(impl->mt_add);
        destroy_static_type(impl->mt_add_result);
        destroy_static_field(impl->mf_x); 
        destroy_static_field(impl->mf_y); 
        destroy_static_field(impl->mf_result);  
    }

    return destroy_objectex((etch_object*) impl);
}


/**
 * new_my_valufactory_impl()
 * constructor for inheriting value factory instance data
 */
static my_valufactory_impl* new_my_valufactory_impl()
{
    unsigned short class_id = CLASSID_MY_VF_IMPL? CLASSID_MY_VF_IMPL: 
        (CLASSID_MY_VF_IMPL = get_dynamic_classid());

    my_valufactory_impl* impl = (my_valufactory_impl*) new_object
        (sizeof(my_valufactory_impl), ETCHTYPEB_VALUEFACTIMP, class_id);

    ((etch_object*)impl)->destroy = destroy_my_valufactory_impl;

    impl->mt_add = new_static_type(L"add");
    impl->mt_add_result = new_static_type(L"add_result");
    impl->mf_x = new_static_field(L"x");
    impl->mf_y = new_static_field(L"y");
    impl->mf_result = new_static_field(L"xresult");

    /* we replace generated ids with 1-byte IDs to make test data buffers easier to construct */
    impl->mt_add->id        = FAKEID_TYPE_ADD;
    impl->mt_add_result->id = FAKEID_TYPE_ADD_RESULT;
    impl->mf_x->id          = FAKEID_FIELD_X;
    impl->mf_y->id          = FAKEID_FIELD_Y;
    impl->mf_result->id     = FAKEID_FIELD_RESULT;

    etchtype_put_validator(impl->mt_add, clone_field(impl->mf_x), 
        (etch_object*) etchvtor_int32_get(0));
    etchtype_put_validator(impl->mt_add, clone_field(impl->mf_y), 
        (etch_object*) etchvtor_int32_get(0));
    etchtype_put_validator(impl->mt_add, clone_field(builtins._mf__message_id), 
        (etch_object*) etchvtor_int64_get(0));

    etchtype_put_validator(impl->mt_add_result, clone_field(impl->mf_result), 
        (etch_object*) etchvtor_int32_get(0));
    etchtype_put_validator(impl->mt_add_result, clone_field(builtins._mf__message_id), 
        (etch_object*) etchvtor_int64_get(0));
    etchtype_put_validator(impl->mt_add_result, clone_field(builtins._mf__in_reply_to), 
        (etch_object*) etchvtor_int64_get(0));

    return impl;
}


/**
 * new_bogus_valuefactory()
 * constructor for value factory version 2 inheriting from default_value_factory
 */
static default_value_factory* new_bogus_valuefactory()
{
    etchparentinfo* inheritlist = NULL;
    my_valufactory_impl* impl = NULL;

    /* instantiate the new value factory.  
     * this vf does NOT own its type maps since we supply them here. 
     * however if we wanted to abandon ownership, we could clear the
     * vf.is_own_types and vf.is_own_class_to_type flags here.
     */
    g_my_typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(g_my_typemap);
    /* since we explicitly instantiate a type map, and since we explicitly destroy
     * the test's custom types, we want the type maps destructor to not destroy
     * the map content. overriding the map's content clear callback is one way 
     * to do this. */
    g_my_typemap->freehook = etch_noop_clear_handler;
    g_my_c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(g_my_c2tmap);
    defvf_initialize_static(g_my_typemap, g_my_c2tmap);
    g_my_vf = new_default_value_factory(g_my_typemap, g_my_c2tmap);
    CU_ASSERT_PTR_NOT_NULL_FATAL(g_my_vf);

   /* ensure parent type keys exist in the (one-based) inheritance list. 
    * parent class of our custom vf is default_value_factory.
    * inheritance list is used by validators and object assignment logic.
    */
    inheritlist = get_vtab_inheritance_list((etch_object*)g_my_vf, 2, 1, CLASSID_MY_VF_VTAB);
    inheritlist[1].o.obj_type = ETCHTYPEB_VALUEFACTORY;  
    inheritlist[1].c.class_id = CLASSID_VALUEFACTORY;  /* parent class */
    ((etch_object*)g_my_vf)->class_id = CLASSID_MY_VF;  /* our class */

    /* instantiate the custom vf's instance data and assign it to the vf. 
     * the impl comprises all data specific to the inheriting class, including 
     * data and methods if any. the default value factory destructor will call 
     * the destructor on the vf's impl object.
     */  
    impl = new_my_valufactory_impl();
    g_my_vf->impl = (etch_object*) impl;

    ((struct i_value_factory*)((etch_object*)g_my_vf)->vtab)->add_type(g_my_vf, impl->mt_add);
    ((struct i_value_factory*)((etch_object*)g_my_vf)->vtab)->add_type(g_my_vf, impl->mt_add_result);

    return g_my_vf;
}


/* - - - - - - - - - - - - - - - - - - - - -
 * unit tests
 * - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * test_setup()
 */
static void test_setup(void)
{
    setup_this_test();

    do
    {   

    } while(0);
    
    teardown_this_test();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_tcpds_construction()
 * test tcp delivery service constructor and destructor
 */
static void test_tcpds_construction(void)
{
    setup_this_test();

    do 
    {   etch_tcp_delivery_service* tcpds = NULL;
        etch_tcp_connection* nullconnection = NULL;
        /* h_url* url = new_url(L"http://www.cisco.com:9999/cuae:?Messagizer.format=binary"); */
        /* messagizer format is supplied in resource map - see get_etch_transport_resources() */
        etch_url* url = new_url(L"http://www.cisco.com:9999/cuae"); 
        etch_server_factory* impl_factory = new_server_factory (NULL, NULL, NULL, NULL);
        impl_factory->in_resx = g_my_resources;

        tcpds = new_tcp_delivery_service(url, (etch_factory_params*) impl_factory, nullconnection);  

        CU_ASSERT_PTR_NOT_NULL_FATAL(tcpds);  

        etch_object_destroy(tcpds);    

        etch_object_destroy(url);
        etch_object_destroy(impl_factory);

    } while(0);
    
    teardown_this_test();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}



/**
 * test_transport_construction()
 * test delivery service constructor and destructor
 */
static void test_transport_construction(void)
{
    setup_this_test();

    do 
    {   etch_server_factory* impl_factory = new_server_factory (NULL, NULL, NULL, NULL);
        etch_tcp_connection* nullconnection = NULL;
        i_delivery_service* ds = NULL;
        impl_factory->in_resx = g_my_resources;

        ds = new_etch_transport(L"http://www.cisco.com:9999/cuae", 
            (etch_factory_params*) impl_factory, nullconnection);  

        CU_ASSERT_PTR_NOT_NULL_FATAL(ds);  

        etch_object_destroy(ds); /* destroys transport implementation via interface */
        etch_object_destroy(impl_factory);

    } while(0);
    
    teardown_this_test();

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
CU_pSuite test_etch_transport_suite()
{
    CU_pSuite ps = CU_add_suite("transport test suite", init_suite, clean_suite);

    CU_add_test(ps, "test test setup and teardown", test_setup);
    CU_add_test(ps, "test tcp delivery service constructor", test_tcpds_construction);
    CU_add_test(ps, "test transport constructor", test_transport_construction);
       
    return ps;
}
