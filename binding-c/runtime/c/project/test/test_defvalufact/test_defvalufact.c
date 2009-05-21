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

#include "apr_time.h" /* some apr must be included first */
#include "etchthread.h"
#include "etch_validator.h"  /* must be included second */

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etch_tagdata.h"
#include "etch_global.h"
#include "etchmap.h"
#include "etchlog.h"
#include "etch_id_name.h"
#include "etch_defvalufact.h"
#include "etch_arrayval.h"
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

/**
 * do_test_type_field
 */
int do_test_type_field(default_value_factory* vf, etch_type* type, etch_field* key, objmask* value)
{
    etch_message* msg = new_message(type, 1, (etch_value_factory*)vf);

    const int result = message_put(msg, key, value);

    msg->destroy(msg);

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
void test_constructor(void)
{
    default_value_factory *vf = new_default_value_factory(NULL, NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(vf);

    vf->destroy(vf);

    etchvf_free_builtins();    

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_builtin_types()
 * test that the vf contains all expected default types
 */
void test_builtin_types(void)
{
    default_value_factory *vf = new_default_value_factory(NULL, NULL);
    int errs = 0;

    if (NULL == vf->vtab->get_type_by_name(vf, builtins._mt__etch_auth_exception->name))
        errs++; 
    if (NULL == vf->vtab->get_type_by_name(vf, builtins._mt__etch_datetime->name))
        errs++; 
    if (NULL == vf->vtab->get_type_by_name(vf, builtins._mt__etch_list->name))
        errs++; 
    if (NULL == vf->vtab->get_type_by_name(vf, builtins._mt__etch_map->name))
        errs++; 
    if (NULL == vf->vtab->get_type_by_name(vf, builtins._mt__etch_runtime_exception->name))
        errs++; 
    if (NULL == vf->vtab->get_type_by_name(vf, builtins._mt__etch_set->name))
        errs++; 
    if (NULL == vf->vtab->get_type_by_name(vf, builtins._mt__exception->name))
        errs++; 

    CU_ASSERT_EQUAL(errs, 0);  

    vf->destroy(vf); 
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_add_type_a()
 * test that a type can be added and its memory automatically freed.
 */
void test_add_type_a(void)
{
    default_value_factory *dvf = new_default_value_factory(NULL, NULL);

    etch_type* newtype = new_static_type(L"testtype");
    etch_type* efftype = dvf->vtab->add_type(dvf, newtype);  /* not owned by vf */
    CU_ASSERT_PTR_EQUAL(newtype, efftype);

    dvf->destroy(dvf); 
    destroy_static_type(efftype); /* since not owned by vf */
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_add_type_b()
 * test that a duplicate type is rejected as expected.
 */
void test_add_type_b(void)
{
    default_value_factory *dvf = new_default_value_factory(NULL, NULL);

    wchar_t* samestring = L"sametype";
    etch_type* newtype1 = new_static_type(samestring);
    etch_type* newtype2 = new_static_type(samestring);

    etch_type* efftype = dvf->vtab->add_type(dvf, newtype1);  /* not owned by vf */
    CU_ASSERT_PTR_EQUAL(efftype, newtype1);

    /* when try to add a duplicate, vf returns original and destroys duplicate */
    efftype = dvf->vtab->add_type(dvf, newtype2);  
    CU_ASSERT_PTR_EQUAL(efftype, newtype1);

    dvf->destroy(dvf); 
    destroy_static_type(newtype1); /* since not owned by vf */
    /* don't destroy newtype2 since vf destroyed it */
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_add_type_c()
 * test that a duplicate of a builtin type is handled as expected.
 */
void test_add_type_c(void)
{
    default_value_factory *dvf = new_default_value_factory(NULL, NULL);

    etch_type* duptype = new_type(str_etch_runtime_exception);

    /* when try to add a duplicate, vf returns original and destroys duplicate */
    etch_type* efftype = dvf->vtab->add_type(dvf, duptype);   
    CU_ASSERT_PTR_EQUAL(efftype, builtins._mt__etch_runtime_exception);

    dvf->destroy(dvf); 
    /* don't destroy duptype since vf destroyed it */
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_get_type_by_id()
 * test that a type can be retrived by its id, and memory accounted for.
 */
void test_get_type_by_id(void)
{
   /* note that a *reference* is returned (not disposable), not a copy. */
    unsigned saved_id = 0;
    etch_type* newtype, *foundtype;
    default_value_factory *dvf = new_default_value_factory(NULL, NULL);

    newtype  = new_type(L"testtype");
    saved_id = newtype->id;
    dvf->vtab->add_type(dvf, newtype);  

    foundtype = dvf->vtab->get_type_by_id(dvf, saved_id);

    CU_ASSERT_PTR_NOT_NULL_FATAL(foundtype);  
    CU_ASSERT_EQUAL(saved_id, foundtype->id);  

    dvf->destroy(dvf); 
    /* newtype->destroy(newtype); */  /* vf now owns its nonstatic user types */
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();     
}


/**
 * test_get_type_by_name()
 * test that a type can be retrived by its name, and memory accounted for.
 */
void test_get_type_by_name(void)
{
   /* note that a *reference* is returned (not disposable), not a copy. */
    etch_type* newtype, *foundtype;
    default_value_factory *dvf = new_default_value_factory(NULL, NULL);

    newtype = new_type(L"testtype");
    dvf->vtab->add_type(dvf, newtype);

    foundtype = dvf->vtab->get_type_by_name(dvf, L"testtype");
    CU_ASSERT_PTR_NOT_NULL_FATAL(foundtype);  

    dvf->destroy(dvf); 
    /* newtype->destroy(newtype); */ /* types map now owns nonstatic user types */
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();     
}


/**
 * test_get_types()
 * test that all types can be retrived as a list, and memory accounted for.
 */
void test_get_types(void)
{
    int  result;
    etch_hashtable* map;
    etch_iterator iterator;
    etch_arraylist* typeslist;
    etch_hashitem hashbucket, *thisitem = &hashbucket; 

    default_value_factory *dvf = new_default_value_factory(NULL, NULL);
    map = dvf->types;

    typeslist = dvf->vtab->get_types(dvf);

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
        result = map->vtab->removeh
            (map->realtable, thistype->hashkey, map, &thisitem);   
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
    typeslist->destroy(typeslist);

    dvf->destroy(dvf); 
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();     
}


/**
 * test_authxcp_fields()
 * test that auth exception type is configured as expected
 */
void test_authxcp_fields(void)
{
    default_value_factory *vf = new_default_value_factory(NULL, NULL);

    /* apparently the purpose of this test, ported from java test, is to test that
     * the authxcp type has a _mf_msg field, and that if we then make a message
     * of type authxcp and put to it a string value keyed by _mf_msg, that it
     * validates OK. i.e., the _mf_msg field of a authxcp should be a string.
     */
    etch_type*  msgtype  = vf->vtab->get_type_by_name(vf, str_etch_auth_exception); 
    etch_field* msgkey1  = clone_field(builtins._mf_msg); 
    etch_field* msgkey2  = clone_field(builtins._mf_msg); 
    objmask*    val_good = (objmask*) new_string(L"dlrow olleh", ETCH_ENCODING_UTF16);
    objmask*    val_unxp = (objmask*) new_int32(0);

    int result = do_test_type_field(vf, msgtype, msgkey1, val_good);
    CU_ASSERT_EQUAL(result, 0); 

    result = do_test_type_field(vf, msgtype, msgkey2, val_unxp);
    CU_ASSERT_EQUAL(result, -1); /* type validation should fail */  

    vf->destroy(vf); 
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
} 


/**
 * test_rtxcp_fields()
 * test that runtime exception type is configured as expected
 */
void test_rtxcp_fields(void)
{
    default_value_factory *vf = new_default_value_factory(NULL, NULL);

    etch_type*  msgtype  = vf->vtab->get_type_by_name(vf, str_etch_runtime_exception); 
    etch_field* msgkey1  = clone_field(builtins._mf_msg); 
    etch_field* msgkey2  = clone_field(builtins._mf_msg); 
    objmask*    val_good = (objmask*) new_string(L"dlrow olleh", ETCH_ENCODING_UTF16);
    objmask*    val_unxp = (objmask*) new_int32(0);

    int result = do_test_type_field(vf, msgtype, msgkey1, val_good);
    CU_ASSERT_EQUAL(result, 0); 

    result = do_test_type_field(vf, msgtype, msgkey2, val_unxp);
    CU_ASSERT_EQUAL(result, -1); /* type validation should fail */  

    vf->destroy(vf); 
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
} 


/**
 * test_exception_fields()
 * test that exception type is configured as expected
 */
void test_exception_fields(void)
{
    default_value_factory *vf = new_default_value_factory(NULL, NULL);

    etch_type*  msgtype     = vf->vtab->get_type_by_name(vf, str_exception); 
    etch_field* key_result  = clone_field(builtins._mf_result); 
    etch_field* key_msg_id  = clone_field(builtins._mf__message_id); 
    etch_field* key_inrepto = clone_field(builtins._mf__in_reply_to); 
    objmask*    val_rtexp   = (objmask*) new_etch_exception(EXCPTYPE_ETCHRUNTIME, L"whatever", ETCHEXCP_COPYTEXT);
    objmask*    val_long1   = (objmask*) new_int64(123);
    objmask*    val_long2   = (objmask*) new_int64(456);    

    int result = do_test_type_field(vf, msgtype, key_result, val_rtexp);
    CU_ASSERT_EQUAL(result, 0); 

    result = do_test_type_field(vf, msgtype, key_msg_id, val_long1);
    CU_ASSERT_EQUAL(result, 0); 

    result = do_test_type_field(vf, msgtype, key_inrepto, val_long2);
    CU_ASSERT_EQUAL(result, 0); 

    vf->destroy(vf); 
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
} 


/**
 * test_get_string_encoding()
 */
void test_get_string_encoding(void)
{
    wchar_t*     encoding; 
    etch_string* newstring;
    int  result, etch_encoding;  
    char* teststring = "8 bit characters"; 
    default_value_factory *dvf = new_default_value_factory(NULL, NULL);

    /* note that a reference to a raw string is returned */
    encoding = dvf->vtab->get_string_encoding(dvf);

    result = wcscmp(encoding, L"utf-8");
    CU_ASSERT_EQUAL(result, 0);  

    etch_encoding = get_etch_string_encoding((etch_value_factory*)dvf);

    newstring = new_string("8 bit characters", etch_encoding);
    CU_ASSERT_PTR_NOT_NULL_FATAL(newstring);
    result = strcmp(newstring->v.valc, teststring);
    CU_ASSERT_EQUAL(result, 0);  
    
    newstring->destroy(newstring);
    dvf->destroy(dvf); 
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();     
} 


/**
 * test_add_mixin()
 */
void test_add_mixin(void)
{
    int result = 0;
    default_value_factory *mixvf1, *mixvf2;
    default_value_factory *vf = new_default_value_factory(NULL, NULL); 

    mixvf1 = new_default_value_factory(NULL, NULL); 
    CU_ASSERT_PTR_NOT_NULL_FATAL(mixvf1);
    mixvf2 = new_default_value_factory(NULL, NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(mixvf2);

    result = defvf_add_mixin(vf, (etch_value_factory*) mixvf1);
    CU_ASSERT_EQUAL(result, 0);  
    result = defvf_add_mixin(vf, (etch_value_factory*) mixvf2);
    CU_ASSERT_EQUAL(result, 0);  
 
    result = vf->mixins->count;
    CU_ASSERT_EQUAL(result, 2);  

    mixvf2->destroy(mixvf2); 
    mixvf1->destroy(mixvf1); 
    vf->destroy(vf); 
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear(); 
}


/**
 * test_mixin_recursion
 * exercise the various vf apis which are mixin-recursive
 */
void test_mixin_recursion(void)
{
    int result = 0;
    etch_type* rettype = NULL;
    wchar_t* str_foo = L"foo", *str_bar = L"bar", *str_bogus = L"wtf";
    default_value_factory *mixvf1 = NULL, *mixvf2 = NULL;
    default_value_factory *vf = new_default_value_factory(NULL, NULL); 
    etch_type* newtype1 = new_static_type(str_foo);
    etch_type* newtype2 = new_static_type(str_bar);

    mixvf1 = new_default_value_factory(NULL, NULL); 
    CU_ASSERT_PTR_NOT_NULL_FATAL(mixvf1);
    rettype =  mixvf1->vtab->add_type(mixvf1, newtype1);
    CU_ASSERT_PTR_EQUAL_FATAL(rettype, newtype1);

    mixvf2 = new_default_value_factory(NULL, NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(mixvf2);
    rettype = mixvf2->vtab->add_type(mixvf2, newtype2);
    CU_ASSERT_PTR_EQUAL_FATAL(rettype, newtype2);

    result = defvf_add_mixin(vf, (etch_value_factory*) mixvf1);
    CU_ASSERT_EQUAL(result, 0);  
    result = defvf_add_mixin(vf, (etch_value_factory*) mixvf2);
    CU_ASSERT_EQUAL(result, 0);  

    result = defvf_add_mixin(mixvf1, (etch_value_factory*) mixvf2);
    CU_ASSERT_EQUAL(result, 0);  

    rettype = vf->vtab->get_type_by_name(vf, str_exception);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* builtin type */

    rettype = vf->vtab->get_type_by_name(mixvf1, str_exception);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* builtin type */

    rettype = vf->vtab->get_type_by_name(mixvf2, str_exception);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* builtin type */

    /* mixin recursive by-name lookup */
    rettype = mixvf1->vtab->get_type_by_name(mixvf1, str_bar);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* via mixvf1 mixin 1 */

    rettype = vf->vtab->get_type_by_name(vf, str_foo);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* via vf mixin 1 */ 

    rettype = vf->vtab->get_type_by_name(vf, str_bar);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* via vf mixin 2 */ 

    /* mixin recursive by-id lookup */
    rettype = mixvf1->vtab->get_type_by_id(mixvf1, newtype2->id);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* via mixvf1 mixin 1 */

    rettype = vf->vtab->get_type_by_id(vf, newtype1->id);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* via vf mixin 1 */ 

    rettype = vf->vtab->get_type_by_id(vf, newtype2->id);
    CU_ASSERT_PTR_NOT_NULL(rettype);  /* via vf mixin 2 */ 

    /* negative tests will recurse all mixins */
    rettype =  vf->vtab->get_type_by_name(mixvf1, str_bogus);
    CU_ASSERT_PTR_NULL(rettype);   

    rettype = vf->vtab->get_type_by_id(vf, compute_id_name_id_from_widename(str_bogus));
    CU_ASSERT_PTR_NULL(rettype);  /* via vf mixin 1 */  
      
    mixvf2->destroy(mixvf2); 
    mixvf1->destroy(mixvf1); 
    vf->destroy(vf); 

    destroy_static_type(newtype1);
    destroy_static_type(newtype2);
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear(); 
}


/**
 * test_custom_struct_type
 *  
 */
void test_custom_struct_type(void)
{
    int result = 0;
    etch_type* rettype = NULL;
    wchar_t *str_vf0 = L"vf0", *str_vf1 = L"vf1", *str_vf2 = L"vf2";
    default_value_factory *vf0, *vf1 = NULL, *vf2 = NULL;

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

    vf0 = new_default_value_factory(NULL, NULL); 
    result = class_to_type_map_put(vf0->class_to_type, csclass0, cstype0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    vf1 = new_default_value_factory(NULL, NULL); 
    CU_ASSERT_PTR_NOT_NULL_FATAL(vf1);
    result = class_to_type_map_put(vf1->class_to_type, csclass1, cstype1);
    CU_ASSERT_EQUAL_FATAL(result,0);

    vf2 = new_default_value_factory(NULL, NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(vf2);
    result = class_to_type_map_put(vf2->class_to_type, csclass2, cstype2);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = defvf_add_mixin(vf0, (etch_value_factory*) vf1);
    CU_ASSERT_EQUAL(result, 0);  
    result = defvf_add_mixin(vf0, (etch_value_factory*) vf2);
    CU_ASSERT_EQUAL(result, 0);  

    result = defvf_add_mixin(vf1, (etch_value_factory*) vf2);
    CU_ASSERT_EQUAL(result, 0);  

    rettype = vf0->vtab->get_custom_struct_type(vf0, csclass0);
    CU_ASSERT_PTR_EQUAL(rettype, cstype0);  

    /* recurse mixed-in vfs for custom types */
    rettype = vf0->vtab->get_custom_struct_type(vf0, csclass1);
    CU_ASSERT_PTR_EQUAL(rettype, cstype1); 
  
    rettype = vf0->vtab->get_custom_struct_type(vf0, csclass2);
     CU_ASSERT_PTR_EQUAL(rettype, cstype2); 

    /* negative test will recurse all mixins */
    rettype = vf0->vtab->get_custom_struct_type(vf0, csbogus);
    CU_ASSERT_PTR_NULL(rettype);   
      
    vf2->destroy(vf2); 
    vf1->destroy(vf1); 
    vf0->destroy(vf0); 

    destroy_static_type(cstype0);
    destroy_static_type(cstype1);
    destroy_static_type(cstype2);
    etchvf_free_builtins(); 

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear(); 
}


/**
 * test_get_set_message_id()
 */
void test_get_set_message_id(void)
{
    int result = 0;
    int64 idvalue = 12345;
    etch_type* newtype = new_static_type(L"testtype");
    etch_int64* idobj  = new_int64(idvalue), *retobj = NULL;

    default_value_factory *dvf = new_default_value_factory(NULL, NULL);

    etch_message* msg = new_message(newtype, ETCH_DEFSIZE, (etch_value_factory*) dvf);

    etchtype_put_validator(newtype, builtins._mf__message_id, (objmask*) etchvtor_int64_get(0));
    
    result = dvf->vtab->set_message_id(dvf, msg, idobj);  /* we no longer own idobj */
    CU_ASSERT_EQUAL_FATAL(result,0);  /* ensure idobj validates ok */

    /* we get back a reference to the ID object, not a copy */
    retobj = dvf->vtab->get_message_id(dvf, msg); 

    CU_ASSERT_PTR_NOT_NULL_FATAL(retobj); 
    CU_ASSERT_EQUAL(retobj->value, idvalue); 

    msg->destroy(msg);
    dvf->destroy(dvf); 
    destroy_static_type(newtype);
    etchvf_free_builtins();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();     
}


/**
 * test_get_set_in_reply_to()
 */
void test_get_set_in_reply_to(void)
{
    int result = 0;
    int64 idvalue = 12345;
    etch_type* newtype = new_static_type(L"testtype");
    etch_int64* idobj  = new_int64(idvalue), *retobj = NULL;

    default_value_factory *dvf = new_default_value_factory(NULL, NULL);

    etch_message* msg = new_message(newtype, ETCH_DEFSIZE, (etch_value_factory*) dvf);

    etchtype_put_validator(newtype, builtins._mf__in_reply_to, (objmask*) etchvtor_int64_get(0));
    
    result = dvf->vtab->set_in_reply_to(dvf, msg, idobj);
    CU_ASSERT_EQUAL_FATAL(result,0);  /* ensure idobj validates ok */

    /* we get back a reference to the ID, not a copy */
    retobj = dvf->vtab->get_in_reply_to(dvf, msg); 

    CU_ASSERT_PTR_NOT_NULL_FATAL(retobj); 
    CU_ASSERT_EQUAL(retobj->value, idvalue); 

    msg->destroy(msg);
    dvf->destroy(dvf); 
    destroy_static_type(newtype);
    etchvf_free_builtins();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();     
}


/**
 * test_export_custom_value_exception()
 * export an etch_exception
 */
void test_export_custom_value_exception(void)
{
    int result = 0;
    etch_structvalue* sv = NULL;
    etch_serializer* impexhelper = NULL;
    etch_string* exported_stringobj = NULL;
    etch_exception* nullptr_exception = NULL;
    wchar_t* exception_string_u = L"dlr0w 0ll3h";
    char* exception_string_a    =  "dlr0w 0ll3h";
    char* exception_string_out = NULL, *substr = NULL;

    default_value_factory *vf = new_default_value_factory(NULL, NULL);

    nullptr_exception = new_etch_exception
        (EXCPTYPE_NULLPTR, exception_string_u, ETCHEXCP_COPYTEXT); 

    /* retain ownership of nullptr_exception, assume ownership of returned struct */
    sv = vf->vtab->export_custom_value(vf, (objmask*) nullptr_exception);
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

    exception_string_out = exported_stringobj->v.valc;
    CU_ASSERT_PTR_NOT_NULL_FATAL(exception_string_out);

    substr = strstr(exception_string_out, exception_string_a);
    CU_ASSERT_PTR_NOT_NULL(substr);  

    nullptr_exception->destroy(nullptr_exception);
    sv->destroy(sv);
    vf->destroy(vf); 
    etchvf_free_builtins();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   
}


/**
 * test_import_custom_value_exception()
 * import an etch exception type
 */
void test_import_custom_value_exception(void)
{
    etch_structvalue* sv = NULL;
    etch_exception* inexcp = NULL;
    default_value_factory *vf = NULL;
    etch_serializer* impexhelper = NULL;
    char *outtext     =  "null pointer exception fubar";
    wchar_t *expected = L"null pointer exception fubar", *intext = NULL;
    int result = 0;

    vf = new_default_value_factory(NULL, NULL);

    impexhelper = etchtype_get_impexphelper(builtins._mt__exception);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    sv = new_structvalue(builtins._mt__exception, 1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    result = structvalue_put(sv, clone_field(impexhelper->field), 
        (objmask*) new_string(outtext, ETCH_ENCODING_UTF8));
    CU_ASSERT_EQUAL_FATAL(result,0); 

    /* relinquish ownership of struct, assume ownership of inexcp */
    /* todo need to serialize exception type perhaps, in order to be able to
     * re-instantiate an exception as a null pointer exception for example */
    inexcp = (etch_exception*) vf->vtab->import_custom_value(vf, sv);
    CU_ASSERT_EQUAL_FATAL(is_etch_exception(inexcp), TRUE); 

    intext = inexcp->value->excptext;
    CU_ASSERT_PTR_NOT_NULL_FATAL(intext); 
    result = wcscmp(intext, expected);
    CU_ASSERT_EQUAL(result,0);  

    inexcp->destroy(inexcp);
    vf->destroy(vf); 
    etchvf_free_builtins();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   
}


/**
 * test_export_custom_value_runtime_exception()
 * export an etch_runtime_exception
 */
void test_export_custom_value_runtime_exception(void)
{
    int result = 0;
    etch_structvalue* sv = NULL;
    etch_serializer* impexhelper = NULL;
    etch_string* exported_stringobj = NULL;
    etch_exception* runtime_exception = NULL;
    wchar_t* exception_string_u = L"tey rabuf ew era";
    char* exception_string_a    =  "tey rabuf ew era";
    char* exception_string_out = NULL, *substr = NULL;

    default_value_factory *vf = new_default_value_factory(NULL, NULL);

    runtime_exception = new_etch_exception
        (EXCPTYPE_ETCHRUNTIME, exception_string_u, ETCHEXCP_COPYTEXT); 

    /* retain ownership of runtime_exception, assume ownership of returned struct */
    sv = vf->vtab->export_custom_value(vf, (objmask*) runtime_exception);
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

    exception_string_out = exported_stringobj->v.valc;
    CU_ASSERT_PTR_NOT_NULL_FATAL(exception_string_out);

    substr = strstr(exception_string_out, exception_string_a);
    CU_ASSERT_PTR_NOT_NULL(substr);  

    runtime_exception->destroy(runtime_exception);
    sv->destroy(sv);
    vf->destroy(vf); 
    etchvf_free_builtins();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   
}


/**
 * test_import_custom_value_runtime_exception()
 * import an etch rutime exception type
 */
void test_import_custom_value_runtime_exception(void)
{
    etch_structvalue* sv = NULL;
    etch_exception* inexcp = NULL;
    default_value_factory *vf = NULL;
    etch_serializer* impexhelper = NULL;
    char *outtext     =  "etch runtime exception fubar";
    wchar_t *expected = L"etch runtime exception fubar", *intext = NULL;
    int result = 0;

    vf = new_default_value_factory(NULL, NULL);

    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_runtime_exception);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    sv = new_structvalue(builtins._mt__etch_runtime_exception, 1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    result = structvalue_put(sv, clone_field(impexhelper->field), 
        (objmask*) new_string(outtext, ETCH_ENCODING_UTF8));
    CU_ASSERT_EQUAL_FATAL(result,0); 

    /* relinquish ownership of struct, assume ownership of inexcp */
    inexcp = (etch_exception*) vf->vtab->import_custom_value(vf, sv);
    CU_ASSERT_EQUAL_FATAL(is_etch_exception(inexcp), TRUE); 

    intext = inexcp->value->excptext;
    CU_ASSERT_PTR_NOT_NULL_FATAL(intext); 
    result = wcscmp(intext, expected);
    CU_ASSERT_EQUAL(result,0);  

    inexcp->destroy(inexcp);
    vf->destroy(vf); 
    etchvf_free_builtins();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   
}


/**
 * test_export_custom_value_list()
 * export an etch list type
 */
void test_export_custom_value_list(void)
{
    int result = 0;
    etch_iterator iterator;
    etch_structvalue* sv = NULL;
    etch_arraylist*  outlist = NULL;
    etch_arraylist*  explist = NULL;
    etch_serializer* impexhelper = NULL;

    default_value_factory *vf = new_default_value_factory(NULL, NULL);

    outlist = new_arraylist(0,0);
    outlist->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
    outlist->is_readonly  = FALSE;
    outlist->content_obj_type = ETCHTYPEB_PRIMITIVE;
    outlist->content_class_id = CLASSID_PRIMITIVE_INT32;
    arraylist_add(outlist, new_int32(1));
    arraylist_add(outlist, new_int32(2));         

    /* retain ownership of outlist, assume ownership of returned struct */
    sv = vf->vtab->export_custom_value(vf, (objmask*) outlist);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    /* see comments at this spot in test_export_custom_value_exception() */
    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_list);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    /* get reference to exported object from struct, an etch_arraylist whose
     * members are references to the same objects populated in outlist.
     * note that while it may seem convoluted to export to the same type of
     * objects as the object being exported from, the reason is a consistent
     * export interface, i.e. memory for the exported from object is always 
     * relinquished, memory for exported to objects is always acquired. and 
     * since we can't pass around raw arrays as does the java binding in this
     * case, the list export helper chooses to use an arraylist as the export  
     * object type for lists. 
     */
    explist = (etch_arraylist*) structvalue_get(sv, impexhelper->field); 
    result  = is_etch_arraylist(explist);
    CU_ASSERT_EQUAL_FATAL(result,TRUE); 
    CU_ASSERT_EQUAL(explist->count,2);

    set_iterator(&iterator, explist, &explist->iterable);

    /* verify that exported list content is same as original */
    while(iterator.has_next(&iterator))
    {  
       etch_int32* intobj = iterator.current_value;
       CU_ASSERT_EQUAL_FATAL(is_etch_int32(intobj), TRUE);
       switch(intobj->value) { case 1: case 2: break; default: result = -1; }       
       CU_ASSERT_NOT_EQUAL(result, -1);  
       iterator.next(&iterator); 
    }

    outlist->destroy(outlist);
    sv->destroy(sv);
    vf->destroy(vf); 
    etchvf_free_builtins();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   
}


/**
 * test_import_custom_value_list()
 * import an etch list type
 */
void test_import_custom_value_list(void)
{
    etch_iterator iterator;
    etch_structvalue* sv = NULL;
    etch_arraylist* inlist = NULL;
    etch_arraylist* outlist = NULL;
    default_value_factory *vf = NULL;
    etch_serializer* impexhelper = NULL;
    int result = 0;

    vf = new_default_value_factory(NULL, NULL);

    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_list);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    sv = new_structvalue(builtins._mt__etch_list, 1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    outlist = new_synchronized_arraylist(2,0);
    outlist->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
    outlist->content_obj_type = ETCHTYPEB_PRIMITIVE;
    outlist->content_class_id = CLASSID_PRIMITIVE_INT32;
    arraylist_add(outlist, new_int32(1));
    arraylist_add(outlist, new_int32(2));     

    result = structvalue_put(sv, clone_field(impexhelper->field), (objmask*) outlist);
    CU_ASSERT_EQUAL_FATAL(result,0); 

    /* relinquish ownership of struct and outlist, assume ownership of inlist */
    inlist = (etch_arraylist*) vf->vtab->import_custom_value(vf, sv);
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

    inlist->destroy(inlist);
    vf->destroy(vf); 
    etchvf_free_builtins();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   
}


/**
 * test_export_custom_value_map()
 * export an etch map type
 */
void test_export_custom_value_map(void)
{
    etch_iterator iterator;
    etch_structvalue* sv = NULL;
    etch_hashtable*  outmap = NULL;
    etch_arraylist*  explist = NULL;
    etch_serializer* impexhelper = NULL;
    int  result = 0, outmapcount = 0;

    default_value_factory *vf = new_default_value_factory(NULL, NULL);

    outmap = new_etch_map(0);  /* etch_map is an object-to-object hashtable */
    etchmap_map_add (outmap, (objmask*) new_string("1", ETCH_ENCODING_ASCII), (objmask*) new_int32(1));
    etchmap_map_add (outmap, (objmask*) new_string("2", ETCH_ENCODING_ASCII), (objmask*) new_int32(2)); 
    outmapcount = etchmap_count(outmap);
    CU_ASSERT_EQUAL_FATAL(outmapcount, 2); 

    /* retain ownership of outmap, assume ownership of returned struct */
    sv = vf->vtab->export_custom_value(vf, (objmask*) outmap);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    /* see comments at this spot in test_export_custom_value_exception() */
    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_map);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    /* get reference to exported object from struct, an etch_arraylist whose
     * members are references to the keys and values from outmap.
     */
    explist = (etch_arraylist*) structvalue_get(sv, impexhelper->field); 
    CU_ASSERT_EQUAL_FATAL(is_etch_arraylist(explist), TRUE); 
    CU_ASSERT_EQUAL(explist->count, outmapcount * 2);

    set_iterator(&iterator, explist, &explist->iterable);

    /* verify that exported list content matches that of original map */
    while(iterator.has_next(&iterator))
    {  
       if (iterator.ordinal & 1)  /* key is at every other list entry */
       {   etch_string* key = iterator.current_value;
           CU_ASSERT_EQUAL(is_etch_string(key), TRUE);
       }
       else   /* value follows its key */
       {   etch_int32* val = iterator.current_value;
           CU_ASSERT_EQUAL_FATAL(is_etch_int32(val), TRUE);
           switch(val->value) { case 1: case 2: break; default: result = -1; }       
           CU_ASSERT_NOT_EQUAL(result, -1);  
       }

       iterator.next(&iterator); 
    }

    outmap->destroy(outmap);
    sv->destroy(sv);
    vf->destroy(vf); 
    etchvf_free_builtins();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   
}


/**
 * test_import_custom_value_map()
 * import an etch map type
 */
void test_import_custom_value_map(void)
{
    etch_iterator iterator;
    etch_structvalue* sv = NULL;
    etch_hashtable* inmap = NULL;
    etch_arraylist* outlist = NULL;
    default_value_factory *vf = NULL;
    etch_serializer* impexhelper = NULL;
    int  result = 0, mapcount = 0;

    vf = new_default_value_factory(NULL, NULL);

    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_map);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    sv = new_structvalue(builtins._mt__etch_map, 1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    outlist = new_synchronized_arraylist(4,0);
    arraylist_add (outlist, (objmask*) new_string("1", ETCH_ENCODING_ASCII));
    arraylist_add (outlist, (objmask*) new_int32(1));
    arraylist_add (outlist, (objmask*) new_string("2", ETCH_ENCODING_ASCII));
    arraylist_add (outlist, (objmask*) new_int32(2));

    result = structvalue_put(sv, clone_field(impexhelper->field), (objmask*) outlist);
    CU_ASSERT_EQUAL_FATAL(result,0); 

    /* relinquish ownership of struct and outlist, assume ownership of inmap */
    inmap = (etch_hashtable*) vf->vtab->import_custom_value(vf, sv);
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

    inmap->destroy(inmap);  
    vf->destroy(vf); 
    etchvf_free_builtins();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   
}


/**
 * test_export_custom_value_set()
 * export an etch set type
 */
void test_export_custom_value_set(void)
{
    etch_iterator iterator;
    etch_structvalue* sv = NULL;
    etch_hashtable*  outset = NULL;
    etch_arraylist*  explist = NULL;
    etch_serializer* impexhelper = NULL;
    int  result = 0, outsetcount = 0;

    default_value_factory *vf = new_default_value_factory(NULL, NULL);

    outset = new_etch_set(0);  /* etch_set is an object-to-null hashtable */
    etchmap_set_add (outset, (objmask*) new_string("1", ETCH_ENCODING_ASCII));
    etchmap_set_add (outset, (objmask*) new_string("2", ETCH_ENCODING_ASCII)); 
    outsetcount = etchmap_count(outset);
    CU_ASSERT_EQUAL_FATAL(outsetcount, 2); 

    /* retain ownership of outset, assume ownership of returned struct */
    sv = vf->vtab->export_custom_value(vf, (objmask*) outset);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    /* see comments at this spot in test_export_custom_value_exception() */
    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_set);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    /* get reference to exported object from struct, an etch_arraylist  
     * whose values are references to the keys from the original set */
    explist = (etch_arraylist*) structvalue_get(sv, impexhelper->field); 
    CU_ASSERT_EQUAL_FATAL(is_etch_arraylist(explist), TRUE); 
    CU_ASSERT_EQUAL(explist->count, outsetcount);

    set_iterator(&iterator, explist, &explist->iterable);

    /* verify that exported list content matches that of original set */
    while(iterator.has_next(&iterator))
    {  
       etch_string* setmember = iterator.current_value;
       CU_ASSERT_EQUAL(is_etch_string(setmember), TRUE);
       iterator.next(&iterator); 
    }

    outset->destroy(outset);
    sv->destroy(sv);
    vf->destroy(vf); 
    etchvf_free_builtins();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   
}


/**
 * test_import_custom_value_set()
 * import an etch set type
 */
void test_import_custom_value_set(void)
{
    etch_iterator iterator;
    etch_structvalue* sv = NULL;
    etch_hashtable* inset = NULL;
    etch_arraylist* outlist = NULL;
    default_value_factory *vf = NULL;
    etch_serializer* impexhelper = NULL;
    int  result = 0, setcount = 0;

    vf = new_default_value_factory(NULL, NULL);

    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_set);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    sv = new_structvalue(builtins._mt__etch_set, 1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    outlist = new_synchronized_arraylist(2,0);
    arraylist_add (outlist, (objmask*) new_string("1", ETCH_ENCODING_ASCII));
    arraylist_add (outlist, (objmask*) new_string("2", ETCH_ENCODING_ASCII));

    result = structvalue_put(sv, clone_field(impexhelper->field), (objmask*) outlist);
    CU_ASSERT_EQUAL_FATAL(result,0); 

    /* relinquish ownership of struct and outlist, assume ownership of inset */
    inset = (etch_hashtable*) vf->vtab->import_custom_value(vf, sv);
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

    inset->destroy(inset);  
    vf->destroy(vf); 
    etchvf_free_builtins();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   
}


/**
 * test_export_custom_value_date()
 * export an etch date type
 */
void test_export_custom_value_date(void)
{
    etch_date*  outdate  = NULL;
    etch_int64* expdate  = NULL;
    etch_structvalue* sv = NULL;
    etch_serializer* impexhelper = NULL;

    default_value_factory *vf = new_default_value_factory(NULL, NULL);

    outdate = new_date();   

    /* retain ownership of outdate, assume ownership of returned struct */
    sv = vf->vtab->export_custom_value(vf, (objmask*) outdate);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    /* see comments at this spot in test_export_custom_value_exception() */
    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_datetime);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    /* get reference to exported etch_int64 object from struct */
    expdate = (etch_int64*) structvalue_get(sv, impexhelper->field); 
    CU_ASSERT_EQUAL_FATAL(is_etch_int64(expdate), TRUE); 

    outdate->destroy(outdate);
    sv->destroy(sv);
    vf->destroy(vf); 
    etchvf_free_builtins();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   
}


/**
 * test_import_custom_value_date()
 * import an etch date type
 */
void test_import_custom_value_date(void)
{
    etch_date*  indate = NULL;
    etch_int64* outval = NULL;
    etch_structvalue* sv = NULL;
    default_value_factory *vf = NULL;
    etch_serializer* impexhelper = NULL;
    time_t outtime, intime;
    int result = 0;
    time(&outtime);

    vf = new_default_value_factory(NULL, NULL);

    impexhelper = etchtype_get_impexphelper(builtins._mt__etch_datetime);
    CU_ASSERT_PTR_NOT_NULL_FATAL(impexhelper); 

    sv = new_structvalue(builtins._mt__etch_datetime, 1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(sv); 

    result = structvalue_put(sv, clone_field(impexhelper->field), (objmask*) new_int64(outtime));
    CU_ASSERT_EQUAL_FATAL(result,0); 

    /* relinquish ownership of struct, assume ownership of indate */
    indate = (etch_date*) vf->vtab->import_custom_value(vf, sv);
    CU_ASSERT_EQUAL_FATAL(is_etch_date(indate), TRUE); 

    intime = (time_t) indate->value;
    CU_ASSERT_EQUAL(intime, outtime); 

    indate->destroy(indate);
    vf->destroy(vf); 
    etchvf_free_builtins();

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();   
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
    pSuite = CU_add_suite("suite_defvf", init_suite, clean_suite);
    CU_set_output_filename("../test_defvf");
    etch_watch_id = 0; 

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