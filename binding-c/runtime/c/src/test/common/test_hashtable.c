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
 * test_hashtable.c 
 * tests the C implementation of etch hashtable.
 * this hashtable implementation uses the public domain jenkins hashtable.
 */
#include "etch_runtime.h"
#include "etch_hash.h"
#include "etch_general.h"
#include "etch_objecttypes.h"

#include <stdio.h>
#include "CUnit.h"

#define TESTDATAPATH "./jenktest.txt"
#define MAXLINELEN 64
#define DATASIZE    4
#define DATAVALUE  "foo" 
#define INITIAL_TABLE_SIZE 64

#define IS_DEBUG_CONSOLE FALSE

// extern types
extern apr_pool_t* g_etch_main_pool;

// intern types
static int g_is_automated_test;

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
    //this_teardown();
    etch_runtime_shutdown();
    return 0;
}

/**
 * test_new_hashtable
 * unit test for new_hashtable
 */
static void test_new_hashtable(void)
{
    /* new_hashtable always returns mimimal initial size if the input value is too small */
    etch_hashtable* eht = new_hashtable(-1);
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), ETCH_DEFAULT_HASHTABLE_SIZE);
    etch_object_destroy(eht);

    /* the hashtable size will be modified to previous power of 2 */
    eht = new_hashtable(63);
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 32);  /* size will be 3 2instead of 64 */
    etch_object_destroy(eht);

    /* good input case, the size is within boundary and power of 2 */
    eht = new_hashtable(32);
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 32);
    etch_object_destroy(eht);

    /* the maximum initial size is MAX_INITIAL_HASHTABLE_SIZE */
    eht = new_hashtable(MAX_INITIAL_HASHTABLE_SIZE+1);
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), MAX_INITIAL_HASHTABLE_SIZE);
    etch_object_destroy(eht);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


static void test_map(void)
{
	etch_hashtable* map = new_etch_map(0);

	int count = 0;

	etch_int32*  content1 = new_int32(1);
	etch_string* content2 = new_stringa("Hallo");

	etch_int32*  key1 = (etch_int32*)new_stringa("Hallo");
	etch_string* key2 = (etch_string*)new_int32(1);

	count = etchmap_count(map);
	CU_ASSERT_TRUE(count == 0);

	etchmap_add(map, key1, content1);
	etchmap_add(map, key2, content2);

	count = etchmap_count(map);
	CU_ASSERT_TRUE(count == 2);

	etchmap_del(map,key1);
	etchmap_del(map,key2);

	count = etchmap_count(map);
	CU_ASSERT_TRUE(count == 0);

	key1 = new_int32(2);
	etchmap_add(map, key1, content1);
	key1->value = 3;
	etchmap_add(map, key1, content2);
	
	count = etchmap_count(map);
	CU_ASSERT_TRUE(count == 1);
}

/**
 * test_new_etch_hashtable
 * unit test for new_etch_hashtable
 */
//void test_new_etch_hashtable(void)
//{
//    /* new_etch_hashtable does not have meat in it */
//    etch_hashtable* eht = new_etch_hashtable();
//    CU_ASSERT_PTR_NOT_NULL_FATAL(eht);
//    CU_ASSERT_PTR_NULL(eht->realtable);
//    CU_ASSERT_PTR_NULL(((etch_object*)eht)->vtab);
//    etch_free(eht);
//}

/**
 * test_destroy_hashtable
 * unit test for destroy_hashtable
 */
//void test_destroy_hashtable(void)
//{
//    wchar_t *key = NULL, *val = NULL;
//
//    /* try to delete key and value but nothing in it */
//    etch_hashtable* eht = new_etch_hashtable();
//    destroy_hashtable(eht, TRUE, TRUE);
//
//    /* negative test with NULL hash table */
//    destroy_hashtable(NULL, TRUE, TRUE);
//
//    /* TODO: add more cases to verify key/value deletion */
//}

/**
 * test_jenkins_insert
 * unit test for jenkins_insert
 */
static void test_jenkins_insert(void)
{
    int keylen = 0, vallen=0;
    wchar_t key[100];

    etch_hashtable* eht = new_hashtable(16);
    
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 16);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 0);

    etch_snwprintf(key, 100, L"key%d", 0);
    keylen = (int)wcslen(key);
    vallen = 3;
    jenkins_insert(eht->realtable, key, (keylen+1)*sizeof(wchar_t), L"val", (vallen+1)*sizeof(wchar_t), 0, 0);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 16);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 1);
    /* insert the same key value pair */
    jenkins_insert(eht->realtable, key, (keylen+1)*sizeof(wchar_t), L"val", (vallen+1)*sizeof(wchar_t), 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 1);

    /* insert the same key but different value */
    jenkins_insert(eht->realtable, key, (keylen+1)*sizeof(wchar_t), L"va1", (vallen+1)*sizeof(wchar_t), 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 1);

    /* insert different key but the same value */
    etch_snwprintf(key, 100, L"key%d", 1);
    jenkins_insert(eht->realtable, key, (keylen+1)*sizeof(wchar_t), L"val", (vallen+1)*sizeof(wchar_t), 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 2);

    /* insert different key value pair */
    etch_snwprintf(key, 100, L"key%d", 2);
    jenkins_insert(eht->realtable, key, (keylen+1)*sizeof(wchar_t), L"va1", (vallen+1)*sizeof(wchar_t), 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 3);

    /* insert NULL key and value */
    jenkins_insert(eht->realtable, NULL, 0, NULL, 0, 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 3);

    etch_object_destroy(eht);
}

