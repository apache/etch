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

#include "apr_time.h" /* some apr must be included first */
#include "etch_queue.h"  

#include <tchar.h>
#include <stdio.h>
#include <conio.h>

#include "cunit.h"
#include "basic.h"
#include "automated.h"

#include "etchthread.h"
#include "etch_global.h"

#include "etch_threadpool_apr.h"  

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


/* - - - - - - - - - - - - - - 
 * unit test support
 * - - - - - - - - - - - - - -
 */

#define NUMBER_CONSUMERS     2  
#define CONSUMER_ACTIVITY    4
#define NUMBER_PRODUCERS     3  
#define PRODUCER_ACTIVITY    5
#define ERROR_BAD_THREADDATA 1
int g_error, g_producercount, g_consumercount, g_unique_id, g_pro, g_con;


unsigned next_test_id()
{
    apr_atomic_inc32 ((volatile apr_uint32_t*) &g_unique_id);
    return g_unique_id;
}

unsigned next_producer()
{
    apr_atomic_inc32 ((volatile apr_uint32_t*) &g_pro);
    return g_pro;
}

unsigned next_consumer()
{
    apr_atomic_inc32 ((volatile apr_uint32_t*) &g_con);
    return g_con;
}

 
/**
 * push_some_objects()
 * push specified number of new objects onto the queue
 */
int push_some_objects(etch_queue* q, const int howmany)
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
int pop_some_objects(etch_queue* q, const int howmany)
{
    int count = 0, result = 0;
    objmask* thisobj = NULL;

    while(count++ < howmany && result == 0)
    {
        result = etchqueue_get_withwait(q, ETCH_NOWAIT, &thisobj);
        if (thisobj) 
            thisobj->destroy(thisobj);
        thisobj = NULL;
    }

    return result;
}


/**
 * pop_all_objects()
 * pop all objects off the queue and destroy each object retrieved
 */
int pop_all_objects(etch_queue* q)
{
    int popcount = 0, result = 0;
    const int queuecount = etchqueue_size(q);
    objmask* thisobj = NULL;

    while(popcount++ < queuecount && result == 0)
    {
        result = etchqueue_get_withwait(q, ETCH_NOWAIT, &thisobj);
        if (thisobj) 
            thisobj->destroy(thisobj);
        thisobj = NULL;
    }

    return result;
}


/**
 * mytest_threaddata()
 * thread data for our test threads. we could pass the queue directly, but then
 * we'd have to configure the threadpool to not destroy a thread's data.
 */
typedef struct mytest_threaddata  { etch_queue* queue; }  mytest_threaddata;
 
/**
 * new_testdata()
 * constructor for test thread data
 */
mytest_threaddata* new_testdata(etch_queue* q)
{
    mytest_threaddata* td = etch_malloc(sizeof(mytest_threaddata), 0);
    td->queue = q;
    return td;
}


/**
 * consumer_threadproc()
 * consumer thread procedure. derived from APR queue test.
 */
