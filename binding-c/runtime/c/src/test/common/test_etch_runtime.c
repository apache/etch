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
 * test_runtime.c -- test etch runtime
 */
#include "etch_runtime.h"
#include "etch_config.h"
#include "etch_mem.h"

#include <stdio.h>
#include <wchar.h>
#include "CUnit.h"

#define IS_DEBUG_CONSOLE FALSE

// extern types
extern apr_pool_t* g_etch_main_pool;

static int init_suite(void)
{
    return 0;
}

static int clean_suite(void)
{
    return 0;
}

//**********************
// START testcases here
//**********************

static void test_etch_runtime_get_version(void)
{
    etch_status_t etch_status = ETCH_SUCCESS;
    uint16        major       = 0;
    uint16        minor       = 0;
    uint16        revision    = 0;

    // initialize runtime
    etch_status = etch_runtime_get_version(NULL, NULL, NULL);
    CU_ASSERT_EQUAL(etch_status, ETCH_EINVAL);

    etch_status = etch_runtime_get_version(&major, &minor, &revision);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);
}


static void test_etch_runtime_initialize_shutdown(void)
{
    etch_status_t etch_status = ETCH_SUCCESS;
    // initialize runtime
    etch_status = etch_runtime_initialize(NULL);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);
    // check apr pool
    CU_ASSERT_PTR_NOT_NULL(g_etch_main_pool);
    // shutdown runtime
    etch_runtime_shutdown();
    // check apr pool
    CU_ASSERT_PTR_NULL(g_etch_main_pool);
}

static void test_etch_runtime_get_config(void)
{
    etch_status_t etch_status  = ETCH_SUCCESS;
    etch_config_t *config      = NULL;
    etch_config_t *configtemp  = NULL;

    etch_status = etch_config_create(&config);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_status = etch_runtime_initialize(config);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_status = etch_runtime_get_config(&configtemp);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(configtemp);

    // shutdown runtime
    etch_runtime_shutdown();

    etch_config_destroy(config);
}

static void test_etch_runtime_multiple_times(void)
{
    etch_status_t etch_status_runtime  = ETCH_SUCCESS;
    etch_status_t etch_status_shutdown  = ETCH_SUCCESS;
    int i=0;
    
    for (i=0; i<5; i++)
    {
        etch_status_runtime = etch_runtime_initialize(NULL);
        CU_ASSERT_EQUAL(etch_status_runtime, ETCH_SUCCESS);

        // shutdown runtime
        etch_status_shutdown = etch_runtime_shutdown();
        CU_ASSERT_EQUAL(etch_status_shutdown, ETCH_SUCCESS);
    }
}


static void test_etch_runtime_config_multiple_times(void)
{
    etch_status_t etch_status_config  = ETCH_SUCCESS;
    etch_status_t etch_status_runtime  = ETCH_SUCCESS;
    etch_status_t etch_status_shutdown  = ETCH_SUCCESS;
    etch_config_t *config;
    etch_config_t *configtemp  = NULL;

    int i=0;
    
    for (i=0; i<5; i++)
    {
        etch_status_config = etch_config_create(&config);
        CU_ASSERT_EQUAL(etch_status_config, ETCH_SUCCESS);

        etch_status_runtime = etch_runtime_initialize(config);
        CU_ASSERT_EQUAL(etch_status_runtime, ETCH_SUCCESS);

        etch_status_config = etch_runtime_get_config(&configtemp);
        CU_ASSERT_EQUAL(etch_status_config, ETCH_SUCCESS);
        CU_ASSERT_PTR_NOT_NULL(configtemp);

        // shutdown runtime
        etch_status_shutdown = etch_runtime_shutdown();
        CU_ASSERT_EQUAL(etch_status_shutdown, ETCH_SUCCESS);

        etch_status_config = etch_config_destroy(config);
        CU_ASSERT_EQUAL(etch_status_config, ETCH_SUCCESS);
    }
}

static void test_etch_runtime_initialize_shutdown_refcount(void)
{
    etch_status_t  status = ETCH_SUCCESS;

    /* 1 initialisation */
    status = etch_runtime_initialize(NULL);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    /* 2 initialisation */
    status = etch_runtime_initialize(NULL);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    /* 3 initialisation */
    status = etch_runtime_initialize(NULL);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    
    // check apr pool
    CU_ASSERT_PTR_NOT_NULL(g_etch_main_pool);

    /* 1 shutdown */
    status = etch_runtime_shutdown();
    /* 2 shutdown */
    status = etch_runtime_shutdown();
    /* 3 shutdown */
    status = etch_runtime_shutdown();

    // check apr pool
    CU_ASSERT_PTR_NULL(g_etch_main_pool);
}


//**********************
// END testcases here
//**********************

/**
 * main
 */
CU_pSuite test_etch_runtime_suite(int argc, char** argv)
{
    CU_pSuite ps = CU_add_suite("etch_runtime", init_suite, clean_suite);
    
    CU_add_test(ps, "test_etch_runtime_get_version", test_etch_runtime_get_version);
    CU_add_test(ps, "test_etch_runtime_initialize_shutdown", test_etch_runtime_initialize_shutdown);
    CU_add_test(ps, "test_etch_runtime_get_config", test_etch_runtime_get_config);
    CU_add_test(ps, "test_etch_runtime_multiple_times", test_etch_runtime_multiple_times);
    CU_add_test(ps, "test_etch_runtime_config_multiple_times", test_etch_runtime_config_multiple_times);
    CU_add_test(ps, "test_etch_runtime_initialize_shutdown_refcount", test_etch_runtime_initialize_shutdown_refcount);

    return ps;
}
