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

#define ETCH_MAX_NATIVE_DIM 3 /* arbitrary limit on native array dimensions */
struct etchexception; /* declared in etchexcp.h which we can't include here */
struct vtabmask;


/**
 * when an etchobject is returned from a request such as a method, it will contain
 * an etchresult to indicate the result of the request, and any possible exception.
 */ 
typedef struct etchresult
{
    int resultcode;   
    int reasoncode;
    struct etchexception* exception; 

} etchresult;


/**
 * function signatures for object virtuals
 */
typedef int   (*objdtor)  (void*);
typedef void* (*objclone) (void*);
typedef int   (*etch_destructor) (void*);

typedef struct objmask objmask;
typedef int  (*obj_gethashkey) (objmask*);
struct etch_id_name;


/**
 * objmask
 * mask over all etch objects
 */
typedef struct objmask
{
    unsigned int     hashkey;     /* unique key used by a hash map */
    unsigned short   obj_type;    /* type of this object */
    unsigned short   class_id;    /* class identifier */
    struct vtabmask* vtab;        /* virtual function table */
    int   (*destroy)(void*);      /* destructor */
    void* (*clone)  (void*);      /* copy constructor */ 
    obj_gethashkey   get_hashkey; /* hash key calculation override */
    struct objmask*  parent;      /* class from which this derives */
    etchresult*      result;      /* embedded result and exception */
    unsigned int     refcount;    /* non-zero implies ref counted  */
    unsigned int     length;      /* byte count of the flat object */
    unsigned char    is_null;     /* does object wrap a null value */
    unsigned char    is_copy;     /* is object content not owned   */
    unsigned char    is_static;   /* should destructor free object */
    unsigned char    reserved;

} objmask;


/**
 * etch_object  
 * object wrapper for any object 
 */
struct etch_object
{
    unsigned int     hashkey;    
    unsigned short   obj_type;  
    unsigned short   class_id;   
    struct vtabmask* vtab;       
    int   (*destroy)(void*);     
    void* (*clone)  (void*); 
    obj_gethashkey   get_hashkey;   
    struct objmask*  parent;    
    etchresult*      result;    
    unsigned int     refcount;      
    unsigned int     length;     
    unsigned char    is_null;   
    unsigned char    is_copy;  
    unsigned char    is_static;  
    unsigned char    reserved;

    void* value; 
 
    unsigned char   is_value_object;
    unsigned char   is_value_owned; 
    unsigned char   unused[2];
}; 


/**
 * etchparentinfo
 * object inheritance list entry 
 */
typedef struct etchparentinfo
{
   union {
   unsigned short  obj_type; 
   unsigned short  list_size;  /* entry[0] in any inheritance list */
   };
   union {
   unsigned short  class_id;   
   unsigned short  list_count; /* entry[0] in any inheritance list */ 
   };
   
} etchparentinfo;
                          

/**
 * vtabmask
 * mask over any vtable 
 */
struct vtabmask
{
    unsigned int     hashkey;    
    unsigned short   obj_type;          
    unsigned short   class_id;   
    struct vtabmask* vtab;       
    int   (*destroy)(void*);     
    void* (*clone)  (void*); 
    obj_gethashkey   get_hashkey;    
    struct objmask*  parent;     
    etchresult*      result;     
    unsigned int     refcount;       
    unsigned int     length;     
    unsigned char    is_null;   
    unsigned char    is_copy;   
    unsigned char    is_static;  
    unsigned char    reserved;

    etchparentinfo*  inherits_from;  /* inheritance list */ 

    unsigned char unused[4];
    
    /* function pointers start here */              
};

typedef struct vtabmask vtabmask;



/**
 * lastresult global objects  
 */
objmask   _lastresobj;
objmask*   lastresobj;
etchresult lastresult;
void clear_lastresult();