/**
 * test_jenkins_find
 * unit test for jenkins_find
 */
static void test_jenkins_find(void)
{
    int result = 0, keylen = 0, vallen = 0;
    wchar_t* key1 = 0;
    wchar_t* key2 = 0;
    wchar_t* val1 = 0;
    wchar_t* val2 = 0;
    etch_hashitem* outentry;
    etch_hashtable* eht = 0;

    eht = new_hashtable(32);
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 32);

    outentry = (etch_hashitem*)malloc(sizeof(etch_hashitem)); 
    outentry->key = malloc(sizeof(wchar_t) * 100);
    outentry->value = malloc(sizeof(wchar_t) * 100);

    key1 = malloc(sizeof(wchar_t) * 100);
    key2 = malloc(sizeof(wchar_t) * 100);
    etch_snwprintf(key1, 100, L"key%d", 1);
    etch_snwprintf(key2, 100, L"key%d", 2);
    keylen = (int)wcslen(key1);

    val1 = malloc(sizeof(wchar_t) * 100);
    val2 = malloc(sizeof(wchar_t) * 100);
    etch_snwprintf(val1, 100, L"val%d", 1);
    etch_snwprintf(val2, 100, L"val%d", 2);
    vallen = (int)wcslen(val1);

    jenkins_insert(eht->realtable, key1, (keylen + 1)*sizeof(wchar_t), val1, (vallen + 1)*sizeof(wchar_t), 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 1);

    /* get first, first key = first key */
    result = jenkins_find(eht->realtable, key2, keylen*2+2, 0, &outentry);
    CU_ASSERT_EQUAL(result, -1);

    jenkins_insert(eht->realtable, key2, keylen*2+2, val2, vallen*2+2, 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 2);

    /* get first, first key = first key */
    result = jenkins_find(eht->realtable, key2, keylen*2+2, 0, &outentry);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_EQUAL(key2, outentry->key);

    etch_object_destroy(eht);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_jenkins_findh
 * unit test for jenkins_findh
 */
static void test_jenkins_findh(void)
{
    int result = 0, keylen = 0, vallen = 0, hash = 0, bytes_allocated = 0;
    wchar_t* key1 = 0;
    wchar_t* key2 = 0;
    wchar_t* val1 = 0;
    wchar_t* val2 = 0;
    etch_hashitem* outentry;

    etch_hashtable* eht = new_hashtable(32);
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 32);

    outentry = (etch_hashitem*)malloc(sizeof(etch_hashitem)); 
    outentry->key   = malloc(sizeof(wchar_t) * 100);
    outentry->value = malloc(sizeof(wchar_t) * 100);

    key1 = malloc(sizeof(wchar_t) * 100);
    key2 = malloc(sizeof(wchar_t) * 100);
    etch_snwprintf(key1, 100, L"key%d", 1);
    etch_snwprintf(key2, 100, L"key%d", 2);
    keylen = (int)wcslen(key1);

    val1 = malloc(sizeof(wchar_t) * 100);
    val2 = malloc(sizeof(wchar_t) * 100);
    etch_snwprintf(val1, 100, L"val%d", 1);
    etch_snwprintf(val2, 100, L"val%d", 2);
    vallen = (int)wcslen(val1);

    jenkins_insert(eht->realtable, key1, (keylen + 1) * sizeof(wchar_t), val1, (vallen + 1) * sizeof(wchar_t), 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 1);
    jenkins_insert(eht->realtable, key2, (keylen + 1) * sizeof(wchar_t), val2, (vallen + 1) * sizeof(wchar_t), 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 2);

    /* Find the second key */
    result = jenkins_find(eht->realtable, key2, (keylen + 1) * sizeof(wchar_t), 0, &outentry);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_EQUAL(key2, outentry->key);

    etch_object_destroy(eht);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * etch_testobject  
 * test object for test_jenkins_inserth
 */
typedef struct etch_testobject
{
    etch_object object;

    char* hashvalue; 

} etch_testobject; 


 
/**
 * test_inserth_get_hashkey
 * object hashkey computation virtual for test_jenkins_inserth
 * attempts to compute and set hashkey in object, returning hashkey so set, or zero.
 */
static int test_inserth_get_hashkey(etch_object* obj)
{
    char* hashitem = ((etch_testobject*)obj)->hashvalue;
    if (NULL == hashitem) return 0;
    return obj->hashkey = jenkins_hashx(hashitem, (int) strlen(hashitem), 0);
}


/**
 * new_testobject  
 * construct test object for test_jenkins_inserth
 * illustrates that constructor should set the get_hashkey virtual.
 */
static etch_testobject* new_testobject(char* hashsource)
{
   etch_testobject* newobj = (etch_testobject*) new_object
        (sizeof(etch_testobject), ETCHTYPEB_NONE, CLASSID_NONE);

   newobj->hashvalue = hashsource;

   ((etch_object*)newobj)->get_hashkey = test_inserth_get_hashkey;

   ((etch_object*)newobj)->get_hashkey((etch_object*)newobj);

   return newobj;
}



