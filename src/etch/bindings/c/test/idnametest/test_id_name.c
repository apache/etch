/**
 * test_id_name.c 
 * Tests the C implementation of the etch_id_name object.
 */

#include <stdio.h>
#include "cunit.h"
#include "basic.h"
#include "automated.h"
#include "id_name.h"
#include "etchhash.h"
#include "etchmem.h"

#define ID 987654321 /* pre-defined ID */

/**
 * init_suite_id_name
 * Initialization tasks for the test suite
 */
int init_suite_id_name(void)
{
    return 0;
}

/**
 * clean_suite_id_name
 * Clean up tasks for the test suite
 */
int clean_suite_id_name(void)
{
    return 0;
}

/**
 * test_id_name_1
 * Unit test id_name_1
 */
void test_id_name_1(void)
{
    const wchar_t* wstr = L"abracadabra";
    const size_t numElements = wcslen(wstr);
    const size_t numBytes = sizeof(wchar_t) * numElements;

    wchar_t *name = NULL;
    size_t bytelen = 0;
    etch_id_name *id_name = NULL;
    errno_t result = 0;

    /* name is NULL */
    id_name = id_name_1(name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(id_name);

    /* assign name */
    name = malloc(numBytes+2);
    CU_ASSERT_PTR_NOT_NULL(name);

    result = wcscpy_s(name, numElements+1, wstr);
    CU_ASSERT_EQUAL(result, 0);

    /* bytelen < 0 */
    bytelen = -1;
    id_name = id_name_1(name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(id_name);
    
    /* bytelen > max bytelen */
    bytelen = MAX_ETCHHASHKEY*sizeof(wchar_t)+1;
    id_name = id_name_1(name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(id_name);

    /* positive test with correct arguments */
    bytelen = numBytes;
    id_name = id_name_1(name, bytelen, NULL);

    /* input name and id_name's name should be the same */
    CU_ASSERT_PTR_EQUAL(id_name->name, name);

    /* name byte length should be the same as input byte length */
    CU_ASSERT_EQUAL(id_name->namebytelen, bytelen);

    /* no id, so generated is is the hash based on name and bytelength */
    CU_ASSERT_EQUAL(id_name->id, id_name_hashfunc(name, bytelen));

    /* destry the id_name */
    id_name_destroy(id_name);

    /* clean up */
    if (name != NULL)
        free(name);
}

/**
 * test_id_name_2
 * Unit test id_name_2
 */
void test_id_name_2(void)
{
    const wchar_t* wstr = L"abracadabra";
    const size_t numElements = wcslen(wstr);
    const size_t numBytes = sizeof(wchar_t) * numElements;

    wchar_t *name = NULL;
    size_t bytelen = 0;
    etch_id_name *id_name = NULL;
    errno_t result = 0;

    /* name is NULL */
    id_name = id_name_1(name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(id_name);

    /* assign name */
    name = malloc(numBytes+2);
    CU_ASSERT_PTR_NOT_NULL(name);

    result = wcscpy_s(name, numElements+1, wstr);
    CU_ASSERT_EQUAL(result, 0);

    /* bytelen < 0 */
    bytelen = -1;
    id_name = id_name_2(ID, name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(id_name);
    
    /* bytelen > max bytelen */
    bytelen = MAX_ETCHHASHKEY*sizeof(wchar_t)+1;
    id_name = id_name_2(ID, name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(id_name);

    /* positive test with correct arguments */
    bytelen = numBytes;
    id_name = id_name_2(ID, name, bytelen, NULL);

    /* input name and id_name's name should be the same */
    CU_ASSERT_PTR_EQUAL(id_name->name, name);

    /* name byte length should be the same as input byte length */
    CU_ASSERT_EQUAL(id_name->namebytelen, bytelen);

    /* no id, so generated is is the hash based on name and bytelength */
    CU_ASSERT_EQUAL(id_name->id, ID);

    /* destry the id_name */
    id_name_destroy(id_name);

    /* clean up */
    if (name != NULL)
        free(name);
}

/**
 * test_id_name_destroy
 * Unit test id_name_destroy
 */
void test_id_name_destroy(void)
{
    const wchar_t* wstr = L"abracadabra";
    const size_t numElements = wcslen(wstr);
    const size_t numBytes = sizeof(wchar_t) * numElements;

    wchar_t *name = NULL;
    size_t bytelen = 0;
    etch_id_name *id_name = NULL;
    errno_t result = 0;

    /* create id_name */
    name = malloc(numBytes+2);
    CU_ASSERT_PTR_NOT_NULL(name);

    result = wcscpy_s(name, numElements+1, wstr);
    CU_ASSERT_EQUAL(result, 0);

    bytelen = numBytes;
    id_name = id_name_1(name, bytelen, NULL);

    CU_ASSERT_PTR_EQUAL(id_name->name, name);

    /* name byte length should be the same as input byte length */
    CU_ASSERT_EQUAL(id_name->namebytelen, bytelen);

    /* no id, so generated is is the hash based on name and bytelength */
    CU_ASSERT_EQUAL(id_name->id, id_name_hashfunc(name, bytelen));

    /* destry the id_name */
    id_name_destroy(id_name);

    if (id_name != NULL)
    {
        /* old value should be gone */
        CU_ASSERT_NOT_EQUAL(id_name->namebytelen, bytelen);

        /* old value should be gone */
        CU_ASSERT_NOT_EQUAL(id_name->id, id_name_hashfunc(name, bytelen));
    }

    if (name != NULL)
        free(name);
}

/**
 * test_id_name_hashfunc
 * Unit test id_name_hashfunc
 */
void test_id_name_hashfunc(void)
{
    const wchar_t* wstr1 = L"a1";
    const wchar_t* wstr2 = L"a2";
    const size_t numElements1 = wcslen(wstr1);
    const size_t numElements2 = wcslen(wstr2);
    const size_t numBytes1 = sizeof(wchar_t) * numElements1;
    const size_t numBytes2 = sizeof(wchar_t) * numElements2;
    int hash1 = 0;
    int hash2 = 0;

    hash1 = id_name_hashfunc(wstr1, numBytes1);
    hash2 = id_name_hashfunc(wstr1, numBytes1);

    CU_ASSERT_EQUAL(hash1, hash2);

    hash2 = id_name_hashfunc(wstr2, numBytes2);

    CU_ASSERT_NOT_EQUAL(hash1, hash2);
}

/**
 * test_id_name_hash
 * Unit test id_name_hash
 */
void test_id_name_hash(void)
{
    const wchar_t* wstr1 = L"a1";
    const wchar_t* wstr2 = L"a2";
    const size_t numElements1 = wcslen(wstr1);
    const size_t numElements2 = wcslen(wstr2);
    const size_t numBytes1 = sizeof(wchar_t) * numElements1;
    const size_t numBytes2 = sizeof(wchar_t) * numElements2;
    int hash1 = 0;
    int hash2 = 0;

    wchar_t *name = NULL;
    size_t bytelen = 0;
    etch_id_name *id_name1 = NULL;
    etch_id_name *id_name2 = NULL;
    errno_t result = 0;

    /* create id_name1 */
    name = malloc(numBytes1+2);
    CU_ASSERT_PTR_NOT_NULL(name);

    result = wcscpy_s(name, numElements1+1, wstr1);
    CU_ASSERT_EQUAL(result, 0);
    id_name1 = id_name_1(name, numBytes1, NULL);

    result = wcscpy_s(name, numElements1+1, wstr1);
    CU_ASSERT_EQUAL(result, 0);
    id_name2 = id_name_1(name, numBytes1, NULL);

    hash1 = id_name_hash(id_name1);
    hash2 = id_name_hash(id_name2);

    CU_ASSERT_EQUAL(hash1, hash2);

    if (name != NULL)
        free(name);

    name = malloc(numBytes2+2);
    CU_ASSERT_PTR_NOT_NULL(name);

    result = wcscpy_s(name, numElements2+1, wstr2);
    CU_ASSERT_EQUAL(result, 0);
    id_name2 = id_name_1(name, numBytes2, NULL);

    hash2 = id_name_hash(id_name2);

    CU_ASSERT_NOT_EQUAL(hash1, hash2);
}


void test_all(void)
{
    /* When _UNICODE is defined _tmain expands to wmain, otherwise main 
     * Note that we are creating a copy of the string below on the heap
     * in order to verify the wide character functionality, along with
     * testing the etch_id_name object.
     */
    char c=0;
    int  testresult = FALSE;
    errno_t result1 = 0, result2 = 0;
    unsigned hash1  = 0, hash2   = 0;
    /* In this test, we test the etch_id_name constructors used both with 
     * new objects (heap), and pre-existing objects (stack). Obviously we must
     * keep track of which of the objects requires a destroy() call.
     */
    etch_id_name static_id_name;   

    etch_hashtable* myhashtable = NULL;    /* define a hashtable */
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
    etch_id_name *idname1 = NULL, *idname2 = NULL;

    /* verify wcscpy_s: allocate new keys on heap and copy into them keys from stack.  */
    /* This illustrates some important considerations niot obvious from the MSDN docs. */
    /* wcscpy_s parameter 2 is documented as "numberOfElements: size of the buffer".   */
    /* Trial and error shows that we must allocate a (wcslen(s)+1)*2) byte buffer, and */
    /* specify the wcscpy_s numberOfElements parameter as wcslen(s)+1. The MSDN docs   */
    /* don't explain this because the examples are all templated C++ in which there is */
    /* no length parameter to wcscpy_s(). */
    key1 = malloc(numBytes1 + 2);  /* NOTE 1: wchar_t buffer size will be */
    key2 = malloc(numBytes2 + 2);  /* (number of characters + 1) * 2      */
    CU_ASSERT_PTR_NOT_NULL(key1);
    CU_ASSERT_PTR_NOT_NULL(key2);
        
    result1 = wcscpy_s(key1, numElements1+1, wstr1);  /* NOTE 2: wcscpy_s param 2 is */
    result2 = wcscpy_s(key2, numElements1+1, wstr2);  /* number of characters + 1    */
    CU_ASSERT_EQUAL(result1, 0);
    CU_ASSERT_EQUAL(result2, 0);

    /* create an etch_id_name object for each of the keys */
    idname1 = id_name_1(key1, (int)numBytes1, NULL);
    idname2 = id_name_2(ID, key2, (int)numBytes2, &static_id_name);
    CU_ASSERT_PTR_NOT_NULL(idname1);
    CU_ASSERT_PTR_NOT_NULL(idname2);

    /* do an ad hoc hash of the two keys and ensure that the hashkeys match */
    hash1 = id_name_hashfunc(key1, numBytes1);
    hash2 = id_name_hashfunc(key2, numBytes2);
    CU_ASSERT_NOT_EQUAL(hash1, 0);
    CU_ASSERT_NOT_EQUAL(hash2, 0);

    myhashtable = new_hashtable(16);  /* create a hash table */
    CU_ASSERT_PTR_NOT_NULL(myhashtable);

    /* insert the two keys into the hashtable allowing the hashtable to hash the keys */
    myhashtable->vtab->insert(myhashtable->realtable, key1, (int)numBytes1, NULL, 0, 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(myhashtable->realtable, 0, 0), 1);

    myhashtable->vtab->insert(myhashtable->realtable, key2, (int)numBytes2, NULL, 0, 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(myhashtable->realtable, 0, 0), 2);


    /* do a hashtable lookup on our ad-hoc hashed keys, which should be identical */
    /* to the hashkeys created by the hashtable */
    if (0 == myhashtable->vtab->findh(myhashtable->realtable, hash1, NULL, &myentry))
        foundkey1 = (wchar_t*)myentry->key;    
    CU_ASSERT_PTR_NOT_NULL(foundkey1);

    if (0 == myhashtable->vtab->findh(myhashtable->realtable, hash2, NULL, &myentry))
        foundkey2 = (wchar_t*)myentry->key;
    CU_ASSERT_PTR_NOT_NULL(foundkey2);

    CU_ASSERT_EQUAL(0, memcmp(foundkey1, key1, numBytes1));
    CU_ASSERT_EQUAL(0, memcmp(foundkey2, key2, numBytes2));

    if (key1) 
        free(key1); 
    if (key2) 
        free(key2);  
    id_name_destroy(idname1);       /* this object is on the heap */
    /* id_name_destroy(idname2); */ /* this object is on the stack */

    /* destroy hashtable *without* freeing its contents */
    destroy_hashtable(myhashtable, FALSE, FALSE);  

    debug_showmem(TRUE, TRUE);
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
    CU_set_output_filename("../test_id_name");

    /* add a suite to the registry */
    pSuite = CU_add_suite("suite_id_name", init_suite_id_name, clean_suite_id_name);
    if (NULL == pSuite) 
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (CU_add_test(pSuite, "id_name_1", test_id_name_1) == NULL)   
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuite, "id_name_2", test_id_name_2) == NULL)   
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuite, "id_name_destroy", test_id_name_destroy) == NULL)   
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuite, "id_name_hashfunc", test_id_name_hashfunc) == NULL)   
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuite, "id_name_hash", test_id_name_hash) == NULL)   
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
