
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
 * etchobj.h 
 * etch object definitions
 */

#ifndef ETCHOBJ_H
#define ETCHOBJ_H

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "etch.h"
#include "etch_mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

struct etch_object;
struct vtabmask;
struct etchexception; /* declared in etchexcp.h which we can't include here */

/**
 * function signatures for object virtuals
 */
typedef int   (*etch_object_destructor)(void*); /* should tak etch_object or etch_object */
typedef void* (*etch_object_clone)(void*); /* should take and return etch_object or etch_object */
typedef uint32   (*obj_gethashkey)(void*); /* should take etch_object or etch_object */

typedef struct etchresult
{
    int resultcode;   
    int reasoncode;
    struct etchexception* exception;
} etchresult;

/**
 * etch_object
 * mask over all etch objects
 */
typedef struct etch_object
{
    unsigned int hashkey;     /* unique key used by a hash map */
    unsigned short   obj_type;    /* type of this object */
    unsigned short   class_id;    /* class identifier */
    struct vtabmask* vtab;        /* virtual function table */
    etch_object_destructor          destroy;
    etch_object_clone         clone;
    obj_gethashkey   get_hashkey; /* hash key calculation override */
    struct etch_object*  parent;      /* class from which this derives */
    etchresult*      result;      /* embedded result and exception */
    unsigned int     refcount;    /* non-zero implies ref counted  */
    unsigned int     length;      /* byte count of the flat object */
    unsigned char    is_null;     /* does object wrap a null value */
    unsigned char    is_copy;     /* is object content not owned   */
    unsigned char    is_static;   /* should destructor free object */
    unsigned char    reserved;	  /* reserved for individual use   */
    etch_mutex_hookproc  synchook; /* hook for synchronization      */
    etch_mutex_t*   synclock; /* synchronization mutex         */
} etch_object;

typedef struct etch_byte    etch_byte;
typedef struct etch_boolean etch_boolean;
typedef struct etch_int8    etch_int8;
typedef struct etch_int16   etch_int16;
typedef struct etch_int32   etch_int32;
typedef struct etch_int64   etch_int64;
typedef struct etch_float   etch_float;
typedef struct etch_double  etch_double;
typedef struct etch_string  etch_string;
typedef struct etch_date    etch_date;
typedef struct etch_exception etch_exception;

/**
 * etchparentinfo
 * object inheritance list entry
 */
typedef struct etchparentinfo
{
   union {
     unsigned short  obj_type;
     unsigned short  list_size;  /* entry[0] in any inheritance list */
   } o;
   union {
     unsigned short  class_id;
     unsigned short  list_count; /* entry[0] in any inheritance list */
   } c;
} etchparentinfo;

/**
 * vtabmask
 * mask over any vtable 
 */
typedef struct vtabmask
{
    etch_object object;

    // inheritance list
    etchparentinfo*  inherits_from;   // 4 byte
    unsigned char unused[4];          // 4 byte
    
    /* function pointers start here */
} vtabmask;


/**
 * etch_byte - wrapped byte 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_byte
{
    etch_object object;
    signed char value;          // 1 byte
};

/**
 * etch_boolean - wrapped boolean 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_boolean
{
    etch_object object;
    unsigned char value;        // 1 byte
};

/**
 * etch_int8 - wrapped 8-bit integer 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_int8
{
    etch_object object;
    signed char value;          // 1 byte
};

/**
 * etch_int16 - wrapped 16-bit integer 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_int16
{
    etch_object object;
    signed short    value;      // 2 byte
};

/**
 * etch_int32 - wrapped 32-bit integer 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_int32
{
    etch_object object;
    signed int value;           // 4 byte
};

/**
 * etch_int64 - wrapped 64-bit integer 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_int64
{
    etch_object object;
    int64 value;    // 8 byte
};

/**
 * etch_float - wrapped 32-bit float 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_float
{
    etch_object object;
    float value;                // 4 byte
};

/**
 * etch_double - wrapped 64-bit float 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_double
{
    etch_object object;
    double value;       // 8 byte
};


typedef union etch_charptr
{
  void*    value;
  wchar_t* valw;
  char*    valc;
} etch_charptr;


/**
 * etch_string - wrapped pointer to unicode string  
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_string
{
    etch_object object;
    union etch_charptr v;     /* pointer to string value */
    unsigned int char_count;  /* number of characters */
    unsigned int byte_count;  /* including terminator */
    unsigned char   encoding; 
};

