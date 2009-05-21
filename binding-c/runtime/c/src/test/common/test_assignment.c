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
 * test_assignment.c -- test etch object assignability and assignment
 */
#include "apr_time.h" /* some apr must be included first */
#include "etchthread.h"
#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etch_global.h"
#include "etchobj.h"


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
#define THISTEST_OBJTYPE      0xeee0
#define CLASSID_CLASS_A       0xeee1
#define CLASSID_CLASS_B       0xeee2
#define CLASSID_CLASS_C       0xeee3
#define CLASSID_CLASS_X       0xeee4
#define CLASSID_CLASS_Y       0xeee5
#define CLASSID_CLASSA_VTABLE 0xeed1
#define CLASSID_CLASSB_VTABLE 0xeed2
#define CLASSID_CLASSC_VTABLE 0xeed3
#define CLASSID_CLASSX_VTABLE 0xeed4
#define CLASSID_CLASSY_VTABLE 0xeed5


/**
 * class_a: base class
 */
typedef struct class_a
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

    etch_string* a_string;
     
} class_a;


/**
 * class_a destructor
 */
int destroy_class_a(class_a* thisp)
{
    if (thisp->a_string)
        thisp->a_string->destroy(thisp->a_string);

    destroy_object((objmask*) thisp);
    return 0;
}

/**
 * class_a copy consttructor
 */
class_a* clone_class_a(class_a* origobj)
{
    class_a* newobj  = (class_a*) new_object(sizeof(class_a), THISTEST_OBJTYPE, CLASSID_CLASS_A);
    memcpy(newobj, origobj, origobj->length);
    newobj->a_string = new_string(origobj->a_string->v.valw, ETCH_ENCODING_UTF16);  
    newobj->is_copy  = TRUE; 
    return newobj; 
}


/**
 * class_a constructor
 */
class_a* new_class_a(const wchar_t* strval)
{
    vtabmask* vtab   = NULL;
    class_a* newobj  = (class_a*) new_object(sizeof(class_a), THISTEST_OBJTYPE, CLASSID_CLASS_A);
    newobj->destroy  = destroy_class_a;
    newobj->clone    = clone_class_a;
    newobj->a_string = new_string(strval, ETCH_ENCODING_UTF16);

    if(!(vtab = cache_find(get_vtable_cachehkey(CLASSID_CLASSA_VTABLE), 0)))  
    {    vtab = new_vtable(NULL, sizeof(vtabmask), CLASSID_CLASSA_VTABLE);
         cache_insert(vtab->hashkey, vtab, FALSE);
    } 
 
    newobj->vtab = vtab;   
    return newobj;
}


/**
 * class_b: inherits from class_a
 */
typedef struct class_b
{
    unsigned int    hashkey;    
    unsigned short  obj_type; 
    unsigned short  class_id;
    vtabmask*       vtab;       
    int  (*destroy)(void*);
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;
    class_a*        parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;     
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;

    char* data; 
    int   datasize;  

} class_b;


/**
 * class_b destructor
 */
int destroy_class_b(class_b* thisp)
{
    etch_free(thisp->data);
    destroy_object((objmask*) thisp);
    return 0;
}


/**
 * class_b copy consttructor
 */
class_b* clone_class_b(class_b* origobj)
{
    class_b* newobj  = (class_b*) new_object(sizeof(class_b), THISTEST_OBJTYPE, CLASSID_CLASS_B);
    memcpy(newobj, origobj, origobj->length);

    newobj->parent = origobj->parent? 
        origobj->parent->clone(origobj->parent): 
        new_class_a(NULL);

    if (origobj->data)
    {   newobj->data     = etch_malloc(origobj->datasize, ETCHTYPEB_BYTES); 
        newobj->datasize = origobj->datasize;
        memcpy(newobj->data, origobj->data, origobj->datasize); 
    }
 
    newobj->is_copy = TRUE;
    return newobj;
}


/**
 * class_b constructor
 */
