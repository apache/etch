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
 * test_packetizer.c
 */
#include "etch_runtime.h"
#include "etch_packetizer.h"
#include "etch_connection.h"
#include "etch_nativearray.h"
#include "etch_encoding.h"
#include "etch_thread.h"
#include "etch_objecttypes.h"
#include "etch_general.h"
#include "etch_log.h"
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
    etch_runtime_shutdown();
    return 0;
}

/* - - - - - - - - - - - - - - 
 * unit test support
 * - - - - - - - - - - - - - -
 */

static etch_packetizer*        g_my_packetizer;
static i_sessionpacket*        g_my_sessionpacket;
static i_transportdata*        g_my_transportdata; 
static etch_flexbuffer*        g_mybuf;
static etch_who*               g_who;

#define THISTEST_WHO_VALUE 0x5151
static unsigned short CLASSID_MY_IMPL_SP;
static unsigned short CLASSID_MY_IMPL_TD;

#define is_my_impl_transportdata(x) \
 (x && ((etch_object*)x)->obj_type == ETCHTYPEB_TRANSPORTDATA && ((etch_object*)x)->class_id == CLASSID_MY_IMPL_TD)

#define is_my_impl_sessionpkt(x) \
 (x && ((etch_object*)x)->obj_type == ETCHTYPEB_SESSIONPKT&& ((etch_object*)x)->class_id == CLASSID_MY_IMPL_SP)

static int check_packetizer_results(i_transportdata*, etch_nativearray*);
static int check_packetizer_resultx(i_sessionpacket*, etch_nativearray*);
static int check_packetized_results (etch_arraylist*, etch_nativearray*);
static int check_packetized_result  (byte* v1, size_t c1, byte* v2, size_t c2);
static etch_arraylist* new_packetizertest_list();


typedef enum etch_what
{ WHAT_NONE, WHAT_DATA, WHAT_PACKET, 
  TRANSPORT_QUERY, TRANSPORT_CONTROL, TRANSPORT_NOTIFY,
  SESSION_QUERY,   SESSION_CONTROL,   SESSION_NOTIFY
} etch_what;


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


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 * my_impl_transportdata (i_transportdata implementation) 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 */

/**
 * my_impl_transportdata
 * test object implementing i_transportdata
 */
typedef struct my_impl_transportdata
{
    etch_object object;

    /* i_transportdata interface and methods, plus original destructor
     * which becomes replaced with a custom destructor to destroy this
     * object. this is the model for destroying an interface wrapper object
     * when we do not save and pass around a pointer to the wrapper, but rather
     * a pointer to the interface. the interface in question, i_transportdata
     * in this case, contains a pointer to the wrapper object, in this case a
     * my_impl_transportdata*. when the interface is instantiated, its original 
     * destructor is saved, and is replaced with a destructor which invokes
     * the wrapper's destructor. the wrapper destructor must then know to 
     * invoke the interface's original destructor when destroying the interface.
     */
    i_transportdata* itd;         /* owned */ 
    etch_object_destructor destroy_transportdata;     /* i_transportdata original destructor */
    etch_transport_data transport_data;  /* i_transportdata::transport_data() */

    i_sessiondata*  session;      /* not owned */

    etch_what       what;          
    etch_who*       recipient;    /* not owned */
    etch_arraylist* list;         /* owned */    
    etch_object*    query;        /* owned */
    etch_object*    query_result; /* owned */
    etch_object*    control;      /* owned */
    etch_object*    value;        /* owned */
    etch_object*    eventx;       /* owned */

} my_impl_transportdata;


/**
 * destroy_my_impl_transportdata()
 * my_impl_transportdata destructor
 */
static int destroy_my_impl_transportdata (void* data)
{

  my_impl_transportdata* thisx = (my_impl_transportdata*)data;
    if (!is_etchobj_static_content(thisx))
    {       /* invoke original i_transportdata destructor */
        if (thisx->itd && thisx->destroy_transportdata)
            thisx->destroy_transportdata(thisx->itd);

       etch_object_destroy(thisx->list);

        etch_object_destroy(thisx->query);

        etch_object_destroy(thisx->query_result);

        etch_object_destroy(thisx->control);

        etch_object_destroy(thisx->value);

        etch_object_destroy(thisx->eventx);
    }

   return destroy_objectex((etch_object*) thisx);
}



/**
 * impl_transport_data()
 * my_impl_transportdata::transport_data
 * @param whoto caller retains, can be null
 * @param fbuf caller retains
 */
static int impl_transport_data (void* data, void* who, void* buffer)
{
    my_impl_transportdata* mytd = (my_impl_transportdata*)data;
    etch_who* whoto = (etch_who*)who;
    etch_flexbuffer* fbuf = (etch_flexbuffer*)buffer;

    etch_nativearray* wrapped_array = NULL;
    byte* newbuf = NULL;
    size_t bytecount = 0;

    assert(is_my_impl_transportdata(mytd));   
    assert(is_etch_transportdata(mytd->itd));     

    mytd->what = WHAT_DATA;
    mytd->recipient = whoto;   
   
    /* get a new byte vector of the entire packet from the buffer */
    newbuf = etch_flexbuf_get_all (fbuf, &bytecount);

    /* wrap the packet bytes in an etch_nativearray object */
    wrapped_array = new_etch_nativearray_from (newbuf,
        CLASSID_ARRAY_BYTE, sizeof(byte), 1, (int) bytecount, 0, 0); 

    /* we want wrapped array to own new packet bytes memory */
    wrapped_array->is_content_owned = TRUE;

    return etch_arraylist_add (mytd->list, wrapped_array);
}


/**
 * my_transport_control()
 * my_impl_transportdata::itransport::transport_control 
 */
static int my_transport_control (my_impl_transportdata* mytd, etch_object* control, etch_object* value)
{
    /* changed parameter from i_transportdata* to my_impl_transportdata to correspond to change  
     * in etch_pktizer_transport_control to pass pzr->transport->thisx rather than pzr->transport.
     * likewise for my_transport_notify() and my_transport_query. */
    CU_ASSERT_FATAL(is_my_impl_transportdata(mytd));
    mytd->what    = TRANSPORT_CONTROL;
    mytd->control = control;
    mytd->value   = value;
    return 0;
}


/**
 * my_transport_notify()
 * my_impl_transportdata::itransport::transport_notify 
 */
static int my_transport_notify (my_impl_transportdata* mytd, etch_object* evt)
{
    CU_ASSERT_FATAL(is_my_impl_transportdata(mytd));
    mytd->what   = TRANSPORT_NOTIFY;
    mytd->eventx = evt;
    return 0;
}


/**
 * my_transport_query()
 * my_impl_transportdata::itransport::transport_query 
 */
static etch_object* my_transport_query (my_impl_transportdata* mytd, etch_object* query) 
{
    etch_object* resultobj = NULL;
    CU_ASSERT_FATAL(is_my_impl_transportdata(mytd));
    resultobj   = mytd->query_result; /* set artificially in test */
    mytd->what  = TRANSPORT_QUERY;
    mytd->query = query;
    mytd->query_result = NULL;
    return (etch_object*) resultobj;  /* caller owns */
}


/**
 * my_transport_get_session()
 * my_impl_transportdata::itransport::get_session 
 */
static i_sessiondata* my_transport_get_session (my_impl_transportdata* mytd)
{
    ETCH_ASSERT(is_etch_transportdata(mytd));
    return mytd->session;
}


/**
 * my_transport_set_session()
 * my_impl_transportdata::itransport::set_session
 */
static void my_transport_set_session (my_impl_transportdata* mytd, i_sessiondata* newsession)
{   
    ETCH_ASSERT(is_etch_transportdata(mytd));
    ETCH_ASSERT(is_etch_sessiondata(newsession));
    mytd->session = newsession;
}


/*
 * destroy_my_transportdata()
 * i_transportdata destructor
 * this destructor will destroy its parent (my_impl_transportdata), 
 * which will in turn destroy this object.
 */
static int destroy_my_transportdata(void* data)
{
    i_transportdata* itd = (i_transportdata*)data;
    my_impl_transportdata* mytd = NULL;
    if (NULL == itd) return -1;

    mytd = itd->thisx;  

    etch_object_destroy(mytd);

    return 0;
}


/**
 * new_my_impl_transportdata()
 * my_impl_transportdata constructor
 */
