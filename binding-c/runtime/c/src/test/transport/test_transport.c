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

#include "apr_time.h" /* some apr must be included first */
#include "etch_transport.h"  
#include "etchthread.h"

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etch_defvalufact.h"
#include "etch_plainmboxmgr.h"
#include "etchmap.h"
#include "etchlog.h"
#include "etch_global.h"
#include "etchlog.h"

default_value_factory* new_bogus_valuefactory();


/* - - - - - - - - - - - - - - 
 * unit test infrastructure
 * - - - - - - - - - - - - - -
 */

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
    return 0;
}

int this_teardown()
{    
    return 0;
}


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

unsigned short CLASSID_MY_VF;
unsigned short CLASSID_MY_VF_VTAB;
unsigned short CLASSID_MY_VF_IMPL;
unsigned short CLASSID_MY_IMPL_TP;
unsigned short CLASSID_MY_IMPL_SM;

etch_resources*         g_my_resources;
default_value_factory*  g_my_vf;
etch_who*               g_who;

/**
 * setup_this_test()
 * set up an individual unit test
 */
int setup_this_test()
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
    etch_resources_add(g_my_resources, ETCH_RESXKEY_MSGIZER_VALUFACT, (objmask*) g_my_vf); 
   
    g_who = new_who(new_int32(THISTEST_WHO_VALUE), TRUE);

    #if(IS_DEBUG_CONSOLE)
    printf("\n");
    #endif
    return 0;
}


/**
 * teardown_this_test()
 * tear down an individual unit test
 */
void teardown_this_test()
{ 
    if (g_my_resources)  
    {   /* we did a set_etchobj_static_all() on the g_my_vf value factory  
         * and as a result the map will not destroy it. if we had not done
         * so, the vf would have been destroyed with the resources map. */
        g_my_resources->destroy(g_my_resources);
    }

    if (g_my_vf) 
    {   /* we clear the set_etchobj_static_all() on the g_my_vf value factory  
         * and as a result we can then destroy it */
        clear_etchobj_static_all(g_my_vf);
        g_my_vf->destroy(g_my_vf);
    }
 
    if (g_who) 
        g_who->destroy(g_who);

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
    unsigned int    hashkey;  
    unsigned short  obj_type; 
    unsigned short  class_id;
    struct objmask* vtab;  
    int  (*destroy)(void*);
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;
    struct objmask* parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;

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
int destroy_my_valufactory_impl(my_valufactory_impl* impl)
{
    if (NULL == impl) return -1;
    if (impl->refcount > 0 && --impl->refcount > 0) return -1;  

    if (!is_etchobj_static_content(impl))
    {
 	    destroy_static_type(impl->mt_add);
        destroy_static_type(impl->mt_add_result);
        destroy_static_field(impl->mf_x); 
        destroy_static_field(impl->mf_y); 
        destroy_static_field(impl->mf_result);  
    }

    return destroy_objectex((objmask*) impl);
}


/**
 * new_my_valufactory_impl()
 * constructor for inheriting value factory instance data
 */
my_valufactory_impl* new_my_valufactory_impl()
{
    unsigned short class_id = CLASSID_MY_VF_IMPL? CLASSID_MY_VF_IMPL: 
        (CLASSID_MY_VF_IMPL = get_dynamic_classid());

    my_valufactory_impl* impl = (my_valufactory_impl*) new_object
        (sizeof(my_valufactory_impl), ETCHTYPEB_VALUEFACTIMP, class_id);

    impl->destroy = destroy_my_valufactory_impl;

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
        (objmask*) etchvtor_int32_get(0));
    etchtype_put_validator(impl->mt_add, clone_field(impl->mf_y), 
        (objmask*) etchvtor_int32_get(0));
    etchtype_put_validator(impl->mt_add, clone_field(builtins._mf__message_id), 
        (objmask*) etchvtor_int64_get(0));

    etchtype_put_validator(impl->mt_add_result, clone_field(impl->mf_result), 
        (objmask*) etchvtor_int32_get(0));
    etchtype_put_validator(impl->mt_add_result, clone_field(builtins._mf__message_id), 
        (objmask*) etchvtor_int64_get(0));
    etchtype_put_validator(impl->mt_add_result, clone_field(builtins._mf__in_reply_to), 
        (objmask*) etchvtor_int64_get(0));

    return impl;
}


