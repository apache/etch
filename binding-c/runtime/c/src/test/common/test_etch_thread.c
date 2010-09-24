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
 * test_etch_threadpool.c -- test etch threadpool
 */
#include "etch_runtime.h"
#include "etch_thread2.h"
#include "etch_mem.h"

#include "CUnit.h"
#include <stdio.h>
#include <wchar.h>

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

static void test_etch_thread_create_destroy(void)
{
    //status = etch_thread_create(&thread, 



    //status = etch_threadpool_create(&threadpool, ETCH_THREADPOOL_TYPE_FREE, 5);
    //CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    //status = etch_threadpool_destroy(threadpool);
    //CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
}

//**********************
// END testcases here
//**********************

/**
 * main   
 */
CU_pSuite test_etch_thread_suite()
{
    CU_pSuite ps = CU_add_suite("etch_thread", init_suite, clean_suite);

    CU_add_test(ps, "test etch_thread_create_destroy", test_etch_thread_create_destroy);
    
    return ps;
}
