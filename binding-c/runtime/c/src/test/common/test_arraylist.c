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
 * test_arraylist.c -- test etch_arraylist
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
#include "etch_arraylist.h"

#define OBJSIG 0xbadf00d
#define NUMITEMS 3
#define ETCHTYPEA_TESTOBJ 0xff


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

int bytes_prior;


typedef struct TESTOBJ
{   int id;
    int signature;
} TESTOBJ;


TESTOBJ* objfactory(const int id)
{ 
   TESTOBJ* obj = etch_malloc(sizeof(TESTOBJ), ETCHTYPEA_TESTOBJ);
   obj->id = id; obj->signature = OBJSIG;
   return obj;
}


/**
 * comparator callback for contains(), indexof(), functions
 * typedef int (*etch_comparator) (void* myobj, void* otherobj);
 */
int al_comparator(void* p, void* q)
{   
    int result = 0;
    TESTOBJ* myobj = (TESTOBJ*)p, *othobj = (TESTOBJ*)q;
    if (!myobj || !othobj) result = -2;
    else if (myobj->signature != OBJSIG || othobj->signature != OBJSIG) result = -2;
    else if (myobj->id < othobj->id) result = -1;
    else if (myobj->id > othobj->id) result = 1;
    else result = 0; /* equality */
    return result;
}


/* 
 * test new_arraylist()
 */
void test_new_arraylist(void)
{
    const int TESTSIZE = 2048, TESTDELTA = 1024;

    etch_arraylist* list = new_arraylist(0, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(list);
    CU_ASSERT_PTR_NOT_NULL_FATAL(list->base);
    CU_ASSERT_EQUAL(list->count,0);
    CU_ASSERT_EQUAL(list->size,  ETCHARRAYLIST_DEFSIZE * sizeof(void**));
    CU_ASSERT_EQUAL(list->delta, ETCHARRAYLIST_DEFSIZE * sizeof(void**));

    arraylist_destroy(list,TRUE); /* free memory and ensure nothing remains allocated */
    g_bytes_allocated = etch_showmem(TRUE,FALSE);
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);

    list = new_arraylist(TESTSIZE, TESTDELTA);
    CU_ASSERT_EQUAL(list->count,0);
    CU_ASSERT_EQUAL(list->size,  TESTSIZE  * sizeof(void**));
    CU_ASSERT_EQUAL(list->delta, TESTDELTA * sizeof(void**));
    memset(list->base, 0xff, list->size); /* try write to entire buffer */

    arraylist_destroy(list,TRUE); /* free memory; ensure nothing remains allocated */
    g_bytes_allocated = etch_showmem(TRUE,FALSE);
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);
    memtable_clear(); /* start fresh for next test */
}


/* 
 * test arraylist_add()
 */
void test_add(void)
{
   TESTOBJ *x1, *x2;
   etch_arraylist* list = new_arraylist(8,0); 
   CU_ASSERT_EQUAL(list->base[0],NULL);
   CU_ASSERT_EQUAL(list->base[1],NULL);

   arraylist_add(list, x1 = objfactory(1));
   CU_ASSERT_EQUAL(list->count,1);
   CU_ASSERT_EQUAL(list->base[0], x1); /* ensure item's buffer slot OK */

   arraylist_add(list, x2 = objfactory(2));
   CU_ASSERT_EQUAL(list->count,2);
   CU_ASSERT_EQUAL(list->base[1], x2); /* ensure item's buffer slot OK */

   arraylist_destroy(list,TRUE); /* free memory including test objects */
   g_bytes_allocated = etch_showmem(TRUE,FALSE);
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
   memtable_clear(); /* start fresh for next test */
}


/* 
 * test arraylist_destroy()
 */
void test_destroy(void)
{
    CU_PASS("tested in all previous and subsequent tests"); 
}


/* 
 * test arraylist_remove()
 */