static my_impl_transportdata* new_my_impl_transportdata()
{
    i_transportdata* itd  = NULL;
    i_transport* itransport = NULL;
    /* this is a model for dynamic class ID assigment */
    unsigned short class_id = CLASSID_MY_IMPL_TD? CLASSID_MY_IMPL_TD: (CLASSID_MY_IMPL_TD = get_dynamic_classid());

    my_impl_transportdata* mytd = (my_impl_transportdata*) new_object
      (sizeof(my_impl_transportdata), ETCHTYPEB_TRANSPORTDATA, class_id);

    ((etch_object*)mytd)->destroy = destroy_my_impl_transportdata;

    itransport = new_transport_interface_ex (mytd,
        (etch_transport_control)     my_transport_control, 
        (etch_transport_notify)      my_transport_notify, 
        (etch_transport_query)       my_transport_query,
        (etch_transport_get_session) my_transport_get_session, 
        (etch_transport_set_session) my_transport_set_session);

    itd = new_transportdata_interface(mytd, impl_transport_data, itransport);

    /* save off i_transportdata destructor */
    mytd->destroy_transportdata = ((etch_object*)itd)->destroy;   

    /* replace i_transportdata destructor with one which will destroy this object */
    ((etch_object*)itd)->destroy = destroy_my_transportdata;

    mytd->list = new_packetizertest_list();

    /* g_my_transportdata will get set to this interface */
    mytd->itd = itd;  

    return mytd;
}


/* - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 * my_impl_sessionpacket (i_sessionpacket implementation) 
 * - - - - - - - - - - - - - - - - - - - - - - - - - - - - - - 
 */

/**
 * my_impl_sessionpacket
 * test object implementing i_sessionpacket
 */
typedef struct my_impl_sessionpacket
{
    etch_object object;

    /* i_sessionpacket interface and methods, plus original destructor
     * which becomes replaced with a custom destructor to destroy this
     * object. this is the model for destroying an interface wrapper object
     * when we do not save and pass around a pointer to the wrapper, but rather
     * a pointer to the interface. the interface in question, i_sessionpacket
     * in this case, contains a pointer to the wrapper object, in this case a
     * my_impl_sessionpacket*. when the interface is instantiated, its original 
     * destructor is saved, and is replaced with a destructor which invokes
     * the wrapper's destructor. the wrapper destructor must then know to 
     * invoke the interface's original destructor when destroying the interface.
     */
    i_sessionpacket* isp;          /* owned */

    etch_object_destructor destroy_sessionpacket; /* i_sessionpacket original destructor */
    etch_session_packet session_packet;  /* session_packet() */

    etch_what       what;          
    etch_who*       sender;        /* not owned */ 
    etch_arraylist* list;          /* owned */    
    etch_object*    query;         /* owned */
    etch_object*    query_result;  /* owned */
    etch_object*    control;       /* owned */
    etch_object*    value;         /* owned */
    etch_object*    eventx;        /* owned */

} my_impl_sessionpacket;


/**
 * destroy_my_impl_sessionpacket()
 * my_impl_sessionpacket destructor
 */
static int destroy_my_impl_sessionpacket(void* data)
{

    my_impl_sessionpacket* thisx = (my_impl_sessionpacket*)data;
    if (!is_etchobj_static_content(thisx))
    {       /* invoke original i_sessionpacket destructor */
        if (thisx->isp && thisx->destroy_sessionpacket)   
            thisx->destroy_sessionpacket(thisx->isp);
  
        etch_object_destroy(thisx->list);

        etch_object_destroy(thisx->query);

        etch_object_destroy(thisx->query_result);

        etch_object_destroy(thisx->control);

        etch_object_destroy(thisx->value);

        etch_object_destroy(thisx->eventx);
    }

   return destroy_objectex((etch_object*) thisx);
}


/**
 * impl_session_packet()
 * my_impl_sessionpacket::session_packet
 * @param whofrom caller retains, can be null
 * @param fbuf caller retains
 */
static int impl_session_packet (void* data, void* who, void* buffer)
{
    my_impl_sessionpacket* mysp = (my_impl_sessionpacket*)data;
    etch_who* whofrom = (etch_who*)who;
    etch_flexbuffer* fbuf = (etch_flexbuffer*)buffer;

    etch_nativearray* wrapped_array = NULL;
    byte* newbuf = NULL;
    size_t bytecount = 0;
    CU_ASSERT_FATAL(is_my_impl_sessionpkt(mysp));
    mysp->what   = WHAT_PACKET;
    mysp->sender = whofrom;   
   
    /* get a new byte vector of the entire packet from the buffer */
    newbuf = etch_flexbuf_get_all(fbuf, &bytecount);

    /* wrap the packet bytes in an etch_nativearray object */
    wrapped_array = new_etch_nativearray_from(newbuf,
        CLASSID_ARRAY_BYTE, sizeof(byte), 1, (int) bytecount, 0, 0); 

    /* we want wrapped array to own new packet bytes memory */
    wrapped_array->is_content_owned = TRUE;

    return etch_arraylist_add(mysp->list, wrapped_array);
}


/**
 * my_session_control()
 * my_impl_sessionpacket::isession::session_control 
 */
static int my_session_control (my_impl_sessionpacket* mysp, etch_object* control, etch_object* value)
{
    CU_ASSERT_FATAL(is_my_impl_sessionpkt(mysp));
    mysp->what    = SESSION_CONTROL;
    mysp->control = control;
    mysp->value   = value;
    return 0;
}


/**
 * my_session_notify()
 * my_impl_sessionpacket::isession::session_notify 
 */
static int my_session_notify (my_impl_sessionpacket* mysp, etch_object* evt)
{
    CU_ASSERT_FATAL(is_my_impl_sessionpkt(mysp));
    mysp->what   = SESSION_NOTIFY;
    mysp->eventx = evt;
    return 0;
}


/**
 * my_session_query()
 * my_impl_sessionpacket::isession::session_query 
 */
static etch_object* my_session_query (my_impl_sessionpacket* mysp, etch_object* query) 
{
    etch_object* resultobj = NULL;
    CU_ASSERT_FATAL(is_my_impl_sessionpkt(mysp));
    resultobj = mysp->query_result; /* set artificially in test */
    mysp->what  = SESSION_QUERY;
    mysp->query = query;
    mysp->query_result = NULL;
    return (etch_object*) resultobj;  /* caller owns */
}


/**
 * new_packetizertest_list()
 * constructor for a list which owns content memory and where content is etch_object derived.
 * content will be etch_nativearray objects
 */
static etch_arraylist* new_packetizertest_list()
{
    etch_arraylist* list = new_etch_arraylist(32,0);
    list->content_type = ETCHARRAYLIST_CONTENT_OBJECT; /* list can call destroy() on list item */
    list->is_readonly  = FALSE;
    return list;
}


/*
 * destroy_my_sessionpacket()
 * i_sessionpacket destructor
 * this destructor will destroy its parent (my_impl_sessionpacket), 
 * which will in turn destroy this object.
 */
static int destroy_my_sessionpacket(void* data)
{
    i_sessionpacket* itp = (i_sessionpacket*)data;
    my_impl_sessionpacket* mytp = NULL;
    if (NULL == itp) return -1;

    mytp = itp->thisx;  

    etch_object_destroy(mytp);

    return 0;
}


/**
 * new_my_impl_sessionpacket()
 * my_impl_sessionpacket constructor
 */
static my_impl_sessionpacket* new_my_impl_sessionpacket()
{
    i_sessionpacket* isessionpkt  = NULL;
    i_session* isession = NULL;
    /* this is a model for dynamic class ID assigment */
    unsigned short class_id = CLASSID_MY_IMPL_SP? CLASSID_MY_IMPL_SP: 
        (CLASSID_MY_IMPL_SP = get_dynamic_classid());

    my_impl_sessionpacket* mysessionpkt = (my_impl_sessionpacket*) new_object
      (sizeof(my_impl_sessionpacket), ETCHTYPEB_SESSIONPKT, class_id);

    ((etch_object*)mysessionpkt)->destroy = destroy_my_impl_sessionpacket;

    mysessionpkt->list = new_packetizertest_list();

    isession = new_session_interface(mysessionpkt,
        (etch_session_control) my_session_control, 
        (etch_session_notify)  my_session_notify, 
        (etch_session_query)   my_session_query);

    isessionpkt = new_sessionpkt_interface(mysessionpkt, impl_session_packet, isession);

    /* save off i_sessionpacket destructor */
    mysessionpkt->destroy_sessionpacket = ((etch_object*)isessionpkt)->destroy;

    /* replace i_sessionpacket destructor with one which will destroy this object */
    ((etch_object*)isessionpkt)->destroy = destroy_my_sessionpacket;

    /* g_my_sessionpacket will get set to this interface */
    mysessionpkt->isp = isessionpkt;  

    return mysessionpkt;
}


