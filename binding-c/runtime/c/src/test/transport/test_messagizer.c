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
 * test_messagizer.c 
 */

#include "apr_time.h" /* some apr must be included first */
#include "etch_messagizer.h"  
#include "etch_defvalufact.h"

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etchthread.h"
#include "etch_global.h"
#include "etchmap.h"
#include "etchlog.h"
#include "etch_syncobj.h"
#include "etchexcp.h"

int apr_setup(void);
int apr_teardown(void);
int this_setup();
int this_teardown();
apr_pool_t* g_apr_mempool;
const char* pooltag = "etchpool";


/* - - - - - - - - - - - - - - 
 * unit test infrastructure
 * - - - - - - - - - - - - - -
 */

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

#define IS_DEBUG_CONSOLE FALSE

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
    etch_apr_mempool = g_apr_mempool;
    return 0;
}

int this_teardown()
{    
    return 0;
}


/* - - - - - - - - - - - - - - 
 * unit test support
 * - - - - - - - - - - - - - -
 */
typedef struct test_value_factory test_value_factory;

etch_messagizer*        g_my_messagizer;
i_transportpacket*      g_my_transportpacket;
i_sessionmessage*       g_my_sessionmessage; 
etch_flexbuffer*        g_flexbuffer;                  
etch_resources*         g_my_resources;
vf_idname_map*          g_type_map;
class_to_type_map*      g_class_to_type_map;
etch_who*               g_who;
default_value_factory*  g_my_vf;
test_value_factory*     g_my_test_vf;
int g_which_valuefactory;

unsigned short CLASSID_MY_VF;
unsigned short CLASSID_MY_VF_VTAB;
unsigned short CLASSID_MY_VF_IMPL;
unsigned short CLASSID_MY_IMPL_TP;
unsigned short CLASSID_MY_IMPL_SM;
#define OBJTYPE_MY_IMPL_TP 0x5170
#define OBJTYPE_MY_IMPL_SM 0x5171

#define is_my_impl_tp(x) (x && ((objmask*)x)->obj_type == OBJTYPE_MY_IMPL_TP) 
#define is_my_impl_sm(x) (x && ((objmask*)x)->obj_type == OBJTYPE_MY_IMPL_SM) 

#define THISTEST_HEADERSIZE 8
#define TAGDATA_VERSION     3
#define TYPECODE_EOD_MARK   (-127)
#define THISTEST_WHO_VALUE 0x5151
#define WHICHVF_TESTVF 1
#define WHICHVF_MYVF   2
#define FAKEID_TYPE_ADD         1 
#define FAKEID_TYPE_ADD_RESULT  2 
#define FAKEID_FIELD_X          3 
#define FAKEID_FIELD_Y          4
#define FAKEID_FIELD_RESULT     5

default_value_factory* get_current_valuefactory()
{
    if (g_my_test_vf) return (default_value_factory*) g_my_test_vf;   
    if (g_my_vf)      return g_my_vf;  
    return NULL; 
}

typedef enum etch_what
{ WHAT_NONE, 
  TRANSPORT_PACKET, TRANSPORT_QUERY, TRANSPORT_CONTROL, TRANSPORT_NOTIFY,
  SESSION_MESSAGE,  SESSION_QUERY,   SESSION_CONTROL,   SESSION_NOTIFY
} etch_what;


int is_equal_who(etch_who* who1, etch_who* who2)
{
    int n1 = 0, n2 = 0;
    if (!who1 || !who2) return FALSE;
    if (who1->class_id != CLASSID_WHO || who2->class_id != CLASSID_WHO) return FALSE;
    if (!who1->value  || !who2->value) return FALSE;
    if (!is_etch_int32(who1->value) || !is_etch_int32(who2->value)) return FALSE;
    n1 = ((etch_int32*)who1->value)->value;
    n2 = ((etch_int32*)who2->value)->value;
    return n1 == n2;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 * my_impl_transportpacket (i_transportpacket implementation) 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 */

/**
 * my_impl_transportpacket
 * test object implementing i_transportpacket
 */
typedef struct my_impl_transportpacket
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

    /* i_transportpacket interface and methods, plus original destructor
     * which becomes replaced with a custom destructor to destroy this
     * object. this is the model for destroying an interface wrapper object
     * when we do not save and pass around a pointer to the wrapper, but rather
     * a pointer to the interface. the interface in question, i_transportpacket
     * in this case, contains a pointer to the wrapper object, in this case a
     * my_impl_transportpacket*. when the interface is instantiated, its original 
     * destructor is saved, and is replaced with a destructor which invokes
     * the wrapper's destructor. the wrapper destructor must then know to 
     * invoke the interface's original destructor when destroying the interface.
     */
    i_transportpacket* ixp;       /* owned */ 
    objdtor destroy_transportpacket;  /* i_transportpacket original destructor */
    etch_transport_packet transport_packet;        /* transport_packet() */
    etch_transport_packet_headersize  header_size; /* header_size() */

    i_sessionpacket* session;     /* not owned */

    etch_who*       recipient;    /* not owned */
    etch_what       what;          
    size_t          bufcount;  
    char*           buf;          /* owned */    
    etch_object*    query;        /* owned */
    etch_object*    query_result; /* owned */
    etch_object*    control;      /* owned */
    etch_object*    value;        /* owned */
    etch_object*    eventx;       /* owned */

} my_impl_transportpacket;


/**
 * destroy_my_impl_transportpacket()
 * my_impl_transportpacket destructor
 */
int destroy_my_impl_transportpacket(my_impl_transportpacket* thisx)
{
    assert(is_my_impl_tp(thisx));
    if (thisx->refcount > 0 && --thisx->refcount > 0) return -1;  

    if (!is_etchobj_static_content(thisx))
    {       /* invoke original i_transportpacket destructor */
        if (thisx->ixp && thisx->destroy_transportpacket)   
            thisx->destroy_transportpacket(thisx->ixp);

        if (thisx->buf)
            etch_free(thisx->buf);

        if (thisx->query)
            thisx->query->destroy(thisx->query);

        if (thisx->query_result)
            thisx->query_result->destroy(thisx->query_result);

        if (thisx->control)
            thisx->control->destroy(thisx->control);

        if (thisx->value)
            thisx->value->destroy(thisx->value);

        if (thisx->eventx)
            thisx->eventx->destroy(thisx->eventx);
    }

   return destroy_objectex((objmask*) thisx);
}


/**
 * impl_transport_packet()
 * my_impl_transportpacket::transport_packet
 * @param whoto caller retains, can be null
 * @param fbuf caller retains
 */
