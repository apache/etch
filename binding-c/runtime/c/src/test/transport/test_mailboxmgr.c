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
 * test_mailboxmgr.c 
 */
#include "etch_runtime.h"
#include "etch_plain_mailbox.h"  
#include "etch_plain_mailbox_manager.h"
#include "etch_transport_message.h"
#include "etch_default_value_factory.h"
#include "etch_map.h"
#include "etch_log.h"
#include "etch_exception.h"
#include "etch_simpletimer.h"
#include "etch_objecttypes.h"
#include "etch_general.h"
#include "etch_object.h"

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

/* - - - - - - - - - - - - - - 
 * unit test support
 * - - - - - - - - - - - - - -
 */


typedef enum etch_what
{ WHAT_NONE, 
  SESSION_MESSAGE,   SESSION_QUERY,   SESSION_CONTROL,   SESSION_NOTIFY,
  TRANSPORT_MESSAGE, TRANSPORT_QUERY, TRANSPORT_CONTROL, TRANSPORT_NOTIFY,
} etch_what;


static etch_plainmailboxmgr*  g_manager;
static i_sessionmessage*      g_my_session;
static i_transportmessage*    g_my_transport; 
static etch_who*              g_who1;
static etch_type*             g_type1;
static etch_mutex*            g_rwlock;
static default_value_factory* g_vf;
static vf_idname_map*         g_typemap;
static class_to_type_map*     g_c2tmap;

static int g_is_unregistered;  
static int g_mailbox_status;

static unsigned short CLASSID_MY_IMPL_TP;
static unsigned short CLASSID_MY_IMPL_SM;
#define OBJTYPE_MY_IMPL_TM 0x5170
#define OBJTYPE_MY_IMPL_SM 0x5171

#define is_my_impl_tm(x) (x && ((etch_object*)x)->obj_type == OBJTYPE_MY_IMPL_TM) 
#define is_my_impl_sm(x) (x && ((etch_object*)x)->obj_type == OBJTYPE_MY_IMPL_SM) 

typedef struct my_impl_transportmessage my_impl_transportmessage;
typedef struct my_impl_sessionmessage   my_impl_sessionmessage;

static my_impl_transportmessage* new_my_impl_transportmessage();
static my_impl_sessionmessage*   new_my_impl_sessionmessage();


#if 0
/**
 * mymboxmgr_unregister()
 * override for mailbox manager unregister
 */
static int mymboxmgr_unregister (i_mailbox_manager* imgr, i_mailbox* mbox)
{
    g_is_unregistered = TRUE;
    return 0;
}


/**
 * mymboxmgr_redeliver()
 * override for mailbox manager redeliver
 */
static int mymboxmgr_redeliver (i_mailbox_manager* imgr, etch_who* whofrom, etch_message* msg)
{
    return 0;
} 


/**
 * my_mailbox_notify()
 * override for mailbox notify
 */
static int my_mailbox_notify (etch_plainmailbox* mbox, i_mailbox* mb, etch_object* state, const int is_closed) 
{
    g_mailbox = mb;
    g_mailbox_state = state;
    g_mailbox_status = TRUE;
    g_mailbox_isclosed = is_closed;
    return 0;
}

static int is_equal_who(etch_who* who1, etch_who* who2)
{
    int n1 = 0, n2 = 0;
    if (!who1 || !who2) return FALSE;
    if (((etch_object*)who1)->class_id != CLASSID_WHO || ((etch_object*)who2)->class_id != CLASSID_WHO) return FALSE;
    if (!who1->value  || !who2->value) return FALSE;
    if (!is_etch_int32(who1->value) || !is_etch_int32(who2->value)) return FALSE;
    n1 = ((etch_int32*)who1->value)->value;
    n2 = ((etch_int32*)who2->value)->value;
    return n1 == n2;
}
#endif


/**
 * new_add_message
 * convenience method to create a message of type "add"
 */
static etch_message* new_add_message()
{
    /* this call gets the "add" type from the value factory's types map.  
     * if the type is not present, one is created and added to the map. the vf's 
     * types map contains all builtin types, plus user types such as this. when  
     * the vf is destroyed, its types map is destroyed (if we let the vf create  
     * its own map at construction time rather than supplying a map to the vf
     * constructor). when a types map is destroyed, it "destroys" its types.
     * however builtin types are marked static and so the type destructor will
     * take no action. user types are not so marked and so will be destroyed
     * at that time.
     */
    etch_type* mt_add = etchtypemap_get_by_name(g_vf->types, L"add");
    etch_message* newmsg = new_message(mt_add, ETCH_DEFSIZE, (etch_value_factory*) g_vf);
    etchtype_put_validator(mt_add, clone_field(builtins._mf__message_id), (etch_object*) etchvtor_int64_get(0)); 
    return newmsg;
}


/**
 * get_add_result_type
 */
static etch_type* get_add_result_type()
{
    etch_type* mt_add_result = etchtypemap_get_by_name(g_vf->types, L"add_result");
    etchtype_put_validator(mt_add_result, clone_field(builtins._mf__message_id), (etch_object*) etchvtor_int64_get(0)); 
    etchtype_put_validator(mt_add_result, clone_field(builtins._mf__in_reply_to),(etch_object*) etchvtor_int64_get(0));  
    return mt_add_result;
}


/**
 * new_add_result_message
 * convenience method to create a message of type "add_result"
 */
