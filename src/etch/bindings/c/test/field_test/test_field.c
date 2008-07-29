/**
 * test_field.c 
 * Tests the C implementation of the etch_field object.
 * This is the same as the id_name test, but using the etch_field typedef.
 */

#include <stdio.h>
#include "cunit.h"
#include "basic.h"
#include "automated.h"
#include "field.h"
#include "etchhash.h"
#include "etchmem.h"

#define ID 987654321 /* pre-defined field ID */

/**
 * init_suite_test
 * Initialization tasks for the test suite
 */
int init_suite_field(void)
{
    return 0;
}

/**
 * clean_suite_test
 * Clean up tasks for the test suite
 */
int clean_suite_field(void)
{
    return 0;
}

/**
 * test_field_ctor1
 * Unit test field_ctor1
 */
void test_field_ctor1(void)
{
    const wchar_t* wstr = L"abracadabra";
    const size_t numElements = wcslen(wstr);
    const size_t numBytes = sizeof(wchar_t) * numElements;

    wchar_t *name = NULL;
    size_t bytelen = 0;
    etch_field *field = NULL;
    errno_t result = 0;

    /* name is NULL */
    field = new_field_1(name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(field);

    /* assign name */
    name = malloc(numBytes+2);
    CU_ASSERT_PTR_NOT_NULL(name);

    result = wcscpy_s(name, numElements+1, wstr);
    CU_ASSERT_EQUAL(result, 0);

    /* bytelen < 0 */
    bytelen = -1;
    field = new_field_1(name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(field);
    
    /* bytelen > max bytelen */
    bytelen = MAX_ETCHHASHKEY*sizeof(wchar_t)+1;
    field = new_field_1(name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(field);

    /* positive test with correct arguments */
    bytelen = numBytes;
    field = new_field_1(name, bytelen, NULL);

    /* input name and field name should be the same */
    CU_ASSERT_PTR_EQUAL(field->name, name);

    /* name byte length should be the same as input byte length */
    CU_ASSERT_EQUAL(field->namebytelen, bytelen);

    /* no id, so generated is is the hash based on name and bytelength */
    CU_ASSERT_EQUAL(field->id, id_name_hashfunc(name, bytelen));

    /* destry the field */
    field_destroy(field);

    /* clean up */
    if (name != NULL)
        free(name);
}

/**
 * test_field_ctor2
 * Unit test field_ctor2 
 * TODO this test is wrong. Need to test constructor 2, not 1.
 */
void test_field_ctor2(void)
{
    const wchar_t* wstr = L"abracadabra";
    const size_t numElements = wcslen(wstr);
    const size_t numBytes = sizeof(wchar_t) * numElements;

    wchar_t *name = NULL;
    size_t bytelen = 0;
    etch_field *field = NULL;
    errno_t result = 0;

    /* name is NULL */
    field = new_field_1(name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(field);

    /* assign name */
    name = malloc(numBytes+2);
    CU_ASSERT_PTR_NOT_NULL(name);

    result = wcscpy_s(name, numElements+1, wstr);
    CU_ASSERT_EQUAL(result, 0);

    /* bytelen < 0 */
    bytelen = -1;
    field = new_field_1(ID, name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(field);
    
    /* bytelen > max bytelen */
    bytelen = MAX_ETCHHASHKEY*sizeof(wchar_t)+1;
    field = new_field_1(ID, name, bytelen, NULL);
    CU_ASSERT_PTR_NULL(field);

    /* positive test with correct arguments */
    bytelen = numBytes;
    field = new_field_1(ID, name, bytelen, NULL);

    /* input name and field name should be the same */
    CU_ASSERT_PTR_EQUAL(field->name, name);

    /* name byte length should be the same as input byte length */
    CU_ASSERT_EQUAL(field->namebytelen, bytelen);

    /* no id, so generated is is the hash based on name and bytelength */
    CU_ASSERT_EQUAL(field->id, ID);

    /* destry the field */
    field_destroy(field);

    /* clean up */
    if (name != NULL)
        free(name);
}

/**
 * test_field_destroy
 * Unit test field_destroy
 */
void test_field_destroy(void)
{
    const wchar_t* wstr = L"abracadabra";
    const size_t numElements = wcslen(wstr);
    const size_t numBytes = sizeof(wchar_t) * numElements;

    wchar_t *name = NULL;
    size_t bytelen = 0;
    etch_field *field = NULL;
    errno_t result = 0;

    /* create field */
    name = malloc(numBytes+2);
    CU_ASSERT_PTR_NOT_NULL(name);

    result = wcscpy_s(name, numElements+1, wstr);
    CU_ASSERT_EQUAL(result, 0);

    bytelen = numBytes;
    field = field_new_field_1(name, bytelen, NULL);

    CU_ASSERT_PTR_EQUAL(field->name, name);

    /* name byte length should be the same as input byte length */
    CU_ASSERT_EQUAL(field->namebytelen, bytelen);

    /* no id, so generated is is the hash based on name and bytelength */
    CU_ASSERT_EQUAL(field->id, id_name_hashfunc(name, bytelen));

    /* destry the field */
    field_destroy(field);

    if (field != NULL)
    {
        /* old value should be gone */
        CU_ASSERT_NOT_EQUAL(field->namebytelen, bytelen);

        /* old value should be gone */
        CU_ASSERT_NOT_EQUAL(field->id, id_name_hashfunc(name, bytelen));
    }

    if (name != NULL)
        free(name);
}

/**
 * test_field_hashfunc
 * Unit test field_hashfunc
 */
void test_field_hashfunc(void)
{
    const wchar_t* wstr1 = L"a1";
    const wchar_t* wstr2 = L"a2";
    const size_t numElements1 = wcslen(wstr1);
    const size_t numElements2 = wcslen(wstr2);
    const size_t numBytes1 = sizeof(wchar_t) * numElements1;
    const size_t numBytes2 = sizeof(wchar_t) * numElements2;
    int hash1 = 0;
    int hash2 = 0;

    hash1 = field_hashfunc(wstr1, numBytes1);
    hash2 = field_hashfunc(wstr1, numBytes1);

    CU_ASSERT_EQUAL(hash1, hash2);

    hash2 = field_hashfunc(wstr2, numBytes2);

    CU_ASSERT_NOT_EQUAL(hash1, hash2);
}

/**
 * test_all
 * Unit test for all field features
 */
void test_all(void)
{
    char c=0;
    int  testresult = FALSE;
    errno_t result1 = 0, result2 = 0;
    unsigned hash1  = 0, hash2   = 0;
    etch_field static_field;

    etch_hashtable* myhash = NULL;    /* define a hashtable */
    etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket;

    wchar_t* wstr1 = L"abracadabra";  /* is L macro Windows only? */
    wchar_t* wstr2 = L"gilgamesh";

    const size_t numElements1 = wcslen(wstr1);
    const size_t numElements2 = wcslen(wstr2);

    const size_t numBytes1 = sizeof(wchar_t) * numElements1;
    const size_t numBytes2 = sizeof(wchar_t) * numElements2;

    wchar_t *key1 = NULL, *key2 = NULL;
    wchar_t *foundkey1 = NULL,  *foundkey2 = NULL; 
    etch_field *field1 = NULL, *field2 = NULL;

    /* verify wcscpy_s: allocate new keys on heap and copy into them keys from stack.  
     * This illustrates some important considerations not obvious from the MSDN docs. 
     * wcscpy_s parameter 2 is documented as "numberOfElements: size of the buffer".   
     * Trial and error shows that we must allocate a (wcslen(s)+1)*2) byte buffer, and 
     * specify the wcscpy_s numberOfElements parameter as wcslen(s)+1. The MSDN docs   
     * don't explain this because the examples are all templated C++ in which there is 
     * no length parameter to wcscpy_s(). */
    key1 = malloc(numBytes1 + 2);  /* NOTE 1: wchar_t buffer size will be */
    key2 = malloc(numBytes2 + 2);  /* (number of characters + 1) * 2      */
    CU_ASSERT_PTR_NOT_NULL(key1);
    CU_ASSERT_PTR_NOT_NULL(key2);

    result1 = wcscpy_s(key1, numElements1+1, wstr1);  /* NOTE 2: wcscpy_s param 2 is */
    result2 = wcscpy_s(key2, numElements1+1, wstr2);  /* number of characters + 1    */
    CU_ASSERT_EQUAL(result1, 0);
    CU_ASSERT_EQUAL(result2, 0);

    /* create an etch_id_name object for each of the keys */
    field1 = new_field_1(key1, (int)numBytes1, NULL);
    field2 = new_field_2(ID, key2, (int)numBytes2, &static_field);
    CU_ASSERT_PTR_NOT_NULL(field1);
    CU_ASSERT_PTR_NOT_NULL(field2);
    
    /* do an ad hoc hash of the two keys and ensure that the hashkeys match */
    hash1 = field_hashfunc(key1, numBytes1);
    hash2 = field_hashfunc(key2, numBytes2);
    CU_ASSERT_NOT_EQUAL(hash1, 0);
    CU_ASSERT_NOT_EQUAL(hash2, 0);
    
    myhash = new_hashtable(16);  /* create a hash table */
    CU_ASSERT_PTR_NOT_NULL(myhash);

    /* insert the two keys into the hashtable allowing the hashtable to hash the keys */
    myhash->vtab->insert(myhash->realtable, key1, (int)numBytes1, NULL, 0, 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(myhash->realtable, 0, 0), 1);

    myhash->vtab->insert(myhash->realtable, key2, (int)numBytes2, NULL, 0, 0, 0);
    CU_ASSERT_EQUAL(jenkins_count(myhash->realtable, 0, 0), 2);


    /* do a hashtable lookup on our ad-hoc hashed keys, which should be identical  
     * to the hashkeys created by the hashtable */
    if (0 == myhash->vtab->findh(myhash->realtable, hash1, NULL, &myentry))
        foundkey1 = (wchar_t*)myentry->key;    
    CU_ASSERT_PTR_NOT_NULL(foundkey1);

    if (0 == myhash->vtab->findh(myhash->realtable, hash2, NULL, &myentry))
        foundkey2 = (wchar_t*)myentry->key;
    CU_ASSERT_PTR_NOT_NULL(foundkey2);

    CU_ASSERT_EQUAL(0, memcmp(foundkey1, key1, numBytes1));
    CU_ASSERT_EQUAL(0, memcmp(foundkey2, key2, numBytes2));
 
    if (key1) 
        free(key1); 
    if (key2) 
        free(key2);  

    field_destroy(field1);
                 
    /* destroy hashtable *without* freeing its contents */
    destroy_hashtable(myhash, FALSE, FALSE);  

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
    CU_set_output_filename("../test_field");

    /* add a suite to the registry */
    pSuite = CU_add_suite("suite_field", init_suite_field, clean_suite_field);
    if (NULL == pSuite) 
    {
        CU_cleanup_registry();
        return CU_get_error();
    }

    /* add the tests to the suite */
    if (CU_add_test(pSuite, "new_field_1", test_field_ctor1) == NULL)   
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuite, "new_field_2", test_field_ctor2) == NULL)   
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuite, "field_destroy", test_field_destroy) == NULL)   
    {
        CU_cleanup_registry();
        return CU_get_error();
    }
    if (CU_add_test(pSuite, "field_hashfunc", test_field_hashfunc) == NULL)   
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