int impl_transport_packet (my_impl_transportpacket* mytp, etch_who* whoto, etch_flexbuffer* fbuf)
{
    CU_ASSERT_FATAL(is_my_impl_tp(mytp));
    mytp->what      = TRANSPORT_PACKET;
    mytp->recipient = whoto;   
    /* retrieve the packet data. don't skip over header (0 is skip bytes) */
    mytp->buf = etch_flexbuf_get_allfrom(fbuf, 0, &mytp->bufcount); 
    return 0;
}


/**
 * my_transport_control()
 * my_impl_transportpacket::itransport::transport_control 
 */
int my_transport_control (my_impl_transportpacket* mytp, etch_object* control, etch_object* value)
{
    CU_ASSERT_FATAL(is_my_impl_tp(mytp));
    mytp->what    = TRANSPORT_CONTROL;
    mytp->control = control;
    mytp->value   = value;
    return 0;
}


/**
 * my_transport_notify()
 * my_impl_transportpacket::itransport::transport_notify 
 */
int my_transport_notify (my_impl_transportpacket* mytp, etch_object* evt)
{
    CU_ASSERT_FATAL(is_my_impl_tp(mytp)); 
    mytp->what   = TRANSPORT_NOTIFY;
    mytp->eventx = evt;
    return 0;
}


/**
 * my_transport_query()
 * my_impl_transportpacket::itransport::transport_query 
 */
objmask* my_transport_query (my_impl_transportpacket* mytp, etch_object* query) 
{
    etch_object* resultobj = NULL;
    CU_ASSERT_FATAL(is_my_impl_tp(mytp));
    resultobj = mytp->query_result; /* set artificially in test */
    mytp->what  = TRANSPORT_QUERY;
    mytp->query = query;
    mytp->query_result = NULL;
    return (objmask*) resultobj;  /* caller owns */
}


/**
 * my_transport_get_session()
 * my_impl_transportpacket::itransport::get_session 
 */
i_sessionpacket* my_transport_get_session(my_impl_transportpacket* mytp)
{
    CU_ASSERT_FATAL(is_my_impl_tp(mytp));
    return mytp->session;
}


/**
 * my_transport_set_session()
 * my_impl_transportpacket::itransport::set_session
 */
void my_transport_set_session(my_impl_transportpacket* mytp, i_sessionpacket* session)
{   
    CU_ASSERT_FATAL(is_my_impl_tp(mytp));
    CU_ASSERT_FATAL(is_etch_sessionpacket(session));
    mytp->session = session;
}


/*
 * destroy_my_transportpacket()
 * i_transportpacket destructor
 * this destructor will destroy its parent (my_impl_transportpacket), 
 * which will in turn destroy this object.
 */
int destroy_my_transportpacket(i_transportpacket* itp)
{
    my_impl_transportpacket* mytp = NULL;
    assert(is_etch_transportpkt(itp));

    mytp = itp->thisx;  
    assert(is_my_impl_tp(mytp));

    mytp->destroy(mytp);

    return 0;
}


/**
 * new_my_impl_transportpacket()
 * my_impl_transportpacket constructor
 */
my_impl_transportpacket* new_my_impl_transportpacket()
{
    i_transportpacket* itp  = NULL;
    i_transport* itransport = NULL;
    /* this is a model for dynamic class ID assigment */
    unsigned short class_id = get_dynamic_classid_unique(&CLASSID_MY_IMPL_TP);

    my_impl_transportpacket* mytp = (my_impl_transportpacket*) new_object
      (sizeof(my_impl_transportpacket), OBJTYPE_MY_IMPL_TP, class_id);

    mytp->destroy = destroy_my_impl_transportpacket;

    itransport = new_transport_interface_ex(mytp,
        (etch_transport_control)     my_transport_control, 
        (etch_transport_notify)      my_transport_notify, 
        (etch_transport_query)       my_transport_query,
        (etch_transport_get_session) my_transport_get_session, 
        (etch_transport_set_session) my_transport_set_session);

    itp = new_transportpkt_interface(mytp, impl_transport_packet, itransport);

    /* default header_size() will return this value so no need to override */
    itp->header_size = THISTEST_HEADERSIZE; 

    /* save off i_transportpacket destructor */
    mytp->destroy_transportpacket = itp->destroy;

    /* replace i_transportpacket destructor with one which will destroy this object */
    itp->destroy = destroy_my_transportpacket;

    /* g_my_transportpacket will get set to this interface */
    mytp->ixp = itp;  

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
typedef struct my_impl_sessionmessage
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
    objdtor destroy_sessionmessage; /* i_sessionmessage original destructor */
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

    i_sessionpacket* session;

} my_impl_sessionmessage;



/**
 * destroy_my_impl_sessionmessage()
 * my_impl_sessionmessage destructor
 */
int destroy_my_impl_sessionmessage(my_impl_sessionmessage* thisx)
{
    if (thisx->refcount > 0 && --thisx->refcount > 0) return -1;  

    if (!is_etchobj_static_content(thisx))
    {       /* invoke original i_sessionmessage destructor */
        if (thisx->ism && thisx->destroy_sessionmessage)
            thisx->destroy_sessionmessage(thisx->ism);

        /* these are objects which would be destroyed in the binding 
         * by the last method to touch them */
        if (thisx->msg)
            thisx->msg->destroy(thisx->msg);

        if (thisx->query)
            thisx->query->destroy(thisx->query);

        if (thisx->query_result)
            thisx->query_result->destroy(thisx->query_result);

        if (thisx->control)
            thisx->control->destroy(thisx->control);

        if (thisx->value)
            thisx->value->destroy(thisx->value);

        if (thisx->eventx)
            thisx->eventx->destroy(thisx->eventx);
    }

   return destroy_objectex((objmask*) thisx);
}


/**
 * impl_session_message()
 * my_impl_sessionmessage::ism::session_message.
 * @param whofrom caller retains, can be null.
 * @param msg caller abandons
 */
