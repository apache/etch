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
 * test_queue.c 
 */
#include "etch_runtime.h"
#include "etch_queue.h"
#include "etch_thread.h"
#include "etch_objecttypes.h"
#include "etch_threadpool_apr.h"
#include "etch_mem.h"

#include <stdio.h>
#include "CUnit.h"


#define IS_DEBUG_CONSOLE FALSE

// extern types
extern apr_pool_t* g_etch_main_pool;

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

/* - - - - - - - - - - - - - - 
 * unit test support
 * - - - - - - - - - - - - - -
 */

#define NUMBER_CONSUMERS     2  
#define CONSUMER_ACTIVITY    4
#define NUMBER_PRODUCERS     3  
#define PRODUCER_ACTIVITY    5
#define ERROR_BAD_THREADDATA 1
static int g_error, g_producercount, g_consumercount, g_unique_id, g_pro, g_con;


static unsigned next_test_id()
{
    apr_atomic_inc32 ((volatile apr_uint32_t*) &g_unique_id);
    return g_unique_id;
}

static unsigned next_producer()
{
    apr_atomic_inc32 ((volatile apr_uint32_t*) &g_pro);
    return g_pro;
}

static unsigned next_consumer()
{
    apr_atomic_inc32 ((volatile apr_uint32_t*) &g_con);
    return g_con;
}

 
/**
 * push_some_objects()
 * push specified number of new objects onto the queue
 */
static int push_some_objects(etch_queue* q, const int howmany)
{
    int count = 0, result = 0;

    while(count < howmany && result == 0)   
          result = etchqueue_put_withwait(q, ETCH_NOWAIT, new_int32(count++));

    return result;
}


/**
 * pop_some_objects()
 * pop specified number of objects off the queue and destroy each object retrieved
 */
static int pop_some_objects(etch_queue* q, const int howmany)
{
    int count = 0, result = 0;
    etch_object* thisobj = NULL;

    while(count++ < howmany && result == 0)
    {
      result = etchqueue_get_withwait(q, ETCH_NOWAIT, (void**)&thisobj);
        etch_object_destroy(thisobj);
        thisobj = NULL;
    }

    return result;
}


#if 0
/**
 * pop_all_objects()
 * pop all objects off the queue and destroy each object retrieved
 */
static int pop_all_objects(etch_queue* q)
{
    int popcount = 0, result = 0;
    const int queuecount = etchqueue_size(q);
    etch_object* thisobj = NULL;

    while(popcount++ < queuecount && result == 0)
    {
      result = etchqueue_get_withwait(q, ETCH_NOWAIT, (void**)&thisobj);
        if (thisobj) 
            thisobj->destroy(thisobj);
        thisobj = NULL;
    }

    return result;
}
#endif

/**
 * mytest_threaddata()
 * thread data for our test threads. we could pass the queue directly, but then
 * we'd have to configure the threadpool to not destroy a thread's data.
 */
typedef struct mytest_threaddata  { etch_queue* queue; }  mytest_threaddata;

#if 0
/**
 * new_testdata()
 * constructor for test thread data
 */
static mytest_threaddata* new_testdata(etch_queue* q)
{
    mytest_threaddata* td = etch_malloc(sizeof(mytest_threaddata), 0);
    td->queue = q;
    return td;
}
#endif

/**
 * consumer_threadproc()
 * consumer thread procedure. derived from APR queue test.
 */
