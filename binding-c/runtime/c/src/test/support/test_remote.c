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

#include "apr_time.h" /* some apr must be included first */
#include "etch_svcobj_masks.h"
#include "etch_transport.h"  
#include "etchthread.h"
#include "etch_stub.h"

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etch_remote.h"
#include "etch_defvalufact.h"
#include "etch_plainmailbox.h"
#include "etch_plainmboxmgr.h"
#include "etchmap.h"
#include "etchlog.h"
#include "etch_global.h"
#include "etchlog.h"

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

#define THISTEST_WHO_VALUE 0x5151

#define TEST_MSGID  0x1001
#define TEST_QDELAYMS 1000
#define TEST_MBOX_LIFETIME_UNTIL_CLOSE 0
#define TEST_LIFETIME_ONE_SECOND 1000
#define TEST_MAXMSGS_ONE 1

unsigned short CLASSID_MY_VF;
unsigned short CLASSID_MY_VF_VTAB;
unsigned short CLASSID_MY_VF_IMPL;
unsigned short CLASSID_MY_REMOTEIMPL;
unsigned short CLASSID_MY_IMPLBASE;
unsigned short CLASSID_MY_REMOTEBASE;
unsigned short CLASSID_MY_RESULT;
 
typedef enum whats
{ TRANSPORT_MESSAGE = 1, TRANSPORT_QUERY, TRANSPORT_CONTROL, TRANSPORT_NOTIFY,
  BEGIN_CALL, END_CALL
} whats;

etch_resources*         g_my_resources;
default_value_factory*  g_my_vf;
etch_plainmailbox*      g_my_mbox; 
i_mailbox*              g_my_ibox; 
etch_plainmailboxmgr*   g_my_mboxmgr;           
i_delivery_service*     g_my_ds;
etch_type*              g_mt_foo;
etch_type*              g_mt_bar;

etch_who*               gds_who;
int64                   gds_messageid;
int                     gds_what;
int                     gds_rtypeid;
int                     gds_eventval;
int                     gds_queryval;
int                     gds_controlval;
int                     gds_valueval;

i_delivery_service* new_my_delivery_service();
int my_pmboxmgr_session_notify (etch_plainmailboxmgr*, etch_event*);
default_value_factory* new_fake_valuefactory();
char* LOGSRC = "TEST";


/**
 * new_my_resources()
 * resources map constructor
 */
etch_resources* new_my_resources(void* valuefactory)
{   
    etch_resources* resx = get_etch_transport_resources(NULL);
    etch_resources_add(resx, ETCH_RESXKEY_MSGIZER_VALUFACT, valuefactory); 
    return resx;
}


/**
 * setup_this_test()
 * set up an individual unit test
 */
int setup_this_test()
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
void teardown_this_test()
{ 
    if (g_my_mboxmgr)
        if (g_my_mbox)
            g_my_mbox->destroy(g_my_mbox);

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

    if (g_my_ds) 
        g_my_ds->destroy(g_my_ds);

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

int my_pmboxmgr_session_notify (etch_plainmailboxmgr* mgr, etch_event* evt)
{
    switch(evt->value)
    {   case ETCHEVT_SESSION_DOWN: pmboxmgr_unregister_all(mgr);
    }
    ETCHOBJ_DESTROY(evt);
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
        /* no custom types to destroy - they are global for this test */
    }

    return destroy_objectex((objmask*) impl);
}


/**
 * new_my_valufactory_impl()
 * constructor for our value factory's instance data
 */
my_valufactory_impl* new_my_valufactory_impl()
{
    unsigned short class_id = CLASSID_MY_VF_IMPL? CLASSID_MY_VF_IMPL: 
        (CLASSID_MY_VF_IMPL = get_dynamic_classid());

    my_valufactory_impl* impl = (my_valufactory_impl*) new_object
        (sizeof(my_valufactory_impl), ETCHTYPEB_VALUEFACTIMP, class_id);

    impl->destroy = destroy_my_valufactory_impl;

    return impl;
}


/**
 * new_fake_valuefactory()
 */
