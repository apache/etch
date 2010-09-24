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
#include "etch_runtime.h"
#include "etch_arraylist.h"
#include "etch_thread.h"
#include "etch_mem.h"

#include <stdio.h>
#include "CUnit.h"

#define OBJSIG 0xbadf00d
#define NUMITEMS 3
#define ETCHTYPEA_TESTOBJ 0xff
#define IS_DEBUG_CONSOLE FALSE

static int init_suite(void)
{
    etch_status_t etch_status = ETCH_SUCCESS;

    etch_status = etch_runtime_initialize(NULL);
    return etch_status;
}

static int clean_suite(void)
{
    return etch_runtime_shutdown();
}

typedef struct TESTOBJ
{   int id;
    int signature;
} TESTOBJ;


static TESTOBJ* objfactory(const int id)
{ 
   TESTOBJ* obj = etch_malloc(sizeof(TESTOBJ), ETCHTYPEA_TESTOBJ);
   obj->id = id; obj->signature = OBJSIG;
   return obj;
}


/**
 * comparator callback for contains(), indexof(), functions
 * typedef int (*etch_comparator) (void* myobj, void* otherobj);
 */
static int al_comparator(void* p, void* q)
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
static void test_new_arraylist(void)
{
    const int TESTSIZE = 2048, TESTDELTA = 1024;

    // check default size
    etch_arraylist* list = new_etch_arraylist(0, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(list);
    CU_ASSERT_PTR_NOT_NULL_FATAL(list->base);
    CU_ASSERT_EQUAL(list->count,0);
    CU_ASSERT_EQUAL(list->size,  ETCHARRAYLIST_DEFSIZE * sizeof(void**));
    CU_ASSERT_EQUAL(list->delta, ETCHARRAYLIST_DEFSIZE * sizeof(void**));

    //free memory and ensure nothing remains allocated
    etch_arraylist_destroy(list,TRUE);
#ifdef ETCH_DEBUGALLOC
    g_bytes_allocated = etch_showmem(TRUE,FALSE);
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);
#endif

    // check custome size
    list = new_etch_arraylist(TESTSIZE, TESTDELTA);
    CU_ASSERT_EQUAL(list->count,0);
    CU_ASSERT_EQUAL(list->size,  TESTSIZE  * sizeof(void**));
    CU_ASSERT_EQUAL(list->delta, TESTDELTA * sizeof(void**));
    memset(list->base, 0xff, list->size); /* try write to entire buffer */

    // free memory; ensure nothing remains allocated
    etch_arraylist_destroy(list,TRUE);
#ifdef ETCH_DEBUGALLOC
    g_bytes_allocated = etch_showmem(TRUE,FALSE);
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);
    // start fresh for next test
    memtable_clear();
#endif
}


/* 
 * test arraylist_add()
 */
static void test_add(void)
{
   TESTOBJ *x1, *x2;
   etch_arraylist* list = new_etch_arraylist(8,0); 
   CU_ASSERT_EQUAL(list->base[0],NULL);
   CU_ASSERT_EQUAL(list->base[1],NULL);

   etch_arraylist_add(list, x1 = objfactory(1));
   CU_ASSERT_EQUAL(list->count,1);
   CU_ASSERT_EQUAL(list->base[0], x1); /* ensure item's buffer slot OK */

   etch_arraylist_add(list, x2 = objfactory(2));
   CU_ASSERT_EQUAL(list->count,2);
   CU_ASSERT_EQUAL(list->base[1], x2); /* ensure item's buffer slot OK */

   // free memory including test objects
   etch_arraylist_destroy(list,TRUE);
#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(TRUE,IS_DEBUG_CONSOLE);
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test arraylist_destroy()
 */
static void test_destroy(void)
{
    CU_PASS("tested in all previous and subsequent tests"); 
}

/*
 * test arraylist_count()
 */
static void test_count(void)
{
    TESTOBJ *x1, *x2;
    etch_arraylist* list = new_etch_arraylist(8,0); 
    CU_ASSERT_EQUAL(list->count,0);
    CU_ASSERT_EQUAL(etch_arraylist_count(list),0);

    etch_arraylist_add(list, x1 = objfactory(1));
    CU_ASSERT_EQUAL(list->count,1);
    CU_ASSERT_EQUAL(etch_arraylist_count(list),1);

    etch_arraylist_add(list, x2 = objfactory(2));
    CU_ASSERT_EQUAL(list->count,2);
    CU_ASSERT_EQUAL(etch_arraylist_count(list),2);

    // free memory including test objects
    etch_arraylist_destroy(list,TRUE);
 #ifdef ETCH_DEBUGALLOC
    g_bytes_allocated = etch_showmem(TRUE,IS_DEBUG_CONSOLE);
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);
    // start fresh for next test
    memtable_clear();
 #endif
}