static void consumer_threadproc(void *data)
{
    etch_thread_params* params = (etch_thread_params*) data;
    etch_queue* queue = (etch_queue*) params->data; 

    int result = 0; 
    etch_int32* content = NULL;    
    int64 sleeprate = 1000000/CONSUMER_ACTIVITY;
    next_consumer();

    if (!is_etch_queue(queue)) /* can't test assert outside main thread */
    {          
        printf("consumer_threadproc data is not etch_queue\n");      
        g_error = ERROR_BAD_THREADDATA;
        return;
    }  

    #if IS_DEBUG_CONSOLE
    printf("start consumer thread %d\n", thisthread); fflush(stdout);
    #endif

    etch_sleep((rand() % 4) * 1000);  /* sleep random seconds */

    while(-1 != result)  /* until shutdown ... */
    {
        g_consumercount++;
                               
        switch(result = etchqueue_get(queue, (void**)&content))
        {
            case -1:   
                 break;     /* shutdown or error */

            case ETCH_QUEUE_OPERATION_CANCELED: 
                 continue;  /* signaled */

            case ETCH_QUEUE_OPERATION_TIMEOUT:
            default:        /* timed out (try again) or success */

                 if (content)
                 {   
                     #if IS_DEBUG_CONSOLE
                     printf("consumer thread %d popped content %d\n", thisthread, content->value); fflush(stdout);
                     #endif
                     etch_object_destroy(content);
                     content = NULL;
                 }
                 apr_sleep(sleeprate); /* sleep this long to achieve our rate */
        }
    }

    #if IS_DEBUG_CONSOLE
    printf("consumer thread %d exit\n", thisthread); fflush(stdout);
    #endif
}


/**
 * producer_threadproc()
 * producer thread procedure. derived from APR queue test.
 */
static void producer_threadproc(void *data)
{
    etch_thread_params* params = (etch_thread_params*) data;
    etch_queue* queue = (etch_queue*) params->data; 

    int result = 0, is_timeout = 0;
    etch_int32* newcontent = NULL;
    const int thisthread = next_producer();
    int64 sleeprate = 1000000/PRODUCER_ACTIVITY;

    if (!is_etch_queue(queue)) /* can't test assert outside main thread */
    {   printf("producer_threadproc data is not etch_queue\n");
        g_error = ERROR_BAD_THREADDATA;
        return;
    } 

    #if IS_DEBUG_CONSOLE
    printf("start producer thread %d\n", thisthread); fflush(stdout); 
    #endif

    etch_sleep((rand() % 4) * 1000); /* sleep random seconds */

    while(-1 != result)  /* until shutdown ... */
    {
        g_producercount++;   

        if  (is_timeout)
        {    
             #ifdef IS_DEBUG_CONSOLE
             printf("producer thread %d re-pushing content %d\n", thisthread, newcontent->value); fflush(stdout);
             #endif
             is_timeout = FALSE;
        }
        else 
        {   newcontent = new_int32(next_test_id());  
            #if IS_DEBUG_CONSOLE
            printf("producer thread %d pushing content %d\n", thisthread, newcontent->value); fflush(stdout);
            #endif
        }

        switch(result = etchqueue_put(queue, newcontent))
        {
            case -1:  /* queue closed or error */ 
                 etch_object_destroy(newcontent); 
                 newcontent = NULL;
                 break;

            case ETCH_QUEUE_OPERATION_CANCELED: 
                 etch_object_destroy(newcontent); 
                 newcontent = NULL;
                 continue; /* signaled */

            case ETCH_QUEUE_OPERATION_TIMEOUT:
                 is_timeout = TRUE;     /* fall thru ... */

            default:       /* timeout or success */
                 apr_sleep(sleeprate);   
        }
    }

    #if IS_DEBUG_CONSOLE
    printf("producer thread %d exit\n", thisthread); fflush(stdout);
    #endif
}


/* - - - - - - - - - - - - - - 
 * unit tests
 * - - - - - - - - - - - - - -
 */

/**
 * test_constructor
 */
