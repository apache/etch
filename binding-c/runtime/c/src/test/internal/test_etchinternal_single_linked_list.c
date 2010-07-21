/*
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 *
 */

#include "CUnit.h"
#include "etchinternal_single_linked_list.h"

static int intFinder(void* data, void* toFind) {
  int iToFind = (int)toFind;
  int intData = *((int*)data);
  return iToFind == intData;
}

static void testFind() {
  etchinternal_single_linked_list* l = etchinternal_single_linked_list_create();

  int v = 1;
  etchinternal_single_linked_list_add(l, &v, sizeof(v));
  v = 2;
  etchinternal_single_linked_list_add(l, &v, sizeof(v));

  CU_ASSERT_EQUAL(1, etchinternal_single_linked_list_find(l, intFinder, (void*)1) != 0);
  CU_ASSERT_EQUAL(1, etchinternal_single_linked_list_find(l, intFinder, (void*)2) != 0);
  CU_ASSERT_EQUAL(0, etchinternal_single_linked_list_find(l, intFinder, (void*)3) != 0);

  etchinternal_single_linked_list_destroy(l);
}

CU_pSuite test_etchinternal_single_linked_list()
{
  CU_pSuite pSuite = CU_add_suite("etchinternal_single_linked_list", 0, 0);
  CU_add_test(pSuite, "testFind()", testFind); 
  return pSuite;
}
