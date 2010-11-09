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
 * test_mutex.c -- test etch mutex
 */
#include "etch_runtime.h"
#include "etch_mutex.h"

#include <stdio.h>
#include <wchar.h>
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
    //this_teardown();
    etch_runtime_shutdown();
    return 0;
}

//**********************
// START testcases here
//**********************

static void test_apr_mutex(void)
{
    apr_status_t        apr_status  = APR_SUCCESS;
    apr_thread_mutex_t* apr_mutex   = NULL;

    // test create, lock, unlock, destory
    // create mutex
    apr_status = apr_thread_mutex_create(&apr_mutex, APR_THREAD_MUTEX_NESTED, g_etch_main_pool);
    CU_ASSERT_EQUAL(apr_status, APR_SUCCESS);
    // lock mutex
    apr_status = apr_thread_mutex_lock(apr_mutex);
    CU_ASSERT_EQUAL(apr_status, APR_SUCCESS);
    // unlock mutex
    apr_status = apr_thread_mutex_unlock(apr_mutex);
    CU_ASSERT_EQUAL(apr_status, APR_SUCCESS);
    // destroy mutex
    apr_status = apr_thread_mutex_destroy(apr_mutex);
    CU_ASSERT_EQUAL(apr_status, APR_SUCCESS);
    apr_mutex = NULL;


    // test create, lock, lock, destory
    // create mutex
    apr_status = apr_thread_mutex_create(&apr_mutex, APR_THREAD_MUTEX_NESTED, g_etch_main_pool);
    CU_ASSERT_EQUAL(apr_status, APR_SUCCESS);
    // lock mutex first time
    apr_status = apr_thread_mutex_lock(apr_mutex);
    CU_ASSERT_EQUAL(apr_status, APR_SUCCESS);
    // lock mutex second time
    apr_status = apr_thread_mutex_lock(apr_mutex);
    CU_ASSERT_EQUAL(apr_status, APR_SUCCESS);
    // unlock mutex second time
    apr_status = apr_thread_mutex_unlock(apr_mutex);
    CU_ASSERT_EQUAL(apr_status, APR_SUCCESS);
    apr_status = apr_thread_mutex_unlock(apr_mutex);
    CU_ASSERT_EQUAL(apr_status, APR_SUCCESS);
    // destroy mutex
    apr_status = apr_thread_mutex_destroy(apr_mutex);
    CU_ASSERT_EQUAL(apr_status, APR_SUCCESS);
    if(apr_status != APR_SUCCESS) {
        char buffer[1024];
        apr_strerror(apr_status, buffer, 1024);
        printf("error: %s\n", buffer);
    }
    apr_mutex = NULL;
}

static void test_etch_mutex_unnested_create_destroy(void)
{
    etch_status_t status  = APR_SUCCESS;
    etch_mutex_t* mutex   = NULL;

    status = etch_mutex_create(&mutex, ETCH_MUTEX_UNNESTED, g_etch_main_pool);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_lock(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_trylock(mutex);
    CU_ASSERT_EQUAL(status, ETCH_EBUSY);

    status = etch_mutex_unlock(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_destroy(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

}

static void test_etch_mutex_nested_create_destroy(void)
{
    etch_status_t status  = APR_SUCCESS;
    etch_mutex_t* mutex   = NULL;

    status = etch_mutex_create(&mutex, ETCH_MUTEX_NESTED, g_etch_main_pool);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_lock(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_lock(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_trylock(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_unlock(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_unlock(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_unlock(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_destroy(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

}

static void test_etch_mutex_try_nested_create_destroy(void)
{
    etch_status_t status  = APR_SUCCESS;
    etch_mutex_t* mutex   = NULL;

    status = etch_mutex_create(&mutex, ETCH_MUTEX_NESTED, g_etch_main_pool);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_trylock(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_lock(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_unlock(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_unlock(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_mutex_destroy(mutex);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

}

//**********************
// END testcases here
//**********************

/**
 * main   
 */
CU_pSuite test_etch_mutex_suite()
{
    CU_pSuite ps = CU_add_suite("etch_mutex", init_suite, clean_suite);

    CU_add_test(ps, "apr_mutex", test_apr_mutex);
    CU_add_test(ps, "test_etch_mutex_unnested_create_destroy", test_etch_mutex_unnested_create_destroy);
    CU_add_test(ps, "test_etch_mutex_nested_create_destroy", test_etch_mutex_nested_create_destroy);
    CU_add_test(ps, "test_etch_mutex_try_nested_create_destroy", test_etch_mutex_try_nested_create_destroy);
    return ps;
}
