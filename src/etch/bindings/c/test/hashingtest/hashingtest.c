/**
 * hashingtest.c 
 * test hashing of various complex items and associated memory management 
 */

#include <stdio.h>
#include <conio.h>
#include "cunit.h"
#include "basic.h"
#include "automated.h"
#include "etch_global.h"
#include "id_name.h"
#include "field.h"
#include "type.h"

#define IS_DEBUG_CONSOLE FALSE

int suite_init(void)
{
    return etch_init();
}

int suite_cleanup(void)
{
    return etch_runtime_cleanup(0,0);
}


/**
 * destroy_content_id_name()
 * passed a content pointer out of the hashtable, interprets pointer as etch_id_name,
 * frees the name string if requested, and frees the etch_id_name shell.
 */
int destroy_content_id_name(void* content, const int is_free_name)
{
    etch_id_name* idname = (etch_id_name*) content;
    if (idname == NULL) return -1;
    id_name_destroy(idname, is_free_name);
    return 0;
}


/**
 * destroy_content_field()
 * passed a content pointer out of the hashtable, interprets pointer as etch_field,
 * frees the name string if requested, and frees the etch_field shell.
 * the field code is all typdef'ed and #defined to be the same as etch_id_name,
 * so this will call the same code as destroy_content_id_name.
 */
int destroy_content_field(void* content, const int is_free_name)
{
    etch_field* field = (etch_field*) content;
    if (field == NULL) return -1;
    field_destroy(field, is_free_name);
    return 0;
}


/**
 * destroy_content_type()
 * passed a content pointer out of the hashtable, interprets pointer as etch_type,
 * frees the name string if requested, and frees the etch_type shell.
 * the field code is all typdef'ed and #defined to be the same as etch_id_name,
 * so this will call the same code as destroy_content_id_name.
 */
int destroy_content_type(void* content, const int is_free_name)
{
    etch_type* type = (etch_type*) content;
    if (type == NULL) return -1;
    type_destroy(type, is_free_name);
    return 0;
}



/**
 * test_idname_as_key
 * tests that we can use etch_id_name (and etch_field and etch_type, since they
 * are etch_id_name typedefs), as hashkeys, and subsequently acccess the original
 * objects, individually clean up the keys, and ask hashtable to clean up the values.
 */