/**
 * test_inserth_freehandler
 * hashtable content memory management callback for test_jenkins_inserth
 */
static int test_inserth_freehandler(etch_object* key, etch_object* val)
{
    etch_object_destroy(key);
    etch_object_destroy(val);
    return 0;
}


/**
 * test_jenkins_inserth
 * unit test for jenkins_inserth
 * this test was written before the etch etch_object* included a get_hashkey virtual
 */
static void test_jenkins_inserth(void)
{
    int result = 0; 
    unsigned hash1 = 0, hash2 = 0;
    etch_hashitem outentry, *outitem = &outentry; 
    char *s1 = "abracadabra", *s2 =  "gilgamesh";
    etch_testobject *key1, *key2;
    etch_object *val1, *val2;
    etch_hashtable* eht;
    memset(outitem, 0, sizeof(etch_hashitem));   

    key1 = new_testobject(s1);
    key2 = new_testobject(s2);

    val1 = new_object(sizeof(etch_object), ETCHTYPEB_NONE, CLASSID_NONE);
    val2 = new_object(sizeof(etch_object), ETCHTYPEB_NONE, CLASSID_NONE);

    eht = new_hashtable(32);
    CU_ASSERT_PTR_NOT_NULL_FATAL(eht);

    /* set hook to free all hashtable content when we destroy hashtable */
    eht->freehook = test_inserth_freehandler;

    CU_ASSERT_NOT_EQUAL_FATAL(((etch_object*)key1)->hashkey, 0); /* ensure test object ctor set hashkey */
    CU_ASSERT_NOT_EQUAL_FATAL(((etch_object*)key2)->hashkey, 0);
    /* recompute expected hashkeys and compare to effective hashkeys */
    hash1 = jenkins_hashx(s1, (int) strlen(s1), 0);
    hash2 = jenkins_hashx(s2, (int) strlen(s2), 0);
    CU_ASSERT_EQUAL(hash1,((etch_object*)key1)->hashkey);
    CU_ASSERT_EQUAL(hash2,((etch_object*)key2)->hashkey);

    /* insert pairs to map using the pre-computed hashkeys */
    result = jenkins_inserth(eht->realtable, key1, val1, NULL, NULL);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 1);

    result = jenkins_inserth(eht->realtable, key2, val2, NULL, NULL);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 2);

    /* look them up using expected hashkeys */
    result = jenkins_findh(eht->realtable, hash1, 0, &outitem);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_EQUAL(key1, outentry.key);

    result = jenkins_findh(eht->realtable, hash2, 0, &outitem);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_EQUAL(key2, outentry.key);

    etch_object_destroy(eht);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}

 
/**
 * default_get_hashkey
 * object hashkey computation virtual for test_default_hashkey
 * attempts to compute and set a hashkey in object, 
 * for use when no dedicated override is supplied.
 */
static int default_get_hashkey(etch_object* obj)
{
    void* hashitem = obj;  /* we use the object address as hash source */
    return obj->hashkey = jenkins_hashx((char*)&hashitem, sizeof(void*), 0);
}


/**
 * new_default_object
 * test object constructor for test_default_hashkey
 * sets a default hashkey function and invokes it
 */
static etch_testobject* new_default_object()
{
   etch_testobject* newobj = (etch_testobject*) new_object
        (sizeof(etch_testobject), ETCHTYPEB_NONE, CLASSID_NONE);

   ((etch_object*)newobj)->get_hashkey = default_get_hashkey;

   ((etch_object*)newobj)->get_hashkey((etch_object*)newobj);

   return newobj;
}


/**
 * test_default_hashkey
 * unit test for hashing using the default hashkey function
 * this test was written before the etch etch_object* included a get_hashkey virtual
 */
static void test_default_hashkey(void)
{
    int result = 0; 
    unsigned hash1 = 0, hash2 = 0;
    etch_hashitem outentry, *outitem = &outentry; 
    etch_testobject *key1, *key2, *val1, *val2;
    etch_hashtable* eht;
    memset(outitem, 0, sizeof(etch_hashitem));   

    key1 = new_default_object();
    key2 = new_default_object();
    CU_ASSERT_NOT_EQUAL_FATAL(((etch_object*)key1)->hashkey, 0); /* ensure test object ctor set hashkey */
    CU_ASSERT_NOT_EQUAL_FATAL(((etch_object*)key2)->hashkey, 0);
    val1 = new_default_object();
    val2 = new_default_object();
    eht = new_hashtable(32);
    /* set hook to free all hashtable content when we destroy hashtable */
    eht->freehook = test_inserth_freehandler;

    /* insert pairs to map using the pre-computed hashkeys */
    result = jenkins_inserth(eht->realtable, key1, val1, NULL, NULL);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 1);

    result = jenkins_inserth(eht->realtable, key2, val2, NULL, NULL);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 2);

    /* look them up using effective hashkeys */
    result = jenkins_findh(eht->realtable, ((etch_object*)key1)->hashkey, 0, &outitem);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_EQUAL(key1, outentry.key);

    result = jenkins_findh(eht->realtable, ((etch_object*)key2)->hashkey, 0, &outitem);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_EQUAL(key2, outentry.key);

    /* compute expected hashkeys from addresses of key objects */
    hash1 = jenkins_hashx((char*)&key1, sizeof(void*), 0);
    hash2 = jenkins_hashx((char*)&key2, sizeof(void*), 0);

    /* look them up using computed hashkeys */
    result = jenkins_findh(eht->realtable, hash1, 0, &outitem);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_EQUAL(key1, outentry.key);

    result = jenkins_findh(eht->realtable, hash2, 0, &outitem);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_EQUAL(key2, outentry.key);

    etch_object_destroy(eht);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_jenkins_first
 * unit test for jenkins_first
 */