/**
 * etch_date - date object 
 */
struct etch_date
{
    etch_object object;

    time_t  value;
    clock_t ticks;
};

typedef enum excptype
{ 
  EXCPTYPE_NONE          = 0x0,
  EXCPTYPE_BUILTIN       = 0x1,
  EXCPTYPE_USERDEFINED   = 0x2
} excptype_t; 

/**
 * etch_collection_mask  
 * masks etch object collections such as etch_nativearray, etch_arraylist, 
 * and etch_hashtable. not instantiated.
 */
typedef struct etch_collection_mask
{
    etch_object object;

    void* p;
    unsigned short content_obj_type;
    unsigned short content_class_id;
    unsigned int n;

} etch_collection_mask; 


/**
 * etch_arraytype is used as a mask over etch_nativearray and etch_arrayvalue.
 * a function using an etch_arraytype* parameter will test for one or the other
 * and possibly convert the passed array to the other type.
 */  
typedef struct etch_collection_mask etch_arraytype;

/**
 * etch_objclass
 * parameter structure identifying an etch object type
 */
typedef struct etch_objclass
{
    unsigned short  obj_type;
    unsigned short  class_id;
    unsigned short  content_obj_type;
    unsigned short  content_class_id;
    unsigned short  vtable_class_id;
    unsigned int    numdims;
    etchparentinfo* inherits_from; 
    etch_object* parent;

} etch_objclass;


/**
 * etch_component_type_params
 * parameter and result structure for get_component_type() etc.
 */
typedef struct etch_component_type_params
{
    unsigned int   dimensions;
    unsigned short origl_class_id;
    unsigned short origl_obj_type;
    unsigned short final_obj_type;
    unsigned short final_class_id;
    struct etch_nativearray* origl_array;
    struct etch_nativearray* final_array;

} etch_component_type_params;


typedef struct etch_array_id_params
{
    unsigned short array_obj_type;
    unsigned short array_class_id;
    unsigned short content_obj_type;
    unsigned short content_class_id;
   
} etch_array_id_params;


/* 
 * default base object virtual method implementations
 */
int  destroy_object(void*);
int  destroy_objectex(etch_object*);
int  destroy_string(void*);
void*  clone_object (void*);
void*  clone_string (void*);
void*  clone_null(void*);
etch_object*  new_object(const int objsize, 
    const unsigned short obj_type, const unsigned short class_id);

/* 
 * wide char string clone
 */
wchar_t* new_wchar(const wchar_t* s);

/* 
 * narrow char string clone
 */
char* new_char(const char* s);

typedef struct etch_who {
    etch_object object;
    void* value;
} etch_who; 


typedef etch_int32  etch_event;
typedef etch_int32  etch_query;
typedef etch_int32  etch_control;
etch_who* new_who(void* whoobj); 
etch_object*  new_nullobj();

int destroy_etch_object_value(etch_object*);
etch_object* etchobj_assign_to(etch_object*, etch_object*);


/* 
 * etch primitive constructors
 */
etch_byte*      new_byte(const signed char);
etch_boolean*   new_boolean(boolean);
etch_int8*      new_int8(signed char);
etch_int16*     new_int16(short);
etch_int32*     new_int32(int);
etch_int64*     new_int64(int64);
etch_float*     new_float(float);
etch_double*    new_double(double);
etch_string*    new_string(const void*, const unsigned char encoding);
etch_string*    new_stringw(const void*); 
etch_string*    new_stringa(const void*); 
etch_string*    new_string_from(const void*, const unsigned char encoding);
etch_date*      new_date(); 
etch_object*    new_primitive(const unsigned, const unsigned short);

