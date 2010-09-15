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
 * test_remote.c
 * test remote, delivery service, etc.
 */

#include "etch_runtime.h"
#include "etch_svcobj_masks.h"
#include "etch_transport.h"  
#include "etch_thread.h"
#include "etch_stub.h"
#include "etch_remote.h"
#include "etch_default_value_factory.h"
#include "etch_plain_mailbox.h"
#include "etch_plain_mailbox_manager.h"
#include "etch_objecttypes.h"
#include "etch_general.h"
#include "etch_map.h"
#include "etch_log.h"

#include <stdio.h>
#include "CUnit.h"

#ifdef _WIN32
#pragma message ( "this testsuite is not active" )
#else
#warning "this testsuite is not active"
#endif

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

#if 0
// extern types
extern apr_pool_t* g_etch_main_pool;

/* - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * unit test support
 * - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

#define THISTEST_WHO_VALUE 0x5151

#define TEST_MSGID  0x1001
#define TEST_QDELAYMS 1000
#define TEST_MBOX_LIFETIME_UNTIL_CLOSE 0
#define TEST_LIFETIME_ONE_SECOND 1000
#define TEST_MAXMSGS_ONE 1

static unsigned short CLASSID_MY_VF;
static unsigned short CLASSID_MY_VF_VTAB;
static unsigned short CLASSID_MY_VF_IMPL;
static unsigned short CLASSID_MY_REMOTEIMPL;
static unsigned short CLASSID_MY_IMPLBASE;
static unsigned short CLASSID_MY_REMOTEBASE;
static unsigned short CLASSID_MY_RESULT;
 
typedef enum whats
{ TRANSPORT_MESSAGE = 1, TRANSPORT_QUERY, TRANSPORT_CONTROL, TRANSPORT_NOTIFY,
  BEGIN_CALL, END_CALL
} whats;

static etch_resources*         g_my_resources;
static default_value_factory*  g_my_vf;
static etch_plainmailbox*      g_my_mbox; 
static i_mailbox*              g_my_ibox; 
static etch_plainmailboxmgr*   g_my_mboxmgr;           
static i_delivery_service*     g_my_ds;
static etch_type*              g_mt_foo;
static etch_type*              g_mt_bar;

static etch_who*               gds_who;
static int64                   gds_messageid;
static int                     gds_what;
static int                     gds_rtypeid;
static int                     gds_eventval;
static int                     gds_queryval;
static int                     gds_controlval;
static int                     gds_valueval;

static i_delivery_service* new_my_delivery_service();
static int my_pmboxmgr_session_notify (etch_plainmailboxmgr*, etch_event*);
static default_value_factory* new_fake_valuefactory();
static char* LOGSRC = "TEST";


/**
 * new_my_resources()
 * resources map constructor
 */
static etch_resources* new_my_resources(void* valuefactory)
{   
    etch_resources* resx = get_etch_transport_resources(NULL);
    etch_resources_add(resx, ETCH_RESXKEY_MSGIZER_VALUFACT, valuefactory); 
    return resx;
}


/**
 * setup_this_test()
 * set up an individual unit test
 */
static int setup_this_test()
{
    CLASSID_MY_VF      = get_dynamic_classid();
    CLASSID_MY_VF_VTAB = get_dynamic_classid();
    CLASSID_MY_VF_IMPL = get_dynamic_classid();
    #if(IS_DEBUG_CONSOLE)
    printf("\n");
    #endif

    g_mt_foo = new_static_type(L"foo");
    g_mt_bar = new_static_type(L"bar");

    g_my_vf = new_fake_valuefactory();  
    set_etchobj_static_all(g_my_vf);   /* so resources map will not destroy */ 

    /* get resources map populated with transport resources such as thread pools */
    g_my_resources = new_my_resources(g_my_vf);

    g_my_ds = new_my_delivery_service ();

    /* the ids itm object belongs to the ds transport which is the mailbox manager */
    g_my_mboxmgr = g_my_ds->transport->thisx;
    CU_ASSERT_FATAL(is_etch_mailboxmgr(g_my_mboxmgr));

    /* register a new mailbox with the manager */
    g_my_mbox = new_mailbox (g_my_mboxmgr->imanager, TEST_MSGID, 
       TEST_QDELAYMS, TEST_MBOX_LIFETIME_UNTIL_CLOSE, TEST_MAXMSGS_ONE);

    CU_ASSERT_PTR_NOT_NULL_FATAL(g_my_mbox);
    g_my_ibox = g_my_mbox->imailbox;
    g_my_mboxmgr->session_notify = my_pmboxmgr_session_notify;

    return 0;
}


