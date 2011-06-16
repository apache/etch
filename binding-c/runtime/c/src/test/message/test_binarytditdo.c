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
 * test_binarytditdo.c 
 * test binary input and output
 */

#include "etch_runtime.h"
#include "etch_binary_tdi.h"
#include "etch_binary_tdo.h"
#include "etch_connection.h"
#include "etch_default_value_factory.h"
#include "etch_binary_tdo.h"
#include "etch_binary_tdi.h"
#include "etch_messagizer.h"
#include "etch_resources.h"
#include "etch_nativearray.h"
#include "etch_arrayval.h"
#include "etch_thread.h"
#include "etch_exception.h"
#include "etch_objecttypes.h"
#include "etch_general.h"
#include "etch_map.h"
#include "etch_log.h"

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

static etch_type*  mt_footype;
static etch_field* mf_fookey;
static int objcount, arraycount; 
static unsigned short CLASSID_MY_IMPL_TP;


static void this_test_setup()
{
    mt_footype = new_static_type(L"footype");
    mf_fookey  = new_static_field(L"foofield");
}

static void this_test_teardown()
{
    destroy_static_type(mt_footype);
    destroy_static_field(mf_fookey);
}

#if 0
/**
 * etch_system_nanotime()
 * operating system specific implementation of java System.nanotime().
 */
static int64 etch_system_nanotime()
{
    int64 result = 0;

#ifdef WIN32
    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    result = li.QuadPart;
#else
     #warning etch_system_nanotime have to be checked, we need a nanotime function
    //TODO check this
    struct timeval time;
    gettimeofday(&time, NULL);
    result = time.tv_usec * 1000;
#endif

    return result;
}
#endif

static signed char* get_test_bytes(signed char minval, int n)
{
    int i = 0;
    signed char* vals = malloc(n * sizeof(char));
    while(n--) vals[i++] = minval++;
    return vals;
}


static short* get_test_shorts(short minval, int n)
{
    int i = 0;
    short* vals = malloc(n * sizeof(short));
    while(n--) vals[i++] = minval++;
    return vals;
}


static int* get_test_ints(int* out)
{
    int n = 65536+2+2, k = 65536+2, i = 0;
    int minval = ETCHTYPE_MIN_INT16 - 1;
    int*  vals = malloc(n * sizeof(int));
    while(k--) vals[i++] = minval++;
    vals[i++] = ETCHTYPE_MIN_INT32;
    vals[i++] = ETCHTYPE_MAX_INT32;
    *out = n;
    return vals;
}


static int64* get_test_longs(int* out)
{
    int n = 65536+2+6, k = 65536+2, i = 0;
    int minval  = ETCHTYPE_MIN_INT16 - 1;
    int64* vals = malloc(n * sizeof(int64));
    while(k--) vals[i++] = minval++;
    vals[i++] = ETCHTYPE_MIN_INT32;
    vals[i++] = ETCHTYPE_MAX_INT32;
    vals[i++] = (int64) ETCHTYPE_MIN_INT32 - 1;
    vals[i++] = (int64) ETCHTYPE_MAX_INT32 + 1;
    vals[i++] = ETCHTYPE_MIN_INT64;
    vals[i++] = ETCHTYPE_MAX_INT64;
    *out = n;
    return vals;
}


#if 0
/**
 * do_perftest
 * measure performance of messagizer.transport_message()
 */
static void do_perftest(char* name, const int iter, etch_messagizer* messagizer, 
    etch_message* msg, const int n)
{
    int64 t0, t1;
    double dtime, drate;
    int i = 0, result = 0;
    etch_who* whofrom = NULL;

    /* ensure message will not be destroyed by transport_message 
     * since we invoke transport_message multiple times here */
    unsigned char save_msg_staticflag = ((etch_object*)msg)->is_static;
    set_etchobj_static_all(msg); 
    printf("\n");

    t0 = etch_system_nanotime();

    for(; i < n; i++)
        result = messagizer->transport_message(messagizer, whofrom, msg);

    t1 = etch_system_nanotime();

    dtime = (t1 - t0) / (double) 1000000000;
    drate = (double) n / dtime;

    printf("%s iteration %d time %7.3f count %d rate %7.3f\n", name, iter, dtime, n, drate);

    ((etch_object*)msg)->is_static = save_msg_staticflag;
}
#endif

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
    etch_object object;

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
    i_transportpacket* ixp;           /* owned */ 
    etch_object_destructor destroy_transportpacket;  /* i_transportpacket original destructor */
    etch_transport_packet transport_packet;        /* transport_packet() */
    etch_transport_packet_headersize  header_size; /* header_size() */

    i_sessionpacket* session;         /* not owned */

} my_impl_transportpacket;


/**
 * destroy_my_impl_transportpacket()
 * my_impl_transportpacket destructor
 */
static int destroy_my_impl_transportpacket(void* transport_package)
{
    my_impl_transportpacket* thisx = (my_impl_transportpacket*)transport_package;

    if (!is_etchobj_static_content(thisx))
    {       /* invoke original i_transportpacket destructor */
        if (thisx->ixp && thisx->destroy_transportpacket)   
            thisx->destroy_transportpacket(thisx->ixp);
    }

   return destroy_objectex((etch_object*) thisx);
}


/**
 * impl_transport_packet()
 * my_impl_transportpacket::transport_packet
 * @param whoto caller retains, can be null
 * @param fbuf caller retains
 */
static int impl_transport_packet (void* data, void* whoto, void* fbuf)
{
    return 0;
}


/**
 * my_transport_control()
 * my_impl_transportpacket::itransport::transport_control 
 */
static int my_transport_control (void* data, etch_object* control, etch_object* value)
{
    return 0;
}


/**
 * my_transport_notify()
 * my_impl_transportpacket::itransport::transport_notify 
 */
static int my_transport_notify (i_transportpacket* itp, etch_object* evt)
{
    return 0;
}


/**
 * my_transport_query()
 * my_impl_transportpacket::itransport::transport_query 
 */
static etch_object* my_transport_query (i_transportpacket* itp, etch_object* query) 
{
    return NULL;
}


/**
 * my_transport_get_session()
 * my_impl_transportpacket::itransport::get_session 
 */
static i_sessionpacket* my_transport_get_session(i_transportpacket* itp)
{
    my_impl_transportpacket* mytp = itp->thisx; 
    return mytp->session;
}


/**
 * my_transport_set_session()
 * my_impl_transportpacket::itransport::set_session
 */
static void my_transport_set_session(i_transportpacket* itp, i_sessionpacket* session)
{   
    my_impl_transportpacket* mytp = itp->thisx; 
    mytp->session = session;
}


/*
 * destroy_my_transportpacket()
 * i_transportpacket destructor
 * this destructor will destroy its parent (my_impl_transportpacket), 
 * which will in turn destroy this object.
 */
static int destroy_my_transportpacket(void* data)
{
    i_transportpacket* itp = (i_transportpacket*)data;
    my_impl_transportpacket* mytp = NULL;
    if (NULL == itp) return -1;

    mytp = itp->thisx;  

    etch_object_destroy(mytp);

    return 0;
}

/**
 * new_my_impl_transportpacket()
 * my_impl_transportpacket constructor
 */
static my_impl_transportpacket* new_my_impl_transportpacket()
{
    i_transportpacket* itp  = NULL;
    i_transport* itransport = NULL;
    /* this is a model for dynamic class ID assigment */
    unsigned short class_id = CLASSID_MY_IMPL_TP? CLASSID_MY_IMPL_TP: 
        (CLASSID_MY_IMPL_TP = get_dynamic_classid());

    my_impl_transportpacket* mytp = (my_impl_transportpacket*) new_object
      (sizeof(my_impl_transportpacket), ETCHTYPEB_TRANSPORTPKT, class_id);

    ((etch_object*)mytp)->destroy = destroy_my_impl_transportpacket;

    itransport = new_transport_interface_ex(mytp,
        (etch_transport_control)     my_transport_control, 
        (etch_transport_notify)      my_transport_notify, 
        (etch_transport_query)       my_transport_query,
        (etch_transport_get_session) my_transport_get_session, 
        (etch_transport_set_session) my_transport_set_session);

    itp = new_transportpkt_interface(mytp, impl_transport_packet, itransport);

    /* save off i_transportpacket destructor */
    mytp->destroy_transportpacket = ((etch_object*)itp)->destroy;

    /* replace i_transportpacket destructor with one which will destroy this object */
    ((etch_object*)itp)->destroy = destroy_my_transportpacket;

    /* g_my_transportpacket will get set to this interface */
    mytp->ixp = itp;  

    return mytp;
}



/* - - - - - - - - - - - - - - 
 * unit tests
 * - - - - - - - - - - - - - -
 */

/**
 * test_constructor_0
 * verify that memory is freed as expected
 */