int impl_session_message (my_impl_sessionmessage* mysm, etch_who* whofrom, etch_message* msg)
{
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
int my_session_control (my_impl_sessionmessage* mysm, etch_object* control, etch_object* value)
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
int my_session_notify (my_impl_sessionmessage* mysm, etch_object* evt)
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
objmask* my_session_query (my_impl_sessionmessage* mysm, etch_object* query) 
{
    etch_object* resultobj = NULL;
    CU_ASSERT_FATAL(is_my_impl_sm(mysm));
    resultobj   = mysm->query_result; /* artifically set in test */
    mysm->what  = SESSION_QUERY;
    mysm->query = query;
    mysm->query_result = NULL;
    return (objmask*) resultobj; /* caller owns */
}


/*
 * destroy_my_sessionmessage()
 * i_sessionmessage destructor
 * this destructor will destroy its parent (my_impl_sessionmessage), 
 * which will in turn destroy this object.
 */
int destroy_my_sessionmessage(i_sessionmessage* ism)
{
    my_impl_sessionmessage* mysm = NULL;
    if (NULL == ism) return -1;

    mysm = ism->thisx;  

    mysm->destroy(mysm);

    return 0;
}


/**
 * new_my_impl_sessionmessage()
 * my_impl_sessionmessage constructor
 */
my_impl_sessionmessage* new_my_impl_sessionmessage()
{
    i_sessionmessage* ism  = NULL;
    i_session* isession = NULL;
    /* this is a model for dynamic class ID assigment */
    unsigned short class_id = CLASSID_MY_IMPL_SM? CLASSID_MY_IMPL_SM: 
        (CLASSID_MY_IMPL_SM = get_dynamic_classid());

    my_impl_sessionmessage* mysm = (my_impl_sessionmessage*) new_object
      (sizeof(my_impl_sessionmessage), OBJTYPE_MY_IMPL_SM, class_id);

    mysm->destroy = destroy_my_impl_sessionmessage;

    isession = new_session_interface(mysm,
        (etch_session_control)     my_session_control, 
        (etch_session_notify)      my_session_notify, 
        (etch_session_query)       my_session_query);

    ism = new_sessionmsg_interface(mysm, impl_session_message, isession);

    /* save off i_sessionmessage destructor */
    mysm->destroy_sessionmessage = ism->destroy;

    /* custom destructor will destroy the my_impl_sessionmessage */
    ism->destroy = destroy_my_sessionmessage;

    /* g_my_sessionmessage will get set to this interface */
    mysm->ism = ism;  

    return mysm;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 * test value factories
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 */

/**
 * test_value_factory
 * value factory version 1
 * this version uses inherited data coded into the object, as opposed to 
 * instantiating an impl object to contain inherited data and methods.
 */
typedef struct test_value_factory
{
    unsigned int     hashkey;  
    unsigned short   obj_type; 
    unsigned short   class_id;
    i_value_factory* vtab;  
    int  (*destroy) (void*);
    void*(*clone)   (void*); 
    obj_gethashkey   get_hashkey;
    struct objmask*  parent;
    etchresult*      result;
    unsigned int     refcount;
    unsigned int     length;
    unsigned char    is_null;
    unsigned char    is_copy;
    unsigned char    is_static;
    unsigned char    reserved;

    /* - - - - - - - - - - - - - 
     * default value factory  
     * - - - - - - - - - - - - - 
     */
    objmask* impl;

    class_to_type_map* class_to_type;
    vf_idname_map*  types;  
    etch_arraylist* mixins;
  
    unsigned char is_own_types;
    unsigned char is_own_class_to_type;

    /* - - - - - - - - - - - - - 
     * test value factory  
     * - - - - - - - - - - - - - 
     */
    etch_type*    mt_add;
    etch_type*    mt_add_result;
    etch_field*   mf_x;
    etch_field*   mf_y;
    etch_field*   mf_result;

} test_value_factory;


/**
 * destroy_test_value_factory()
 * destructor for value factory version 1 
 */
int destroy_test_value_factory(test_value_factory* vf)
{
    if (NULL == vf) return -1;
    if (vf->refcount > 0)
        if (vf->refcount != 1)  /* if refcount is 1 */
        {   vf->refcount--;     /* parent dtor will decrement */
            return -1;
        }

    if (!is_etchobj_static_content(vf))
    {
 	    destroy_static_type(vf->mt_add);
        destroy_static_type(vf->mt_add_result);
        destroy_static_field(vf->mf_x); 
        destroy_static_field(vf->mf_y); 
        destroy_static_field(vf->mf_result);  
    }

    return destroy_default_value_factory((default_value_factory*) vf);
}


/**
 * new_test_valuefactory()
 * constructor for value factory version 1 inheriting from default_value_factory
 */
test_value_factory* new_test_valuefactory()
{
    etchparentinfo* inheritlist = NULL;
    test_value_factory* vf = NULL;

    /* establish global dynamic class IDs for the custom vf. 
     * these global objects are generated by the etch compiler.
     */
    const unsigned short class_id_vf = CLASSID_MY_VF? CLASSID_MY_VF: 
         (CLASSID_MY_VF = get_dynamic_classid());

    const unsigned short class_id_vf_vtab = CLASSID_MY_VF_VTAB? CLASSID_MY_VF_VTAB: 
         (CLASSID_MY_VF_VTAB = get_dynamic_classid());

    g_type_map = new_vf_types_collection(ETCH_DEFSIZE);
    /* since we explicitly instantiate a type map, and since we explicitly destroy
     * the test's custom types, we want the type maps destructor to not destroy
     * the map content. overriding the map's content clear callback is one way 
     * to do this. */
    g_type_map->freehook = etch_noop_clear_handler;  
    g_class_to_type_map  = new_class_to_type_map(ETCH_DEFSIZE);

    /* instantiate the new value factory.  
     * this vf does NOT own its type maps since we supply them here. 
     * however if we wanted to abandon ownership, we could set the   
     * vf.is_own_types and vf.is_own_class_to_type flags here.
     */    
    vf = (test_value_factory*) new_default_value_factory_a
        (sizeof(test_value_factory), g_type_map, g_class_to_type_map);

    vf->destroy = destroy_test_value_factory;

   /* ensure parent type keys exist in the (one-based) inheritance list. 
    * parent class of our custom vf is default_value_factory.
    * inheritance list is used by validators and object assignment logic.
    */
    inheritlist = get_vtab_inheritance_list((objmask*)vf, 2, 1, CLASSID_MY_VF_VTAB);
    inheritlist[1].obj_type = ETCHTYPEB_VALUEFACTORY;  
    inheritlist[1].class_id = CLASSID_VALUEFACTORY;  /* parent class */
    vf->class_id = CLASSID_MY_VF;  /* our class */

    /* instantiate the custom vf's instance data and assign it to the vf. 
     */  
    vf->mt_add = new_static_type(L"add");
    vf->mt_add_result = new_static_type(L"add_result");
    vf->mf_x = new_static_field(L"x");
    vf->mf_y = new_static_field(L"y");
    vf->mf_result = new_static_field(L"xresult");

    /* we replace generated ids with 1-byte IDs to make test data buffers easier to construct */
    vf->mt_add->id        = FAKEID_TYPE_ADD;
    vf->mt_add_result->id = FAKEID_TYPE_ADD_RESULT;
    vf->mf_x->id          = FAKEID_FIELD_X;
    vf->mf_y->id          = FAKEID_FIELD_Y;
    vf->mf_result->id     = FAKEID_FIELD_RESULT;

    vf->vtab->add_type(vf, vf->mt_add);
    vf->vtab->add_type(vf, vf->mt_add_result);

    etchtype_put_validator(vf->mt_add, clone_field(vf->mf_x), 
        (objmask*) etchvtor_int32_get(0));
    etchtype_put_validator(vf->mt_add, clone_field(vf->mf_y), 
        (objmask*) etchvtor_int32_get(0));
    etchtype_put_validator(vf->mt_add, clone_field(builtins._mf__message_id), 
        (objmask*) etchvtor_int64_get(0));

    etchtype_put_validator(vf->mt_add_result, clone_field(vf->mf_result), 
        (objmask*) etchvtor_int32_get(0));
    etchtype_put_validator(vf->mt_add_result, clone_field(builtins._mf__message_id), 
        (objmask*) etchvtor_int64_get(0));
    etchtype_put_validator(vf->mt_add_result, clone_field(builtins._mf__in_reply_to), 
        (objmask*) etchvtor_int64_get(0));

    g_my_test_vf = vf;
    return g_my_test_vf;
}


/**
 * my_valufactory_impl
 * value factory version 2 instance data object
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
 * destructor for inheriting value factory version 2 instance data
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
 * constructor for inheriting value factory version 2 instance data
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

    g_type_map = new_vf_types_collection(ETCH_DEFSIZE);
    /* since we explicitly instantiate a type map, and since we explicitly destroy
     * the test's custom types, we want the type maps destructor to not destroy
     * the map content. overriding the map's content clear callback is one way 
     * to do this. */
    g_type_map->freehook = etch_noop_clear_handler;  
    g_class_to_type_map  = new_class_to_type_map(ETCH_DEFSIZE);

    /* instantiate the new value factory.  
     * this vf does NOT own its type maps since we supply them here. 
     * however if we wanted to abandon ownership, we could clear the   
     * vf.is_own_types and vf.is_own_class_to_type flags here.
     */    
    g_my_vf = new_default_value_factory(g_type_map, g_class_to_type_map);

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


/**
 * get_vftype()
 * return a type depending on which version of value factory is instantiated
 */
etch_type* get_vftype(const int typeid)
{
    etch_type* type = NULL;

    if (g_my_vf)
    {
        my_valufactory_impl* impl = (my_valufactory_impl*) g_my_vf->impl;

        if (typeid == impl->mt_add->id)
            type = impl->mt_add; 
        else
        if (typeid == impl->mt_add_result->id)
            type = impl->mt_add_result; 
    }
    else
    if (g_my_test_vf)
    {
        if (typeid == g_my_test_vf->mt_add->id)
            type = g_my_test_vf->mt_add; 
        else
        if (typeid == g_my_test_vf->mt_add_result->id)
            type = g_my_test_vf->mt_add_result; 
    }

    return type;
}

 
/* - - - - - - - - - - - - - - - - -
 * test messagizer
 * - - - - - - - - - - - - - - - - -
 */

/**
 * new_bogus_messagizer()
 */
etch_messagizer* new_bogus_messagizer(i_transportpacket* transport)
{
    g_my_messagizer = new_messagizer(transport, L"foo:?Messagizer.format=binary", g_my_resources);
    return g_my_messagizer;
}
 

/* - - - - - - - - - - - - - - - - - - - - -
 *  individual test data setup and teardown
 * - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * setup_this_test()
 */
int setup_this_test(const int which_valuefactory)
{
    my_impl_transportpacket* mytp_impl = NULL;
    my_impl_sessionmessage*  mysm_impl = NULL;
    default_value_factory*   thisvf = NULL;
    g_which_valuefactory = which_valuefactory;

    /* two versions of value factory using different inheritance models */
    if (which_valuefactory == WHICHVF_TESTVF)  /* vf version 1 inline inheritance */
    {   thisvf = (default_value_factory*) new_test_valuefactory();     
        set_etchobj_static_all(g_my_test_vf);  /* so resources will not destroy */
    }
    else 
    {   thisvf = new_bogus_valuefactory();     /* vf version 2 impl object inheritance */
        set_etchobj_static_all(g_my_vf);       /* so resources will not destroy */
    }

    g_my_resources = new_etch_resources(ETCH_DEFSIZE);
    etch_resources_add(g_my_resources, ETCH_RESXKEY_MSGIZER_VALUFACT, (objmask*) thisvf); 

    /* we instantiate a wrapper x which implements and instantiates i_transportpacket.
     * the instantiation of i_transportpacket will contain a pointer to x.
     * our global reference g_my_transportpacket is a pointer to the interface.
     * the purpose of this excercise is that, in the real binding we can pass
     * around the interface, whose methods can be then invoked without knowing
     * anything about the wrapper. when we want to reference the wrapper x, 
     * it is (my_impl_transportpacket) g_my_transportpacket->thisx. 
     */
    mytp_impl = new_my_impl_transportpacket();
    g_my_transportpacket = mytp_impl->ixp;

    /* we instantiate a wrapper y which implements and instantiates i_sessionmessage.
     * the instantiation of i_sessionmessage will contain a pointer to y.
     * our global reference g_my_sessionmessage is a pointer to the interface.
     * the purpose of this excercise is that, in the real binding we can pass
     * around the interface, whose methods can be then invoked without knowing
     * anything about the wrapper. when we want to reference the wrapper x, 
     * it is (my_impl_sessionmessage*) g_my_sessionmessage->thisx. 
     */
    mysm_impl = new_my_impl_sessionmessage();
    g_my_sessionmessage = mysm_impl->ism;
    
    g_who = new_who(new_int32(THISTEST_WHO_VALUE), TRUE);

    /* finally instantiate the test messagizer */
    new_bogus_messagizer(g_my_transportpacket);
    CU_ASSERT_PTR_NOT_NULL_FATAL(g_my_messagizer);
    etch_msgizer_set_session(g_my_messagizer, g_my_sessionmessage);
  
    return 0; 
}


/**
 * teardown_this_test()
 */
int teardown_this_test()
{
    g_my_messagizer->destroy(g_my_messagizer);

    g_my_transportpacket->destroy(g_my_transportpacket);

    g_my_sessionmessage->destroy(g_my_sessionmessage);

    g_my_resources->destroy(g_my_resources);

    if (g_my_vf)
    {   clear_etchobj_static_all(g_my_vf);
        g_my_vf->destroy(g_my_vf);
    }
    else  /* can only instantiate one or the other */ 
    if (g_my_test_vf)
    {   clear_etchobj_static_all(g_my_test_vf);
        g_my_test_vf->destroy(g_my_test_vf);
    }
   
    g_who->destroy(g_who);

    g_type_map->destroy(g_type_map);    // ************************************************
    g_class_to_type_map->destroy(g_class_to_type_map);

    if (g_flexbuffer)
        g_flexbuffer->destroy(g_flexbuffer);

    g_my_transportpacket = NULL;
    g_my_sessionmessage = NULL; 
    g_class_to_type_map = NULL;
    g_my_messagizer = NULL;
    g_my_resources = NULL;
    g_flexbuffer = NULL;                  
    g_my_test_vf = NULL;
    g_type_map = NULL;
    g_my_vf = NULL;
    g_who = NULL;

    etchvf_free_builtins(); 

    return 0; 
}


/* - - - - - - - - - - - - - - 
 * unit tests 
 * - - - - - - - - - - - - - -
 */

/**
 * test_transportpacket_constructor()
 */
void test_transportpacket_constructor(void)
{
    my_impl_transportpacket* mytp_impl = new_my_impl_transportpacket();
    CU_ASSERT_PTR_NOT_NULL_FATAL(mytp_impl);

    /* the custom interface object destructors are coded to destroy their
     * implementing objects, so we do that here to verify this functionality.
     */
    do 
    {   i_transportpacket* itp = mytp_impl->ixp;
        itp->destroy(itp);

    } while(0);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_sessionmessage_constructor()
 */
void test_sessionmessage_constructor(void)
{
    my_impl_sessionmessage* mysm_impl = new_my_impl_sessionmessage();
    CU_ASSERT_PTR_NOT_NULL_FATAL(mysm_impl);

    /* the custom interface object destructors are coded to destroy their
     * implementing objects, so we do that here to verify this functionality.
     */
    do 
    {   i_sessionmessage* ism = mysm_impl->ism;
        ism->destroy(ism);

    } while(0);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_messagizer_constructor()
 */
void test_messagizer_constructor(void)
{
    etch_messagizer* mzr = NULL;
    i_transportpacket* itp = NULL;
    etch_resources* resxmap = NULL;
    default_value_factory* vf = NULL;
    my_impl_transportpacket* mytp_impl = NULL;

    vf = new_default_value_factory(NULL, NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(vf);
    set_etchobj_static_all(vf); /* so resources will not destroy */

    resxmap = new_etch_resources(ETCH_DEFSIZE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(resxmap);
    etch_resources_add(resxmap, ETCH_RESXKEY_MSGIZER_VALUFACT, (objmask*) vf); 

    mytp_impl = new_my_impl_transportpacket();   
    CU_ASSERT_PTR_NOT_NULL_FATAL(mytp_impl);

    itp = mytp_impl->ixp;
    CU_ASSERT_PTR_NOT_NULL_FATAL(itp);

    /* messagizer does not own i_transportpacket* itp */
    mzr = new_messagizer(itp, L"foo:?Messagizer.format=binary", resxmap);
    CU_ASSERT_PTR_NOT_NULL_FATAL(mzr);

    mzr->destroy(mzr);    

    /* i_transportpacket.destroy() will destroy my_impl_transportpacket */
    itp->destroy(itp);    

    resxmap->destroy(resxmap);
    clear_etchobj_static_all(vf);  /* so we can destroy it now */
    vf->destroy(vf);
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_testsetup_teardown_a
 */
void test_testsetup_teardown_a(void)
{
    setup_this_test(WHICHVF_TESTVF);
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_testsetup_teardown_b
 */
void test_testsetup_teardown_b(void)
{
    setup_this_test(WHICHVF_MYVF);
    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_packet_1
 * mimics the transport messagizing a packet and delivering the message to the session
 */
void test_packet_1(void)
{
    setup_this_test(WHICHVF_TESTVF);

    do          
    {   int result = 0;
        etch_type* msgtype = NULL;
        etch_flexbuffer* fbuf = NULL;
        const int THISTEST_BUFSIZE = 4;
        char* buf = etch_malloc(THISTEST_BUFSIZE, ETCHTYPEB_BYTES);

        /* g_my_sessionmessage is the i_sessionmessage interface    
         * my_impl_sessionmessage is the implementing test class */
        my_impl_sessionmessage* my_session = g_my_sessionmessage->thisx;
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session);

        /* load up the packet buffer with test data */
        buf[0] = TAGDATA_VERSION; buf[1] = FAKEID_TYPE_ADD; buf[2] = 0; buf[3] = TYPECODE_EOD_MARK;
        g_flexbuffer = new_flexbuffer_from(buf, THISTEST_BUFSIZE, THISTEST_BUFSIZE, 0);

        my_session->is_msg_handled = TRUE;

        /* messagize the packet and deliver the message to the session */
        result = g_my_messagizer->session_packet (g_my_messagizer, g_who, g_flexbuffer);
        CU_ASSERT_EQUAL(result, 0); 
        if (0 != result) break;

        CU_ASSERT_EQUAL(my_session->what, SESSION_MESSAGE); 
        result = is_equal_who(my_session->sender, g_who);
        CU_ASSERT_EQUAL(result, TRUE);
        CU_ASSERT_EQUAL(message_size(my_session->msg), 0);
        CU_ASSERT_PTR_NULL(my_session->eventx);
        /* assert that message type is "add" (since we buffered FAKEID_TYPE_ADD above) */
        msgtype = message_type(my_session->msg); 
        CU_ASSERT_PTR_NOT_NULL_FATAL(msgtype);
        CU_ASSERT_EQUAL(msgtype->id, FAKEID_TYPE_ADD);

    } while(0);

    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_packet_2
 * mimics the transport messagizing a packet and delivering the message to the session
 * which rejects the message and forwards the message as rejected.
 */
void test_packet_2(void)
{
    setup_this_test(WHICHVF_TESTVF);

    do          
    {   int result = 0;
        etch_type* msgtype = NULL;
        etch_flexbuffer* fbuf = NULL;
        const int THISTEST_BUFSIZE = 4;
        etch_message* thismessage = NULL;
        etch_unwanted_message* uwmsg = NULL;
        char* buf = etch_malloc(THISTEST_BUFSIZE, ETCHTYPEB_BYTES);

        /* g_my_sessionmessage is the i_sessionmessage interface    
         * my_impl_sessionmessage is the implementing test class */
        my_impl_sessionmessage* my_session = g_my_sessionmessage->thisx;
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session);

        /* load up the packet buffer with test data */
        buf[0] = TAGDATA_VERSION; buf[1] = FAKEID_TYPE_ADD; buf[2] = 0; buf[3] = TYPECODE_EOD_MARK;
        g_flexbuffer = new_flexbuffer_from(buf, THISTEST_BUFSIZE, THISTEST_BUFSIZE, 0);

        /* for the purposes of this test we manually specify that the message was not handled.
         * when this is the case, the message and who will be wrapped up in an "unwanted message"
         * which is forwarded to the session as an event. when this is the case, memory for the
         * message and who is not cleaned up by the session in the normal manner, but is instead
         * owned by the unwanted message, and destroyed with that object. 
         */
        my_session->is_msg_handled = FALSE;

        /* messagize the packet and deliver the message to the session */
        result = g_my_messagizer->session_packet (g_my_messagizer, g_who, g_flexbuffer);
        CU_ASSERT_EQUAL(result, 0); 
        if (0 != result) break;

        CU_ASSERT_EQUAL(my_session->what, SESSION_NOTIFY); 
        CU_ASSERT_EQUAL(message_size(my_session->msg), 0);      

        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session->eventx);

        uwmsg = (etch_unwanted_message*) my_session->eventx; 
        CU_ASSERT_EQUAL_FATAL(uwmsg->class_id, CLASSID_EVENT_UNWANTMSG); 

        /* find the message and who in the "unwanted message" wrapper */
        result = is_equal_who(uwmsg->whofrom, g_who);
        CU_ASSERT_EQUAL(result, TRUE);

        thismessage = uwmsg->message;
        CU_ASSERT_PTR_NOT_NULL_FATAL(thismessage);

        /* assert that message type is "add" (since we buffered FAKEID_TYPE_ADD above) */
        msgtype = message_type(thismessage); 
        CU_ASSERT_PTR_NOT_NULL_FATAL(msgtype);
        CU_ASSERT_EQUAL(msgtype->id, FAKEID_TYPE_ADD);

    } while(0);

    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_packet_3
 * mimics the transport messagizing a packet and delivering the message to the session
 */
void test_packet_3(void)
{
    setup_this_test(WHICHVF_TESTVF);

    do          
    {   int result = 0;
        etch_type* msgtype = NULL;
        etch_flexbuffer* fbuf = NULL;
        const int THISTEST_BUFSIZE = 4;
        etch_message* thismessage = NULL;
        etch_unwanted_message* uwmsg = NULL;
        char* buf = etch_malloc(THISTEST_BUFSIZE, ETCHTYPEB_BYTES);

        /* g_my_sessionmessage is the i_sessionmessage interface    
         * my_impl_sessionmessage is the implementing test class */
        my_impl_sessionmessage* my_session = g_my_sessionmessage->thisx;
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session);

        /* load up the packet buffer with test data */
        buf[0] = TAGDATA_VERSION; 
        buf[1] = FAKEID_TYPE_ADD_RESULT; 
        buf[2] = 0; 
        buf[3] = TYPECODE_EOD_MARK;

        g_flexbuffer = new_flexbuffer_from(buf, THISTEST_BUFSIZE, THISTEST_BUFSIZE, 0);

        /* for the purposes of this test we manually specify that the message was handled */
        my_session->is_msg_handled = TRUE;

        /* messagize the packet and deliver the message to the session */
        result = g_my_messagizer->session_packet (g_my_messagizer, g_who, g_flexbuffer);
        CU_ASSERT_EQUAL(result, 0); 
        if (0 != result) break;

        CU_ASSERT_EQUAL(my_session->what, SESSION_MESSAGE); 
        CU_ASSERT_EQUAL(message_size(my_session->msg), 0);      

        /* assert that message type is "add_result" (since we buffered FAKEID_TYPE_ADD_RESULT above) */
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session->msg);
        msgtype = message_type(my_session->msg); 
        CU_ASSERT_PTR_NOT_NULL_FATAL(msgtype);
        CU_ASSERT_EQUAL(msgtype->id, FAKEID_TYPE_ADD_RESULT);

    } while(0);

    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_message_1
 * mimics the session buffering a message and delivering it to the transport
 */
void test_message_1(void)
{
    setup_this_test(WHICHVF_TESTVF);

    do          
    {   int result = 0;
        etch_type* msgtype = NULL;
        etch_value_factory* vf = NULL;
        const int EXPECTED_BUFSIZE = 4;
        etch_message* thismessage = NULL;

        /* g_my_transportpacket is the i_transportpacket interface    
         * my_impl_transportpacket is the implementing test class */
        my_impl_transportpacket* my_transport = g_my_transportpacket->thisx;
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport);

        vf = (etch_value_factory*) get_current_valuefactory();
        msgtype = get_vftype(FAKEID_TYPE_ADD);

        g_my_transportpacket->header_size = 0; /* test no header */

        thismessage = new_message(msgtype, ETCH_DEFSIZE, vf);

        /* buffer up the message and deliver to transport */
        result = g_my_messagizer->transport_message (g_my_messagizer, g_who, thismessage);
        CU_ASSERT_EQUAL(result, 0); 
        if (0 != result) break;

        CU_ASSERT_EQUAL(my_transport->what, TRANSPORT_PACKET); 
        result = is_equal_who(my_transport->recipient, g_who);
        CU_ASSERT_EQUAL(result, TRUE);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport->buf); 
        CU_ASSERT_EQUAL(my_transport->bufcount, EXPECTED_BUFSIZE);

        /* check that packet buffer contains expected serialized message */
        CU_ASSERT_EQUAL(my_transport->buf[0], TAGDATA_VERSION); 
        CU_ASSERT_EQUAL(my_transport->buf[1], FAKEID_TYPE_ADD); 
        CU_ASSERT_EQUAL(my_transport->buf[2], 0); 
        CU_ASSERT_EQUAL(my_transport->buf[3], TYPECODE_EOD_MARK);         

    } while(0);

    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_message_2
 * mimics the session buffering a message and delivering it to the transport
 */
void test_message_2(void)
{
    setup_this_test(WHICHVF_TESTVF);

    do          
    {   int result = 0, i = 0, errs = 0;
        etch_type* msgtype = NULL;
        etch_value_factory* vf = NULL;
        etch_message* thismessage = NULL;
        const int TEST_HEADER_SIZE = 8, EXPECTED_BUFSIZE = 4 + TEST_HEADER_SIZE;

        /* g_my_transportpacket is the i_transportpacket interface    
         * my_impl_transportpacket is the implementing test class */
        my_impl_transportpacket* my_transport = g_my_transportpacket->thisx;
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport);

        vf = (etch_value_factory*) get_current_valuefactory();
        msgtype = get_vftype(FAKEID_TYPE_ADD_RESULT);

        g_my_transportpacket->header_size = TEST_HEADER_SIZE; /* test with 8-byte header */

        thismessage = new_message(msgtype, ETCH_DEFSIZE, vf);

        /* buffer up the message and deliver to transport */
        result = g_my_messagizer->transport_message(g_my_messagizer, g_who, thismessage);
        CU_ASSERT_EQUAL(result, 0); 
        if (0 != result) break;

        CU_ASSERT_EQUAL(my_transport->what, TRANSPORT_PACKET); 
        result = is_equal_who(my_transport->recipient, g_who);
        CU_ASSERT_EQUAL(result, TRUE);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport->buf); 
        CU_ASSERT_EQUAL(my_transport->bufcount, EXPECTED_BUFSIZE);

        /* check that packet buffer contains expected serialized message */
        for(; i < TEST_HEADER_SIZE; i++) if (my_transport->buf[i]) errs++;
        CU_ASSERT_EQUAL(errs, 0); 
        CU_ASSERT_EQUAL(my_transport->buf[8], TAGDATA_VERSION); 
        CU_ASSERT_EQUAL(my_transport->buf[9], FAKEID_TYPE_ADD_RESULT); 
        CU_ASSERT_EQUAL(my_transport->buf[10],0); 
        CU_ASSERT_EQUAL(my_transport->buf[11],TYPECODE_EOD_MARK);         

    } while(0);

    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_session_control
 * test the session control notification plumbing
 */
void test_session_control(void)
{
    setup_this_test(WHICHVF_TESTVF);

    do          
    {   int result = 0;
        const int MY_CONTROL_CLASSID = 0x5200, MY_VALUE_CLASSID = 0x5201;
        etch_object* mycontrolobj = new_etch_object(MY_CONTROL_CLASSID, NULL);
        etch_object* myvalueobj   = new_etch_object(MY_VALUE_CLASSID, NULL);

        /* g_my_sessionmessage is the i_sessionmessage interface    
         * my_impl_sessionmessage is the implementing test class */
        my_impl_sessionmessage* my_session = g_my_sessionmessage->thisx;
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session);
        CU_ASSERT_EQUAL(my_session->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_session->control);
        CU_ASSERT_PTR_NULL(my_session->value);

        /* we relinquish memory for mycontrolobj and myvalueobj here. 
         * the session_control terminal destination must destroy them, which here
         * is handled by our session object destructor when we teardown_this_test() */
        g_my_messagizer->session_control(g_my_messagizer, mycontrolobj, myvalueobj);

        CU_ASSERT_EQUAL(my_session->what, SESSION_CONTROL);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session->control);
        CU_ASSERT_EQUAL(my_session->control->class_id, MY_CONTROL_CLASSID);
        CU_ASSERT_EQUAL(my_session->value->class_id,   MY_VALUE_CLASSID);

    } while(0);

    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_session_notify
 * test the session notify notification plumbing
 */
void test_session_notify(void)
{
    setup_this_test(WHICHVF_TESTVF);

    do          
    {   int result = 0;
        const int MY_EVENT_CLASSID = 0x5202;
        etch_object* myeventobj = new_etch_object(MY_EVENT_CLASSID, NULL);

        /* g_my_sessionmessage is the i_sessionmessage interface    
         * my_impl_sessionmessage is the implementing test class */
        my_impl_sessionmessage* my_session = g_my_sessionmessage->thisx;
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session);
        CU_ASSERT_EQUAL(my_session->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_session->eventx);

        /* we relinquish memory for myeventobj here. 
         * the session_control terminal destination must destroy it, which here
         * is handled by our session object destructor when we teardown_this_test() */
        g_my_messagizer->session_notify(g_my_messagizer, myeventobj);

        CU_ASSERT_EQUAL(my_session->what, SESSION_NOTIFY);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session->eventx);
        CU_ASSERT_EQUAL(my_session->eventx->class_id, MY_EVENT_CLASSID);

    } while(0);

    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_session_query
 * test the session query notification plumbing
 */
void test_session_query(void)
{
    setup_this_test(WHICHVF_TESTVF);

    do          
    {   int result = 0;
        const int MY_QUERY_CLASSID = 0x5203, MY_RESULT_CLASSID = 0x5204;
        etch_object* myqueryobj  = new_etch_object(MY_QUERY_CLASSID, NULL);
        etch_object* myresultobj = new_etch_object(MY_RESULT_CLASSID, NULL);
        etch_object* queryresult = NULL;

        /* g_my_sessionmessage is the i_sessionmessage interface    
         * my_impl_sessionmessage is the implementing test class */
        my_impl_sessionmessage* my_session = g_my_sessionmessage->thisx;
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session);
        CU_ASSERT_EQUAL(my_session->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_session->query);

        /* we relinquish myresultobj here. see comments following */
        my_session->query_result = myresultobj; 

        /* we relinquish memory for myqueryobj here and assume queryresult. 
         * the session_control terminal destination must destroy it, which here
         * is handled by our session object destructor when we teardown_this_test() */
        queryresult = g_my_messagizer->session_query (g_my_messagizer, myqueryobj);

        CU_ASSERT_EQUAL(my_session->what, SESSION_QUERY);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session->query);
        CU_ASSERT_EQUAL(my_session->query->class_id, MY_QUERY_CLASSID);
        CU_ASSERT_PTR_NOT_NULL_FATAL(queryresult);
        CU_ASSERT_EQUAL(queryresult->class_id, MY_RESULT_CLASSID);
        queryresult->destroy(queryresult);

    } while(0);

    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_transport_control
 * test the transport control notification plumbing
 */
void test_transport_control(void)
{
    setup_this_test(WHICHVF_TESTVF);

    do          
    {   int result = 0;
        my_impl_transportpacket* my_transport = NULL;
        const int MY_CONTROL_CLASSID = 0x5200, MY_VALUE_CLASSID = 0x5201;
        etch_object* mycontrolobj = new_etch_object(MY_CONTROL_CLASSID, NULL);
        etch_object* myvalueobj   = new_etch_object(MY_VALUE_CLASSID, NULL);

        /* g_my_transportpacket    is the i_transportpacket interface    
         * my_impl_transportpacket is the implementing test class 
         */
        g_my_messagizer->transport = g_my_transportpacket;

        my_transport = g_my_transportpacket->thisx;
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport);
        CU_ASSERT_EQUAL(my_transport->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_transport->control);
        CU_ASSERT_PTR_NULL(my_transport->value);

        /* we relinquish memory for mycontrolobj and myvalueobj here. 
         * the transport_control terminal destination must destroy them, which here
         * is handled by our transport object destructor when we teardown_this_test() 
         */
        g_my_messagizer->transport_control (g_my_messagizer, mycontrolobj, myvalueobj);

        CU_ASSERT_EQUAL(my_transport->what, TRANSPORT_CONTROL);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport->control);
        CU_ASSERT_EQUAL(my_transport->control->class_id, MY_CONTROL_CLASSID);
        CU_ASSERT_EQUAL(my_transport->value->class_id,   MY_VALUE_CLASSID);

    } while(0);

    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_transport_notify
 * test the transport notify notification plumbing
 */
void test_transport_notify(void)
{
    setup_this_test(WHICHVF_TESTVF);

    do          
    {   int result = 0;
        my_impl_transportpacket* my_transport = NULL;
        const int MY_EVENT_CLASSID = 0x5202;
        etch_object* myeventobj = new_etch_object(MY_EVENT_CLASSID, NULL);

        /* g_my_transportpacket    is the i_transportpacket interface    
         * my_impl_transportpacket is the implementing test class 
         */
        g_my_messagizer->transport = g_my_transportpacket;

        my_transport = g_my_transportpacket->thisx;

        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport);
        CU_ASSERT_EQUAL(my_transport->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_transport->control);
        CU_ASSERT_PTR_NULL(my_transport->value);

        /* we relinquish memory for myeventobj here. 
         * the transport_control terminal destination must destroy it, which here
         * is handled by our transport object destructor when we teardown_this_test() */
        g_my_messagizer->transport_notify (g_my_messagizer, myeventobj);

        CU_ASSERT_EQUAL(my_transport->what, TRANSPORT_NOTIFY);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport->eventx);
        CU_ASSERT_EQUAL(my_transport->eventx->class_id, MY_EVENT_CLASSID);

    } while(0);

    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_transport_query
 * test the transport query notification plumbing
 */