etchresult* new_etchresult(const int result, const int reason);

int32 etch_number_as_int32(const void* object);

/* these are wrapped integers for now. if we need more data, we can define dedicated objects */
etch_event*   new_etch_event  (const unsigned short class_id, const int value);
etch_query*   new_etch_query  (const unsigned short class_id, const int value);
etch_control* new_etch_control(const unsigned short class_id, const int value);

etch_status_t etch_object_destroy(void* object);
etch_object* etch_object_clone_func(void* pobject);

#define ETCH_NOREFCOUNT_MARKER 0xffffffff
#define is_etchobj_refcount_decremented(x) (x->get_hashkey(x) == ETCH_NOREFCOUNT_MARKER)

#define ETCHOBJCLASS(x) ETCHMAKECLASS(((etch_object*)x)->obj_type, ((etch_object*)x)->class_id)
#define ETCH_CONX_SIG 0xe5d4c3b2

typedef union
{
  char   vbyte;  unsigned char vubyte; short vint16; int vint32; int64 vint64; double vdouble; 
  float  vfloat; void* vaddr;  etch_string* vstring; 
  struct etch_object* vetch_object; 
  struct etch_nativearray* vnatarray; struct etch_structvalue* vsv; 
  struct etch_arrayvalue*  varrayval; struct etch_arraylist* varraylist;
  int64  all;
} union_alltypes;