/**
 * etch_byte - wrapped byte 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_byte
{
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    vtabmask*       vtab;       
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

    signed char value;  

    unsigned char unused[7];  
};

/**
 * etch_boolean - wrapped boolean 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_boolean
{
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    vtabmask*       vtab;      
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

    unsigned char value; 

    unsigned char unused[7];     
};

/**
 * etch_int8 - wrapped 8-bit integer 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_int8
{
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    vtabmask*       vtab;     
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

    signed char value; 

    unsigned char unused[7];     
};

/**
 * etch_int16 - wrapped 16-bit integer 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_int16
{
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    vtabmask*       vtab;       
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

    signed short value; 

    unsigned char unused[6];     
};

/**
 * etch_int32 - wrapped 32-bit integer 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_int32
{
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    vtabmask*       vtab;       
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

    signed int value;

    unsigned char unused[4];   
};

/**
 * etch_int64 - wrapped 64-bit integer 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_int64
{
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    vtabmask*       vtab;       
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

    int64 value;    
};

/**
 * etch_float - wrapped 32-bit float 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_float
{
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    vtabmask*       vtab;       
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

    float value;   

    unsigned char unused[4];   
};

/**
 * etch_double - wrapped 64-bit float 
 * note that this and all CLASSID_PRIMITIVE_XXXX wrapped primitives are expected
 * by code which generalizes such objects, to have the object's value as the first
 * non-header item in the object.
 */
struct etch_double
{
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    vtabmask*       vtab;      
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

    double value;    
};


typedef union etch_charptr
{ void*    value;
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
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    vtabmask*       vtab;       
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
    unsigned char   encoding;  

    union etch_charptr v;     /* pointer to string value */
    unsigned int char_count;  /* number of characters */
    unsigned int byte_count;  /* including terminator */
};

/**
 * etch_date - date object 
 */
struct etch_date
{
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    vtabmask*       vtab;        
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

  /* if we have a number of seconds then just add the seconds to the time_t.
   * otherwise call localtime() to get a pointer to struct tm, adjust the  
   * hours, minutes or seconds accordingly, and invoke mktime() to normalize
   * the tm object. for example to increment the current time by 9 hours,
   * add 9 to tm_hours with no concern for whether the result may be > 24.
   */

    time_t  value; 
    clock_t ticks; 
};

/**
 * etch_exception - wrapped etchexception   
 */
struct etch_exception
{
    unsigned int    hashkey;    
    unsigned short  obj_type;   
    unsigned short  class_id;   
    vtabmask*       vtab;        
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

    struct etchexception* value; 
};


/**
 * etch_collection_mask  
 * masks etch object collections such as etch_nativearray, etch_arraylist, 
 * and etch_hashtable. not instantiated.
 */