void test_transport_query(void)
{
    setup_this_test(WHICHVF_TESTVF);

    do          
    {   int result = 0;
        my_impl_transportpacket* my_transport = NULL;
        const int MY_QUERY_CLASSID = 0x5203, MY_RESULT_CLASSID = 0x5204;
        etch_object* myqueryobj  = new_etch_object(MY_QUERY_CLASSID, NULL);
        etch_object* myresultobj = new_etch_object(MY_RESULT_CLASSID, NULL);
        etch_object* queryresult = NULL;

        /* g_my_transportpacket    is the i_transportpacket interface    
         * my_impl_transportpacket is the implementing test class 
         */
        g_my_messagizer->transport = g_my_transportpacket;

        my_transport = g_my_transportpacket->thisx;

        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport);
        CU_ASSERT_EQUAL(my_transport->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_transport->control);
        CU_ASSERT_PTR_NULL(my_transport->value);

        /* we relinquish myresultobj here. see comments following */
        my_transport->query_result = myresultobj; 

        /* we relinquish memory for myqueryobj here and assume queryresult. 
         * the transport_control terminal destination must destroy it, which here
         * is handled by our transport object destructor when we teardown_this_test() */
        queryresult = g_my_messagizer->transport_query (g_my_messagizer, myqueryobj);

        CU_ASSERT_EQUAL(my_transport->what, TRANSPORT_QUERY);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport->query);
        CU_ASSERT_EQUAL(my_transport->query->class_id, MY_QUERY_CLASSID);
        CU_ASSERT_PTR_NOT_NULL_FATAL(queryresult);
        CU_ASSERT_EQUAL(queryresult->class_id, MY_RESULT_CLASSID);
        queryresult->destroy(queryresult);

    } while(0);

    teardown_this_test();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * main   
 */
