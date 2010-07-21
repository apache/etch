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
 * test_etch_wait.c -- test etch wait
 */
#include "etch_runtime.h"
#include "etch_wait.h"

#include "apr_thread_proc.h"

#include <stdio.h>
#include <wchar.h>
#include "CUnit.h"

#define IS_DEBUG_CONSOLE FALSE

// extern types
extern apr_pool_t* g_etch_main_pool;

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
    //this_teardown();
    etch_runtime_shutdown();
    return 0;
}

//**********************
// START testcases here
//**********************

typedef struct apr_thread_data_t
{
    etch_wait_t* waiter;
    uint16       mode;
    uint16       rv;
} apr_thread_data_t;

static void* APR_THREAD_FUNC thread_func1(apr_thread_t *thd, void *data)
{
    apr_status_t       status;
    apr_thread_data_t* thd_data = data;
    clock_t            start    = 0;
    clock_t            end      = 0;
    clock_t            diff     = 0;

    switch(thd_data->mode) {
        case 0:
            start = clock();
            status = etch_wait_timedwait(thd_data->waiter, 4, 400);
            CU_ASSERT_EQUAL(status, ETCH_ETIMEOUT);
            end = clock();
            diff = end - start;
            break;
        case 1:
            start = clock();
            status = etch_wait_timedwait(thd_data->waiter, 2, 500);
            CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
            end = clock();
            diff = end - start;
            break;
    }

    apr_thread_exit(thd, 0);
    return NULL;
}

static void test_etch_wait_create_destroy(void)
{
    etch_status_t status = ETCH_SUCCESS;
    etch_wait_t*  waiter = NULL;

    status = etch_wait_create(&waiter, g_etch_main_pool);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_wait_destroy(waiter);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
}

static void test_etch_wait_timedwait(void)
{
    etch_status_t status     = ETCH_SUCCESS;
    etch_wait_t*  waiter     = NULL;

    
    apr_status_t      apr_status;
    apr_status_t      apr_thread_status;
    apr_thread_data_t apr_thread_data;
    apr_thread_t*     apr_thread = NULL;

    status = etch_wait_create(&waiter, g_etch_main_pool);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_wait_timedwait(waiter, 2, 100);
    CU_ASSERT_EQUAL(status, ETCH_ETIMEOUT);

    // timeout / signal test
    apr_thread_data.waiter = waiter;
    apr_thread_data.mode   = 0;

    apr_status = apr_thread_create(&apr_thread, NULL, thread_func1, &apr_thread_data, g_etch_main_pool);
    CU_ASSERT_EQUAL(apr_status, APR_SUCCESS);

    apr_sleep(200);
    etch_wait_set(waiter, 1);

    apr_status = apr_thread_join(&apr_thread_status, apr_thread);
    CU_ASSERT_EQUAL(apr_status, APR_SUCCESS);

    // timeout / signal test
    apr_thread_data.waiter = waiter;
    apr_thread_data.mode   = 1;

    apr_status = apr_thread_create(&apr_thread, NULL, thread_func1, &apr_thread_data, g_etch_main_pool);
    CU_ASSERT_EQUAL(apr_status, APR_SUCCESS);
    
    etch_wait_set(waiter, 2);

    apr_status = apr_thread_join(&apr_thread_status, apr_thread);
    CU_ASSERT_EQUAL(apr_status, APR_SUCCESS);

    status = etch_wait_destroy(waiter);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
}

//**********************
// END testcases here
//**********************

/**
 * main   
 */
CU_pSuite test_etch_wait_suite()
{
    CU_pSuite ps = CU_add_suite("etch_wait", init_suite, clean_suite);

    CU_add_test(ps, "test test_etch_wait_create_destroy", test_etch_wait_create_destroy);
    CU_add_test(ps, "test test_etch_wait_timedwait", test_etch_wait_timedwait);

    return ps;
}