typedef struct etch_collection_mask
{
    unsigned int    hashkey;    
    unsigned short  obj_type;  
    unsigned short  class_id;   
    vtabmask*       vtab;       
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
 * etch_nativearray  
 * object wrapper and methods for an n-dimensioned array of any type
 * represented in memory as a single byte vector.
 */
struct etch_nativearray
{
    unsigned int    hashkey;    
    unsigned short  obj_type;  
    unsigned short  class_id;   
    vtabmask*       vtab;       
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
    unsigned char   is_content_owned; /* is content memory owned by this object */

    void*  values;  /* flattened array content */
    unsigned short content_obj_type;   /* ETCHTYPEB_INT32 means content is int */
    unsigned short content_class_id;  /* CLASSID_NONE means content not wrapped */
    int  numdims;   /* number of dimensions, e.g., 2 for x[3][4]  */

    /* this object may be masked by etch_collection_mask to determine content
     * type and class, so do not add any fields above this comment */

    size_t itemsize;  /* size in bytes of an item, e.g. sizeof(int) */
    size_t bytecount; /* length in bytes of array content (values)  */
    size_t dimension[ETCH_MAX_NATIVE_DIM]; /* for int x[2][3] [0] is 3, [1] is 2  */
    size_t dimsize  [ETCH_MAX_NATIVE_DIM]; /* for int x[2][3] [0] is 4, [1] is 12 */
    size_t counts   [ETCH_MAX_NATIVE_DIM]; /* optional actual population counts */

    int (*put1) (struct etch_nativearray*, void* v, int i); 
    int (*put2) (struct etch_nativearray*, void* v, int i, int j); 
    int (*put3) (struct etch_nativearray*, void* v, int i, int j, int k);
    int (*get1) (struct etch_nativearray*, void* v, int i); 
    int (*get2) (struct etch_nativearray*, void* v, int i, int j); 
    int (*get3) (struct etch_nativearray*, void* v, int i, int j, int k); 
}; 


typedef struct etch_object  etch_object;
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
typedef struct etch_date    etch_date;
typedef struct etch_exception   etch_exception;
typedef struct etch_nativearray etch_nativearray;


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
    objmask* parent;

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
    etch_nativearray* origl_array;
    etch_nativearray* final_array;

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
int  destroy_object(objmask*);
int  destroy_objectex(objmask*);
int  destroy_string(etch_string*);
objmask*  clone_object (objmask*);
objmask*  clone_null(objmask*);
objmask*  new_object(const int objsize, 
    const unsigned short obj_type, const unsigned short class_id);

/* 
 * wide char string clone
 */
wchar_t* new_wchar(const wchar_t* s);

/* 
 * narrow char string clone
 */
char* new_char(const char* s);

typedef etch_object etch_who; 
typedef etch_int32  etch_event;
typedef etch_int32  etch_query;
typedef etch_int32  etch_control;
etch_who* new_who(void* whoobj, const int is_owned); 
objmask*  new_nullobj();

int destroy_etch_object_value(etch_object*);
objmask* etchobj_assign_to(objmask*, objmask*);


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
etch_object*    new_etch_object(const unsigned short class_id, void* value);
objmask*        new_primitive(const unsigned, const unsigned short);

etchresult* new_etchresult(const int result, const int reason);

/* these are wrapped integers for now. if we need more data, we can define dedicated objects */
etch_event*   new_etch_event  (const unsigned short class_id, const int value);
etch_query*   new_etch_query  (const unsigned short class_id, const int value);
etch_control* new_etch_control(const unsigned short class_id, const int value);

#define ETCH_NOREFCOUNT_MARKER 0xffffffff
#define is_etchobj_refcount_decremented(x) (x->hashkey == ETCH_NOREFCOUNT_MARKER)

#define ETCHOBJCLASS(x) ETCHMAKECLASS(x->obj_type, x->class_id)
#define ETCH_CONX_SIG 0xe5d4c3b2

#define ETCHOBJ_DESTROY(x) do { if (x) x->destroy(x); x = NULL; } while(0);
#define ETCHOBJ_FREE(x) do { if (x) etch_free(x); x = NULL; } while(0);


typedef union  
{ char   vbyte;  short vint16; int vint32; int64 vint64; double vdouble; 
  float  vfloat; void* vaddr;  etch_string* vstring; 
  struct objmask* vobjmask; 
  struct etch_nativearray* vnatarray; struct etch_structvalue* vsv; 
  struct etch_arrayvalue*  varrayval; struct etch_arraylist* varraylist;
  int64  all; 
} union_alltypes;

etch_nativearray* new_nativearray (unsigned short class_id, const size_t itemsize, const int numdims, 
    const int dim0, const int dim1, const int dim2);
etch_nativearray* new_nativearray_from (void* values, unsigned short class_id, const size_t itemsize, 
    const int numdims, const int dim0, const int dim1, const int dim2);
etch_nativearray* new_nativearray_of(unsigned short content_obj_type, unsigned short content_class_id,  
    const int numdims, const int dim0, const int dim1, const int dim2);
etch_nativearray* new_subarray(etch_nativearray* a, const int i); 
etch_nativearray* etch_nativearray_assign_to(etch_nativearray*, etch_nativearray*);
objmask* etch_nativearray_get_element(etch_nativearray*, const int i); 
int etch_nativearray_get_component_type(etch_object* classobj, etch_component_type_params*);
int etch_nativearray_get_wrapped_component(etch_nativearray*, const int i, objmask** out);
int destroy_nativearray_content(etch_nativearray*);
int destroy_nativearray(etch_nativearray*);
size_t etch_nativearray_off1 (etch_nativearray* a, int i);
size_t etch_nativearray_off2 (etch_nativearray* a, int i, int j);
size_t etch_nativearray_off3 (etch_nativearray* a, int i, int j, int k);

#define is_etch_byte(x)         (x && ((objmask*)x)->class_id == CLASSID_PRIMITIVE_BYTE)
#define is_etch_boolean(x)      (x && ((objmask*)x)->class_id == CLASSID_PRIMITIVE_BOOL)
#define is_etch_int8(x)         (x && ((objmask*)x)->class_id == CLASSID_PRIMITIVE_INT8)
#define is_etch_int16(x)        (x && ((objmask*)x)->class_id == CLASSID_PRIMITIVE_INT16)
#define is_etch_int32(x)        (x && ((objmask*)x)->class_id == CLASSID_PRIMITIVE_INT32)
#define is_etch_int64(x)        (x && ((objmask*)x)->class_id == CLASSID_PRIMITIVE_INT64)
#define is_etch_float(x)        (x && ((objmask*)x)->class_id == CLASSID_PRIMITIVE_FLOAT)
#define is_etch_double(x)       (x && ((objmask*)x)->class_id == CLASSID_PRIMITIVE_DOUBLE)
#define is_etch_primitive(x)    (x && ((objmask*)x)->obj_type == ETCHTYPEB_PRIMITIVE)
#define is_etch_struct(x)       (x && ((objmask*)x)->obj_type == ETCHTYPEB_STRUCTVAL)
#define is_etch_validator(x)    (x && ((objmask*)x)->obj_type == ETCHTYPEB_VALIDATOR)
#define is_etch_arraylist(x)    (x && ((objmask*)x)->obj_type == ETCHTYPEB_ARRAYLIST)
#define is_etch_arrayvalue(x)   (x && ((objmask*)x)->obj_type == ETCHTYPEB_ARRAYVAL)
#define is_etch_valuefact(x)    (x && ((objmask*)x)->obj_type == ETCHTYPEB_VALUEFACTORY)
#define is_etch_valuefactimpl(x)(x && ((objmask*)x)->obj_type == ETCHTYPEB_VALUEFACTIMP)
#define is_etch_exception(x)    (x && ((objmask*)x)->obj_type == ETCHTYPEB_EXCEPTION)
#define is_etch_queue(x)        (x && ((objmask*)x)->obj_type == ETCHTYPEB_ETCHQUEUE)
#define is_etch_wait(x)         (x && ((objmask*)x)->obj_type == ETCHTYPEB_WAIT)
#define is_etch_mailbox(x)      (x && ((objmask*)x)->obj_type == ETCHTYPEB_MAILBOX)
#define is_etch_imailbox(x)     (x && ((objmask*)x)->obj_type == ETCHTYPEB_MAILBOXINT)
#define is_etch_mailboxmgr(x)   (x && ((objmask*)x)->obj_type == ETCHTYPEB_MBOXMGR_IMPL)
#define is_etch_imailboxmgr(x)  (x && ((objmask*)x)->obj_type == ETCHTYPEB_MBOX_MANAGER)
#define is_etch_sessionmsg(x)   (x && ((objmask*)x)->obj_type == ETCHTYPEB_SESSIONMSG)
#define is_etch_transportmsg(x) (x && ((objmask*)x)->obj_type == ETCHTYPEB_TRANSPORTMSG)
#define is_etch_hashtable(x)    (x && ((objmask*)x)->obj_type == ETCHTYPEB_HASHTABLE) 
#define is_etch_nativearray(x)  (x && ((objmask*)x)->obj_type == ETCHTYPEB_NATIVEARRAY)
#define is_etch_deliverysvc(x)  (x && ((objmask*)x)->obj_type == ETCHTYPEB_DELIVERYSVC_IMPL)
#define is_etch_ideliverysvc(x) (x && ((objmask*)x)->obj_type == ETCHTYPEB_DELIVERYSVCINT)
#define is_etch_tcpserver(x)    (x && ((objmask*)x)->obj_type == ETCHTYPEB_TCPSERVER)
#define is_etch_sessionlxr(x)   (x && ((objmask*)x)->obj_type == ETCHTYPEB_SESSIONLXR)
#define is_etch_sessionpacket(x)(x && ((objmask*)x)->obj_type == ETCHTYPEB_SESSIONPKT) 
#define is_etch_transportpkt(x) (x && ((objmask*)x)->obj_type == ETCHTYPEB_TRANSPORTPKT) 
#define is_etch_sessiondata(x)  (x && ((objmask*)x)->obj_type == ETCHTYPEB_SESSIONDATA) 
#define is_etch_transportdata(x)(x && ((objmask*)x)->obj_type == ETCHTYPEB_TRANSPORTDATA) 
#define is_etch_packetizer(x)   (x && ((objmask*)x)->obj_type == ETCHTYPEB_PACKETIZER) 
#define is_etch_messagizer(x)   (x && ((objmask*)x)->obj_type == ETCHTYPEB_MESSAGIZER) 
#define is_etch_serverimpl(x)   (x && ((objmask*)x)->obj_type == ETCHTYPEB_EXESERVERIMPL) 
#define is_etch_serverbase(x)   (x && ((objmask*)x)->obj_type == ETCHTYPEB_EXESERVERBASE) 
#define is_etch_factoryparams(x)(x && ((objmask*)x)->obj_type == ETCHTYPEB_FACTORYPARAMS) 
#define is_etch_clientsession(x)(x && ((objmask*)x)->obj_type == ETCHTYPEB_CLIENT_SESSION) 
#define is_etch_remote_server(x)(x && ((objmask*)x)->obj_type == ETCHTYPEB_REMOTESERVER) 
#define is_etch_client_impl(x)  (x && ((objmask*)x)->obj_type == ETCHTYPEB_EXECLIENTIMPL) 
#define is_etch_client_stub(x)  (x && ((objmask*)x)->obj_type == ETCHTYPEB_CLIENTSTUB) 
#define is_etch_server_stub(x)  (x && ((objmask*)x)->obj_type == ETCHTYPEB_SERVERSTUB) 
#define is_etch_client_base(x)  (x && ((objmask*)x)->obj_type == ETCHTYPEB_EXECLIENTBASE) 
#define is_etch_threadpool(x)   (x && ((objmask*)x)->obj_type == ETCHTYPEB_THREADPOOL)
#define is_etch_flexbuffer(x)   (x && ((objmask*)x)->obj_type == ETCHTYPEB_FLEXBUF) 
#define is_etch_message(x)      (x && ((objmask*)x)->obj_type == ETCHTYPEB_MESSAGE) 
#define is_etch_socket(x)       (x && ((objmask*)x)->obj_type == ETCHTYPEB_SOCKET) 
#define is_etch_thread(x)       (x && ((objmask*)x)->obj_type == ETCHTYPEB_THREAD) 
                   
#define is_etch_string(x)      (x && ((objmask*)x)->class_id == CLASSID_STRING \
 && ((objmask*)x)->obj_type == ETCHTYPEB_PRIMITIVE)  

