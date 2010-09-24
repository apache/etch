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
 * test_threadpool.c
 */
#include "etch_runtime.h"
#include "etch_thread.h"
#include "etch_mutex.h"
#include "etch_wait.h"
#include "etch_arraylist.h"
#include "etch_mem.h"

#include <stdio.h>
#include "CUnit.h"

#define IS_DEBUG_CONSOLE FALSE

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

static etch_mutex* mutex_threadid;
static int g_next_threadid;

#define IS_DUMP_AFTER_REMOVE FALSE
#define IS_TESTING_REMOVE    TRUE
#define THREAD_TESTDATA_SIG 0xfeedface
struct usertestdata { int x; int y; };

static int next_threadid()
{
    etch_mutex_lock(mutex_threadid);
    ++g_next_threadid;
    etch_mutex_unlock(mutex_threadid);
    return g_next_threadid;
}

/*
 * get_threaddata()
 * get a handle to internal thread create parameters
 */
static etch_apr_threaddata* get_threaddata()
{
    static etch_apr_threaddata tdata;
    memset(&tdata, 0, sizeof(etch_apr_threaddata));
    return &tdata;
}


/**
 * on_threadstart()
 * thread start callback
 */
static int on_threadstart(void* param)
{
    #if (IS_DEBUG_CONSOLE)
    if  (NULL == param) 
         printf("pool thread start\n");  
    else printf("pool thread %04x start\n", params->etch_thread_id); 
    fflush(stdout);
    #endif
    return 0;
}


/**
 * on_threadexit()
 * thread stop callback
 */
static int on_threadexit(void* param)
{
    #if (IS_DEBUG_CONSOLE)
    if  (NULL == param)
         printf("pool thread exit\n");  
    else printf("pool thread %04x exit\n", params->etch_thread_id); 
    fflush(stdout);
    #endif
    return 0;
}


/**
 * createthread_threadproc
 */
static void createthread_threadproc(void* data)
{
    etch_thread_params* params;
    struct usertestdata* ud; 
    #if (IS_DEBUG_CONSOLE)
    printf("in createthread_threadproc\n"); 
    #endif

    params = (etch_thread_params*) data;
    ud = (struct usertestdata*) params->data;
    CU_ASSERT_PTR_NOT_NULL_FATAL(ud);
    CU_ASSERT_EQUAL(ud->x, 1);
    CU_ASSERT_EQUAL(ud->y, 2);
}


/**
 * test_createthread
 */