static void test_constructor_0(void)
{
    binary_tagged_data_output* tdo = new_binary_tagdata_output(NULL, NULL);

    etch_object_destroy(tdo);

    etchvf_free_builtins(); 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_constructor_1
 * verify that memory is freed as expected
 */
static void test_constructor_1(void)
{

    default_value_factory*  vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    binary_tagged_data_output* tdo          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    tdo = new_binary_tagdata_output((etch_value_factory*) vf, NULL);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    etch_object_destroy(tdo);

    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    // TODO: cleanup statics
    //etchvf_free_builtins()

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_constructor_2
 * verify that memory is freed as expected
 */
static void test_constructor_2(void)
{
    default_value_factory*  vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    binary_tagged_data_output* tdo          = NULL;
    etch_flexbuffer*        fbuf            = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    fbuf = new_flexbuffer(0);
    CU_ASSERT_PTR_NOT_NULL(fbuf);

    /* since we pass in vf and buf, we retain ownership */
    tdo = new_binary_tagdata_output((etch_value_factory*)vf, fbuf);
    CU_ASSERT_PTR_NOT_NULL(tdo);

    etch_object_destroy(fbuf);
    // destroy vf
    etch_object_destroy(vf);
    
    etch_object_destroy(tdo);

    // TODO: cleanup statics
    //etchvf_free_builtins()


#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_check_value()
 */
static void test_check_value(void)
{
    int  i = 0;
    signed char typecode = 0;
    binary_tagged_data_output* tdo = new_binary_tagdata_output(NULL, NULL);

    do  /* check byte values */
    {   signed char *testbytes = get_test_bytes(ETCHTYPE_MIN_BYTE, 256), c = 0;
        etch_byte* byteobj = new_byte(0);

        for(i = 0; i < 256; i++)
        {
            byteobj->value = c = testbytes[i];
            typecode = etchtagdata_check_value((etch_object*) byteobj);

            if (is_inrange_tiny_for_signed_chars(c)) 
               { CU_ASSERT_EQUAL(typecode, c);  }
            else
               { CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_BYTE); }
        }

        free(testbytes);
        etch_object_destroy(byteobj);

    } while(0);

    do  /* check short values */
    {   short *testshorts = get_test_shorts(ETCHTYPE_MIN_INT16, 65536), n = 0;
        etch_int16* shortobj = new_int16(0);

        for(i = 0; i < 65536; i++)
        {                             
            shortobj->value = n = testshorts[i];
            typecode = etchtagdata_check_value((etch_object*) shortobj);

            if (is_inrange_tiny(n)) 
               { CU_ASSERT_EQUAL(typecode, (signed char) n);  }
            else
            if (is_inrange_byte(n))
               { CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_BYTE); }
            else
               { CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_SHORT); }
        }

        free(testshorts);
        etch_object_destroy(shortobj);

    } while(0);

    do  /* check int values */
    {   int intcount = 0, *testints = get_test_ints(&intcount), n = 0;
        etch_int32* intobj = new_int32(0);

        for(i = 0; i < intcount; i++)
        {                             
            intobj->value = n = testints[i];
            typecode = etchtagdata_check_value((etch_object*) intobj);

            if (is_inrange_tiny(n)) 
               { CU_ASSERT_EQUAL(typecode, (signed char) n);  }
            else
            if (is_inrange_byte(n))
               { CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_BYTE); }
            else
            if (is_inrange_int16(n))
               { CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_SHORT); }
            else
               { CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_INT); }
        }

        free(testints);
        etch_object_destroy(intobj);

    } while(0);

    do  /* check long values */
    {   int   longcount = 0;
        int64 *testlongs = get_test_longs(&longcount), n = 0;
        etch_int64* longobj = new_int64(0);

        for(i = 0; i < longcount; i++)
        {                             
            longobj->value = n = testlongs[i];
            typecode = etchtagdata_check_value((etch_object*) longobj);

            if (is_inrange_tiny(n)) 
               { CU_ASSERT_EQUAL(typecode, (signed char) n);  }
            else
            if (is_inrange_byte(n))
               { CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_BYTE); }
            else
            if (is_inrange_int16(n))
               { CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_SHORT); }
            else
            if (is_inrange_int32(n))
               { CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_INT); }
            else
               { CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_LONG); }
        }

        free(testlongs);
        etch_object_destroy(longobj);

    } while(0);

    typecode = etchtagdata_check_value(NULL);
    CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_NULL);

    do {  /* check boolean values */
        etch_boolean* boolobj = new_boolean(FALSE);
        typecode = etchtagdata_check_value((etch_object*)boolobj);
        CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_BOOLEAN_FALSE);
        boolobj->value = TRUE;
        typecode = etchtagdata_check_value((etch_object*)boolobj);
        CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE);
        etch_object_destroy(boolobj);
    } while(0);

    do {  /* check ieee values */
    etch_float* floatobj = new_float((float) 3.14159);
    etch_double* doubobj = new_double(3.14159);
    typecode = etchtagdata_check_value((etch_object*)floatobj);
    CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_FLOAT);
    typecode = etchtagdata_check_value((etch_object*)doubobj);
    CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_DOUBLE);
    etch_object_destroy(floatobj);
    etch_object_destroy(doubobj);
    } while(0);

    do {  /* check bytes */
    etch_nativearray* bytearray = new_etch_nativearray(CLASSID_ARRAY_BYTE, sizeof(byte), 1, 4, 0, 0);  
    etch_arrayvalue*  arrayvalu = new_arrayvalue(ETCH_XTRNL_TYPECODE_BYTES, NULL, 1, 0, 0, 0, 0);
    arrayvalu->content_obj_type = ETCHTYPEB_BYTE; 
    typecode = etchtagdata_check_value((etch_object*)bytearray);
    CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_BYTES);
    typecode = etchtagdata_check_value((etch_object*)arrayvalu);
    CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_BYTES);
    etch_object_destroy(bytearray);
    etch_object_destroy(arrayvalu);
    } while(0);

    do {  /* check string */
    etch_string* strempty = new_stringw(L"");
    etch_string* strother = new_stringw(L"x");
    typecode = etchtagdata_check_value((etch_object*)strempty);
    CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_EMPTY_STRING);
    typecode = etchtagdata_check_value((etch_object*)strother);
    CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_STRING);
    etch_object_destroy(strempty);
    etch_object_destroy(strother);
    } while(0);

    do {  /* check custom */
    etch_type* newtype = new_type(L"unknown");
    etch_date* newdate = new_date();
    etch_structvalue* sv = new_structvalue(newtype, 0);
    typecode = etchtagdata_check_value((etch_object*)sv);
    CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_CUSTOM); 
    typecode = etchtagdata_check_value((etch_object*)newdate);
    CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_CUSTOM); 
    etch_object_destroy(newdate);
    etch_object_destroy(newtype);
    etch_object_destroy(sv);
    } while(0);

    do {  /* check end of data marker */
    etch_object* eodobj = etchtagdata_new_eodmarker(FALSE);  
    typecode = etchtagdata_check_value((etch_object*)eodobj);
    CU_ASSERT_EQUAL(typecode, (signed char) ETCH_XTRNL_TYPECODE_NONE); 
    etch_object_destroy(eodobj);
    } while(0);

    etch_object_destroy(tdo);

    etchvf_free_builtins(); 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * message_to_bytes()
 * return the encoded bytes for a message.
 * caller relinquishes msg, assumes returned bytes.
 */
static byte* message_to_bytes(default_value_factory* vf, etch_message* msg, size_t* out)
{
    size_t bufcount = 0;
    byte* msgbytes = NULL;

    etch_flexbuffer* fbuf = new_flexbuffer(ETCH_DEFSIZE);
    binary_tagged_data_output* tdo = new_binary_tagdata_output(msg->vf, fbuf);

    if (0 == ((struct i_tagged_data_output*)((etch_object*)tdo)->vtab)->write_message((tagged_data_output*) tdo, msg, fbuf))
    {
        etch_flexbuf_set_index(fbuf, 0);
        msgbytes = etch_flexbuf_get_all(fbuf, &bufcount); 
    }
                                                                        
    etch_object_destroy(fbuf);
    etch_object_destroy(tdo);
    etch_object_destroy(msg);
             
    if (out) *out = bufcount;
    return msgbytes;
}


/**
 * bytes_to_message()
 * return the message constructed from the supplied bytes.
 * caller relinquishes bytes, assumes returned message.
 */
static etch_message* bytes_to_message(default_value_factory* vf, byte* bytes, const size_t bytecount)   
{           
    etch_flexbuffer* fbuf = new_flexbuffer_from(bytes, bytecount, bytecount, 0);

    binary_tagged_data_input* tdi = new_binary_tagdata_input((etch_value_factory*) vf);

    etch_message* msg = bintdi_read_message((tagged_data_input*) tdi, fbuf);

    etch_object_destroy(tdi);
    etch_object_destroy(fbuf);  /* flexbuf destroys bytes */
    return msg;
}