/**
 * teardown_this_test()
 * tear down an individual unit test
 */
static void teardown_this_test()
{ 
    if (g_my_mboxmgr)
        if (g_my_mbox)
            etch_object_destroy(g_my_mbox);

    if (g_my_resources)  
    {   /* we did a set_etchobj_static_all() on the g_my_vf value factory  
         * and as a result the map will not destroy it. if we had not done
         * so, the vf would have been destroyed with the resources map. */
        etch_object_destroy(g_my_resources);   
    }

    if (g_my_vf) 
    {   /* we clear the set_etchobj_static_all() on the g_my_vf value factory  
         * and as a result we can then destroy it */
        clear_etchobj_static_all(g_my_vf);
        etch_object_destroy(g_my_vf);
    }

    etch_object_destroy(g_my_ds);

    destroy_static_type(g_mt_foo);
    destroy_static_type(g_mt_bar);

    gds_what  = 0;
    gds_who   = NULL;
    g_my_vf   = NULL;
    g_my_ds   = NULL;
    g_my_mbox = NULL;
    g_my_ibox = NULL;
    g_my_mboxmgr = NULL;  

    g_mt_foo = g_mt_bar = NULL;
    gds_eventval= gds_queryval = gds_controlval = gds_valueval = 0;
    g_my_resources = NULL;
    gds_messageid = 0;  
    gds_rtypeid = 0;

    etchvf_free_builtins(); 
}

/* - - - - - - - - - -
 * mailbox manager
 * - - - - - - - - - - 
 */

static int my_pmboxmgr_session_notify (etch_plainmailboxmgr* mgr, etch_event* evt)
{
    switch(evt->value)
    {   case ETCHEVT_SESSION_DOWN: pmboxmgr_unregister_all(mgr);
    }
    //    ETCHOBJ_DESTROY();
    etch_object_destroy(evt);
    evt = NULL;

    return 0;
}


/* - - - - - - - - - -
 * remote object
 * - - - - - - - - - - 
 */

/**
 * my_remoteobj
 * xxxx_remote_either
 * this represents for this test what would be a service remote_server or remote_client in practice.
 */
typedef struct my_remoteobj
{
    etch_object object;

    i_xxxx_either* xxxx_either_base;  /* owned server or client */
    xxxx_remote*   remote_base;       /* owned */
    void*          either_factory;    /* owned */
    default_value_factory*  vf;       /* owned by base */

    /* note that the transport interface is accessed via the remote_base */ 

} my_remoteobj;



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

} my_valufactory_impl;


/**
 * destroy_my_valufactory_impl()
 * destructor for inheriting value factory instance data
 */
static int destroy_my_valufactory_impl(my_valufactory_impl* impl)
{
    if (NULL == impl) return -1;

    if (!is_etchobj_static_content(impl))
    {
        /* no custom types to destroy - they are global for this test */
    }

    return destroy_objectex((etch_object*) impl);
}


/**
 * new_my_valufactory_impl()
 * constructor for our value factory's instance data
 */
static my_valufactory_impl* new_my_valufactory_impl()
{
    unsigned short class_id = CLASSID_MY_VF_IMPL? CLASSID_MY_VF_IMPL: 
        (CLASSID_MY_VF_IMPL = get_dynamic_classid());

    my_valufactory_impl* impl = (my_valufactory_impl*) new_object
        (sizeof(my_valufactory_impl), ETCHTYPEB_VALUEFACTIMP, class_id);

    ((etch_object*)impl)->destroy = destroy_my_valufactory_impl;

    return impl;
}


/**
 * new_fake_valuefactory()
 */