#define is_etch_date(x) (x && ((objmask*)x)->class_id == CLASSID_DATE \
 && ((objmask*)x)->obj_type == ETCHTYPEB_PRIMITIVE) 

#define is_etch_object(x)     (x && ((objmask*)x)->obj_type == ETCHTYPEB_ETCHOBJECT) 
#define is_etch_object_type(a,b)(a == ETCHTYPEB_ETCHOBJECT || b == CLASSID_OBJECT ) 
#define is_etch_nativearray_type(t,c) (t == ETCHTYPEB_NATIVEARRAY)
#define is_etch_arraylist_type(t,c)   (t == ETCHTYPEB_ARRAYLIST)
#define is_etch_objarray_type(t,c)    (c == CLASSID_ARRAY_OBJECT)

#define is_etch_tcpconnection(x) (x \
 && ((objmask*)x)->obj_type == ETCHTYPEB_CONNECTION \
 && ((objmask*)x)->class_id == CLASSID_TCP_CONNECTION) 

#define is_etch_serverparams(x) (x \
 && ((objmask*)x)->obj_type == ETCHTYPEB_FACTORYPARAMS \
 && ((objmask*)x)->class_id == CLASSID_SERVERFACTORY) 

#define is_etch_clientparams(x) (x \
 && ((objmask*)x)->obj_type == ETCHTYPEB_FACTORYPARAMS \
 && ((objmask*)x)->class_id == CLASSID_CLIENTFACTORY) 