/* 
 * test arraylist_remove()
 */
static void test_remove_firstitem(void)
{
   TESTOBJ *x1, *x2;
   etch_arraylist* list = new_etch_arraylist(8,0);

   etch_arraylist_add(list, x1 = objfactory(1));   /* allocate and add test obj x1 */
   etch_arraylist_add(list, x2 = objfactory(2));   /* allocate and add test obj x2 */

   CU_ASSERT_EQUAL_FATAL(list->count,2);
   CU_ASSERT_EQUAL(list->base[0], x1);
   CU_ASSERT_EQUAL(list->base[1], x2);

   //remove list[0] and free its x1 memory
   etch_arraylist_remove(list,0,TRUE);

   CU_ASSERT_EQUAL_FATAL(list->count,1);

   // ensure TESTBOJ bytes were freed
   // ensure item's buffer slot cleared
   CU_ASSERT_EQUAL(list->base[1], NULL);

   // ensure item 1 is now item 0
   CU_ASSERT_EQUAL_FATAL(list->base[0], x2);
   
   // remove list[0] but don't free x2 memory
   etch_arraylist_remove(list,0,FALSE);

   CU_ASSERT_EQUAL(list->count,0); /* ensure no bytes were freed */

    // ensure item's buffer slot cleared
   CU_ASSERT_EQUAL(list->base[0], NULL);

   /* finally free the last test object, which would have been freed in the
    * prior arraylist_remove() had we not specified to not do so. Also if we
    * did not free it here it would be freed in the memtable_clear() below.
    */
   etch_free(x2);

   // destroy the arraylist
   etch_arraylist_destroy(list,TRUE);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}

/* 
 * test arraylist_remove()
 */
static void test_remove_lastitem(void)
{
   TESTOBJ *x1, *x2, *x3;
   etch_arraylist* list = new_etch_arraylist(8,0); 

   etch_arraylist_add(list, x1 = objfactory(1));   /* allocate and add test obj x1 */
   etch_arraylist_add(list, x2 = objfactory(2));   /* allocate and add test obj x2 */
   etch_arraylist_add(list, x3 = objfactory(3));   /* allocate and add test obj x3 */
   CU_ASSERT_EQUAL_FATAL(list->count,3);

   CU_ASSERT_EQUAL(list->base[2], x3);

   // remove list[2] and free its x3 memory
   etch_arraylist_remove(list,2,TRUE);

   CU_ASSERT_EQUAL(list->base[2], NULL); /* ensure item's buffer slot cleared */

   CU_ASSERT_EQUAL_FATAL(list->base[0], x1);
   CU_ASSERT_EQUAL_FATAL(list->base[1], x2);
   
   /* destroy the arraylist freeing the test object content as well */
   etch_arraylist_destroy(list,TRUE); 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}

/*
static void dumplist(etch_arraylist* list)
{
    int i=0; char c=0;
    const int n = list->count;
    printf("\ndumping list ...\n"); fflush(stdout);
    for(; i < n; i++)
    {   TESTOBJ* item = list->base[i];
        printf("\n list[%d] %d\n", i, item->id); fflush(stdout);
    }
    
    printf("\nany key ..."); while(!c) c = getchar(); printf("\n"); 
}
*/

/* 
 * removetest_comparator()
 * comparator for test_remove() arraylist_indexof
 * note that the test value is passed as an int not an object
 */