static etch_message* new_add_result_message()
{
    /* this call gets the "add_result" type from the value factory's types map.  
     * if the type is not present, one is created and added to the map. the vf's 
     * types map contains all builtin types, plus user types such as this. when  
     * the vf is destroyed, its types map is destroyed (if we let the vf create  
     * its own map at construction time rather than supplying a map to the vf
     * constructor). when a types map is destroyed, it "destroys" its types.
     * however builtin types are marked static and so the type destructor will
     * take no action. user types are not so marked and so will be destroyed
     * at that time.
     */
    etch_type* mt_add_result = get_add_result_type();
    etch_message* newmsg = new_message(mt_add_result, ETCH_DEFSIZE, (etch_value_factory*) g_vf);
    return newmsg;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 * my_impl_transportmessage (i_transportmessage implementation) 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 */

/**
 * my_impl_transportmessage
 * test object implementing i_transportmessage
 */
//typedef struct my_impl_transportmessage
struct my_impl_transportmessage
{
    unsigned int hashkey;  
    unsigned short  obj_type; 
    unsigned short  class_id;
    struct etch_object* vtab;  
    int  (*destroy)(void*);
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;
    struct etch_object* parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;

    /* i_transportmessage interface and methods, plus original destructor
     * which becomes replaced with a custom destructor to destroy this
     * object. this is the model for destroying an interface wrapper object
     * when we do not save and pass around a pointer to the wrapper, but rather
     * a pointer to the interface. the interface in question, i_transportmessage
     * in this case, contains a pointer to the wrapper object, in this case a
     * my_impl_transportmessage*. when the interface is instantiated, its original 
     * destructor is saved, and is replaced with a destructor which invokes
     * the wrapper's destructor. the wrapper destructor must then know to 
     * invoke the interface's original destructor when destroying the interface.
     */
    i_transportmessage* ixm;       /* owned */ 
    etch_object_destructor destroy_transportmessage;  /* i_transportmessage original destructor */
    etch_transport_message transport_message;   /* transport_message() */

    i_sessionmessage* session;    /* not owned */

    etch_who*       recipient;    /* not owned */
    etch_message*   msg;          /* not owned */ 
    etch_what       what;         
    size_t          bufcount;  
    char*           buf;          /* owned */    
    etch_object*    query;        /* owned */
    etch_object*    query_result; /* owned */
    etch_object*    control;      /* owned */
    etch_object*    value;        /* owned */
    etch_object*    eventx;       /* owned */
};
//} my_impl_transportmessage;


/**
 * destroy_my_impl_transportmessage()
 * my_impl_transportmessage destructor
 */
static int destroy_my_impl_transportmessage (void* data)
{
  my_impl_transportmessage* thisx = (my_impl_transportmessage*)data;
    if (!is_etchobj_static_content(thisx))
    {       /* invoke original i_transportmessage destructor */
        if (thisx->ixm && thisx->destroy_transportmessage)   
            thisx->destroy_transportmessage(thisx->ixm);

        if (thisx->buf)
            etch_free(thisx->buf);

        
        etch_object_destroy(thisx->query);
        etch_object_destroy(thisx->query_result);
        etch_object_destroy(thisx->control);
        etch_object_destroy(thisx->value);
        etch_object_destroy(thisx->eventx);
    }

   return destroy_objectex((etch_object*) thisx);
}


/**
 * impl_transport_message()
 * my_impl_transportmessage::transport_message
 * @param whoto caller retains, can be null
 * @param fbuf caller retains
 */
static int impl_transport_message (void* data, void* who, void* messageData)
{
    my_impl_transportmessage* mytm = (my_impl_transportmessage*)data;
    etch_who* whoto = (etch_who*)who;
    etch_message* msg = (etch_message*)messageData;

    CU_ASSERT_FATAL(is_my_impl_tm(mytm));
    mytm->what = TRANSPORT_MESSAGE;
    mytm->recipient = whoto;
    mytm->msg = msg;
    return 0;
}


/**
 * my_transport_control()
 * my_impl_transportmessage::itransport::transport_control 
 */
static int my_transport_control (my_impl_transportmessage* mytm, etch_object* control, etch_object* value)
{
    CU_ASSERT_FATAL(is_my_impl_tm(mytm));
    mytm->what    = TRANSPORT_CONTROL;
    mytm->control = control;
    mytm->value   = value;
    return 0;
}


/**
 * my_transport_notify()
 * my_impl_transportmessage::itransport::transport_notify 
 */
static int my_transport_notify (my_impl_transportmessage* mytm, etch_object* evt)
{
    CU_ASSERT_FATAL(is_my_impl_tm(mytm));
    mytm->what   = TRANSPORT_NOTIFY;
    mytm->eventx = evt;
    return 0;
}


/**
 * my_transport_query()
 * my_impl_transportmessage::itransport::transport_query 
 */
static etch_object* my_transport_query (my_impl_transportmessage* mytm, etch_object* query) 
{
    etch_object* resultobj = NULL;
    CU_ASSERT_FATAL(is_my_impl_tm(mytm));
    resultobj   = mytm->query_result; /* set artificially in test */
    mytm->what  = TRANSPORT_QUERY;
    mytm->query = query;
    mytm->query_result = NULL;
    return (etch_object*) resultobj;  /* caller owns */
}


/**
 * my_transport_get_session()
 * my_impl_transportmessage::itransport::get_session 
 */
static i_sessionmessage* my_transport_get_session(my_impl_transportmessage* mytm)
{
    CU_ASSERT_FATAL(is_my_impl_tm(mytm));
    return mytm->session;
}


/**
 * my_transport_set_session()
 * my_impl_transportmessage::itransport::set_session
 */
static void my_transport_set_session (my_impl_transportmessage* mytm, i_sessionmessage* session)
{   
    CU_ASSERT_FATAL(is_my_impl_tm(mytm));
    mytm->session = session;
}


/*
 * destroy_my_transportmessage()
 * i_transportmessage destructor
 * this destructor will destroy its parent (my_impl_transportmessage), 
 * which will in turn destroy this object.
 */
static int destroy_my_transportmessage (void* data)
{
  i_transportmessage* itm = (i_transportmessage*)data;
    my_impl_transportmessage* mytp = NULL;
    if (NULL == itm) return -1;

    mytp = itm->thisx;  

    etch_object_destroy(mytp);

    return 0;
}


/**
 * new_my_impl_transportmessage()
 * my_impl_transportmessage constructor
 */
static my_impl_transportmessage* new_my_impl_transportmessage()
{
    i_transportmessage* itp  = NULL;
    i_transport* itransport = NULL;
    /* this is a model for dynamic class ID assigment */
    unsigned short class_id = CLASSID_MY_IMPL_TP? CLASSID_MY_IMPL_TP: (CLASSID_MY_IMPL_TP = get_dynamic_classid());

    my_impl_transportmessage* mytp = (my_impl_transportmessage*) new_object
        (sizeof(my_impl_transportmessage), OBJTYPE_MY_IMPL_TM, class_id);

    mytp->destroy = destroy_my_impl_transportmessage;

    itransport = new_transport_interface_ex(mytp,
        (etch_transport_control)     my_transport_control, 
        (etch_transport_notify)      my_transport_notify, 
        (etch_transport_query)       my_transport_query,
        (etch_transport_get_session) my_transport_get_session, 
        (etch_transport_set_session) my_transport_set_session);

    itp = new_transportmsg_interface(mytp, impl_transport_message, itransport);

    /* save off i_transportmessage destructor */
    mytp->destroy_transportmessage = ((etch_object*)itp)->destroy;

    /* replace i_transportmessage destructor with one which will destroy this object */
    ((etch_object*)itp)->destroy = destroy_my_transportmessage;

    /* g_my_transport is set to this interface */
    mytp->ixm = itp;  

    return mytp;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 * my_impl_sessionmessage (i_sessionmessage implementation) 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 */

/**
 * my_impl_sessionmessage
 * test object implementing i_sessionmessage
 */
struct my_impl_sessionmessage
{
    etch_object object;

    /*
     * i_sessionmessage interface and methods, plus original destructor
     * which becomes replaced with a custom destructor to destroy this
     * object. this is the model for destroying an interface wrapper object
     * when we do not save and pass around a pointer to the wrapper, but rather
     * a pointer to the interface. the interface in question, i_sessionmessage
     * in this case, contains a pointer to the wrapper object, in this case a
     * my_impl_sessionmessage*. when the interface is instantiated, its original 
     * destructor is saved, and is replaced with a destructor which invokes
     * the wrapper's destructor. the wrapper destructor must then know to 
     * invoke the interface's original destructor when destroying the interface.
     */
    i_sessionmessage* ism;          /* owned */
    etch_object_destructor destroy_sessionmessage; /* i_sessionmessage original destructor */
    etch_session_message session_message;  /* session_message() method */

    etch_what       what;
    etch_who*       sender;         /* not owned */
    etch_message*   msg;            /* not owned */
    int             is_msg_handled;
    etch_object*    query;          /* owned */
    etch_object*    query_result;   /* owned */
    etch_object*    control;        /* owned */
    etch_object*    value;          /* owned */
    etch_object*    eventx;         /* owned */
};
//} my_impl_sessionmessage;



/**
 * destroy_my_impl_sessionmessage()
 * my_impl_sessionmessage destructor
 */
static int destroy_my_impl_sessionmessage(void* data)
{
  my_impl_sessionmessage* thisx = (my_impl_sessionmessage*)data;
    if (!is_etchobj_static_content(thisx))
    {       /* invoke original i_sessionmessage destructor */
        if (thisx->ism && thisx->destroy_sessionmessage)
            thisx->destroy_sessionmessage(thisx->ism);

        /* these are objects which would be destroyed in the binding 
         * by the last method to touch them */
        etch_object_destroy(thisx->msg);

        etch_object_destroy(thisx->query);

        etch_object_destroy(thisx->query_result);

        etch_object_destroy(thisx->control);

        etch_object_destroy(thisx->value);

        etch_object_destroy(thisx->eventx);
    }

   return destroy_objectex((etch_object*) thisx);
}


/**
 * impl_session_message()
 * my_impl_sessionmessage::ism::session_message.
 * @param whofrom caller retains, can be null.
 * @param msg caller abandons
 */
static int impl_session_message (void* data, etch_who* whofrom, etch_message* msg)
{
    my_impl_sessionmessage* mysm = (my_impl_sessionmessage*)data;

    CU_ASSERT_FATAL(is_my_impl_sm(mysm));
    mysm->what = SESSION_MESSAGE;

    /* in this emulation we are the session consuming a message. if successful, 
     * (i.e., the message is handled), the binding will eventually destroy the 
     * message (the caller relinquishes message memory), and the who object.
     * if not successful (message not handled) the caller retains message and 
     * who memory in order to forward the message and who somewhere else
     * (as an unwanted message). so we model that here: if the message is not 
     * handled (a manual switch in these tests), we do not save references to
     * the messaqe and who for cleanup, because the unwanted message, containing
     * these objects, will be cleaned up instead. 
     */
    if (mysm->is_msg_handled)
    {   
        mysm->msg = msg;
        mysm->sender = whofrom;
        return 0;
    }
    
    mysm->msg = NULL;
    mysm->sender = NULL;
    return -1;
}


/**
 * my_session_control()
 * my_impl_sessionmessage::ism::isession::session_control 
 * control and value are always abandoned by caller so mysm must clean them up.
 */
static int my_session_control (my_impl_sessionmessage* mysm, etch_object* control, etch_object* value)
{
    CU_ASSERT_FATAL(is_my_impl_sm(mysm));
    mysm->what    = SESSION_CONTROL;
    mysm->control = control;
    mysm->value   = value;
    return 0;
}


/**
 * my_session_notify()
 * my_impl_sessionmessage::ism::isession::session_notify 
 * evt is always abandoned by caller so mysm must clean it up.
 */
static int my_session_notify (my_impl_sessionmessage* mysm, etch_object* evt)
{
    CU_ASSERT_FATAL(is_my_impl_sm(mysm));
    mysm->what   = SESSION_NOTIFY;
    mysm->eventx = evt;
    return 0;
}


/**
 * my_session_query()
 * my_impl_sessionmessage::ism::isession::session_query 
 * query is always abandoned by caller so mysm must clean it up.
 */
static etch_object* my_session_query (my_impl_sessionmessage* mysm, etch_object* query) 
{
    etch_object* resultobj = NULL;
    CU_ASSERT_FATAL(is_my_impl_sm(mysm));
    resultobj   = mysm->query_result; /* artifically set in test */
    mysm->what  = SESSION_QUERY;
    mysm->query = query;
    mysm->query_result = NULL;
    return (etch_object*) resultobj; /* caller owns */
}


/*
 * destroy_my_sessionmessage()
 * i_sessionmessage destructor
 * this destructor will destroy its parent (my_impl_sessionmessage), 
 * which will in turn destroy this object.
 */
static int destroy_my_sessionmessage(void* data)
{
  i_sessionmessage* ism = (i_sessionmessage*)data;
    my_impl_sessionmessage* mysm = NULL;
    if (NULL == ism) return -1;

    mysm = ism->thisx;  

    etch_object_destroy(mysm);

    return 0;
}


/**
 * new_my_impl_sessionmessage()
 * my_impl_sessionmessage constructor
 */
static my_impl_sessionmessage* new_my_impl_sessionmessage()
{
    i_sessionmessage* ism  = NULL;
    i_session* isession = NULL;
    /* this is a model for dynamic class ID assigment */
    unsigned short class_id = CLASSID_MY_IMPL_SM? CLASSID_MY_IMPL_SM: 
        (CLASSID_MY_IMPL_SM = get_dynamic_classid());

    my_impl_sessionmessage* mysm = (my_impl_sessionmessage*) new_object
      (sizeof(my_impl_sessionmessage), OBJTYPE_MY_IMPL_SM, class_id);

    ((etch_object*)mysm)->destroy = destroy_my_impl_sessionmessage;

    isession = new_session_interface(mysm,
        (etch_session_control)     my_session_control, 
        (etch_session_notify)      my_session_notify, 
        (etch_session_query)       my_session_query);

    ism = new_sessionmsg_interface(mysm, impl_session_message, isession);

    /* save off i_sessionmessage destructor */
    mysm->destroy_sessionmessage = ((etch_object*)ism)->destroy;

    /* custom destructor will destroy the my_impl_sessionmessage */
    ((etch_object*)ism)->destroy = destroy_my_sessionmessage;

    /* g_my_session will get set to this interface */
    mysm->ism = ism;  

    return mysm;
}


/* - - - - - - - - - - - - - - - 
 * setup/teardown for each test
 * - - - - - - - - - - - - - - -
 */

/**
 * setup_this_test()
 * set up an individual unit test
 */
static int setup_this_test()
{
    my_impl_transportmessage* mytm_impl = NULL;
    my_impl_sessionmessage*   mysm_impl = NULL;

    g_who1   = new_who(new_int32(1));
    g_type1  = new_type(L"type1");

    g_typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(g_typemap);
    g_c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(g_c2tmap);  
    defvf_initialize_static(g_typemap, g_c2tmap);
    g_vf = new_default_value_factory(g_typemap, g_c2tmap);
    CU_ASSERT_PTR_NOT_NULL(g_vf);

    // TODO: pool
    etch_mutex_create(&g_rwlock, ETCH_MUTEX_UNNESTED, NULL);

    etchtype_put_validator(g_type1, clone_field(builtins._mf__message_id), (etch_object*) etchvtor_int64_get(0));

    /* we instantiate a wrapper x which implements and instantiates i_transportmessage.
     * the instantiation of i_transportmessage will contain a pointer to x.
     * our global reference g_my_transport is a pointer to the interface.
     * the purpose of this excercise is that, in the real binding we can pass
     * around the interface, whose methods can be then invoked without knowing
     * anything about the wrapper. when we want to reference the wrapper x, 
     * it is (my_impl_transportmessage) g_my_transport->thisx. 
     */
    mytm_impl = new_my_impl_transportmessage();
    g_my_transport = mytm_impl->ixm;

    mysm_impl = new_my_impl_sessionmessage();
    g_my_session = mysm_impl->ism;

    g_manager = new_plain_mailbox_manager (g_my_transport, NULL, NULL, g_rwlock); 
    g_manager->set_session (g_manager, g_my_session);  
    
    return g_manager? 0: -1;
}


/**
 * teardown_this_test()
 * tear down an individual unit test
 */
static void teardown_this_test()
{
    etch_object_destroy(g_who1);
    g_who1 = NULL;

    etch_object_destroy(g_type1);
    g_type1 = NULL;

    etch_object_destroy(g_vf);
    g_vf = NULL;

    etch_object_destroy(g_c2tmap);
    g_c2tmap = NULL;

    etch_object_destroy(g_typemap);
    g_typemap = NULL;

    // TODO: cleanup statics
    //etchvf_free_builtins()

    etch_object_destroy(g_manager);
    g_manager = NULL;

    etch_mutex_destroy(g_rwlock);
    g_rwlock = NULL;

    etch_object_destroy(g_my_transport);
    g_my_transport = NULL;

    etch_object_destroy(g_my_session);
    g_my_session = NULL;

    g_is_unregistered = g_mailbox_status = 0;
    etchvf_free_builtins(); 
}


/* - - - - - - - - - - - - - - 
 * unit tests 
 * - - - - - - - - - - - - - -
 */

/**
 * test_test_setup()
 * run test setup and teardown and verify all memory accounted for.
 */
static void test_test_setup(void)
{
    int result = setup_this_test();
    CU_ASSERT_EQUAL_FATAL(result,0);
    teardown_this_test();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_constructor()
 */
static void test_constructor(void)
{
    setup_this_test();

    do
    {  etch_object* session = 0;
       CU_ASSERT_PTR_NOT_NULL_FATAL(g_my_transport);  /* g_my_transport->thisx = my_impl_transportmessage */
      
       session = (etch_object*)g_my_transport->get_session (g_my_transport->thisx);
       CU_ASSERT(is_etch_sessionmsg(session));
       CU_ASSERT(is_etch_sessionmsg(g_manager->session));
       /* used to work, no longer is the case, why are these no longer the same? */
       // CU_ASSERT_PTR_EQUAL(g_manager->session, session);

       CU_ASSERT_PTR_NOT_NULL_FATAL(g_my_session);
       CU_ASSERT_PTR_EQUAL(g_my_session, g_manager->get_session (g_manager));

       CU_ASSERT_PTR_EQUAL(g_my_transport, g_manager->transport);

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
 * test_session_query
 */
static void test_session_query(void)
{
    setup_this_test();

    do
    {   const int MY_QUERY_CLASSID = 0x111, MY_RESULT_CLASSID = 0x112;
        etch_object* myqueryobj = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_QUERY_CLASSID);
        etch_object* myresultob = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_RESULT_CLASSID);
        etch_object* query_result = NULL;
        my_impl_sessionmessage* my_sessionimpl = g_my_session->thisx;  
        my_sessionimpl->query_result = myresultob;

        query_result = g_manager->session_query(g_manager, (etch_query*)myqueryobj);

        CU_ASSERT_PTR_NOT_NULL_FATAL(query_result);
        CU_ASSERT_EQUAL(my_sessionimpl->what, SESSION_QUERY);
        CU_ASSERT_PTR_EQUAL(my_sessionimpl->query, myqueryobj);
        CU_ASSERT_PTR_EQUAL(myresultob, query_result);
        etch_object_destroy(myresultob);

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
 * test_session_control
 */
static void test_session_control(void)
{
    setup_this_test();

    do
    {   int result = 0, MY_CONTROL_CLASSID = 0x111, MY_VALUE_CLASSID = 0x112;
        etch_object* mycontrolobj = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_CONTROL_CLASSID);
        etch_object* myvalueobj = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_VALUE_CLASSID);
        my_impl_sessionmessage* my_sessionimpl = g_my_session->thisx;  

        result = g_manager->session_control(g_manager, (etch_event*)mycontrolobj, myvalueobj);

        CU_ASSERT_EQUAL(result,0);
        CU_ASSERT_EQUAL(my_sessionimpl->what, SESSION_CONTROL);
        CU_ASSERT_PTR_EQUAL(my_sessionimpl->control, mycontrolobj);
        CU_ASSERT_PTR_EQUAL(my_sessionimpl->value, myvalueobj);

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
 * test_session_notify
 */
static void test_session_notify(void)
{
    setup_this_test();

    do
    {   int result = 0, MY_EVENT_CLASSID = 0x111;
        etch_object* myeventobj = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_EVENT_CLASSID);
        my_impl_sessionmessage* my_sessionimpl = g_my_session->thisx;  

        result = g_manager->session_notify(g_manager, (etch_event*)myeventobj);

        CU_ASSERT_EQUAL(result,0);
        CU_ASSERT_EQUAL(my_sessionimpl->what, SESSION_NOTIFY);
        CU_ASSERT_PTR_EQUAL(my_sessionimpl->eventx, myeventobj);

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
 * test_transport_query
 */
static void test_transport_query(void)
{
    setup_this_test();

    do
    {   const int MY_QUERY_CLASSID = 0x111, MY_RESULT_CLASSID = 0x112;
        etch_object* myqueryobj = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_QUERY_CLASSID);
        etch_object* myresultob = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_RESULT_CLASSID);
        etch_object* query_result = NULL;
        my_impl_transportmessage* my_transportimpl = g_my_transport->thisx;  
        my_transportimpl->query_result = myresultob;

        query_result = g_manager->transport_query(g_manager, (etch_query*)myqueryobj);

        CU_ASSERT_PTR_NOT_NULL_FATAL(query_result);
        CU_ASSERT_EQUAL(my_transportimpl->what, TRANSPORT_QUERY);
        CU_ASSERT_PTR_EQUAL(my_transportimpl->query, myqueryobj);
        CU_ASSERT_PTR_EQUAL(myresultob, query_result);
        etch_object_destroy(myresultob);

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
 * test_transport_control
 */
static void test_transport_control(void)
{
    setup_this_test();

    do
    {   int result = 0, MY_CONTROL_CLASSID = 0x111, MY_VALUE_CLASSID = 0x112;
        etch_object* mycontrolobj = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_CONTROL_CLASSID);
        etch_object* myvalueobj = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_VALUE_CLASSID);
        my_impl_transportmessage* my_transportimpl = g_my_transport->thisx;  

        result = g_manager->transport_control(g_manager, (etch_event*)mycontrolobj, myvalueobj);
        CU_ASSERT_EQUAL(result,0);
        CU_ASSERT_EQUAL(my_transportimpl->what, TRANSPORT_CONTROL);
        CU_ASSERT_PTR_EQUAL(my_transportimpl->control, mycontrolobj);
        CU_ASSERT_PTR_EQUAL(my_transportimpl->value, myvalueobj);

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
 * test_transport_notify
 */
static void test_transport_notify(void)
{
    setup_this_test();

    do
    {   int result = 0, MY_EVENT_CLASSID = 0x111;
        etch_object* myeventobj = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_EVENT_CLASSID);
        my_impl_transportmessage* my_transportimpl = g_my_transport->thisx;  

        result = g_manager->transport_notify(g_manager, (etch_event*)myeventobj);
        CU_ASSERT_EQUAL(result,0);
        CU_ASSERT_EQUAL(my_transportimpl->what, TRANSPORT_NOTIFY);
        CU_ASSERT_PTR_EQUAL(my_transportimpl->eventx, myeventobj);

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
 * test_transport_message_1()
 */
static void test_transport_message_1(void)
{
    setup_this_test();

    do
    {   int  result = 0;
        i_mailbox* mailbox = NULL;
        etch_message* addmsg = NULL;
        etch_int64* addmsg_id = NULL, *addmsg_inreplyto = NULL;

        my_impl_transportmessage* my_transport_impl = (my_impl_transportmessage*) g_my_transport->thisx;
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport_impl);
        CU_ASSERT_EQUAL(my_transport_impl->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_transport_impl->recipient);
        CU_ASSERT_PTR_NULL(my_transport_impl->msg);

        addmsg = new_add_message();
        CU_ASSERT_PTR_NOT_NULL_FATAL(addmsg);
        
        addmsg_id =  etch_message_get_id(addmsg);
        CU_ASSERT_PTR_NULL(addmsg_id);
        addmsg_inreplyto = etch_message_get_in_reply_to(addmsg);
        CU_ASSERT_PTR_NULL(addmsg_inreplyto);

        result = g_manager->transport_message(g_manager, g_who1, addmsg);

        CU_ASSERT_EQUAL(result, 0);
        CU_ASSERT_EQUAL(my_transport_impl->what, TRANSPORT_MESSAGE);
        CU_ASSERT_EQUAL(my_transport_impl->recipient, g_who1);
        CU_ASSERT_PTR_EQUAL(my_transport_impl->msg, addmsg);

        addmsg_id = etch_message_get_id(addmsg);
        CU_ASSERT_PTR_NOT_NULL(addmsg_id);
        addmsg_inreplyto = etch_message_get_in_reply_to(addmsg);
        CU_ASSERT_PTR_NULL(addmsg_inreplyto);

        mailbox = pmboxmgr_get_mailbox(g_manager, addmsg_id);
        CU_ASSERT_PTR_NULL(mailbox);

        /* transport does not own the message so we destroy it here.
         * a real world transport does not have state such as message, however
         * if the transport is always the message terminal within this test suite
         * we can destroy the message in the my_impl_transportmessage destructor,
         * modeling the real world passing on of the message by the transport.
         */
        etch_object_destroy(addmsg);

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
 * test_transport_message_2()
 */
static void test_transport_message_2(void)
{
    setup_this_test();

    do
    {   int  result = 0;
        const int THISTESTID = 1;
        i_mailbox* mailbox = NULL;
        etch_message* add_resultmsg = NULL;
        etch_int64* add_resultmsg_id = NULL, *add_resultmsg_inreplyto = NULL;

        my_impl_transportmessage* my_transport_impl = (my_impl_transportmessage*) g_my_transport->thisx;
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport_impl);
        CU_ASSERT_EQUAL(my_transport_impl->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_transport_impl->recipient);
        CU_ASSERT_PTR_NULL(my_transport_impl->msg);

        add_resultmsg = new_add_result_message();
        CU_ASSERT_PTR_NOT_NULL_FATAL(add_resultmsg);
        
        add_resultmsg_id = etch_message_get_id(add_resultmsg);
        CU_ASSERT_PTR_NULL(add_resultmsg_id);
        add_resultmsg_inreplyto = etch_message_get_in_reply_to(add_resultmsg);
        CU_ASSERT_PTR_NULL(add_resultmsg_inreplyto);

        result = etch_message_set_in_reply_to(add_resultmsg, new_int64(THISTESTID));
        CU_ASSERT_EQUAL(result, 0);

        result = g_manager->transport_message(g_manager, g_who1, add_resultmsg);
        CU_ASSERT_EQUAL(result, 0);
        CU_ASSERT_EQUAL(my_transport_impl->what, TRANSPORT_MESSAGE);
        CU_ASSERT_EQUAL(my_transport_impl->recipient, g_who1);
        CU_ASSERT_PTR_EQUAL(my_transport_impl->msg, add_resultmsg);

        add_resultmsg_id = etch_message_get_id(add_resultmsg);
        CU_ASSERT_PTR_NOT_NULL(add_resultmsg_id);
        add_resultmsg_inreplyto = etch_message_get_in_reply_to(add_resultmsg);
        CU_ASSERT_PTR_NOT_NULL(add_resultmsg_inreplyto);
        if (add_resultmsg_inreplyto)
        {   CU_ASSERT_EQUAL(add_resultmsg_inreplyto->value, THISTESTID);
        }

        mailbox = pmboxmgr_get_mailbox(g_manager, add_resultmsg_id);
        CU_ASSERT_PTR_NULL(mailbox);

        /* transport does not own the message so we destroy it here.
         * a real world transport does not have state such as message, however
         * if the transport is always the message terminal within this test suite
         * we can destroy the message in the my_impl_transportmessage destructor,
         * modeling the real world passing on of the message by the transport.
         */
        etch_object_destroy(add_resultmsg);

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
 * test_transport_message_3()
 * test attempt to send message that has already been sent
 */
static void test_transport_message_3(void)
{
    setup_this_test();

    do
    {   int  result = 0;
        const int THISTESTID = 1;
        etch_message* addmsg = NULL;
        etch_int64* addmsg_id = NULL;

        my_impl_transportmessage* my_transport_impl = (my_impl_transportmessage*) g_my_transport->thisx;
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport_impl);
        CU_ASSERT_EQUAL(my_transport_impl->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_transport_impl->recipient);
        CU_ASSERT_PTR_NULL(my_transport_impl->msg);

        addmsg = new_add_message();
        CU_ASSERT_PTR_NOT_NULL_FATAL(addmsg);
        
        addmsg_id = etch_message_get_id(addmsg);
        CU_ASSERT_PTR_NULL(addmsg_id);
        result = etch_message_set_id(addmsg, new_int64(THISTESTID));
        CU_ASSERT_EQUAL(result, 0);
        addmsg_id = etch_message_get_id(addmsg);
        CU_ASSERT_PTR_NOT_NULL_FATAL(addmsg_id);

        result = g_manager->transport_message(g_manager, g_who1, addmsg);
        CU_ASSERT_EQUAL(result, -1); /* should fail as already sent */

        etch_object_destroy(addmsg);

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
 * test_transport_call_1()
 * test sending a call message, then close mailbox for read
 */
static void test_transport_call_1(void)
{
    setup_this_test();

    do
    {   int  result = 0;
        etch_message* addmsg = NULL;
        etch_plainmailbox* my_mbox_impl = NULL;
        i_mailbox *mailbox = NULL, *got_mailbox = NULL;
        etch_int64* addmsg_id = NULL, *addmsg_inreplyto = NULL;

        my_impl_transportmessage* my_transport_impl = (my_impl_transportmessage*) g_my_transport->thisx;
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport_impl);
        CU_ASSERT_EQUAL(my_transport_impl->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_transport_impl->recipient);
        CU_ASSERT_PTR_NULL(my_transport_impl->msg);

        g_manager->session_notify (g_manager, new_etch_event(0, ETCHEVT_SESSION_UP));

        addmsg = new_add_message();
        CU_ASSERT_PTR_NOT_NULL_FATAL(addmsg);

        result = g_manager->transport_call (g_manager->imanager, g_who1,addmsg, &mailbox); 

        CU_ASSERT_EQUAL(result, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(mailbox);

        CU_ASSERT_EQUAL(my_transport_impl->what, TRANSPORT_MESSAGE);
        CU_ASSERT_EQUAL(my_transport_impl->recipient, g_who1);
        CU_ASSERT_PTR_EQUAL(my_transport_impl->msg, addmsg);

        result = pmboxmgr_size(g_manager);
        CU_ASSERT_EQUAL(result, 1);

        addmsg_id = etch_message_get_id(addmsg);
        CU_ASSERT_PTR_NOT_NULL(addmsg_id);
        addmsg_inreplyto = etch_message_get_in_reply_to(addmsg);
        CU_ASSERT_PTR_NULL(addmsg_inreplyto);

        got_mailbox = pmboxmgr_get_mailbox(g_manager, addmsg_id);
        CU_ASSERT_PTR_NOT_NULL(got_mailbox);
        CU_ASSERT_PTR_EQUAL(mailbox, got_mailbox);

        mailbox->close_read (mailbox);

        result = pmboxmgr_size(g_manager);
        CU_ASSERT_EQUAL(result, 0);

        got_mailbox = pmboxmgr_get_mailbox(g_manager, addmsg_id);
        CU_ASSERT_PTR_NULL(got_mailbox);

        /* in practice the last entity to handle the message would destroy it */
        etch_object_destroy(addmsg);

        /* it is not yet determined whether the mailbox manager should destroy an
         * unregistered mailbox - for now it does not, so we destroy it now. */
        my_mbox_impl = mailbox->thisx;  /* get implementation from interface */
        etch_object_destroy(my_mbox_impl);

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
 * test_transport_call_2()
 * test sending a call message, then close mailbox for delivery
 */
static void test_transport_call_2(void)
{
    setup_this_test();

    do
    {   int  result = 0;
        etch_message* addmsg = NULL;
        etch_plainmailbox* my_mbox_impl = NULL;
        i_mailbox *mailbox = NULL, *got_mailbox = NULL;
        etch_int64* addmsg_id = NULL, *addmsg_inreplyto = NULL;

        my_impl_transportmessage* my_transport_impl = (my_impl_transportmessage*) g_my_transport->thisx;
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport_impl);
        CU_ASSERT_EQUAL(my_transport_impl->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_transport_impl->recipient);
        CU_ASSERT_PTR_NULL(my_transport_impl->msg);

        g_manager->session_notify(g_manager, new_etch_event(0, ETCHEVT_SESSION_UP));

        addmsg = new_add_message();
        CU_ASSERT_PTR_NOT_NULL_FATAL(addmsg);

        result = g_manager->transport_call(g_manager->imanager, g_who1,addmsg, &mailbox); 
        CU_ASSERT_EQUAL(result, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(mailbox);

        CU_ASSERT_EQUAL(my_transport_impl->what, TRANSPORT_MESSAGE);
        CU_ASSERT_EQUAL(my_transport_impl->recipient, g_who1);
        CU_ASSERT_PTR_EQUAL(my_transport_impl->msg, addmsg);

        result = pmboxmgr_size(g_manager);
        CU_ASSERT_EQUAL(result, 1);

        addmsg_id = etch_message_get_id(addmsg);
        CU_ASSERT_PTR_NOT_NULL(addmsg_id);
        addmsg_inreplyto = etch_message_get_in_reply_to(addmsg);
        CU_ASSERT_PTR_NULL(addmsg_inreplyto);

        got_mailbox = pmboxmgr_get_mailbox(g_manager, addmsg_id);
        CU_ASSERT_PTR_NOT_NULL(got_mailbox);
        CU_ASSERT_PTR_EQUAL(mailbox, got_mailbox);

        mailbox->close_delivery(mailbox);

        result = pmboxmgr_size(g_manager);
        CU_ASSERT_EQUAL(result, 0);

        got_mailbox = pmboxmgr_get_mailbox(g_manager, addmsg_id);
        CU_ASSERT_PTR_NULL(got_mailbox);

        /* in practice the last entity to handle the message would destroy it */
        etch_object_destroy(addmsg);

        /* it is not yet determined whether the mailbox manager should destroy an
         * unregistered mailbox - for now it does not, so we destroy it now. */
        my_mbox_impl = mailbox->thisx;  /* get implementation from interface */
        etch_object_destroy(my_mbox_impl);

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
 * test_transport_call_3()
 * test attempt to send a call message that has already been sent
 */
static void test_transport_call_3(void)
{
    setup_this_test();

    do
    {   int result = 0;
        i_mailbox* mailbox = NULL;
        etch_int64* addmsg_id = NULL;

        etch_message* addmsg = new_add_message();
        CU_ASSERT_PTR_NOT_NULL_FATAL(addmsg);
        result = etch_message_set_id(addmsg, new_int64(1));  /* set ID essentially marking message sent */
        CU_ASSERT_EQUAL(result, 0);

        result = g_manager->transport_call(g_manager->imanager, g_who1, addmsg, &mailbox); 
        CU_ASSERT_NOT_EQUAL(result, 0);  /* result should indicate error, already sent */

        CU_ASSERT_PTR_NULL(mailbox); /* no mailbox should have been created */
        result = pmboxmgr_size(g_manager);
        CU_ASSERT_EQUAL(result, 0);

        addmsg_id = etch_message_get_id(addmsg);
        CU_ASSERT_PTR_NOT_NULL_FATAL(addmsg_id);
        mailbox = pmboxmgr_get_mailbox(g_manager, addmsg_id);
        CU_ASSERT_PTR_NULL(mailbox); /* no mailbox should have been created */

        etch_object_destroy(addmsg);

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
 * test_transport_call_4()
 * test attempt to send a call message that is marked as a reply message
 */
static void test_transport_call_4(void)
{
    setup_this_test();

    do
    {   int result = 0;
        etch_message* addresultmsg = new_add_result_message();
        CU_ASSERT_PTR_NOT_NULL_FATAL(addresultmsg);
        result = etch_message_set_in_reply_to(addresultmsg, new_int64(1));  /* set in reply to ID */
        CU_ASSERT_EQUAL(result, 0);

        result = g_manager->transport_call(g_manager->imanager, g_who1, addresultmsg, NULL); 
        CU_ASSERT_NOT_EQUAL(result, 0);  /* result should indicate error, message is a reply */

        result = pmboxmgr_size(g_manager); /* no mailbox should have been created */
        CU_ASSERT_EQUAL(result, 0);

        etch_object_destroy(addresultmsg);

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
 * test_session_message_1
 * test reply to a message without a reply to ID
 */
void test_session_message_1(void)
{
    setup_this_test();

    do
    {   int result = 0;
        etch_message* addmsg = NULL;

        my_impl_sessionmessage* my_sessionimpl = g_my_session->thisx;    
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_sessionimpl);
        CU_ASSERT_EQUAL(my_sessionimpl->what, 0);
        CU_ASSERT_PTR_NULL(my_sessionimpl->sender);
        CU_ASSERT_PTR_NULL(my_sessionimpl->msg);
         
        addmsg = new_add_message();
        CU_ASSERT_PTR_NOT_NULL_FATAL(addmsg);

        my_sessionimpl->is_msg_handled = TRUE;
                 
        /* we pass impl rather than interface here - is this right */  
        /* on success we relinquish ownership of the message to the session */
                        
        result = g_manager->session_message(g_manager, g_who1, addmsg);

        CU_ASSERT_EQUAL(result, 0);  /* result should indicate message was handled */

        CU_ASSERT_EQUAL(my_sessionimpl->what, SESSION_MESSAGE);
        CU_ASSERT_PTR_EQUAL(my_sessionimpl->sender, g_who1);
        CU_ASSERT_PTR_EQUAL(my_sessionimpl->msg, addmsg);         

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
 * test_session_message_2
 * test reply to a message without a reply to ID
 */
static void test_session_message_2(void)
{
    setup_this_test();

    do
    {   int result = 0;
        etch_message* addmsg = NULL;

        my_impl_sessionmessage* my_sessionimpl = g_my_session->thisx;    
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_sessionimpl);
        CU_ASSERT_EQUAL(my_sessionimpl->what, 0);
        CU_ASSERT_PTR_NULL(my_sessionimpl->sender);
        CU_ASSERT_PTR_NULL(my_sessionimpl->msg);
         
        addmsg = new_add_message();
        CU_ASSERT_PTR_NOT_NULL_FATAL(addmsg);

        my_sessionimpl->is_msg_handled = FALSE;  /* the difference from test 1 */
                 
        /* we pass impl rather than interface here - is this right */  
        /* on failure we retain ownership of the message */  
                      
        result = g_manager->session_message (g_manager, g_who1, addmsg);

        CU_ASSERT_NOT_EQUAL_FATAL(result, 0);  /* result should indicate message not handled */

        etch_object_destroy(addmsg); /* since session error we still own the message */

        CU_ASSERT_EQUAL(my_sessionimpl->what, SESSION_MESSAGE);
        CU_ASSERT_PTR_NULL(my_sessionimpl->sender);
        CU_ASSERT_PTR_NULL(my_sessionimpl->msg);         

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
 * test_session_message_3
 * test message having reply to ID not matching any mailbox
 */
static void test_session_message_3(void)
{
    setup_this_test();

    do
    {   int result = 0;
        my_impl_sessionmessage* my_sessionimpl = g_my_session->thisx;    

        etch_message* addresultmsg = new_add_result_message();
        /* no mailbox should be found for this reply to ID */
        etch_message_set_in_reply_to(addresultmsg, new_int64(1));
             
        my_sessionimpl->is_msg_handled = TRUE;   
                 
        /* we pass impl rather than interface here - is this right */  
        /* on failure we retain ownership of the message */                        
        result = g_manager->session_message(g_manager, g_who1, addresultmsg);
        CU_ASSERT_NOT_EQUAL_FATAL(result, 0);  /* result should indicate message not handled */

        etch_object_destroy(addresultmsg); /* since session error we still own the message */

        CU_ASSERT_EQUAL(my_sessionimpl->what, 0);
        CU_ASSERT_PTR_NULL(my_sessionimpl->sender);
        CU_ASSERT_PTR_NULL(my_sessionimpl->msg);         

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
 * test_session_message_5
 * test message having reply to ID matching an open mailbox
 */
static void test_session_message_5(void)
{
    setup_this_test();

    do
    {   int result = 0;
        etch_int64* msgid = NULL;
        etch_type* mt_add_result = NULL;
        etch_mailbox_element* thiselt = NULL;
        etch_plainmailbox* my_mbox_impl = NULL;
        i_mailbox* mailbox = NULL, *got_mailbox = NULL;
        my_impl_sessionmessage* my_sessionimpl = g_my_session->thisx;  
        my_impl_transportmessage* my_transport_impl = g_my_transport->thisx;  

        etch_message *replymsg = NULL, *addmsg = new_add_message();
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_sessionimpl);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport_impl);

        g_manager->session_notify(g_manager, new_etch_event(0, ETCHEVT_SESSION_UP));

        result = g_manager->transport_call(g_manager->imanager, g_who1, addmsg, &mailbox); 
        CU_ASSERT_EQUAL(result, 0);  
        CU_ASSERT_PTR_NOT_NULL_FATAL(mailbox);
             
        CU_ASSERT_EQUAL(my_transport_impl->what, TRANSPORT_MESSAGE);
        CU_ASSERT_PTR_EQUAL(my_transport_impl->recipient, g_who1);
        CU_ASSERT_PTR_EQUAL(my_transport_impl->msg, addmsg);

        result = pmboxmgr_size(g_manager);
        CU_ASSERT_EQUAL(result, 1);
 
        msgid = etch_message_get_id(addmsg);
        CU_ASSERT_PTR_NOT_NULL_FATAL(msgid);
        got_mailbox = pmboxmgr_get_mailbox(g_manager, msgid);
        CU_ASSERT_PTR_EQUAL(got_mailbox, mailbox);

        /* as of now our custom types are not compiled statically. perhaps this will change,
         * or perhaps we'll simply continue to do it this way in the C binding. */
        mt_add_result = get_add_result_type();

        /* construct a reply message */
        replymsg = etch_message_reply(addmsg, mt_add_result);
        result = is_etch_exception(replymsg);
        CU_ASSERT_EQUAL_FATAL(result, FALSE);
        my_sessionimpl->what = WHAT_NONE;

        /* we pass impl rather than interface here - is this right */
        /* post reply message to mailbox */  
        /* on success we relinquish ownership of the reply message */                        
        result = g_manager->session_message(g_manager, g_who1, replymsg);
        CU_ASSERT_EQUAL_FATAL(result, 0);  /* result should indicate message handled */

        CU_ASSERT_EQUAL(my_sessionimpl->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_sessionimpl->sender);
        CU_ASSERT_PTR_NULL(my_sessionimpl->msg);

        /* pop reply message from the mailbox */
        result = mailbox->read(mailbox, &thiselt); 
        CU_ASSERT_EQUAL_FATAL(result, 0);   
        CU_ASSERT_PTR_EQUAL(thiselt->whofrom, g_who1);
        CU_ASSERT_PTR_EQUAL(thiselt->msg, replymsg);
        etch_object_destroy(thiselt);   /* we read it, we own it */
        replymsg = NULL;  /* thiselt destructor destroyed replymsg */

        result = pmboxmgr_size(g_manager);
        CU_ASSERT_EQUAL(result, 1);  /* expect one mailbox */

        result = mailbox->close_read(mailbox);
        CU_ASSERT_EQUAL(result, 0);

        result = pmboxmgr_size(g_manager); /* after close, expect zero mailboxes */
        CU_ASSERT_EQUAL(result, 0);

        /* in practice the last entity to handle the message would destroy it */
        etch_object_destroy(addmsg);

        /* it is not yet determined whether the mailbox manager should destroy an
         * unregistered mailbox - for now it does not, so we destroy it now. */
        my_mbox_impl = mailbox->thisx;  /* get implementation from interface */
        etch_object_destroy(my_mbox_impl);    

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
 * test_session_message_6
 * reply via closed mailbox
 */
static void test_session_message_6(void)
{
    setup_this_test();

    do
    {   int result = 0;
        etch_int64* msgid = NULL;
        etch_type* mt_add_result = NULL;
        etch_plainmailbox* my_mbox_impl = NULL;
        i_mailbox* mailbox = NULL, *got_mailbox = NULL;
        my_impl_sessionmessage* my_sessionimpl = g_my_session->thisx;  
        my_impl_transportmessage* my_transport_impl = g_my_transport->thisx;  

        etch_message *replymsg = NULL, *addmsg = new_add_message();
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_sessionimpl);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport_impl);

        g_manager->session_notify(g_manager, new_etch_event(0, ETCHEVT_SESSION_UP));

        result = g_manager->transport_call(g_manager->imanager, g_who1, addmsg, &mailbox); 
        CU_ASSERT_EQUAL(result, 0);  
        CU_ASSERT_PTR_NOT_NULL_FATAL(mailbox);
             
        CU_ASSERT_EQUAL(my_transport_impl->what, TRANSPORT_MESSAGE);
        CU_ASSERT_PTR_EQUAL(my_transport_impl->recipient, g_who1);
        CU_ASSERT_PTR_EQUAL(my_transport_impl->msg, addmsg);

        result = pmboxmgr_size(g_manager);
        CU_ASSERT_EQUAL(result, 1);
 
        msgid = etch_message_get_id(addmsg);
        CU_ASSERT_PTR_NOT_NULL_FATAL(msgid);
        got_mailbox = pmboxmgr_get_mailbox(g_manager, msgid);
        CU_ASSERT_PTR_EQUAL(got_mailbox, mailbox);

        result = mailbox->close_read(mailbox);
        CU_ASSERT_EQUAL(result, 0);

        result = pmboxmgr_size(g_manager);
        CU_ASSERT_EQUAL(result, 0);   

        /* construct a reply message */
        mt_add_result = get_add_result_type();
        replymsg = etch_message_reply(addmsg, mt_add_result);
        result = is_etch_exception(replymsg);
        CU_ASSERT_EQUAL_FATAL(result, FALSE);
        my_sessionimpl->what = WHAT_NONE;

        /* we pass impl rather than interface here - is this right */
        /* on failure we retain ownership of the reply message */                        
        result = g_manager->session_message(g_manager, g_who1, replymsg);
        CU_ASSERT_NOT_EQUAL_FATAL(result, 0);  /* result should indicate message not handled */
        etch_object_destroy(replymsg); replymsg = NULL;

        CU_ASSERT_EQUAL(my_sessionimpl->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_sessionimpl->sender);
        CU_ASSERT_PTR_NULL(my_sessionimpl->msg);

        /* in practice the last entity to handle the message would destroy it */
        etch_object_destroy(addmsg);

        /* it is not yet determined whether the mailbox manager should destroy an
         * unregistered mailbox - for now it does not, so we destroy it now. */
        my_mbox_impl = mailbox->thisx;  /* get implementation from interface */
        etch_object_destroy(my_mbox_impl);    

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
CU_pSuite test_etch_mailboxmgr_suite()
{    
    CU_pSuite pSuite = CU_add_suite("mailbox manager test suite", init_suite, clean_suite);

    CU_add_test(pSuite, "test test setup",  test_test_setup); 
    CU_add_test(pSuite, "test constructor", test_constructor);

    CU_add_test(pSuite, "test transport query",   test_transport_query);
    CU_add_test(pSuite, "test transport control", test_transport_control);
    CU_add_test(pSuite, "test transport notify",  test_transport_notify);

    CU_add_test(pSuite, "test session query",     test_session_query);
    CU_add_test(pSuite, "test session control",   test_session_control);
    CU_add_test(pSuite, "test session notify",    test_session_notify);

    CU_add_test(pSuite, "test transport message 1", test_transport_message_1);
    CU_add_test(pSuite, "test transport message 2", test_transport_message_2);
    CU_add_test(pSuite, "test transport message 3", test_transport_message_3);

    CU_add_test(pSuite, "test transport call 1",  test_transport_call_1); 
    CU_add_test(pSuite, "test transport call 2",  test_transport_call_2);         
    CU_add_test(pSuite, "test transport call 3",  test_transport_call_3);         
    CU_add_test(pSuite, "test transport call 4",  test_transport_call_4); 
  
    CU_add_test(pSuite, "test session message 1", test_session_message_1);
    CU_add_test(pSuite, "test session message 2", test_session_message_2);
    CU_add_test(pSuite, "test session message 3", test_session_message_3);
    CU_add_test(pSuite, "test session message 5", test_session_message_5);
    CU_add_test(pSuite, "test session message 6", test_session_message_6);

    return pSuite;
}