static void test_jenkins_first(void)
{
    int result = 0, keylen = 0, vallen = 0;
    wchar_t* key1 = 0;
    wchar_t* key2 = 0;
    wchar_t* val1 = 0;
    wchar_t* val2 = 0;
    etch_hashitem* outentry;
    int ptrEqual;

    etch_hashtable* eht = new_hashtable(32);
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 32);

    outentry = (etch_hashitem*)malloc(sizeof(etch_hashitem)); 
    outentry->key = malloc(sizeof(wchar_t) * 100);
    outentry->value = malloc(sizeof(wchar_t) * 100);

    key1 = malloc(sizeof(wchar_t) * 100);
    key2 = malloc(sizeof(wchar_t) * 100);
    etch_snwprintf(key1, 100, L"key%d", 1);
    etch_snwprintf(key2, 100, L"key%d", 2);
    keylen = (int)wcslen(key1);

    val1 = malloc(sizeof(wchar_t) * 100);
    val2 = malloc(sizeof(wchar_t) * 100);
    etch_snwprintf(val1, 100, L"val%d", 1);
    etch_snwprintf(val2, 100, L"val%d", 2);
    vallen = (int)wcslen(val1);

    jenkins_insert(eht->realtable, key1, (keylen + 1) * sizeof(wchar_t), val1, (vallen + 1) * sizeof(wchar_t), 0, 0);
    jenkins_insert(eht->realtable, key2, (keylen + 1) * sizeof(wchar_t), val2, (vallen + 1) * sizeof(wchar_t), 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 2);

    /* get first, first key = first key */
    result = jenkins_first(eht->realtable, 0, &outentry);
    CU_ASSERT_EQUAL(result, 0);
    printf("jenkins_first: %ls=%ls\n", key1, (wchar_t*)outentry->key);
    ptrEqual = key1 == (wchar_t*)outentry->key || 
                key2 == (wchar_t*)outentry->key;
    CU_ASSERT_TRUE(ptrEqual);

    /* next is key2 */
    result = jenkins_next(eht->realtable, 0, &outentry);
    CU_ASSERT_EQUAL(result, 0);
    ptrEqual = key1 == (wchar_t*)outentry->key || 
                key2 == (wchar_t*)outentry->key;
    CU_ASSERT_TRUE(ptrEqual);

    etch_object_destroy(eht);
}


/**
 * test_jenkins_next
 * unit test for jenkins_next
 */
static void test_jenkins_next(void)
{
    int result = 0, keylen = 0, vallen = 0;
    wchar_t* key1 = 0;
    wchar_t* key2 = 0;
    wchar_t* key3 = 0;
    wchar_t* val1 = 0;
    wchar_t* val2 = 0;
    wchar_t* val3 = 0;
    etch_hashitem* outentry;

    etch_hashtable* eht = new_hashtable(32);
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 32);

    outentry = (etch_hashitem*)malloc(sizeof(etch_hashitem)); 
    outentry->key = malloc(sizeof(wchar_t) * 100);
    outentry->value = malloc(sizeof(wchar_t) * 100);

    key1 = malloc(sizeof(wchar_t) * 100);
    key2 = malloc(sizeof(wchar_t) * 100);
    key3 = malloc(sizeof(wchar_t) * 100);
    etch_snwprintf(key1, 100, L"key%d", 1);
    etch_snwprintf(key2, 100, L"key%d", 2);
    etch_snwprintf(key3, 100, L"key%d", 3);
    keylen = (int)wcslen(key1);

    val1 = malloc(sizeof(wchar_t) * 100);
    val2 = malloc(sizeof(wchar_t) * 100);
    val3 = malloc(sizeof(wchar_t) * 100);
    etch_snwprintf(val1, 100, L"val%d", 1);
    etch_snwprintf(val2, 100, L"val%d", 2);
    etch_snwprintf(val3, 100, L"val%d", 3);
    vallen = (int)wcslen(val1);

    jenkins_insert(eht->realtable, key1, (keylen+1)*sizeof(wchar_t), val1, (vallen+1)*sizeof(wchar_t), 0, 0);
    jenkins_insert(eht->realtable, key2, (keylen+1)*sizeof(wchar_t), val2, (vallen+1)*sizeof(wchar_t), 0, 0);
    jenkins_insert(eht->realtable, key3, (keylen+1)*sizeof(wchar_t), val3, (vallen+1)*sizeof(wchar_t), 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 3);

    result = jenkins_first(eht->realtable, 0, &outentry);
    CU_ASSERT_EQUAL(result, 0);

    result = jenkins_next(eht->realtable, 0, &outentry);
    CU_ASSERT_EQUAL(result, 0);

    result = jenkins_next(eht->realtable, 0, &outentry);
    CU_ASSERT_EQUAL(result, 0);

    result = jenkins_next(eht->realtable, 0, &outentry);
    CU_ASSERT_EQUAL(result, -1);

    etch_object_destroy(eht);
}