/**
 * new_bogus_valuefactory()
 * constructor for value factory version 2 inheriting from default_value_factory
 */
default_value_factory* new_bogus_valuefactory()
{
    etchparentinfo* inheritlist = NULL;
    my_valufactory_impl* impl = NULL;

    /* establish global dynamic class IDs for the custom vf. 
     * these global objects are generated by the etch compiler.
     */
    const unsigned short class_id_vf = CLASSID_MY_VF? CLASSID_MY_VF: 
         (CLASSID_MY_VF = get_dynamic_classid());

    const unsigned short class_id_vf_vtab = CLASSID_MY_VF_VTAB? CLASSID_MY_VF_VTAB: 
         (CLASSID_MY_VF_VTAB = get_dynamic_classid());

    /* instantiate the new value factory.  
     * this vf does NOT own its type maps since we supply them here. 
     * however if we wanted to abandon ownership, we could clear the   
     * vf.is_own_types and vf.is_own_class_to_type flags here.
     */    
    g_my_vf = new_default_value_factory(NULL, NULL);

   /* ensure parent type keys exist in the (one-based) inheritance list. 
    * parent class of our custom vf is default_value_factory.
    * inheritance list is used by validators and object assignment logic.
    */
    inheritlist = get_vtab_inheritance_list((objmask*)g_my_vf, 2, 1, CLASSID_MY_VF_VTAB);
    inheritlist[1].obj_type = ETCHTYPEB_VALUEFACTORY;  
    inheritlist[1].class_id = CLASSID_VALUEFACTORY;  /* parent class */
    g_my_vf->class_id = CLASSID_MY_VF;  /* our class */

    /* instantiate the custom vf's instance data and assign it to the vf. 
     * the impl comprises all data specific to the inheriting class, including 
     * data and methods if any. the default value factory destructor will call 
     * the destructor on the vf's impl object.
     */  
    impl = new_my_valufactory_impl();
    g_my_vf->impl = (objmask*) impl;

    g_my_vf->vtab->add_type(g_my_vf, impl->mt_add);
    g_my_vf->vtab->add_type(g_my_vf, impl->mt_add_result);

    return g_my_vf;
}


/* - - - - - - - - - - - - - - - - - - - - -
 * unit tests
 * - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * test_setup()
 */
void test_setup(void)
{
    setup_this_test();

    do
    {   

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_tcpds_construction()
 * test tcp delivery service constructor and destructor
 */
void test_tcpds_construction(void)
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

        tcpds->destroy(tcpds);    

        url->destroy(url);
        impl_factory->destroy(impl_factory);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   /* start fresh for next test */   
}



/**
 * test_transport_construction()
 * test delivery service constructor and destructor
 */
void test_transport_construction(void)
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

        ds->destroy(ds); /* destroys transport implementation via interface */
        impl_factory->destroy(impl_factory);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   /* start fresh for next test */   
}


/**
 * main   
 */
int _tmain(int argc, _TCHAR* argv[])
{
    CU_pSuite ps = NULL;
    g_is_automated_test = argc > 1 && 0 != wcscmp(argv[1], L"-a");
    if (CUE_SUCCESS != CU_initialize_registry()) return 0;
    CU_set_output_filename("../test_transport");
    ps = CU_add_suite("transport test suite", init_suite, clean_suite);
    etch_watch_id = 0; 
        
    CU_add_test(ps, "test test setup and teardown", test_setup);
    CU_add_test(ps, "test tcp delivery service constructor", test_tcpds_construction);
    CU_add_test(ps, "test transport constructor", test_transport_construction);
       
    if (g_is_automated_test)    
        CU_automated_run_tests();    
    else
    {   CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    }

    if (!g_is_automated_test) { printf("any key ..."); while(!_getch()); wprintf(L"\n"); }     
    CU_cleanup_registry();
    return CU_get_error();
}