void test_remove_firstitem(void)
{
   TESTOBJ *x1, *x2;
   etch_arraylist* list = new_arraylist(8,0); 

   arraylist_add(list, x1 = objfactory(1));   /* allocate and add test obj x1 */
   arraylist_add(list, x2 = objfactory(2));   /* allocate and add test obj x2 */

   CU_ASSERT_EQUAL_FATAL(list->count,2);
   CU_ASSERT_EQUAL(list->base[0], x1);  
   CU_ASSERT_EQUAL(list->base[1], x2);  
   bytes_prior = etch_showmem(0,0);

   arraylist_remove(list,0,TRUE);    /* remove list[0] and free its x1 memory */

   CU_ASSERT_EQUAL_FATAL(list->count,1); 
   g_bytes_allocated = etch_showmem(0,0);  /* ensure TESTBOJ bytes were freed */
   CU_ASSERT_EQUAL(bytes_prior - g_bytes_allocated, sizeof(TESTOBJ));
   CU_ASSERT_EQUAL(list->base[1], NULL); /* ensure item's buffer slot cleared */

   CU_ASSERT_EQUAL_FATAL(list->base[0], x2);   /* ensure item 1 is now item 0 */
   
   arraylist_remove(list,0,FALSE); /* remove list[0] but don't free x2 memory */

   bytes_prior = etch_showmem(0,0);  
   CU_ASSERT_EQUAL(list->count,0); /* ensure no bytes were freed */
   CU_ASSERT_EQUAL(bytes_prior, g_bytes_allocated);
   CU_ASSERT_EQUAL(list->base[0], NULL); /* ensure item's buffer slot cleared */

   /* finally free the last test object, which would have been freed in the
    * prior arraylist_remove() had we not specified to not do so. Also if we
    * did not free it here it would be freed in the memtable_clear() below.
    */
   etch_free(x2);  

   arraylist_destroy(list,TRUE); /* destroy the arraylist */

   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
   memtable_clear();  /* start fresh for next test */   
}


/* 
 * test arraylist_remove()
 */
void test_remove_lastitem(void)
{
   TESTOBJ *x1, *x2, *x3;
   etch_arraylist* list = new_arraylist(8,0); 

   arraylist_add(list, x1 = objfactory(1));   /* allocate and add test obj x1 */
   arraylist_add(list, x2 = objfactory(2));   /* allocate and add test obj x2 */
   arraylist_add(list, x3 = objfactory(3));   /* allocate and add test obj x3 */
   CU_ASSERT_EQUAL_FATAL(list->count,3);

   CU_ASSERT_EQUAL(list->base[2], x3);

   arraylist_remove(list,2,TRUE);    /* remove list[2] and free its x1 memory */

   CU_ASSERT_EQUAL(list->base[2], NULL); /* ensure item's buffer slot cleared */

   CU_ASSERT_EQUAL_FATAL(list->base[0], x1);    
   CU_ASSERT_EQUAL_FATAL(list->base[1], x2); 
   
   /* destroy the arraylist freeing the test object content as well */
   arraylist_destroy(list,TRUE); 

   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
   memtable_clear();  /* start fresh for next test */   
}


void dumplist(etch_arraylist* list)
{
    int i=0; char c=0;
    const int n = list->count;
    printf("\ndumping list ...\n"); fflush(stdout);
    for(; i < n; i++)
    {   TESTOBJ* item = list->base[i];
        printf("\n list[%d] %d\n", i, item->id); fflush(stdout);
    }
    
    printf("\nany key ..."); while(!c) c = _getch(); printf("\n"); 
}


/* 
 * removetest_comparator()
 * comparator for test_remove() arraylist_indexof
 * note that the test value is passed as an int not an object
 */
int removetest_comparator(void* testvalue, void* listcontent)
{
    const int ivalue  = (int) (size_t) testvalue;
    TESTOBJ*  listobj = (TESTOBJ*) listcontent;
    int jvalue = listobj->id;
    return ivalue < jvalue? -1: ivalue > jvalue? 1: 0;
} 


