/**
 * test_type.c 
 * Tests the C implementation of the etch_type object.
 * This is the same as the id_name test, but using the etch_type typedef.
 */

#include <stdio.h>
#include "cunit.h"
#include "basic.h"
#include "automated.h"
#include "type.h"
#include "etch_global.h"

#define ID 987654321 /* pre-defined type ID */

/**
 * init_suite_type
 * Initialization tasks for the test suite
 */
int init_suite_type(void)
{
    return 0;
}

/**
 * clean_suite_type
 * Clean up tasks for the test suite
 */
int clean_suite_type(void)
{
    return 0;
}

/**
 * test_type_ctor1
 * Unit test type_ctor1
 */
void test_type_ctor1(void)
{
    const wchar_t* wstr = L"abracadabra";
    const size_t numElements = wcslen(wstr);
    const size_t numBytes = sizeof(wchar_t) * numElements;

    wchar_t *name = NULL;
    size_t bytelen = 0;
    etch_type *type = NULL;
    errno_t result = 0;

    /* name is NULL */
    type = type_ctor1(name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(type);

    /* assign name */
    name = malloc(numBytes+2);
    CU_ASSERT_PTR_NOT_NULL(name);

    result = wcscpy_s(name, numElements+1, wstr);
    CU_ASSERT_EQUAL(result, 0);

    /* bytelen < 0 */
    bytelen = -1;
    type = type_ctor1(name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(type);
    
    /* bytelen > max bytelen */
    bytelen = MAX_ETCHHASHKEY*sizeof(wchar_t)+1;
    type = type_ctor1(name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(type);

    /* positive test with correct arguments */
    bytelen = numBytes;
    type = type_ctor1(name, bytelen, NULL);

    /* input name and type name should be the same */
    CU_ASSERT_PTR_EQUAL(type->name, name);

    /* name byte length should be the same as input byte length */
    CU_ASSERT_EQUAL(type->namebytelen, bytelen);

    /* no id, so generated is is the hash based on name and bytelength */
    CU_ASSERT_EQUAL(type->id, id_name_hashfunc(name, bytelen));

    /* destry the type */
    type_destroy(type);

    /* clean up */
    if (name != NULL)
        free(name);
}

/**
 * test_type_ctor2
 * Unit test type_ctor2
 * TODO this test is wrong -- need to test constructor 2, not 1
 */
void test_type_ctor2(void)
{
    const wchar_t* wstr = L"abracadabra";
    const size_t numElements = wcslen(wstr);
    const size_t numBytes = sizeof(wchar_t) * numElements;

    wchar_t *name = NULL;
    size_t bytelen = 0;
    etch_type *type = NULL;
    errno_t result = 0;

    /* name is NULL */
    type = new_type_1(name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(type);

    /* assign name */
    name = malloc(numBytes+2);
    CU_ASSERT_PTR_NOT_NULL(name);

    result = wcscpy_s(name, numElements+1, wstr);
    CU_ASSERT_EQUAL(result, 0);

    /* bytelen < 0 */
    bytelen = -1;
    type = new_type_1(ID, name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(type);
    
    /* bytelen > max bytelen */
    bytelen = MAX_ETCHHASHKEY*sizeof(wchar_t)+1;
    type = type_ctor2(ID, name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(type);

    /* positive test with correct arguments */
    bytelen = numBytes;
    type = type_ctor1(ID, name, bytelen, NULL);

    /* input name and type name should be the same */
    CU_ASSERT_PTR_EQUAL(type->name, name);

    /* name byte length should be the same as input byte length */
    CU_ASSERT_EQUAL(type->namebytelen, bytelen);

    /* no id, so generated is is the hash based on name and bytelength */
    CU_ASSERT_EQUAL(type->id, ID);

    /* destry the type */
    type_destroy(type);

    /* clean up */
    if (name != NULL)
        free(name);
}

/**
 * test_type_destroy
 * Unit test type_destroy
 */
void test_type_destroy(void)
{
    const wchar_t* wstr = L"abracadabra";
    const size_t numElements = wcslen(wstr);
    const size_t numBytes = sizeof(wchar_t) * numElements;

    wchar_t *name = NULL;
    size_t bytelen = 0;
    etch_type *type = NULL;
    errno_t result = 0;

    /* create type */
    name = malloc(numBytes+2);
    CU_ASSERT_PTR_NOT_NULL(name);

    result = wcscpy_s(name, numElements+1, wstr);
    CU_ASSERT_EQUAL(result, 0);

    bytelen = numBytes;
    type = type_ctor1(name, bytelen, NULL);

    CU_ASSERT_PTR_EQUAL(type->name, name);

    /* name byte length should be the same as input byte length */
    CU_ASSERT_EQUAL(type->namebytelen, bytelen);

    /* no id, so generated is is the hash based on name and bytelength */
    CU_ASSERT_EQUAL(type->id, id_name_hashfunc(name, bytelen));

    /* destry the type */
    type_destroy(type);

    if (type != NULL)
    {
        /* old value should be gone */
        CU_ASSERT_NOT_EQUAL(type->namebytelen, bytelen);

        /* old value should be gone */
        CU_ASSERT_NOT_EQUAL(type->id, id_name_hashfunc(name, bytelen));
    }

    if (name != NULL)
        free(name);
}

/**
 * test_type_hashfunc
 * Unit test type_hashfunc
 */
void test_type_hashfunc(void)
{
    const wchar_t* wstr1 = L"a1";
    const wchar_t* wstr2 = L"a2";
    const size_t numElements1 = wcslen(wstr1);
    const size_t numElements2 = wcslen(wstr2);
    const size_t numBytes1 = sizeof(wchar_t) * numElements1;
    const size_t numBytes2 = sizeof(wchar_t) * numElements2;
    int hash1 = 0;
    int hash2 = 0;

    hash1 = type_hashfunc(wstr1, numBytes1);
    hash2 = type_hashfunc(wstr1, numBytes1);

    CU_ASSERT_EQUAL(hash1, hash2);

    hash2 = type_hashfunc(wstr2, numBytes2);

    CU_ASSERT_NOT_EQUAL(hash1, hash2);
}

/**
 * test_all
 * Unit test for all type features
 */
void test_all(void)
{
    char c=0;
    int  testresult = FALSE;
    errno_t result1 = 0, result2 = 0;
    size_t  actlen1 = 0, actlen2 = 0;
    unsigned hash1  = 0, hash2   = 0;
    etch_type static_type;

    etch_hashtable* myhash = NULL;    /* define a myhash */
    etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket;

    wchar_t* wstr1 = L"abracadabra"; /* is L macro Windows only? */
    wchar_t* wstr2 = L"gilgamesh";

    const size_t numElements1 = wcslen(wstr1);
    const size_t numElements2 = wcslen(wstr2);

    const size_t numBytes1 = sizeof(wchar_t) * numElements1;
    const size_t numBytes2 = sizeof(wchar_t) * numElements2;

    wchar_t *key1 = NULL, *key2 = NULL;
    wchar_t *foundkey1 = NULL,  *foundkey2 = NULL; 
    etch_type *type1 = NULL, *type2 = NULL;

    wprintf(L"begin type test\n\n");

    do {

    /* verify wcscpy_s: allocate new keys on heap and copy into them keys from stack.   
     * This illustrates some important considerations not obvious from the MSDN docs.   
     * wcscpy_s parameter 2 is documented as "numberOfElements: size of the buffer".   
     * Trial and error shows that we must allocate a (wcslen(s)+1)*2) byte buffer, and 
     * specify the wcscpy_s numberOfElements parameter as wcslen(s)+1. The MSDN docs    
     * don't explain this because the examples are all templated C++ in which there is  
     * no length parameter to wcscpy_s(). */
    key1 = malloc(numBytes1 + 2);  /* NOTE 1: wchar_t buffer size will be */
    key2 = malloc(numBytes2 + 2);  /* (number of characters + 1) * 2      */
    if (key1 == NULL || key2 == NULL) break;
        
    result1 = wcscpy_s(key1, numElements1+1, wstr1);  /* NOTE 2: wcscpy_s param 2 is */
    result2 = wcscpy_s(key2, numElements1+1, wstr2);  /* number of characters + 1    */

    if (result1 != 0 || result2 != 0) 
    {   wprintf(L"wscpy_s error %d %d\n",result1,result2);
        break;
    }
    actlen1 = wcslen(key1); actlen2 = wcslen(key2);
    wprintf(L"key 1 %s length %d bytes %d\n",key1,actlen1,numBytes1);
    wprintf(L"key 2 %s length %d bytes %d\n",key2,actlen2,numBytes2);

    /* create an etch_id_name object for each of the keys */
    type1 = new_type_1(key1, (int)numBytes1, NULL);
    type2 = new_type_2(ID, key2, (int)numBytes2, &static_type);
    if (!type1 || !type2)
    {   wprintf(L"type_ctor error\n");
        break;
    }          
    wprintf(L"type 1: %s %d %d\n",type1->name,type1->namebytelen,type1->id);
    wprintf(L"type 2: %s %d %d\n",type2->name,type2->namebytelen,type2->id);

    /* do an ad hoc hash of the two keys and ensure that the hashkeys match */
    hash1 = type_hashfunc(key1, numBytes1);
    hash2 = type_hashfunc(key2, numBytes2);
    if (!hash1 || !hash2)
    {   wprintf(L"type_hashfunc error\n");
        break;
    } 
    wprintf(L"ad hoc hash of key 1: %d key 2: %d\n",hash1,hash2);  

    myhash = new_hashtable(16);  /* create a hash table */
    if (!myhash) 
    {   wprintf(L"new_myhash error\n");
        break;
    } 

    /* insert the two keys into the hashtable allowing the hashtable to hash the keys */
    if (0 != myhash->vtab->insert(myhash->realtable, key1, (int)numBytes1, NULL,0,0,0))         
    {   wprintf(L"myhash->vtab->insert %s %d error\n", key1, numBytes1);
        break;
    }
    if (0 != myhash->vtab->insert(myhash->realtable, key2, (int)numBytes2, NULL,0,0,0))         
    {   wprintf(L"myhash->vtab->insert %s %d error\n", key2, numBytes2);
        break;
    }

    /* do a myhash lookup on our ad-hoc hashed keys, which should be identical */
    /* to the hashkeys created by the hashtable */
    if (0 == myhash->vtab->findh(myhash->realtable, hash1, NULL, &myentry))
        foundkey1 = (wchar_t*)myentry->key;    
    else
    {   wprintf(L"myhash->vtab->findh %d error\n", hash1);
        break;
    }

    if (0 == myhash->vtab->findh(myhash->realtable, hash2, NULL, &myentry))
        foundkey2 = (wchar_t*)myentry->key;
    else
    {   wprintf(L"myhash->vtab->findh %d error\n", hash2);
        break;
    }

    /* verify that the lookup returned good key pointers */
    if (!foundkey1 || !foundkey2)
    {   wprintf(L"myhash->vtab->findh %d error\n", hash2);
        break;
    }

    /* display keys found on lookup */
    wprintf(L"key %s found on hash %d lookup\n",foundkey1, hash1);
    wprintf(L"key %s found on hash %d lookup\n",foundkey2, hash2);

    if (0 != memcmp(foundkey1, key1, numBytes1) || 
        0 != memcmp(foundkey2, key2, numBytes2))  
    {   wprintf(L"error: keys inserted do not match keys located\n");
        break;
    }
 
    testresult = TRUE;

    } while(0);

    if (key1) 
        free(key1); 
    if (key2) 
        free(key2);  
    type_destroy(type1);
    /* type_destroy(type2); */    /* allocated on stack */

    /* destroy hashtable *without* freeing its contents */
    destroy_hashtable(myhash, FALSE, FALSE);

    debug_showmem(TRUE, TRUE);
    /* note: cache clear must occur last since still accessing vtables */
    cache_clear(); 
}

/**
 * _tmain
 * Unit test entry point
 */
int _tmain(int argc, _TCHAR* argv[])
{
    CU_pSuite pSuite = NULL;
    /* initialize the CUnit test registry */
    if (CUE_SUCCESS != CU_initialize_registry())
        return CU_get_error();

    /* set output XML file for automatic tests result */
    CU_set_output_filename("../test_type");

    /* add a suite to the registry */
    pSuite = CU_add_suite("suite_type", init_suite_type, clean_suite_type);
    if (NULL == pSuite) 
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (CU_add_test(pSuite, "type_ctor1", test_type_ctor1) == NULL)   
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuite, "type_ctor2", test_type_ctor2) == NULL)   
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuite, "type_destroy", test_type_destroy) == NULL)   
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuite, "type_hashfunc", test_type_hashfunc) == NULL)   
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuite, "all", test_all) == NULL)   
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    if (argc == 2 && !wcscmp(argv[1], L"-a"))
    {
        /* Run automatic tests */
        CU_automated_run_tests();
    }
    else
    {
        /* Run all tests using the CUnit Basic interface */
        CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    }
    CU_cleanup_registry();

    return CU_get_error();    
}