/**
 * test_jenkins_current
 * unit test for jenkins_current
 */
static void test_jenkins_current(void)
{
    int result = 0, keylen = 0, vallen = 0;
    wchar_t* key1 = 0;
    wchar_t* key2 = 0;
    wchar_t* val1 = 0;
    wchar_t* val2 = 0;
    etch_hashitem* outentry;

    etch_hashtable* eht = new_hashtable(32);
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 32);

    /* get current, no key */
    outentry = (etch_hashitem*)malloc(sizeof(etch_hashitem)); 
    outentry->key = malloc(sizeof(wchar_t) * 100);
    outentry->value = malloc(sizeof(wchar_t) * 100);
    result = jenkins_current(eht->realtable, 0, &outentry);
    CU_ASSERT_EQUAL(result, -1);

    key1 = malloc(sizeof(wchar_t) * 100);
    key2 = malloc(sizeof(wchar_t) * 100);
    etch_snwprintf(key1, 100, L"key%d", 1);
    etch_snwprintf(key2, 100, L"key%d", 2);
    keylen = (int)wcslen(key1);

    val1 = malloc(sizeof(wchar_t) * 100);
    val2 = malloc(sizeof(wchar_t) * 100);
    etch_snwprintf(val1, 100, L"val%d", 1);
    etch_snwprintf(val2, 100, L"val%d", 2);
    vallen = (int)wcslen(val1);

    jenkins_insert(eht->realtable, key1, (keylen+1)*sizeof(wchar_t), val1, (vallen+1)*sizeof(wchar_t), 0, 0);
    jenkins_insert(eht->realtable, key2, (keylen+1)*sizeof(wchar_t), val2, (vallen+1)*sizeof(wchar_t), 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 2);

    /* get current, current key = key2 */
    result = jenkins_current(eht->realtable, 0, &outentry);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_EQUAL(key2, outentry->key);

    /* remove second entry */
    result = jenkins_remove(eht->realtable, key2, (keylen+1)*sizeof(wchar_t), 0, &outentry);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_EQUAL(key2, outentry->key);

    /* get current, current key = key1 */
    result = jenkins_current(eht->realtable, 0, &outentry);
    CU_ASSERT_EQUAL(result, 0);
    CU_ASSERT_PTR_EQUAL(key1, outentry->key);

    etch_object_destroy(eht);
}

/**
 * test_jenkins_remove
 * unit test for jenkins_remove
 */
static void test_jenkins_remove(void)
{
    int count = 0, keylen = 0, vallen = 0;
    wchar_t* key1 = 0;
    wchar_t* key2 = 0;
    wchar_t* val1 = 0;
    //wchar_t* val2 = 0;
    etch_hashitem* outentry;

    etch_hashtable* eht = new_hashtable(32);
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 32);

    key1 = malloc(sizeof(wchar_t) * 100);
    key2 = malloc(sizeof(wchar_t) * 100);
    etch_snwprintf(key1, 100, L"key%d", 1);
    etch_snwprintf(key2, 100, L"key%d", 2);
    keylen = (int)wcslen(key1);

    val1 = malloc(sizeof(wchar_t) * 100);
    //val2 = malloc(sizeof(wchar_t) * 100);
    etch_snwprintf(val1, 100, L"val%d", 1);
    //etch_snwprintf(val2, 100, L"val%d", 2);
    vallen = (int)wcslen(val1);

    outentry = (etch_hashitem*)malloc(sizeof(etch_hashitem)); 
    outentry->key   = malloc(sizeof(wchar_t) * 100);
    outentry->value = malloc(sizeof(wchar_t) * 100);

    jenkins_insert(eht->realtable, key1, (keylen+1)*sizeof(wchar_t), val1, (vallen+1)*sizeof(wchar_t), 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 1);

    /* remove wrong entry */
    count = jenkins_remove(eht->realtable, key2, (keylen+1)*sizeof(wchar_t), 0, &outentry);
    CU_ASSERT_EQUAL(count, -1);

    /* remove the entry */
    count = jenkins_remove(eht->realtable, key1,(keylen+1)*sizeof(wchar_t), 0, &outentry);
    CU_ASSERT_EQUAL(count, 0);
    CU_ASSERT_PTR_EQUAL(key1, outentry->key);

    etch_object_destroy(eht);
}


/**
 * test_jenkins_clear
 * unit test for jenkins_clear
 */