#define is_etch_byte(x)         (x && ((etch_object*)x)->class_id == CLASSID_PRIMITIVE_BYTE)
#define is_etch_boolean(x)      (x && ((etch_object*)x)->class_id == CLASSID_PRIMITIVE_BOOL)
#define is_etch_int8(x)         (x && ((etch_object*)x)->class_id == CLASSID_PRIMITIVE_INT8)
#define is_etch_int16(x)        (x && ((etch_object*)x)->class_id == CLASSID_PRIMITIVE_INT16)
#define is_etch_int32(x)        (x && ((etch_object*)x)->class_id == CLASSID_PRIMITIVE_INT32)
#define is_etch_int64(x)        (x && ((etch_object*)x)->class_id == CLASSID_PRIMITIVE_INT64)
#define is_etch_float(x)        (x && ((etch_object*)x)->class_id == CLASSID_PRIMITIVE_FLOAT)
#define is_etch_double(x)       (x && ((etch_object*)x)->class_id == CLASSID_PRIMITIVE_DOUBLE)
#define is_etch_primitive(x)    (x && ((etch_object*)x)->obj_type == ETCHTYPEB_PRIMITIVE)
#define is_etch_struct(x)       (x && ((etch_object*)x)->obj_type == ETCHTYPEB_STRUCTVAL)
#define is_etch_validator(x)    (x && ((etch_object*)x)->obj_type == ETCHTYPEB_VALIDATOR)
#define is_etch_arraylist(x)    (x && ((etch_object*)x)->obj_type == ETCHTYPEB_ARRAYLIST)
#define is_etch_arrayvalue(x)   (x && ((etch_object*)x)->obj_type == ETCHTYPEB_ARRAYVAL)
#define is_etch_valuefact(x)    (x && ((etch_object*)x)->obj_type == ETCHTYPEB_VALUEFACTORY)
#define is_etch_valuefactimpl(x)(x && ((etch_object*)x)->obj_type == ETCHTYPEB_VALUEFACTIMP)
#define is_etch_exception(x)    (x && ((etch_object*)x)->obj_type == ETCHTYPEB_EXCEPTION)
#define is_etch_queue(x)        (x && ((etch_object*)x)->obj_type == ETCHTYPEB_ETCHQUEUE)
#define is_etch_wait(x)         (x && ((etch_object*)x)->obj_type == ETCHTYPEB_WAIT)
#define is_etch_mailbox(x)      (x && ((etch_object*)x)->obj_type == ETCHTYPEB_MAILBOX)
#define is_etch_imailbox(x)     (x && ((etch_object*)x)->obj_type == ETCHTYPEB_MAILBOXINT)
#define is_etch_mailboxmgr(x)   (x && ((etch_object*)x)->obj_type == ETCHTYPEB_MBOXMGR_IMPL)
#define is_etch_imailboxmgr(x)  (x && ((etch_object*)x)->obj_type == ETCHTYPEB_MBOX_MANAGER)
#define is_etch_sessionmsg(x)   (x && ((etch_object*)x)->obj_type == ETCHTYPEB_SESSIONMSG)
#define is_etch_transportmsg(x) (x && ((etch_object*)x)->obj_type == ETCHTYPEB_TRANSPORTMSG)
#define is_etch_hashtable(x)    (x && ((etch_object*)x)->obj_type == ETCHTYPEB_HASHTABLE) 
#define is_etch_nativearray(x)  (x && ((etch_object*)x)->obj_type == ETCHTYPEB_NATIVEARRAY)
#define is_etch_deliverysvc(x)  (x && ((etch_object*)x)->obj_type == ETCHTYPEB_DELIVERYSVC_IMPL)
#define is_etch_ideliverysvc(x) (x && ((etch_object*)x)->obj_type == ETCHTYPEB_DELIVERYSVCINT)
#define is_etch_tcpserver(x)    (x && ((etch_object*)x)->obj_type == ETCHTYPEB_TCPSERVER)
#define is_etch_sessionlxr(x)   (x && ((etch_object*)x)->obj_type == ETCHTYPEB_SESSIONLXR)
#define is_etch_sessionpacket(x)(x && ((etch_object*)x)->obj_type == ETCHTYPEB_SESSIONPKT) 
#define is_etch_transportpkt(x) (x && ((etch_object*)x)->obj_type == ETCHTYPEB_TRANSPORTPKT) 
#define is_etch_sessiondata(x)  (x && ((etch_object*)x)->obj_type == ETCHTYPEB_SESSIONDATA) 
#define is_etch_transportdata(x)(x && ((etch_object*)x)->obj_type == ETCHTYPEB_TRANSPORTDATA) 
#define is_etch_packetizer(x)   (x && ((etch_object*)x)->obj_type == ETCHTYPEB_PACKETIZER) 
#define is_etch_messagizer(x)   (x && ((etch_object*)x)->obj_type == ETCHTYPEB_MESSAGIZER) 
#define is_etch_serverimpl(x)   (x && ((etch_object*)x)->obj_type == ETCHTYPEB_EXESERVERIMPL) 
#define is_etch_serverbase(x)   (x && ((etch_object*)x)->obj_type == ETCHTYPEB_EXESERVERBASE) 
#define is_etch_factoryparams(x)(x && ((etch_object*)x)->obj_type == ETCHTYPEB_FACTORYPARAMS) 
#define is_etch_clientsession(x)(x && ((etch_object*)x)->obj_type == ETCHTYPEB_CLIENT_SESSION) 
#define is_etch_remote_server(x)(x && ((etch_object*)x)->obj_type == ETCHTYPEB_REMOTESERVER) 
#define is_etch_client_impl(x)  (x && ((etch_object*)x)->obj_type == ETCHTYPEB_EXECLIENTIMPL) 
#define is_etch_client_stub(x)  (x && ((etch_object*)x)->obj_type == ETCHTYPEB_CLIENTSTUB) 
#define is_etch_server_stub(x)  (x && ((etch_object*)x)->obj_type == ETCHTYPEB_SERVERSTUB) 
#define is_etch_client_base(x)  (x && ((etch_object*)x)->obj_type == ETCHTYPEB_EXECLIENTBASE) 
#define is_etch_threadpool(x)   (x && ((etch_object*)x)->obj_type == ETCHTYPEB_THREADPOOL)
#define is_etch_flexbuffer(x)   (x && ((etch_object*)x)->obj_type == ETCHTYPEB_FLEXBUF) 
#define is_etch_message(x)      (x && ((etch_object*)x)->obj_type == ETCHTYPEB_MESSAGE) 
#define is_etch_thread(x)       (x && ((etch_object*)x)->obj_type == ETCHTYPEB_THREAD) 