class_b* new_class_b(class_a* parent, const int datalen)
{
   vtabmask* vtab  = NULL;
   etchparentinfo* inheritlist = NULL;
   class_b* newobj = (class_b*) new_object(sizeof(class_b), THISTEST_OBJTYPE, CLASSID_CLASS_B);
   newobj->parent  = parent? parent: new_class_a(NULL);
   newobj->destroy = destroy_class_b;
   newobj->clone   = clone_class_b;
   newobj->data    = etch_malloc(datalen, ETCHTYPEB_BYTES);
   memset(newobj->data, 'x', datalen);
   newobj->datasize = datalen;

   if (NULL == (vtab = cache_find(get_vtable_cachehkey(CLASSID_CLASSB_VTABLE), 0)))  
   {   
       vtab = new_vtable(parent->vtab, sizeof(vtabmask), CLASSID_CLASSB_VTABLE);
       cache_insert(vtab->hashkey, vtab, FALSE);
        
       inheritlist = get_vtab_inheritance_list((objmask*)newobj,
           2, 1, CLASSID_CLASSB_VTABLE); /* create inheritance list */ 
       inheritlist[1].obj_type = THISTEST_OBJTYPE;
       inheritlist[1].class_id = CLASSID_CLASS_A;
    } 
   newobj->vtab = vtab;  
   return newobj;
}


/**
 * class_c: inherits from class_b
 */
typedef struct class_c
{
    unsigned int    hashkey;    
    unsigned short  obj_type; 
    unsigned short  class_id;
    vtabmask*       vtab;       
    int  (*destroy)(void*);
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;
    class_b*        parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;

    int* intarray;
    int  numitems;

} class_c;


/**
 * class_c destructor
 */
int destroy_class_c(class_c* thisp)
{
    etch_free(thisp->intarray);
    destroy_object((objmask*) thisp);
    return 0;
}


/**
 * class_c copy consttructor
 */
class_c* clone_class_c(class_c* origobj)
{
    class_c* newobj  = (class_c*) new_object(sizeof(class_c), THISTEST_OBJTYPE, CLASSID_CLASS_C);
    memcpy(newobj, origobj, sizeof(objmask));

    if  (origobj->parent)
         newobj->parent = origobj->parent->clone(origobj->parent);
    else newobj->parent = new_class_b(NULL, 0);

    if (origobj->intarray)
    {
        newobj->intarray = etch_malloc(origobj->numitems * sizeof(int), ETCHTYPEB_BYTES);
        newobj->numitems = origobj->numitems;
        memcpy(newobj->intarray, origobj->intarray, origobj->numitems * sizeof(int)); 
    }

    newobj->is_copy = TRUE;
    return newobj; 
}


/**
 * class_c constructor
 */
class_c* new_class_c(class_b* parent)
{
    int i = 0;
    vtabmask* vtab   = NULL;
    etchparentinfo*  inheritlist = NULL;
    class_c* newobj  = (class_c*) new_object(sizeof(class_c), THISTEST_OBJTYPE, CLASSID_CLASS_C);
    newobj->parent   = parent? parent: new_class_b(NULL, 0);
    newobj->destroy  = destroy_class_c;
    newobj->clone    = clone_class_c;

    newobj->numitems = 4;
    newobj->intarray = etch_malloc(4 * sizeof(int), ETCHTYPEB_BYTES);
    for(; i < 4; i++) newobj->intarray[i] = i;

    if (NULL == (vtab = cache_find(get_vtable_cachehkey(CLASSID_CLASSC_VTABLE), 0)))  
    {   
       vtab = new_vtable(parent->vtab, sizeof(vtabmask), CLASSID_CLASSC_VTABLE);
       cache_insert(vtab->hashkey, vtab, FALSE);
        
       inheritlist = get_vtab_inheritance_list((objmask*)newobj,
           3, 2, CLASSID_CLASSC_VTABLE); /* create inheritance list */ 
       inheritlist[1].obj_type = THISTEST_OBJTYPE;
       inheritlist[1].class_id = CLASSID_CLASS_B;
       inheritlist[2].obj_type = THISTEST_OBJTYPE;
       inheritlist[2].class_id = CLASSID_CLASS_A;
    } 
    newobj->vtab = vtab;  
    return newobj;
}


/**
 * class_y: inherits from class_x, but is flattened to contain class_x data.
 * inheritance chain is identified through the object vtable inheritance list
 */
typedef struct class_y
{
    unsigned int    hashkey;    
    unsigned short  obj_type; 
    unsigned short  class_id;
    vtabmask*       vtab;       
    int  (*destroy)(void*);
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;
    class_b*        parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;

    int class_x_instance_data;
    int class_y_instance_data;

} class_y;


/**
 * class_x: parent of class_y
 */
