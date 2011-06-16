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
 * test_defvalufact.c 
 * test default impl of value factory
 */
#include "etch_runtime.h"
#include "etch_validator.h"  /* must be included second */
#include "etch_tagged_data.h"
#include "etch_map.h"
#include "etch_log.h"
#include "etch_id_name.h"
#include "etch_default_value_factory.h"
#include "etch_arrayval.h"
#include "etch_exception.h"
#include "etch_objecttypes.h"
#include "etch_nativearray.h"
#include "etch_serializer.h"

#include <stdio.h>
#include "CUnit.h"
#include <wchar.h>

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

/**
 * do_test_type_field
 */
static int do_test_type_field(default_value_factory* vf, etch_type* type, etch_field* key, etch_object* value)
{
    etch_message* msg = new_message(type, 1, (etch_value_factory*)vf);

    const int result = etch_message_put(msg, key, value);

    etch_object_destroy(msg);

    return result; 
}


/* - - - - - - - - - - - - - - 
 * unit tests
 * - - - - - - - - - - - - - -
 */

/**
 * test_constructor
 * tests that all objects instatiated with a value factory such as builtin types 
 * and associated validators, are freed as expected. 
 */
static void test_constructor(void)
{
    vf_idname_map*          typemap = NULL;
    class_to_type_map*      c2tmap  = NULL;
    default_value_factory*  vf      = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    defvf_initialize_static(typemap, c2tmap);
    vf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL_FATAL(vf);

    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);

    // TODO: cleanup statics
    //etchvf_free_builtins();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_builtin_types()
 * test that the vf contains all expected default types
 */
static void test_builtin_types(void)
{
    int errs = 0;
    vf_idname_map*          typemap = NULL;
    class_to_type_map*      c2tmap  = NULL;
    default_value_factory*  vf      = NULL;
    struct i_value_factory* vtab    = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    // init statics
    defvf_initialize_static(typemap, c2tmap);
    vf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf);
    vtab = (struct i_value_factory*)((etch_object*)vf)->vtab;
    if (NULL == vtab->get_type_by_name(vf, builtins._mt__etch_auth_exception->name))
        errs++; 
    if (NULL == vtab->get_type_by_name(vf, builtins._mt__etch_datetime->name))
        errs++; 
    if (NULL == vtab->get_type_by_name(vf, builtins._mt__etch_list->name))
        errs++; 
    if (NULL == vtab->get_type_by_name(vf, builtins._mt__etch_map->name))
        errs++; 
    if (NULL == vtab->get_type_by_name(vf, builtins._mt__etch_runtime_exception->name))
        errs++; 
    if (NULL == vtab->get_type_by_name(vf, builtins._mt__etch_set->name))
        errs++; 
    if (NULL == vtab->get_type_by_name(vf, builtins._mt__exception->name))
        errs++; 

    CU_ASSERT_EQUAL(errs, 0);  

    etch_object_destroy(vf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);

    // TODO: cleanup statics
    //etchvf_free_builtins();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_add_type_a()
 * test that a type can be added and its memory automatically freed.
 */