static default_value_factory* new_fake_valuefactory()
{
    my_valufactory_impl* impl = NULL;
    etchparentinfo* inheritlist = NULL;
    const unsigned short classid_vf = get_dynamic_classid_unique(&CLASSID_MY_VF);
    const unsigned short classid_vf_vtab = get_dynamic_classid_unique(&CLASSID_MY_VF_VTAB);

    g_my_vf = new_default_value_factory(NULL, NULL);

   /* ensure parent type keys exist in the (one-based) inheritance list. 
    * parent class of our custom vf is default_value_factory.
    * inheritance list is used by validators and object assignment logic.
    */
    inheritlist = get_vtab_inheritance_list((etch_object*)g_my_vf, 2, 1, classid_vf_vtab);
    inheritlist[1].obj_type = ETCHTYPEB_VALUEFACTORY;  
    inheritlist[1].class_id = CLASSID_VALUEFACTORY;  /* parent class */
    ((etch_object*)g_my_vf)->class_id = classid_vf;  /* our class */

    /* instantiate the custom vf's instance data and assign it to the vf. 
     * the impl comprises all data specific to the inheriting class, including 
     * data and methods if any. the default value factory destructor will call 
     * the destructor on the vf's impl object.
     */  
    impl = new_my_valufactory_impl();
    g_my_vf->impl = (etch_object*) impl;
    ((struct i_value_factory*)((etch_object*)g_my_vf)->vtab)->add_type(g_my_vf, g_mt_foo);
    ((struct i_value_factory*)((etch_object*)g_my_vf)->vtab)->add_type(g_my_vf, g_mt_bar);

    /* set msgid validator so we can set and retrieve message IDs */
    etchtype_put_validator(g_mt_foo, builtins._mf__message_id, (etch_object*) etchvtor_int64_get(0));
    etchtype_put_validator(g_mt_bar, builtins._mf__message_id, (etch_object*) etchvtor_int64_get(0));

    return g_my_vf;
}


/* - - - - - - - - - -
 * delivery service
 * - - - - - - - - - - 
 */

/**
 * myds_begincall()
 * override for i_delivery_service.begin_call().
 * typedef int (*etch_delivsvc_begincall)(void* thisx, etch_message*, void** out);  
 * @param msg caller relinquishes on success, retains on failure
 * @param out mailbox interface returned on success
 * @return 0 success, or -1 failure. new mailbox return in out parameter.
 */
static int myds_begincall (i_delivery_service* thisx, etch_message* msg, void** out)
{
    etch_int64* msgid = NULL;        
    assert(out);
    *out = g_my_ibox;
    gds_what = BEGIN_CALL;
    msgid = message_get_id(msg);  
    gds_messageid = msgid? msgid->value: 0;
    etch_object_destroy(msg);
    msg = NULL;

    return 0;
}


/**
 * myds_endcall()
 * override for i_delivery_service.end_call().
 * typedef int (*etch_delvisvc_endcall)(void* thisx, i_mailbox*, etch_type*, void** out); 
 * message response received. close mailbox and return response.
 * @param mbox the current mailbox (interface), caller retains.
 * @param response_type type of the response message, caller retains.
 * @param out pointer to caller's location to receive the message response object.
 * @return 0 success, -1 failure. response object returned via out parameter.
 * @remarks assumed that the reply message and its wrapper are destroyed with the mailbox.
 */
static int myds_endcall (i_delivery_service* thisx, i_mailbox* mbox, etch_type* rtype, void** out)
{
    etch_int32* resultobj = new_int32(CLASSID_MY_RESULT);
    ((etch_object*)resultobj)->class_id = CLASSID_MY_RESULT;
    gds_what = END_CALL;
    gds_rtypeid = rtype->id;
    assert(out);
    *out = resultobj; 
    return 0;
}


/* - - - - - - - - - - - - - - - - -  
 * delivery service i_sessionmessage  
 * - - - - - - - - - - - - - - - - -  
 */

/* this is the i_delivery_service implementation of i_sessionmessage,
 * distinct from the transport.session's implementation of i_sessionmessage
 * which is implemented externally and set via set_session().
 */

/**
 * myds_session_message()
 * override for i_delivery_service.ism.session_message().
 * @param whofrom caller retains, can be null.
 * @param msg caller relinquishes
 * @return 0 (message handled), or -1 (error, closed, or timeout)  
 */
static int myds_session_message (etch_tcp_delivery_service* thisx, etch_who* whofrom, etch_message* msg)
{
  etch_object_destroy(msg);
  msg = NULL;

    return -1;
}