/* 
 * test_remove()
 * remove items 
 */
void test_remove(void)
{
    const int numitems = 10;
    int i=0, curritems = numitems;
    etch_arraylist* list = new_arraylist(64,0); 

    for(; i < numitems; i++)        
        arraylist_add(list, objfactory(i));
        
    #if IS_DEBUG_CONSOLE
    dumplist(list);
    #endif

    for(i=2; i < numitems; i+=2)
    {
        /* remove the list content whose value is i */
        const int index = arraylist_indexof(list, (void*)(size_t) i, 0, removetest_comparator);
        CU_ASSERT_NOT_EQUAL(index, -1);

        if (index >= 0)
        {
            if (0 == arraylist_remove(list, index, TRUE))
                curritems--;
            
            #if IS_DEBUG_CONSOLE
            dumplist(list);
            #endif
        }
        else
        {
            #if IS_DEBUG_CONSOLE
            printf("could not remove list content %d\n", i);
            #endif
        }

        CU_ASSERT_EQUAL(list->count, curritems);
    }

    /* destroy the arraylist freeing the test object content as well */
    arraylist_destroy(list,TRUE); 

    g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/* 
 * test arraylist_insert()
 */
void test_insert_first(void)
{
   TESTOBJ *x1, *x2, *x3;
   etch_arraylist* list = new_arraylist(8,0); 

   arraylist_add(list, x2 = objfactory(2));   /* allocate and add test obj x2 */
   arraylist_add(list, x3 = objfactory(3));   /* allocate and add test obj x3 */

   /* insert test obj x1 into slot 0 */
   arraylist_insert(list, 0, x1 = objfactory(1)); 

   CU_ASSERT_EQUAL_FATAL(list->count,3);
   CU_ASSERT_EQUAL(list->base[0], x1);
   CU_ASSERT_EQUAL(list->base[1], x2);    
   CU_ASSERT_EQUAL(list->base[2], x3);   

   /* destroy the arraylist freeing the test object content as well */
   arraylist_destroy(list,TRUE); 

   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
   memtable_clear();  /* start fresh for next test */   
}


/* 
 * test arraylist_insert()
 */
void test_insert_mid(void)
{
   TESTOBJ *x1, *x2, *x3;
   etch_arraylist* list = new_arraylist(8,0); 

   arraylist_add(list, x1 = objfactory(1));    
   arraylist_add(list, x3 = objfactory(3));    

   /* insert test obj x1 into slot 1 */
   arraylist_insert(list, 1, x2 = objfactory(2)); 

   CU_ASSERT_EQUAL_FATAL(list->count,3);
   CU_ASSERT_EQUAL(list->base[0], x1);
   CU_ASSERT_EQUAL(list->base[1], x2);    
   CU_ASSERT_EQUAL(list->base[2], x3);   

   /* destroy the arraylist freeing the test object content as well */
   arraylist_destroy(list,TRUE); 

   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
   memtable_clear();  /* start fresh for next test */   
}


/* 
 * test arraylist_insert()
 */
void test_insert_end(void)
{
   TESTOBJ *x1, *x2, *x3;
   etch_arraylist* list = new_arraylist(8,0); 

   arraylist_add(list, x1 = objfactory(1));    
   arraylist_add(list, x2 = objfactory(2));   

   /* insert test obj x1 into slot 1 */
   arraylist_insert(list, 2, x3 = objfactory(3)); 

   CU_ASSERT_EQUAL_FATAL(list->count,3);
   CU_ASSERT_EQUAL(list->base[0], x1);
   CU_ASSERT_EQUAL(list->base[1], x2);    
   CU_ASSERT_EQUAL(list->base[2], x3);   

   /* destroy the arraylist freeing the test object content as well */
   arraylist_destroy(list,TRUE); 

   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
   memtable_clear();  /* start fresh for next test */   
}


/* 
 * test arraylist_containsp(), arraylist_contains()
 */
void test_contains(void)
{
   TESTOBJ *x1, *x2, *x3;
   int result = 0;
   etch_arraylist* list = new_arraylist(8,0); 

   arraylist_add(list, x1 = objfactory(1));    
   arraylist_add(list, x2 = objfactory(2));
   arraylist_add(list, x3 = objfactory(3)); 

   result = arraylist_containsp(list, x1, 0); 
   CU_ASSERT_EQUAL(result, TRUE); 

   result = arraylist_containsp(list, x2, 0); 
   CU_ASSERT_EQUAL(result, TRUE); 

   result = arraylist_containsp(list, x3, 0); 
   CU_ASSERT_EQUAL(result, TRUE); 

   result = arraylist_containsp(list, x1, 1); /* start at index 1 */ 
   CU_ASSERT_EQUAL(result, FALSE); 

   result = arraylist_contains(list, x1, 0, al_comparator);
   CU_ASSERT_EQUAL(result, TRUE); 

   result = arraylist_contains(list, x2, 0, al_comparator);
   CU_ASSERT_EQUAL(result, TRUE); 

   result = arraylist_contains(list, x3, 0, al_comparator);
   CU_ASSERT_EQUAL(result, TRUE); 

   result = arraylist_contains(list, x1, 2, al_comparator); /* start at index 2 */ 
   CU_ASSERT_EQUAL(result, FALSE); 

   /* destroy the arraylist freeing the test object content as well */
   arraylist_destroy(list,TRUE); 

   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
   memtable_clear();  /* start fresh for next test */   
}

/* 
 * test arraylist_indexofp(), arraylist_indexof()
 */
void test_indexof(void)
{
   TESTOBJ *x1, *x2, *x3;
   int result = 0;
   etch_arraylist* list = new_arraylist(8,0); 
   x3 = objfactory(3);

   arraylist_add(list, x1 = objfactory(1));    
   arraylist_add(list, x2 = objfactory(2));

   result = arraylist_indexofp(list, x1, 0); 
   CU_ASSERT_EQUAL(result, 0); 

   result = arraylist_indexofp(list, x2, 0); 
   CU_ASSERT_EQUAL(result, 1); 

   result = arraylist_indexofp(list, x3, 0); 
   CU_ASSERT_EQUAL(result, -1);  /* not found */

   result = arraylist_indexofp(list, x1, 1); /* start at index 1 */ 
   CU_ASSERT_EQUAL(result, -1); 

   result = arraylist_indexof(list, x1, 0, al_comparator);
   CU_ASSERT_EQUAL(result, 0); 

   result = arraylist_indexof(list, x2, 0, al_comparator);
   CU_ASSERT_EQUAL(result, 1); 

   result = arraylist_indexof(list, x3, 0, al_comparator);
   CU_ASSERT_EQUAL(result, -1); 

   result = arraylist_indexof(list, x1, 1, al_comparator);   
   CU_ASSERT_EQUAL(result, -1); 

   etch_free(x3);

   /* destroy the arraylist freeing the test object content as well */
   arraylist_destroy(list,TRUE); 

   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
   memtable_clear();  /* start fresh for next test */   
}


/* 
 * test arraylist_get()
 */
void test_get(void)
{
   TESTOBJ *x1, *x2, *x3, *x;
   etch_arraylist* list = new_arraylist(8,0); 

   arraylist_add(list, x1 = objfactory(1));    
   arraylist_add(list, x2 = objfactory(2));
   arraylist_add(list, x3 = objfactory(3));

   x = arraylist_get(list,0); 
   CU_ASSERT_EQUAL(x, x1); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->signature, OBJSIG); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->id, 1); 

   x = arraylist_get(list,1); 
   CU_ASSERT_EQUAL(x, x2); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->signature, OBJSIG); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->id, 2); 

   x = arraylist_get(list,2); 
   CU_ASSERT_EQUAL(x, x3); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->signature, OBJSIG); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->id, 3); 

   x = arraylist_get(list,3); 
   CU_ASSERT_EQUAL(x, NULL); 

   /* destroy the arraylist freeing the test object content as well */
   arraylist_destroy(list,TRUE); 

   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
   memtable_clear();  /* start fresh for next test */   
}