/**
 * is_equal_objects()
 * currently we don't have a convenient way to determine if objects are equal,
 * primarily because the default hashkey is generated from object address, and
 * we have to date omitted the equality function from out object definition.
 * it remains to be seen whether we need a general equality test, in practice
 * we may only need it for certain high level objects. so we'll wing it for now.
 */
static int is_equal_objects(etch_object* a, etch_object* b)
{
    if ((((etch_object*)a)->obj_type == b->obj_type) && (((etch_object*)a)->class_id == b->class_id)) 
        return TRUE;

    /* todo: tests for array type and content comparison here */

    return FALSE;
}


/*
 * show_value() - debug display for do_tdo_tdi_test()
 */
static void show_value(etch_object* obj)
{
    #if IS_DEBUG_CONSOLE

    char x[2] = {0,0};

    if (is_etch_byte(obj))  
    {   x[0] = ((etch_byte*)obj)->value;
        printf("  >>> '%s'\n", x);  
    }
    else 
    if (is_etch_arrayvalue(obj))
    {   etch_arrayvalue* av = (etch_arrayvalue*) obj;
        const int dim = av->dim;
        const int count = av->list->count;
        printf("$$$ array dim %d count %d\n", dim, count);  
    }
    fflush(stdout);

    #endif
}


/*
 * do_tdo_tdi_test()
*/
static etch_object* do_tdo_tdi_test(default_value_factory* vf, etch_object* thisobj, etch_validator* vtor)
{
    etch_message* thismsg = NULL, *resultmsg = NULL;
    etch_hashitem x, *thisitem = &x; 
    etch_object* returnobj = NULL;
    byte*  msgbytes  = NULL;
    size_t bytecount = 0;
    int    result = 0;
    memset(thisitem, 0, sizeof(thisitem));
    show_value(thisobj); 

    etchtype_clear_validator(mt_footype, mf_fookey);
    result = etchtype_put_validator (mt_footype, clone_field(mf_fookey), (etch_object*) vtor);
    CU_ASSERT_EQUAL(result, 0);
    /* protect caller's object so that temporary owners in this test can't destroy it */
    set_etchobj_static_all(thisobj); 

    thismsg = new_message(mt_footype, ETCH_DEFSIZE, (etch_value_factory*) vf);

    result  = etch_message_put(thismsg, clone_field(mf_fookey), thisobj);
    CU_ASSERT_EQUAL_FATAL(result, 0);

    /* serialize thismsg. relinquish thismsg, assume msgbytes */
    msgbytes = message_to_bytes(vf, thismsg, &bytecount); 
    CU_ASSERT_PTR_NOT_NULL(msgbytes);
    CU_ASSERT_NOT_EQUAL(bytecount, 0);

    /* deserialize msgbytes. relinquish msgbytes, assume resultmsg */
    resultmsg = bytes_to_message(vf, msgbytes, bytecount);  
    CU_ASSERT_PTR_NOT_NULL_FATAL(resultmsg);

    result = structvalue_is_type(resultmsg->sv, mt_footype);
    CU_ASSERT_EQUAL(result, TRUE);

    result = etch_message_size(resultmsg);
    CU_ASSERT_EQUAL(result, 1);

    result = etchmap_map_find(resultmsg->sv->items, (etch_object*) mf_fookey, &thisitem);
    CU_ASSERT_EQUAL(result, 0);

    /* we verified that the deserialized message contains caller's reconstituted object.
     * we want to return that object to caller so that it can be compared with the
     * original object; however before we return, we are going to destroy the message, 
     * and since the message owns its content, it will destroy the object we intend to
     * return, so we remove the object from the message before we destroy the message.
     * this call frees all hashtable memory associated with this key/value pair except
     * except for the value object itself, including the key.
     */
    if (result == 0)
        returnobj = etch_message_remove(resultmsg, mf_fookey);

    etch_object_destroy(resultmsg);
    clear_etchobj_static_all(thisobj); /* unprotect caller's object protected above */  
    return returnobj;
}


/**
 * do_recursive_test()
 * recurse input object creating message from all content from scalar up to top level
 * enclosing array. serialize and deserialize each message and verify that deserialized
 * object matches original.
 */
static void do_recursive_test(default_value_factory* vf, etch_object* obj, etch_validator* vtor)
{
    etch_object* thisobj = obj, *reconstituted_obj = NULL; 

    if (is_etch_arrayvalue(thisobj)) 
    {
        etch_iterator iterator;        
        arrayvalue_set_iterator((etch_arrayvalue*) thisobj, &iterator); 

        while(iterator.has_next(&iterator))
        {
            etch_object* subobj = iterator.current_value;

            do_recursive_test(vf, subobj, vtor->element_validator(vtor)); 

            iterator.next(&iterator);
        }
    }
    /* printf("\n(%05d. type %d class %d)\n", ++objcount, ((etch_object*)obj)->obj_type, ((etch_object*)obj)->class_id); fflush(stdout); */
   
    reconstituted_obj = do_tdo_tdi_test(vf, thisobj, vtor);  

    CU_ASSERT_PTR_NOT_NULL_FATAL(reconstituted_obj);
    CU_ASSERT_EQUAL(is_equal_objects(thisobj, reconstituted_obj), TRUE);     
    etch_object_destroy(reconstituted_obj);
}

#ifdef _WIN32
#pragma message ( "test_byte_array_out_in deactivated" )
#else
#warning "test_byte_array_out_in deactivated"
#endif

#if 0
/**
 * test_byte_array_out_in()
 * test serialization and subsequent deserialization of a byte array
 */
static void test_byte_array_out_in(void)
{
    const int NUMDIMS = 2;
    char  x[2][3] = { {'a','b','c', }, {'d','e','f', }, };
    default_value_factory*  vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    etch_nativearray*       natarray        = NULL;
    etch_arrayvalue*        arrayval        = NULL;
    etch_arrayvalue* reconstituted_arrayval = NULL;
    etch_validator*         validator       = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    natarray  = new_etch_nativearray_from(x, CLASSID_ARRAY_BYTE, sizeof(byte), NUMDIMS, 3, 2, 0);
    arrayval  = new_arrayvalue_from(natarray, ETCH_XTRNL_TYPECODE_BYTES, NULL, 2, 0, FALSE);
    validator = etchvtor_byte_get(NUMDIMS);

    this_test_setup();
    ((struct i_value_factory*)((etch_object*)vf)->vtab)->add_type(vf, mt_footype); /* the type we are testing with */
    objcount = arraycount = 0; /* initialize global counters */

    /* test serialization and deserialization of the array object as a whole. 
     * we are returned an object which is the reconstituted array after it has
     * been serialized and subsequently deserialized */
    reconstituted_arrayval = (etch_arrayvalue*) do_tdo_tdi_test
        (vf, (etch_object*) arrayval, validator);

    CU_ASSERT_EQUAL_FATAL(is_etch_arrayvalue(reconstituted_arrayval), TRUE);

    /* recursively test serialization and deserialization of all the components 
     * of the array object, including the array itself, subarrays, and scalars,
     * as per the java version of this test. this really ensures that we have 
     * all our memory management ducks in a row. */

    do_recursive_test (vf, (etch_object*) arrayval, validator);

    etch_object_destroy(arrayval);
    etch_object_destroy(reconstituted_arrayval);
   
    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    this_test_teardown();
    // TODO: cleanup statics
    //etchvf_free_builtins()

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}
#endif

/**
 * test_bool_array_out_in()
 * test serialization and subsequent deserialization of a boolean array
 */