typedef struct class_x
{
    unsigned int    hashkey;    
    unsigned short  obj_type; 
    unsigned short  class_id;
    vtabmask*       vtab;       
    int  (*destroy)(void*);
    void*(*clone)  (void*); 
    obj_gethashkey  get_hashkey;
    class_b*        parent;
    etchresult*     result;
    unsigned int    refcount;
    unsigned int    length;
    unsigned char   is_null;
    unsigned char   is_copy;
    unsigned char   is_static;
    unsigned char   reserved;

    int class_x_instance_data;

} class_x;


/**
 * class_x constructor
 */
class_x* new_class_x(int classx_data)
{
    class_x* newobj = (class_x*) new_object(sizeof(class_x), THISTEST_OBJTYPE, CLASSID_CLASS_X);

    newobj->class_x_instance_data = classx_data;
 
    /* class_x has no parent so we can omit vtable and inheritance list if we want */
    return newobj;
}


/**
 * class_y constructor
 * inherits from class_x via inheritance type 2 - flat object
 */
class_y* new_class_y(int classx_data, int classy_data)
{
    etchparentinfo* inheritlist = NULL;
    vtabmask* vtab  = NULL;
    class_y* newobj = (class_y*) new_object(sizeof(class_y), THISTEST_OBJTYPE, CLASSID_CLASS_Y);

    newobj->class_x_instance_data = classx_data;
    newobj->class_y_instance_data = classy_data;
 
    if (NULL == (vtab = cache_find(get_vtable_cachehkey(CLASSID_CLASSY_VTABLE), 0)))  
    {   
        vtab = new_vtable(NULL, sizeof(vtabmask), CLASSID_CLASSY_VTABLE);
        cache_insert(vtab->hashkey, vtab, FALSE);
        
        inheritlist = get_vtab_inheritance_list((objmask*)newobj,
            2, 1, CLASSID_CLASSY_VTABLE); /* create inheritance list */ 
        inheritlist[1].obj_type = THISTEST_OBJTYPE;
        inheritlist[1].class_id = CLASSID_CLASS_X; 
    } 
    newobj->vtab = vtab;  
    return newobj;
}


void classarg_init_object(etch_objclass* arg)
{
    memset(arg, 0, sizeof(etch_objclass));
    arg->obj_type = ETCHTYPEB_ETCHOBJECT;
    arg->class_id = CLASSID_OBJECT;
}


/**
 * classarg_init_nativearray()
 * initialize class parameters for a wrapped primitive
 */
void classarg_init_primitive(etch_objclass* arg, short class_id)
{
    memset(arg, 0, sizeof(etch_objclass));
    arg->obj_type = ETCHTYPEB_PRIMITIVE;
    arg->class_id = class_id;
}


/**
 * classarg_init_nativearray()
 * initialize class parameters for native array 
 */
void classarg_init_nativearray(etch_objclass* arg, short class_id, int dim, short conttype, short contclass )
{
    memset(arg, 0, sizeof(etch_objclass));
    arg->obj_type = ETCHTYPEB_NATIVEARRAY;
    arg->class_id = class_id;
    arg->numdims  = dim;
    arg->content_obj_type = conttype;
    arg->content_class_id = contclass;
}


/**
 * classarg_init_customtype_scalar()
 * initialize class parameters for a custom type with an inheritance hierarchy.
 * real code would not allocate an inheritance list as we do here, this would  
 * be handled in the object vtable constructor, and the list subsequently 
 * accessed via the object's cached vtable.  
 */
void classarg_init_customtype_scalar(etch_objclass* arg, short class_id, int numsupers,
   short superclass_1, short superclass_2)
{
    memset(arg, 0, sizeof(etch_objclass));
    arg->obj_type = THISTEST_OBJTYPE;
    arg->class_id = class_id;

    arg->inherits_from = etch_malloc(sizeof(etchparentinfo) * (numsupers + 1), ETCHTYPEB_BYTES);
    arg->inherits_from[0].list_size  = numsupers + 1;
    arg->inherits_from[0].list_count = numsupers;

    if (numsupers > 0) 
    { arg->inherits_from[1].obj_type = THISTEST_OBJTYPE; 
      arg->inherits_from[1].class_id = superclass_1; 
    }

    if (numsupers > 1) 
    { arg->inherits_from[2].obj_type = THISTEST_OBJTYPE; 
      arg->inherits_from[2].class_id = superclass_2; 
    }
}


void classarg_cleanup(etch_objclass* target, etch_objclass* source) 
{
    if (target->inherits_from) etch_free(target->inherits_from);
    if (source->inherits_from) etch_free(source->inherits_from);
    target->inherits_from = source->inherits_from = NULL;
}