void test_idname_as_key_hashclean_values(void)
{
    etch_field*   my_field  = NULL;
    etch_type*    my_type   = NULL;
    etch_id_name* my_idname = NULL;

    const int id_name_size = sizeof(etch_id_name);
    const int field_size   = sizeof(etch_field);
    const int type_size    = sizeof(etch_type);

    etch_hashtable* myhashtab = NULL;  
    etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket; 

    wchar_t* name1 = L"name number one";  
    wchar_t* name2 = L"nombre numero dos";
    wchar_t* name3 = L"le troisieme nom";

    const size_t numElements1 = wcslen(name1);
    const size_t numElements2 = wcslen(name2);
    const size_t numElements3 = wcslen(name3);

    const size_t numBytes1 = sizeof(wchar_t) * numElements1;
    const size_t numBytes2 = sizeof(wchar_t) * numElements2;
    const size_t numBytes3 = sizeof(wchar_t) * numElements3;
    void* value1 = NULL, *value2 = NULL, *value3 = NULL;

    size_t actlen1 = 0, actlen2 = 0, actlen3 = 0;
    wchar_t *pname1 = NULL, *pname2 = NULL, *pname3 = NULL;
    int result = 0, result1 = 0, result2 = 0, result3 = 0;
    unsigned bytes_allocated = 0;

    value1 = etch_malloc(numBytes1 + 2, 0);   
    value2 = etch_malloc(numBytes2 + 2, 0);  
    value3 = etch_malloc(numBytes3 + 2, 0);  
    wcscpy_s(value1, numElements1+1, name1);   
    wcscpy_s(value2, numElements2+1, name2);   
    wcscpy_s(value3, numElements3+1, name3);   

    pname1 = etch_malloc(numBytes1 + 2, 0);   
    pname2 = etch_malloc(numBytes2 + 2, 0);  
    pname3 = etch_malloc(numBytes3 + 2, 0);  

    result1 = wcscpy_s(pname1, numElements1+1, name1);   
    result2 = wcscpy_s(pname2, numElements2+1, name2);   
    result3 = wcscpy_s(pname3, numElements3+1, name3);   

    actlen1 = wcslen(pname1);  actlen2 = wcslen(pname2); actlen3 = wcslen(pname3);

    myhashtab = new_hashtable(16);  
    myhashtab->is_readonly_keys   = TRUE;  /* keys will NOT be freed on destroy */
    myhashtab->is_readonly_values = FALSE; /* values WILL be freed on destroy */
    myhashtab->is_tracked_memory  = TRUE;  /* hashtable will use etch_free */
    /* TODO since the id_name's are not objects, they will not be cleaned up */
    /* we will probably need a custom method to iterate the hashtable and clean up */
    /* or implement a per-item callback on destroy  */

    my_idname = new_id_name_1(pname1, actlen1, NULL);
    my_field  = new_field_1  (pname2, actlen2, NULL);
    my_type   = new_type_1   (pname3, actlen3, NULL);

    CU_ASSERT_PTR_NOT_NULL_FATAL(my_idname);
    CU_ASSERT_PTR_NOT_NULL_FATAL(my_field);
    CU_ASSERT_PTR_NOT_NULL_FATAL(my_type);

    /* ensure constructors populated the id (hash of name) */
    CU_ASSERT_NOT_EQUAL_FATAL(my_idname->id,0);
    CU_ASSERT_NOT_EQUAL_FATAL(my_field->id,0);
    CU_ASSERT_NOT_EQUAL_FATAL(my_type->id,0);

    result = myhashtab->vtab->insert(myhashtab->realtable, my_idname, id_name_size, value1, 0, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = myhashtab->vtab->insert(myhashtab->realtable, my_field,  field_size, value2, 0, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = myhashtab->vtab->insert(myhashtab->realtable, my_type,  type_size, value3, 0, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = myhashtab->vtab->find(myhashtab->realtable, my_idname, id_name_size, NULL, &myentry);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_PTR_NOT_NULL(myentry->key);
    CU_ASSERT_PTR_NOT_NULL(myentry->value);
    result = destroy_content_id_name(myentry->key, TRUE);
    CU_ASSERT_EQUAL(result,0);    

    result = myhashtab->vtab->find(myhashtab->realtable, my_field, field_size, NULL, &myentry);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_PTR_NOT_NULL(myentry->key);
    CU_ASSERT_PTR_NOT_NULL(myentry->value);
    result = destroy_content_field(myentry->key, TRUE);
    CU_ASSERT_EQUAL(result,0);   

    result = myhashtab->vtab->find(myhashtab->realtable, my_type, type_size, NULL, &myentry);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_PTR_NOT_NULL(myentry->key);
    CU_ASSERT_PTR_NOT_NULL(myentry->value);
    result = destroy_content_type(myentry->key, TRUE);
    CU_ASSERT_EQUAL(result,0);  

    /** above we looked up each item and freed memory for the id_name, field, and type keys,
     * but not for their values. so here when we destroy the hashtable we will specify that
     * the hashtable should free memory for the value content, but not for the key content.
     * when we're done, there should be zero memory still allocated.
     */
    destroy_hashtable(myhashtab, FALSE, TRUE);

    bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_idname_as_key
 * tests that we can use etch_id_name (and etch_field and etch_type, since they
 * are etch_id_name typedefs), as hashkeys, and subsequently acccess the original
 * objects and clean up both the keys and values.
 */
void test_idname_as_key_hashclean_none(void)
{
    etch_field*   my_field  = NULL;
    etch_type*    my_type   = NULL;
    etch_id_name* my_idname = NULL;

    const int id_name_size = sizeof(etch_id_name);
    const int field_size   = sizeof(etch_field);
    const int type_size   = sizeof(etch_type);

    etch_hashtable* myhashtab = NULL;  
    etch_hashitem   hashbucket;
    etch_hashitem*  myentry = &hashbucket; 

    wchar_t* name1 = L"name number one";  
    wchar_t* name2 = L"nombre numero dos";
    wchar_t* name3 = L"le troisieme nom";

    const size_t numElements1 = wcslen(name1);
    const size_t numElements2 = wcslen(name2);
    const size_t numElements3 = wcslen(name3);

    const size_t numBytes1 = sizeof(wchar_t) * numElements1;
    const size_t numBytes2 = sizeof(wchar_t) * numElements2;
    const size_t numBytes3 = sizeof(wchar_t) * numElements3;
    void* value1 = NULL, *value2 = NULL, *value3 = NULL;

    size_t actlen1 = 0, actlen2 = 0, actlen3 = 0;
    int result = 0, result1 = 0, result2 = 0, result3 = 0;
    unsigned bytes_allocated = 0;

    value1 = etch_malloc(numBytes1 + 2, 0);   
    value2 = etch_malloc(numBytes2 + 2, 0);  
    value3 = etch_malloc(numBytes3 + 2, 0);  
    wcscpy_s(value1, numElements1+1, name1);   
    wcscpy_s(value2, numElements2+1, name2);   
    wcscpy_s(value3, numElements3+1, name3);   

    actlen1 = wcslen(name1);  actlen2 = wcslen(name2); actlen3 = wcslen(name3);

    myhashtab = new_hashtable(16);  
    myhashtab->is_readonly_keys   = TRUE;  /* keys will be freed on destroy  */
    myhashtab->is_readonly_values = TRUE; /* values will be freed on destroy */
    myhashtab->is_tracked_memory  = TRUE; /* hashtable will etch_free content */

    /* here we are creating id_name, field, and type objects using names allocated
     * on out local stack. therefore the cleanup parameters must be set to not free 
     * memory for the name part, or we will crash. note that name is part of the 
     * key structs, and is also the value of the hashed key/value pair, however 
     * we etch_malloc'ed memory for the values and copied our stack strings into it,
     * so we will want to free the value memory. we could ask the hashtable to do
     * so but we'll do it manually here instead.
     */
    my_idname = new_id_name_1(name1, actlen1, NULL);
    my_field  = new_field_1  (name2, actlen2, NULL);
    my_type   = new_type_1   (name3, actlen3, NULL);

    CU_ASSERT_PTR_NOT_NULL_FATAL(my_idname);
    CU_ASSERT_PTR_NOT_NULL_FATAL(my_field);
    CU_ASSERT_PTR_NOT_NULL_FATAL(my_type);

    /* ensure constructors populated the id (hash of name) */
    CU_ASSERT_NOT_EQUAL_FATAL(my_idname->id,0);
    CU_ASSERT_NOT_EQUAL_FATAL(my_field->id,0);
    CU_ASSERT_NOT_EQUAL_FATAL(my_type->id,0);

    result = myhashtab->vtab->insert(myhashtab->realtable, my_idname, id_name_size, value1, 0, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = myhashtab->vtab->insert(myhashtab->realtable, my_field,  field_size, value2, 0, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = myhashtab->vtab->insert(myhashtab->realtable, my_type,  type_size, value3, 0, 0, 0);
    CU_ASSERT_EQUAL_FATAL(result,0);

    result = myhashtab->vtab->find(myhashtab->realtable, my_idname, id_name_size, NULL, &myentry);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_PTR_NOT_NULL(myentry->key);
    CU_ASSERT_PTR_NOT_NULL(myentry->value);
    result = destroy_content_id_name(myentry->key, FALSE); /* free shell but not name */
    CU_ASSERT_EQUAL(result,0);  
    etch_free(myentry->value); 

    result = myhashtab->vtab->find(myhashtab->realtable, my_field, field_size, NULL, &myentry);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_PTR_NOT_NULL(myentry->key);
    CU_ASSERT_PTR_NOT_NULL(myentry->value);
    result = destroy_content_field(myentry->key, FALSE); /* free shell but not name */
    CU_ASSERT_EQUAL(result,0); 
    etch_free(myentry->value);  

    result = myhashtab->vtab->find(myhashtab->realtable, my_type, type_size, NULL, &myentry);
    CU_ASSERT_EQUAL(result,0);
    CU_ASSERT_PTR_NOT_NULL(myentry->key);
    CU_ASSERT_PTR_NOT_NULL(myentry->value);
    result = destroy_content_type(myentry->key, FALSE); /* free shell but not name */
    CU_ASSERT_EQUAL(result,0); 
    etch_free(myentry->value);  

    destroy_hashtable(myhashtab, FALSE, FALSE);  /* tell hashtable to not free any content */

    bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */ 
}



int _tmain(int argc, _TCHAR* argv[])
{
    CU_pSuite pSuite = 0; char c=0; 
    if (CUE_SUCCESS != CU_initialize_registry()) return -1;
    pSuite = CU_add_suite("suite_arrayvalue", suite_init, suite_cleanup);
    CU_set_output_filename("../test_hashing");

    CU_add_test(pSuite, "test hash id_name etc - auto-cleanup values", test_idname_as_key_hashclean_values);
    CU_add_test(pSuite, "test hash id_name etc - no auto-cleanup", test_idname_as_key_hashclean_none);

    if (argc > 1 && !wcscmp(argv[1], L"-a"))
        CU_automated_run_tests();
    else
    {   CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    }

    printf("any key ..."); while(!c) c = _getch(); wprintf(L"\n"); 
    CU_cleanup_registry();
    return CU_get_error(); 
}