static int removetest_comparator(void* testvalue, void* listcontent)
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
static void test_remove(void)
{
    const int numitems = 10;
    int i=0, curritems = numitems;
    etch_arraylist* list = new_etch_arraylist(64,0); 

    for(; i < numitems; i++)
        etch_arraylist_add(list, objfactory(i));
        
    #if IS_DEBUG_CONSOLE
    dumplist(list);
    #endif

    for(i=2; i < numitems; i+=2)
    {
        /* remove the list content whose value is i */
        const int index = etch_arraylist_indexof(list, (void*)(size_t) i, 0, removetest_comparator);
        CU_ASSERT_NOT_EQUAL(index, -1);

        if (index >= 0)
        {
            if (0 == etch_arraylist_remove(list, index, TRUE))
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
    etch_arraylist_destroy(list,TRUE); 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test arraylist_insert()
 */
static void test_insert_first(void)
{
   TESTOBJ *x1, *x2, *x3;
   etch_arraylist* list = new_etch_arraylist(8,0); 

   etch_arraylist_add(list, x2 = objfactory(2));   /* allocate and add test obj x2 */
   etch_arraylist_add(list, x3 = objfactory(3));   /* allocate and add test obj x3 */

   /* insert test obj x1 into slot 0 */
   etch_arraylist_insert(list, 0, x1 = objfactory(1)); 

   CU_ASSERT_EQUAL_FATAL(list->count,3);
   CU_ASSERT_EQUAL(list->base[0], x1);
   CU_ASSERT_EQUAL(list->base[1], x2);    
   CU_ASSERT_EQUAL(list->base[2], x3);   

   /* destroy the arraylist freeing the test object content as well */
   etch_arraylist_destroy(list,TRUE); 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test arraylist_insert()
 */
static void test_insert_mid(void)
{
   TESTOBJ *x1, *x2, *x3;
   etch_arraylist* list = new_etch_arraylist(8,0); 

   etch_arraylist_add(list, x1 = objfactory(1));    
   etch_arraylist_add(list, x3 = objfactory(3));    

   /* insert test obj x1 into slot 1 */
   etch_arraylist_insert(list, 1, x2 = objfactory(2)); 

   CU_ASSERT_EQUAL_FATAL(list->count,3);
   CU_ASSERT_EQUAL(list->base[0], x1);
   CU_ASSERT_EQUAL(list->base[1], x2);    
   CU_ASSERT_EQUAL(list->base[2], x3);   

   /* destroy the arraylist freeing the test object content as well */
   etch_arraylist_destroy(list,TRUE); 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test arraylist_insert()
 */
static void test_insert_end(void)
{
   TESTOBJ *x1, *x2, *x3;
   etch_arraylist* list = new_etch_arraylist(8,0); 

   etch_arraylist_add(list, x1 = objfactory(1));    
   etch_arraylist_add(list, x2 = objfactory(2));   

   /* insert test obj x1 into slot 1 */
   etch_arraylist_insert(list, 2, x3 = objfactory(3)); 

   CU_ASSERT_EQUAL_FATAL(list->count,3);
   CU_ASSERT_EQUAL(list->base[0], x1);
   CU_ASSERT_EQUAL(list->base[1], x2);    
   CU_ASSERT_EQUAL(list->base[2], x3);   

   /* destroy the arraylist freeing the test object content as well */
   etch_arraylist_destroy(list,TRUE); 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test arraylist_containsp(), arraylist_contains()
 */
static void test_contains(void)
{
   TESTOBJ *x1, *x2, *x3;
   int result = 0;
   etch_arraylist* list = new_etch_arraylist(8,0); 

   etch_arraylist_add(list, x1 = objfactory(1));    
   etch_arraylist_add(list, x2 = objfactory(2));
   etch_arraylist_add(list, x3 = objfactory(3)); 

   result = etch_arraylist_containsp(list, x1, 0); 
   CU_ASSERT_EQUAL(result, TRUE); 

   result = etch_arraylist_containsp(list, x2, 0); 
   CU_ASSERT_EQUAL(result, TRUE); 

   result = etch_arraylist_containsp(list, x3, 0); 
   CU_ASSERT_EQUAL(result, TRUE); 

   result = etch_arraylist_containsp(list, x1, 1); /* start at index 1 */ 
   CU_ASSERT_EQUAL(result, FALSE); 

   result = etch_arraylist_contains(list, x1, 0, al_comparator);
   CU_ASSERT_EQUAL(result, TRUE); 

   result = etch_arraylist_contains(list, x2, 0, al_comparator);
   CU_ASSERT_EQUAL(result, TRUE); 

   result = etch_arraylist_contains(list, x3, 0, al_comparator);
   CU_ASSERT_EQUAL(result, TRUE); 

   result = etch_arraylist_contains(list, x1, 2, al_comparator); /* start at index 2 */ 
   CU_ASSERT_EQUAL(result, FALSE); 

   /* destroy the arraylist freeing the test object content as well */
   etch_arraylist_destroy(list,TRUE); 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}

/* 
 * test arraylist_indexofp(), arraylist_indexof()
 */
static void test_indexof(void)
{
   TESTOBJ *x1, *x2, *x3;
   int result = 0;
   etch_arraylist* list = new_etch_arraylist(8,0); 
   x3 = objfactory(3);

   etch_arraylist_add(list, x1 = objfactory(1));    
   etch_arraylist_add(list, x2 = objfactory(2));

   result = etch_arraylist_indexofp(list, x1, 0); 
   CU_ASSERT_EQUAL(result, 0); 

   result = etch_arraylist_indexofp(list, x2, 0); 
   CU_ASSERT_EQUAL(result, 1); 

   result = etch_arraylist_indexofp(list, x3, 0); 
   CU_ASSERT_EQUAL(result, -1);  /* not found */

   result = etch_arraylist_indexofp(list, x1, 1); /* start at index 1 */ 
   CU_ASSERT_EQUAL(result, -1); 

   result = etch_arraylist_indexof(list, x1, 0, al_comparator);
   CU_ASSERT_EQUAL(result, 0); 

   result = etch_arraylist_indexof(list, x2, 0, al_comparator);
   CU_ASSERT_EQUAL(result, 1); 

   result = etch_arraylist_indexof(list, x3, 0, al_comparator);
   CU_ASSERT_EQUAL(result, -1); 

   result = etch_arraylist_indexof(list, x1, 1, al_comparator);   
   CU_ASSERT_EQUAL(result, -1); 

   etch_free(x3);

   /* destroy the arraylist freeing the test object content as well */
   etch_arraylist_destroy(list,TRUE); 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test arraylist_get()
 */
static void test_get(void)
{
   TESTOBJ *x1, *x2, *x3, *x;
   etch_arraylist* list = new_etch_arraylist(8,0); 

   etch_arraylist_add(list, x1 = objfactory(1));    
   etch_arraylist_add(list, x2 = objfactory(2));
   etch_arraylist_add(list, x3 = objfactory(3));

   x = etch_arraylist_get(list,0); 
   CU_ASSERT_EQUAL(x, x1); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->signature, OBJSIG); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->id, 1); 

   x = etch_arraylist_get(list,1); 
   CU_ASSERT_EQUAL(x, x2); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->signature, OBJSIG); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->id, 2); 

   x = etch_arraylist_get(list,2); 
   CU_ASSERT_EQUAL(x, x3); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->signature, OBJSIG); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->id, 3); 

   x = etch_arraylist_get(list,3); 
   CU_ASSERT_EQUAL(x, NULL); 

   /* destroy the arraylist freeing the test object content as well */
   etch_arraylist_destroy(list,TRUE); 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test arraylist_set()
 */
static void test_set(void)
{
   int result = 0;
   TESTOBJ *x1, *x2, *x3, *x4, *x5;
   etch_arraylist* list = new_etch_arraylist(8,0); 

   etch_arraylist_add(list, x1 = objfactory(1));    
   etch_arraylist_add(list, x2 = objfactory(2));
   etch_arraylist_add(list, x3 = objfactory(3));

   result = etch_arraylist_set(list, 0, x4 = objfactory(4)); 
   CU_ASSERT_EQUAL(result,0); 
   CU_ASSERT_EQUAL(list->base[0], x4); 

   result = etch_arraylist_set(list, 2, x5 = objfactory(5)); 
   CU_ASSERT_EQUAL(result,0); 
   CU_ASSERT_EQUAL(list->base[2], x5); 

   result = etch_arraylist_set(list, 3, x5); /* attempt replace beyond end of list */
   CU_ASSERT_EQUAL(result,-1); 

   /* destroy the arraylist without freeing the test object content */
   etch_arraylist_destroy(list,FALSE); 

   etch_free(x1); etch_free(x2); etch_free(x3); etch_free(x4); etch_free(x5); 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* 
 * test arraylist auto reallocation
 */
static void test_expand(void)
{
   const int INIT_MAX = 4;
   TESTOBJ *x1, *x2, *x3, *x4, *x5, *x6, *x;
   etch_arraylist* list = new_etch_arraylist(INIT_MAX,0); 

   etch_arraylist_add(list, x1 = objfactory(1));
   etch_arraylist_add(list, x2 = objfactory(2));
   etch_arraylist_add(list, x3 = objfactory(3));
   etch_arraylist_add(list, x4 = objfactory(4));
   CU_ASSERT_EQUAL(list->size, INIT_MAX * sizeof(void**)); 

   etch_arraylist_add(list, x5 = objfactory(5));
   CU_ASSERT_EQUAL(list->size, INIT_MAX * sizeof(void**) * 2); 
   etch_arraylist_add(list, x6 = objfactory(6));

   x = etch_arraylist_get(list,4); 
   CU_ASSERT_EQUAL(x, x5); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->signature, OBJSIG); 
   CU_ASSERT_EQUAL(((TESTOBJ*)x)->id, 5); 
   x = etch_arraylist_get(list,5); 
   CU_ASSERT_EQUAL(x, x6); 

   /* destroy the arraylist freeing the test object content as well */
   etch_arraylist_destroy(list,TRUE); 

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,0);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}

CU_pSuite test_arraylist_suite()
{
    CU_pSuite pSuite = CU_add_suite("suite_arraylist", init_suite, clean_suite);
    CU_add_test(pSuite, "test new_arraylist()", test_new_arraylist); 
    CU_add_test(pSuite, "test arraylist_add()", test_add);
    CU_add_test(pSuite, "test arraylist_destroy()", test_destroy);
    CU_add_test(pSuite, "test arraylist_count()", test_count);
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
    return pSuite;
}