#define is_etch_combo_validator(x) (x \
 && ((objmask*)x)->obj_type == ETCHTYPEB_VALIDATOR \
 && ((objmask*)x)->class_id == CLASSID_COMBO_VALIDATOR) 

#define is_etch_primitive_number(x) (x \
 && ((objmask*)x)->class_id >= CLASSID_PRIMITIVE_BYTE \
 && ((objmask*)x)->class_id <= CLASSID_PRIMITIVE_DOUBLE)

#define is_etch_arraytype(x) (x \
 && (((objmask*)x)->obj_type >= ETCHTYPEB_NATIVEARRAY \
  || ((objmask*)x)->obj_type <= ETCHTYPEB_ARRAYVAL))

#define is_etch_unwantedmsg(x) (x \
 && ((objmask*)x)->obj_type == ETCHTYPEB_EVENT \
 && ((objmask*)x)->class_id == CLASSID_EVENT_UNWANTMSG) 


#define is_etch_objparams(x,a,b) (x && ((objmask*)x)->obj_type == a && ((objmask*)x)->class_id == b)  

#define is_etch_connection(cx) (cx && (*(unsigned*)cx) == ETCH_CONX_SIG)

/**
 * macros to interpret state of the object byteflag is_static 
 */
#define ETCHOBJ_IMMUTABLE_SHELL   1   /* object shell not to be freed */
#define ETCHOBJ_IMMUTABLE_CONTENT 2   /* object content not to be freed */
#define ETCHOBJ_IMMUTABLE_ALL     3   /* entire object not to be freed */
#define ETCHOBJ_STATIC_RESOURCE   4   /* object will not be destroyed by resources mgr */
#define is_etchobj_static_shell(x)   (  ( x->is_static  &  ETCHOBJ_IMMUTABLE_SHELL ) != 0 )
#define is_etchobj_static_shellonly(x)  ( x->is_static ==  ETCHOBJ_IMMUTABLE_SHELL )
#define is_etchobj_static_content(x)    ( x->is_static >=  ETCHOBJ_IMMUTABLE_CONTENT ) 
#define is_etchobj_static_contonly(x)   ( x->is_static ==  ETCHOBJ_IMMUTABLE_CONTENT )
#define is_etchobj_static_all(x)        ( x->is_static ==  ETCHOBJ_IMMUTABLE_ALL )
#define is_etchobj_static_resource(x) ( ( x->is_static  &  ETCHOBJ_STATIC_RESOURCE) != 0 )
#define set_etchobj_static_all(x)       ( x->is_static |=  ETCHOBJ_IMMUTABLE_ALL )
#define set_etchobj_static_shell(x)     ( x->is_static |=  ETCHOBJ_IMMUTABLE_SHELL )
#define set_etchobj_static_content(x)   ( x->is_static |=  ETCHOBJ_IMMUTABLE_CONTENT )
#define set_etchobj_static_resource(x)  ( x->is_static |=  ETCHOBJ_STATIC_RESOURCE )
#define clear_etchobj_static_shell(x)   ( x->is_static &= ~ETCHOBJ_IMMUTABLE_SHELL )
#define clear_etchobj_static_content(x) ( x->is_static &= ~ETCHOBJ_IMMUTABLE_CONTENT )
#define clear_etchobj_static_all(x)     ( x->is_static &= ~ETCHOBJ_IMMUTABLE_ALL )
#define clear_etchobj_static_resource(x)( x->is_static &= ~ETCHOBJ_STATIC_RESOURCE )