#define is_etch_string(x)      (x && ((etch_object*)x)->class_id == CLASSID_STRING \
 && ((etch_object*)x)->obj_type == ETCHTYPEB_PRIMITIVE)  

#define is_etch_date(x) (x && ((etch_object*)x)->class_id == CLASSID_DATE \
 && ((etch_object*)x)->obj_type == ETCHTYPEB_PRIMITIVE) 

#define is_etch_object(x)     (x && ((etch_object*)x)->obj_type == ETCHTYPEB_ETCHOBJECT) 
#define is_etch_object_type(a,b)(a == ETCHTYPEB_ETCHOBJECT || b == CLASSID_OBJECT ) 
#define is_etch_nativearray_type(t,c) (t == ETCHTYPEB_NATIVEARRAY)
#define is_etch_arraylist_type(t,c)   (t == ETCHTYPEB_ARRAYLIST)
#define is_etch_objarray_type(t,c)    (c == CLASSID_ARRAY_OBJECT)

#define is_etch_tcpconnection(x) (x \
 && ((etch_object*)x)->obj_type == ETCHTYPEB_CONNECTION \
 && ((etch_object*)x)->class_id == CLASSID_TCP_CONNECTION) 

#define is_etch_serverparams(x) (x \
 && ((etch_object*)x)->obj_type == ETCHTYPEB_FACTORYPARAMS \
 && ((etch_object*)x)->class_id == CLASSID_SERVERFACTORY) 

#define is_etch_clientparams(x) (x \
 && ((etch_object*)x)->obj_type == ETCHTYPEB_FACTORYPARAMS \
 && ((etch_object*)x)->class_id == CLASSID_CLIENTFACTORY) 

#define is_etch_combo_validator(x) (x \
 && ((etch_object*)x)->obj_type == ETCHTYPEB_VALIDATOR \
 && ((etch_object*)x)->class_id == CLASSID_COMBO_VALIDATOR) 

#define is_etch_primitive_number(x) (x \
 && ((etch_object*)x)->class_id >= CLASSID_PRIMITIVE_BYTE \
 && ((etch_object*)x)->class_id <= CLASSID_PRIMITIVE_DOUBLE)

#define is_etch_arraytype(x) (x \
 && (((etch_object*)x)->obj_type >= ETCHTYPEB_NATIVEARRAY \
  || ((etch_object*)x)->obj_type <= ETCHTYPEB_ARRAYVAL))

#define is_etch_unwantedmsg(x) (x \
 && ((etch_object*)x)->obj_type == ETCHTYPEB_EVENT \
 && ((etch_object*)x)->class_id == CLASSID_EVENT_UNWANTMSG) 


#define is_etch_objparams(x,a,b) (x && ((etch_object*)x)->obj_type == a && ((etch_object*)x)->class_id == b)  

#define is_etch_connection(cx) (cx && (*(unsigned*)cx) == ETCH_CONX_SIG)

/**
 * macros to interpret state of the object byteflag is_static 
 */