static void test_add_type_a(void)
{
    vf_idname_map*          typemap  = NULL;
    class_to_type_map*      c2tmap   = NULL;
    default_value_factory*  dvf      = NULL;
    etch_type*              newtype  = NULL;
    etch_type*              efftype  = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    // init statics
    defvf_initialize_static(typemap, c2tmap);

    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    newtype = new_static_type(L"testtype");
    efftype = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->add_type(dvf, newtype);  /* not owned by vf */
    CU_ASSERT_PTR_EQUAL(newtype, efftype);

    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    destroy_static_type(efftype); /* since not owned by vf */

    // TODO: cleanup statics
    //etchvf_free_builtins();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_add_type_b()
 * test that a duplicate type is rejected as expected.
 */
static void test_add_type_b(void)
{
    vf_idname_map*          typemap  = NULL;
    class_to_type_map*      c2tmap   = NULL;
    default_value_factory*  dvf      = NULL;
    wchar_t* samestring              = L"sametype";
    etch_type*              newtype1 = NULL;
    etch_type*              newtype2 = NULL;
    etch_type*              efftype  = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    // init statics
    defvf_initialize_static(typemap, c2tmap);

    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    newtype1 = new_static_type(samestring);
    newtype2 = new_static_type(samestring);

    efftype = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->add_type(dvf, newtype1);  /* not owned by vf */
    CU_ASSERT_PTR_EQUAL(efftype, newtype1);

    /* when try to add a duplicate, vf returns original and destroys duplicate */
    efftype = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->add_type(dvf, newtype2);  
    CU_ASSERT_PTR_EQUAL(efftype, newtype1);


    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    /* detroy newtype1 since not owned by vf */
    destroy_static_type(newtype1);
    /* don't destroy newtype2 since vf destroyed it */

    // TODO: cleanup statics
    //etchvf_free_builtins();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_add_type_c()
 * test that a duplicate of a builtin type is handled as expected.
 */
static void test_add_type_c(void)
{
    vf_idname_map*          typemap  = NULL;
    class_to_type_map*      c2tmap   = NULL;
    default_value_factory*  dvf      = NULL;
    etch_type*              duptype  = NULL;
    etch_type*              efftype  = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    // init statics
    defvf_initialize_static(typemap, c2tmap);
    
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    duptype = new_type(str_etch_runtime_exception);

    /* when try to add a duplicate, vf returns original and destroys duplicate */
    efftype = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->add_type(dvf, duptype);
    CU_ASSERT_PTR_EQUAL(efftype, builtins._mt__etch_runtime_exception);

    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);

    // TODO: cleanup statics
    //etchvf_free_builtins();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_get_type_by_id()
 * test that a type can be retrived by its id, and memory accounted for.
 */
static void test_get_type_by_id(void)
{
    vf_idname_map*          typemap  = NULL;
    class_to_type_map*      c2tmap   = NULL;
    default_value_factory*  dvf      = NULL;
   /* note that a *reference* is returned (not disposable), not a copy. */
    unsigned saved_id = 0;
    etch_type*              newtype   = NULL;
    etch_type*              foundtype = NULL;
    
    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    // init statics
    defvf_initialize_static(typemap, c2tmap);

    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    newtype  = new_type(L"testtype");
    saved_id = newtype->id;
    ((struct i_value_factory*)((etch_object*)dvf)->vtab)->add_type(dvf, newtype);

    foundtype = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_type_by_id(dvf, saved_id);
    CU_ASSERT_PTR_NOT_NULL_FATAL(foundtype);
    CU_ASSERT_EQUAL(saved_id, foundtype->id);

    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);

    etch_object_destroy(newtype);   /* vf now owns its nonstatic user types */

    // TODO: cleanup statics
    //etchvf_free_builtins();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_get_type_by_name()
 * test that a type can be retrived by its name, and memory accounted for.
 */
static void test_get_type_by_name(void)
{
    vf_idname_map*          typemap  = NULL;
    class_to_type_map*      c2tmap   = NULL;
    default_value_factory*  dvf      = NULL;
    /* note that a *reference* is returned (not disposable), not a copy. */
    etch_type*              newtype   = NULL;
    etch_type*              foundtype = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    // init statics
    defvf_initialize_static(typemap, c2tmap);

    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    newtype = new_type(L"testtype");
    ((struct i_value_factory*)((etch_object*)dvf)->vtab)->add_type(dvf, newtype);

    foundtype = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_type_by_name(dvf, L"testtype");
    CU_ASSERT_PTR_NOT_NULL_FATAL(foundtype);

    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);

    etch_object_destroy(newtype);   /* vf now owns its nonstatic user types */

    // TODO: cleanup statics
    //etchvf_free_builtins();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_get_types()
 * test that all types can be retrived as a list, and memory accounted for.
 */
static void test_get_types(void)
{
    int  result;
    vf_idname_map*      typemap    = NULL;
    class_to_type_map*  c2tmap     = NULL;
    default_value_factory*  dvf    = NULL;
    etch_hashtable*     map        = NULL;
    etch_iterator       iterator;
    etch_arraylist*     typeslist  = NULL;
    etch_hashitem       hashbucket;
    etch_hashitem*      thisitem   =  &hashbucket;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    // init statics
    defvf_initialize_static(typemap, c2tmap);

    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    // get type map
    map = dvf->types;
    // get type list
    typeslist = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_types(dvf);

    set_iterator(&iterator, typeslist, &typeslist->iterable);

    while(iterator.has_next(&iterator))
    {
        /* we're iterating the arraylist, so we're looking at the value,
         * which of course was the hashtable key */
        etch_type* thistype = (etch_type*) iterator.current_value;
        CU_ASSERT_PTR_NOT_NULL_FATAL(thistype); 

        /* map.remove() sends us back the key and value if we ask for it. so we 
         * are using remove both to look up the type, and to remove it so we can
         * verify that the list content is one to one with the types map */
        result = ((struct i_hashtable*)((etch_object*)map)->vtab)->removeh
	  (map->realtable, ((etch_object*)thistype)->get_hashkey(thistype), map, (void**)&thisitem);
        CU_ASSERT_EQUAL(result, 0); 

        result = etchmap_count(map);
        iterator.next(&iterator);
    }

    /* assert that list contnent and map content were the same */
    result = etchmap_count(map);
    CU_ASSERT_EQUAL(result, 0);

    /* the list is marked to not free content, which is what we want, however
     * if would not make a difference if it were not, since the types are refs
     * to the builtin types, thus marked static, and so were the list to invoke
     * destructors on its content, the destructor would take no action.
     */
    etch_object_destroy(typeslist);

    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);



    // TODO: cleanup statics
    //etchvf_free_builtins();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_authxcp_fields()
 * test that auth exception type is configured as expected
 */
static void test_authxcp_fields(void)
{
    int result = 0;
    vf_idname_map*      typemap    = NULL;
    class_to_type_map*  c2tmap     = NULL;
    default_value_factory*  dvf     = NULL;
    etch_type*              msgtype = NULL;
    etch_field*             msgkey1 = NULL;
    etch_field*             msgkey2 = NULL;
    etch_object*                val_good = NULL;
    etch_object*                val_unxp = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    // init statics
    defvf_initialize_static(typemap, c2tmap);

    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    /* apparently the purpose of this test, ported from java test, is to test that
     * the authxcp type has a _mf_msg field, and that if we then make a message
     * of type authxcp and put to it a string value keyed by _mf_msg, that it
     * validates OK. i.e., the _mf_msg field of a authxcp should be a string.
     */
    msgtype  = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_type_by_name(dvf, str_etch_auth_exception); 
    msgkey1  = clone_field(builtins._mf_msg); 
    msgkey2  = clone_field(builtins._mf_msg); 
    val_good = (etch_object*) new_stringw(L"dlrow olleh");
    val_unxp = (etch_object*) new_int32(0);

    result = do_test_type_field(dvf, msgtype, msgkey1, val_good);
    CU_ASSERT_EQUAL(result, 0); 

    result = do_test_type_field(dvf, msgtype, msgkey2, val_unxp);
    CU_ASSERT_EQUAL(result, -1); /* type validation should fail */  


    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);


    // TODO: cleanup statics
    //etchvf_free_builtins();

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
} 


/**
 * test_rtxcp_fields()
 * test that runtime exception type is configured as expected
 */
static void test_rtxcp_fields(void)
{
    int result = 0;
    vf_idname_map*      typemap    = NULL;
    class_to_type_map*  c2tmap     = NULL;
    default_value_factory*  dvf     = NULL;
    etch_type*              msgtype = NULL;
    etch_field*             msgkey1 = NULL;
    etch_field*             msgkey2 = NULL;
    etch_object*                val_good = NULL;
    etch_object*                val_unxp = NULL;
    
    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    // init statics
    defvf_initialize_static(typemap, c2tmap);

    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    msgtype  = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_type_by_name(dvf, str_etch_runtime_exception); 
    msgkey1  = clone_field(builtins._mf_msg); 
    msgkey2  = clone_field(builtins._mf_msg); 
    val_good = (etch_object*) new_stringw(L"dlrow olleh");
    val_unxp = (etch_object*) new_int32(0);

    result = do_test_type_field(dvf, msgtype, msgkey1, val_good);
    CU_ASSERT_EQUAL(result, 0); 

    result = do_test_type_field(dvf, msgtype, msgkey2, val_unxp);
    CU_ASSERT_EQUAL(result, -1); /* type validation should fail */  

    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);

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
 * test_exception_fields()
 * test that exception type is configured as expected
 */
static void test_exception_fields(void)
{
    int result = 0;
    vf_idname_map*      typemap        = NULL;
    class_to_type_map*  c2tmap         = NULL;
    default_value_factory*  dvf        = NULL;
    etch_type*              msgtype    = NULL;
    etch_field*             key_result = NULL;
    etch_field*             key_msg_id = NULL;
    etch_field*             key_inrepto = NULL;
    etch_object*                val_rtexp   = NULL;
    etch_object*                val_long1   = NULL;
    etch_object*                val_long2   = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    // init statics
    defvf_initialize_static(typemap, c2tmap);

    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    msgtype     = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_type_by_name(dvf, str_exception); 
    key_result  = clone_field(builtins._mf_result); 
    key_msg_id  = clone_field(builtins._mf__message_id); 
    key_inrepto = clone_field(builtins._mf__in_reply_to); 
    val_rtexp   = (etch_object*) new_etch_exception_from_errorcode(ETCH_ERROR);
    val_long1   = (etch_object*) new_int64(123);
    val_long2   = (etch_object*) new_int64(456);

    result = do_test_type_field(dvf, msgtype, key_result, val_rtexp);
    CU_ASSERT_EQUAL(result, 0); 

    result = do_test_type_field(dvf, msgtype, key_msg_id, val_long1);
    CU_ASSERT_EQUAL(result, 0); 

    result = do_test_type_field(dvf, msgtype, key_inrepto, val_long2);
    CU_ASSERT_EQUAL(result, 0); 

    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);

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
 * test_get_string_encoding()
 */
static void test_get_string_encoding(void)
{
    vf_idname_map*          typemap    = NULL;
    class_to_type_map*      c2tmap     = NULL;
    default_value_factory*  dvf        = NULL;
    wchar_t*                encoding   = NULL; 
    etch_string*            newstring  = NULL;
    int  result;
    int etch_encoding;
    char* teststring                   = "8 bit characters";

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    // init statics
    defvf_initialize_static(typemap, c2tmap);

    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    /* note that a reference to a raw string is returned */
    encoding = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_string_encoding(dvf);

    result = wcscmp(encoding, L"utf-8");
    CU_ASSERT_EQUAL(result, 0);

    etch_encoding = get_etch_string_encoding((etch_value_factory*)dvf);

    newstring = new_string("8 bit characters", etch_encoding);
    CU_ASSERT_PTR_NOT_NULL_FATAL(newstring);
    result = strcmp(newstring->v.valc, teststring);
    CU_ASSERT_EQUAL(result, 0);  
    
    etch_object_destroy(newstring);

    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);

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
 * test_add_mixin()
 */
static void test_add_mixin(void)
{
    int result = 0;
    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    
    default_value_factory*  mixvf1_vf       = NULL;
    vf_idname_map*          mixvf1_typemap  = NULL;
    class_to_type_map*      mixvf1_c2tmap   = NULL;
    
    default_value_factory*  mixvf2_vf      = NULL;
    vf_idname_map*          mixvf2_typemap  = NULL;
    class_to_type_map*      mixvf2_c2tmap   = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    // init statics
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    mixvf1_typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(mixvf1_typemap);
    mixvf1_c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(mixvf1_c2tmap);
    // init statics
    defvf_initialize_static(mixvf1_typemap, mixvf1_c2tmap);
    mixvf1_vf = new_default_value_factory(mixvf1_typemap, mixvf1_c2tmap);
    CU_ASSERT_PTR_NOT_NULL(mixvf1_vf);

    mixvf2_typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(mixvf2_typemap);
    mixvf2_c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(mixvf2_c2tmap);
    // init statics
    defvf_initialize_static(mixvf2_typemap, mixvf2_c2tmap);
    mixvf2_vf = new_default_value_factory(mixvf2_typemap, mixvf2_c2tmap);
    CU_ASSERT_PTR_NOT_NULL(mixvf2_vf);

    result = defvf_add_mixin(dvf, (etch_value_factory*) mixvf1_vf);
    CU_ASSERT_EQUAL(result, 0);  
    result = defvf_add_mixin(dvf, (etch_value_factory*) mixvf2_vf);
    CU_ASSERT_EQUAL(result, 0);  
 
    result = dvf->mixins->count;
    CU_ASSERT_EQUAL(result, 2);  

    // destroy dvf
    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_mixin_recursion
 * exercise the various vf apis which are mixin-recursive
 */
static void test_mixin_recursion(void)
{
    int result = 0;
    etch_type* rettype = NULL;
    wchar_t* str_foo = L"foo", *str_bar = L"bar", *str_bogus = L"wtf";
    etch_type* newtype1 = NULL;
    etch_type* newtype2 = NULL;


    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    default_value_factory*  mixin1_dvf      = NULL;
    vf_idname_map*          mixin1_typemap  = NULL;
    class_to_type_map*      mixin1_c2tmap   = NULL;

    default_value_factory*  mixin2_dvf      = NULL;
    vf_idname_map*          mixin2_typemap  = NULL;
    class_to_type_map*      mixin2_c2tmap   = NULL;

    // main vf
    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    // mixin1 vf
    mixin1_typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(mixin1_typemap);
    mixin1_c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(mixin1_c2tmap);
    defvf_initialize_static(mixin1_typemap, mixin1_c2tmap);
    mixin1_dvf = new_default_value_factory(mixin1_typemap, mixin1_c2tmap);
    CU_ASSERT_PTR_NOT_NULL(mixin1_dvf);

    // mixin2 vf
    mixin2_typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(mixin2_typemap);
    mixin2_c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(mixin2_c2tmap);
    defvf_initialize_static(mixin2_typemap, mixin2_c2tmap);
    mixin2_dvf = new_default_value_factory(mixin2_typemap, mixin2_c2tmap);
    CU_ASSERT_PTR_NOT_NULL(mixin2_dvf);

    newtype1 = new_static_type(str_foo);
    newtype2 = new_static_type(str_bar);
    
    rettype =  ((struct i_value_factory*)((etch_object*)mixin1_dvf)->vtab)->add_type(mixin1_dvf, newtype1);
    CU_ASSERT_PTR_EQUAL_FATAL(rettype, newtype1);

    rettype = ((struct i_value_factory*)((etch_object*)mixin2_dvf)->vtab)->add_type(mixin2_dvf, newtype2);
    CU_ASSERT_PTR_EQUAL_FATAL(rettype, newtype2);

    result = defvf_add_mixin(dvf, (etch_value_factory*) mixin1_dvf);
    CU_ASSERT_EQUAL(result, 0);  
    result = defvf_add_mixin(dvf, (etch_value_factory*) mixin2_dvf);
    CU_ASSERT_EQUAL(result, 0);  

    result = defvf_add_mixin(mixin1_dvf, (etch_value_factory*) mixin2_dvf);
    CU_ASSERT_EQUAL(result, 0);

    rettype = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_type_by_name(dvf, str_exception);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* builtin type */

    rettype = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_type_by_name(mixin1_dvf, str_exception);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* builtin type */

    rettype = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_type_by_name(mixin2_dvf, str_exception);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* builtin type */

    /* mixin recursive by-name lookup */
    rettype = ((struct i_value_factory*)((etch_object*)mixin1_dvf)->vtab)->get_type_by_name(mixin1_dvf, str_bar);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* via mixvf1 mixin 1 */

    rettype = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_type_by_name(dvf, str_foo);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* via vf mixin 1 */ 

    rettype = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_type_by_name(dvf, str_bar);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* via vf mixin 2 */ 

    /* mixin recursive by-id lookup */
    rettype = ((struct i_value_factory*)((etch_object*)mixin1_dvf)->vtab)->get_type_by_id(mixin1_dvf, newtype2->id);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* via mixvf1 mixin 1 */

    rettype = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_type_by_id(dvf, newtype1->id);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* via vf mixin 1 */ 

    rettype = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_type_by_id(dvf, newtype2->id);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* via vf mixin 2 */ 

    /* negative tests will recurse all mixins */
    rettype =  ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_type_by_name(mixin1_dvf, str_bogus);
    CU_ASSERT_PTR_NULL(rettype);

    rettype = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_type_by_id(dvf, compute_id_name_id_from_widename(str_bogus));
    CU_ASSERT_PTR_NULL(rettype);  /* via vf mixin 1 */  

    destroy_static_type(newtype1);
    destroy_static_type(newtype2);
    
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
 * test_custom_struct_type
 *  
 */
static void test_custom_struct_type(void)
{
    int result = 0;
    etch_type* rettype = NULL;
    wchar_t *str_vf0 = L"vf0", *str_vf1 = L"vf1", *str_vf2 = L"vf2";
    
    default_value_factory*  vf0             = NULL;
    vf_idname_map*          vf0_typemap     = NULL;
    class_to_type_map*      vf0_c2tmap      = NULL;

    default_value_factory*  vf1             = NULL;
    vf_idname_map*          vf1_typemap     = NULL;
    class_to_type_map*      vf1_c2tmap      = NULL;

    default_value_factory*  vf2             = NULL;
    vf_idname_map*          vf2_typemap     = NULL;
    class_to_type_map*      vf2_c2tmap      = NULL;

    etch_type* cstype0 = new_static_type(str_vf0);
    etch_type* cstype1 = new_static_type(str_vf1);
    etch_type* cstype2 = new_static_type(str_vf2);

    const unsigned short ETCHTYPE_CSCLASS0 = 0xe0, CLASSID_CSCLASS0 = 0xf0;
    const unsigned short ETCHTYPE_CSCLASS1 = 0xe1, CLASSID_CSCLASS1 = 0xf1;
    const unsigned short ETCHTYPE_CSCLASS2 = 0xe2, CLASSID_CSCLASS2 = 0xf2;
    const unsigned short ETCHTYPE_CSBOGUS  = 0xe3, CLASSID_BOGUS    = 0xf3;

    const unsigned csclass0 = ETCHMAKECLASS(ETCHTYPE_CSCLASS0, CLASSID_CSCLASS0);
    const unsigned csclass1 = ETCHMAKECLASS(ETCHTYPE_CSCLASS1, CLASSID_CSCLASS1);
    const unsigned csclass2 = ETCHMAKECLASS(ETCHTYPE_CSCLASS2, CLASSID_CSCLASS2);
    const unsigned csbogus  = ETCHMAKECLASS(ETCHTYPE_CSBOGUS,  CLASSID_BOGUS);

    vf0_typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(vf0_typemap);
    vf0_c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(vf0_c2tmap);
    defvf_initialize_static(vf0_typemap, vf0_c2tmap);
    vf0 = new_default_value_factory(vf0_typemap, vf0_c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf0);
    result = class_to_type_map_put(vf0->class_to_type, csclass0, cstype0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    vf1_typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(vf1_typemap);
    vf1_c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(vf1_c2tmap);
    defvf_initialize_static(vf1_typemap, vf1_c2tmap);
    vf1 = new_default_value_factory(vf1_typemap, vf1_c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf1);
    result = class_to_type_map_put(vf1->class_to_type, csclass1, cstype1);
    CU_ASSERT_EQUAL_FATAL(result,0);

    vf2_typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(vf2_typemap);
    vf2_c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(vf2_c2tmap);
    defvf_initialize_static(vf2_typemap, vf2_c2tmap);
    vf2 = new_default_value_factory(vf2_typemap, vf2_c2tmap);
    CU_ASSERT_PTR_NOT_NULL(vf2);
    result = class_to_type_map_put(vf2->class_to_type, csclass2, cstype2);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = defvf_add_mixin(vf0, (etch_value_factory*) vf1);
    CU_ASSERT_EQUAL(result, 0);
    result = defvf_add_mixin(vf0, (etch_value_factory*) vf2);
    CU_ASSERT_EQUAL(result, 0);

    result = defvf_add_mixin(vf1, (etch_value_factory*) vf2);
    CU_ASSERT_EQUAL(result, 0);  

    rettype = ((struct i_value_factory*)((etch_object*)vf0)->vtab)->get_custom_struct_type(vf0, csclass0);
    CU_ASSERT_PTR_EQUAL(rettype, cstype0);  

    /* recurse mixed-in vfs for custom types */
    rettype = ((struct i_value_factory*)((etch_object*)vf0)->vtab)->get_custom_struct_type(vf0, csclass1);
    CU_ASSERT_PTR_EQUAL(rettype, cstype1); 
  
    rettype = ((struct i_value_factory*)((etch_object*)vf0)->vtab)->get_custom_struct_type(vf0, csclass2);
     CU_ASSERT_PTR_EQUAL(rettype, cstype2); 

    /* negative test will recurse all mixins */
    rettype = ((struct i_value_factory*)((etch_object*)vf0)->vtab)->get_custom_struct_type(vf0, csbogus);
    CU_ASSERT_PTR_NULL(rettype);   

    destroy_static_type(cstype0);
    destroy_static_type(cstype1);
    destroy_static_type(cstype2);

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
 * test_get_set_message_id()
 */
static void test_get_set_message_id(void)
{
    int result = 0;
    int64 idvalue = 12345;
    etch_type* newtype = new_static_type(L"testtype");
    etch_int64* idobj  = new_int64(idvalue), *retobj = NULL;
    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
     etch_message*          msg             = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    msg = new_message(newtype, ETCH_DEFSIZE, (etch_value_factory*) dvf);

    etchtype_put_validator(newtype, builtins._mf__message_id, (etch_object*) etchvtor_int64_get(0));
    
    result = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->set_message_id(dvf, msg, idobj);  /* we no longer own idobj */
    CU_ASSERT_EQUAL_FATAL(result,0);  /* ensure idobj validates ok */

    /* we get back a reference to the ID object, not a copy */
    retobj = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_message_id(dvf, msg); 

    CU_ASSERT_PTR_NOT_NULL_FATAL(retobj); 
    CU_ASSERT_EQUAL(retobj->value, idvalue); 

    etch_object_destroy(msg);
    
    // destroy vf
    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);

    destroy_static_type(newtype);

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
 * test_get_set_in_reply_to()
 */
static void test_get_set_in_reply_to(void)
{
    int result = 0;
    int64 idvalue = 12345;
    etch_type* newtype = new_static_type(L"testtype");
    etch_int64* idobj  = new_int64(idvalue), *retobj = NULL;
    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    etch_message*           msg             = NULL;
    
    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    msg = new_message(newtype, ETCH_DEFSIZE, (etch_value_factory*) dvf);

    etchtype_put_validator(newtype, builtins._mf__in_reply_to, (etch_object*) etchvtor_int64_get(0));
    
    result = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->set_in_reply_to(dvf, msg, idobj);
    CU_ASSERT_EQUAL_FATAL(result,0);  /* ensure idobj validates ok */

    /* we get back a reference to the ID, not a copy */
    retobj = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->get_in_reply_to(dvf, msg); 

    CU_ASSERT_PTR_NOT_NULL_FATAL(retobj); 
    CU_ASSERT_EQUAL(retobj->value, idvalue); 

    etch_object_destroy(msg);
    
    // destroy vf
    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
    destroy_static_type(newtype);

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
 * test_export_custom_value_exception()
 * export an etch_exception
 */
static void test_export_custom_value_exception(void)
{
    int result = 0;
    etch_structvalue* sv = NULL;
    etch_serializer* impexhelper = NULL;
    etch_string* exported_stringobj = NULL;
    etch_exception* nullptr_exception = NULL;
    wchar_t* exception_string_out = NULL;

    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    nullptr_exception = new_etch_exception_from_errorcode(ETCH_EINVAL); 

    /* retain ownership of nullptr_exception, assume ownership of returned struct */
    sv = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->export_custom_value(dvf, (etch_object*) nullptr_exception);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    /* we could hard code the struct key for the test, but to be thorough
     * let's get it programmatically. we get the import-export helper for 
     * the type of object we are exporting (exception). from the helper
     * will get the key for the expected string export value, which should
     * be an etch_field of name "msg" */
    impexhelper = etchtype_get_impexphelper(builtins._mt__exception);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    /* get reference to exported object from struct, expected to be an 
     * etch_string wrapping an ascii string which is the exception text.
     * the exception text will contain the instance text specified above,
     * specifically it should be "null pointer exception dlr0w 0ll3h" */
    exported_stringobj = (etch_string*) structvalue_get(sv, impexhelper->field); 
    result = is_etch_string(exported_stringobj);
    CU_ASSERT_EQUAL_FATAL(result,TRUE); 

    exception_string_out = exported_stringobj->v.valw;
    CU_ASSERT_PTR_NOT_NULL_FATAL(exception_string_out);

    etch_object_destroy(sv);

    // destroy vf
    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
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
 * test_import_custom_value_exception()
 * import an etch exception type
 */
static void test_import_custom_value_exception(void)
{
    etch_structvalue* sv = NULL;
    etch_exception* inexcp = NULL;
    etch_serializer* impexhelper = NULL;
    char *outtext     =  "null pointer exception fubar";
    wchar_t *expected = L"null pointer exception fubar", *intext = NULL;
    int result = 0;
    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    
    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);
    
    impexhelper = etchtype_get_impexphelper(builtins._mt__exception);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    sv = new_structvalue(builtins._mt__exception, 1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    result = structvalue_put(sv, clone_field(impexhelper->field), 
        (etch_object*) new_string(outtext, ETCH_ENCODING_UTF8));
    CU_ASSERT_EQUAL_FATAL(result,0); 

    /* relinquish ownership of struct, assume ownership of inexcp */
    /* todo need to serialize exception type perhaps, in order to be able to
     * re-instantiate an exception as a null pointer exception for example */
    inexcp = (etch_exception*) ((struct i_value_factory*)((etch_object*)dvf)->vtab)->import_custom_value(dvf, sv);
    CU_ASSERT_EQUAL_FATAL(is_etch_exception(inexcp), TRUE); 

    intext = etch_exception_get_message(inexcp)->v.valw;
    CU_ASSERT_PTR_NOT_NULL_FATAL(intext); 
    result = wcscmp(intext, expected);
    CU_ASSERT_EQUAL(result,0);  

    etch_object_destroy(inexcp);
    
    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
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
 * test_export_custom_value_runtime_exception()
 * export an etch_runtime_exception
 */
static void test_export_custom_value_runtime_exception(void)
{
    int result = 0;
    etch_structvalue* sv = NULL;
    etch_serializer* impexhelper = NULL;
    etch_string* exported_stringobj = NULL;
    etch_exception* runtime_exception = NULL;
    wchar_t* exception_string_out = NULL;
    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    runtime_exception = new_etch_exception_from_errorcode(ETCH_ERROR); 

    /* retain ownership of runtime_exception, assume ownership of returned struct */
    sv = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->export_custom_value(dvf, (etch_object*) runtime_exception);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    /* see comments at this spot in test_export_custom_value_exception */
    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_runtime_exception);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    /* get reference to exported object from struct, expected to be an 
     * etch_string wrapping an ascii string which is the exception text.
     * the exception text will contain the instance text specified above,
     * specifically it should be "etch runtime exception tey rabuf ew era" */
    exported_stringobj = (etch_string*) structvalue_get(sv, impexhelper->field); 
    result = is_etch_string(exported_stringobj);
    CU_ASSERT_EQUAL_FATAL(result,TRUE); 

    exception_string_out = exported_stringobj->v.valw;
    CU_ASSERT_PTR_NOT_NULL_FATAL(exception_string_out);

    
    etch_object_destroy(sv);
    
    // destroy vf
    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
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
 * test_import_custom_value_runtime_exception()
 * import an etch rutime exception type
 */
static void test_import_custom_value_runtime_exception(void)
{
    etch_structvalue* sv = NULL;
    etch_exception* inexcp = NULL;
    etch_serializer* impexhelper = NULL;
    char *outtext     =  "etch runtime exception fubar";
    wchar_t *expected = L"etch runtime exception fubar", *intext = NULL;
    int result = 0;
    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    
        typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_runtime_exception);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    sv = new_structvalue(builtins._mt__etch_runtime_exception, 1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    result = structvalue_put(sv, clone_field(impexhelper->field), 
        (etch_object*) new_string(outtext, ETCH_ENCODING_UTF8));
    CU_ASSERT_EQUAL_FATAL(result,0); 

    /* relinquish ownership of struct, assume ownership of inexcp */
    inexcp = (etch_exception*) ((struct i_value_factory*)((etch_object*)dvf)->vtab)->import_custom_value(dvf, sv);
    CU_ASSERT_EQUAL_FATAL(is_etch_exception(inexcp), TRUE); 

    intext = etch_exception_get_message(inexcp)->v.valw;
    CU_ASSERT_PTR_NOT_NULL_FATAL(intext); 
    result = wcscmp(intext, expected);
    CU_ASSERT_EQUAL(result,0);  

    etch_object_destroy(inexcp);
    // destroy vf
    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
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
 * test_export_custom_value_list()
 * export an etch list type
 */
static void test_export_custom_value_list(void)
{
    int result = 0;
    etch_structvalue* sv = NULL;
    etch_arraylist*  outlist = NULL;
    etch_nativearray*  explist = NULL;
	etch_int32* content = NULL;
    etch_serializer* impexhelper = NULL;
    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    
    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    outlist = new_etch_arraylist(0,0);
    outlist->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
    outlist->is_readonly  = FALSE;
    outlist->content_obj_type = ETCHTYPEB_PRIMITIVE;
    outlist->content_class_id = CLASSID_PRIMITIVE_INT32;
    etch_arraylist_add(outlist, new_int32(1));
    etch_arraylist_add(outlist, new_int32(2));

    /* retain ownership of outlist, assume ownership of returned struct */
    sv = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->export_custom_value(dvf, (etch_object*) outlist);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    /* see comments at this spot in test_export_custom_value_exception() */
    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_list);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

	explist = (etch_nativearray*) structvalue_get(sv, impexhelper->field); 

    result  = is_etch_nativearray(explist);
    CU_ASSERT_EQUAL_FATAL(result,TRUE); 
    CU_ASSERT_EQUAL(explist->dimension[0],2);

	explist->get1(explist, &content, 0);
	CU_ASSERT_EQUAL_FATAL(is_etch_int32(content), TRUE);
	CU_ASSERT_EQUAL(content->value, 1);
	content = NULL;
    explist->get1(explist, &content, 1);
	CU_ASSERT_EQUAL_FATAL(is_etch_int32(content), TRUE);
	CU_ASSERT_EQUAL(content->value, 2);

    etch_object_destroy(outlist);
    etch_object_destroy(sv);

   // destroy vf
    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
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
 * test_import_custom_value_list()
 * import an etch list type
 */
static void test_import_custom_value_list(void)
{
    etch_iterator iterator;
    etch_structvalue* sv = NULL;
    etch_arraylist* inlist = NULL;
    etch_arrayvalue* outlist = NULL;
    etch_serializer* impexhelper = NULL;
    int result = 0;
    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    
        typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_list);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    sv = new_structvalue(builtins._mt__etch_list, 1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

	outlist  = new_arrayvalue_default();
	arrayvalue_add(outlist, (ETCH_ARRAY_ELEMENT*)new_int32(1));
	arrayvalue_add(outlist, (ETCH_ARRAY_ELEMENT*)new_int32(2));

    result = structvalue_put(sv, clone_field(impexhelper->field), (etch_object*) outlist);
    CU_ASSERT_EQUAL_FATAL(result,0); 

    /* relinquish ownership of struct and outlist, assume ownership of inlist */
    inlist = (etch_arraylist*) ((struct i_value_factory*)((etch_object*)dvf)->vtab)->import_custom_value(dvf, sv);
    CU_ASSERT_EQUAL_FATAL(is_etch_arraylist(inlist), TRUE); 

    set_iterator(&iterator, inlist, &inlist->iterable);

    /* verify that imported list content is same as original */
    while(iterator.has_next(&iterator))
    {  
       etch_int32* intobj = iterator.current_value;
       CU_ASSERT_EQUAL_FATAL(is_etch_int32(intobj), TRUE);
       switch(intobj->value) { case 1: case 2: break; default: result = -1; }       
       CU_ASSERT_NOT_EQUAL(result, -1);  
       iterator.next(&iterator); 
    }

    etch_object_destroy(inlist);
   // destroy vf
    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
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
 * test_export_custom_value_map()
 * export an etch map type
 */
static void test_export_custom_value_map(void)
{
    etch_structvalue* sv = NULL;
    etch_hashtable*  outmap = NULL;
    etch_nativearray* explist = NULL;
	etch_int32* icontent = NULL;
	etch_string* scontent = NULL;
    etch_serializer* impexhelper = NULL;
    int  result = 0, outmapcount = 0;
    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    
    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    outmap = new_etch_map(0);  /* etch_map is an object-to-object hashtable */
    etchmap_map_add (outmap, (etch_object*) new_string("1", ETCH_ENCODING_ASCII), (etch_object*) new_int32(1));
    etchmap_map_add (outmap, (etch_object*) new_string("2", ETCH_ENCODING_ASCII), (etch_object*) new_int32(2)); 
    outmapcount = etchmap_count(outmap);
    CU_ASSERT_EQUAL_FATAL(outmapcount, 2);

    /* retain ownership of outmap, assume ownership of returned struct */
    sv = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->export_custom_value(dvf, (etch_object*) outmap);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    /* see comments at this spot in test_export_custom_value_exception() */
    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_map);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

	explist = (etch_nativearray*) structvalue_get(sv, impexhelper->field); 

    result  = is_etch_nativearray(explist);
    CU_ASSERT_EQUAL_FATAL(result,TRUE); 
    CU_ASSERT_EQUAL(explist->dimension[0],4);

	explist->get1(explist, &scontent, 0);
	CU_ASSERT_EQUAL_FATAL(is_etch_string(scontent), TRUE);
	icontent = NULL;
	scontent = NULL;
    explist->get1(explist, &icontent, 1);
	CU_ASSERT_EQUAL_FATAL(is_etch_int32(icontent), TRUE);
	CU_ASSERT_EQUAL(icontent->value, 1);
	icontent = NULL;
	scontent = NULL;


	explist->get1(explist, &scontent, 2);
	CU_ASSERT_EQUAL_FATAL(is_etch_string(scontent), TRUE);
	icontent = NULL;
	scontent = NULL;
    explist->get1(explist, &icontent, 3);
	CU_ASSERT_EQUAL_FATAL(is_etch_int32(icontent), TRUE);
	CU_ASSERT_EQUAL(icontent->value, 2);
	icontent = NULL;
	scontent = NULL;

    etch_object_destroy(outmap);
    etch_object_destroy(sv);

   // destroy vf
    etch_object_destroy(dvf);
    etch_object_destroy(c2tmap);
    etch_object_destroy(typemap);
    
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
 * test_import_custom_value_map()
 * import an etch map type
 */
static void test_import_custom_value_map(void)
{
    etch_iterator iterator;
    etch_structvalue* sv = NULL;
    etch_hashtable* inmap = NULL;
    etch_arrayvalue* outlist = NULL;
    etch_serializer* impexhelper = NULL;
    int  result = 0;
    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    
    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_map);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    sv = new_structvalue(builtins._mt__etch_map, 1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

	outlist  = new_arrayvalue_default();
	arrayvalue_add(outlist, (ETCH_ARRAY_ELEMENT*)new_string("1", ETCH_ENCODING_ASCII));
	arrayvalue_add(outlist, (ETCH_ARRAY_ELEMENT*)new_int32(1));
	arrayvalue_add(outlist, (ETCH_ARRAY_ELEMENT*)new_string("2", ETCH_ENCODING_ASCII));
	arrayvalue_add(outlist, (ETCH_ARRAY_ELEMENT*)new_int32(2));

    result = structvalue_put(sv, clone_field(impexhelper->field), (etch_object*) outlist);
    CU_ASSERT_EQUAL_FATAL(result,0); 

    /* relinquish ownership of struct and outlist, assume ownership of inmap */
    inmap = (etch_hashtable*) ((struct i_value_factory*)((etch_object*)dvf)->vtab)->import_custom_value(dvf, sv);
    CU_ASSERT_EQUAL_FATAL(is_etch_hashtable(inmap), TRUE); 

    set_iterator(&iterator, inmap, &inmap->iterable);

    /* verify that imported map content is same as original */
    while(iterator.has_next(&iterator))
    {  
       etch_string* keyobj = iterator.current_key;
       etch_int32*  valobj = iterator.current_value;
       CU_ASSERT_EQUAL_FATAL(is_etch_string(keyobj), TRUE);
       CU_ASSERT_EQUAL_FATAL(is_etch_int32(valobj), TRUE);
       switch(valobj->value) { case 1: case 2: break; default: result = -1; }       
       CU_ASSERT_NOT_EQUAL(result, -1); 
 
       iterator.next(&iterator); 
    }
    
    etch_object_destroy(inmap);  

    // destroy vf
    etch_object_destroy(dvf);
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
 * test_export_custom_value_set()
 * export an etch set type
 */
static void test_export_custom_value_set(void)
{
    etch_structvalue* sv = NULL;
	etch_string* content = NULL;
    etch_hashtable*  outset = NULL;
    etch_nativearray*  explist = NULL;
    etch_serializer* impexhelper = NULL;
    int  result = 0, outsetcount = 0;
    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    outset = new_etch_set(0);  /* etch_set is an object-to-null hashtable */
    etchmap_set_add (outset, (etch_object*) new_string("1", ETCH_ENCODING_ASCII));
    etchmap_set_add (outset, (etch_object*) new_string("2", ETCH_ENCODING_ASCII)); 
    outsetcount = etchmap_count(outset);
    CU_ASSERT_EQUAL_FATAL(outsetcount, 2); 

    /* retain ownership of outset, assume ownership of returned struct */
    sv = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->export_custom_value(dvf, (etch_object*) outset);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    /* see comments at this spot in test_export_custom_value_exception() */
    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_set);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

	explist = (etch_nativearray*) structvalue_get(sv, impexhelper->field); 

    result  = is_etch_nativearray(explist);
    CU_ASSERT_EQUAL_FATAL(result,TRUE); 
    CU_ASSERT_EQUAL(explist->dimension[0],2);

	explist->get1(explist, &content, 0);
	CU_ASSERT_EQUAL_FATAL(is_etch_string(content), TRUE);
	content = NULL;
	explist->get1(explist, &content, 1);
	CU_ASSERT_EQUAL_FATAL(is_etch_string(content), TRUE);
	content = NULL;

    etch_object_destroy(outset);
    etch_object_destroy(sv);

    // destroy vf
    etch_object_destroy(dvf);
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
 * test_import_custom_value_set()
 * import an etch set type
 */
static void test_import_custom_value_set(void)
{
    etch_iterator iterator;
    etch_structvalue* sv = NULL;
    etch_hashtable* inset = NULL;
    etch_arrayvalue* outlist = NULL;
    etch_serializer* impexhelper = NULL;
    int  result = 0;
    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    
    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_set);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    sv = new_structvalue(builtins._mt__etch_set, 1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

	outlist  = new_arrayvalue_default();
	arrayvalue_add(outlist, (ETCH_ARRAY_ELEMENT*)new_string("1", ETCH_ENCODING_ASCII));
	arrayvalue_add(outlist, (ETCH_ARRAY_ELEMENT*)new_string("2", ETCH_ENCODING_ASCII));

    result = structvalue_put(sv, clone_field(impexhelper->field), (etch_object*) outlist);
    CU_ASSERT_EQUAL_FATAL(result,0); 

    /* relinquish ownership of struct and outlist, assume ownership of inset */
    inset = (etch_hashtable*) ((struct i_value_factory*)((etch_object*)dvf)->vtab)->import_custom_value(dvf, sv);
    CU_ASSERT_EQUAL_FATAL(is_etch_set(inset), TRUE); 

    set_iterator(&iterator, inset, &inset->iterable);

    /* verify that imported map content is same as original */
    while(iterator.has_next(&iterator))
    {  
       etch_string* setobj = iterator.current_key;
       CU_ASSERT_EQUAL(is_etch_string(setobj), TRUE);
       CU_ASSERT_PTR_NULL(iterator.current_value);
       iterator.next(&iterator); 
    }

    etch_object_destroy(inset);
    // destroy vf
    etch_object_destroy(dvf);
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
 * test_export_custom_value_date()
 * export an etch date type
 */
static void test_export_custom_value_date(void)
{
    etch_date*  outdate  = NULL;
    etch_int64* expdate  = NULL;
    etch_structvalue* sv = NULL;
    etch_serializer* impexhelper = NULL;
    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;

    typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    outdate = new_date();

    /* retain ownership of outdate, assume ownership of returned struct */
    sv = ((struct i_value_factory*)((etch_object*)dvf)->vtab)->export_custom_value(dvf, (etch_object*) outdate);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    /* see comments at this spot in test_export_custom_value_exception() */
    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_datetime);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    /* get reference to exported etch_int64 object from struct */
    expdate = (etch_int64*) structvalue_get(sv, impexhelper->field); 
    CU_ASSERT_EQUAL_FATAL(is_etch_int64(expdate), TRUE); 

    etch_object_destroy(outdate);
    etch_object_destroy(sv);
    // destroy vf
    etch_object_destroy(dvf);
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
 * test_import_custom_value_date()
 * import an etch date type
 */
static void test_import_custom_value_date(void)
{
    etch_date*  indate = NULL;
    etch_structvalue* sv = NULL;
    etch_serializer* impexhelper = NULL;
    time_t outtime, intime;
    int result = 0;
    default_value_factory*  dvf             = NULL;
    vf_idname_map*          typemap         = NULL;
    class_to_type_map*      c2tmap          = NULL;
    
     typemap = new_vf_types_collection(ETCH_DEFVF_IDNMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(typemap);
    c2tmap  = new_class_to_type_map(ETCH_DEVVF_C2TMAP_DEFINITSIZE);
    CU_ASSERT_PTR_NOT_NULL(c2tmap);  
    defvf_initialize_static(typemap, c2tmap);
    dvf = new_default_value_factory(typemap, c2tmap);
    CU_ASSERT_PTR_NOT_NULL(dvf);

    time(&outtime);

    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_datetime);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    sv = new_structvalue(builtins._mt__etch_datetime, 1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    result = structvalue_put(sv, clone_field(impexhelper->field), (etch_object*) new_int64(outtime));
    CU_ASSERT_EQUAL_FATAL(result,0); 

    /* relinquish ownership of struct, assume ownership of indate */
    indate = (etch_date*) ((struct i_value_factory*)((etch_object*)dvf)->vtab)->import_custom_value(dvf, sv);
    CU_ASSERT_EQUAL_FATAL(is_etch_date(indate), TRUE); 

    intime = (time_t) indate->value;
    CU_ASSERT_EQUAL(intime, outtime); 

    etch_object_destroy(indate);
    // destroy vf
    etch_object_destroy(dvf);
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
CU_pSuite test_etch_defvaluefact_suite()
{    
    CU_pSuite pSuite = CU_add_suite("suite_defvf", init_suite, clean_suite);

    CU_add_test(pSuite, "test constructor", test_constructor); 
    CU_add_test(pSuite, "test builtins",  test_builtin_types);  
    CU_add_test(pSuite, "test add type a",  test_add_type_a); 
    CU_add_test(pSuite, "test add type b",  test_add_type_b); 
    CU_add_test(pSuite, "test add type c",  test_add_type_c);  
    CU_add_test(pSuite, "test get type by id", test_get_type_by_id);  
    CU_add_test(pSuite, "test get type by name", test_get_type_by_name); 
    CU_add_test(pSuite, "test get all types", test_get_types); 
    CU_add_test(pSuite, "test auth excp fields",  test_authxcp_fields); 
    CU_add_test(pSuite, "test runtime excp fields", test_rtxcp_fields);
    CU_add_test(pSuite, "test get_set_message_id",  test_get_set_message_id); 
    CU_add_test(pSuite, "test get_set_in_reply_to", test_get_set_in_reply_to);  
    CU_add_test(pSuite, "test exception fields",  test_exception_fields);
    CU_add_test(pSuite, "test get encoding", test_get_string_encoding); 

    CU_add_test(pSuite, "test add mixin", test_add_mixin); 
    CU_add_test(pSuite, "test mixin recursion", test_mixin_recursion); 
    CU_add_test(pSuite, "test custom struct type", test_custom_struct_type); 
                                                                   
    CU_add_test(pSuite, "test export custom (exception)", test_export_custom_value_exception); 
    CU_add_test(pSuite, "test export custom (runtime)", test_export_custom_value_runtime_exception); 
    CU_add_test(pSuite, "test export custom (list)", test_export_custom_value_list);
    CU_add_test(pSuite, "test export custom (map)",  test_export_custom_value_map);
    CU_add_test(pSuite, "test export custom (set)",  test_export_custom_value_set);
    CU_add_test(pSuite, "test export custom (date)", test_export_custom_value_date);

    CU_add_test(pSuite, "test import custom (exception)", test_import_custom_value_exception); 
    CU_add_test(pSuite, "test import custom (runtime)", test_import_custom_value_runtime_exception); 
    CU_add_test(pSuite, "test import custom (list)", test_import_custom_value_list); 
    CU_add_test(pSuite, "test import custom (map)",  test_import_custom_value_map); 
    CU_add_test(pSuite, "test import custom (set)",  test_import_custom_value_set); 
    CU_add_test(pSuite, "test import custom (date)", test_import_custom_value_date); 

    return pSuite;
}