/**
 * macros to interpret etchobject etchresult and etchexception content
 */
#define is_exception(x)  ( x && (x->obj_type == ETCHTYPEB_EXCEPTION || (x->result && x->result->exception)))
#define etch_catch(x)    ( x->result && x->result->exception? x: 0 )
#define get_exception(x) ( x->result? x->result->exception: NULL ) 
#define get_result(x)    ( x->result? x->result->resultcode: 0 )
#define get_reason(x)    ( x->result? x->result->reasoncode: 0 )
#define is_resulterror(x)( x->result? x->result->resultcode < 0: 0 )
#define is_good_result(o)( o && (!o->result || !o->result->exception || o->result->resultcode >= 0) ) 

/**
 * macros to get size/count from etch object inheritance list
 */        
#define has_parents(obj) {obj && ((vtabmask*)obj->vtab)->inherits_from \
     && ((vtabmask*)obj->vtab)->inherits_from[0].list_count > 0))     
#define get_etchobj_parent_count(obj) \
  (obj && obj->vtab && ((vtabmask*)obj->vtab)->inherits_from? \
  ((vtabmask*)obj->vtab)->inherits_from[0].list_count: 0)
#define get_etchobj_parent_listsize(obj) \
  (obj && obj->vtab && ((vtabmask*)obj->vtab)->inherits_from? \
  ((vtabmask*)obj->vtab)->inherits_from[0].list_size: 0) 