static void test_constructor(void)
{
    etch_queue* q = new_queue(ETCH_DEFSIZE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(q); 

    etch_object_destroy(q);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_putget()
 */
static void test_putget(void)
{
    int result = 0, qdepth = 0;
    etch_int32* thiscontent = NULL;
    etch_int32* thiscontent1 = new_int32(1);
    etch_int32* thiscontent2 = new_int32(2);
    etch_queue* q = new_queue(ETCH_DEFSIZE);
     
    result = etchqueue_put(q, thiscontent1);
    CU_ASSERT_EQUAL(result,0);

    qdepth = etchqueue_size(q);
    CU_ASSERT_EQUAL(qdepth,1);

    result = etchqueue_put(q, thiscontent2);
    CU_ASSERT_EQUAL(result,0);

    qdepth = etchqueue_size(q);
    CU_ASSERT_EQUAL(qdepth,2);

    result = etchqueue_get(q, (void**)&thiscontent);
    CU_ASSERT_EQUAL(result,0);
    result = is_etch_int32(thiscontent);
    CU_ASSERT_EQUAL(result, TRUE);
    CU_ASSERT_EQUAL(thiscontent->value, thiscontent1->value);

    qdepth = etchqueue_size(q);
    CU_ASSERT_EQUAL(qdepth,1);

    result = etchqueue_get(q, (void**)&thiscontent);
    CU_ASSERT_EQUAL(result,0);
    result = is_etch_int32(thiscontent);
    CU_ASSERT_EQUAL(result, TRUE);
    CU_ASSERT_EQUAL(thiscontent->value, thiscontent2->value);

    qdepth = etchqueue_size(q);
    CU_ASSERT_EQUAL(qdepth,0);

    etch_object_destroy(q);
    etch_object_destroy(thiscontent1);
    etch_object_destroy(thiscontent2);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_bulkputget()
 */
static void test_bulkputget(void)
{
    int result = 0, qdepth = 0;
    const int HOWMANY = 3;
    etch_queue* q = new_queue(HOWMANY);
     
    result = push_some_objects(q, HOWMANY);
    CU_ASSERT_EQUAL(result,0);

    qdepth = etchqueue_size(q);
    CU_ASSERT_EQUAL(qdepth, HOWMANY);

    result = pop_some_objects(q, HOWMANY);
    CU_ASSERT_EQUAL(result,0);

    qdepth = etchqueue_size(q);
    CU_ASSERT_EQUAL(qdepth, 0);

    etch_object_destroy(q);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_destroy_nonempty_queue()
 * destroy a nonempty queue and ensure content memory is freed as expected
 */
static void test_destroy_nonempty_queue(void)
{
    int result = 0;
    const int HOWMANY = 3;
    etch_queue* q = new_queue(HOWMANY);
     
    result = push_some_objects(q, HOWMANY);

    etch_object_destroy(q);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_queue_overflow()
 */
static void test_queue_overflow(void)
{
    int result = 0, qdepth = 0;
    const int MAXDEPTH = 2;
    etch_int32* newcontent = NULL;
    etch_queue* q = new_queue(MAXDEPTH);

    result = etchqueue_put_withwait(q, ETCH_NOWAIT, newcontent = new_int32(0));
    CU_ASSERT_EQUAL(result, 0);
     
    result = etchqueue_put_withwait(q, ETCH_NOWAIT, newcontent = new_int32(0));
    CU_ASSERT_EQUAL(result, 0);

    result = etchqueue_try_put(q, newcontent = new_int32(0));
    CU_ASSERT_EQUAL(result, -1);

    result = etchqueue_put_withwait(q, ETCH_NOWAIT, newcontent);
    CU_ASSERT_NOT_EQUAL_FATAL(result, 0);
    etch_object_destroy(newcontent);

    qdepth = etchqueue_size(q);
    CU_ASSERT_EQUAL(qdepth, MAXDEPTH);

    etch_object_destroy(q);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_queue_underflow()
 */
static void test_queue_underflow(void)
{
    int result = 0, qdepth = 0;
    const int MAXDEPTH = 2;
    etch_int32* content = NULL;
    etch_queue* q = new_queue(MAXDEPTH);

    result = etchqueue_put_withwait(q, ETCH_NOWAIT, content = new_int32(0));
    CU_ASSERT_EQUAL(result, 0);
     
    result = etchqueue_get_withwait(q, ETCH_NOWAIT, (void**)&content);
    CU_ASSERT_EQUAL(result, 0);
    etch_object_destroy(content);
    content = NULL;

    result = etchqueue_try_get(q, (void**)&content);
    CU_ASSERT_EQUAL(result, -1);

    result = etchqueue_get_withwait(q, ETCH_NOWAIT, (void**)&content);
    CU_ASSERT_NOT_EQUAL_FATAL(result, 0);

    qdepth = etchqueue_size(q);
    CU_ASSERT_EQUAL(qdepth, 0);

    etch_object_destroy(q);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_iterator()
 */
static void test_iterator(void)
{
    etch_iterator iterator;
    int  iterated = 0;
    const int HOWMANY = 3;

    etch_queue* q = new_queue(ETCH_DEFSIZE);

    push_some_objects(q, HOWMANY);

    set_iterator(&iterator, q, &q->iterable);

    while(iterator.has_next(&iterator))
    {
        etch_int32* content = (etch_int32*) iterator.current_value;
        CU_ASSERT_EQUAL_FATAL(is_etch_int32(content), TRUE);
        CU_ASSERT_EQUAL(content->value, iterated++);
        
        iterator.next(&iterator);
    }  

    CU_ASSERT_EQUAL(iterated, HOWMANY);  

    etch_object_destroy(q);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/**
 * test_blocking()
 * derived from apr test_queue.
 * launch some number of threads which blocking write etch objects to a queue, 
 * and some number of threads which blocking read from the same queue. let the 
 * threads  do their thing for a while, and close the queue. verify that all 
 * memory is accounted for.
 */
static void test_blocking(void)
{
    #define NUMTHREADS NUMBER_CONSUMERS + NUMBER_PRODUCERS
    int result = 0, i = 0;
    etch_queue* myqueue = NULL;

    etch_threadpool* threadpool = new_threadpool(ETCH_THREADPOOLTYPE_FREE, ETCH_DEFSIZE);
    threadpool->is_free_data = FALSE;   /* thread data will be our queue */

    myqueue = new_queue(ETCH_DEFSIZE);  /* create the single queue */
    srand((unsigned int)apr_time_now());  
    g_error = g_producercount = g_consumercount = g_unique_id = g_pro = g_con = 0;

    for(i=0; i < NUMBER_CONSUMERS; i++) /* launch all consumer threads */
    {   void* newthread = threadpool->run(threadpool, consumer_threadproc, myqueue);
        CU_ASSERT_PTR_NOT_NULL(newthread);
    }

    for(i=0; i < NUMBER_PRODUCERS; i++) /* launch all producer threads */
    {   void* newthread = threadpool->run(threadpool, producer_threadproc, myqueue);
        CU_ASSERT_PTR_NOT_NULL(newthread);
    }

    etch_sleep(3000);  /* pause long enough to let the threads do some work */ 

    result = etchqueue_close(myqueue, TRUE);  /* mark queue closed and notify all waiters */

    /* pause to observe waiter threads unblocking  
       etch_sleep(3000);  
     */

    #ifdef TEST_BLOCKING_PRECLEAR
    while(1)
    {   /* when the queue is closed it clears the queue similarly to this. 
         * this code is here to verify clearing the queue prior to closing it.  
         */
        etch_int32* queuecontent = NULL;
        result = etchqueue_get_withwait(myqueue, ETCHQUEUE_CLEARING_CLOSED_QUEUE, &queuecontent);
        CU_ASSERT_EQUAL(result, -1);  
        if (-1 == result) break;
        etch_object_destroy(queuecontent);
    } 
    #endif 

    /* note that we should always destroy (or otherwise join) the queue  
     * accessor threads, before destroying the queue, as we do here */
    etch_object_destroy(threadpool);   

    etch_object_destroy(myqueue);

#ifdef ETCH_DEBUGALLOC
   g_bytes_allocated = etch_showmem(0,IS_DEBUG_CONSOLE);  /* verify all memory freed */
   CU_ASSERT_EQUAL(g_bytes_allocated, 0);
   // start fresh for next test
   memtable_clear();
#endif
}


/* - - - - - - - - - - - - - - - - - -
 * APR queue test with APR threadpool
 * - - - - - - - - - - - - - - - - - -
 */

static etch_apr_queue_t* g_queue; /* global queue for APR queue test */


/**
 * aprqtest_consumer_thread()
 */
static void *APR_THREAD_FUNC aprqtest_consumer_thread(apr_thread_t *thd, void *data)
{
    long sleeprate;
    apr_status_t rv;
    void *v;

    sleeprate = 1000000/CONSUMER_ACTIVITY;
    apr_sleep((rand() % 4) * 1000000); /* sleep random seconds */

    while (1)
    {   
        rv = etch_apr_queue_pop(g_queue, 0, &v);

        if (rv == APR_EINTR)
            continue;
        if (rv == APR_EOF)
            break;
        apr_sleep(sleeprate); /* sleep this long to acheive our rate */
    }

    return NULL;
}

 
/**
 * aprqtest_producer_thread()
 */
static void * APR_THREAD_FUNC aprqtest_producer_thread(apr_thread_t *thd, void *data)
{
    long sleeprate;
    apr_status_t rv;

    sleeprate = 1000000/PRODUCER_ACTIVITY;
    apr_sleep((rand() % 4) * 1000000); /* sleep random seconds */

    while (1)
    {
        rv = etch_apr_queue_push(g_queue, 0, NULL);

        if (rv == APR_EINTR)
            continue;
        if (rv == APR_EOF)
            break;
        apr_sleep(sleeprate); /* sleep this long to acheive our rate */
    }

    return NULL;
}
 

/**
 * test_queue_producer_consumer()
 * this is the APR queue test modified to use etch_apr_queue calls.
 * it uses the APR threadpool 
 */
static void test_queue_producer_consumer(void)
{   
    unsigned int i;
    apr_status_t rv;
    apr_thread_pool_t *threadpool;
    const int QUEUE_SIZE = 32, ZEROPRIORITY = 0;
    void* NULLPARAM = NULL, *NULLOWNER = NULL;
    srand((unsigned int)apr_time_now());
    g_error = g_producercount = g_consumercount = g_unique_id = g_pro = g_con = 0;

    rv = etch_apr_queue_create(&g_queue, QUEUE_SIZE, g_etch_main_pool);
    CU_ASSERT_EQUAL(rv,0);

    rv = etch_apr_thread_pool_create(&threadpool, 0, NUMBER_CONSUMERS + NUMBER_PRODUCERS, g_etch_main_pool);
    CU_ASSERT_EQUAL(rv,0);

    for (i = 0; i < NUMBER_CONSUMERS; i++) 
    {
        rv = etch_apr_thread_pool_push(threadpool, aprqtest_consumer_thread, NULLPARAM, ZEROPRIORITY, NULLOWNER);
        CU_ASSERT_EQUAL(rv,0);
    }

    for (i = 0; i < NUMBER_PRODUCERS; i++) 
    {
        rv = etch_apr_thread_pool_push(threadpool, aprqtest_producer_thread, NULLPARAM, ZEROPRIORITY, NULLOWNER);
        CU_ASSERT_EQUAL(rv,0);
    }

    etch_sleep(5000); /* let threads work for a bit */

    rv = etch_apr_queue_term(g_queue);
    CU_ASSERT_EQUAL(rv,0);

    /* pause to observe waiter threads unblocking  
       etch_sleep(3000);  
     */

    rv = etch_apr_thread_pool_destroy(threadpool);
    CU_ASSERT_EQUAL(rv,0);;
}


/**
 * main   
 */
//int wmain( int argc, wchar_t* argv[], wchar_t* envp[])
CU_pSuite test_etch_queue()
{    
    CU_pSuite pSuite = CU_add_suite("queue test suite", init_suite, clean_suite);

    CU_add_test(pSuite, "test constructor", test_constructor); 
    CU_add_test(pSuite, "test put and get", test_putget);   
    CU_add_test(pSuite, "test bulk put and get", test_bulkputget);   
    CU_add_test(pSuite, "test destroy nonempty", test_destroy_nonempty_queue);   
    CU_add_test(pSuite, "test overflow",  test_queue_overflow);   
    CU_add_test(pSuite, "test underflow", test_queue_underflow);
    CU_add_test(pSuite, "test iterator",  test_iterator);
    CU_add_test(pSuite, "test blocking", test_blocking);
    CU_add_test(pSuite, "test producer-consumer (APR)", test_queue_producer_consumer);

    return pSuite;
}