/**
 * test_is_assignable_1()
 * test assignability of various classes 
 */
void test_is_assignable_1(void)
{
    etch_objclass target, source;
    int result = 0;

    /* int32obj = int16obj */
    classarg_init_primitive(&target, CLASSID_PRIMITIVE_INT32);
    classarg_init_primitive(&source, CLASSID_PRIMITIVE_INT16);
    result = etchobj_is_assignable_from(&target, &source); 
    CU_ASSERT_EQUAL(result, FALSE);
    result = etchobj_is_assignable_from(&source, &target); 
    CU_ASSERT_EQUAL(result, FALSE);

    /* short[]obj = shortobj */
    classarg_init_nativearray(&target, CLASSID_ARRAY_INT16, 1, ETCHTYPEB_PRIMITIVE, CLASSID_PRIMITIVE_INT16);
    classarg_init_primitive(&source, CLASSID_PRIMITIVE_INT16);
    result = etchobj_is_assignable_from(&target, &source); 
    CU_ASSERT_EQUAL(result, FALSE);
    result = etchobj_is_assignable_from(&source, &target); 
    CU_ASSERT_EQUAL(result, FALSE);

    /* etchobj = longobj */
    classarg_init_object(&target);
    classarg_init_primitive(&source, CLASSID_PRIMITIVE_INT64);
    result = etchobj_is_assignable_from(&target, &source); 
    CU_ASSERT_EQUAL(result, TRUE);
    result = etchobj_is_assignable_from(&source, &target); 
    CU_ASSERT_EQUAL(result, FALSE);

    /* obj[]obj = shortobj */
    classarg_init_nativearray(&target, CLASSID_ARRAY_OBJECT, 1, ETCHTYPEB_ETCHOBJECT, CLASSID_OBJECT);
    classarg_init_primitive(&source, CLASSID_PRIMITIVE_INT16);
    result = etchobj_is_assignable_from(&target, &source); 
    CU_ASSERT_EQUAL(result, FALSE);
    result = etchobj_is_assignable_from(&source, &target); 
    CU_ASSERT_EQUAL(result, FALSE);

    /* obj[]obj = short[]obj */
    classarg_init_nativearray(&target, CLASSID_ARRAY_OBJECT, 1, ETCHTYPEB_ETCHOBJECT, CLASSID_OBJECT);
    classarg_init_nativearray(&source, CLASSID_ARRAY_INT16,  1, ETCHTYPEB_PRIMITIVE, CLASSID_PRIMITIVE_INT16);
    result = etchobj_is_assignable_from(&target, &source); 
    CU_ASSERT_EQUAL(result, TRUE);
    result = etchobj_is_assignable_from(&source, &target); 
    CU_ASSERT_EQUAL(result, FALSE);

    /* short[]obj = short[]obj */
    classarg_init_nativearray(&target, CLASSID_ARRAY_INT16, 1, ETCHTYPEB_PRIMITIVE, CLASSID_PRIMITIVE_INT16);
    classarg_init_nativearray(&source, CLASSID_ARRAY_INT16, 1, ETCHTYPEB_PRIMITIVE, CLASSID_PRIMITIVE_INT16);
    result = etchobj_is_assignable_from(&target, &source); 
    CU_ASSERT_EQUAL(result, TRUE);
    result = etchobj_is_assignable_from(&source, &target); 
    CU_ASSERT_EQUAL(result, TRUE);

    /* custom1obj = custom0obj */
    classarg_init_customtype_scalar(&target, CLASSID_CLASS_A, 0, 0, 0);
    classarg_init_customtype_scalar(&source, CLASSID_CLASS_B, 1, CLASSID_CLASS_A, 0);
    result = etchobj_is_assignable_from(&target, &source); 
    CU_ASSERT_EQUAL(result, TRUE);
    result = etchobj_is_assignable_from(&source, &target); 
    CU_ASSERT_EQUAL(result, FALSE);
    classarg_cleanup(&source, &target); 

    /* custom2obj = custom0obj */
    classarg_init_customtype_scalar(&target, CLASSID_CLASS_A, 0, 0, 0);
    classarg_init_customtype_scalar(&source, CLASSID_CLASS_C, 2, CLASSID_CLASS_B, CLASSID_CLASS_A);
    result = etchobj_is_assignable_from(&target, &source); 
    CU_ASSERT_EQUAL(result, TRUE);
    result = etchobj_is_assignable_from(&source, &target); 
    CU_ASSERT_EQUAL(result, FALSE);
    classarg_cleanup(&source, &target); 

    /* custom2obj = custom1obj */
    classarg_init_customtype_scalar(&target, CLASSID_CLASS_B, 1, CLASSID_CLASS_A, 0);
    classarg_init_customtype_scalar(&source, CLASSID_CLASS_C, 2, CLASSID_CLASS_B, CLASSID_CLASS_A);
    result = etchobj_is_assignable_from(&target, &source); 
    CU_ASSERT_EQUAL(result, TRUE);
    result = etchobj_is_assignable_from(&source, &target); 
    CU_ASSERT_EQUAL(result, FALSE);
    classarg_cleanup(&source, &target); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_assign_long_to_object
 */
void test_assign_long_to_object(void)
{
    char* objval = etch_malloc(128, ETCHTYPEB_BYTES);
    etch_object* class_object = new_etch_object(CLASSID_OBJECT, objval);
    etch_int64*  class_long   = new_int64(0);
    objmask* resultobj = NULL;
    int result = etchobj_is_assignable_fromobj((objmask*) class_object, (objmask*) class_long);
    CU_ASSERT_EQUAL_FATAL(result, TRUE);

    /* an assignment to object causes the source object to be 
     * wrapped by the target object */

    resultobj = etchobj_assign_to((objmask*) class_object, (objmask*) class_long);
    CU_ASSERT_PTR_EQUAL(resultobj, class_object);

    class_long->destroy(class_long);
    class_object->destroy(class_object);

    /* we destroy class_long allocated above, since the assignment caused
     * it to be wrapped by, but of course not consumed by, class_object.
     * that is, class_object now contains a reference to class_long, but
     * does not own the reference (destructor will not attempt to free it).
     * we do not destroy the character vector objval allocated above, since  
     * the assignment to class_object caused any content already owned by it,    
     * in this case objval, to be destroyed as part of the assignment.
     */

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */  
}


/**
 * test_invalid_array_assignment_1
 * ensure assignment fails when dimensions do not match
 */
void test_invalid_array_assignment_1(void)
{
    const int numdimbyte = 1, dim0byte = 4;
    const int numdimobj  = 2, dim0obj  = 4, dim1obj  = 2;
    objmask* resultobj = NULL;

    etch_nativearray* array_of_byte = new_nativearray 
        (CLASSID_ARRAY_BYTE, sizeof(byte), numdimbyte, dim0byte, 0, 0);  

    etch_nativearray* array_of_object = new_nativearray 
        (CLASSID_ARRAY_OBJECT, sizeof(void*), numdimobj, dim0obj, dim1obj, 0); 

    int result = etchobj_is_assignable_fromobj((objmask*) array_of_object, (objmask*) array_of_byte);
    CU_ASSERT_EQUAL_FATAL(result, FALSE);

    resultobj = etchobj_assign_to((objmask*) array_of_object, (objmask*) array_of_byte);
    CU_ASSERT_PTR_EQUAL(resultobj, NULL);

    array_of_byte->destroy(array_of_byte);
    array_of_object->destroy(array_of_object);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */  
}


/**
 * test_invalid_array_assignment_2
 * ensure assignment fails when types do not match
 */
void test_invalid_array_assignment_2(void)
{
    const int numdim = 1, dim0 = 4;
    objmask* resultobj = NULL;

    etch_nativearray* array_of_byte = new_nativearray 
        (CLASSID_ARRAY_BYTE, sizeof(byte), numdim, dim0, 0, 0);  

    etch_nativearray* array_of_bool = new_nativearray 
        (CLASSID_ARRAY_BOOL, sizeof(byte), numdim, dim0, 0, 0);  

    int result  = etchobj_is_assignable_fromobj((objmask*) array_of_bool, (objmask*) array_of_byte);
    CU_ASSERT_EQUAL(result, FALSE);
    result      = etchobj_is_assignable_fromobj((objmask*) array_of_byte, (objmask*) array_of_bool);
    CU_ASSERT_EQUAL(result, FALSE);

    resultobj = etchobj_assign_to((objmask*) array_of_bool, (objmask*) array_of_byte);
    CU_ASSERT_EQUAL(result, NULL);

    array_of_byte->destroy(array_of_byte);
    array_of_bool->destroy(array_of_bool);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */  
}


/**
 * test_assign_arrayofbyte_to_arrayofobject
 */
void test_assign_arrayofbyte_to_arrayofobject(void)
{
    const int numdimensions = 2, dim0count = 4, dim1count = 2;
    objmask* resultobj = NULL;

    etch_nativearray* array_of_byte = new_nativearray 
        (CLASSID_ARRAY_BYTE, sizeof(byte), numdimensions, dim0count, dim1count, 0);  

    etch_nativearray* array_of_object = new_nativearray 
        (CLASSID_ARRAY_OBJECT, sizeof(void*), numdimensions, dim0count, dim1count, 0); 

    /* an assignment of one array to the other replaces all array attributes 
     * and content in the target object. the target object will not own the 
     * array content of course */

    int result = etchobj_is_assignable_fromobj((objmask*) array_of_object, (objmask*) array_of_byte);
    CU_ASSERT_EQUAL_FATAL(result, TRUE);

    resultobj = etchobj_assign_to((objmask*) array_of_object, (objmask*) array_of_byte);
    CU_ASSERT_PTR_EQUAL(resultobj, array_of_object);

    array_of_byte->destroy(array_of_byte);
    array_of_object->destroy(array_of_object);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */  
}


/**
 * test_assign_verify_arraybyte_to_arrayobj
 * assign array of byte to array of object and verify content is identical
 */
void test_assign_verify_arraybyte_to_arrayobj(void)
{
    int   i = 0, j = 0, result = 0;
    char  x[2][4] = { {'a','b','c','d'}, {'e','f','g','h'}, }, thisx = 0;
    const int numdimensions = 2, dim0count = 4, dim1count = 2;
    objmask* resultobj = NULL;

    etch_nativearray* array_of_byte = new_nativearray 
        (CLASSID_ARRAY_BYTE, sizeof(byte), numdimensions, dim0count, dim1count, 0);   

    etch_nativearray* array_of_object = new_nativearray 
        (CLASSID_ARRAY_OBJECT, sizeof(void*), numdimensions, dim0count, dim1count, 0); 

    for(i = 0; i < dim1count; i++)    /* initialize array of byte */       
    {
        for(j = 0; j < dim0count; j++)      
        {                                  
            result = array_of_byte->put2(array_of_byte, &x[i][j], i, j); 
            CU_ASSERT_EQUAL(result, 0); 
        }
    }

    resultobj = etchobj_assign_to((objmask*) array_of_object, (objmask*) array_of_byte);
    CU_ASSERT_PTR_EQUAL_FATAL(resultobj, array_of_object);

    for(i = 0; i < dim1count; i++)         /* verify arrays are now the same */
    {
        for(j = 0; j < dim0count; j++)      
        {                                   
            result = array_of_object->get2(array_of_object, &thisx, i, j); 
            CU_ASSERT_EQUAL(result, 0); 
            CU_ASSERT_EQUAL(x[i][j], thisx); 
        }
    }

    array_of_byte->destroy(array_of_byte);
    array_of_object->destroy(array_of_object);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */  
}


/**
 * test_assign_arrayofbyte_to_arrayofbyte
 */
void test_assign_arrayofbyte_to_arrayofbyte(void)
{
    const int numdimensions = 2, dim0count = 4, dim1count = 2;
    objmask* resultobj = NULL;

    etch_nativearray* array1_of_byte = new_nativearray 
        (CLASSID_ARRAY_BYTE, sizeof(byte), numdimensions, dim0count, dim1count, 0);  

    etch_nativearray* array2_of_byte = new_nativearray 
        (CLASSID_ARRAY_BYTE, sizeof(byte), numdimensions, dim0count, dim1count, 0);  

    int result = etchobj_is_assignable_fromobj((objmask*) array1_of_byte, (objmask*) array2_of_byte);
    CU_ASSERT_EQUAL_FATAL(result, TRUE);
    result     = etchobj_is_assignable_fromobj((objmask*) array2_of_byte, (objmask*) array1_of_byte);
    CU_ASSERT_EQUAL_FATAL(result, TRUE);

    resultobj = etchobj_assign_to((objmask*) array1_of_byte, (objmask*) array2_of_byte);
    CU_ASSERT_PTR_EQUAL(resultobj, array1_of_byte);

    resultobj = etchobj_assign_to((objmask*) array2_of_byte, (objmask*) array1_of_byte);
    CU_ASSERT_PTR_EQUAL(resultobj, array2_of_byte);

    array1_of_byte->destroy(array1_of_byte);
    array2_of_byte->destroy(array2_of_byte);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */  
}


/**
 * test_assign_verify_arraybyte_to_arraybyte
 * assign array of byte to array of byte and verify content is identical
 */
void test_assign_verify_arraybyte_to_arraybyte(void)
{
    int   i = 0, j = 0, result = 0;
    char  x[2][4] = { {'a','b','c','d'}, {'e','f','g','h'}, }, thisx = 0;
    char  y[2][4] = { {'s','t','u','v'}, {'w','x','y','z'}, };
    const int numdimensions = 2, dim0count = 4, dim1count = 2;
    objmask* resultobj = NULL;

    etch_nativearray* array1_of_byte = new_nativearray 
        (CLASSID_ARRAY_BYTE, sizeof(byte), numdimensions, dim0count, dim1count, 0);   

    etch_nativearray* array2_of_byte = new_nativearray 
        (CLASSID_ARRAY_BYTE, sizeof(byte), numdimensions, dim0count, dim1count, 0); 

    for(i = 0; i < dim1count; i++)    /* initialize both arrays */       
    {
        for(j = 0; j < dim0count; j++)      
        {                                  
            result = array1_of_byte->put2(array1_of_byte, &x[i][j], i, j); 
            CU_ASSERT_EQUAL(result, 0); 

            result = array2_of_byte->put2(array2_of_byte, &y[i][j], i, j); 
            CU_ASSERT_EQUAL(result, 0); 
        }
    }

    resultobj = etchobj_assign_to((objmask*) array1_of_byte, (objmask*) array2_of_byte);
    CU_ASSERT_PTR_EQUAL_FATAL(resultobj, array1_of_byte);

    for(i = 0; i < dim1count; i++)   /* verify assignment */
    {
        for(j = 0; j < dim0count; j++)      
        {                                   
            result = array1_of_byte->get2(array1_of_byte, &thisx, i, j); 
            CU_ASSERT_EQUAL(result, 0); 
            CU_ASSERT_EQUAL(y[i][j], thisx); 
        }
    }

    array1_of_byte->destroy(array1_of_byte);
    array2_of_byte->destroy(array2_of_byte);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);   
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  
}