void consumer_threadproc(void *data)
{
    etch_threadparams* params = (etch_threadparams*) data;
    etch_queue* queue = (etch_queue*) params->data; 

    int result = 0; 
    const int thisthread = next_consumer();
    etch_int32* content = NULL;    
    int64 sleeprate = 1000000/CONSUMER_ACTIVITY;

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
                               
        switch(result = etchqueue_get(queue, &content))
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
                     content->destroy(content);
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
void producer_threadproc(void *data)
{
    etch_threadparams* params = (etch_threadparams*) data;
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
                 newcontent->destroy(newcontent); 
                 newcontent = NULL;
                 break;

            case ETCH_QUEUE_OPERATION_CANCELED: 
                 newcontent->destroy(newcontent); 
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
void test_constructor(void)
{
    etch_queue* q = new_queue(ETCH_DEFSIZE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(q); 

    q->destroy(q);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_putget()
 */
void test_putget(void)
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

    result = etchqueue_get(q, &thiscontent);
    CU_ASSERT_EQUAL(result,0);
    result = is_etch_int32(thiscontent);
    CU_ASSERT_EQUAL(result, TRUE);
    CU_ASSERT_EQUAL(thiscontent->value, thiscontent1->value);

    qdepth = etchqueue_size(q);
    CU_ASSERT_EQUAL(qdepth,1);

    result = etchqueue_get(q, &thiscontent);
    CU_ASSERT_EQUAL(result,0);
    result = is_etch_int32(thiscontent);
    CU_ASSERT_EQUAL(result, TRUE);
    CU_ASSERT_EQUAL(thiscontent->value, thiscontent2->value);

    qdepth = etchqueue_size(q);
    CU_ASSERT_EQUAL(qdepth,0);

    q->destroy(q);
    thiscontent1->destroy(thiscontent1);
    thiscontent2->destroy(thiscontent2);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_bulkputget()
 */
void test_bulkputget(void)
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

    q->destroy(q);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_destroy_nonempty_queue()
 * destroy a nonempty queue and ensure content memory is freed as expected
 */
void test_destroy_nonempty_queue(void)
{
    int result = 0, qdepth = 0;
    const int HOWMANY = 3;
    etch_queue* q = new_queue(HOWMANY);
     
    result = push_some_objects(q, HOWMANY);

    q->destroy(q);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_queue_overflow()
 */
void test_queue_overflow(void)
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
    newcontent->destroy(newcontent);

    qdepth = etchqueue_size(q);
    CU_ASSERT_EQUAL(qdepth, MAXDEPTH);

    q->destroy(q);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_queue_underflow()
 */
void test_queue_underflow(void)
{
    int result = 0, qdepth = 0;
    const int MAXDEPTH = 2;
    etch_int32* content = NULL;
    etch_queue* q = new_queue(MAXDEPTH);

    result = etchqueue_put_withwait(q, ETCH_NOWAIT, content = new_int32(0));
    CU_ASSERT_EQUAL(result, 0);
     
    result = etchqueue_get_withwait(q, ETCH_NOWAIT, &content);
    CU_ASSERT_EQUAL(result, 0);
    content->destroy(content);
    content = NULL;

    result = etchqueue_try_get(q, &content);
    CU_ASSERT_EQUAL(result, -1);

    result = etchqueue_get_withwait(q, ETCH_NOWAIT, &content);
    CU_ASSERT_NOT_EQUAL_FATAL(result, 0);

    qdepth = etchqueue_size(q);
    CU_ASSERT_EQUAL(qdepth, 0);

    q->destroy(q);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_iterator()
 */
void test_iterator(void)
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

    q->destroy(q);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/**
 * test_blocking()
 * derived from apr test_queue.
 * launch some number of threads which blocking write etch objects to a queue, 
 * and some number of threads which blocking read from the same queue. let the 
 * threads  do their thing for a while, and close the queue. verify that all 
 * memory is accounted for.
 */
void test_blocking(void)
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
        queuecontent->destroy(queuecontent);
    } 
    #endif 

    /* note that we should always destroy (or otherwise join) the queue  
     * accessor threads, before destroying the queue, as we do here */
    threadpool->destroy(threadpool);   

    myqueue->destroy(myqueue);

    g_bytes_allocated = etch_showmem(0, IS_DEBUG_CONSOLE); /* verify all memory freed */
    CU_ASSERT_EQUAL(g_bytes_allocated, 0);  
    memtable_clear();  /* start fresh for next test */   
}


/* - - - - - - - - - - - - - - - - - -
 * APR queue test with APR threadpool
 * - - - - - - - - - - - - - - - - - -
 */

etch_apr_queue_t* g_queue; /* global queue for APR queue test */


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
void test_queue_producer_consumer(void)
{   
    unsigned int i;
    apr_status_t rv;
    apr_thread_pool_t *threadpool;
    const int QUEUE_SIZE = 32, ZEROPRIORITY = 0;
    void* NULLPARAM = NULL, *NULLOWNER = NULL;
    srand((unsigned int)apr_time_now());
    g_error = g_producercount = g_consumercount = g_unique_id = g_pro = g_con = 0;

    rv = etch_apr_queue_create(&g_queue, QUEUE_SIZE, etch_apr_mempool);
    CU_ASSERT_EQUAL(rv,0);

    rv = etch_apr_thread_pool_create(&threadpool, 0, NUMBER_CONSUMERS + NUMBER_PRODUCERS, etch_apr_mempool);
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
int _tmain(int argc, _TCHAR* argv[])
{    
    char c=0;
    CU_pSuite pSuite = NULL;
    g_is_automated_test = argc > 1 && 0 != wcscmp(argv[1], L"-a");
    if (CUE_SUCCESS != CU_initialize_registry()) return 0;
    pSuite = CU_add_suite("queue test suite", init_suite, clean_suite);
    CU_set_output_filename("../test_queue");
    etch_watch_id = 0; 

    CU_add_test(pSuite, "test constructor", test_constructor); 
    CU_add_test(pSuite, "test put and get", test_putget);   
    CU_add_test(pSuite, "test bulk put and get", test_bulkputget);   
    CU_add_test(pSuite, "test destroy nonempty", test_destroy_nonempty_queue);   
    CU_add_test(pSuite, "test overflow",  test_queue_overflow);   
    CU_add_test(pSuite, "test underflow", test_queue_underflow);
    CU_add_test(pSuite, "test iterator",  test_iterator);
    CU_add_test(pSuite, "test blocking", test_blocking);
    CU_add_test(pSuite, "test producer-consumer (APR)", test_queue_producer_consumer);

    if (g_is_automated_test)    
        CU_automated_run_tests();    
    else
    {   CU_basic_set_mode(CU_BRM_VERBOSE);
        CU_basic_run_tests();
    }

    if (!g_is_automated_test) { printf("any key ..."); while(!c) c = _getch(); printf("\n"); }     
    CU_cleanup_registry();
    return CU_get_error(); 
}