/**
 * myds_session_control()
 * override for i_delivery_service.ism.session_control().  
 * @param control event, caller relinquishes.
 * @param value control value, caller relinquishes.
 */
static int myds_session_control (etch_tcp_delivery_service* thisx, etch_event* control, etch_object* value)
{
    etch_object_destroy(control);
    control = NULL;

    etch_object_destroy(value);
    value = NULL;

    return -1;
}


/**
 * myds_session_notify()
 * override for i_delivery_service.ism.session_notify().
 * @param evt event, caller relinquishes.
 */
static int myds_session_notify (etch_tcp_delivery_service* thisx, etch_event* evt)
{
    etch_object_destroy(evt);
    evt = NULL;

    return -1;
}


/**
 * myds_session_query()
 * override for i_delivery_service.ism.session_query().
 * @param query, caller relinquishes.
 */
static etch_object* myds_session_query (void* data, etch_query* query) 
{
    etch_tcp_delivery_service* thisx = (etch_tcp_delivery_service*)data;

    etch_object_destroy(query);
    query = NULL;
    return NULL;
}


/* - - - - - - - - - - - - - - - - - -   
 * delivery service i_transportmessage  
 * - - - - - - - - - - - - - - - - - -   
 */

/**
 * myds_transport_message()
 * override for i_delivery_service.itm.transport_message().
 * @param whoto recipient - caller retains 
 * @param message caller relinquishes on success, retains on failure.  
 * @return 0 success, -1 error.
 */
static int myds_transport_message (etch_tcp_delivery_service* thisx, etch_who* whoto, etch_message* msg)
{  
    etch_int64* msgid = NULL;        
    gds_what = TRANSPORT_MESSAGE;
    gds_who  = whoto;    
    msgid = message_get_id(msg);  
    gds_messageid = msgid? msgid->value: 0;
    etch_object_destroy(msg);
    msg = NULL;

    return 0;
}


/**
 * myds_transport_control()
 * override for i_delivery_service.itm.transport_control()
 * @param control caller relinquishes.
 * @param value caller relinquishes.
 */
static int myds_transport_control (void* data, etch_event* control, etch_object* value)
{
    etch_tcp_delivery_service* thisx = (etch_tcp_delivery_service*)data;
    etch_int32* i = NULL;
    assert(control);
    gds_what = TRANSPORT_CONTROL;
    gds_controlval = control->value;
    i = (etch_int32*)value;
    gds_valueval   = i? i->value: -1;
    etch_object_destroy(control);
    control = NULL;

    etch_object_destroy(value);
    value = NULL;

    return 0;
}


/**
 * myds_transport_notify()
 * override for i_delivery_service.itm.transport_notify()
 * i_transportmessage::transport_notify override.
 * @param evt, caller relinquishes.
 */
static int myds_transport_notify (etch_tcp_delivery_service* thisx, etch_event* evt)
{
    assert(evt);
    gds_what = TRANSPORT_NOTIFY;
    gds_eventval = evt->value;
    etch_object_destroy(evt);
    return 0;
}


/**
 * myds_transport_query()
 * override for i_delivery_service.itm.transport_query()
 * i_transportmessage::transport_query override.
 * @param query, caller relinquishes.
 */
static etch_object* myds_transport_query (etch_tcp_delivery_service* thisx, etch_query* query) 
{
    etch_int32* resultobj = new_int32(CLASSID_MY_RESULT);
    ((etch_object*)resultobj)->class_id = CLASSID_MY_RESULT;
    gds_what = TRANSPORT_QUERY;
    gds_queryval = query->value;
    etch_object_destroy(query);
    return (etch_object*) resultobj;
}

/**
 * myds_get_session()
 * override for i_delivery_service.itm.get_session()
 * i_transportmessage::get_session override.
 */
static struct i_sessionmessage* myds_get_session (void* data) 
{
    etch_tcp_delivery_service* thisx = (etch_tcp_delivery_service*)data;
    assert(FALSE);
    return NULL;
}


/**
 * myds_set_session()
 * override for i_delivery_service.itm.set_session()
 * i_transportmessage::set_session override.
 */
