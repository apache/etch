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
#include "etch_linked_list.h"

#include <stdio.h>
#include <wchar.h>
#include "CUnit.h"

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

static void test_etch_linked_list_create_destroy(void)
{
    etch_status_t       status = ETCH_SUCCESS;
    etch_linked_list_t* list   = NULL;

    status = etch_linked_list_create(&list, 0);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    status = etch_linked_list_destroy(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
}

static void test_etch_linked_list_add(void)
{
    etch_status_t       status = ETCH_SUCCESS;
    etch_linked_list_t* list   = NULL;
    char*               data   = "test_0";
    uint32              count  = 0;
    uint32              i      = 0;

    status = etch_linked_list_add(NULL, NULL);
    CU_ASSERT_EQUAL(status, ETCH_EINVAL);

    status = etch_linked_list_create(&list, 0);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_add(list, data);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 1);

    status = etch_linked_list_remove(list, data);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 0);

    status = etch_linked_list_add(list, NULL);
    CU_ASSERT_EQUAL(status, ETCH_EINVAL);

    status = etch_linked_list_add(list, "test_1");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_add(list, "test_2");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_destroy(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_create(&list, ETCH_LINKED_LIST_DATA_FREE);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_add(list, etch_malloc(10, 0));
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_add(list, etch_malloc(20, 0));
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_destroy(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_create(&list, 0);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    for(i = 0; i < 1000; i++) {
        status = etch_linked_list_add(list, data);
        CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    }
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 1000);

    for(i = 0; i < 1000; i++) {
        status = etch_linked_list_remove(list, data);
        CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    }
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 0);


    status = etch_linked_list_destroy(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

}

void test_etch_linked_list_count(void)
{
    etch_status_t       status = ETCH_SUCCESS;
    etch_linked_list_t* list   = NULL;
    uint32              count  = 0;

    status = etch_linked_list_create(&list, 0);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 0);

    status = etch_linked_list_add(list, "test_1");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 1);

    status = etch_linked_list_destroy(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
}

void test_etch_linked_list_contains(void)
{
    etch_status_t       status    = ETCH_SUCCESS;
    etch_linked_list_t* list      = NULL;
    uint32              contains  = 0;
    char*               data      = "test_1";

    status = etch_linked_list_create(&list, 0);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_add(list, "test_2");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_add(list, data);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    contains = etch_linked_list_contains(NULL, NULL);
    CU_ASSERT_EQUAL(contains, 0);

    contains = etch_linked_list_contains(list, NULL);
    CU_ASSERT_EQUAL(contains, 0);

    contains = etch_linked_list_contains(list, "test_3");
    CU_ASSERT_EQUAL(contains, 0);

    contains = etch_linked_list_contains(list, data);
    CU_ASSERT_EQUAL(contains, 1);

    status = etch_linked_list_destroy(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
}

void test_etch_linked_list_insert(void)
{
    etch_status_t       status    = ETCH_SUCCESS;
    etch_linked_list_t* list      = NULL;
    uint32              count     = 0;

    status = etch_linked_list_create(&list, 0);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_insert(list, 2, "test_1");
    CU_ASSERT_EQUAL(status, ETCH_EOUTOFBOUNDS);

    status = etch_linked_list_add(list, "test_2");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 1);

    status = etch_linked_list_insert(list, 0, "test_3");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 2);

    status = etch_linked_list_insert(list, 1, "test_4");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 3);

    status = etch_linked_list_insert(list, 2, "test_4");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 4);

    status = etch_linked_list_destroy(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
}

void test_etch_linked_list_get(void)
{
    etch_status_t       status    = ETCH_SUCCESS;
    etch_linked_list_t* list      = NULL;
    uint32              count     = 0;
    uint32              i         = 0;
    char*               data      = NULL;

    status = etch_linked_list_create(&list, 0);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_add(list, "test_0 added");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_insert(list, 0, "test_1 insert 0");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_insert(list, 1, "test_2 insert 1");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_insert(list, 2, "test_3 insert 2");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 4);

    for(i = 0; i < count; i++) {
      status = etch_linked_list_get(list, i, (void**)&data);
        CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
        //printf("%s\n", data);
    }

    status = etch_linked_list_destroy(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
}

void test_etch_linked_list_remove_at(void)
{
    etch_status_t       status    = ETCH_SUCCESS;
    etch_linked_list_t* list      = NULL;
    uint32              count     = 0;

    status = etch_linked_list_create(&list, 0);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_add(list, "test_0 added");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_remove_at(list, 0);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 0);

    status = etch_linked_list_add(list, "test_0 added");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    status = etch_linked_list_add(list, "test_1 added");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    status = etch_linked_list_add(list, "test_2 added");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 3);
    status = etch_linked_list_remove_at(list, 1);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 2);

    status = etch_linked_list_remove_at(list, 1);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 1);

    status = etch_linked_list_remove_at(list, 0);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 0);

    status = etch_linked_list_destroy(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    // check remove of last list element
    status = etch_linked_list_create(&list, 0);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_add(list, "test_0 added");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    status = etch_linked_list_add(list, "test_1 added");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    status = etch_linked_list_add(list, "test_2 added");
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    status = etch_linked_list_remove_at(list, 2);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 2);

    status = etch_linked_list_remove_at(list, 1);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    status = etch_linked_list_remove_at(list, 0);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 0);

    status = etch_linked_list_destroy(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

}

void test_etch_linked_list_remove(void)
{
    etch_status_t       status    = ETCH_SUCCESS;
    etch_linked_list_t* list      = NULL;
    uint32              count     = 0;
    char*               data      = NULL;

    status = etch_linked_list_create(&list, 0);
    CU_ASSERT_PTR_NOT_NULL(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);

    data = "test_0 added";

    status = etch_linked_list_add(list, data);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 1);

    status = etch_linked_list_remove(list, data);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
    count = etch_linked_list_count(list);
    CU_ASSERT_EQUAL(count, 0);

    status = etch_linked_list_destroy(list);
    CU_ASSERT_EQUAL(status, ETCH_SUCCESS);
}

//**********************
// END testcases here
//**********************

/**
 * main
 */
CU_pSuite test_etch_linked_list_suite()
{
    CU_pSuite ps = CU_add_suite("etch_runtime", init_suite, clean_suite);
    
    CU_add_test(ps, "test_etch_linked_list_create_destroy", test_etch_linked_list_create_destroy);
    CU_add_test(ps, "test_etch_linked_list_add", test_etch_linked_list_add);
    CU_add_test(ps, "test_etch_linked_list_count", test_etch_linked_list_count);
    CU_add_test(ps, "test_etch_linked_list_contains", test_etch_linked_list_contains);
    CU_add_test(ps, "test_etch_linked_list_insert", test_etch_linked_list_insert);
    CU_add_test(ps, "test_etch_linked_list_get", test_etch_linked_list_get);
    CU_add_test(ps, "test_etch_linked_list_remove_at", test_etch_linked_list_remove_at);
    CU_add_test(ps, "test_etch_linked_list_remove", test_etch_linked_list_remove);

    return ps;




}