default_value_factory* new_fake_valuefactory()
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
    inheritlist = get_vtab_inheritance_list((objmask*)g_my_vf, 2, 1, classid_vf_vtab);
    inheritlist[1].obj_type = ETCHTYPEB_VALUEFACTORY;  
    inheritlist[1].class_id = CLASSID_VALUEFACTORY;  /* parent class */
    g_my_vf->class_id = classid_vf;  /* our class */

    /* instantiate the custom vf's instance data and assign it to the vf. 
     * the impl comprises all data specific to the inheriting class, including 
     * data and methods if any. the default value factory destructor will call 
     * the destructor on the vf's impl object.
     */  
    impl = new_my_valufactory_impl();
    g_my_vf->impl = (objmask*) impl;
    g_my_vf->vtab->add_type(g_my_vf, g_mt_foo);
    g_my_vf->vtab->add_type(g_my_vf, g_mt_bar);

    /* set msgid validator so we can set and retrieve message IDs */
    etchtype_put_validator(g_mt_foo, builtins._mf__message_id, (objmask*) etchvtor_int64_get(0));
    etchtype_put_validator(g_mt_bar, builtins._mf__message_id, (objmask*) etchvtor_int64_get(0));

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
int myds_begincall (i_delivery_service* thisx, etch_message* msg, void** out)
{
    etch_int64* msgid = NULL;        
    assert(out);
    *out = g_my_ibox;
    gds_what = BEGIN_CALL;
    msgid = message_get_id(msg);  
    gds_messageid = msgid? msgid->value: 0;
    ETCHOBJ_DESTROY(msg);
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
int myds_endcall (i_delivery_service* thisx, i_mailbox* mbox, etch_type* rtype, void** out)
{
    etch_int32* resultobj = new_int32(CLASSID_MY_RESULT);
    resultobj->class_id = CLASSID_MY_RESULT;
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
int myds_session_message (etch_tcp_delivery_service* thisx, etch_who* whofrom, etch_message* msg)
{
    ETCHOBJ_DESTROY(msg);
    return -1;
}


/**
 * myds_session_control()
 * override for i_delivery_service.ism.session_control().  
 * @param control event, caller relinquishes.
 * @param value control value, caller relinquishes.
 */
int myds_session_control (etch_tcp_delivery_service* thisx, etch_event* control, objmask* value)
{
    ETCHOBJ_DESTROY(control);
    ETCHOBJ_DESTROY(value);
    return -1;
}


/**
 * myds_session_notify()
 * override for i_delivery_service.ism.session_notify().
 * @param evt event, caller relinquishes.
 */
int myds_session_notify (etch_tcp_delivery_service* thisx, etch_event* evt)
{
    ETCHOBJ_DESTROY(evt);
    return -1;
}


/**
 * myds_session_query()
 * override for i_delivery_service.ism.session_query().
 * @param query, caller relinquishes.
 */
objmask* myds_session_query (etch_tcp_delivery_service* thisx, objmask* query) 
{
    ETCHOBJ_DESTROY(query);
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
int myds_transport_message (etch_tcp_delivery_service* thisx, etch_who* whoto, etch_message* msg)
{  
    etch_int64* msgid = NULL;        
    gds_what = TRANSPORT_MESSAGE;
    gds_who  = whoto;    
    msgid = message_get_id(msg);  
    gds_messageid = msgid? msgid->value: 0;
    ETCHOBJ_DESTROY(msg);
    return 0;
}


/**
 * myds_transport_control()
 * override for i_delivery_service.itm.transport_control()
 * @param control caller relinquishes.
 * @param value caller relinquishes.
 */
int myds_transport_control (etch_tcp_delivery_service* thisx, etch_event* control, etch_int32* value)
{
    assert(control);
    gds_what = TRANSPORT_CONTROL;
    gds_controlval = control->value;   
    gds_valueval   = value? value->value: -1;
    ETCHOBJ_DESTROY(control);
    ETCHOBJ_DESTROY(value);                                                        
    return 0;
}


/**
 * myds_transport_notify()
 * override for i_delivery_service.itm.transport_notify()
 * i_transportmessage::transport_notify override.
 * @param evt, caller relinquishes.
 */
int myds_transport_notify (etch_tcp_delivery_service* thisx, etch_event* evt)
{
    assert(evt);
    gds_what = TRANSPORT_NOTIFY;
    gds_eventval = evt->value;
    evt->destroy(evt);
    return 0;
}


/**
 * myds_transport_query()
 * override for i_delivery_service.itm.transport_query()
 * i_transportmessage::transport_query override.
 * @param query, caller relinquishes.
 */
objmask* myds_transport_query (etch_tcp_delivery_service* thisx, etch_query* query) 
{
    etch_int32* resultobj = new_int32(CLASSID_MY_RESULT);
    resultobj->class_id = CLASSID_MY_RESULT;
    gds_what = TRANSPORT_QUERY;
    gds_queryval = query->value;
    query->destroy(query);
    return (objmask*) resultobj;
}

/**
 * myds_get_session()
 * override for i_delivery_service.itm.get_session()
 * i_transportmessage::get_session override.
 */
i_sessionmessage* myds_get_session (etch_tcp_delivery_service* thisx) 
{
    assert(FALSE);
    return NULL;
}


/**
 * myds_set_session()
 * override for i_delivery_service.itm.set_session()
 * i_transportmessage::set_session override.
 */
void myds_set_session (etch_tcp_delivery_service* thisx, i_sessionmessage* newsession) 
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
i_delivery_service* new_my_delivery_service()
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
int destroy_my_remoteobj(my_remoteobj* thisx)
{
    ETCHOBJ_DESTROY(thisx->xxxx_either_base);
    ETCHOBJ_DESTROY(thisx->remote_base);
    return destroy_objectex((objmask*) thisx);
}

/**
 * destroy_my_clientorserver_impl()
 * destructor for our remoteobj.xxxx_either_base
 */
int destroy_my_clientorserver_impl(i_xxxx_either* thisx)
{
    etch_free(thisx->iobjsession);
    return destroy_objectex((objmask*) thisx);
}


/**
 * new_my_clientorserver_impl()
 * instatiate my_remoteobj.xxxx_either_base
 */
i_xxxx_either* new_my_clientorserver_impl(my_remoteobj* thisx)
{
    i_xxxx_either* either_base = (i_xxxx_either*) new_object (sizeof(i_xxxx_either), 
        ETCHTYPEB_EXESERVERBASE, get_dynamic_classid_unique(&CLASSID_MY_IMPLBASE));

    either_base->destroy = destroy_my_clientorserver_impl;

    either_base->thisx = (objmask*) thisx;
    
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
xxxx_remote* new_my_remote_base (void* thisx, 
    i_delivery_service* ids, etch_value_factory* vf, objmask* ixxxx)
{
    xxxx_remote* remote = new_etch_remote_base (thisx, ETCH_DEFSIZE, 
        get_dynamic_classid_unique(&CLASSID_MY_REMOTEBASE), ids, vf, ixxxx);
 
    return remote;
}


/**
 * new_my_remote_client_or_server()
 * instatiate and return an implementing object for the remote base.
 */
my_remoteobj* new_my_remote_client_or_server (void* thisx, i_delivery_service* ids, etch_value_factory* vf)
{
    /* for these tests it doesn't matter whether we use obj_type of client or server */
    my_remoteobj* remoteobj = (my_remoteobj*) new_object (sizeof(my_remoteobj), 
        ETCHTYPEB_REMOTESERVER, get_dynamic_classid_unique(&CLASSID_MY_REMOTEIMPL)); 

    remoteobj->destroy = destroy_my_remoteobj;
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
 * test_new_message()
 */
void test_new_message(void)
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

        newmsg->destroy(newmsg);
        myremote->destroy(myremote);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   /* start fresh for next test */   
}


/**
 * test_send()
 */
void test_send(void)
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
            ETCHOBJ_DESTROY(newmsg);

        myremote->destroy(myremote);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   /* start fresh for next test */   
}


/**
 * test_begincall()
 */
void test_begincall(void)
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
            ETCHOBJ_DESTROY(newmsg);

        myremote->destroy(myremote);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   /* start fresh for next test */   
}


/**
 * test_endcall()
 */
void test_endcall(void)
{
    setup_this_test();

    do 
    {   int result = 0, resultval = 0;
        objmask* resultobj = NULL;

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
        CU_ASSERT_EQUAL(resultobj->class_id, CLASSID_MY_RESULT); 
        CU_ASSERT_EQUAL(g_mt_bar->id, gds_rtypeid); 
        
        ETCHOBJ_DESTROY(resultobj);

        myremote->destroy(myremote);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   /* start fresh for next test */   
}


/**
 * test_transport_query()
 */
void test_transport_query(void)
{
    setup_this_test();

    do 
    {   int result = 0, resultval = 0, THISQUERYVAL = 76543;
        objmask* resultobj = NULL;

        my_remoteobj* myremote = new_my_remote_client_or_server (NULL, 
            g_my_ds, (etch_value_factory*) g_my_vf); 
        etch_remote*  remote = myremote->remote_base;

        /* this call goes through delivery service.transport_query,
         * which we overrode to myds_transport_query() above.
         */
        resultobj = remote->transport_query (remote, new_etch_query(0, THISQUERYVAL));

        CU_ASSERT_PTR_NOT_NULL_FATAL(resultobj); 
        CU_ASSERT_EQUAL(resultobj->class_id, CLASSID_MY_RESULT); 
        CU_ASSERT_EQUAL(TRANSPORT_QUERY, gds_what);

        ETCHOBJ_DESTROY(resultobj);
        myremote->destroy(myremote);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   /* start fresh for next test */   
}


/**
 * test_transport_control()
 */
void test_transport_control(void)
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

        myremote->destroy(myremote);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   /* start fresh for next test */   
}


/**
 * test_transport_notify()
 */
void test_transport_notify(void)
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

        myremote->destroy(myremote);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   /* start fresh for next test */   
}


/**
 * test_start_waitup()
 */
void test_start_waitup(void)
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

        myremote->destroy(myremote);

    } while(0);
    
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   /* start fresh for next test */   
}


/**
 * test_stop_waitdown()
 */
void test_stop_waitdown(void)
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

        myremote->destroy(myremote);

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
    CU_set_output_filename("../test_remote");
    ps = CU_add_suite("remote base test suite", init_suite, clean_suite);
    etch_watch_id = 0; 
        
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