static void test_createthread(void)
{
    int result = 0;
    etch_thread_params tp;
    struct usertestdata  ud;
    etch_apr_threaddata* td = get_threaddata();
    memset(&tp, 0, sizeof(etch_thread_params));
    ud.x = 1; ud.y = 2;

    // TODO: pool
    etch_mutex_create(&mutex_threadid, ETCH_MUTEX_NESTED, NULL);

    etch_init_threadparams(&tp);
    tp.etch_thread_id = next_threadid();
    tp.data       = &ud;
    tp.libdata    = td;
    tp.on_start   = on_threadstart;
    tp.on_exit    = on_threadexit;
    tp.threadproc = createthread_threadproc;
    #if (IS_DEBUG_CONSOLE)
    printf("\n");
    #endif
    
    result = etch_createthread(&tp);    
    CU_ASSERT_EQUAL(result, 0);

    etch_thread_join(&tp);  /* block until thread exit */
    CU_ASSERT_EQUAL(result, 0);

    etch_mutex_destroy(mutex_threadid);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * mutextestparams
 * thread data for mutex test
 */
struct mutextestparams
{
    unsigned signature;
    etch_mutex* mutex;
    unsigned flag;
};


/**
 * mutex_threadproc_a
 */
static void mutex_threadproc_a(void* data)
{
    etch_status_t status;
    int result = 0;
    etch_thread_params* params;
    struct mutextestparams* p;
    params = (etch_thread_params*) data;
    p = (struct mutextestparams*) params->data;
    #if (IS_DEBUG_CONSOLE)
    printf("in threadproc_a\n"); fflush(stdout);
    #endif

    /* can't use a FATAL cunit macro outside the main thread 
     * since if it fails the exit handling causes an OS fault */

    CU_ASSERT_PTR_NOT_NULL(p); if (!p) return;
    CU_ASSERT_EQUAL(p->signature, THREAD_TESTDATA_SIG);
    if (p->signature != THREAD_TESTDATA_SIG) return;
    CU_ASSERT_PTR_NOT_NULL(p->mutex); if (!p->mutex) return;

    #if (IS_DEBUG_CONSOLE)
    printf("thread a waiting ...\n"); fflush(stdout);
    #endif

    while(p->flag == 0) /* wait until ready to acquire lock */   
          apr_sleep(1 * 1000 * 1000);

    CU_ASSERT_EQUAL(p->flag, 1); if (p->flag != 1) return;

    etch_mutex_lock(p->mutex);
    CU_ASSERT_EQUAL(result, 0); if (result != 0) return;

    p->flag = 2; /* signal thread b to attempt acquire */

    #if (IS_DEBUG_CONSOLE)
    printf("thread a waiting ...\n"); fflush(stdout);
    #endif

    while(p->flag == 2) /* wait for thread thread b signal */
          apr_sleep(1 * 1000 * 1000);

    status = etch_mutex_unlock(p->mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    p->flag = 4; /* signal thread b to proceed */

    #if (IS_DEBUG_CONSOLE)
    printf("thread a waiting ...\n"); fflush(stdout);
    #endif

    while(p->flag == 4) /* wait until thread b signals done */   
          apr_sleep(1 * 1000 * 1000);

    #if (IS_DEBUG_CONSOLE)
    printf("thread s exits\n"); fflush(stdout);
    #endif
}


/**
 * mutex_threadproc_b
 */
static void mutex_threadproc_b(void* data)
{
    etch_status_t status;
    etch_thread_params* params;
    struct mutextestparams* p;
    params = (etch_thread_params*) data;
    p = (struct mutextestparams*) params->data;
    #if (IS_DEBUG_CONSOLE)
    printf("in mutex_threadproc_b\n"); fflush(stdout);
    #endif

    CU_ASSERT_PTR_NOT_NULL(p);
    CU_ASSERT_EQUAL(p->signature, THREAD_TESTDATA_SIG);
    CU_ASSERT_PTR_NOT_NULL(p->mutex);

    #if (IS_DEBUG_CONSOLE)
    printf("thread b waiting ...\n"); fflush(stdout);
    #endif

    while(p->flag != 2) /* wait until ready to attempt lock */   
          apr_sleep(1 * 1000 * 1000);
    
    status = etch_mutex_trylock(p->mutex);
    CU_ASSERT_NOT_EQUAL(status, ETCH_SUCCESS);

    p->flag = 3; /* signal thread a to continue */

    #if (IS_DEBUG_CONSOLE)
    printf("thread b waiting ...\n"); fflush(stdout);
    #endif

    while(p->flag == 3) /* wait for thread a to release lock */   
          apr_sleep(1 * 1000 * 1000);

    status = etch_mutex_trylock(p->mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_unlock(p->mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    p->flag = 5; /* signal thread a that we are done here */

    #if (IS_DEBUG_CONSOLE)
    printf("thread b waiting 2 secs ...\n"); fflush(stdout);
    #endif

    /* ensure thread b is last to exit for simplicity */
    apr_sleep(2 * 1000 * 1000); 

    #if (IS_DEBUG_CONSOLE)
    printf("thread_b exits\n"); fflush(stdout);
    #endif
}


/**
 * test_mutex()
 * launches two threads which share a mutex and a signal flag with the main thread.
 * 1. thread a sets the lock, signals thread b, and waits.
 * 2. thread b tries to set the lock, fails as expected, signals thread a, and waits.
 * 3. thread a releases the lock, signals thread b, and waits.
 * 4. thread b tries the lock, succeeds as expected, releases the lock, signals thread a.
 * 5. thread a exits
 * 6. thread b exits
 */
static void test_mutex(void)
{
    etch_status_t status;
    int result = 0;
    etch_mutex* mutex = 0;
    etch_apr_threaddata* td = get_threaddata();
    etch_thread_params tpa, tpb;
    struct mutextestparams threadparams;
    etch_init_threadparams(&tpa);
    etch_init_threadparams(&tpb);

    // TODO: pool
    etch_mutex_create(&mutex_threadid, ETCH_MUTEX_NESTED, NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(mutex_threadid);

    // TODO: pool
    etch_mutex_create(&mutex, ETCH_MUTEX_NESTED, NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(mutex);

    status = etch_mutex_lock(mutex);
    CU_ASSERT_EQUAL_FATAL(status, ETCH_SUCCESS);

    // result = mutex->tryacquire(mutex); /* mutexes are nestable */
    // CU_ASSERT_NOT_EQUAL_FATAL(result, 0);
    status = etch_mutex_unlock(mutex);
    CU_ASSERT_EQUAL_FATAL(status, ETCH_SUCCESS);

    threadparams.mutex = mutex;
    threadparams.flag  = 0;
    threadparams.signature = THREAD_TESTDATA_SIG;

    tpa.etch_thread_id = next_threadid();
    tpa.libdata    = td;
    tpa.on_start   = on_threadstart;
    tpa.on_exit    = on_threadexit;
    tpa.threadproc = mutex_threadproc_a;  
    tpa.data = &threadparams;

    #if (IS_DEBUG_CONSOLE)
    printf("\n");
    #endif

    result = etch_createthread(&tpa); /* create thread a */
    CU_ASSERT_EQUAL(result, 0);

    memcpy(&tpb, &tpa, sizeof(etch_thread_params));
    tpb.etch_thread_id = next_threadid();
    tpb.threadproc = mutex_threadproc_b;

    result = etch_createthread(&tpb); /* create thread b */   
    CU_ASSERT_EQUAL(result, 0);

    /* signal the new threads to commence doing something */
    threadparams.flag = 1;

    etch_thread_join(&tpa);  /* wait for thread a exit */
    etch_thread_join(&tpb);  /* wait for thread b exit */
    
    etch_mutex_destroy(mutex);
    etch_mutex_destroy(mutex_threadid);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * synchedlist_testparams
 * thread data for synched list test
 */
typedef struct synchedlist_testparams
{
    unsigned signature;
    etch_arraylist* list;
    etch_mutex* mutex;
    uint16 flag;
} synchedlist_testparams;

/*
static void dumplist(etch_arraylist* list)
{
    int i=0;
    char result;
    const int n = list->count;
    result = etch_arraylist_trylock(list);
    if (result != 0) return;
    // printf("\ndumping list ...\n"); fflush(stdout);
    for(; i < n; i++)
    {   etch_int32* item = list->base[i];
        printf("\n list[%d] %d\n", i, item->value); fflush(stdout);
    }

    result = etch_arraylist_rellock(list);
    // printf("\nany key ..."); while(!c) c = _getch(); printf("\n"); 
}
*/

/* 
 * synched_arraylist_comparator()
 * comparator to compare an integer constant with an etch_int32.
 */
static int synched_arraylist_comparator(void* testvalue, void* listcontent)
{
    const int ivalue  = (int) (size_t) testvalue;
    etch_int32*  listobj = (etch_int32*) listcontent;
    int jvalue = listobj->value;
    return ivalue < jvalue? -1: ivalue > jvalue? 1: 0;
} 


/**
 * synchedlist_threadproc_a
 */
static void synchedlist_threadproc_a(void* data)
{
    int result = 0, i;
    uint16 flag = 0;
    etch_iterator iterator;
    etch_thread_params* params;
    synchedlist_testparams* p;
    params = (etch_thread_params*) data;
    p = (struct synchedlist_testparams*) params->data;
    #if (IS_DEBUG_CONSOLE)
    printf("in threadproc_a\n"); fflush(stdout);
    #endif

    /* can't use a FATAL cunit macro outside the main thread 
     * since if it fails the exit handling causes an OS fault */

    CU_ASSERT_PTR_NOT_NULL(p); if (!p) return;
    CU_ASSERT_EQUAL(p->signature, THREAD_TESTDATA_SIG);
    if (p->signature != THREAD_TESTDATA_SIG) return;
    CU_ASSERT_PTR_NOT_NULL(p->list); if (!p->list) return;

    #if (IS_DEBUG_CONSOLE)
    printf("thread a waiting ...\n"); fflush(stdout);
    #endif

    /* wait until main signals start */
    do {
        etch_mutex_lock(p->mutex);
        flag = p->flag;
        etch_mutex_unlock(p->mutex);
        apr_sleep(1 * 500 * 1000);
    } while(flag < 2);

     CU_ASSERT_TRUE(p->flag >= 2);
    if (p->flag < 2) {
        #if (IS_DEBUG_CONSOLE)
            printf("thread a p->flag < 2 is false ...\n"); fflush(stdout);
        #endif
        return;
    }

    /* both threads should be contending for the list now */
        /* wait until main signals start */
    for(i = 1; i <= 20; i++)
    {
        #if (IS_DEBUG_CONSOLE)
        printf("thread a arraylist_add ...\n"); fflush(stdout);
        #endif
        etch_arraylist_add(p->list, new_int32(i));
        etch_thread_yield();        
    }

    etch_mutex_lock(p->mutex);
    p->flag++;
    etch_mutex_unlock(p->mutex);
    do {
        etch_mutex_lock(p->mutex);
        flag = p->flag;
        etch_mutex_unlock(p->mutex);
        apr_sleep(1 * 500 * 1000);
    } while(flag < 3);


    CU_ASSERT_EQUAL(p->list->count, 40); 
    
    etch_mutex_lock(p->mutex);
    p->flag++;
    etch_mutex_unlock(p->mutex);
    do {
        etch_mutex_lock(p->mutex);
        flag = p->flag;
        etch_mutex_unlock(p->mutex);
        apr_sleep(1 * 500 * 1000);
    } while(flag < 5);

    #if IS_TESTING_REMOVE

    /* both threads should be contending for the list now */
    for(i = 4; i <= 20; i += 4)
    {
        /* get the list index for value i */
        const int index = etch_arraylist_indexof(p->list, (void*)(size_t) i, 0, synched_arraylist_comparator);
        CU_ASSERT_NOT_EQUAL(index, -1);

        if (index >= 0)
        {
            result = etch_arraylist_remove(p->list, index, TRUE);
            CU_ASSERT_EQUAL(result, 0);

            #if (IS_DEBUG_CONSOLE)
            if  (result == 0)
                 printf("thread a arraylist_remove index %d value %d\n", index, i);
            else printf("thread a could not remove index %d value %d\n", index, i);
            fflush(stdout);
            #endif 
        }

        etch_thread_yield();
    }

    #endif /* IS_TESTING_REMOVE */

    p->flag++;
    while(p->flag < 7) /* wait until both threads are done removing from list */   
          apr_sleep(1 * 1000 * 500);

    #if IS_TESTING_REMOVE
    CU_ASSERT_EQUAL(p->list->count, 30); /* each thread removed 5 items */
    #endif

    etch_mutex_lock(p->mutex);
    p->flag++;
    etch_mutex_unlock(p->mutex);
    do {
        etch_mutex_lock(p->mutex);
        flag = p->flag;
        etch_mutex_unlock(p->mutex);
        apr_sleep(1 * 500 * 1000);
    } while(flag < 9);

    set_iterator(&iterator, p->list, &p->list->iterable);

    result = etch_arraylist_getlock(p->list);
    CU_ASSERT_EQUAL(result, 0); 

    #if (IS_DEBUG_CONSOLE)
    printf("thread a iterating list ...\n"); fflush(stdout);
    #endif  

    while(iterator.has_next(&iterator))
    {
        etch_int32* item = (etch_int32*) iterator.current_value;
        CU_ASSERT_PTR_NOT_NULL(item); 

        if (item)  
        {   result = item->value;
            #if (IS_DEBUG_CONSOLE)
            printf("thread a list value %d\n", result); fflush(stdout);
            #endif  
            #if IS_TESTING_REMOVE
            CU_ASSERT_NOT_EQUAL(result % 4, 0); /* we removed all modulo 4 values */
            #endif
        }
        
        result = iterator.next(&iterator);
    }

    etch_sleep(1000);
    result = etch_arraylist_rellock(p->list);


    etch_mutex_lock(p->mutex);
    p->flag++;
    etch_mutex_unlock(p->mutex);
    #if (IS_DEBUG_CONSOLE)
         printf("thread a waiting to exit ...\n"); fflush(stdout);
    #endif
    do {
        etch_mutex_lock(p->mutex);
        flag = p->flag;
        etch_mutex_unlock(p->mutex);
        apr_sleep(1 * 500 * 1000);
    } while(flag < 11);

    #if (IS_DEBUG_CONSOLE)
    printf("thread a exits\n"); fflush(stdout);
    #endif
}


/**
 * synchedlist_threadproc_b
 */
static void synchedlist_threadproc_b(void* data)
{
    int result = 0, i;
    uint16 flag = 0;
    etch_iterator iterator;
    etch_thread_params* params;
    struct synchedlist_testparams* p; 
    params = (etch_thread_params*) data;
    p = (struct synchedlist_testparams*) params->data;
    #if (IS_DEBUG_CONSOLE)
    printf("in threadproc_b\n"); fflush(stdout);
    #endif

    /* can't use a FATAL cunit macro outside the main thread 
     * since if it fails the exit handling causes an OS fault */

    CU_ASSERT_PTR_NOT_NULL(p); if (!p) return;
    CU_ASSERT_EQUAL(p->signature, THREAD_TESTDATA_SIG);
    if (p->signature != THREAD_TESTDATA_SIG) return;
    CU_ASSERT_PTR_NOT_NULL(p->list); if (!p->list) return;

    #if (IS_DEBUG_CONSOLE)
    printf("thread b waiting ...\n"); fflush(stdout);
    #endif

    /* wait until main signals start */
    do {
        etch_mutex_lock(p->mutex);
        flag = p->flag;
        etch_mutex_unlock(p->mutex);
        apr_sleep(1 * 500 * 1000);
    } while(flag == 0);

    CU_ASSERT_EQUAL(p->flag, 1); if (p->flag != 1) return;

    /* both threads should be contending for the list now */
    for(i = 21; i <= 40; i++)
    {
        #if (IS_DEBUG_CONSOLE)
        printf("thread b arraylist_add ...\n"); fflush(stdout);
        #endif
        etch_arraylist_add(p->list, new_int32(i));
        etch_thread_yield();
    }

    p->flag++;
    while(p->flag < 3) /* wait until both threads are done adding to list */   
          apr_sleep(1 * 1000 * 500);

    CU_ASSERT_EQUAL(p->list->count, 40); 
    p->flag++;
    while(p->flag < 5) /* wait until both threads are done checking list */   
          apr_sleep(1 * 1000 * 500);

    #if IS_TESTING_REMOVE

    /* both threads should be contending for the list now */
    for(i = 24; i <= 40; i += 4)
    {
        const int index = etch_arraylist_indexof(p->list, (void*)(size_t) i, 0, synched_arraylist_comparator);
        CU_ASSERT_NOT_EQUAL(index, -1);

        if (index >= 0)
        {
            result = etch_arraylist_remove(p->list, index, TRUE);
            CU_ASSERT_EQUAL(result, 0);

            #if (IS_DEBUG_CONSOLE)
            if  (result == 0)
                 printf("thread b arraylist_remove index %d value %d\n", index, i);
            else printf("thread b could not remove index %d value %d\n", index, i);
            fflush(stdout);
            #endif 
        }
            
        etch_thread_yield();        
    }

    #endif /*  #if IS_TESTING_REMOVE */

    p->flag++;
    while(p->flag < 7) /* wait until both threads are done removing from list */   
          apr_sleep(1 * 1000 * 500);

    #if IS_TESTING_REMOVE
    CU_ASSERT_EQUAL(p->list->count, 30); /* each thread removed 5 items */

    #if (IS_DEBUG_CONSOLE)
    printf("thread b list count after remove is %d\n", p->list->count); fflush(stdout);
    #endif /* IS_DEBUG_CONSOLE */
    #endif /* IS_TESTING_REMOVE */

    #if (IS_DEBUG_CONSOLE)
    printf("thread b waiting for list lock ...\n"); fflush(stdout);
    #endif /* IS_DEBUG_CONSOLE */
    p->flag++;
    while(p->flag < 9) /* wait until both threads are done checking list */   
          apr_sleep(1 * 1000 * 500);

    set_iterator(&iterator, p->list, &p->list->iterable);  

    result = etch_arraylist_getlock(p->list);
    CU_ASSERT_EQUAL(result, 0); 

    #if (IS_DEBUG_CONSOLE)
    printf("thread b iterating list ...\n"); fflush(stdout);
    #endif  /* IS_DEBUG_CONSOLE */ 

    while(iterator.has_next(&iterator))
    {   
        etch_int32* item = (etch_int32*) iterator.current_value;
        CU_ASSERT_PTR_NOT_NULL(item); 

        if (item)  
        {   result = item->value;
            #if (IS_DEBUG_CONSOLE)
            printf("thread b list value %d\n", result); fflush(stdout);
            #endif  
            #if IS_TESTING_REMOVE
            CU_ASSERT_NOT_EQUAL(result % 4, 0); /* we removed all modulo 4 values */
            #endif
        }
        
        iterator.next(&iterator);
    }

    result = etch_arraylist_rellock(p->list);

    #if (IS_DEBUG_CONSOLE)
         printf("thread b waiting to exit ...\n"); fflush(stdout);
    #endif 

    p->flag++;
    while(p->flag < 11) /* wait until both threads are done iterating list */   
          apr_sleep(1 * 1000 * 500);

    /* ensure thread b is last to exit for simplicity */
    apr_sleep(1 * 1000 * 1000); 

    #if (IS_DEBUG_CONSOLE)
    printf("thread b exits\n"); fflush(stdout);
    #endif
}


/**
 * test_synched_arraylist()
 * launches two threads which share a synchronized arraylist.
 */
static void test_synched_arraylist(void)
{
    int result = 0;
    etch_mutex* mutex = 0;
    synchedlist_testparams tparams;
    etch_apr_threaddata* td = get_threaddata();
    etch_thread_params tpa, tpb;
    memset(&tpa, 0, sizeof(etch_thread_params));
    memset(&tpb, 0, sizeof(etch_thread_params));

    // TODO: pool
    etch_mutex_create(&mutex_threadid, ETCH_MUTEX_NESTED, NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(mutex_threadid);

    // TODO: pool
    etch_mutex_create(&mutex, ETCH_MUTEX_NESTED, NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(mutex_threadid);

    tparams.list  = new_etch_arraylist(0,0);
    ((etch_object*)tparams.list)->synclock = mutex; /* list now owns the mutex */
    tparams.flag  = 0;
    tparams.signature = THREAD_TESTDATA_SIG;

    // TODO: pool
    etch_mutex_create(&tparams.mutex, ETCH_MUTEX_NESTED, NULL);
    CU_ASSERT_PTR_NOT_NULL_FATAL(mutex_threadid);

    tpa.etch_thread_id = ++g_next_threadid;
    tpa.libdata    = td;
    tpa.on_start   = on_threadstart;
    tpa.on_exit    = on_threadexit;
    tpa.threadproc = synchedlist_threadproc_a;  
    tpa.data = &tparams;

    #if (IS_DEBUG_CONSOLE)
    printf("\n");
    #endif

    result = etch_createthread(&tpa); /* create thread a */
    CU_ASSERT_EQUAL(result, 0);

    memcpy(&tpb, &tpa, sizeof(etch_thread_params));
    tpb.etch_thread_id = ++g_next_threadid;
    tpb.threadproc = synchedlist_threadproc_b;

    result = etch_createthread(&tpb); /* create thread b */   
    CU_ASSERT_EQUAL(result, 0);

    /* signal both threads to start doing something */
    etch_mutex_lock(tparams.mutex);
    tparams.flag = 1;
    etch_mutex_unlock(tparams.mutex);

    etch_thread_join(&tpb);  /* wait for thread b exit */
    etch_thread_join(&tpa);  /* wait for thread a exit */
    
    etch_object_destroy(tparams.list); /* destroy list and mutex */

    etch_mutex_destroy(mutex_threadid);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


typedef struct waittestdata { int flag; int num; int is_disposable; } waittestdata;


/**
 * wait_threadproc_a
 */
static void wait_threadproc_a(void* data)
{
    etch_thread_params* params;
    waittestdata* threadtestdata; 
    params = (etch_thread_params*) data;
    threadtestdata = (waittestdata*) params->data;

    #if (IS_DEBUG_CONSOLE)
    printf("\nin wait thread_a\n"); fflush(stdout);
    #endif

    threadtestdata->flag = 1; 

    etch_sleep(3000);

    if (threadtestdata->is_disposable)
        etch_free(threadtestdata);

    #if (IS_DEBUG_CONSOLE)
    printf("exit wait thread_a\n"); fflush(stdout);
    #endif
}


/**
 * test_wait()
 * test separate thread create and start   
 */
static void test_wait(void)
{
    int  result = 0;
    etch_thread* thread    = 0;
    waittestdata* testdata = 0;  
    etch_thread_params* tp  = 0;

    testdata = etch_malloc(sizeof(waittestdata),0);
    testdata->flag = testdata->num = 0;
    testdata->is_disposable = FALSE;

    /* create a thread that waits to be started */
    thread = new_thread(wait_threadproc_a, testdata);
    CU_ASSERT_PTR_NOT_NULL_FATAL(thread);

    #if (IS_DEBUG_CONSOLE)
    printf("\nnew thread waiting for signal coming in ... "); fflush(stdout);
    #endif

    tp = &thread->params;
    /* testdata = tp->data; */

    #if (IS_DEBUG_CONSOLE)
    printf("3 ... "); fflush(stdout);
    #endif   
    etch_sleep(1000);

    #if (IS_DEBUG_CONSOLE)
    printf("2 ... "); fflush(stdout);
    #endif   
    etch_sleep(1000);

    #if (IS_DEBUG_CONSOLE)
    printf("1 ... "); fflush(stdout);
    #endif   
    etch_sleep(1000);

    #if (IS_DEBUG_CONSOLE)
    printf("\nmain signaling new thread to start ...\n"); fflush(stdout);
    #endif

    thread->start(thread);

    #if (IS_DEBUG_CONSOLE)
    printf("main issued newthread.start, waiting for exit ...\n"); fflush(stdout);
    #endif  

    etch_sleep(1000);
    
    etch_thread_join(tp);  /* block until thread exit */

    result = testdata->flag;
    CU_ASSERT_EQUAL(result, 1);
    etch_free(testdata);
    etch_object_destroy(thread);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


typedef struct wait_until_testdata 
{
  etch_wait_t* waiter;   /* the condition variable waiter */
  int64 waitingfor;   /* value the waiter is waiting for */
  int   sleepstartms; 
  int   sleepexitms; 
} wait_until_testdata;


/**
 * wait_until_threadproc_a
 */
static void wait_until_threadproc_a(void* data)
{
    etch_wait_t* waiter;
    etch_thread_params* params;
    wait_until_testdata* threadtestdata; 
    params = (etch_thread_params*) data;
    threadtestdata = (wait_until_testdata*) params->data;
    waiter = threadtestdata->waiter;

    #if (IS_DEBUG_CONSOLE)
    printf("\nthread: started ...\n", threadtestdata->sleepstartms); fflush(stdout);
    #endif

    etch_sleep(threadtestdata->sleepstartms);  /* wait a bit before setting condition */

    #if (IS_DEBUG_CONSOLE)
    printf("\nthread: setting unblock condition ...\n"); fflush(stdout);
    #endif

    etch_wait_set(waiter, threadtestdata->waitingfor);

//    waiter->set(waiter, threadtestdata->waitingfor);  /* set wait condition to unblock waiters */

    etch_sleep(threadtestdata->sleepexitms);  /* wait a bit before exiting thread */

    etch_free(threadtestdata);  /* free caller's memory */

    #if (IS_DEBUG_CONSOLE)
    printf("thread: exiting ...\n"); fflush(stdout);
    #endif
}


/**
 * test_wait_until()
 * test wait on condition variable -- block until the specified condition is met.  
 */
static void test_wait_until(void)
{
    etch_status_t status;
    int   result = 0;
    etch_wait_t* waiter;
    etch_thread* thread = 0;
    etch_thread_params* tp = 0;
    const int64 VALUE_TO_WAIT_FOR = 1;
    wait_until_testdata* testdata = 0;  

    // TODO: pool
    etch_wait_create(&waiter, NULL);

    testdata = etch_malloc(sizeof(wait_until_testdata),0);
    memset(testdata, 0, sizeof(wait_until_testdata));
    testdata->waiter = waiter;
    testdata->waitingfor = VALUE_TO_WAIT_FOR;  
    testdata->sleepstartms = testdata->sleepexitms = 2000;
   
    /* create a thread that waits to be started */
    thread = new_thread(wait_until_threadproc_a, testdata);
    CU_ASSERT_PTR_NOT_NULL_FATAL(thread);

    #if (IS_DEBUG_CONSOLE)
    printf("\nmain: countdown to start worker thread ... "); fflush(stdout);
    #endif

    tp = &thread->params;
    testdata = tp->data; 

    #if (IS_DEBUG_CONSOLE)
    printf("3 ... "); fflush(stdout);
    #endif   
    etch_sleep(1000);

    #if (IS_DEBUG_CONSOLE)
    printf("2 ... "); fflush(stdout);
    #endif   
    etch_sleep(1000);

    #if (IS_DEBUG_CONSOLE)
    printf("1 ... "); fflush(stdout);
    #endif   
    etch_sleep(1000);

    #if (IS_DEBUG_CONSOLE)
    printf("\nmain: signaling worker thread to start ...\n"); fflush(stdout);
    #endif

    thread->start(thread);

    #if (IS_DEBUG_CONSOLE)
    printf("main: worker thread started\n");  
    printf("main: start blocking on condition variable ...\n"); fflush(stdout);
    #endif  

    /* wait for worker thread to set condition variable */
    //result = waiter->timed_waitequal(waiter, &condvar, VALUE_TO_WAIT_FOR, 5000);  
    status = etch_wait_timedwait(waiter, VALUE_TO_WAIT_FOR, 5000);
    switch(status) {
        case ETCH_SUCCESS:
            result = 0;
            break;
        case ETCH_ETIMEOUT:
            result = ETCH_TIMEOUT;
            break;
        default:
            result = -1;
            break;
    }

    #if (IS_DEBUG_CONSOLE)
    printf("main: wait unblocked, waiting for worker thread to exit ...\n"); fflush(stdout);
    #endif  
    CU_ASSERT_NOT_EQUAL_FATAL(result, -1);
    CU_ASSERT_NOT_EQUAL(result, ETCH_TIMEOUT);

    etch_thread_join(tp);  /* block until thread exit */
    
    etch_wait_destroy(waiter);
    etch_object_destroy(thread);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}



/**
 * test_wait_until_preexisting()
 * test wait on condition variable when unblock condition exists prior to request.  
 */
static void test_wait_until_preexisting(void)
{
    etch_status_t status;
    int   result = 0;
    etch_wait_t* waiter;
    etch_thread* thread = 0;
    etch_thread_params* tp = 0;
    const int64 VALUE_TO_WAIT_FOR = 1;
    wait_until_testdata* testdata = 0;  
    clock_t tickcount1 = 0, tickcount2 = 0, tickdiff = 0;

    // TODO: pool
    etch_wait_create(&waiter, NULL);

    testdata = etch_malloc(sizeof(wait_until_testdata),0);
    memset(testdata, 0, sizeof(wait_until_testdata));
    testdata->waiter = waiter;
    testdata->waitingfor = VALUE_TO_WAIT_FOR;  
    testdata->sleepstartms = testdata->sleepexitms = 2000;
   
    /* create a thread that waits to be started */
    thread = new_thread(wait_until_threadproc_a, testdata);
    CU_ASSERT_PTR_NOT_NULL_FATAL(thread);

    #if (IS_DEBUG_CONSOLE)
    printf("\nmain: countdown to start worker thread ... "); fflush(stdout);
    #endif

    tp = &thread->params;
    testdata = tp->data; 

    #if (IS_DEBUG_CONSOLE)
    printf("3 ... "); fflush(stdout);
    #endif   
    etch_sleep(1000);

    #if (IS_DEBUG_CONSOLE)
    printf("2 ... "); fflush(stdout);
    #endif   
    etch_sleep(1000);

    #if (IS_DEBUG_CONSOLE)
    printf("1 ... "); fflush(stdout);
    #endif   
    etch_sleep(1000);

    #if (IS_DEBUG_CONSOLE)
    printf("\nmain: signaling worker thread to start ...\n"); fflush(stdout);
    #endif

    thread->start(thread);

    #if (IS_DEBUG_CONSOLE)
    printf("main: worker thread started\n");  
    #endif  

    //condvar = VALUE_TO_WAIT_FOR; /* pre-set unblock condition */
    // TODO: check if this is correct
    etch_wait_set(waiter, VALUE_TO_WAIT_FOR);
    tickcount1 = clock();

    /* wait for worker thread to set condition variable. we expect that the 
     * thread will not be started since the wait condition pre-exists */
    status = etch_wait_timedwait(waiter, VALUE_TO_WAIT_FOR, 5000);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    /* we expect zero wait time since the wait condition preexisted */
    tickcount2 = clock();
    tickdiff = tickcount2 - tickcount1;
    result = tickdiff < 20; /* resolution of tick timer could be as high as 20ms */ 
    CU_ASSERT_EQUAL(result, TRUE); /* if debugger stepping this assert will fail */

    /* block until thread exit. however we expect that it has already done so. */
    etch_thread_join(tp);  
    
    etch_wait_destroy(waiter);
    etch_object_destroy(thread);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_wait_until_negative_test()
 * test wait on condition variable  
 */
static void test_wait_until_negative_test(void)
{
    etch_status_t status;
    etch_wait_t* waiter;
    etch_thread* thread = 0;
    etch_thread_params* tp = 0;
    const int VALUE_TO_WAIT_FOR = 1, VALUE_TO_SET = 2, THREAD_PAUSE_MS = 1000;
    wait_until_testdata* testdata = 0;  

    // TODO: pool
    etch_wait_create(&waiter, NULL);

    testdata = etch_malloc(sizeof(wait_until_testdata),0);
    memset(testdata, 0, sizeof(wait_until_testdata));
    testdata->waiter = waiter;
    testdata->waitingfor = VALUE_TO_SET;  /* not the value we're going to wait for */ 
    testdata->sleepstartms = testdata->sleepexitms = THREAD_PAUSE_MS;
   
    /* create a thread that waits to be started */
    thread = new_thread(wait_until_threadproc_a, testdata);
    CU_ASSERT_PTR_NOT_NULL_FATAL(thread);

    #if (IS_DEBUG_CONSOLE)
    printf("\nmain: countdown to start worker thread ... "); fflush(stdout);
    #endif

    tp = &thread->params;
    testdata = tp->data; 

    #if (IS_DEBUG_CONSOLE)
    printf("3 ... "); fflush(stdout);
    #endif   
    etch_sleep(1000);

    #if (IS_DEBUG_CONSOLE)
    printf("2 ... "); fflush(stdout);
    #endif   
    etch_sleep(1000);

    #if (IS_DEBUG_CONSOLE)
    printf("1 ... "); fflush(stdout);
    #endif   
    etch_sleep(1000);

    #if (IS_DEBUG_CONSOLE)
    printf("\nmain: signaling worker thread to start ...\n"); fflush(stdout);
    #endif

    thread->start(thread);

    #if (IS_DEBUG_CONSOLE)
    printf("main: worker thread started\n");  
    printf("main: start blocking on condition variable ...\n"); fflush(stdout);
    #endif  

    /* wait for worker thread to set condition variable. in this test, we have told
     * the thread to set some other value than what we are waiting for, so we want
     * to see a wait timeout here. in other words, we are testing that setting the
     * waiter to some value other than what we are waiting for, does not unblock. 
     */
    status = etch_wait_timedwait(waiter, VALUE_TO_WAIT_FOR, THREAD_PAUSE_MS + 1000);
    CU_ASSERT_EQUAL(status, ETCH_ETIMEOUT);
    //result = waiter->timed_waitequal(waiter, &condvar, VALUE_TO_WAIT_FOR, THREAD_PAUSE_MS + 1000 );  

    

    #if (IS_DEBUG_CONSOLE)
    printf("main: wait unblocked, waiting for worker thread to exit ...\n"); fflush(stdout);
    #endif  

    etch_thread_join(tp);  /* block until thread exit */

    etch_wait_destroy(waiter);
    etch_object_destroy(thread);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * wait_multiple_threadproc
 */
static void wait_multiple_threadproc(void* data)
{
    int threadnum, waitms, remainingms;
    etch_thread_params* params = (etch_thread_params*) data;
    waittestdata* tdata = (waittestdata*) params->data; 
    threadnum = tdata->num;
    waitms = tdata->flag;
    remainingms = waitms;

    #if (IS_DEBUG_CONSOLE)
    printf("\nwait thread %d sleeping %d ms ...\n", threadnum, waitms); fflush(stdout);
    #endif 

    while(remainingms > 0)
    {
        if (params->threadstate > ETCH_THREADSTATE_STARTED) break; 
        etch_sleep(remainingms > 500? 500: remainingms);
        remainingms -= 500;
    }

    #if (IS_DEBUG_CONSOLE)
    printf("exit wait thread %d\n", threadnum); fflush(stdout);
    #endif
}


/**
 * test_wait_for_multiple()
 * launch multiple threads and wait for them all to complete   
 */
static void test_wait_for_multiple(void)
{
    #define TEST_WAIT_MULT_NUMTHREADS 4

    etch_thread* thread[TEST_WAIT_MULT_NUMTHREADS];
    int waitms[TEST_WAIT_MULT_NUMTHREADS], i, priori = 1;

    for(i=0; i < TEST_WAIT_MULT_NUMTHREADS; i++)
    {   /* initialize wait times for each thread */
        const int newi = i & 1? priori - 1: priori + 2;
        waitms[i] = ((priori = newi) * 1000);        
    }

    #if (IS_DEBUG_CONSOLE)
    printf("main creating all threads ...\n"); fflush(stdout);
    #endif 
  
    for(i=0; i < TEST_WAIT_MULT_NUMTHREADS; i++) /* create all threads */
    {   
        etch_thread* newthread;
        waittestdata* thread_data = etch_malloc(sizeof(waittestdata),0);
        thread_data->num = i; thread_data->flag = waitms[i];

        newthread = new_thread(wait_multiple_threadproc, thread_data);
        CU_ASSERT_PTR_NOT_NULL_FATAL(newthread);

        /* configure thread to free memory for the waittestdata we passed it */
        newthread->params.is_own_data = TRUE;  
        thread[i] = newthread;
    }

    #if (IS_DEBUG_CONSOLE)
    printf("main starting all threads ...\n"); fflush(stdout);
    #endif 

    for(i=0; i < TEST_WAIT_MULT_NUMTHREADS; i++) /* start all threads */
        thread[i]->start(thread[i]);
         
    #if (IS_DEBUG_CONSOLE)
    printf("main waiting for all threads to exit ...\n"); fflush(stdout);
    #endif 

    for(i=0; i < TEST_WAIT_MULT_NUMTHREADS; i++) /* wait for all threads to exit */
        etch_join(thread[i]); 

    #if (IS_DEBUG_CONSOLE)
    printf("main all threads accounted for\n"); fflush(stdout);
    #endif 

    for(i=0; i < TEST_WAIT_MULT_NUMTHREADS; i++) /* free thread objects */
        etch_object_destroy(thread[i]);
       
#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_free_threadpool()
 * test thread "pool" (ad hoc threads)   
 */
static void test_free_threadpool(void)
{
    #define TEST_FREEPOOL_NUMTHREADS 4
    etch_threadpool* threadpool = NULL;
    void* runthread = NULL;
    int waitms[TEST_FREEPOOL_NUMTHREADS], i, priori = 1;

    for(i=0; i < TEST_FREEPOOL_NUMTHREADS; i++)
    {   /* initialize wait times for each thread */
        const int newi = i & 1? priori - 1: priori + 2;
        waitms[i] = ((priori = newi) * 1000);        
    }

    threadpool = new_threadpool(ETCH_THREADPOOLTYPE_FREE, 0);
    CU_ASSERT_PTR_NOT_NULL_FATAL(threadpool);
    threadpool->is_free_threads = TRUE;

    #if (IS_DEBUG_CONSOLE)
    printf("main creating %d pool threads ...\n", TEST_FREEPOOL_NUMTHREADS); fflush(stdout);
    #endif 
  
    for(i=0; i < TEST_FREEPOOL_NUMTHREADS; i++) /* launch all threads */
    {   
        waittestdata* thread_data = etch_malloc(sizeof(waittestdata),0);
        thread_data->num = i; thread_data->flag = waitms[i];

        runthread = threadpool->run (threadpool, wait_multiple_threadproc, thread_data);

        CU_ASSERT_PTR_NOT_NULL(runthread);
    }
         
    #if (IS_DEBUG_CONSOLE)
    printf("main all pool threads launched\n"); fflush(stdout);
    printf("main destroying thread pool ...\n"); fflush(stdout);
    #endif 

    etch_object_destroy(threadpool);

    #if (IS_DEBUG_CONSOLE)
    printf("main all threads accounted for\n"); fflush(stdout);
    #endif 
       
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
//int _tmain(int argc, _TCHAR* argv[])
CU_pSuite test_etch_threadpool_suite()
{    
    CU_pSuite ps = CU_add_suite("suite_threadpool", init_suite, clean_suite);

    CU_add_test(ps, "test create thread", test_createthread);
    CU_add_test(ps, "test etch mutex", test_mutex);
    CU_add_test(ps, "test arraylist synchronization", test_synched_arraylist);
    CU_add_test(ps, "test thread wait/start", test_wait);

    CU_add_test(ps, "test wait on condition", test_wait_until); 
    CU_add_test(ps, "test wait on pre-existing condition", test_wait_until_preexisting);
    CU_add_test(ps, "test wait on condition - negative", test_wait_until_negative_test); 

    CU_add_test(ps, "test wait all threads exit", test_wait_for_multiple);
    CU_add_test(ps, "test free threadpool", test_free_threadpool);

    return ps;
}
