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
#include "gtest/gtest.h"
#include "serialization/EtchClass2TypeMap.h"
#include "serialization/EtchValueFactory.h"
#include "common/EtchString.h"
#include "common/EtchByte.h"

TEST(EtchClass2TypeMapTest, createTest) {
  EtchClass2TypeMap * collection = new EtchClass2TypeMap();
  EXPECT_TRUE(collection != NULL);
  delete collection;
}

TEST(EtchClass2TypeMapTest, getTest) {
  EtchClass2TypeMap *test = NULL;
  test = new EtchClass2TypeMap();

  EtchType* type = new EtchType(EtchString("string"));
  //add new field to collection
  EXPECT_EQ(ETCH_OK, test->put(EtchString::TYPE(), type));

  //get the new field
  EtchType* tmp1 = NULL;
  EXPECT_EQ(ETCH_OK, test->get(EtchString::TYPE(), &tmp1));
  //check the content
  EXPECT_TRUE(type->equals(tmp1));

  //lock the collection
  test->lock();
  //get field
  EXPECT_EQ(ETCH_OK, test->get(EtchString::TYPE(), &tmp1));
  //check content
  EXPECT_TRUE(type->equals(tmp1));

  //Try to add a new field
  EXPECT_EQ(ETCH_EINVAL, test->put(EtchByte::TYPE(), type));
  //try to get non existing element by name
  EXPECT_EQ(ETCH_ENOT_EXIST, test->get(EtchByte::TYPE(), &tmp1));

  delete test;
  delete type;
}

TEST(EtchClass2TypeMapTest, putTest) {
  EtchClass2TypeMap *test = NULL;
  test = new EtchClass2TypeMap();

  EtchType* type = new EtchType(EtchString("string"));
  //add new field to collection
  EXPECT_EQ(ETCH_OK, test->put(EtchString::TYPE(), type));

  //Try to add existing field
  EXPECT_EQ(ETCH_OK, test->put(EtchString::TYPE(), type));

  //lock the collection
  test->lock();
  //try to add new field
  EXPECT_EQ(ETCH_EINVAL, test->put(EtchString::TYPE(), type));
  delete test;
  delete type;
}

TEST(EtchClass2TypeMapTest, lockTest) {
  EtchClass2TypeMap *test = NULL;
  test = new EtchClass2TypeMap();

  EtchType* type = new EtchType(EtchString("string"));
  //add new field to collection
  EXPECT_EQ(ETCH_OK, test->put(EtchString::TYPE(), type));

  //lock the collection
  test->lock();
  //try to add new field
  EXPECT_EQ(ETCH_EINVAL, test->put(EtchString::TYPE(), type));
  delete test;
  delete type;
}

