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
#include "serialization/EtchField.h"
#include "serialization/EtchFieldMap.h"

TEST(EtchFieldMapTest, createTest) {
  EtchFieldMap *test = NULL;
  test = new EtchFieldMap();
  EXPECT_TRUE(test != NULL);
  delete test;
}

TEST(EtchFieldMapTest, addTest) {
  EtchFieldMap *test = NULL;
  test = new EtchFieldMap();
  EtchString str = "testfield1";
  EtchField field(10, str);
  //add new field to collection
  EXPECT_EQ(ETCH_OK, test->add(field));
  //Try to add existing field
  EXPECT_EQ(ETCH_ERROR, test->add(field));
  //lock the collection
  test->lock();
  //try to add new field
  EXPECT_EQ(ETCH_EINVAL, test->add(field));
  delete test;
}

TEST(EtchFieldMapTest, getTest) {
  EtchFieldMap *test = NULL;
  test = new EtchFieldMap();
  EtchString str = "testfield1";
  EtchField field(10, str);
  EtchField tmp1;
  EtchField tmp2;
  //add new field to collection
  EXPECT_EQ(ETCH_OK, test->add(field));
  //get the new field
  EXPECT_EQ(ETCH_OK, test->get(10, &tmp1));
  //check the content
  EXPECT_TRUE(field.equals(&tmp1));
  //try to get non existing element by name (which will be added)
  EXPECT_EQ(ETCH_OK, test->get("testfield2", &tmp1));  
  //lock the collection
  test->lock();
  //get field
  EXPECT_EQ(ETCH_OK, test->get(10, &tmp1));
  //check content
  EXPECT_TRUE(field.equals(&tmp1));
  //get field by name
  EXPECT_EQ(ETCH_OK, test->get("testfield1", &tmp2));
  //check content
  EXPECT_TRUE(field.equals(&tmp2));
  //Try to add a new field
  EXPECT_EQ(ETCH_EINVAL, test->add(field));
  //try to get non existing element
  EXPECT_EQ(ETCH_ENOT_EXIST, test->get(11, &tmp1));
  delete test;
}

TEST(EtchFieldMapTest, sizeTest) {
  EtchFieldMap *test = NULL;
  test = new EtchFieldMap();
  EtchString str = "testfield1";
  EtchField field(10, str);
  //check size
  EXPECT_EQ(0, test->size());
  //add element
  EXPECT_EQ(ETCH_OK, test->add(field));
  //check size
  EXPECT_EQ(1, test->size());
  //try to add existing element
  EXPECT_EQ(ETCH_ERROR, test->add(field));
  //check size
  EXPECT_EQ(1, test->size());
  delete test;
}

TEST(EtchFieldMapTest, lockTest) {
  EtchFieldMap *test = NULL;
  test = new EtchFieldMap();
  EtchString str = "testfield1";
  EtchField field(10, str);
  //check size of empty collection
  EXPECT_EQ(0, test->size());
  //add new field
  EXPECT_EQ(ETCH_OK, test->add(field));
  //try to add same field to collection
  EXPECT_EQ(ETCH_ERROR, test->add(field));
  //lock collection
  test->lock();
  //then try to add a field
  EXPECT_EQ(ETCH_EINVAL, test->add(field));
  //check size again
  EXPECT_EQ(1, test->size());
  delete test;
}