#define ETCHOBJ_IMMUTABLE_SHELL   1   /* object shell not to be freed */
#define ETCHOBJ_IMMUTABLE_CONTENT 2   /* object content not to be freed */
#define ETCHOBJ_IMMUTABLE_ALL     3   /* entire object not to be freed */
#define ETCHOBJ_STATIC_RESOURCE   4   /* object will not be destroyed by resources mgr */
#define is_etchobj_static_shell(x)   (  ( ((etch_object*)x)->is_static  &  ETCHOBJ_IMMUTABLE_SHELL ) != 0 )
#define is_etchobj_static_shellonly(x)  ( ((etch_object*)x)->is_static ==  ETCHOBJ_IMMUTABLE_SHELL )
#define is_etchobj_static_content(x)    ( ((etch_object*)x)->is_static >=  ETCHOBJ_IMMUTABLE_CONTENT ) 
#define is_etchobj_static_contonly(x)   ( ((etch_object*)x)->is_static ==  ETCHOBJ_IMMUTABLE_CONTENT )
#define is_etchobj_static_all(x)        ( ((etch_object*)x)->is_static ==  ETCHOBJ_IMMUTABLE_ALL )
#define is_etchobj_static_resource(x) ( ( ((etch_object*)x)->is_static  &  ETCHOBJ_STATIC_RESOURCE) != 0 )
#define set_etchobj_static_all(x)       ( ((etch_object*)x)->is_static |=  ETCHOBJ_IMMUTABLE_ALL )
#define set_etchobj_static_shell(x)     ( ((etch_object*)x)->is_static |=  ETCHOBJ_IMMUTABLE_SHELL )
#define set_etchobj_static_content(x)   ( ((etch_object*)x)->is_static |=  ETCHOBJ_IMMUTABLE_CONTENT )
#define set_etchobj_static_resource(x)  ( ((etch_object*)x)->is_static |=  ETCHOBJ_STATIC_RESOURCE )
#define clear_etchobj_static_shell(x)   ( ((etch_object*)x)->is_static &= ~ETCHOBJ_IMMUTABLE_SHELL )
#define clear_etchobj_static_content(x) ( ((etch_object*)x)->is_static &= ~ETCHOBJ_IMMUTABLE_CONTENT )
#define clear_etchobj_static_all(x)     ( ((etch_object*)x)->is_static &= ~ETCHOBJ_IMMUTABLE_ALL )
#define clear_etchobj_static_resource(x)( ((etch_object*)x)->is_static &= ~ETCHOBJ_STATIC_RESOURCE )


/**
 * macros to get size/count from etch object inheritance list
 */        
#define has_parents(obj) {obj && ((vtabmask*)((etch_object*)obj)->vtab)->inherits_from \
     && ((vtabmask*)((etch_object*)obj)->vtab)->inherits_from[0].list_count > 0))     
#define get_etchobj_parent_count(obj) \
  (obj && ((etch_object*)obj)->vtab && ((vtabmask*)obj->vtab)->inherits_from? \
  ((vtabmask*)((etch_object*)obj)->vtab)->inherits_from[0].list_count: 0)
#define get_etchobj_parent_listsize(obj) \
  (obj && ((etch_object*)obj)->vtab && ((vtabmask*)obj->vtab)->inherits_from? \
  ((vtabmask*)((etch_object*)obj)->vtab)->inherits_from[0].list_size: 0) 

/**
 * methods to access and/or instantiate object's inheritance hierarchy
 */ 
etchparentinfo* get_vtab_inheritance_list(etch_object*, 
   const short size, const short count, const short vtabclass);
etchparentinfo* new_etch_inheritance_list(const short size, const short count, 
   etchparentinfo* oldlist);
etchparentinfo* get_next_etch_parent(etch_object*, int current_index);
etchparentinfo* get_next_etch_parentex(const unsigned short class_id, 
   etchparentinfo* inhertlist, int current_index);

int  etchobj_is_assignable_from(etch_objclass* target, etch_objclass* source);
int  etchobj_is_assignable_fromobj(etch_object* targetobj, etch_object* sourceobj);
void set_etch_assignable_arg_from(etch_objclass*, etch_object*);

unsigned etch_get_char_hashkey (const char*);
unsigned etch_get_wchar_hashkey(const wchar_t*);

int      is_derives_from_object(etch_object*);
int      is_derives_from_object_class(const unsigned short class_id);
int      verify_object(etch_object*, const unsigned short, const unsigned short, void** out);
void*    new_vtable(const void* parentvtab, const size_t size, const short classid);
short    short_type(unsigned i, unsigned j); 
void*    get_base_vtable(etch_object*); 
unsigned etch_addref(etch_object*);
unsigned etch_release(etch_object*);
unsigned etch_release_wrapper(etch_object*);
uint32 defgethashkey(void*);
uint32 etch_number_get_hashkey(void*);
unsigned get_vtable_cachehkey(unsigned short class_id);
unsigned get_class_cachekey(unsigned short obj_type, unsigned short class_id);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCHOBJ_H */