static void test_bool_array_out_in(void)
{
    const int NUMDIMS = 2;
    boolean x[2][3] = { { TRUE, FALSE, TRUE, }, { FALSE, TRUE, FALSE, }, };
    default_value_factory*  vf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    etch_nativearray*       natarray        = NULL;
    etch_arrayvalue*        arrayval        = NULL;
    etch_arrayvalue* reconstituted_arrayval = NULL;
    etch_validator*         validator       = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    natarray  = new_etch_nativearray_from(x, CLASSID_ARRAY_BOOL, sizeof(boolean), NUMDIMS, 3, 2, 0);
    arrayval  = new_arrayvalue_from(natarray, ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE, NULL, 2, 0, FALSE);
    validator = etchvtor_boolean_get(NUMDIMS);

    this_test_setup();
    ((struct i_value_factory*)((etch_object*)vf)->vtab)->add_type(vf, mt_footype); /* the type we are testing with */
    objcount = arraycount = 0; /* initialize global counters */

    /* test serialization and deserialization of the array object as a whole. 
     * we are returned an object which is the reconstituted array after it has
     * been serialized and subsequently deserialized */
    reconstituted_arrayval = (etch_arrayvalue*) do_tdo_tdi_test(vf, (etch_object*) arrayval, validator);

    CU_ASSERT_EQUAL_FATAL(is_etch_arrayvalue(reconstituted_arrayval), TRUE);

    /* recursively test serialization and deserialization of all the components 
     * of the array object, including the array itself, subarrays, and scalars,
     * as per the java version of this test. this really ensures that we have 
     * all our memory management ducks in a row. */

    do_recursive_test (vf, (etch_object*) arrayval, validator);

    etch_object_destroy(arrayval);
    etch_object_destroy(reconstituted_arrayval);
   
    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    this_test_teardown();
    
    // TODO: cleanup statics
    //etchvf_free_builtins(

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_short_array_out_in()
 * test serialization and subsequent deserialization of a short integer array
 */
static void test_short_array_out_in(void)
{
    const int NUMDIMS = 2;
    short x[2][3] = { { 0, 1, 2, }, { -1, -32768, 32767, }, };
    default_value_factory*  vf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    etch_nativearray*       natarray        = NULL;
    etch_arrayvalue*        arrayval        = NULL;
    etch_arrayvalue* reconstituted_arrayval = NULL;
    etch_validator*         validator       = NULL;


    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    natarray  = new_etch_nativearray_from(x, CLASSID_ARRAY_INT16, sizeof(short), NUMDIMS, 3, 2, 0);
    arrayval  = new_arrayvalue_from(natarray, ETCH_XTRNL_TYPECODE_SHORT, NULL, 2, 0, FALSE);
    validator = etchvtor_int16_get(NUMDIMS);

    this_test_setup();
    ((struct i_value_factory*)((etch_object*)vf)->vtab)->add_type(vf, mt_footype); /* the type we are testing with */
    objcount = arraycount = 0; /* initialize global counters */

    /* test serialization and deserialization of the array object as a whole. 
     * we are returned an object which is the reconstituted array after it has
     * been serialized and subsequently deserialized */
    reconstituted_arrayval = (etch_arrayvalue*) do_tdo_tdi_test(vf, (etch_object*) arrayval, validator);

    CU_ASSERT_EQUAL_FATAL(is_etch_arrayvalue(reconstituted_arrayval), TRUE);

    /* recursively test serialization and deserialization of all the components 
     * of the array object, including the array itself, subarrays, and scalars,
     * as per the java version of this test. this really ensures that we have 
     * all our memory management ducks in a row. */

    do_recursive_test (vf, (etch_object*) arrayval, validator);

    etch_object_destroy(arrayval);
    etch_object_destroy(reconstituted_arrayval);
    
    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);

    this_test_teardown();

    // TODO: cleanup statics
    //etchvf_free_builtins()

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_int_array_out_in()
 * test serialization and subsequent deserialization of an int array
 */
static void test_int_array_out_in(void)
{
    const int NUMDIMS = 2;
    int x[2][3] = { { 0, 1, 2, }, { -1, ETCHTYPE_MIN_INT32, ETCHTYPE_MAX_INT32, }, };
    default_value_factory*  vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    etch_nativearray*       natarray        = NULL;
    etch_arrayvalue*        arrayval        = NULL;
    etch_arrayvalue* reconstituted_arrayval = NULL;
    etch_validator*         validator       = NULL;

    
     typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    natarray  = new_etch_nativearray_from(x, CLASSID_ARRAY_INT32, sizeof(int), NUMDIMS, 3, 2, 0);
    arrayval  = new_arrayvalue_from(natarray, ETCH_XTRNL_TYPECODE_INT, NULL, 2, 0, FALSE);
    validator = etchvtor_int32_get(NUMDIMS);

    this_test_setup();
    ((struct i_value_factory*)((etch_object*)vf)->vtab)->add_type(vf, mt_footype); /* the type we are testing with */
    objcount = arraycount = 0; /* initialize global counters */

    /* test serialization and deserialization of the array object as a whole. 
     * we are returned an object which is the reconstituted array after it has
     * been serialized and subsequently deserialized */
    reconstituted_arrayval = (etch_arrayvalue*) do_tdo_tdi_test(vf, (etch_object*) arrayval, validator);

    CU_ASSERT_EQUAL_FATAL(is_etch_arrayvalue(reconstituted_arrayval), TRUE);

    /* recursively test serialization and deserialization of all the components 
     * of the array object, including the array itself, subarrays, and scalars,
     * as per the java version of this test. this really ensures that we have 
     * all our memory management ducks in a row. */

    do_recursive_test (vf, (etch_object*) arrayval, validator);

    etch_object_destroy(arrayval);
    etch_object_destroy(reconstituted_arrayval);
   
    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    this_test_teardown();
    
    // TODO: cleanup statics
    //etchvf_free_builtins()

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_long_array_out_in()
 * test serialization and subsequent deserialization of a long array
 */
static void test_long_array_out_in(void)
{
    const int NUMDIMS = 2;
    int64 x[2][3] = { { 0, 1, 2, }, { -1, ETCHTYPE_MIN_INT64, ETCHTYPE_MAX_INT64, }, };
    default_value_factory*  vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    etch_nativearray*       natarray        = NULL;
    etch_arrayvalue*        arrayval        = NULL;
    etch_arrayvalue* reconstituted_arrayval = NULL;
    etch_validator*         validator       = NULL;
    
    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    natarray  = new_etch_nativearray_from(x, CLASSID_ARRAY_INT64, sizeof(int64), NUMDIMS, 3, 2, 0);
    arrayval  = new_arrayvalue_from(natarray, ETCH_XTRNL_TYPECODE_LONG, NULL, 2, 0, FALSE);
    validator = etchvtor_int64_get(NUMDIMS);

    this_test_setup();
    ((struct i_value_factory*)((etch_object*)vf)->vtab)->add_type(vf, mt_footype); /* the type we are testing with */
    objcount = arraycount = 0; /* initialize global counters */

    /* test serialization and deserialization of the array object as a whole. 
     * we are returned an object which is the reconstituted array after it has
     * been serialized and subsequently deserialized */
    reconstituted_arrayval = (etch_arrayvalue*) do_tdo_tdi_test(vf, (etch_object*) arrayval, validator);

    CU_ASSERT_EQUAL_FATAL(is_etch_arrayvalue(reconstituted_arrayval), TRUE);

    /* recursively test serialization and deserialization of all the components 
     * of the array object, including the array itself, subarrays, and scalars,
     * as per the java version of this test. this really ensures that we have 
     * all our memory management ducks in a row. */

    do_recursive_test (vf, (etch_object*) arrayval, validator);

    etch_object_destroy(arrayval);
    etch_object_destroy(reconstituted_arrayval);
   
    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);

    this_test_teardown();

    // TODO: cleanup statics
    //etchvf_free_builtins()

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_float_array_out_in()
 * test serialization and subsequent deserialization of a float array
 */
static void test_float_array_out_in(void)
{
    const int NUMDIMS = 2;
    float x[2][3] = { { 0.0, 1.0, 2.0, }, { -1.0, ETCHTYPE_MIN_FLOAT, ETCHTYPE_MAX_FLOAT, }, };
    default_value_factory*  vf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    etch_nativearray*       natarray        = NULL;
    etch_arrayvalue*        arrayval        = NULL;
    etch_arrayvalue* reconstituted_arrayval = NULL;
    etch_validator*         validator       = NULL;

     typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    natarray  = new_etch_nativearray_from(x, CLASSID_ARRAY_FLOAT, sizeof(float), NUMDIMS, 3, 2, 0);
    arrayval  = new_arrayvalue_from(natarray, ETCH_XTRNL_TYPECODE_FLOAT, NULL, 2, 0, FALSE);
    validator = etchvtor_float_get(NUMDIMS);

    this_test_setup();
    ((struct i_value_factory*)((etch_object*)vf)->vtab)->add_type(vf, mt_footype); /* the type we are testing with */
    objcount = arraycount = 0; /* initialize global counters */

    /* test serialization and deserialization of the array object as a whole. 
     * we are returned an object which is the reconstituted array after it has
     * been serialized and subsequently deserialized */
    reconstituted_arrayval = (etch_arrayvalue*) do_tdo_tdi_test(vf, (etch_object*) arrayval, validator);

    CU_ASSERT_EQUAL_FATAL(is_etch_arrayvalue(reconstituted_arrayval), TRUE);

    /* recursively test serialization and deserialization of all the components 
     * of the array object, including the array itself, subarrays, and scalars,
     * as per the java version of this test. this really ensures that we have 
     * all our memory management ducks in a row. */

    do_recursive_test (vf, (etch_object*) arrayval, validator);

    etch_object_destroy(arrayval);
    etch_object_destroy(reconstituted_arrayval);
   
    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    this_test_teardown();
    
    // TODO: cleanup statics
    // etchvf_free_builtins()


#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_double_array_out_in()
 * test serialization and subsequent deserialization of a double array
 */
static void test_double_array_out_in(void)
{
    const int NUMDIMS = 2;
    double x[2][3] = { { 0.0, 1.0, 2.0, }, { -1.0, ETCHTYPE_MIN_DOUBLE, ETCHTYPE_MAX_DOUBLE, }, };
    default_value_factory*  vf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    etch_nativearray*       natarray        = NULL;
    etch_arrayvalue*        arrayval        = NULL;
    etch_arrayvalue* reconstituted_arrayval = NULL;
    etch_validator*         validator       = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    natarray  = new_etch_nativearray_from(x, CLASSID_ARRAY_DOUBLE, sizeof(double), NUMDIMS, 3, 2, 0);
    arrayval  = new_arrayvalue_from(natarray, ETCH_XTRNL_TYPECODE_DOUBLE, NULL, 2, 0, FALSE);
    validator = etchvtor_double_get(NUMDIMS);

    this_test_setup();
    ((struct i_value_factory*)((etch_object*)vf)->vtab)->add_type(vf, mt_footype); /* the type we are testing with */
    objcount = arraycount = 0; /* initialize global counters */

    /* test serialization and deserialization of the array object as a whole. 
     * we are returned an object which is the reconstituted array after it has
     * been serialized and subsequently deserialized */
    reconstituted_arrayval = (etch_arrayvalue*) do_tdo_tdi_test(vf, (etch_object*) arrayval, validator);

    CU_ASSERT_EQUAL_FATAL(is_etch_arrayvalue(reconstituted_arrayval), TRUE);

    /* recursively test serialization and deserialization of all the components 
     * of the array object, including the array itself, subarrays, and scalars,
     * as per the java version of this test. this really ensures that we have 
     * all our memory management ducks in a row. */

    do_recursive_test (vf, (etch_object*) arrayval, validator);

    etch_object_destroy(arrayval);
    etch_object_destroy(reconstituted_arrayval);
   
    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    this_test_teardown();
    
    // TODO: cleanup statics
    // etchvf_free_builtins()


#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


static etch_string* ns(int n) 
{ 
    wchar_t buf[32]; etch_snwprintf(buf, 32, L"%d", (size_t) n);
    return new_stringw(buf);
}


/**
 * test_string_array_out_in()
 * test serialization and subsequent deserialization of a string array
 */
static void test_string_array_out_in(void)
{
    const int NUMDIMS = 2;
    etch_string* x[2][3] = { { ns(0), ns(1), ns(2), }, { ns(3), ns(4), ns(5), }, };
    default_value_factory*  vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    etch_nativearray*       natarray        = NULL;
    etch_arrayvalue*        arrayval        = NULL;
    etch_arrayvalue* reconstituted_arrayval = NULL;
    etch_validator*         validator       = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);


    natarray  = new_etch_nativearray_from(x, CLASSID_ARRAY_STRING, sizeof(void*), NUMDIMS, 3, 2, 0);
    arrayval  = new_arrayvalue_from(natarray, ETCH_XTRNL_TYPECODE_STRING, NULL, 2, 0, FALSE);
    validator = etchvtor_string_get(NUMDIMS);

    this_test_setup();
    ((struct i_value_factory*)((etch_object*)vf)->vtab)->add_type(vf, mt_footype); /* the type we are testing with */
    objcount = arraycount = 0; /* initialize global counters */

    /* test serialization and deserialization of the array object as a whole. 
     * we are returned an object which is the reconstituted array after it has
     * been serialized and subsequently deserialized */
    reconstituted_arrayval = (etch_arrayvalue*) do_tdo_tdi_test(vf, (etch_object*) arrayval, validator);

    CU_ASSERT_EQUAL_FATAL(is_etch_arrayvalue(reconstituted_arrayval), TRUE);

    /* recursively test serialization and deserialization of all the components 
     * of the array object, including the array itself, subarrays, and scalars,
     * as per the java version of this test. this really ensures that we have 
     * all our memory management ducks in a row. */

    do_recursive_test (vf, (etch_object*) arrayval, validator);

    etch_object_destroy(arrayval);
    etch_object_destroy(reconstituted_arrayval);
   
    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    this_test_teardown();
    
    // TODO: cleanup statics
    // etchvf_free_builtins()

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


#ifdef _WIN32
#pragma message ( "test_add_perf deactivated" )
#else
#warning "test_add_perf deactivated"
#endif

#if 0
/**
 * test_add_perf()
 * test performance of add(x,y) message serialization and delivery
 */
static void test_add_perf(void)
{
    int result = 0;
    etch_who* whofrom = NULL;
    etch_message* msg = NULL;
    etch_messagizer* messagizer = NULL;
    i_transportpacket* my_packetsource = NULL;
    my_impl_transportpacket* my_impl_packetsource = NULL;
    default_value_factory*  vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    etch_type*  addtype = NULL;
    etch_field* xfield  = NULL;
     etch_field* yfield = NULL;
     etch_resources* resx = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);
    
    resx = new_etch_resources(ETCH_RESOURCES_DEFSIZE);

    addtype = new_type(L"add");
    xfield  = new_field(L"x");
    yfield  = new_field(L"y");

    etchtype_put_validator(addtype, clone_field(xfield), (etch_object*) etchvtor_int32_get(0));
    etchtype_put_validator(addtype, clone_field(yfield), (etch_object*) etchvtor_int32_get(0));
    etchtype_put_validator(addtype, clone_field(builtins._mf__message_id), (etch_object*) etchvtor_int64_get(0));

    msg = new_message(addtype, ETCH_DEFSIZE, (etch_value_factory*) vf);
    CU_ASSERT_PTR_NOT_NULL_FATAL(msg);
    etch_message_put(msg, clone_field(xfield), (etch_object*) new_int32(1));  /* relinquish all content */
    etch_message_put(msg, clone_field(yfield), (etch_object*) new_int32(2));
    etch_message_put(msg, clone_field(builtins._mf__message_id), (etch_object*) new_int64(0x0123456789abcdefLL));

    /* we relinquish vf to the resources destructor here */
    etch_resources_add(resx, ETCH_RESXKEY_MSGIZER_VALUFACT, (etch_object*) vf); 

    /* instantiate object which implements i_transportpacket and extract its interface.
     * the interface's destructor is coded to destroy its instantiating object. */
    my_impl_packetsource = new_my_impl_transportpacket();
    CU_ASSERT_PTR_NOT_NULL_FATAL(my_impl_packetsource);
    my_packetsource = my_impl_packetsource->ixp; /* extract interface */
    CU_ASSERT_PTR_NOT_NULL_FATAL(my_packetsource); 

    messagizer = new_messagizer(my_packetsource, L"foo:?Messagizer.format=binary", resx);
    CU_ASSERT_PTR_NOT_NULL_FATAL(messagizer);  

    /* we need to continue to use the message in this test, so since the message's
     * destructor will be called by messagizer.transport_message(), mark the message
     * static such that it is protected from destruction.
     */
    set_etchobj_static_all(msg); 

    /* we ordinarily relinquish msg on success, retain it on failure;  
     * however here we have retained it regardless, since we marked it  
     * static above, thus disabling its destructor. 
     */
    result = messagizer->transport_message(messagizer, whofrom, msg);
    CU_ASSERT_EQUAL(result, 0);  

    if (0 == result)
    {   int i = 0;
        const int n = 900973; /* this code is ported from the java test */

        for(i = 0; i < 3; i++)
            do_perftest("test_add_perf", i, messagizer, msg, n);
    }

    clear_etchobj_static_all(msg);  /* clear static state so we can destroy it now */
    etch_object_destroy(msg);

    etch_object_destroy(xfield);
    etch_object_destroy(yfield);
    etch_object_destroy(addtype);

    etch_object_destroy(resx);  /* content is destroyed also (vf in this case) */
    etch_object_destroy(messagizer);
    etch_object_destroy(my_packetsource); /* destroys my_impl_transportpacket also */
    
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    // TODO: cleanup statics
    //etchvf_free_builtins()

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}
#endif

/**
 * get_bytearray()
 * support for test_value_to_bytes().
 * return an etch_nativearray wrapping the supplied static byte* array.
 */
static etch_nativearray* get_bytearray(byte* bytes, const int bytecount)
{ 
    etch_nativearray* na = new_etch_nativearray_from(bytes, CLASSID_ARRAY_BYTE, sizeof(byte), 1, bytecount, 0, 0);  
    na->content_obj_type = ETCHTYPEB_BYTES;
    na->content_class_id = CLASSID_NONE; /* unwrapped */
    return na;
}

 
 /**
 * assert_value_to_bytes()
 * support for test_value_to_bytes() -- serialize value object, and assert 
 * that the serialized bytes match the supplied expected byte array.
 */
static int assert_value_to_bytes(etch_object* value, etch_nativearray* expected_bytes, etch_value_factory* vf)
{
    byte* buf = NULL;
    int  bytecount = 0, i = 0, result = 0, errs = 0;
    etch_flexbuffer* fbuf = new_flexbuffer(ETCH_DEFSIZE);
    etch_validator*  vtor = etchvtor_object_get(0);

    binary_tagged_data_output* tdo = new_binary_tagdata_output(vf, fbuf);

    /* serialize the specified value object to the buffer fbuf */
    result = bintdo_write_value(tdo, vtor, value);
    CU_ASSERT_EQUAL(result, 0);
    if (0 != result) errs++;

    if (0 == result)
    {
        fbuf->index = 0; /* get the serialized bytes to a new buffer */
        buf = etch_flexbuf_get_all(fbuf, (size_t*) &bytecount); 

	printf("%d <-> %d\n", bytecount, (int)expected_bytes->bytecount);
        CU_ASSERT_EQUAL(bytecount, expected_bytes->bytecount);
        if (bytecount != expected_bytes->bytecount) errs++;

        if (bytecount == expected_bytes->bytecount)
        {   /* verify that serialized bytes match expected bytes  */
            for(i = 0; i < bytecount; i++)
            {   signed char a = buf[i], b = 0;
                expected_bytes->get1(expected_bytes, &b, i);
                result = ((int) a == (int) b);
                CU_ASSERT_EQUAL(result, TRUE);
                if (!result) 
                    errs++;
            }
        }
    }

    /* free memory */
    etch_object_destroy(value);
    etch_object_destroy(expected_bytes);
    etch_object_destroy(tdo);
    etch_object_destroy(fbuf);
    etch_free(buf);

    return errs? -1: 0;
}


/**
 * test_single_value_to_bytes()
 * test that various value objects serialized by the tdo match hard-coded expected byte arrays
 */
static void test_single_value_to_bytes(void)
{
    int  result = 0;
    etch_value_factory*  vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = (etch_value_factory*)new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);
    
    do  /* NULL */
    {   signed char a[] = { ETCH_XTRNL_TYPECODE_NULL }; 
        result = assert_value_to_bytes(NULL, get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    do  /* boolean primitive objects */
    {   signed char a[] = { ETCH_XTRNL_TYPECODE_BOOLEAN_FALSE }; 
        result = assert_value_to_bytes((etch_object*) new_boolean(FALSE), get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    do 
    {   signed char a[] = { ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE }; 
        result = assert_value_to_bytes((etch_object*) new_boolean(TRUE), get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    do  /* some tiny integers */
    {   signed char a[] = { 0 }; 
        result = assert_value_to_bytes((etch_object*) new_byte(0), get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    do   
    {   signed char a[] = { 1 }; 
        result = assert_value_to_bytes((etch_object*) new_byte(1), get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    do   
    {   signed char a[] = { -1 }; 
        result = assert_value_to_bytes((etch_object*) new_byte(-1), get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    do  /* non-tinyint byte */
    {   signed char a[] = { ETCH_XTRNL_TYPECODE_BYTE, -100 }; 
        result = assert_value_to_bytes((etch_object*) new_byte(-100), get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    do  /* some short integers */
    {   signed char a[] = { ETCH_XTRNL_TYPECODE_SHORT, 39, 16 }; 
        result = assert_value_to_bytes((etch_object*) new_int16(10000), get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    do   
    {   signed char a[] = { ETCH_XTRNL_TYPECODE_SHORT, -40, -16 }; 
        result = assert_value_to_bytes((etch_object*) new_int16(-10000), get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    do  /* 32-bit integers */
    {   signed char a[] = { ETCH_XTRNL_TYPECODE_INT, 59, -102, -54, 0 }; 
        result = assert_value_to_bytes((etch_object*) new_int32(1000000000), get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    do   
    {   signed char a[] = { ETCH_XTRNL_TYPECODE_INT, -60, 101, 54, 0 }; 
        result = assert_value_to_bytes((etch_object*) new_int32(-1000000000), get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    do  /* long integers */
    {   signed char a[] = { ETCH_XTRNL_TYPECODE_LONG, 13, -32, -74, -77, -89, 100, 0, 0 }; 
        result = assert_value_to_bytes((etch_object*) new_int64(1000000000000000000LL), get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    do   
    {   signed char a[] = { ETCH_XTRNL_TYPECODE_LONG, -14, 31, 73, 76, 88, -100, 0, 0 }; 
        result = assert_value_to_bytes((etch_object*) new_int64(-1000000000000000000LL), get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    do  /* float */
    {   signed char a[] = { ETCH_XTRNL_TYPECODE_FLOAT, 70, 64, -28, 0 }; 
        result = assert_value_to_bytes((etch_object*) new_float(12345.0), get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    do  /* double */
    {   signed char a[] = { ETCH_XTRNL_TYPECODE_DOUBLE, 64, -42, -24, 0, 0, 0, 0, 0 }; 
        result = assert_value_to_bytes((etch_object*) new_double(23456.0), get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);
    } while(0);

    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    // TODO: cleanup statics
    //etchvf_free_builtins()

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_boolean_array_value_to_bytes()
 * test that various value objects serialized by the tdo match hard-coded expected byte arrays
 */
static void test_boolean_array_value_to_bytes(void)
{
    int  result = 0;
    etch_value_factory*     vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = (etch_value_factory*)new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    do  /* boolean[] */
    {   const int NUMDIMS1 = 1, ITEMCOUNT = 2;

        signed char a[] =
        { ETCH_XTRNL_TYPECODE_ARRAY, /* array type */
          ETCH_XTRNL_TYPECODE_BOOLEAN_CONTENT, /* array content type */
          NUMDIMS1,  /* dimensions count */
          ITEMCOUNT, /* count of content items following */
          ETCH_XTRNL_TYPECODE_BOOLEAN_TRUE,   /* item[0] */
          ETCH_XTRNL_TYPECODE_BOOLEAN_FALSE,  /* item[1] */
          ETCH_XTRNL_TYPECODE_NONE /* eod mark */
        }; 

        etch_arrayvalue* av = new_arrayvalue(ETCH_XTRNL_TYPECODE_BOOLEAN_CONTENT, 
            NULL, NUMDIMS1, ITEMCOUNT, 0, FALSE, FALSE);

        arrayvalue_add(av, (etch_object*) new_boolean(TRUE));
        arrayvalue_add(av, (etch_object*) new_boolean(FALSE));
 
        result = assert_value_to_bytes((etch_object*) av, get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);

    } while(0);
    
    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    // TODO: cleanup statics
    //etchvf_free_builtins()

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_byte_array_value_to_bytes()
 * test that various value objects serialized by the tdo match hard-coded expected byte arrays
 */
static void test_byte_array_value_to_bytes(void)
{
    int  result = 0;
    etch_value_factory*     vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = (etch_value_factory*)new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    do  /* byte[] */
    {   const int ITEMCOUNT = 3;
        signed char content[] = { 1, 2, 3 };

        signed char a[] =  /* byte vector is a special case, not an array type */
        { ETCH_XTRNL_TYPECODE_BYTES, /* array content type */
          ITEMCOUNT,  /* count of items following */
          1,   /* item[0] */
          2,   /* item[1] */
          3    /* item[2] */
        }; 

        etch_nativearray* expected = get_bytearray((byte*)a, sizeof(a));

        /* memory management notes: here we create a nativearray na from static
         * content. new_etch_nativearray_from() defaults nativearray.is_content_owned   
         * to false, so when the nativearray na is destroyed, it will not attempt 
         * to free memory for the static content bytes. we then create an arrayvalue 
         * from nativearray na, specifying FALSE for parameter 6, is_readonly. the  
         * arrayvalue destructor will as a result destroy the content nativearray, 
         * which will in turn *not* destroy its content bytearray. finally, we   
         * relinquish the arrayvalue to assert_value_to_bytes(), which calls the 
         * arrayvalue destructor, causing the above to occur.
         */
        etch_nativearray* na = new_etch_nativearray_from(content, CLASSID_ARRAY_BYTE, sizeof(byte), 1, ITEMCOUNT, 0, 0); 
        etch_arrayvalue*  av = new_arrayvalue_from(na, ETCH_XTRNL_TYPECODE_BYTE, NULL, ITEMCOUNT, 0, FALSE);
      
        result = assert_value_to_bytes((etch_object*) av, expected, vf);
        CU_ASSERT_EQUAL(result,0);

    } while(0);

    do  /* byte[][] */
    {   
        #define NUMDIMS2   2
        #define ITEMCOUNT2 2
        #define ITEMCOUNT3 3
        signed char content[NUMDIMS2][ITEMCOUNT3] = { { 1, 2, 3, }, { 4, 5, 6, }, };

        signed char a[] =    /* array[2] of byte vectors */
        { ETCH_XTRNL_TYPECODE_ARRAY, /* value type array */    
          ETCH_XTRNL_TYPECODE_BYTE,  /* array content type */  
          NUMDIMS2,                  /* dimensions count */
          ITEMCOUNT2,                /* item count in dim[1] */
	      ETCH_XTRNL_TYPECODE_BYTES, /* byte vector follows */
          ITEMCOUNT3,                /* count of items following */
          1,                         /* item[0][0] */
          2,                         /* item[0][1] */
          3,                         /* item[0][2] */          
	      ETCH_XTRNL_TYPECODE_BYTES, /* byte vector follows */
          ITEMCOUNT3,                /* count of items following */
          4,                         /* item[1][0] */
          5,                         /* item[1][1] */
          6,                         /* item[1][2] */             
          ETCH_XTRNL_TYPECODE_NONE   /* eod mark */
        }; 

        etch_nativearray* expected = get_bytearray((byte*)a, sizeof(a));

        /* memory management notes: here we create a nativearray na from static
         * content. new_etch_nativearray_from() defaults nativearray.is_content_owned   
         * to false, so when the nativearray na is destroyed, it will not attempt 
         * to free memory for the static content bytes. we then create an arrayvalue 
         * from nativearray na, specifying FALSE for parameter 6, is_readonly. the  
         * arrayvalue destructor will as a result destroy the content nativearray, 
         * which will in turn *not* destroy its content bytearray. finally, we   
         * relinquish the arrayvalue to assert_value_to_bytes(), which calls the 
         * arrayvalue destructor, causing the above to occur.
         */
        etch_nativearray* na = new_etch_nativearray_from(content, CLASSID_ARRAY_BYTE, 
            sizeof(byte), NUMDIMS2, ITEMCOUNT3, ITEMCOUNT2, 0); 

        /* note here that the arrayvalue content type (ETCH_XTRNL_TYPECODE_BYTE)
         * is the content type serialized, so it must be the same as a[1].
         * the serialized type of arrays of byte vectors is BYTE, as defined by the 
         * tagged data spec, and not BYTES, which one might expect
         */
        etch_arrayvalue*  av = new_arrayvalue_from(na, ETCH_XTRNL_TYPECODE_BYTE, 
            NULL, ETCH_DEFSIZE, 0, FALSE);
      
        result = assert_value_to_bytes((etch_object*) av, expected, vf);
        CU_ASSERT_EQUAL(result,0);

    } while(0);

    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    // TODO: cleanup statics
    //etchvf_free_builtins()

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_short_array_value_to_bytes()
 * test that various value objects serialized by the tdo match hard-coded expected byte arrays
 */
static void test_short_array_value_to_bytes(void)
{
    int  result = 0;
    etch_value_factory*     vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = (etch_value_factory*)new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    do  /* short[] */
    {   const int NUMDIMS1 = 1, ITEMCOUNT = 3;

        signed char a[] =
        { ETCH_XTRNL_TYPECODE_ARRAY, /* array type */
          ETCH_XTRNL_TYPECODE_SHORT, /* array content type */
          NUMDIMS1,  /* dimensions count */
          ITEMCOUNT, /* count of content items following */
          1,   /* item[0] */
          2,   /* item[1] */
          3,   /* item[2] */
          ETCH_XTRNL_TYPECODE_NONE  /* eod mark */
        }; 

        etch_arrayvalue* av = new_arrayvalue(ETCH_XTRNL_TYPECODE_SHORT, 
            NULL, NUMDIMS1, ITEMCOUNT, 0, FALSE, FALSE);

        arrayvalue_add(av, (etch_object*) new_int16(1));
        arrayvalue_add(av, (etch_object*) new_int16(2));
        arrayvalue_add(av, (etch_object*) new_int16(3));
 
        result = assert_value_to_bytes((etch_object*) av, get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);

    } while(0);

    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    // TODO: cleanup statics
    //etchvf_free_builtins()

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_int_array_value_to_bytes()
 * test that various value objects serialized by the tdo match hard-coded expected byte arrays
 */
static void test_int_array_value_to_bytes(void)
{
    int  result = 0;
    etch_value_factory*     vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = (etch_value_factory*)new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);


    do  /* short[] */
    {   const int NUMDIMS1 = 1, ITEMCOUNT = 3;

        signed char a[] =
        { ETCH_XTRNL_TYPECODE_ARRAY, /* array type */
          ETCH_XTRNL_TYPECODE_INT,   /* array content type */
          NUMDIMS1,  /* dimensions count */
          ITEMCOUNT, /* count of content items following */
          1,   /* item[0] */
          2,   /* item[1] */
          3,   /* item[2] */
          ETCH_XTRNL_TYPECODE_NONE  /* eod mark */
        }; 

        etch_arrayvalue* av = new_arrayvalue(ETCH_XTRNL_TYPECODE_INT, 
            NULL, NUMDIMS1, ITEMCOUNT, 0, FALSE, FALSE);

        arrayvalue_add(av, (etch_object*) new_int32(1));
        arrayvalue_add(av, (etch_object*) new_int32(2));
        arrayvalue_add(av, (etch_object*) new_int32(3));
 
        result = assert_value_to_bytes((etch_object*) av, get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);

    } while(0);

    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    // TODO: cleanup statics
    //etchvf_free_builtins() 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_long_array_value_to_bytes()
 * test that various value objects serialized by the tdo match hard-coded expected byte arrays
 */
static void test_long_array_value_to_bytes(void)
{
    int  result = 0;
    etch_value_factory*     vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = (etch_value_factory*)new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    do  /* short[] */
    {   const int NUMDIMS1 = 1, ITEMCOUNT = 3;

        signed char a[] =
        { ETCH_XTRNL_TYPECODE_ARRAY, /* array type */
          ETCH_XTRNL_TYPECODE_LONG,  /* array content type */
          NUMDIMS1,  /* dimensions count */
          ITEMCOUNT, /* count of content items following */
          1,   /* item[0] */
          2,   /* item[1] */
          3,   /* item[2] */
          ETCH_XTRNL_TYPECODE_NONE  /* eod mark */
        }; 

        etch_arrayvalue* av = new_arrayvalue(ETCH_XTRNL_TYPECODE_LONG, 
            NULL, NUMDIMS1, ITEMCOUNT, 0, FALSE, FALSE);

        arrayvalue_add(av, (etch_object*) new_int64(1));
        arrayvalue_add(av, (etch_object*) new_int64(2));
        arrayvalue_add(av, (etch_object*) new_int64(3));
 
        result = assert_value_to_bytes((etch_object*) av, get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);

    } while(0);

    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    // TODO: cleanup statics
    //etchvf_free_builtins() 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_float_array_value_to_bytes()
 * test that various value objects serialized by the tdo match hard-coded expected byte arrays
 */
static void test_float_array_value_to_bytes(void)
{
    int  result = 0;
    etch_value_factory*     vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = (etch_value_factory*)new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);


    do  /* short[] */
    {   const int NUMDIMS1 = 1, ITEMCOUNT = 3;

        signed char a[] =
        { ETCH_XTRNL_TYPECODE_ARRAY, /* array type */
          ETCH_XTRNL_TYPECODE_FLOAT, /* array content type */
          NUMDIMS1,  /* dimensions count */
          ITEMCOUNT, /* count of content items following */
          ETCH_XTRNL_TYPECODE_FLOAT, 63,-128, 0, 0,   /* item[0] */
          ETCH_XTRNL_TYPECODE_FLOAT, 64,   0, 0, 0,   /* item[1] */
          ETCH_XTRNL_TYPECODE_FLOAT, 64,  64, 0, 0,   /* item[2] */
          ETCH_XTRNL_TYPECODE_NONE  /* eod mark */
        }; 

        etch_arrayvalue* av = new_arrayvalue(ETCH_XTRNL_TYPECODE_FLOAT, 
            NULL, NUMDIMS1, ITEMCOUNT, 0, FALSE, FALSE);

        arrayvalue_add(av, (etch_object*) new_float(1.0));
        arrayvalue_add(av, (etch_object*) new_float(2.0));
        arrayvalue_add(av, (etch_object*) new_float(3.0));
 
        result = assert_value_to_bytes((etch_object*) av, get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);

    } while(0);

    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    // TODO: cleanup statics
    //etchvf_free_builtins() 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_double_array_value_to_bytes()
 * test that various value objects serialized by the tdo match hard-coded expected byte arrays
 */
static void test_double_array_value_to_bytes(void)
{
    int  result = 0;
    etch_value_factory*     vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = (etch_value_factory*)new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    do  /* short[] */
    {   const int NUMDIMS1 = 1, ITEMCOUNT = 3;

        signed char a[] =
        { ETCH_XTRNL_TYPECODE_ARRAY, /* array type */
          ETCH_XTRNL_TYPECODE_DOUBLE, /* array content type */
          NUMDIMS1,  /* dimensions count */
          ITEMCOUNT, /* count of content items following */
          ETCH_XTRNL_TYPECODE_DOUBLE, 63,-16, 0, 0,  0, 0, 0, 0,  /* item[0] */
          ETCH_XTRNL_TYPECODE_DOUBLE, 64,  0, 0, 0,  0, 0, 0, 0,  /* item[1] */
          ETCH_XTRNL_TYPECODE_DOUBLE, 64,  8, 0, 0,  0, 0, 0, 0,  /* item[2] */
          ETCH_XTRNL_TYPECODE_NONE  /* eod mark */
        }; 

        etch_arrayvalue* av = new_arrayvalue(ETCH_XTRNL_TYPECODE_DOUBLE, 
            NULL, NUMDIMS1, ITEMCOUNT, 0, FALSE, FALSE);

        arrayvalue_add(av, (etch_object*) new_double(1.0));
        arrayvalue_add(av, (etch_object*) new_double(2.0));
        arrayvalue_add(av, (etch_object*) new_double(3.0));
 
        result = assert_value_to_bytes((etch_object*)av, get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);

    } while(0);

    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    // TODO: cleanup statics
    //etchvf_free_builtins() 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_string_values_to_bytes()
 * test that various value objects serialized by the tdo match hard-coded expected byte arrays
 */
static void test_string_values_to_bytes(void)
{
    int  result = 0;
    etch_value_factory*     vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = (etch_value_factory*)new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    printf("1\n");
    
    do
    {   signed char a[] = { ETCH_XTRNL_TYPECODE_EMPTY_STRING }; 

        result = assert_value_to_bytes((etch_object*) new_stringw(L""), 
            get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);

    } while(0);

    printf("2\n");

    do  /* string */
    {   /* note here that our test object is a unicode string, while our expected byte 
         * array encodes an array of 8-bit characters. this is because we carry all
         * strings internally as unicode, but the tdo converts strings to the configured 
         * wire encoding format, which is utf-8 at this writing. 
         */ 
        const int STRLEN = 3;
        signed char a[] = { ETCH_XTRNL_TYPECODE_STRING, STRLEN, 97, 98, 99 }; 

        result = assert_value_to_bytes((etch_object*) new_stringw(L"abc"), 
            get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);

    } while(0);
    printf("3\n");

    do  /* string[] */
    {   const int NUMDIMS1 = 1, ITEMCOUNT = 3, STRLEN1 = 1;

        signed char a[] =
        { ETCH_XTRNL_TYPECODE_ARRAY,  /* array type */
          ETCH_XTRNL_TYPECODE_STRING, /* array content type */
          NUMDIMS1,  /* dimensions count */
          ITEMCOUNT, /* count of content items following */
          ETCH_XTRNL_TYPECODE_STRING, STRLEN1, 97,  /* item[0] */
          ETCH_XTRNL_TYPECODE_EMPTY_STRING,         /* item[1] */
          ETCH_XTRNL_TYPECODE_STRING, STRLEN1, 99,  /* item[2] */
          ETCH_XTRNL_TYPECODE_NONE    /* eod mark */
        }; 

        etch_arrayvalue* av = new_arrayvalue(ETCH_XTRNL_TYPECODE_STRING, 
            NULL, NUMDIMS1, ITEMCOUNT, 0, FALSE, FALSE);

        arrayvalue_add(av, (etch_object*) new_stringw(L"a"));
        arrayvalue_add(av, (etch_object*) new_stringw(L""));
        arrayvalue_add(av, (etch_object*) new_stringw(L"c"));
 
        result = assert_value_to_bytes((etch_object*)av, get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);

    } while(0);
    printf("4\n");

    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    // TODO: cleanup statics
    //etchvf_free_builtins() 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_date_values_to_bytes()
 * test that various value objects serialized by the tdo match hard-coded expected byte arrays
 */
static void test_date_values_to_bytes(void)
{
    int  result = 0;
    etch_value_factory*     vf              = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    vf = (etch_value_factory*)new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);

    do  /* date */
    {   const int64 BOGUSDATEVAL = 1234567890;

        signed char a[] = 
        { ETCH_XTRNL_TYPECODE_CUSTOM,   /* indicates struct follows */
          ETCH_XTRNL_TYPECODE_INT, 43, 57, 107, -52, /* _mt__etch_datetime type id */
          1,                            /* struct member count */
          ETCH_XTRNL_TYPECODE_INT, 102, 0,  26,  64, /* "dateTime" field id */
          ETCH_XTRNL_TYPECODE_INT, 73, -106, 2, -46, /* serialized 1234567890 */
          ETCH_XTRNL_TYPECODE_NONE      /* eod mark */
        }; 

        etch_date* testdate = new_date();
        testdate->value = (time_t) BOGUSDATEVAL;

        result = assert_value_to_bytes((etch_object*)testdate, get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);

    } while(0);

    do  /* date[] */
    {   const int NUMDIMS1 = 1, ITEMCOUNT1 = 1; 
        const int64 BOGUSDATEVAL1 = 1234567890LL;
        const int64 BOGUSDATEVAL2 = 2345678901LL;

        signed char a[] =
        { ETCH_XTRNL_TYPECODE_ARRAY,  /* array type */
          ETCH_XTRNL_TYPECODE_CUSTOM, /* array content type */
          ETCH_XTRNL_TYPECODE_INT, 43, 57, 107, -52, /* "custom struct type" _mt__etch_datetime */
          NUMDIMS1,   /* array dimensions */
          ITEMCOUNT2, /* count of array content items following */  

          ETCH_XTRNL_TYPECODE_CUSTOM, /* start array member[0] */
          ETCH_XTRNL_TYPECODE_INT, 43, 57, 107, -52, /* struct type _mt__etch_datetime */
          ITEMCOUNT1,                 /* struct item count */
          ETCH_XTRNL_TYPECODE_INT, 102, 0,  26,  64, /* "dateTime" field id */
          ETCH_XTRNL_TYPECODE_INT, 73, -106, 2, -46, /* serialized 1234567890 */     
          ETCH_XTRNL_TYPECODE_NONE,   /* eod mark for struct - end of array member[0] */

          ETCH_XTRNL_TYPECODE_CUSTOM, /* start array member[1] */
          ETCH_XTRNL_TYPECODE_INT, 43, 57, 107, -52, /* struct type _mt__etch_datetime */
          ITEMCOUNT1,                 /* struct item count */
          ETCH_XTRNL_TYPECODE_INT, 102, 0,  26,  64, /* "dateTime" field id */
          ETCH_XTRNL_TYPECODE_LONG,  0, 0, 0, 0,  -117, -48, 56, 53, /* serialized 2345678901 */     
          ETCH_XTRNL_TYPECODE_NONE,   /* eod mark for struct - end of array member[1] */

          ETCH_XTRNL_TYPECODE_NONE,   /* eod mark for array */
        }; 

        etch_arrayvalue* av = NULL;
        etch_type* my_custom_struct_type = builtins._mt__etch_datetime;

        etch_date* date1 = new_date();
        etch_date* date2 = new_date();
        date1->value = BOGUSDATEVAL1;
        date2->value = BOGUSDATEVAL2;

        av = new_arrayvalue(ETCH_XTRNL_TYPECODE_CUSTOM, 
             my_custom_struct_type, NUMDIMS1, ITEMCOUNT2, 0, FALSE, FALSE);

        arrayvalue_add(av, (etch_object*) date1);
        arrayvalue_add(av, (etch_object*) date2);
 
        result = assert_value_to_bytes((etch_object*) av, get_bytearray((byte*)a, sizeof(a)), vf);
        CU_ASSERT_EQUAL(result,0);

    } while(0);

    // destroy vf
    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    // TODO: cleanup statics
    //etchvf_free_builtins()

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
CU_pSuite test_etch_binarytdit_suite()
{
    CU_pSuite ps = CU_add_suite("bintditdo test suite", init_suite, clean_suite);

    CU_add_test(ps, "test constructor 0", test_constructor_0);
    CU_add_test(ps, "test constructor 1", test_constructor_1);
    CU_add_test(ps, "test constructor 2", test_constructor_2);
    CU_add_test(ps, "test check value",   test_check_value);
     
	//TODO implement this test's functionality again!!!
    //CU_add_test(ps, "test byte[][] out and back in",   test_byte_array_out_in);
    CU_add_test(ps, "test bool[][] out and back in",   test_bool_array_out_in);
    CU_add_test(ps, "test short[][] out and back in",  test_short_array_out_in);

    CU_add_test(ps, "test int[][] out and back in",    test_int_array_out_in);
    CU_add_test(ps, "test long[][] out and back in",   test_long_array_out_in);
    CU_add_test(ps, "test float[][] out and back in",  test_float_array_out_in);
    CU_add_test(ps, "test double[][] out and back in", test_double_array_out_in);
    CU_add_test(ps, "test string[][] out and back in", test_string_array_out_in);
//    CU_add_test(ps, "test add perf", test_add_perf);

    CU_add_test(ps, "test single values to bytes", test_single_value_to_bytes);
    CU_add_test(ps, "test bool array to bytes",    test_boolean_array_value_to_bytes);
    CU_add_test(ps, "test byte arrays to bytes",   test_byte_array_value_to_bytes);
    CU_add_test(ps, "test short array to bytes",   test_short_array_value_to_bytes);
    CU_add_test(ps, "test int array to bytes",     test_int_array_value_to_bytes);
    CU_add_test(ps, "test long array to bytes",    test_long_array_value_to_bytes);
    CU_add_test(ps, "test float array to bytes",   test_float_array_value_to_bytes);
    CU_add_test(ps, "test double array to bytes",  test_double_array_value_to_bytes);
    CU_add_test(ps, "test string values to bytes", test_string_values_to_bytes);
    CU_add_test(ps, "test date values to bytes",   test_date_values_to_bytes);

    return ps;
}