int _tmain(int argc, _TCHAR* argv[])
{    
    char c=0;
    CU_pSuite pSuite = NULL;
    g_is_automated_test = argc > 1 && 0 != wcscmp(argv[1], L"-a");
    if (CUE_SUCCESS != CU_initialize_registry()) return 0;
    pSuite = CU_add_suite("suite messagizer", init_suite, clean_suite);
    CU_set_output_filename("../test_messagizer");
    etch_watch_id = 0; 

    CU_add_test(pSuite, "test transportpacket impl constructor", test_transportpacket_constructor); 
    CU_add_test(pSuite, "test sessionmessage impl constructor",  test_sessionmessage_constructor); 
    CU_add_test(pSuite, "test messagizer constructor",  test_messagizer_constructor); 
    CU_add_test(pSuite, "test test setup and teardown v1",  test_testsetup_teardown_a); 
    CU_add_test(pSuite, "test test setup and teardown v2",  test_testsetup_teardown_b); 

    CU_add_test(pSuite, "test packet 1",  test_packet_1); 
    CU_add_test(pSuite, "test packet 2",  test_packet_2); 
    CU_add_test(pSuite, "test packet 3",  test_packet_2); 
    CU_add_test(pSuite, "test message 1", test_message_1); 
    CU_add_test(pSuite, "test message 2", test_message_2); 

    CU_add_test(pSuite, "test session control",test_session_control); 
    CU_add_test(pSuite, "test session notify", test_session_notify); 
    CU_add_test(pSuite, "test session query",  test_session_query); 
    CU_add_test(pSuite, "test transport control",test_transport_control); 
    CU_add_test(pSuite, "test transport notify", test_transport_notify); 
    CU_add_test(pSuite, "test transport query",  test_transport_query); 
     
    if (g_is_automated_test)    
        CU_automated_run_tests();    
    else
    {   CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    }

    if (!g_is_automated_test) { printf("any key ..."); while(!c) c = _getch(); printf("\n"); }     
    CU_cleanup_registry();
    return CU_get_error(); 
}