/* - - - - - - - - - - - - - - - - - - - - - - 
 * support functions
 * - - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * check_packetizer_results()
 * check array of packetizer result byte arrays against a control byte array. 
 * arrays may be packet payloads, or packets with headers, depending on test. 
 * result arrays are found in the packet handler arraylist, each such list item
 * being an etch_nativearray object of one dimension, wrapping that result array. 
 * control arrays are passed as an etch_nativearray of two dimensions, created 
 * from a static 2-dimensional array.
 */
static int check_packetizer_results(i_transportdata* itd, etch_nativearray* expected_array)
{   
    my_impl_transportdata*  mytd = (my_impl_transportdata*) itd->thisx;
    const int packetcount = mytd->list->count; /* # of result arrays in list */
    int errors = 0;

    if (NULL == expected_array) /* todo: also check for zero populated count */
        return packetcount == 0? 0: -1;

    if (packetcount != expected_array->dimension[1]) 
        return -1; /* check against count of expected results */

    errors = check_packetized_results(mytd->list, expected_array);
     
    return errors? -1: 0;
}


/**
 * check_packetizer_resultx()
 * check array of packetizer result byte arrays against a control byte array. 
 * arrays may be packet payloads, or packets with headers, depending on test. 
 * result arrays are found in the packet handler arraylist, each such list item
 * being an etch_nativearray object of one dimension, wrapping that result array. 
 * control arrays are passed as an etch_nativearray of two dimensions, created 
 * from a static 2-dimensional array.
 */
static int check_packetizer_resultx(i_sessionpacket* isp, etch_nativearray* expected_array)
{   
    my_impl_sessionpacket*  mysp = (my_impl_sessionpacket*) isp->thisx;
    const int packetcount = mysp->list->count; /* # of result arrays in list */
    int errors = 0;

    if (NULL == expected_array) /* todo: also check for zero populated count */
        return packetcount == 0? 0: -1;

    if (packetcount != expected_array->dimension[1]) 
        return -1; /* check against count of expected results */

    errors = check_packetized_results(mysp->list, expected_array);
     
    return errors? -1: 0;
}


/**
 * check_packetized_results()
 */
static int check_packetized_results(etch_arraylist* list, etch_nativearray* expected_array)
{
    etch_iterator iterator;
    int result = 0, errors = 0, i = 0;
    set_iterator(&iterator, list, &list->iterable);

    while(iterator.has_next(&iterator))
    {   
        /* get result (one-dimension byte array) out of list */
        etch_nativearray* resultobj = etch_arraylist_get(list, i);
        byte*  packetizedbytes = resultobj->values;   
        size_t packetizedbytecount = resultobj->dimension[0];

        /* get (offset to) this result out of expected results */
        size_t dim2offset = etch_nativearray_off2(expected_array, i, 0);
        byte*  expected_bytes = (byte*) expected_array->values + dim2offset;  
        size_t expected_bytecount = expected_array->dimension[0];

        result = check_packetized_result(packetizedbytes, packetizedbytecount, 
            expected_bytes, expected_bytecount);

        if (result == -1)
            errors++;

        i++;
        iterator.next(&iterator);
    }
     
    return errors;
}


/**
 * check_packetized_result()
 * check a single packetizer result array against a control array, 
 * each represented as a byte vector. the array can be a packet, or a packet payload,
 * depending on tested direction of the packetizer call.
 */
static int check_packetized_result (byte* v1, size_t c1, byte* v2, size_t c2)
{   
    byte  *p = 0, *q = 0;
    size_t i = 0, errors = 0;

    if (c1 != c2)   return FALSE;
    if (!v1 && !v2) return TRUE;
    if (!v1 || !v2) return FALSE;

    for(p = v1, q = v2; i < c1; i++, p++, q++)
        if (*p != *q) 
            errors++;

    return errors? -1: 0;
}
  

/* - - - - - - - - - - - - - - - - - - - - - - 
 * individual setup and teardown
 * - - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * setup_this_test()
 */
