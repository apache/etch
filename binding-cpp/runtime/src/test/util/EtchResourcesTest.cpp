/* $Id$
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

#include <gtest/gtest.h>
#include "util/EtchResources.h"
#include "common/EtchInt32.h"
#include "common/EtchFloat.h"

TEST(EtchResources, constructorTest) {
  EtchResources * res = new EtchResources();
  EXPECT_TRUE(res != NULL);
  delete res;
}

TEST(EtchResources, putTest) {
  EtchResources * res = new EtchResources();
  EtchObject* result;
  EXPECT_TRUE(res != NULL);
  EtchInt32 value1(12);
  EtchFloat value2(12.1f);
  EtchString key1("key1");
  EtchString key2("key2");

  EXPECT_TRUE(res->put(key1, &value1, result) == ETCH_OK);
  EXPECT_TRUE(res->put(key2, &value2, result) == ETCH_OK);

  res->put(key2, &value1, result);
  EXPECT_TRUE(((EtchFloat*) result)->get() == 12.1f);
  delete res;
}

TEST(EtchResources, getTest) {
  EtchResources * res = new EtchResources();
  EXPECT_TRUE(res != NULL);
  EtchInt32 value1(12);
  EtchFloat value2(12.1f);
  EtchObject* result;
  EtchString key1("key1");
  EtchString key2("key2");

  EXPECT_TRUE(res->put(key1, &value1, result) == ETCH_OK);
  EXPECT_TRUE(res->put(key2, &value2, result) == ETCH_OK);

  EtchObject* ptr1;
  EXPECT_TRUE(res->get(key1, ptr1) == ETCH_OK);
  EtchObject* ptr2;
  EXPECT_TRUE(res->get(key2, ptr2) == ETCH_OK);

  EXPECT_TRUE(((EtchInt32*) ptr1)->get() == 12);
  EXPECT_TRUE(((EtchFloat*) ptr2)->get() == 12.1f);

  EXPECT_TRUE(res->put(key2, &value1, result) == ETCH_OK);
  EXPECT_TRUE(((EtchFloat*) result)->get() == 12.1f);
  delete res;
}

TEST(EtchResources, containsKeyTest) {
  EtchResources * res = new EtchResources();
  EXPECT_TRUE(res != NULL);
  EtchInt32 value1(12);
  EtchFloat value2(12.1f);
  EtchObject* result;
  EtchString key1("key1");
  EtchString key2("key2");
  EtchString key3("key3");

  EXPECT_TRUE(res->put(key1, &value1, result) == ETCH_OK);
  EXPECT_TRUE(res->put(key2, &value2, result) == ETCH_OK);

  EXPECT_TRUE(res->containsKey(key1) == true);
  EXPECT_TRUE(res->containsKey(key2) == true);
  EXPECT_TRUE(res->containsKey(key3) == false);

  delete res;
}

TEST(EtchResources, removeTest) {
  EtchResources * res = new EtchResources();
  EXPECT_TRUE(res != NULL);
  EtchInt32 value1(12);
  EtchFloat value2(12.1f);
  EtchObject* result;
  EtchString key1("key1");
  EtchString key2("key2");

  EXPECT_TRUE(res->put(key1, &value1, result) == ETCH_OK);
  EXPECT_TRUE(res->put(key2, &value2, result) == ETCH_OK);
  EtchObject* ptr1;
  EtchObject* ptr2;
  EXPECT_TRUE(res->remove(key1, ptr1) == ETCH_OK);
  EXPECT_TRUE(res->remove(key2, ptr2) == ETCH_OK);
  
  EXPECT_TRUE(((EtchInt32*) ptr1)->get() == 12);
  EXPECT_TRUE(((EtchFloat*) ptr2)->get() == 12.1f);

  EXPECT_TRUE(res->containsKey(key1) == false);
  delete res;
}