static void test_jenkins_clear(void)
{
    int count = 0, keylen = 0, vallen = 0;
    wchar_t* key1 = 0;
    wchar_t* key2 = 0;
    wchar_t* val1 = 0;
    wchar_t* val2 = 0;

    etch_hashtable* eht = new_hashtable(32);
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 32);

    key1 = malloc(sizeof(wchar_t) * 100);
    key2 = malloc(sizeof(wchar_t) * 100);
    etch_snwprintf(key1, 100, L"key%d", 1);
    etch_snwprintf(key2, 100, L"key%d", 2);
    keylen = (int)wcslen(key1);

    val1 = malloc(sizeof(wchar_t) * 100);
    val2 = malloc(sizeof(wchar_t) * 100);
    etch_snwprintf(val1, 100, L"val%d", 1);
    etch_snwprintf(val2, 100, L"val%d", 2);
    vallen = (int)wcslen(val1);

    jenkins_insert(eht->realtable, key1, (keylen+1)*sizeof(wchar_t), val1, (vallen+1)*sizeof(wchar_t), 0, 0);
    jenkins_insert(eht->realtable, key2, (keylen+1)*sizeof(wchar_t), val2, (vallen+1)*sizeof(wchar_t), 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 2);
    count = jenkins_clear(eht->realtable, TRUE, TRUE, 0, 0);
    /* clear 2 key/val pair */
    CU_ASSERT_EQUAL(count, 2);
    CU_ASSERT_PTR_NOT_NULL(eht->realtable);

    etch_object_destroy(eht);
}


/**
 * test_jenkins_count
 * unit test for jenkins_count
 */
static void test_jenkins_count(void)
{
    etch_hashtable* eht = new_hashtable(32);
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 32);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 0);
    jenkins_insert(eht->realtable, L"key", 4 * sizeof(wchar_t), L"val", 4 * sizeof(wchar_t), 0, 0);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 32);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 1);
    etch_object_destroy(eht);
}

/**
 * test_jenkins_size
 * unit test for jenkins_size
 */
static void test_jenkins_size(void)
{
    int i = 0;
    int keylen = 0;
    wchar_t key[100];

    etch_hashtable* eht = new_hashtable(16);
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 16);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 0);
    for (i=0; i<17; i++)
    {
        etch_snwprintf(key, 100, L"key%d", i);
        keylen = (int)wcslen(key);
        jenkins_insert(eht->realtable, key, (keylen + 1) * sizeof(wchar_t), L"val", 4 * sizeof(wchar_t), 0, 0);
    }
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 32);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 17);
    etch_object_destroy(eht);
}

/**
 * test_jenkins_stats
 * unit test for jenkins_stats
 */
static void test_jenkins_stats(void)
{
    
}

/**
 * test_jenkins_hash
 * unit test for jenkins_hash
 */
//void test_jenkins_hash(void)
//{
//    int hash = 0;
//
//    etch_hashtable* eht = new_etch_hashtable();
//    CU_ASSERT_PTR_NOT_NULL(eht);
//    CU_ASSERT_PTR_NULL(eht->realtable);
//    /* realtable is NULL */
//    hash = jenkins_hash(eht->realtable, "key", 3, 0, 0, 0);
//    CU_ASSERT_EQUAL(hash, 0);
//    destroy_hashtable(eht, TRUE, TRUE);
//
//    eht = new_hashtable(16);
//    CU_ASSERT_PTR_NOT_NULL(eht);
//    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 16);
//    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 0);
//    /* key is NULL */
//    hash = jenkins_hash(eht->realtable, 0, 0, 0, 0, 0);
//    CU_ASSERT_EQUAL(hash, 0);
//    /* key length is 0 */
//    hash = jenkins_hash(eht->realtable, "key", 0, 0, 0, 0);
//    CU_ASSERT_EQUAL(hash, 0);
//    /* key length is MAX_ETCHHASHKEY + 1 */
//    hash = jenkins_hash(eht->realtable, "key", MAX_ETCHHASHKEY+1, 0, 0, 0);
//    CU_ASSERT_EQUAL(hash, 0);
//    /* get a good hash */
//    hash = jenkins_hash(eht->realtable, "key", 3, 0, 0, 0);
//    CU_ASSERT_NOT_EQUAL(hash, 0);
//    /* hash should be the same is input arguments are the same */
//    CU_ASSERT_EQUAL(hash, jenkins_hash(eht->realtable, "key", 3, 0, 0, 0));
//    /* hash should be different if input arguments are different */
//    CU_ASSERT_NOT_EQUAL(hash, jenkins_hash(eht->realtable, "kee", 3, 0, 0, 0));
etch_object_destroy(eht);
//}


/**
 * test_intkey
 * test using a 32 bit int as a key
 */
#pragma warning (disable:4996)
static void test_intkey(void)
{
  	etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket;
    char* test_string = "it works"; 
    unsigned int key = 12345, key2 = 11111, key3 = 11111;
    int value_len = (int)strlen(test_string)+1; /* string length of value */
    struct i_hashtable* vtab = NULL;
    etch_hashtable* ht = new_hashtable(4);

    /* our hash key is a 4-byte item, which we will use to contain the 
       value of a 32 bit int.
     */
	char* pkey = malloc(sizeof(int));

    /*  the string pval is our hash value */
    char* pval = malloc(value_len);  
    strcpy(pval, test_string); 

    /* copy the int into our hash key.
     */
    memcpy(pkey, &key, sizeof(int));
    vtab = (struct i_hashtable*)((etch_object*)ht)->vtab;
    CU_ASSERT_EQUAL(vtab->insert(ht->realtable, pkey, sizeof(int), pval, value_len, 0, 0), 0);
    CU_ASSERT_EQUAL(vtab->find  (ht->realtable, pkey, sizeof(int), NULL, &myentry), 0);
    CU_ASSERT_EQUAL(vtab->find  (ht->realtable, &key, sizeof(int), NULL, &myentry), 0);

    /* test two pointers to the same value.
     */
    CU_ASSERT_EQUAL(vtab->insert(ht->realtable, &key2, sizeof(int), pval, value_len, 0, 0), 0);
    CU_ASSERT_EQUAL(vtab->find  (ht->realtable, &key3, sizeof(int), NULL, &myentry), 0);

    destroy_hashtable(ht, FALSE, FALSE);
    free(pkey);
    free(pval);
}


