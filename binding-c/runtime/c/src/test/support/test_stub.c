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
 * test_stub.c
 * test stub, delivery service, etc.
 */


#include "etch_runtime.h"
#include "etch_svcobj_masks.h"
#include "etch_transport.h"  
#include "etch_thread.h"
#include "etch_object.h"
#include "etch_stub.h"
#include "etch_default_value_factory.h"
#include "etch_plain_mailbox_manager.h"
#include "etch_transport.h"
#include "etch_objecttypes.h"
#include "etch_general.h"
#include "etch_map.h"
#include "etch_log.h"
#include "etch_mem.h"


#include <stdio.h>
#include "CUnit.h"

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

#ifdef _WIN32
#pragma message ( "this testsuite is not active" )
#else
#warning "this testsuite is not active"
#endif

#if 0


/* - - - - - - - - - - - - - - - - - - - - - - - - - - -
 * unit test support
 * - - - - - - - - - - - - - - - - - - - - - - - - - - -
 */

#define THISTEST_WHO_VALUE 0x5151

static unsigned short CLASSID_MY_VF;
static unsigned short CLASSID_MY_VF_VTAB;
static unsigned short CLASSID_MY_VF_IMPL;
static unsigned short CLASSID_MY_IMPLOBJ;
static unsigned short CLASSID_MY_IMPLBASE;

static default_value_factory* new_fake_valuefactory();
static etch_server_factory* new_my_stubparams(xxxx_either_impl*, etch_threadpool* qp, etch_threadpool* fp);

static i_delivery_service* new_my_delivery_service();
static etch_thread* thistest_proxy_threadpool_run (etch_threadpool*, etch_threadproc, void*);
static char* LOGSRC = "TEST";
 
typedef enum whats
{ TRANSPORT_MESSAGE = 1, TRANSPORT_QUERY, TRANSPORT_CONTROL, TRANSPORT_NOTIFY,
  SESSION_QUERY, SESSION_CONTROL, SESSION_NOTIFY, HOWDY,
} whats;

static etch_resources*         g_my_resources;
static default_value_factory*  g_my_vf;
static i_delivery_service*     g_my_ds;
static i_delivery_service*     g_my_transport; /* alias for g_my_ds */
static etch_tcp_delivery_service* g_my_dsimpl;
static i_xxxx_either*          g_my_iserver;
static etch_threadpool*        g_qpool;
static etch_threadpool*        g_fpool;
static etch_who*               g_who;
static etch_type*              g_mt_howdy;
static etch_type*              g_mt_nogood;
static etch_server_factory*    g_stubparams; 
static int                     g_is_run_on_queued_pool;
static int                     g_is_run_on_free_pool;
static int                     g_stub_errors;
static etch_run                g_real_fpool_run;  /* saved threadpool run procs */
static etch_run                g_real_qpool_run;

static etch_who*               gds_who;
static etch_message*           gds_message;   
static etch_int32*             gds_query_result;
static int                     gds_what;
static int                     gds_eventval;
static int                     gds_queryval;
static int                     gds_controlval;
static int                     gds_valueval;

static etch_who*               gsv_who;
static etch_message*           gsv_message;   
static etch_int32*             gsv_query_result;
static i_delivery_service*     gsv_ds;
static int                     gsv_what;
static int                     gsv_eventval;    
static int                     gsv_queryval;
static int                     gsv_controlval;
static int                     gsv_valueval;


/**
 * new_my_resources()
 * resources map constructor
 */
static etch_resources* new_my_resources(void* valuefactory)
{   
    etch_resources* resx = get_etch_transport_resources(NULL);
    etch_resources_add(resx, ETCH_RESXKEY_MSGIZER_VALUFACT, valuefactory); 

    g_qpool = (etch_threadpool*) etch_resources_get (resx, ETCH_RESXKEY_POOLTYPE_QUEUED); 
    g_real_qpool_run = g_qpool->run; /* save the real run procedure */
    g_qpool->run = thistest_proxy_threadpool_run; /* intercept the queued pool run */

    g_fpool = (etch_threadpool*) etch_resources_get (resx, ETCH_RESXKEY_POOLTYPE_FREE); 
    g_real_fpool_run = g_fpool->run; /* save the real run procedure */
    g_fpool->run = thistest_proxy_threadpool_run; /* intercept the free pool run */

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

    g_my_vf = new_fake_valuefactory();  
    set_etchobj_static_all(g_my_vf);   /* so resources map will not destroy */ 
 
    /* get resources map populated with transport resources such as thread pools */
    g_my_resources = new_my_resources(g_my_vf);

    //g_my_ds = new_my_delivery_service ();
    //g_my_transport = g_my_ds; /* alias */
   
    g_who = new_who(new_int32(THISTEST_WHO_VALUE));
    gds_query_result = new_int32(1);
    gsv_query_result = new_int32(2);
        
    //g_my_dsimpl = g_my_ds->thisx;
    //assert(is_etch_deliverysvc(g_my_dsimpl));
    //assert(is_etch_sessionmsg(g_my_dsimpl->sessionmsg));

    return 0;
}


