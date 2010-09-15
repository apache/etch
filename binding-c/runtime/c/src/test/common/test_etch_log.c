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
#include "etch_log.h"

#include <stdio.h>
#include <wchar.h>
#include "CUnit.h"

static const char* TEST_ETCH_LOG_CATEGORY = "test_etch_config";

/* - - - - - - - - - - - - - - 
 * unit test infrastructure
 * - - - - - - - - - - - - - -
 */

static int init_suite(void)
{
    return 0;
}

static int clean_suite(void)
{
    return 0;
}

static etch_status_t etch_log_appender_test_create(void** appender, etch_config_t* config)
{
    *appender = NULL;
    return ETCH_SUCCESS;
}

static etch_status_t etch_log_appender_test_open(void* appender)
{
    //printf("etch_log_appender_test_open\n");
    return ETCH_SUCCESS;
}

static etch_status_t etch_log_appender_test_log(void* appender, etch_log_message_t* message)
{
    //printf("etch_log_appender_test_log\n");
    return ETCH_SUCCESS;
}

static etch_status_t etch_log_appender_test_close(void* appender)
{
    //printf("etch_log_appender_test_close\n");
    return ETCH_SUCCESS;
}

static etch_status_t etch_log_appender_test_destroy(void* appender)
{
    return ETCH_SUCCESS;
}

static struct etch_log_appender_desc etch_log_appender_test_desc = {
    etch_log_appender_test_create,
    etch_log_appender_test_open,
    etch_log_appender_test_log,
    etch_log_appender_test_close,
    etch_log_appender_test_destroy
};


//**********************
// START testcases here
//**********************

static void test_etch_log_register_appender(void)
{
    etch_status_t etch_status = ETCH_SUCCESS;
    
    etch_status = etch_log_register_appender("testappender", &etch_log_appender_test_desc);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

}

static void test_etch_log_create_destroy(void)
{
    etch_status_t   etch_status = ETCH_SUCCESS;
    etch_config_t*  config = NULL;
    etch_log_t*     logger = NULL;

    etch_status = etch_config_create(&config);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_status = etch_log_create(&logger, config);
    CU_ASSERT_PTR_NOT_NULL(logger);

    etch_log_destroy(logger);
    etch_config_destroy(config);
}

static void test_etch_log_log(void)
{
    etch_status_t   etch_status = ETCH_SUCCESS;
    etch_config_t*  config = NULL;

    etch_status = etch_config_create(&config);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_status = etch_runtime_initialize(config);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    ETCH_LOG(TEST_ETCH_LOG_CATEGORY, ETCH_LOG_ERROR, "test log message");
    ETCH_LOG(TEST_ETCH_LOG_CATEGORY, ETCH_LOG_XDEBUG, "test log message %s", "test1");
    ETCH_LOG(TEST_ETCH_LOG_CATEGORY, ETCH_LOG_INFO, "test log message %s", "test2");

    etch_runtime_shutdown();
    etch_config_destroy(config);
}

//**********************
// END testcases here
//**********************

CU_pSuite test_etch_log_suite()
{
    CU_pSuite ps = CU_add_suite("etch_log test suite", init_suite, clean_suite);
    
    CU_add_test(ps, "test etch_log_register_appender", test_etch_log_register_appender);
    CU_add_test(ps, "test etch_log_create_destroy", test_etch_log_create_destroy);
    CU_add_test(ps, "test etch_log_log", test_etch_log_log);

    return ps;
}
