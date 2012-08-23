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
#include "common/EtchHashSet.h"
#include "serialization/EtchType.h"
#include "serialization/EtchTypeMap.h"

TEST(EtchTypeMapTest, createTest) {
  EtchTypeMap *test = NULL;
  test = new EtchTypeMap();
  EXPECT_TRUE(test != NULL);
  delete test;
}

TEST(EtchTypeMapTest, addTest) {
  EtchTypeMap *test = NULL;
  test = new EtchTypeMap();
  EtchString str = "testtype1";
  EtchType* type = new EtchType(10, str);
  //add new type to collection
  EXPECT_EQ(ETCH_OK, test->add(type));
  //Try to add existing type
  EXPECT_EQ(ETCH_ERROR, test->add(type));
  //lock the collection
  test->lock();
  //try to add new type
  EXPECT_EQ(ETCH_EINVAL, test->add(type));
  delete test;
}

TEST(EtchTypeMapTest, getTest) {
  EtchTypeMap *test = NULL;
  test = new EtchTypeMap();
  EtchString str = "testtype1";
  EtchType* type = new EtchType(10, str);
  EtchType* tmp1;
  EtchType* tmp2;
  //add new type to collection
  EXPECT_EQ(ETCH_OK, test->add(type));
  //get the new type
  EXPECT_EQ(ETCH_OK, test->get(10, tmp1));
  //check the content
  EXPECT_TRUE(type->equals(tmp1));
  //try to get non existing element by name and add it 
  EXPECT_EQ(ETCH_OK, test->get("testtype2", tmp1));

  //lock the collection
  test->lock();
  //get type
  EXPECT_EQ(ETCH_OK, test->get(10, tmp1));
  //check content
  EXPECT_TRUE(type->equals(tmp1));
  //get type by name
  EXPECT_EQ(ETCH_OK, test->get("testtype1", tmp2));
  //check content
  EXPECT_TRUE(type->equals(tmp2));
  //Try to add a new type
  EXPECT_EQ(ETCH_EINVAL, test->add(type));
  //try to get non existing element
  EXPECT_EQ(ETCH_ENOT_EXIST, test->get(11, tmp1));
  //try to get non existing element by name and add it to the locked collection
  EXPECT_EQ(ETCH_EINVAL, test->get("testtype3", tmp1));
  //Get the already added element
  EXPECT_EQ(ETCH_OK, test->get("testtype2", tmp1));

  delete test;
}

TEST(EtchTypeMapTest, sizeTest) {
  EtchTypeMap *test = NULL;
  test = new EtchTypeMap();
  EtchString str = "testtype1";
  EtchType* type = new EtchType(10, str);
  //check size
  EXPECT_EQ(0, test->size());
  //add element
  EXPECT_EQ(ETCH_OK, test->add(type));
  //check size
  EXPECT_EQ(1, test->size());
  //try to add existing element
  EXPECT_EQ(ETCH_ERROR, test->add(type));
  //check size
  EXPECT_EQ(1, test->size());
  delete test;
}

TEST(EtchTypeMapTest, lockTest) {
  EtchTypeMap *test = NULL;
  test = new EtchTypeMap();
  EtchString str = "testtype1";
  EtchType* type = new EtchType(10, str);
  //check size of empty collection
  EXPECT_EQ(0, test->size());
  //add new type
  EXPECT_EQ(ETCH_OK, test->add(type));
  //try to add same type to collection
  EXPECT_EQ(ETCH_ERROR, test->add(type));
  //lock collection
  test->lock();
  //then try to add a type
  EXPECT_EQ(ETCH_EINVAL, test->add(type));
  //check size again
  EXPECT_EQ(1, test->size());
  delete test;
}

TEST(EtchTypeMapTest, getAllTest) {
  EtchTypeMap *test = NULL;
  test = new EtchTypeMap();
  EtchString str = "testtype1";
  EtchType* type = new EtchType(10, str);
  EtchHashSet<EtchType*> set;

  //check size of empty collection
  EXPECT_EQ(0, test->size());
  //add new type
  EXPECT_EQ(ETCH_OK, test->add(type));
  //try to add same type to collection
  EXPECT_EQ(ETCH_EINVAL, test->getAll(NULL));

  EXPECT_EQ(ETCH_OK, test->getAll(&set));

  EtchType *result;
  EtchHashSet<EtchType*>::Iterator it = set.begin();

  while (it.hasNext()) {
    it.next(&result);
  }

  delete test;
}