static int setup_this_test()
{
    my_impl_transportdata* mytd_impl = NULL;
    my_impl_sessionpacket* mysp_impl = NULL;

    /* we instantiate a wrapper x which implements and instantiates i_transportpacket.
     * the instantiation of i_transportpacket will contain a pointer to x.
     * our global reference g_my_transportpacket is a pointer to the interface.
     * the purpose of this excercise is that, in the real binding we can pass
     * around the interface, whose methods can be then invoked without knowing
     * anything about the wrapper. when we want to reference the wrapper x, 
     * it is (my_impl_transportpacket) g_my_transportpacket->thisx. 
     */
    mytd_impl = new_my_impl_transportdata();
    g_my_transportdata = mytd_impl->itd;
    CU_ASSERT_FATAL(is_my_impl_transportdata(g_my_transportdata->thisx));

    /* we instantiate a wrapper y which implements and instantiates i_sessionpacket.
     * the instantiation of i_sessionpacket will contain a pointer to y.
     * our global reference g_my_sessionpacket is a pointer to the interface.
     * the purpose of this excercise is that, in the real binding we can pass
     * around the interface, whose methods can be then invoked without knowing
     * anything about the wrapper. when we want to reference the wrapper x, 
     * it is (my_impl_sessionpacket*) g_my_sessionpacket->thisx. 
     */
    mysp_impl = new_my_impl_sessionpacket();
    g_my_sessionpacket = mysp_impl->isp;
    
    g_who = new_who(new_int32(THISTEST_WHO_VALUE));

    /* finally instantiate the test packetizer */
    g_my_packetizer = new_packetizer(g_my_transportdata, L"tcp", NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(g_my_packetizer);
    g_my_packetizer->set_session(g_my_packetizer, g_my_sessionpacket);
  
    return 0; 
}


/**
 * teardown_this_test()
 */
static int teardown_this_test()
{
    etch_object_destroy(g_my_packetizer);

    etch_object_destroy(g_my_sessionpacket);  // destroy() is now 0xfeeefeee

    etch_object_destroy(g_my_transportdata);

    etch_object_destroy(g_who);

    etch_object_destroy(g_mybuf);

    g_my_packetizer = NULL;
    g_my_sessionpacket = NULL; 
    g_my_transportdata = NULL;
    g_mybuf = NULL;
    g_who = NULL;

    return 0; 
}


/* - - - - - - - - - - - - - - - - - - - - - - 
 * tests
 * - - - - - - - - - - - - - - - - - - - - - -
 */

/**
 * test_sessionpacket_constructor()
 */
static void test_sessionpacket_constructor(void)
{
    my_impl_sessionpacket* mysp_impl = new_my_impl_sessionpacket();
    CU_ASSERT_PTR_NOT_NULL_FATAL(mysp_impl);

    /* the custom interface object destructors are coded to destroy their
     * implementing objects, so we do that here to verify this functionality.
     */
    do 
    {   i_sessionpacket* isp = mysp_impl->isp;
        etch_object_destroy(isp);

    } while(0);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_transportdata_constructor()
 */
static void test_transportdata_constructor(void)
{
    my_impl_transportdata* mytd_impl = new_my_impl_transportdata();
    CU_ASSERT_PTR_NOT_NULL_FATAL(mytd_impl);

    /* the custom interface object destructors are coded to destroy their
     * implementing objects, so we do that here to verify this functionality.
     */
    do 
    {   i_transportdata* itd = mytd_impl->itd;
        etch_object_destroy(itd);

    } while(0);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_packetizer_constructor()
 */
static void test_packetizer_constructor(void)
{
    etch_packetizer* pzr = NULL;
    i_transportdata* itd = NULL;
    my_impl_transportdata* mytd_impl = NULL;

    mytd_impl = new_my_impl_transportdata();   
    CU_ASSERT_PTR_NOT_NULL_FATAL(mytd_impl);

    itd = mytd_impl->itd;
    CU_ASSERT_PTR_NOT_NULL_FATAL(itd);

    /* packetizer does not own i_transportdata* itd */
	pzr = new_packetizer(itd, L"tcp://127.0.0.1:4001?Packetizer.maxPktSize=110480", NULL);
	CU_ASSERT_TRUE(pzr->maxpacketsize == 110480);

    CU_ASSERT_PTR_NOT_NULL_FATAL(pzr);

    etch_object_destroy(pzr);

    /* i_transportdata.destroy() will destroy my_impl_transportdata */
    etch_object_destroy(itd);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_test_setup()
 * test that the test setup works with all memory accounted for.
 * if this test fails, all subsequent tests would also fail.
 * all subsequent tests should include this skeleton code.
 */
static void test_test_setup(void)
{
    int result = setup_this_test();

    CU_ASSERT_EQUAL_FATAL(result, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(g_who);
    CU_ASSERT_PTR_NOT_NULL_FATAL(g_my_packetizer);
    CU_ASSERT_PTR_NOT_NULL_FATAL(g_my_sessionpacket);
    CU_ASSERT_PTR_NOT_NULL_FATAL(g_my_transportdata);

    teardown_this_test();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_to_source_packet1()
 * test 1 for packets delivered to packet source
 */
static void test_to_source_packet1(void)
{
    setup_this_test();

    do          
    {
        int result = 0;
        byte* disposable_packet = NULL;
        #define EXPECTED_HEADERSIZE 8
        etch_nativearray* expected_result_array = NULL;
        my_impl_transportdata* mytransport = g_my_transportdata->thisx; 

        byte empty_packet[EXPECTED_HEADERSIZE] = { 0,0,0,0, 0,0,0,0, };

        byte expected_result[][EXPECTED_HEADERSIZE] = 
        {
            {
                -34,-83,-66,-17,  0,0,0,0,
            },
        };

        CU_ASSERT_EQUAL_FATAL(g_my_packetizer->headersize, EXPECTED_HEADERSIZE);

        expected_result_array = new_etch_nativearray_from
          (&expected_result, CLASSID_ARRAY_BYTE, sizeof(byte), 2, EXPECTED_HEADERSIZE, 1, 0);
      
        disposable_packet = etch_malloc(sizeof(empty_packet), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, empty_packet, sizeof(empty_packet));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(empty_packet), 8, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);

        result = g_my_packetizer->transport_packet(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result, 0);
         
        result = check_packetizer_results(g_my_packetizer->transport, expected_result_array);
        CU_ASSERT_EQUAL(result, 0);

        CU_ASSERT_EQUAL(mytransport->what, WHAT_DATA);
        result = is_equal_who(mytransport->recipient, g_who);
        CU_ASSERT_EQUAL(result, TRUE);

        etch_object_destroy(expected_result_array);

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
 * test_to_source_packet2()
 * test 2 for packets delivered to packet source
 */
static void test_to_source_packet2(void)
{
   setup_this_test();

    do          
    {   int result = 0;
        #define TTSP2BYTECOUNT 9
        byte* disposable_packet = NULL;
        etch_nativearray* expected_result_array = NULL;
        my_impl_transportdata* mytransport = g_my_transportdata->thisx; 

        byte test_packet[TTSP2BYTECOUNT] = { 0,0,0,0, 0,0,0,0, 1 };

        byte expected_result[][TTSP2BYTECOUNT] = 
        {     /* --signature---  data-length  data  */
            { /* de  ad  be  ef  00 00 00 01  01    */
                -34,-83,-66,-17,  0, 0, 0, 1,  1
            },
        };

        CU_ASSERT_EQUAL_FATAL(g_my_packetizer->headersize, EXPECTED_HEADERSIZE);

        expected_result_array = new_etch_nativearray_from
          (&expected_result, CLASSID_ARRAY_BYTE, sizeof(byte), 2, TTSP2BYTECOUNT, 1, 0);
      
        disposable_packet = etch_malloc(sizeof(test_packet), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, test_packet, sizeof(test_packet));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(test_packet), 9, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);

        result = g_my_packetizer->transport_packet(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result, 0);
         
        result = check_packetizer_results(g_my_packetizer->transport, expected_result_array);
        CU_ASSERT_EQUAL(result, 0);

        CU_ASSERT_EQUAL(mytransport->what, WHAT_DATA);
        result = is_equal_who(mytransport->recipient, g_who);
        CU_ASSERT_EQUAL(result, TRUE);

        etch_object_destroy(expected_result_array);

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
 * test_to_source_packet3()
 * test 3 for packets delivered to packet source
 */
static void test_to_source_packet3(void)
{
    setup_this_test();

    do          
    {   int result = 0;
        #define TTSP3BYTECOUNT 10
        byte* disposable_packet = NULL;
        etch_nativearray* expected_result_array = NULL;
        my_impl_transportdata* mytransport = g_my_transportdata->thisx; 

        byte test_packet[TTSP3BYTECOUNT] = { 0,0,0,0, 0,0,0,0, 2, 3 };

        byte expected_result[][TTSP3BYTECOUNT] = 
        {     /* --signature---  data-length  data  */
            { /* de  ad  be  ef  00 00 00 02  02 03 */
                -34,-83,-66,-17,  0, 0, 0, 2,  2, 3
            },
        };

        CU_ASSERT_EQUAL_FATAL(g_my_packetizer->headersize, EXPECTED_HEADERSIZE);

        expected_result_array = new_etch_nativearray_from
          (&expected_result, CLASSID_ARRAY_BYTE, sizeof(byte), 2, TTSP3BYTECOUNT, 1, 0);
      
        disposable_packet = etch_malloc(sizeof(test_packet), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, test_packet, sizeof(test_packet));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(test_packet), 10, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);

        result = g_my_packetizer->transport_packet(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result, 0);
         
        result = check_packetizer_results(g_my_packetizer->transport, expected_result_array);
        CU_ASSERT_EQUAL(result, 0);

        CU_ASSERT_EQUAL(mytransport->what, WHAT_DATA);
        result = is_equal_who(mytransport->recipient, g_who);
        CU_ASSERT_EQUAL(result, TRUE);

        etch_object_destroy(expected_result_array);

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
 * test_to_source_badpacket1()
 * test 4 for packets delivered to packet source
 */
static void test_to_source_badpacket1(void)
{
    setup_this_test();

    do          
    {   int result = 0;
        #define TTSP4BYTECOUNT 7
        byte* disposable_packet = NULL;

        byte test_packet[TTSP4BYTECOUNT] = { 0,0,0,0, 0,0,0 }; /* too short */
      
        disposable_packet = etch_malloc(sizeof(test_packet), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, test_packet, sizeof(test_packet));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(test_packet), 256, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);

        result = g_my_packetizer->transport_packet(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result, -1);

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
 * test_badpacket1()
 * test 1 for bad packet data
 */
static void test_badpacket1(void)
{
   setup_this_test();

    do          
    {   int result = 0;     
        #define TBP1COUNT 8
        byte* disposable_packet = NULL;        
        byte  test_packet_1[TBP1COUNT] = { 0,0,0,0,          0,0,0,0 };  /* bad sig */
        byte  test_packet_2[TBP1COUNT] = { 1,2,3,4,          0,0,0,0 };  /* bad sig */
        byte  test_packet_3[TBP1COUNT] = { -34,-83,-66,-17,  0,1,0,0 };  /* bad len */
      
        /* bad signature 1 */
        disposable_packet = etch_malloc(sizeof(test_packet_1), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, test_packet_1, sizeof(test_packet_1));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(test_packet_1), 256, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result, -1);

        etch_object_destroy(g_mybuf); g_mybuf = NULL;

        /* bad signature 2 */
        disposable_packet = etch_malloc(sizeof(test_packet_2), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, test_packet_2, sizeof(test_packet_2));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(test_packet_2), 256, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);  

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result, -1);

        etch_object_destroy(g_mybuf); g_mybuf = NULL; 

        /* bad data length (65536) */
        disposable_packet = etch_malloc(sizeof(test_packet_3), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, test_packet_3, sizeof(test_packet_3));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(test_packet_3), 256, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);  

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result, -1);

        etch_object_destroy(g_mybuf); g_mybuf = NULL; 

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
 * test_single_single_data()
 * 3 tests for single packet in a single buffer
 */
static void test_single_single_data(void)
{
    setup_this_test();

    do          
    {   int result = 0;     
        #define TSPSB1BYTECOUNT 8
        #define TSPSB2BYTECOUNT 9
        #define TSPSB2RESULTBYTECOUNT 1
        #define TSPSB3BYTECOUNT 10
        #define TSPSB3RESULTBYTECOUNT 2
        byte* disposable_packet = NULL;        
        etch_nativearray* expected_result_2_array = NULL;
        etch_nativearray* expected_result_3_array = NULL;

        my_impl_sessionpacket* mysessionpkt = g_my_sessionpacket->thisx; 

        byte test_packet_1[TSPSB1BYTECOUNT] = { -34,-83,-66,-17,  0,0,0,0,      }; 
        byte test_packet_2[TSPSB2BYTECOUNT] = { -34,-83,-66,-17,  0,0,0,1,  1,  }; 
        byte test_packet_3[TSPSB3BYTECOUNT] = { -34,-83,-66,-17,  0,0,0,2,  3,4 }; 

        byte expected_result_2[][TSPSB2RESULTBYTECOUNT] = 
        {      
            {  
               1
            },
        };

        byte expected_result_3[][TSPSB3RESULTBYTECOUNT] = 
        {      
            {  
               3, 4
            },
        };

        /* test 1 */
        disposable_packet = etch_malloc(sizeof(test_packet_1), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, test_packet_1, sizeof(test_packet_1));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(test_packet_1), 64, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);
        CU_ASSERT_EQUAL(g_mybuf->datalen, sizeof(test_packet_1));

        result = g_my_packetizer->session_data (g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result,0);

        result = check_packetizer_resultx(g_my_sessionpacket, NULL);
        CU_ASSERT_EQUAL(result, 0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(mysessionpkt->sender);

        etch_object_destroy(g_mybuf); g_mybuf = NULL;

        /* test 2 */ 
        disposable_packet = etch_malloc(sizeof(test_packet_2), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, test_packet_2, sizeof(test_packet_2));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(test_packet_2), 9, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);
        CU_ASSERT_EQUAL(g_mybuf->datalen, sizeof(test_packet_2));

        result = g_my_packetizer->session_data (g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result,0);

        expected_result_2_array = new_etch_nativearray_from
         (&expected_result_2, CLASSID_ARRAY_BYTE, sizeof(byte), 2, TSPSB2RESULTBYTECOUNT, 1, 0);

        result = check_packetizer_resultx (g_my_sessionpacket, expected_result_2_array);
        CU_ASSERT_EQUAL(result, 0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_PACKET);
        result = is_equal_who(mysessionpkt->sender, g_who);
        CU_ASSERT_EQUAL(result, TRUE);

        etch_object_destroy(expected_result_2_array);
        etch_object_destroy(g_mybuf); g_mybuf = NULL; 
        etch_arraylist_clear (mysessionpkt->list, TRUE);

        /* test 3 */
        disposable_packet = etch_malloc(sizeof(test_packet_3), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, test_packet_3, sizeof(test_packet_3));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(test_packet_3), 10, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);
        CU_ASSERT_EQUAL(g_mybuf->datalen, sizeof(test_packet_3));

        result = g_my_packetizer->session_data (g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result,0);

        expected_result_3_array = new_etch_nativearray_from
          (&expected_result_3, CLASSID_ARRAY_BYTE, sizeof(byte), 2, TSPSB3RESULTBYTECOUNT, 1, 0);
        
        result = check_packetizer_resultx (g_my_sessionpacket, expected_result_3_array);
        CU_ASSERT_EQUAL(result, 0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_PACKET);
        result = is_equal_who(mysessionpkt->sender, g_who);
        CU_ASSERT_EQUAL(result, TRUE);

        etch_object_destroy(expected_result_3_array);
        etch_object_destroy(g_mybuf); g_mybuf = NULL; 

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
 * test_two_in_one_buffer_data_1()
 * test two packets in a single buffer
 */
static void test_two_in_one_buffer_data_1(void)
{
    setup_this_test();

    do          
    {   int result = 0;     
        #define TWOIN1_1_BYTECOUNT (8 + 8)
        signed char* disposable_packet;
     
        signed char test_packet[TWOIN1_1_BYTECOUNT] 
            = { -34,-83,-66,-17,  0,0,0,0,   
                -34,-83,-66,-17,  0,0,0,0,  
              }; 

        my_impl_sessionpacket* mysessionpkt = g_my_sessionpacket->thisx; 
        disposable_packet = etch_malloc(sizeof(test_packet), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, test_packet, sizeof(test_packet));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(test_packet), 64, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);
        CU_ASSERT_EQUAL(g_mybuf->datalen, sizeof(test_packet));

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result,0);

        result = check_packetizer_resultx(g_my_sessionpacket, NULL);
        CU_ASSERT_EQUAL(result, 0);
        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(mysessionpkt->sender);

        etch_object_destroy(g_mybuf); g_mybuf = NULL;

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
 * test_two_in_one_buffer_data_2()
 * test two packets in a single buffer - 2
 */
static void test_two_in_one_buffer_data_2(void)
{
    setup_this_test();

    do          
    {   int result = 0;    
        #define TWOIN1_2_BYTECOUNT (8 + 1 + 8)
        #define TWOIN1_2_RESULT_BYTECOUNT 1
        byte* disposable_packet;
     
        byte test_packet[TWOIN1_2_BYTECOUNT] 
            = { -34,-83,-66,-17,  0,0,0,1, 1,  
                -34,-83,-66,-17,  0,0,0,0,  
              }; 

        byte expected_result[][TWOIN1_2_RESULT_BYTECOUNT] = 
        {      
            {  
               1
            },
        };

        my_impl_sessionpacket* mysessionpkt = g_my_sessionpacket->thisx; 

        etch_nativearray* expected_result_array = new_etch_nativearray_from
          (&expected_result, CLASSID_ARRAY_BYTE, sizeof(byte), 2, 
            TWOIN1_2_RESULT_BYTECOUNT, 1, 0);

        disposable_packet = etch_malloc(sizeof(test_packet), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, test_packet, sizeof(test_packet));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(test_packet), 17, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);
        CU_ASSERT_EQUAL(g_mybuf->datalen, sizeof(test_packet));

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result,0);

        result = check_packetizer_resultx(g_my_sessionpacket, expected_result_array);
        CU_ASSERT_EQUAL(result, 0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_PACKET);
        result = is_equal_who(mysessionpkt->sender, g_who);
        CU_ASSERT_EQUAL(result, TRUE);
       
        etch_object_destroy(g_mybuf); g_mybuf = NULL;
        etch_object_destroy(expected_result_array);

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
 * test_two_in_one_buffer_data_3()
 * test two packets in a single buffer - 3
 */
static void test_two_in_one_buffer_data_3(void)
{
    setup_this_test();

    do          
    {   int result = 0;    
        #define TWOIN1_3_BYTECOUNT (8 + 8 + 1)
        #define TWOIN1_3_RESULT_BYTECOUNT 1
        byte* disposable_packet;
     
        byte test_packet[TWOIN1_3_BYTECOUNT] 
            = { -34,-83,-66,-17,  0,0,0,0,   
                -34,-83,-66,-17,  0,0,0,1, 2  
              }; 

        byte expected_result[][TWOIN1_3_RESULT_BYTECOUNT] = 
        {      
            {  
               2
            },
        };

        my_impl_sessionpacket* mysessionpkt = g_my_sessionpacket->thisx; 

        etch_nativearray* expected_result_array = new_etch_nativearray_from
          (&expected_result, CLASSID_ARRAY_BYTE, sizeof(byte), 2, 
            TWOIN1_3_RESULT_BYTECOUNT, 1, 0);

        disposable_packet = etch_malloc(sizeof(test_packet), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, test_packet, sizeof(test_packet));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(test_packet), 17, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);
        CU_ASSERT_EQUAL(g_mybuf->datalen, sizeof(test_packet));

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result,0);

        result = check_packetizer_resultx(g_my_sessionpacket, expected_result_array);
        CU_ASSERT_EQUAL(result, 0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_PACKET);
        result = is_equal_who(mysessionpkt->sender, g_who);
        CU_ASSERT_EQUAL(result, TRUE);

        etch_object_destroy(g_mybuf); g_mybuf = NULL;
        etch_object_destroy(expected_result_array);

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
 * test_two_in_one_buffer_data_4()
 * test two packets in a single buffer - 4
 */
static void test_two_in_one_buffer_data_4(void)
{
    setup_this_test();

    do          
    {   int result = 0;        
        signed char* disposable_packet;

        /* byte count for all packets */
        #define TWOIN1_4_BYTECOUNT (8 + 1 + 8 + 1)

        /* byte count for low order dimension of expected result array ( [2][1] ) */
        #define TWOIN1_4_RESULT_BYTECOUNT 1 
     
        signed char test_packet[TWOIN1_4_BYTECOUNT] 
            = { -34,-83,-66,-17,  0,0,0,1,  1,  
                -34,-83,-66,-17,  0,0,0,1,  2,  
              }; 

        signed char expected_result[][TWOIN1_4_RESULT_BYTECOUNT] = 
        {      
            {  
               1,    /* packet 1 data */
            },
            {  
               2,    /* packet 2 data */
            },
        };

        my_impl_sessionpacket* mysessionpkt = g_my_sessionpacket->thisx; 

        etch_nativearray* expected_result_array = new_etch_nativearray_from
          (&expected_result, CLASSID_ARRAY_BYTE, 
                sizeof(byte), /* size of an array item is 1 */
                2,            /* count of dimensions [2][1] */
                1,            /* count of items in low order dimension */
                2,            /* count of items in secondary dimension */          
                0);           /* 3rd dimension (not applicable) */

        disposable_packet = etch_malloc(sizeof(test_packet), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, test_packet, sizeof(test_packet));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(test_packet), 0, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);
        CU_ASSERT_EQUAL(g_mybuf->datalen, sizeof(test_packet));

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result,0);

        result = check_packetizer_resultx(g_my_sessionpacket, expected_result_array);
        CU_ASSERT_EQUAL(result, 0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_PACKET);
        result = is_equal_who(mysessionpkt->sender, g_who);
        CU_ASSERT_EQUAL(result, TRUE);

        etch_object_destroy(g_mybuf); g_mybuf = NULL;
        etch_object_destroy(expected_result_array);

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
 * test_two_in_one_buffer_data_5()
 * test two packets in a single buffer - 5
 */
static void test_two_in_one_buffer_data_5(void)
{
    setup_this_test();

    do          
    {   int result = 0;        
        signed char* disposable_packet;

        /* byte count for all packets */
        #define TWOIN1_5_BYTECOUNT (8 + 2 + 8 + 2)

        /* byte count for low order dimension of expected result array ( [2][2] ) */
        #define TWOIN1_5_RESULT_BYTECOUNT 2 
     
        signed char test_packet[TWOIN1_5_BYTECOUNT] 
            = { -34,-83,-66,-17,  0,0,0,2,  3, 4,   
                -34,-83,-66,-17,  0,0,0,2,  5, 6,  
              }; 

        signed char expected_result[][TWOIN1_5_RESULT_BYTECOUNT] = 
        {      
            {  
               3, 4,    /* packet 1 data */
            },
            {  
               5, 6,    /* packet 2 data */
            },
        };

        my_impl_sessionpacket* mysessionpkt = g_my_sessionpacket->thisx; 

        etch_nativearray* expected_result_array = new_etch_nativearray_from
          (&expected_result, CLASSID_ARRAY_BYTE, 
                sizeof(byte), /* size of an array item is 1 */
                2,            /* count of dimensions [2][2] */
                2,            /* count of items in low order dimension */
                2,            /* count of items in secondary dimension */          
                0);           /* 3rd dimension (not applicable) */

        disposable_packet = etch_malloc(sizeof(test_packet), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, test_packet, sizeof(test_packet));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(test_packet), 0, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);
        CU_ASSERT_EQUAL(g_mybuf->datalen, sizeof(test_packet));

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result,0);

        result = check_packetizer_resultx(g_my_sessionpacket, expected_result_array);
        CU_ASSERT_EQUAL(result, 0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_PACKET);
        result = is_equal_who(mysessionpkt->sender, g_who);
        CU_ASSERT_EQUAL(result, TRUE);

        etch_object_destroy(g_mybuf); g_mybuf = NULL;
        etch_object_destroy(expected_result_array);

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
 * test_two_in_two_buffers_data_1()
 * test two packets in two buffers with header split across buffers
 * packets are empty in this test
 */
static void test_two_in_two_buffers_data_1(void)
{
    setup_this_test();

    do          
    {   int result = 0;    
        #define TWOIN2_1_BUF1_BYTECOUNT 6
        #define TWOIN2_1_BUF2_BYTECOUNT 10
        etch_flexbuffer* mybuf2=0;
        byte* disposable_packet=0;
     
        byte buf1_data[TWOIN2_1_BUF1_BYTECOUNT] 
            = { -34,-83,-66,-17,  0,0,      /* packet 1 header (partial) */  
              }; 

        byte buf2_data[TWOIN2_1_BUF2_BYTECOUNT] 
            = {  0, 0,                      /* packet 1 header (balance) */ 
                -34,-83,-66,-17,  0,0,0,0,  /* packet 2 header */   
              }; 

        my_impl_sessionpacket* mysessionpkt = g_my_sessionpacket->thisx; 

        /* buffer 1 */
        disposable_packet = etch_malloc(sizeof(buf1_data), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, buf1_data, sizeof(buf1_data));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(buf1_data), 0, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);
        CU_ASSERT_EQUAL(g_mybuf->datalen, sizeof(buf1_data));

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result,0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(mysessionpkt->sender);

        /* buffer 2 */
        disposable_packet = etch_malloc(sizeof(buf2_data), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, buf2_data, sizeof(buf2_data));
       
        mybuf2 = new_flexbuffer_from(disposable_packet, sizeof(buf2_data), 0, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(mybuf2);
        CU_ASSERT_EQUAL(mybuf2->datalen, sizeof(buf2_data));

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result,0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(mysessionpkt->sender);

        /* done - free memory */
        etch_object_destroy(g_mybuf); g_mybuf = NULL;
        etch_object_destroy(mybuf2);    

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
 * test_two_in_two_buffers_data_2()
 * test two packets in two buffers with header split across buffers
 * packets include bodies in this test
 */
static void test_two_in_two_buffers_data_2(void)
{
    setup_this_test();

    do          
    {   int result = 0; 
        #define TWOIN2_2_BUF1_BYTECOUNT 6
        #define TWOIN2_2_BUF2_BYTECOUNT (2 + 2 + 8 + 2)
        /* byte count for low order dimension of expected result array ( [2][2] ) */
        #define TWOIN2_2_RESULT_BYTECOUNT 2 
        etch_flexbuffer* mybuf2=0;
        byte* disposable_packet=0;
     
        byte buf1_data[TWOIN2_2_BUF1_BYTECOUNT] 
            = { -34,-83,-66,-17,  0,0,      /* packet 1 header (partial) */  
              }; 

        byte buf2_data[TWOIN2_2_BUF2_BYTECOUNT] 
            = {  0, 2,                      /* packet 1 header (balance) */ 
                 3, 4,                      /* packet 1 body */ 
                -34,-83,-66,-17,  0,0,0,2,  /* packet 2 header */  
                 5, 6,                      /* packet 2 body */ 
              }; 

        byte expected_result[][TWOIN2_2_RESULT_BYTECOUNT] = 
        {      
            {  
               3, 4,    /* packet 1 body */
            },
            {  
               5, 6,    /* packet 2 body */
            },
        };

        my_impl_sessionpacket* mysessionpkt = g_my_sessionpacket->thisx; 

        etch_nativearray* expected_result_array = new_etch_nativearray_from
          (&expected_result, CLASSID_ARRAY_BYTE, 
                sizeof(byte), /* size of an array item is 1 */
                2,            /* count of dimensions [2][2] */
                2,            /* count of items in low order dimension */
                2,            /* count of items in secondary dimension */          
                0             /* 3rd dimension (not applicable) */
          );           

        /* buffer 1 */
        disposable_packet = etch_malloc(sizeof(buf1_data), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, buf1_data, sizeof(buf1_data));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(buf1_data), 0, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);
        CU_ASSERT_EQUAL(g_mybuf->datalen, sizeof(buf1_data));

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result,0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(mysessionpkt->sender);

        /* buffer 2 */
        disposable_packet = etch_malloc(sizeof(buf2_data), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, buf2_data, sizeof(buf2_data));
       
        mybuf2 = new_flexbuffer_from(disposable_packet, sizeof(buf2_data), 0, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(mybuf2);
        CU_ASSERT_EQUAL(mybuf2->datalen, sizeof(buf2_data));

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, mybuf2);
        CU_ASSERT_EQUAL(result,0);

        result = check_packetizer_resultx(g_my_sessionpacket, expected_result_array);
        CU_ASSERT_EQUAL(result, 0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_PACKET);
        result = is_equal_who(mysessionpkt->sender, g_who);
        CU_ASSERT_EQUAL(result, TRUE);
      
        /* done - free memory */
        etch_object_destroy(expected_result_array);
        etch_object_destroy(g_mybuf); g_mybuf = NULL;
        etch_object_destroy(mybuf2);    

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
 * test_two_in_two_buffers_data_3()
 * test two packets in two buffers with body split across buffers
 */
static void test_two_in_two_buffers_data_3(void)
{
    setup_this_test();

    do
    {   int result = 0; 
        #define TWOIN2_3_BUF1_BYTECOUNT (8 + 1)     /* header1 + body1 1 of 2 */
        #define TWOIN2_3_BUF2_BYTECOUNT (1 + 8 + 2) /* body1 2 of 2 + header2 + body2 */
        /* byte count for low order dimension of expected result array ( [2][2] ) */
        #define TWOIN2_3_RESULT_BYTECOUNT 2 
        etch_flexbuffer* mybuf2 = NULL;
        signed char* disposable_packet = NULL;
     
        signed char buf1_data[TWOIN2_3_BUF1_BYTECOUNT] 
            = { -34,-83,-66,-17,  0, 0, 0, 2,   /* packet 1 header (length 2) */
                 1,                             /* packet 1 partial body */  
              }; 

        signed char buf2_data[TWOIN2_3_BUF2_BYTECOUNT] 
            = {  2,                             /* packet 1 body balance */       
                -34,-83,-66,-17,  0, 0, 0, 2,   /* packet 2 header */  
                 3, 4,                          /* packet 2 body */ 
              }; 

        signed char expected_result[][TWOIN2_3_RESULT_BYTECOUNT] = 
        {      
            {  
               1, 2,    /* packet 1 body */
            },
            {  
               3, 4,    /* packet 2 body */
            },
        };

        my_impl_sessionpacket* mysessionpkt = g_my_sessionpacket->thisx; 

        etch_nativearray* expected_result_array = new_etch_nativearray_from
          (&expected_result, CLASSID_ARRAY_BYTE, 
                sizeof(byte), /* size of an array item is 1 */
                2,            /* count of dimensions [2][2] */
                2,            /* count of items in low order dimension */
                2,            /* count of items in secondary dimension */          
                0             /* 3rd dimension (not applicable) */
          );           

        /* buffer 1 */
        disposable_packet = etch_malloc(sizeof(buf1_data), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, buf1_data, sizeof(buf1_data));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(buf1_data), 0, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);
        CU_ASSERT_EQUAL(g_mybuf->datalen, sizeof(buf1_data));

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result,0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(mysessionpkt->sender);

        /* buffer 2 */
        disposable_packet = etch_malloc(sizeof(buf2_data), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, buf2_data, sizeof(buf2_data));
       
        mybuf2 = new_flexbuffer_from(disposable_packet, sizeof(buf2_data), 0, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(mybuf2);
        CU_ASSERT_EQUAL(mybuf2->datalen, sizeof(buf2_data));

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, mybuf2);
        CU_ASSERT_EQUAL(result,0);

        result = check_packetizer_resultx(g_my_sessionpacket, expected_result_array);
        CU_ASSERT_EQUAL(result, 0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_PACKET);
        result = is_equal_who(mysessionpkt->sender, g_who);
        CU_ASSERT_EQUAL(result, TRUE);

        /* done - free memory */
        etch_object_destroy(expected_result_array);
        etch_object_destroy(g_mybuf); g_mybuf = NULL;
        etch_object_destroy(mybuf2);    

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
 * test_two_in_two_buffers_data_4()
 * test two packets in two buffers with body split across buffers
 */
static void test_two_in_two_buffers_data_4(void)
{
    setup_this_test();

    do          
    {   int result = 0; 
        #define TWOIN2_4_BUF1_BYTECOUNT (8 + 2)     /* header1 + body1 2 of 3 */
        #define TWOIN2_4_BUF2_BYTECOUNT (1 + 8 + 3) /* body1 3 of 3 + header2 + body2 */
        /* byte count for low order dimension of expected result array ( [2][2] ) */
        #define TWOIN2_4_RESULT_BYTECOUNT 3 
        etch_flexbuffer* mybuf2=0;
        byte* disposable_packet=0;
     
        byte buf1_data[TWOIN2_4_BUF1_BYTECOUNT] 
            = { -34,-83,-66,-17,  0, 0, 0, 3,   /* packet 1 header (length 2) */
                 5, 6,                          /* packet 1 partial body */  
              }; 

        byte buf2_data[TWOIN2_4_BUF2_BYTECOUNT] 
            = {  7,                             /* packet 1 body balance */       
                -34,-83,-66,-17,  0, 0, 0, 3,   /* packet 2 header */  
                 8, 9, 10                       /* packet 2 body */ 
              }; 

        byte expected_result[][TWOIN2_4_RESULT_BYTECOUNT] = 
        {      
            {  
               5, 6, 7,    /* packet 1 body */
            },
            {  
               8, 9, 10,   /* packet 2 body */
            },
        };

        etch_nativearray* expected_result_array = new_etch_nativearray_from
          (&expected_result, CLASSID_ARRAY_BYTE, 
                sizeof(byte), /* size of an array item is 1 */
                2,            /* count of dimensions [2][2] */
                3,            /* count of items in low order dimension */
                2,            /* count of items in secondary dimension */          
                0             /* 3rd dimension (not applicable) */
          );  

        my_impl_sessionpacket* mysessionpkt = g_my_sessionpacket->thisx;          

        /* buffer 1 */
        disposable_packet = etch_malloc(sizeof(buf1_data), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, buf1_data, sizeof(buf1_data));
       
        g_mybuf = new_flexbuffer_from(disposable_packet, sizeof(buf1_data), 10, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(g_mybuf);
        CU_ASSERT_EQUAL(g_mybuf->datalen, sizeof(buf1_data));

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, g_mybuf);
        CU_ASSERT_EQUAL(result,0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(mysessionpkt->sender);

        /* buffer 2 */
        disposable_packet = etch_malloc(sizeof(buf2_data), ETCHTYPEB_BYTES);
        memcpy(disposable_packet, buf2_data, sizeof(buf2_data));
       
        mybuf2 = new_flexbuffer_from(disposable_packet, sizeof(buf2_data), 0, 0);
        CU_ASSERT_PTR_NOT_NULL_FATAL(mybuf2);
        CU_ASSERT_EQUAL(mybuf2->datalen, sizeof(buf2_data));

        result = g_my_packetizer->session_data(g_my_packetizer, g_who, mybuf2);
        CU_ASSERT_EQUAL(result,0);

        result = check_packetizer_resultx(g_my_sessionpacket, expected_result_array);
        CU_ASSERT_EQUAL(result, 0);

        CU_ASSERT_EQUAL(mysessionpkt->what, WHAT_PACKET);
        result = is_equal_who(mysessionpkt->sender, g_who);
        CU_ASSERT_EQUAL(result, TRUE);
        /* done - free memory */
        etch_object_destroy(expected_result_array);
        etch_object_destroy(g_mybuf); g_mybuf = NULL;
        etch_object_destroy(mybuf2);    

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
 * test the session control notification plumbing
 */
static void test_session_control(void)
{
    setup_this_test();

    do          
    {   
        const int MY_CONTROL_CLASSID = 0x5200, MY_VALUE_CLASSID = 0x5201;
        etch_object* mycontrolobj = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_CONTROL_CLASSID);
        etch_object* myvalueobj   = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_VALUE_CLASSID);

        /* g_my_sessionpacket is the i_sessionpacket interface    
         * my_impl_sessionpacket is the implementing test class */
        my_impl_sessionpacket* my_session = g_my_sessionpacket->thisx;          
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session);
        CU_ASSERT_EQUAL(my_session->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_session->control);
        CU_ASSERT_PTR_NULL(my_session->value);

        /* we relinquish memory for mycontrolobj and myvalueobj here. 
         * the session_control terminal destination must destroy them, which here
         * is handled by our session object destructor when we teardown_this_test() */
        g_my_packetizer->session_control (g_my_packetizer, (void*)mycontrolobj, myvalueobj);

        CU_ASSERT_EQUAL(my_session->what, SESSION_CONTROL);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session->control);
        CU_ASSERT_EQUAL(((etch_object*)my_session->control)->class_id, MY_CONTROL_CLASSID);
        CU_ASSERT_EQUAL(((etch_object*)my_session->value)->class_id,   MY_VALUE_CLASSID);

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
 * test the session notify notification plumbing
 */
static void test_session_notify(void)
{
    setup_this_test();

    do          
    {   
        const int MY_EVENT_CLASSID = 0x5202;
        etch_object* myeventobj = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_EVENT_CLASSID);

        /* g_my_sessionpacket is the i_sessionpacket interface    
         * my_impl_sessionpacket is the implementing test class */
        my_impl_sessionpacket* my_session = g_my_sessionpacket->thisx;   
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session);
        CU_ASSERT_EQUAL(my_session->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_session->eventx);

        /* we relinquish memory for myeventobj here. 
         * the session_control terminal destination must destroy it, which here
         * is handled by our session object destructor when we teardown_this_test() */
        g_my_packetizer->session_notify(g_my_packetizer, (void*)myeventobj);

        CU_ASSERT_EQUAL(my_session->what, SESSION_NOTIFY);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session->eventx);
        CU_ASSERT_EQUAL(((etch_object*)my_session->eventx)->class_id, MY_EVENT_CLASSID);

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
 * test the session query notification plumbing
 */
static void test_session_query(void)
{
    setup_this_test();

    do          
    {   
        const int MY_QUERY_CLASSID = 0x5203, MY_RESULT_CLASSID = 0x5204;
        etch_object* myqueryobj  = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_QUERY_CLASSID);
        etch_object* myresultobj = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_RESULT_CLASSID);
        etch_object* queryresult = NULL;

        /* g_my_sessionpacket is the i_sessionpacket interface    
         * my_impl_sessionpacket is the implementing test class */
        my_impl_sessionpacket* my_session = g_my_sessionpacket->thisx;          
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session);
        CU_ASSERT_EQUAL(my_session->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_session->query);

        /* we relinquish myresultobj here. see comments following */
        my_session->query_result = myresultobj; 

        /* we relinquish memory for myqueryobj here and assume queryresult. 
         * the session_control terminal destination must destroy it, which here
         * is handled by our session object destructor when we teardown_this_test() */
        queryresult = g_my_packetizer->session_query(g_my_packetizer, (etch_query*)myqueryobj);

        CU_ASSERT_EQUAL(my_session->what, SESSION_QUERY);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_session->query);
        CU_ASSERT_EQUAL(((etch_object*)my_session->query)->class_id, MY_QUERY_CLASSID);
        CU_ASSERT_PTR_NOT_NULL_FATAL(queryresult);
        CU_ASSERT_EQUAL(((etch_object*)queryresult)->class_id, MY_RESULT_CLASSID);
        etch_object_destroy(queryresult);

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
 * test the transport control notification plumbing
 */
static void test_transport_control(void)
{
    setup_this_test();

    do          
    {   
        const int MY_CONTROL_CLASSID = 0x5200, MY_VALUE_CLASSID = 0x5201;
        etch_object* mycontrolobj = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_CONTROL_CLASSID);
        etch_object* myvalueobj   = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_VALUE_CLASSID);

        /* g_my_transportdata is the i_transportdata interface    
         * my_impl_transportdata is the implementing test class */
        my_impl_transportdata* my_transport = g_my_transportdata->thisx; 
        CU_ASSERT_FATAL(is_my_impl_transportdata(my_transport));
        CU_ASSERT_EQUAL(my_transport->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_transport->control);
        CU_ASSERT_PTR_NULL(my_transport->value);

        /* we relinquish memory for mycontrolobj and myvalueobj here. 
         * the transport_control terminal destination must destroy them, which here
         * is handled by our transport object destructor when we teardown_this_test() */
        g_my_packetizer->transport_control(g_my_packetizer, (etch_event*)mycontrolobj, myvalueobj);

        CU_ASSERT_EQUAL(my_transport->what, TRANSPORT_CONTROL);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport->control);
        CU_ASSERT_EQUAL(((etch_object*)my_transport->control)->class_id, MY_CONTROL_CLASSID);
        CU_ASSERT_EQUAL(((etch_object*)my_transport->value)->class_id,   MY_VALUE_CLASSID);

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
 * test the transport notify notification plumbing
 */
static void test_transport_notify(void)
{
    setup_this_test();

    do          
    {   
        const int MY_EVENT_CLASSID = 0x5202;
        etch_object* myeventobj = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_EVENT_CLASSID);

        /* g_my_transportdata is the i_transportdata interface    
         * my_impl_transportdata is the implementing test class */
        my_impl_transportdata* my_transport = g_my_transportdata->thisx; 
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport);
        CU_ASSERT_EQUAL(my_transport->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_transport->control);
        CU_ASSERT_PTR_NULL(my_transport->value);

        /* we relinquish memory for myeventobj here. 
         * the transport_control terminal destination must destroy it, which here
         * is handled by our transport object destructor when we teardown_this_test() */
        g_my_packetizer->transport_notify(g_my_packetizer, (etch_event*)myeventobj);

        CU_ASSERT_EQUAL(my_transport->what, TRANSPORT_NOTIFY);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport->eventx);
        CU_ASSERT_EQUAL(((etch_object*)my_transport->eventx)->class_id, MY_EVENT_CLASSID);

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
 * test the transport query notification plumbing
 */
static void test_transport_query(void)
{
    setup_this_test();

    do          
    {   
        const int MY_QUERY_CLASSID = 0x5203, MY_RESULT_CLASSID = 0x5204;
        etch_object* myqueryobj  = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_QUERY_CLASSID);
        etch_object* myresultobj = new_object(sizeof(etch_object),ETCHTYPEB_ETCHOBJECT, MY_RESULT_CLASSID);
        etch_object* queryresult = NULL;

        /* g_my_transportdata is the i_transportdata interface    
         * my_impl_transportdata is the implementing test class */
        my_impl_transportdata* my_transport = g_my_transportdata->thisx; 
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport);
        CU_ASSERT_EQUAL(my_transport->what, WHAT_NONE);
        CU_ASSERT_PTR_NULL(my_transport->query);

        /* we relinquish myresultobj here. see comments following */
        my_transport->query_result = myresultobj; 

        /* we relinquish memory for myqueryobj here and assume queryresult. 
         * the transport_control terminal destination must destroy it, which here
         * is handled by our transport object destructor when we teardown_this_test() */
        queryresult = g_my_packetizer->transport_query(g_my_packetizer, (etch_query*)myqueryobj);

        CU_ASSERT_EQUAL(my_transport->what, TRANSPORT_QUERY);
        CU_ASSERT_PTR_NOT_NULL_FATAL(my_transport->query);
        CU_ASSERT_EQUAL(((etch_object*)my_transport->query)->class_id, MY_QUERY_CLASSID);
        CU_ASSERT_PTR_NOT_NULL_FATAL(queryresult);
        CU_ASSERT_EQUAL(((etch_object*)queryresult)->class_id, MY_RESULT_CLASSID);
        etch_object_destroy(queryresult);

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
CU_pSuite test_etch_packetizer_suite()
{
    CU_pSuite ps = CU_add_suite("suite_packetizer", init_suite, clean_suite);

    CU_add_test(ps, "test sessionpacket ctor/dtor", test_sessionpacket_constructor);
    CU_add_test(ps, "test transportdata ctor/dtor", test_transportdata_constructor);
    CU_add_test(ps, "test packetizer ctor/dtor", test_packetizer_constructor);
    CU_add_test(ps, "test unit test setup/teardown", test_test_setup);
    CU_add_test(ps, "test packet1 to source", test_to_source_packet1);
    CU_add_test(ps, "test packet2 to source", test_to_source_packet2);
    CU_add_test(ps, "test packet3 to source", test_to_source_packet3);
    CU_add_test(ps, "test bad packet 0", test_to_source_badpacket1);
    CU_add_test(ps, "test bad packet 1", test_badpacket1);

    CU_add_test(ps, "test 1 packet 1 buffer", test_single_single_data);
    CU_add_test(ps, "test 2 in 1 buffer 1",  test_two_in_one_buffer_data_1);
    CU_add_test(ps, "test 2 in 1 buffer 2",  test_two_in_one_buffer_data_2);
    CU_add_test(ps, "test 2 in 1 buffer 3",  test_two_in_one_buffer_data_3);
    CU_add_test(ps, "test 2 in 1 buffer 4",  test_two_in_one_buffer_data_4);
    CU_add_test(ps, "test 2 in 1 buffer 5",  test_two_in_one_buffer_data_5);
    CU_add_test(ps, "test 2 in 2 buffers 1", test_two_in_two_buffers_data_1);
    CU_add_test(ps, "test 2 in 2 buffers 2", test_two_in_two_buffers_data_2);
    CU_add_test(ps, "test 2 in 2 buffers 3", test_two_in_two_buffers_data_3);
    CU_add_test(ps, "test 2 in 2 buffers 4", test_two_in_two_buffers_data_4);

    CU_add_test(ps, "test session control",  test_session_control); 
    CU_add_test(ps, "test session notify",  test_session_notify); 
    CU_add_test(ps, "test session query",  test_session_query); 
    CU_add_test(ps, "test transport control",  test_transport_control); 
    CU_add_test(ps, "test transport notify",  test_transport_notify); 
    CU_add_test(ps, "test transport query",  test_transport_query); 

    return ps;
}