/**
 * setup_this_stub()
 * setup for stub
 */
static int setup_this_stub(xxxx_either_impl* implobj, unsigned char stubtype, 
    etch_threadpool* qp, etch_threadpool* fp) 
{  
    new_my_stubparams(implobj, qp, fp);  
 
    g_my_ds = new_my_delivery_service ();
    g_my_transport = g_my_ds; /* alias */
   
    g_my_dsimpl = g_my_ds->thisx;
    assert(is_etch_deliverysvc(g_my_dsimpl));
    assert(is_etch_sessionmsg(g_my_dsimpl->sessionmsg));

    return 0;
}


/**
 * teardown_this_test()
 * tear down an individual unit test
 */
static void teardown_this_test()
{ 
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
    
    etch_free(g_stubparams);
    etch_free(g_my_iserver);

    etch_object_destroy(g_who);
    etch_object_destroy(gds_message);
    etch_object_destroy(gds_query_result);

    gds_what  = 0;
    gds_who   = NULL;
    g_my_vf   = NULL;
    g_my_ds   = NULL;
    g_who     = NULL;
    g_is_run_on_queued_pool = g_is_run_on_free_pool = g_stub_errors = 0;
    g_mt_howdy  = g_mt_nogood = NULL;
    gds_message = NULL;
    gds_eventval= gds_queryval = gds_controlval = gds_valueval = 0;
    gds_query_result = NULL;
    g_my_resources = NULL;

    etch_object_destroy(gsv_message);
    etch_object_destroy(gsv_query_result);

    gsv_what  = 0;
    gsv_who   = NULL;
    gsv_ds    = NULL;
    gsv_message = NULL;
    gsv_eventval= gsv_queryval = gsv_controlval = gsv_valueval = 0;
    g_my_transport = NULL;
    gsv_query_result = NULL;

    etchvf_free_builtins(); 
}


/* - - - - - - - - - -
 * stub implementor
 * - - - - - - - - - - 
 */

/**
 * my_implob
 * xxxx_either_impl with an extra method.
 * this represents for this test what would be an impl_server or impl_client in practice.
 */
typedef struct my_implobj
{
    etch_object object;

    i_xxxx_either*  either_base;  /* owned */
    etch_object*        ixxxx;        /* not owned */
    xxxx_remote_either* either;   /* not owned */

    int (*destroyex) (void*);  /* user memory destructor */

    /* - - - - - - - - - - - -
     * objsession
     * - - - - - - - - - - - -
     */
    i_objsession* iobjsession;  /* owned by base */
    /* fyi: iobjsession->thisx is my_implobj* */
    etch_session_control _session_control;
    etch_session_notify  _session_notify;
    etch_session_query   _session_query;

    /* - - - - - - - - - - - - - - - - -
     * custom instance data and methods  
     * - - - - - - - - - - - - - - - - -
     */

    int (*howdy) (struct my_implobj*, i_delivery_service*, etch_who*, etch_message*);

} my_implobj;



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

	etch_type*      mt_howdy;
    etch_type*      mt_nogood;

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
        destroy_static_type(impl->mt_howdy);
        destroy_static_type(impl->mt_nogood);
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

    impl->mt_howdy  = new_static_type(L"howdy");
    impl->mt_nogood = new_static_type(L"nogood");
    g_mt_howdy  = impl->mt_howdy;
    g_mt_nogood = impl->mt_nogood;

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
    ((struct i_value_factory*)((etch_object*)g_my_vf)->vtab)->add_type(g_my_vf, impl->mt_howdy);
    ((struct i_value_factory*)((etch_object*)g_my_vf)->vtab)->add_type(g_my_vf, impl->mt_nogood);

    return g_my_vf;
}


/* - - - - - - - - - -
 * delivery service
 * - - - - - - - - - - 
 */

/**
 * myds_begincall()
 * typedef int (*etch_delivsvc_begincall)(void* thisx, etch_message*, void** out);  
 * @param msg caller relinquishes on success, retains on failure
 * @param out mailbox interface returned on success
 * @return 0 success, or -1 failure. new mailbox return in out parameter.
 */
static int myds_begincall (void *data, etch_message* msg, void** out)
{
    assert(out);
    *out = NULL;
    return -1;
}