static void myds_set_session (etch_tcp_delivery_service* thisx, i_sessionmessage* newsession) 
{
    assert(FALSE);
}


/* - - - - - - - - - - - - - - -    
 * delivery service construction
 * - - - - - - - - - - - - - - -    
 */

/**
 * new_my_delivery_service()
 */
static i_delivery_service* new_my_delivery_service()
{
    etch_tcp_connection* nullconnection = NULL;
    etch_tcp_delivery_service* delsvc = NULL;
    i_sessionmessage*   ism = NULL;
    i_transportmessage* itm = NULL;
    etch_client_factory* impl_factory = new_client_factory (NULL, NULL, NULL);

    i_delivery_service* ids = new_etch_transport(L"http://www.cisco.com:9999/cuae", 
        (etch_factory_params*) impl_factory, nullconnection); 
    CU_ASSERT_PTR_NOT_NULL_FATAL(ids);

    delsvc = ids->thisx;
    CU_ASSERT_PTR_NOT_NULL_FATAL(delsvc);
    CU_ASSERT_EQUAL_FATAL(is_etch_deliverysvc(delsvc), TRUE);

    ism = ids->ism;
    itm = ids->itm; 
    CU_ASSERT_PTR_NOT_NULL_FATAL(ism);
    CU_ASSERT_EQUAL_FATAL(is_etch_sessionmsg(ism), TRUE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(itm);
    CU_ASSERT_EQUAL_FATAL(is_etch_transportmsg(itm), TRUE);

    /* override delivery service i_sessionmessage to implementations herein */
    ids->begin_call      = delsvc->begin_call      = myds_begincall;
    ids->end_call        = delsvc->end_call        = myds_endcall;
    ism->session_message = delsvc->session_message = myds_session_message;
    ism->session_control = delsvc->session_control = myds_session_control;
    ism->session_notify  = delsvc->session_notify  = myds_session_notify;
    ism->session_query   = delsvc->session_query   = myds_session_query;

    /* override delivery service i_transportmessage to implementations herein.
     * note that we swap out the virtuals, but the not the itm object, which
     * is the ds transport, which is the mailbox manager's itm. the mailbox
     * manager owns it and will destroy it when destroyed during destruction  
     * of the delivery service.
     */
    itm->transport_message = delsvc->transport_message = myds_transport_message;
    itm->transport_control = delsvc->transport_control = myds_transport_control;
    itm->transport_notify  = delsvc->transport_notify  = myds_transport_notify;
    itm->transport_query   = delsvc->transport_query   = myds_transport_query;
    itm->get_session       = delsvc->get_session       = myds_get_session;
    itm->set_session       = delsvc->set_session       = myds_set_session;

    return ids;
}


/* - - - - - - - - - - - - - - - - -  
 * remote androgynous object 
 * - - - - - - - - - - - - - - - - -  
 */

/**
 * destroy_my_remoteobj()
 * destructor for our remote object
 */
static int destroy_my_remoteobj(my_remoteobj* thisx)
{
  etch_object_destroy(thisx->xxxx_either_base);
  thisx->xxxx_either_base = NULL;

  etch_object_destroy(thisx->remote_base);
  thisx->remote_base = NULL;

    return destroy_objectex((etch_object*) thisx);
}

/**
 * destroy_my_clientorserver_impl()
 * destructor for our remoteobj.xxxx_either_base
 */
static int destroy_my_clientorserver_impl(i_xxxx_either* thisx)
{
    etch_free(thisx->iobjsession);
    return destroy_objectex((etch_object*) thisx);
}


/**
 * new_my_clientorserver_impl()
 * instatiate my_remoteobj.xxxx_either_base
 */
static i_xxxx_either* new_my_clientorserver_impl(my_remoteobj* thisx)
{
    i_xxxx_either* either_base = (i_xxxx_either*) new_object (sizeof(i_xxxx_either), 
        ETCHTYPEB_EXESERVERBASE, get_dynamic_classid_unique(&CLASSID_MY_IMPLBASE));

    ((etch_object*)either_base)->destroy = destroy_my_clientorserver_impl;

    either_base->thisx = (etch_object*) thisx;
    
    {   /* populate as much of xxxx_either_impl as we need */
        i_objsession* ios = new_default_objsession_interface (thisx);
        either_base->iobjsession = ios;
        either_base->_session_control = ios->_session_control; 
        either_base->_session_notify  = ios->_session_notify; 
        either_base->_session_query   = ios->_session_query; 
    }

    return either_base;
}


/**
 * new_my_remote_base
 * instantiates remote base.  
 * @param ids delivery service -- caller retains.
 * @param vf default value factory -- caller retains. 
 * @param ixxxx service interface -- caller retains.
 */
static xxxx_remote* new_my_remote_base (void* thisx, 
    i_delivery_service* ids, etch_value_factory* vf, etch_object* ixxxx)
{
    xxxx_remote* remote = new_etch_remote_base (thisx, ETCH_DEFSIZE, 
        get_dynamic_classid_unique(&CLASSID_MY_REMOTEBASE), ids, vf, ixxxx);
 
    return remote;
}


/**
 * new_my_remote_client_or_server()
 * instatiate and return an implementing object for the remote base.
 */
static my_remoteobj* new_my_remote_client_or_server (void* thisx, i_delivery_service* ids, etch_value_factory* vf)
{
    /* for these tests it doesn't matter whether we use obj_type of client or server */
    my_remoteobj* remoteobj = (my_remoteobj*) new_object (sizeof(my_remoteobj), 
        ETCHTYPEB_REMOTESERVER, get_dynamic_classid_unique(&CLASSID_MY_REMOTEIMPL)); 

    ((etch_object*)remoteobj)->destroy = destroy_my_remoteobj;
    remoteobj->remote_base = new_my_remote_base (remoteobj, ids, vf, NULL);
    remoteobj->xxxx_either_base = new_my_clientorserver_impl(remoteobj);

    return remoteobj;
}


/* - - - - - - - - - - - - - - - - - - - - -
 * unit tests
 * - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * test_setup()
 * test instantiation and teardown of test components such as delivery service,
 * value factory, etc.
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
 * test_new_message()
 */
static void test_new_message(void)
{
    setup_this_test();

    do 
    {   int result = 0;   

        my_remoteobj* myremote = new_my_remote_client_or_server (NULL, 
            g_my_ds, (etch_value_factory*) g_my_vf);  

        etch_remote*  remote = myremote->remote_base;

        /* this call goes through etchremote_new_message() */
        etch_message* newmsg = remote->new_message(remote, g_mt_foo);
        CU_ASSERT_PTR_NOT_NULL_FATAL(newmsg);

        result = is_equal_types(message_type(newmsg), g_mt_foo);
        CU_ASSERT_EQUAL(result, TRUE);  

        CU_ASSERT_PTR_EQUAL(newmsg->vf, g_my_vf);

        etch_object_destroy(newmsg);
        etch_object_destroy(myremote);

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
 * test_send()
 */
static void test_send(void)
{
    setup_this_test();

    do 
    {   int result = 0, MSGIDVAL = 54321; 

        my_remoteobj* myremote = new_my_remote_client_or_server (NULL, 
            g_my_ds, (etch_value_factory*) g_my_vf); 
 
        etch_remote*  remote = myremote->remote_base;
        etch_message* newmsg = remote->new_message (myremote->remote_base, g_mt_foo);
        message_set_id (newmsg, new_int64(MSGIDVAL));

        /* this call goes through delivery service.transport_message,
         * which we overrode to myds_transport_message() above.
         */
        result = remote->send (remote, newmsg);  /* send message */

        CU_ASSERT_EQUAL(result, 0);  
        CU_ASSERT_EQUAL(TRANSPORT_MESSAGE, gds_what);
        CU_ASSERT_EQUAL(gds_messageid, MSGIDVAL);

        if (0 != result) /* message was relinquished on send() success */
	    {
	        etch_object_destroy(newmsg);
	        newmsg = NULL;

	    }

        ((etch_object*)myremote)->destroy(myremote);

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
 * test_begincall()
 */
static void test_begincall(void)
{
    setup_this_test();

    do 
    {   int result = 0, MSGIDVAL = 65432; 
        i_mailbox* outmbox = NULL;

        my_remoteobj* myremote = new_my_remote_client_or_server (NULL, 
            g_my_ds, (etch_value_factory*) g_my_vf); 
  
        etch_remote*  remote = myremote->remote_base;
        etch_message* newmsg = remote->new_message (myremote->remote_base, g_mt_foo);
        message_set_id (newmsg, new_int64(MSGIDVAL));

        /* this call goes through delivery service.begin_call,
         * which we overrode to myds_begincall() above.
         */
        result = remote->begin_call (remote, newmsg, &outmbox);

        CU_ASSERT_EQUAL(result, 0);  
        CU_ASSERT_EQUAL(BEGIN_CALL, gds_what);
        CU_ASSERT_EQUAL(gds_messageid, MSGIDVAL);
        CU_ASSERT_PTR_EQUAL(g_my_ibox, outmbox);

        if (0 != result) /* message was relinquished on begin_call() success */
	    {
    	    etch_object_destroy(newmsg);
	        newmsg = NULL;
	    }

        etch_object_destroy(myremote);

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
 * test_endcall()
 */
static void test_endcall(void)
{
    setup_this_test();

    do 
    {   int result = 0, resultval = 0;
        etch_object* resultobj = NULL;

        my_remoteobj* myremote = new_my_remote_client_or_server (NULL, 
            g_my_ds, (etch_value_factory*) g_my_vf); 
  
        etch_remote*  remote = myremote->remote_base;
    
        /* this call goes through delivery service.end_call,
         * which we overrode to myds_endcall() above.
         */
        result = remote->end_call (remote, g_my_ibox, g_mt_bar, &resultobj);

        CU_ASSERT_EQUAL(result, 0);  
        CU_ASSERT_EQUAL(END_CALL, gds_what);
        CU_ASSERT_PTR_NOT_NULL_FATAL(resultobj);
        CU_ASSERT_EQUAL(((etch_object*)resultobj)->class_id, CLASSID_MY_RESULT); 
        CU_ASSERT_EQUAL(g_mt_bar->id, gds_rtypeid); 
        
	    etch_object_destroy(resultobj);
	    resultobj = NULL;


        etch_object_destroy(myremote);

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
 * test_transport_query()
 */
static void test_transport_query(void)
{
    setup_this_test();

    do 
    {   int result = 0, resultval = 0, THISQUERYVAL = 76543;
        etch_object* resultobj = NULL;

        my_remoteobj* myremote = new_my_remote_client_or_server (NULL, 
            g_my_ds, (etch_value_factory*) g_my_vf); 
        etch_remote*  remote = myremote->remote_base;

        /* this call goes through delivery service.transport_query,
         * which we overrode to myds_transport_query() above.
         */
        resultobj = remote->transport_query (remote, new_etch_query(0, THISQUERYVAL));

        CU_ASSERT_PTR_NOT_NULL_FATAL(resultobj); 
        CU_ASSERT_EQUAL(((etch_object*)resultobj)->class_id, CLASSID_MY_RESULT); 
        CU_ASSERT_EQUAL(TRANSPORT_QUERY, gds_what);

	    etch_object_destroy(resultobj);
	    resultobj = NULL;

        etch_object_destroy(myremote);

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
 * test_transport_control()
 */
static void test_transport_control(void)
{
    setup_this_test();

    do 
    {   int result = 0, resultval = 0, THISCTLVAL = 87654, THISVALVAL = 98765;

        my_remoteobj* myremote = new_my_remote_client_or_server (NULL, 
            g_my_ds, (etch_value_factory*) g_my_vf); 
        etch_remote*  remote = myremote->remote_base;

        /* this call goes through delivery service.transport_control,
         * which we overrode to myds_transport_control() above.
         */
        result = remote->transport_control (remote, new_etch_control(0, THISCTLVAL), new_int32(THISVALVAL));

        CU_ASSERT_EQUAL(result, 0); 
        CU_ASSERT_EQUAL(TRANSPORT_CONTROL, gds_what);
        CU_ASSERT_EQUAL(gds_controlval, THISCTLVAL);
        CU_ASSERT_EQUAL(gds_valueval, THISVALVAL);

        etch_object_destroy(myremote);

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
 * test_transport_notify()
 */
static void test_transport_notify(void)
{
    setup_this_test();

    do 
    {   int result = 0, resultval = 0, THISEVTVAL = 45678;

        my_remoteobj* myremote = new_my_remote_client_or_server (NULL, 
            g_my_ds, (etch_value_factory*) g_my_vf); 
        etch_remote*  remote = myremote->remote_base;

        /* this call goes through delivery service.transport_notify,
         * which we overrode to myds_transport_notify() above.
         */
        result = remote->transport_notify (remote, new_etch_event(0, THISEVTVAL));

        CU_ASSERT_EQUAL(result, 0); 
        CU_ASSERT_EQUAL(TRANSPORT_NOTIFY, gds_what);
        CU_ASSERT_EQUAL(gds_eventval, THISEVTVAL);

        etch_object_destroy(myremote);

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
 * test_start_waitup()
 */
static void test_start_waitup(void)
{
    setup_this_test();

    do 
    {   int result = 0, resultval = 0, THISWAITMS = 3000;

        my_remoteobj* myremote = new_my_remote_client_or_server (NULL, 
            g_my_ds, (etch_value_factory*) g_my_vf); 
        etch_remote*  remote = myremote->remote_base;

        /* this call goes through delivery service.transport_control,
         * which we overrode to myds_transport_control() in our ds constuctor 
         * above. so we're not testing an actual connection waitup, we're testing
         * that the call reaches delivery service.transport_control(), and that
         * we can override delivery service.transport_control() at will.
         */

        result = remote->start_waitup(remote, ETCH_NOWAIT);

        CU_ASSERT_EQUAL(result, 0); 
        CU_ASSERT_EQUAL(TRANSPORT_CONTROL, gds_what);
        CU_ASSERT_EQUAL(gds_controlval, ETCH_NOWAIT);  

        result = remote->start_waitup(remote, THISWAITMS);

        CU_ASSERT_EQUAL(result, 0); 
        CU_ASSERT_EQUAL(gds_controlval, THISWAITMS);       

        etch_object_destroy(myremote);

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
 * test_stop_waitdown()
 */
static void test_stop_waitdown(void)
{
    setup_this_test();

    do 
    {   int result = 0, resultval = 0, THISWAITMS = 3000;

        my_remoteobj* myremote = new_my_remote_client_or_server (NULL, 
            g_my_ds, (etch_value_factory*) g_my_vf); 
        etch_remote*  remote = myremote->remote_base;

        /* this call goes through delivery service.transport_control,
         * which we overrode to myds_transport_control() in our ds constuctor 
         * above. so we're not testing an actual connection waitdown, we're testing
         * that the call reaches delivery service.transport_control(), and that
         * we can override delivery service.transport_control() at will.
         */

        result = remote->stop_waitdown(remote, ETCH_NOWAIT);

        CU_ASSERT_EQUAL(result, 0); 
        CU_ASSERT_EQUAL(TRANSPORT_CONTROL, gds_what);
        CU_ASSERT_EQUAL(gds_controlval, ETCH_NOWAIT);  

        result = remote->stop_waitdown(remote, THISWAITMS);

        CU_ASSERT_EQUAL(result, 0); 
        CU_ASSERT_EQUAL(gds_controlval, THISWAITMS);       

        etch_object_destroy(myremote);

    } while(0);
    
    teardown_this_test();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}

#endif

CU_pSuite test_etch_remote_suite()
{
    CU_pSuite ps = CU_add_suite("remote base test suite", init_suite, clean_suite);
        
    // THESE TESTS ARE BROKEN
    // TODO first fix the stub tests per java tests, then redo these tests similarly.
    //CU_add_test(ps, "test test setup and teardown", test_setup);
    //CU_add_test(ps, "test remote.new_message", test_new_message);
    //CU_add_test(ps, "test remote.send", test_send);
    //CU_add_test(ps, "test remote.begin_call", test_begincall);
    //CU_add_test(ps, "test remote.end_call", test_endcall);
    //CU_add_test(ps, "test remote.transport_query", test_transport_query);
    //CU_add_test(ps, "test remote.transport_control", test_transport_control);
    //CU_add_test(ps, "test remote.transport_notify", test_transport_notify);
    //CU_add_test(ps, "test remote.start_waitup", test_start_waitup);
    //CU_add_test(ps, "test remote.stop_waitdown", test_stop_waitdown);

    return ps;
}