/* 
 * test arraylist_set()
 */
void test_set(void)
{
   int result = 0;
   TESTOBJ *x1, *x2, *x3, *x4, *x5;
   etch_arraylist* list = new_arraylist(8,0); 

   arraylist_add(list, x1 = objfactory(1));    
   arraylist_add(list, x2 = objfactory(2));
   arraylist_add(list, x3 = objfactory(3));

   result = arraylist_set(list, 0, x4 = objfactory(4)); 
   CU_ASSERT_EQUAL(result,0); 
   CU_ASSERT_EQUAL(list->base[0], x4); 

   result = arraylist_set(list, 2, x5 = objfactory(5)); 
   CU_ASSERT_EQUAL(result,0); 
   CU_ASSERT_EQUAL(list->base[2], x5); 

   result = arraylist_set(list, 3, x5); /* attempt replace beyond end of list */
   CU_ASSERT_EQUAL(result,-1); 

   /* destroy the arraylist without freeing the test object content */
   arraylist_destroy(list,FALSE); 

   etch_free(x1); etch_free(x2); etch_free(x3); etch_free(x4); etch_free(x5); 

   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
   memtable_clear();  /* start fresh for next test */   
}


/* 
 * test arraylist auto reallocation
 */
void test_expand(void)
{
   const int INIT_MAX = 4;
   TESTOBJ *x1, *x2, *x3, *x4, *x5, *x6, *x;
   etch_arraylist* list = new_arraylist(INIT_MAX,0); 

   arraylist_add(list, x1 = objfactory(1));    
   arraylist_add(list, x2 = objfactory(2));
   arraylist_add(list, x3 = objfactory(3));
   arraylist_add(list, x4 = objfactory(4));
   CU_ASSERT_EQUAL(list->size, INIT_MAX * sizeof(void**)); 

   arraylist_add(list, x5 = objfactory(5));
   CU_ASSERT_EQUAL(list->size, INIT_MAX * sizeof(void**) * 2); 
   arraylist_add(list, x6 = objfactory(6));

   x = arraylist_get(list,4); 
   CU_ASSERT_EQUAL(x, x5); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->signature, OBJSIG); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->id, 5); 
   x = arraylist_get(list,5); 
   CU_ASSERT_EQUAL(x, x6); 

   /* destroy the arraylist freeing the test object content as well */
   arraylist_destroy(list,TRUE); 

   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
   memtable_clear();  /* start fresh for next test */   
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
    CU_set_output_filename("../test_arraylist");

    pSuite = CU_add_suite("suite_arraylist", init_suite, clean_suite);

    CU_add_test(pSuite, "test new_arraylist()", test_new_arraylist); 
    CU_add_test(pSuite, "test arraylist_add()", test_add);
    CU_add_test(pSuite, "test arraylist_destroy()", test_destroy);
    CU_add_test(pSuite, "test arraylist_remove() first", test_remove_firstitem);
    CU_add_test(pSuite, "test arraylist_remove() last", test_remove_lastitem);
    CU_add_test(pSuite, "test arraylist_remove() mid", test_remove);
    CU_add_test(pSuite, "test arraylist_insert() first", test_insert_first);
    CU_add_test(pSuite, "test arraylist_insert() mid", test_insert_mid); 
    CU_add_test(pSuite, "test arraylist_insert() end", test_insert_end);
    CU_add_test(pSuite, "test arraylist_contains(), containsp", test_contains);  
    CU_add_test(pSuite, "test arraylist_indexof(), indexofp", test_indexof);
    CU_add_test(pSuite, "test arraylist_get()", test_get); 
    CU_add_test(pSuite, "test arraylist set", test_set);
    CU_add_test(pSuite, "test arraylist auto realloc", test_expand);   

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