/**
 * myds_endcall()
 * typedef int (*etch_delvisvc_endcall)(void* thisx, i_mailbox*, etch_type*, void** out); 
 * message response received. close mailbox and return response.
 * @param mbox the current mailbox (interface), caller retains.
 * @param response_type type of the response message, caller retains.
 * @param out pointer to caller's location to receive the message response object.
 * @return 0 success, -1 failure. response object returned via out parameter.
 * @remarks assumed that the reply message and its wrapper are destroyed with the mailbox.
 */
static int myds_endcall (void* data, i_mailbox* mbox, etch_type* rtype, void** out)
{
    assert(out);
    *out = NULL;
    return -1;
}

/* - - - - - - - - - - - - - - - - -  
 * delivery service i_sessionmessage  
 * - - - - - - - - - - - - - - - - -  
 */

/* this is the delivery service interface implementation of i_sessionmessage,
 * distinct from the transport.session's implementation of i_sessionmessage
 * which is implemented externally and set via set_session().
 */

/**
 * myds_session_message()
 * @param whofrom caller retains, can be null.
 * @param msg caller relinquishes
 * @return 0 (message handled), or -1 (error, closed, or timeout)  
 */
static int myds_session_message (void* data, etch_who* whofrom, etch_message* msg)
{
    etch_object_destroy(msg);
    msg = NULL;

    return 0;
}


/**
 * myds_session_control()
 * delivery service interface implementation of i_session_message  
 * @param control event, caller relinquishes.
 * @param value control value, caller relinquishes.
 */
static int myds_session_control (void* data, etch_event* control, etch_object* value)
{
  etch_object_destroy(control);
  control = NULL;

  etch_object_destroy(value);
  value = NULL;

    return 0;
}


/**
 * myds_session_notify()
 * @param evt event, caller relinquishes.
 */
static int myds_session_notify (void* data, etch_event* evt)
{
  etch_object_destroy(evt);
  evt = NULL;

  return 0;
}


/**
 * myds_session_query()
 * @param query, caller relinquishes.
 */