/**
 * test_assign_derived_2
 * test that inheritance type 2-derived object can be assigned to its parent
 */
void test_assign_derived_2(void)
{
    class_a* class_parent = NULL;
    class_b* class_child  = NULL;
    objmask* resultobj = NULL;
    int result = 0;

    class_parent = new_class_a(L"it works!");
    class_child  = new_class_b(class_parent, 128);
 
    CU_ASSERT_PTR_NOT_NULL_FATAL(class_child->parent);
    CU_ASSERT_PTR_NULL(class_parent->parent);

    result = etchobj_is_assignable_fromobj((objmask*) class_child, (objmask*) class_parent);
    CU_ASSERT_EQUAL(result, FALSE);

    result = etchobj_is_assignable_fromobj((objmask*) class_parent, (objmask*) class_child);
    CU_ASSERT_EQUAL(result, TRUE);

    /* these objects use type 2 inheritance, in which inherited objects chain to
     * instantiated parent objects. for such objects, when we assign child class
     * to parent class, the result of the assignment couldl be a different memory
     * reference than the requested target. however in this case, the object we 
     * are assigning to is in fact the same object as is in the source object's
     * inheritance chain (since we passed it to the object's constructor).
     * is not known at this writing whether this scenario can exist in etch c; 
     * that is, if we will ever use this inheritance model in conjunction with
     * the assignment of child to parent.  
     */
    resultobj = etchobj_assign_to((objmask*) class_parent, (objmask*) class_child);
    CU_ASSERT_PTR_NOT_NULL_FATAL(resultobj);
    CU_ASSERT_EQUAL(resultobj->class_id, class_parent->class_id); 

    /* an object destructor will recursively destroy() its superclass instances */
    class_child->destroy(class_child);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_assign_derived_1a
 * test that inheritance type 2-derived object can be assigned to its parent
 */
void test_assign_derived_2a(void)
{
    class_a* class_parent = NULL;
    class_b* class_child1 = NULL;
    class_c* class_child2 = NULL;
    objmask* resultobj = NULL;
    int result = 0;

    class_parent = new_class_a(L"it works!");
    class_child1 = new_class_b(class_parent, 128);
    class_child2 = new_class_c(class_child1);
 
    CU_ASSERT_PTR_NOT_NULL_FATAL(class_child2->parent);
    CU_ASSERT_PTR_NOT_NULL_FATAL(class_child1->parent);
    CU_ASSERT_PTR_NULL(class_parent->parent);

    result = etchobj_is_assignable_fromobj((objmask*) class_child2, (objmask*) class_parent);
    CU_ASSERT_EQUAL(result, FALSE);

    result = etchobj_is_assignable_fromobj((objmask*) class_parent, (objmask*) class_child2);
    CU_ASSERT_EQUAL(result, TRUE);

    /* see comments at test_assign_derived_2() - they apply again here */
    resultobj = etchobj_assign_to((objmask*) class_parent, (objmask*) class_child2);
    CU_ASSERT_PTR_NOT_NULL_FATAL(resultobj);
    CU_ASSERT_EQUAL(resultobj->class_id, class_parent->class_id); 

    /* an object destructor will recursively destroy() its superclass instances */ 
    class_child2->destroy(class_child2);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_assign_derived_1
 * test that inheritance type 1-derived object can be assigned to its parent
 */
void test_assign_derived_1(void)
{
    class_x* class_parent = NULL;
    class_y* class_child  = NULL;
    objmask* resultobj = NULL;
    const int PARENT_CLASSX_DATA = 1, CHILD_CLASSX_DATA = 1000, CHILD_CLASSY_DATA = 1001;
    int result = 0;

    class_parent = new_class_x(PARENT_CLASSX_DATA);
    class_child  = new_class_y(CHILD_CLASSX_DATA, CHILD_CLASSY_DATA);
 
    CU_ASSERT_PTR_NOT_NULL_FATAL(class_parent);
    CU_ASSERT_PTR_NOT_NULL_FATAL(class_child);

    result = etchobj_is_assignable_fromobj((objmask*) class_child, (objmask*) class_parent);
    CU_ASSERT_EQUAL(result, FALSE);

    result = etchobj_is_assignable_fromobj((objmask*) class_parent, (objmask*) class_child);
    CU_ASSERT_EQUAL(result, TRUE);

    /* these objects use type 1 inheritance, in which inherited objects contain
     * their parent's instance data;. there is no physical chaining of objects.
     * rather inheritance is identified from the object vtable's inheritance list. 
     */
    resultobj = etchobj_assign_to((objmask*) class_parent, (objmask*) class_child);

    CU_ASSERT_PTR_NOT_NULL_FATAL(resultobj);
    CU_ASSERT_EQUAL(resultobj->class_id, class_parent->class_id); 
    /* verify that parent's instance data was replaced with that of the child */
    CU_ASSERT_EQUAL(((class_x*)resultobj)->class_x_instance_data, class_child->class_x_instance_data); 

    class_child->destroy(class_child);
    class_parent->destroy(class_parent);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * main   
 */
int _tmain(int argc, _TCHAR* argv[])
{
    char c=0;
    CU_pSuite ps = NULL;
    g_is_automated_test = argc > 1 && 0 != wcscmp(argv[1], L"-a");
    if (CUE_SUCCESS != CU_initialize_registry()) return 0;
    CU_set_output_filename("../test_etchobject");
    ps = CU_add_suite("etchobject test suite", init_suite, clean_suite);
    etch_watch_id = 0; 

    CU_add_test(ps, "test assignability",    test_is_assignable_1); 
    CU_add_test(ps, "assign long to object", test_assign_long_to_object); 
    CU_add_test(ps, "attempt invalid array assignment 1", test_invalid_array_assignment_1); 
    CU_add_test(ps, "attempt invalid array assignment 2", test_invalid_array_assignment_2); 
    CU_add_test(ps, "assign byte[][] to object[][]", test_assign_arrayofbyte_to_arrayofobject); 
    CU_add_test(ps, "assign byte[][] to object[][] and verify", test_assign_verify_arraybyte_to_arrayobj);
    CU_add_test(ps, "assign byte[][] to byte[][]", test_assign_arrayofbyte_to_arrayofbyte); 
    CU_add_test(ps, "assign byte[][] to byte[][] and verify", test_assign_verify_arraybyte_to_arraybyte); 
    CU_add_test(ps, "assign child to parent (type 2)", test_assign_derived_2); 
    CU_add_test(ps, "assign 2x child to parent (type 2)", test_assign_derived_2a); 
    CU_add_test(ps, "assign child to parent (type 1)", test_assign_derived_1); 

    if (g_is_automated_test)    
        CU_automated_run_tests();    
    else
    {   CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    }

    if (!g_is_automated_test) { printf("any key ..."); while(!c) c = _getch(); wprintf(L"\n"); }     
    CU_cleanup_registry();
    return CU_get_error(); 
}