/**
 * methods to access and/or instantiate object's inheritance hierarchy
 */ 
etchparentinfo* get_vtab_inheritance_list(objmask*, 
   const short size, const short count, const short vtabclass);
etchparentinfo* new_etch_inheritance_list(const short size, const short count, 
   etchparentinfo* oldlist);
etchparentinfo* get_next_etch_parent(objmask*, int current_index);
etchparentinfo* get_next_etch_parentex(const unsigned short class_id, 
   etchparentinfo* inhertlist, int current_index);

int  etchobj_is_assignable_from(etch_objclass* target, etch_objclass* source);
int  etchobj_is_assignable_fromobj(objmask* targetobj, objmask* sourceobj);
void set_etch_assignable_arg_from(etch_objclass*, objmask*);

unsigned etch_get_char_hashkey (const char*);
unsigned etch_get_wchar_hashkey(const wchar_t*);

int      is_derives_from_object(objmask*);
int      is_derives_from_object_class(const unsigned short class_id);
int      verify_object(objmask*, const unsigned short, const unsigned short, void** out);
void*    new_vtable(const void* parentvtab, const size_t size, const short classid);
short    short_type(unsigned i, unsigned j); 
void*    get_base_vtable(objmask*); 
unsigned etch_addref(objmask*);
unsigned etch_release(objmask*);
unsigned etch_release_wrapper(etch_object*);
unsigned defgethashkey(objmask*);
unsigned etch_number_get_hashkey(objmask*);
unsigned get_vtable_cachehkey(unsigned short class_id);
unsigned get_class_cachekey(unsigned short obj_type, unsigned short class_id);


#endif /* #ifndef ETCHOBJ_H */