static etch_object* myds_session_query(void* data, etch_query* query) 
{
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
 * @param whoto recipient - caller retains 
 * @param message caller relinquishes on success, retains on failure.  
 * @return 0 success, -1 error.
 */
static int myds_transport_message(void* data, void* whoData, void* messageData)
{  
    gds_what = TRANSPORT_MESSAGE;
    gds_who  = (etch_who*)whoData;
    etch_object_destroy((etch_message*)messageData);

    return 0;
}


/**
 * myds_transport_control()
 * @param control caller relinquishes.
 * @param value caller relinquishes.
 */
static int myds_transport_control (void* data, etch_event* control, etch_object* valueData)
{
  etch_int32* value = (etch_int32*)valueData;
    gds_what = TRANSPORT_CONTROL;
    gds_controlval = control->value;   
    gds_valueval   = value->value;                                                        
    return 0;
}


/**
 * myds_transport_notify()
 * i_transportmessage::transport_notify override.
 * @param evt, caller relinquishes.
 */
static int myds_transport_notify (void* data, etch_event* evt)
{
    gds_what  = TRANSPORT_NOTIFY;
    gds_eventval = evt->value;
    return 0;
}


/**
 * myds_transport_query()
 * i_transportmessage::transport_query override.
 * @param query, caller relinquishes.
 */
static etch_object* myds_transport_query (void* data, etch_query* query) 
{
    gds_what  = TRANSPORT_QUERY;
    gds_queryval = query->value;
    return 0;
}

/**
 * myds_get_session()
 * i_transportmessage::get_session override.
 */
static i_session* myds_get_session (void* data) 
{
    etch_tcp_delivery_service* thisx = (etch_tcp_delivery_service*)data;
    return (i_session*)thisx->session;
}


/**
 * myds_set_session()
 * i_transportmessage::set_session override.
 */
static void myds_set_session (void* data, void* param) 
{
    i_delivery_service* ids = (i_delivery_service*)data;
    i_sessionmessage* newsession = (i_sessionmessage*)param;
    /* we override methods in the ids itm. the itm object belongs to the ds transport,
     * which is the mailbox manager. we need to ensure in the real world that the ds
     * set_session() takes care of housekeeping similarly to this override.
     */
    etch_tcp_delivery_service* tcpds = get_etch_ds_impl(ids);
    assert(is_etch_sessionmsg(newsession));
    assert(tcpds->session == ids->session);
    if (tcpds->session) 
    {   assert(is_etch_sessionmsg(tcpds->session));
        etch_object_destroy(tcpds->session);
    }
    
    /* replace delivery service impl's sessionmsng with stub's sesssionmsg */
    tcpds->session = ids->session = newsession;     
    assert(is_etch_sessionmsg(tcpds->session));  

    /* we say that the ds owns the session, since even though the session belongs 
     * to the stub, the ds is expected to destroy the stub, and thus the session 
     * interface along with it. 
     */
    ids->is_session_owned = TRUE;
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
    i_sessionmessage*    ism = NULL;
    i_transportmessage*  itm = NULL;

    i_delivery_service* ids = new_etch_transport(L"http://www.cisco.com:9999/cuae", 
        (etch_factory_params*) g_stubparams, nullconnection); 
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


/* - - - - - - - - - - - - - - - - - - -
 * stub's implementing object
 * - - - - - - - - - - - - - - - - - - -
 */

/* a stub's object is in practice a server implementation, masked by xxxx_either_impl, 
 * such as perf_server_impl. the obj_type will be ETCHTYPEB_EXESERVERIMPL -   
 * which is the type get_session_callbacks_from() recognizes to identify the obj as  
 * implementing i_objsession. constructors assigning ETCHTYPEB_EXESERVERIMPL are 
 * new_perf_remote_server() and init_perf_server_impl().
 * stub.obj becomes this object. the delivery service session is set to the stub.
 */

#if(0)

CLIENT SIDE
perf_helper.new_remote_server()
  remote_server = new_perf_remote_server (NULL, deliverysvc, vf);
  i_perf_client* myclient = p->client = p->new_client(remote_server);
  perf_client_stub* client_stub = new_perf_client_stub (p);
     newstub->stub_base = new_stub (implobj, stubtype, ids, qp, fp);
        /* set ds session to be stub's i_sessionmessage */
        ids->itm->set_session (ids->itm, stubbase->isession);
        /* copy stub implementor's i_objsession to the stub */
        stubbase->impl_callbacks = etchstub_get_session_callbacks_from (implobj); 
#endif


/* - - - - - - - - - - -
 * implobj i_objsession 
 * - - - - - - - - - - - 
 */

/**
 * mysv_session_control()
 * remote server i_objsession._session_control()
 * @param control event, caller relinquishes.
 * @param value control value, caller relinquishes.
 * assuming we return a zero result to etchstub_session_control(), 
 * etchstub_session_control() assumes the control and value objects have been   
 * assumed by this method, so we destroy them accordingly.
 */
static int mysv_session_control (void* data, etch_event* control, etch_object* param)
{
    etch_int32* value = (etch_int32*)param;
    if (control)
    {   gsv_controlval = control->value;
        etch_object_destroy(control);
    }
    if (value)
    {   gsv_valueval = value->value;
        etch_object_destroy(value);
    }
    gsv_what = SESSION_CONTROL;
    return 0;
}


/**
 * mysv_session_notify()
 * remote server i_objsession._session_notify()
 * @param evt event, caller relinquishes.
 * assuming we return a zero result to etchstub_session_notify(), 
 * etchstub_session_notify() assumes the event object has been   
 * assumed by this method, so we destroy it accordingly.
 */
static int mysv_session_notify (void* data, etch_event* evt)
{
    if (evt)
    {   gsv_eventval = evt->value;
        etch_object_destroy(evt);
    }
    gsv_what  = SESSION_NOTIFY;
    return 0;
}


/**
 * mysv_session_query()
 * remote server i_objsession._session_query()
 * @param query, caller relinquishes. 
 * assuming we return a non-null result object to etchstub_session_query(), 
 * etchstub_session_query() assumes the query object has been assumed by 
 * this method, so we destroy it accordingly.
 */
static etch_int32* mysv_session_query (void* data, etch_query* query) 
{
    if (query)
    {   gsv_queryval = query->value;
        etch_free(query);
    }
    gsv_what = SESSION_QUERY;
    return gsv_query_result;
}


/* - - - - - - - - - - - - - - - - -  
 * implementing object constructor
 * - - - - - - - - - - - - - - - - -  
 */

/**
 * mysv_howdy
 * implementation of my_implobj.howdy().
 */
static int mysv_howdy (my_implobj* thisx, i_delivery_service* ids, etch_who* who, etch_message* msg)
{
    gsv_what = HOWDY;
    gsv_ds   = ids;
    gsv_who  = who;
    gsv_message = msg;
    return 0;
}


/**
 * destroy_my_implobj()
 * destructor for our fake stub impl object
 */
static int destroy_my_implobj(void* data)
{
    my_implobj* thisx = (my_implobj*)data;
    etch_free(thisx->either_base->iobjsession);
    etch_object_destroy(thisx->either_base);
    thisx->either_base = NULL;

    return destroy_objectex((etch_object*) thisx);
}


/**
 * new_my_implobj()
 * instatiate and return an implementing object for the test stub.
 * the only requisite of the stub's implementing object vis a vis this test,
 * is that it implement the i_objsession interface. the stub makes this
 * determination in etchstub_get_session_callbacks_from().
 */
static my_implobj* new_my_implobj()
{
    i_objsession* ios = NULL;

    /* for the test it doesn't matter whether we use obj_type of client or server,
     * it needs to be one or the other so etchstub_get_session_callbacks_from() 
     * will be able to extract its objession interface. in pratice the implementing
     * object would be either a client implementation or a server implementation,
     * however the test uses only the session interfaces so the object is androgynous.
     */
    my_implobj* implobj = (my_implobj*) new_object (sizeof(my_implobj), 
        ETCHTYPEB_EXECLIENTIMPL, get_dynamic_classid_unique(&CLASSID_MY_IMPLOBJ)); 

    implobj->either_base = (i_xxxx_either*) new_object (sizeof(i_xxxx_either), 
        ETCHTYPEB_EXECLIENTBASE, get_dynamic_classid_unique(&CLASSID_MY_IMPLBASE));

    /* populate as much of i_xxxx_either as we need */
    ((etch_object*)implobj)->destroy = destroy_my_implobj;
    implobj->either_base->thisx = (etch_object*) implobj;  
    ios = new_default_objsession_interface (implobj);
    implobj->either_base->iobjsession = ios;
    implobj->either_base->_session_control = ios->_session_control = mysv_session_control;
    implobj->either_base->_session_notify  = ios->_session_notify  = mysv_session_notify;
    implobj->either_base->_session_query   = ios->_session_query   = mysv_session_query;
 
    /* populate as much of xxxx_either_impl as we need */
    implobj->iobjsession = ios;
    implobj->_session_control = ios->_session_control; 
    implobj->_session_notify  = ios->_session_notify; 
    implobj->_session_query   = ios->_session_query; 

    implobj->howdy = mysv_howdy; /* custom method */
    return implobj;
}


/* - - - - - - - - - - -
 * thread pool override 
 * - - - - - - - - - - -
 */

/**
 * thistest_proxy_run()
 * intercept of thread pool's run() which susequently calls the real run()
 */
static etch_thread* thistest_proxy_threadpool_run (etch_threadpool* pool, etch_threadproc threadproc, void* threaddata)
{    
    switch(pool->pooltype)
    { 
        case ETCH_THREADPOOLTYPE_FREE:
             g_is_run_on_free_pool = TRUE;
             return g_real_fpool_run (pool, threadproc, threaddata);

        case ETCH_THREADPOOLTYPE_QUEUED:
             g_is_run_on_queued_pool = TRUE;
             return g_real_qpool_run (pool, threadproc, threaddata);
    }

    return NULL;
}


/* - - - - - - - - - - - - - - - - - - -
 * stub 
 * - - - - - - - - - - - - - - - - - - -
 */

/**
 * stubhelper_howdy()
 * stub helper (execution logic) for the mt_howdy message type. such methods    
 * conform to typedef opaque_stubhelper and are defined with the stub, and
 * attached to the type in the stub implementation constructor. 
 */
static int mystub_run_howdy (void* stub, i_delivery_service* ds, i_xxxx_either* obj, etch_who* whofrom, etch_message* msg)
{
    int result = 0;
    my_implobj* implobj = NULL;
    if (NULL == obj || NULL == obj->thisx)
    {   /* we can't use cunit macros away from the main thread, so we log errors instead */
        ETCH_LOG(LOGSRC, ETCH_LOG_ERROR, "stub helper object null pointer\n"); 
        g_stub_errors++;
        return -1;
    }

    implobj = (my_implobj*) obj->thisx;

    if (((etch_object*)implobj)->class_id != CLASSID_MY_IMPLOBJ)
    {   ETCH_LOG(LOGSRC, ETCH_LOG_ERROR, "unexpected stub helper object type\n"); 
        g_stub_errors++;
        return -1;
    }

    /* execute implemented service method */
    result = implobj->howdy (implobj, ds, whofrom, msg);   

    if (0 != result)
    {   ETCH_LOG(LOGSRC, ETCH_LOG_ERROR, "service method implementation failed\n"); 
        g_stub_errors++;
        return -1;
    }

    return result;
}


/**
 * new_my_stubparams()
 * the stub carries around an etch_server_factory parameter bundle, which
 * contains not only the server "factories" (virtual constructors, not used
 * in these tests), but also various other parameters needed by methods which
 * see the stub, and which a real-world stub would not have access to globally. 
 * one such parameter is the "server", masked by i_xxxx_either, from which the 
 * stub helper exepcts to extract the stub implementation object. we therefore
 * must instantiate one of these objects for the purposes of this test.
 */
static etch_server_factory* new_my_stubparams(xxxx_either_impl* implobj, etch_threadpool* qp, etch_threadpool* fp)
{
    g_stubparams = new_server_factory (NULL, NULL, NULL, NULL);  
    g_stubparams->fpool = fp;
    g_stubparams->qpool = qp;
    // g_stubparams->in_delsvc = g_my_ds;
    g_stubparams->in_resx = g_my_resources;    
    g_stubparams->in_valufact = (etch_value_factory*) g_my_vf;
    /* params->server is an i_xxxx_server, expected by the stub helper
     * (mystub_run_howdy in this case) */    
    g_my_iserver = etch_malloc(sizeof(i_xxxx_either), 0);
    memset(g_my_iserver, 0, sizeof(i_xxxx_either));
    g_my_iserver->thisx  = (etch_object*) implobj; 
    // g_stubparams->server = g_my_iserver;
    return g_stubparams;
}
 

/**
 * new_stub()
 * etch_stub (stub base) constructor. 
 * since we are testing only the stub base, we don't have access to the value 
 * factory through it, and so will instead use our global value factory to access 
 * the howdy message type and then set its stub helper to the above.
 */
static etch_stub* new_mystub (xxxx_either_impl* implobj, unsigned char stubtype, 
    i_delivery_service* ids, etch_threadpool* qp, etch_threadpool* fp) 
// TODO REMOVE DS FROM THIS API
{
    etch_stub* stubbase = NULL;
    xxxx_either_stub* stubimpl = NULL;
    i_xxxx_either* stubimplobj = implobj->either_base;

    setup_this_stub(implobj, stubtype, qp, fp);
    assert(is_etch_sessionmsg(g_my_dsimpl->sessionmsg));

    // stub base is instatiated in new_stubimpl_init()
    // stubbase = new_stub (implobj, stubtype, g_my_ds, qp, fp);

    stubimpl = new_stubimpl_init (stubimplobj, sizeof(xxxx_either_stub), 
        stubtype, NULL, g_my_ds, qp, fp, g_stubparams);

    stubbase = stubimpl->stub_base;
    stubbase->is_implobj_owned = TRUE; /* let stubbase destroy its implobj */
 
    assert(is_etch_sessionmsg(g_my_dsimpl->sessionmsg));

    /* fyi new_stub() has copied the objesession interface from implobj thusly:   
     * stubbase->impl_callbacks = etchstub_get_session_callbacks_from (implobj); 
     */

    /* instantiate parameter bundle to provide stubbase helper arguments */
    // stubbase->params = new_my_stubparams(implobj, qp, fp);   
    // stubbase->params->stubbase = stubbase;   

    /* these tests call the stubbase's session_message(), session_notify, etc. 
     * note that the stubbase's i_sessionmessage implementations, which are
     * etchstub_session_message, etchstub_session_notify(), etc., invoke the
     * objsession callbacks as set per comment immediately above. 
     * of particular note is etchstub_session_message, which gets the type
     * from the message, gets the stubbase "helper" (run procedure) from the message
     * type, checks the type's async mode, and runs the proc inline or on a thread
     * accordingly.
     */
    etchtype_set_type_stubhelper (g_mt_howdy, mystub_run_howdy);
    stubbase->session_message = myds_session_message;

    return stubbase;
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
 * test_session_message_1()
 * test a stub runner run on main thread 
 */
static void test_session_message_1(void)
{
    setup_this_test();

    do 
    {   int result = 0;   
        etch_message* newmsg = NULL;
        my_implobj* my_session = new_my_implobj (); /* should we move this to setup? */

        etch_stub* stub = new_mystub ((xxxx_either_impl*) my_session, 
            ETCH_STUBTYPE_CLIENT, g_my_ds, g_qpool, g_fpool);  

        etchtype_set_async_mode (g_mt_howdy, ETCH_ASYNCMODE_NONE);

        newmsg = new_message (g_mt_howdy, ETCH_DEFSIZE, (etch_value_factory*) g_my_vf);

        /* fyi the default stub->session_message is etchstub_session_message(), 
         * which we do not override. this method gets the type from the message,
         * get's the stub helper proc from the type, checks the type's async mode,
         * and runs the stub helper proc inline or on a thread accordingly.
         */

        /* todo in some places we pass session_message a delivery service, 
         * but here we pass the stub. wtf? find the discrepancy and fix it. 
         * ds can get the stub via its ism.thisx. stub can get ds via stub.params.
         */
        result = stub->session_message (stub, g_who, newmsg);
 
        CU_ASSERT_EQUAL(result, 0);  /* 0 indicates message handled */
        CU_ASSERT_EQUAL(g_stub_errors, 0);  
        CU_ASSERT_EQUAL(HOWDY, gsv_what); 
        CU_ASSERT_EQUAL(g_my_transport, gsv_ds);  
        CU_ASSERT_EQUAL(g_who, gsv_who); 
        CU_ASSERT_EQUAL(newmsg, gsv_message); 
        CU_ASSERT_EQUAL(g_is_run_on_queued_pool, FALSE);
        CU_ASSERT_EQUAL(g_is_run_on_free_pool, FALSE);    
       
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
 * test_session_message_2()
 * test a stub runner run on queued thread pool 
 */
static void test_session_message_2(void)
{
    setup_this_test();

    do 
    {   int result = 0;   
        etch_message* newmsg = NULL;
        my_implobj* my_session = new_my_implobj (); /* should we move this to setup? */

        etch_stub* stub = new_mystub ((xxxx_either_impl*) my_session, 
            ETCH_STUBTYPE_CLIENT, g_my_ds, g_qpool, g_fpool);  

        etchtype_set_async_mode (g_mt_howdy, ETCH_ASYNCMODE_QUEUED);

        newmsg = new_message (g_mt_howdy, ETCH_DEFSIZE, (etch_value_factory*) g_my_vf);

        /* fyi the default stub->session_message is etchstub_session_message(), 
         * which we do not override. this method gets the type from the message,
         * get's the stub helper proc from the type, checks the type's async mode,
         * and runs the stub helper proc inline or on a thread accordingly.
         */
        result = stub->session_message (stub, g_who, newmsg);
 
        CU_ASSERT_EQUAL(result, 0);  /* 0 indicates message handled */
        CU_ASSERT_EQUAL(g_stub_errors, 0);  
        CU_ASSERT_EQUAL(HOWDY, gsv_what); 
        CU_ASSERT_EQUAL(g_my_transport, gsv_ds);  
        CU_ASSERT_EQUAL(g_who, gsv_who); 
        CU_ASSERT_EQUAL(newmsg, gsv_message); 

        /* if we don't implement a queued pool, a free pool was substituted */
        #if ETCH_HAS_QUEUED_THREADPOOL
        CU_ASSERT_EQUAL(g_is_run_on_queued_pool, TRUE);
        CU_ASSERT_EQUAL(g_is_run_on_free_pool, FALSE);  
        #else
        CU_ASSERT_EQUAL(g_is_run_on_free_pool, TRUE);  
        #endif
       
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
 * test_session_message_3()
 * test a stub runner run on free thread pool 
 */
static void test_session_message_3(void)
{
    setup_this_test();

    do 
    {   int result = 0;   
        etch_message* newmsg = NULL;
        my_implobj* my_session = new_my_implobj (); /* should we move this to setup? */

        etch_stub* stub = new_mystub ((xxxx_either_impl*) my_session, 
            ETCH_STUBTYPE_CLIENT, g_my_ds, g_qpool, g_fpool);  

        etchtype_set_async_mode (g_mt_howdy, ETCH_ASYNCMODE_FREE);

        newmsg = new_message (g_mt_howdy, ETCH_DEFSIZE, (etch_value_factory*) g_my_vf);

        /* fyi the default stub->session_message is etchstub_session_message(), 
         * which we do not override. this method gets the type from the message,
         * get's the stub helper proc from the type, checks the type's async mode,
         * and runs the stub helper proc inline or on a thread accordingly.
         */
        result = stub->session_message (stub, g_who, newmsg);
 
        CU_ASSERT_EQUAL(result, 0);  /* 0 indicates message handled */
        CU_ASSERT_EQUAL(g_stub_errors, 0);  
        CU_ASSERT_EQUAL(HOWDY, gsv_what); 
        CU_ASSERT_EQUAL(g_my_transport, gsv_ds);  
        CU_ASSERT_EQUAL(g_who, gsv_who); 
        CU_ASSERT_EQUAL(newmsg, gsv_message); 
        CU_ASSERT_EQUAL(g_is_run_on_queued_pool, FALSE);
        CU_ASSERT_EQUAL(g_is_run_on_free_pool, TRUE);  
       
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
 * test_session_message_4()
 * test sending a message whose type has no stub runner
 */
static void test_session_message_4(void)
{
    setup_this_test();

    do 
    {   int result = 0, was_msg_handled = 0;   
        etch_message* newmsg = NULL;
        my_implobj* my_session = new_my_implobj (); /* should we move this to setup? */

        etch_stub* stub = new_mystub ((xxxx_either_impl*) my_session, 
            ETCH_STUBTYPE_CLIENT, g_my_ds, g_qpool, g_fpool);  

        newmsg = new_message (g_mt_nogood, ETCH_DEFSIZE, (etch_value_factory*) g_my_vf);

        /* fyi the default stub->session_message is etchstub_session_message(), 
         * which we do not override. this method gets the type from the message,
         * get's the stub helper proc from the type, checks the type's async mode,
         * and runs the stub helper proc inline or on a thread accordingly.
         */
        result = stub->session_message (stub, g_who, newmsg);

        was_msg_handled = (0 == result);
        CU_ASSERT_EQUAL(was_msg_handled, FALSE);   
        CU_ASSERT_EQUAL(gsv_what, 0); 
        CU_ASSERT_PTR_NULL(gsv_ds);  
        CU_ASSERT_PTR_NULL(gsv_who); 
        CU_ASSERT_PTR_NULL(gsv_message); 

        /* fyi messages are not relinquished on failure 
         * since caller may want to reroute the message on failure */
        if (!was_msg_handled)
            etch_object_destroy(newmsg);
       
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
 * test_session_query()
 */
static void test_session_query(void)
{
    setup_this_test();

    do 
    {   const int THISQUERYVAL = 12345;
        etch_object* resultobj = NULL;
        my_implobj* my_session = new_my_implobj ();  

        etch_stub* stub = new_mystub ((xxxx_either_impl*) my_session, 
            ETCH_STUBTYPE_SERVER, g_my_ds, g_qpool, g_fpool);  

        resultobj = stub->session_query (stub, new_etch_query(0, THISQUERYVAL));

        CU_ASSERT_PTR_NOT_NULL(resultobj); 
        CU_ASSERT_PTR_EQUAL(resultobj, gsv_query_result); 
        CU_ASSERT_EQUAL(SESSION_QUERY, gsv_what);
        CU_ASSERT_EQUAL(THISQUERYVAL, gsv_queryval);

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
 * test_session_control()
 */
static void test_session_control(void)
{
    setup_this_test();

    do 
    {   const int THISCONTROLVAL = 12345, THISVALUEVAL = 54321;
        int result = 0;
        my_implobj* my_session = new_my_implobj ();  

        etch_stub* stub = new_mystub ((xxxx_either_impl*) my_session, 
            ETCH_STUBTYPE_SERVER, g_my_ds, g_qpool, g_fpool);  

        result = stub->session_control (stub, new_etch_control(0, THISCONTROLVAL), new_int32(THISVALUEVAL));

        CU_ASSERT_EQUAL(result,0); 
        CU_ASSERT_EQUAL(SESSION_CONTROL, gsv_what);
        CU_ASSERT_EQUAL(THISCONTROLVAL, gsv_controlval);
        CU_ASSERT_EQUAL(THISVALUEVAL, gsv_valueval);

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
 * test_session_notify()
 */
static void test_session_notify(void)
{
    setup_this_test();

    do 
    {   const int THISNOTIFYVAL = 12345;
        int result = 0;
        my_implobj* my_session = new_my_implobj ();  

        etch_stub* stub = new_mystub ((xxxx_either_impl*) my_session, 
            ETCH_STUBTYPE_SERVER, g_my_ds, g_qpool, g_fpool);  

        result = stub->session_notify (stub, new_etch_event (0, THISNOTIFYVAL));

        CU_ASSERT_EQUAL(result,0); 
        CU_ASSERT_EQUAL(SESSION_NOTIFY, gsv_what);
        CU_ASSERT_EQUAL(THISNOTIFYVAL, gsv_eventval);

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

/**
 * main   
 */
//int wmain( int argc, wchar_t* argv[], wchar_t* envp[])
CU_pSuite test_etch_stub_suite()
{
    CU_pSuite ps =  CU_add_suite("stub base test suite", init_suite, clean_suite);

    // THESE TESTS ARE BROKEN
    // TODO go back to java test and reimplement these tests
    //CU_add_test(ps, "test test setup and teardown", test_setup);
    //CU_add_test(ps, "test send message main thread", test_session_message_1);
    //CU_add_test(ps, "test send message queued pool", test_session_message_2);
    //CU_add_test(ps, "test send message freepool", test_session_message_3);
    //CU_add_test(ps, "test send message no good", test_session_message_4);   
    //CU_add_test(ps, "test session query", test_session_query); 
    //CU_add_test(ps, "test session control", test_session_control); 
    //CU_add_test(ps, "test session notify", test_session_notify); 
    
    return ps;
}
