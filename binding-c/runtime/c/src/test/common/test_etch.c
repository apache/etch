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
 * test_etch.c -- test etch common
 */
#include "etch.h"

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
    return 0;
}

static int clean_suite(void)
{
    return 0;
}

//**********************
// START testcases here
//**********************

static void test_etch_datatypes(void)
{
    CU_ASSERT_EQUAL(sizeof(char), 1);
    CU_ASSERT_EQUAL(sizeof(byte), 1);
    CU_ASSERT_EQUAL(sizeof(int8), 1);
    CU_ASSERT_EQUAL(sizeof(uint8), 1);
    CU_ASSERT_EQUAL(sizeof(int16), 2);
    CU_ASSERT_EQUAL(sizeof(uint16), 2);
    CU_ASSERT_EQUAL(sizeof(int32), 4);
    CU_ASSERT_EQUAL(sizeof(uint32), 4);
    CU_ASSERT_EQUAL(sizeof(int64), 8);
    CU_ASSERT_EQUAL(sizeof(uint64), 8);
}

//**********************
// END testcases here
//**********************

CU_pSuite test_etch_suite()
{
    CU_pSuite ps = CU_add_suite("etch_suite", init_suite, clean_suite);  
    CU_add_test(ps, "test etch_datatypes", test_etch_datatypes);
    return ps;
}
