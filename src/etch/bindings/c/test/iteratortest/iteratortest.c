/**
 * iteratortest.c -- test etch_iterator over i_iterable classes
 */

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etch_global.h"
#include "etch_objects.h"
#include "etch_arraylist.h"
#include "etchhash.h"


int suite_init(void)    /* pre-suite init */
{
    return etch_init();
}

int suite_cleanup(void) /* post-suite cleanup */
{
   return etch_runtime_cleanup(0,0); /* free memtable and cache etc */
}


#define IS_DEBUG_CONSOLE FALSE  /* TRUE to display diagnostics */

etch_arraylist* testlist;
etch_hashtable* testhash;
int bytes_allocated;


/* 
 * load_listdata_int()
 * load testlist array with some ETCH_INT objects
 */
int load_listdata_int()
{
    int i = 0, numitems = 4;
    ETCH_INT32* newobj = NULL;
    int ints[4] = { 1, 2, 3, 4 };

    for(; i < numitems; i++)
    {
        newobj = new_etch_int32(ints[i]);
        arraylist_add(testlist, newobj);
    }

    return numitems;
}


/* 
 * new_listdata()
 * create testlist array and load it up with data objects
 */
int new_listdata(const int datatype)
{
    int count = 0;
    testlist = new_arraylist(0,0);  
    testlist->content_type = ETCHARRAYLIST_CONTENT_OBJECT;
    count = load_listdata_int(); 
    return count;
}


/* 
 * destroy_listdata()
 * destroy testlist array and content
 */
void destroy_listdata()
{
    arraylist_destroy(testlist, TRUE);
}


/* 
 * load_hashdata_string()
 * load testhash hashtable with some ETCH_STRING objects
 */
int load_hashdata_string()
{
    int i = 0, numitems = 4;
    wchar_t* testval = NULL;
    void*    value = NULL;
    ETCH_STRING* newobj = NULL;
    wchar_t* str0 = L"now ", *str1 = L"is  ", *str2 = L"the ", *str3 = L"time";
    wchar_t* strings[4] = { str0, str1, str2, str3 };
    const size_t bytelen = (wcslen(str0) + 1) * sizeof(wchar_t);

    for(; i < numitems; i++)
    {
        testval = etch_malloc(bytelen, 0);
        memcpy(testval, strings[i], bytelen);
        newobj = new_etch_string(testval); 

        value  = etch_malloc(bytelen, 0);
        memcpy(value, strings[i], bytelen);

        testhash->vtab->insert(testhash->realtable, newobj, (int)bytelen, value, (int)bytelen, NULL, NULL);
    }

    return numitems;
}


/* 
 * new_hashdata()
 * create testhash hashtable and load it up with data objects
 */
int new_hashdata(const int datatype)
{
    int count = 0;
    testhash = new_hashtable(16);  
    count = load_hashdata_string();
    return count;
}


/* 
 * destroy_hashdata()
 * destroy testhash hashtable and content
 */
void destroy_hashdata()
{
    destroy_hashtable(testhash, TRUE, TRUE);
}


/* 
 * test_iterator_over_arraylist
 */
void test_iterator_over_arraylist(void)
{
    etch_iterator* iterator = NULL; 
    int testcount = 0, thiscount = 0;
    testcount = new_listdata(0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(testlist);
    CU_ASSERT_NOT_EQUAL(testlist->count, 0);

    iterator = new_iterator(testlist, &testlist->iterable);

    CU_ASSERT_PTR_NOT_NULL_FATAL(iterator);
    CU_ASSERT_EQUAL_FATAL(iterator->ordinal,1);
    thiscount = 1;

    while(iterator->vtab->has_next(iterator))
          thiscount += (iterator->vtab->next(iterator) == 0);  
        
    CU_ASSERT_EQUAL(testcount, thiscount);

    etch_free(iterator);
    destroy_listdata();

    bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  /* verify all memory freed */
    CU_ASSERT_EQUAL(bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */ 
}


/* 
 * test_iterator_over_hashtable
 */
void test_iterator_over_hashtable(void)
{
    etch_iterator* iterator = NULL; 
    int result = 0, thiscount = 0, testcount = 0, startbytes = 0;
    testcount = new_hashdata(1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(testhash);
    CU_ASSERT_NOT_EQUAL(testhash->vtab->count(testhash->realtable,0,0),0);
    startbytes = etch_showmem(0, FALSE); /* note testdata bytes */

    iterator = new_iterator(testhash, &testhash->iterable);

    CU_ASSERT_PTR_NOT_NULL_FATAL(iterator);
    CU_ASSERT_PTR_NOT_NULL_FATAL(iterator->current_key);
    CU_ASSERT_EQUAL_FATAL(iterator->ordinal,1);
    thiscount = 1;

    while(iterator->vtab->has_next(iterator))
    {
        if (0 != (result = iterator->vtab->next(iterator))) break;  
         
        thiscount++;
        CU_ASSERT_PTR_NOT_NULL(iterator->current_key);
        CU_ASSERT_PTR_NOT_NULL(iterator->current_value);

        #if IS_DEBUG_CONSOLE       
        if  (iterator->current_value)
             wprintf(L"value is %s\n",(wchar_t*)iterator->current_value);
        else wprintf(L"value is null\n");           
        #endif       
    } 
        
    CU_ASSERT_EQUAL(testcount, thiscount);

    etch_free(iterator);
    destroy_hashdata();

    /* we are etch_malloc'ing testdata but hashtable frees content, not etch_free */
    bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE);  
    bytes_allocated -= startbytes;
    CU_ASSERT_TRUE(bytes_allocated <= 0);  
    memtable_clear();  /* start fresh for next test */ 
}



/**
 * main   
 */
int _tmain(int argc, _TCHAR* argv[])
{    
    CU_pSuite pSuite = 0; char c=0; 
    if (CUE_SUCCESS != CU_initialize_registry()) return -1;
    pSuite = CU_add_suite("suite_iterator", suite_init, suite_cleanup);
    CU_set_output_filename("../test_iterator");

    CU_add_test(pSuite, "test forward iterator over arraylist",   test_iterator_over_arraylist);
    CU_add_test(pSuite, "test forward iterator over hashtable",   test_iterator_over_hashtable);

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

