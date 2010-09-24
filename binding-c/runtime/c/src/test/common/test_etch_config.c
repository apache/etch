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
 * test_etch_config.c -- test etch config
 */
#include <stdio.h>
#include <wchar.h>
#include "CUnit.h"

#include "etch_runtime.h"
#include "etch_config.h"

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

//**********************
// START testcases here
//**********************

void test_etch_config_create_destroy(void)
{
    etch_status_t etch_status = ETCH_SUCCESS;
    etch_config_t* config = NULL;
    
    etch_status = etch_config_create(&config);
    CU_ASSERT_PTR_NOT_NULL(config);

    etch_config_destroy(config);
}
/*
static void test_etch_config_open(void)
{
    etch_status_t etch_status = ETCH_SUCCESS;
    etch_config_t* config = NULL;

    etch_status = etch_config_create(&config);
    CU_ASSERT_PTR_NOT_NULL(config);

    etch_status = etch_config_open(config, "C:\\etch_config.properties");
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_config_destroy(config);
}
*/
static void test_etch_config_set_property(void)
{
    etch_status_t etch_status = ETCH_SUCCESS;
    etch_config_t* config = NULL;
    char* value = NULL;

    etch_status = etch_config_create(&config);
    CU_ASSERT_PTR_NOT_NULL(config);

    etch_status = etch_config_clear(config);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_status = etch_config_set_property(NULL, NULL, NULL);
    CU_ASSERT_EQUAL(etch_status, ETCH_EINVAL);

    etch_status = etch_config_set_property(config, NULL, NULL);
    CU_ASSERT_EQUAL(etch_status, ETCH_EINVAL);

    etch_status = etch_config_set_property(config, "test.property", NULL);
    CU_ASSERT_EQUAL(etch_status, ETCH_EINVAL);

    etch_status = etch_config_set_property(config, "test.property", "value");
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_status = etch_config_set_property(config, "test.property", "new value");
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_status = etch_config_get_property_string(config, "test.property", &value);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);
    CU_ASSERT_EQUAL(strcmp(value, "new value"), 0);

    // check resize
    etch_status = etch_config_set_property(config, "test.property1", "value");
    etch_status = etch_config_set_property(config, "test.property2", "value");
    etch_status = etch_config_set_property(config, "test.property3", "value");
    etch_status = etch_config_set_property(config, "test.property4", "value");
    etch_status = etch_config_set_property(config, "test.property5", "value");
    etch_status = etch_config_set_property(config, "test.property6", "value");
    etch_status = etch_config_set_property(config, "test.property7", "value");
    etch_status = etch_config_set_property(config, "test.property8", "value");
    etch_status = etch_config_set_property(config, "test.property9", "value");
    etch_status = etch_config_set_property(config, "test.property10", "value");
    etch_status = etch_config_set_property(config, "test.property11", "value");
    CU_ASSERT_EQUAL(etch_config_get_length(config), 12);
    CU_ASSERT_EQUAL(etch_config_get_size(config), 20);

    etch_config_destroy(config);
}

static void test_etch_config_get_property_string(void)
{
    etch_status_t etch_status = ETCH_SUCCESS;
    etch_config_t* config = NULL;
    char* value1 = NULL;

    etch_status = etch_config_create(&config);
    CU_ASSERT_PTR_NOT_NULL(config);

    etch_status = etch_config_set_property(config, "test.property", "value");
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_status = etch_config_get_property_string(config, NULL, &value1);
    CU_ASSERT_EQUAL(etch_status, ETCH_EINVAL);

    etch_status = etch_config_get_property_string(config, NULL, NULL);
    CU_ASSERT_EQUAL(etch_status, ETCH_EINVAL);

    etch_status = etch_config_get_property_string(config, "test.property", &value1);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);
    CU_ASSERT_EQUAL(strcmp(value1, "value"), 0);

    etch_status = etch_config_get_property_string(config, "test.property1", &value1);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);
    CU_ASSERT_PTR_NULL(value1);

    etch_config_destroy(config);
}

static void test_etch_config_get_property_int(void)
{
    etch_status_t etch_status = ETCH_SUCCESS;
    etch_config_t* config = NULL;
    int32 value1 = 0;

    etch_status = etch_config_create(&config);
    CU_ASSERT_PTR_NOT_NULL(config);

    etch_status = etch_config_set_property(config, "test.property", "10");
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_status = etch_config_set_property(config, "test.property1", "fdg");
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_status = etch_config_get_property_int(config, NULL, &value1);
    CU_ASSERT_EQUAL(etch_status, ETCH_EINVAL);

    etch_status = etch_config_get_property_int(config, NULL, NULL);
    CU_ASSERT_EQUAL(etch_status, ETCH_EINVAL);

    etch_status = etch_config_get_property_int(config, "test.property", &value1);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);
    CU_ASSERT_EQUAL(value1, 10);

    etch_status = etch_config_get_property_int(config, "test.property1", &value1);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_status = etch_config_get_property_int(config, "test.property1", &value1);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_config_destroy(config);
}

static void test_etch_config_get_property_by_index(void)
{
    etch_status_t etch_status = ETCH_SUCCESS;
    etch_config_t* config = NULL;
    char* value1 = NULL;

    etch_status = etch_config_create(&config);
    CU_ASSERT_PTR_NOT_NULL(config);

    etch_status = etch_config_clear(config);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_status = etch_config_set_property(config, "test.property", "value");
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    etch_status = etch_config_get_property_by_index(config, 0, &value1);
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);
    CU_ASSERT_PTR_NOT_NULL(value1);
    if(value1 != NULL) {
        CU_ASSERT_EQUAL(strcmp("value", value1), 0);
    }

    etch_config_destroy(config);
}

static void test_etch_config_has_property(void)
{
    int result = 0;

    etch_status_t etch_status = ETCH_SUCCESS;
    etch_config_t* config = NULL;

    etch_status = etch_config_create(&config);
    CU_ASSERT_PTR_NOT_NULL(config);

    etch_status = etch_config_set_property(config, "test.property", "value");
    CU_ASSERT_EQUAL(etch_status, ETCH_SUCCESS);

    result = etch_config_has_property(config, "test.property");
    CU_ASSERT_EQUAL(result, 1);

    result = etch_config_has_property(config, "test.property2");
    CU_ASSERT_EQUAL(result, 0);

    etch_config_destroy(config);
}

//**********************
// END testcases here
//**********************

CU_pSuite test_etch_config_suite()
{
    CU_pSuite ps = CU_add_suite("etch_config", init_suite, clean_suite);  
    CU_add_test(ps, "test etch_config_create_destroy", test_etch_config_create_destroy);
    //CU_add_test(ps, "test etch_config_open", test_etch_config_open);
    CU_add_test(ps, "test etch_config_set_property", test_etch_config_set_property);
    CU_add_test(ps, "test etch_config_get_property_string", test_etch_config_get_property_string);
    CU_add_test(ps, "test etch_config_get_property_int", test_etch_config_get_property_int);
    CU_add_test(ps, "test etch_config_get_property_by_index", test_etch_config_get_property_by_index);
    CU_add_test(ps, "test etch_config_has_property", test_etch_config_has_property);
    return ps;
}