/**
 * test_jenkins_destroy
 * unit test for jenkins_destroy
 */
//void test_jenkins_destroy(void)
//{
//    int result = 0;
//
//    etch_hashtable* eht = new_etch_hashtable();
//    CU_ASSERT_PTR_NOT_NULL(eht);
//    CU_ASSERT_PTR_NULL(eht->realtable);
//    /* realtable is NULL */
//    result = jenkins_destroy(eht->realtable, 0, 0);
//    CU_ASSERT_EQUAL(result, -1);
//
//    eht = new_hashtable(16);
//    CU_ASSERT_PTR_NOT_NULL(eht);
//    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 16);
//    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 0);
//    /* realtable is not NULL */
//    result = jenkins_destroy(eht->realtable, 0, 0);
//    CU_ASSERT_EQUAL(result, 0);
//}

/**
 * test_jenkins_create
 * unit test for jenkins_create
 */
//void test_jenkins_create(void)
//{
//    int result = 0;
//
//    etch_hashtable* eht = new_etch_hashtable();
//    CU_ASSERT_PTR_NOT_NULL(eht);
//    CU_ASSERT_PTR_NULL(eht->realtable);
//    /* realtable is NULL */
//    result = jenkins_create(16, 0, eht->realtable);
//    CU_ASSERT_EQUAL(result, -1);
//    destroy_hashtable(eht, FALSE, FALSE);
//
//    eht = new_hashtable(16);
//    CU_ASSERT_PTR_NOT_NULL(eht);
//    CU_ASSERT_PTR_NOT_NULL(eht->realtable);
//    /* realtable is NULL */
//    result = jenkins_create(16, 0, eht->realtable);
//    CU_ASSERT_EQUAL(result, 0);
//    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 16);
//    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 0);
//    destroy_hashtable(eht, FALSE, FALSE);
//}

/**
 * test_ctor_jenkins_hashtable
 * unit test for ctor_jenkins_hashtable
 */
static void test_ctor_jenkins_hashtable(void)
{
    etch_hashtable* eht = ctor_jenkins_hashtable(16);
    CU_ASSERT_PTR_NOT_NULL(eht);
    CU_ASSERT_PTR_NOT_NULL(eht->realtable);
    CU_ASSERT_PTR_NOT_NULL(((etch_object*)eht)->vtab);
    CU_ASSERT_EQUAL(jenkins_size(eht->realtable, 0, 0), 16);
    CU_ASSERT_EQUAL(jenkins_count(eht->realtable, 0, 0), 0);

    destroy_hashtable(eht, FALSE, FALSE);
}


#pragma warning (disable:4996)

/*
 * test_key_pointer: test using a memory address as a hash key.
 * return 1 if OK, zero if failed.
 */
static int test_key_pointer(etch_hashtable* ht)
{
	etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket;
    char* test_string = "it works"; 
    const int value_len = (int)strlen(test_string)+1; /* string length of value */
    const int key_len = sizeof(char*); 

    /* our hash key is a 4-byte item, which we will use to contain the 
       value of a memory address.
     */
	char* pkey = malloc(key_len);

    /* the value of the pval memory address is our hash key.
     * the string pval is our hash value. Of course, our hash table stores pointers,
     * not values, so the key is &pval, and the value is pval.
     */
    char* pval = malloc(value_len);  
    strcpy(pval, test_string); 

    /* copy the memory address into our hash key.
     */
    memcpy(pkey, &pval, sizeof(char*));

    CU_ASSERT_EQUAL(((struct i_hashtable*)((etch_object*)ht)->vtab)->insert(ht->realtable, pkey, key_len, pval, value_len, 0, 0), 0);

    CU_ASSERT_EQUAL(((struct i_hashtable*)((etch_object*)ht)->vtab)->find(ht->realtable, pkey, key_len, NULL, &myentry), 0);

    CU_ASSERT_PTR_NOT_NULL(myentry->value);

    /* This code demonstrates what we've done here. The key is the value of a
     * char*, and the value is that same char*. So, indirectly dereferencing  
     * the key must elicit the same result as dereferencing the value. 
     */
    CU_ASSERT_EQUAL(memcmp(myentry->key, &myentry->value, sizeof(char*)), 0);
    
    CU_ASSERT_EQUAL(strcmp(*(char**)myentry->key, (char*)myentry->value), 0);

	return 1;
}

/**
 * test_all
 * unit test for various functions strung together
 */
static void test_combo(void)
{
    char  buf[MAXLINELEN];
    char* pkey; void* pdata;
    int   n, keylen, myincount=0, myhashcount=0, myfreecount=0;
    etch_hashtable* myhash;
    etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket;
    unsigned hashed;
    char  c = 0, *p;
    FILE* f = 0;
    p = "abracadabra"; n = (int)strlen(p); // Test the hash function
    hashed = etchhash(p, n, 0);

    f = fopen(TESTDATAPATH,"r");
    CU_ASSERT_PTR_NOT_NULL_FATAL(f);

    myhash = new_hashtable(INITIAL_TABLE_SIZE);  /* create hash table */
    CU_ASSERT_PTR_NOT_NULL_FATAL(myhash);

    ((struct i_hashtable*)((etch_object*)myhash)->vtab)->stats(myhash->realtable, 0, 0); /* show table stats */
    n = ((struct i_hashtable*)((etch_object*)myhash)->vtab)->size(myhash->realtable, 0, 0);
    CU_ASSERT_EQUAL(n, INITIAL_TABLE_SIZE);
  
    while (fgets((char*)buf, MAXLINELEN, f))   /* read the testdata file line by line */ 
    {
        unsigned char* p = buf; while(*p++) if (*p <= 0x0d) *p = 0;       /* strip crlf */
        keylen = (int)strlen(buf);   /* we use null termed key, but it is not necessary */ 
        pkey   = malloc(keylen+1); strcpy(pkey, buf);    
        pdata  = malloc(DATASIZE);  /* note that user allocates memory for key and data */
        strcpy(pdata, DATAVALUE);   /* hashtable does not make copies of key or of data */
                                    /* insert the new hashtable entry */          
        if  (0 == ((struct i_hashtable*)((etch_object*)myhash)->vtab)->insert(myhash->realtable, pkey, keylen, pdata, DATASIZE, 0, 0))         
        {    myincount++;           /* point at current entry in hashtable */
             ((struct i_hashtable*)((etch_object*)myhash)->vtab)->current(myhash->realtable, 0, &myentry);  
             CU_ASSERT_PTR_EQUAL(myentry->key, pkey); /* ensure items just inserted are there */
             CU_ASSERT_PTR_EQUAL(myentry->value, pdata);
        }
        else                        /* insert failure - probably duplicate */
        {    free(pkey);  
             free(pdata);
        }
    }

    fclose(f); f = 0;
    ((struct i_hashtable*)((etch_object*)myhash)->vtab)->stats(myhash->realtable, 0, 0); /* show table stats */ 

    pkey = "banana";  /* test a hashtable lookup */

    CU_ASSERT_EQUAL(((struct i_hashtable*)((etch_object*)myhash)->vtab)->find(myhash->realtable, pkey, (int)strlen(pkey), NULL, &myentry), 0);
    CU_ASSERT_EQUAL(strcmp(pkey, myentry->key), 0);

    CU_ASSERT_EQUAL(((struct i_hashtable*)((etch_object*)myhash)->vtab)->first(myhash->realtable, NULL, &myentry), 0);
    myhashcount = 1;

    while(0 == ((struct i_hashtable*)((etch_object*)myhash)->vtab)->next(myhash->realtable, NULL, &myentry))          
    {    
        myhashcount++;
    }

    /* test using pointer as key, adding 0 or 1 to myhashcount */
    myhashcount += test_key_pointer(myhash);

    /* destroy hash table and all its contents (free key and data memory) */
    myfreecount = ((struct i_hashtable*)((etch_object*)myhash)->vtab)->clear(myhash->realtable, TRUE, TRUE, 0, 0);

    CU_ASSERT_EQUAL(myfreecount, myhashcount);
}


/**
 * _tmain
 * unit test entry point
 */
//int wmain( int argc, wchar_t* argv[], wchar_t* envp[])
CU_pSuite test_etch_hashtable_suite()
{
    CU_pSuite ps = CU_add_suite("suite_etchhash", init_suite, clean_suite);
   
    CU_add_test(ps, "new_hashtable", test_new_hashtable);
    //CU_add_test(ps, "new_etch_hashtable", test_new_etch_hashtable);
    //CU_add_test(ps, "destroy_hashtable", test_destroy_hashtable);
    //CU_add_test(ps, "jenkins_hash", test_jenkins_hash);
    CU_add_test(ps, "jenkins_find", test_jenkins_find);
    CU_add_test(ps, "jenkins_findh", test_jenkins_findh);
    CU_add_test(ps, "jenkins default key", test_default_hashkey);
    CU_add_test(ps, "jenkins_first", test_jenkins_first);
    CU_add_test(ps, "jenkins_next", test_jenkins_next);
    CU_add_test(ps, "jenkins_current", test_jenkins_current);  
    CU_add_test(ps, "jenkins_remove", test_jenkins_remove);  
    CU_add_test(ps, "jenkins_clear", test_jenkins_clear);  
    CU_add_test(ps, "jenkins_count", test_jenkins_count);  
    CU_add_test(ps, "jenkins_size", test_jenkins_size);  
    CU_add_test(ps, "jenkins_insert", test_jenkins_insert);  
    CU_add_test(ps, "jenkins_inserth", test_jenkins_inserth);  
    CU_add_test(ps, "jenkins_stats", test_jenkins_stats);  
    //CU_add_test(ps, "jenkins_destroy", test_jenkins_destroy);  
    //CU_add_test(ps, "jenkins_create", test_jenkins_create);  
    CU_add_test(ps, "ctor_jenkins_hashtable", test_ctor_jenkins_hashtable);   
    CU_add_test(ps, "test integer key", test_intkey);  
    CU_add_test(ps, "test combinations", test_combo);    
	CU_add_test(ps, "test map", test_map);    

    return ps;
}
