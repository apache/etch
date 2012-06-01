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

#include <gtest/gtest.h>
#include "serialization/EtchField.h"
#include "util/EtchHash.h"

TEST(EtchFieldTest, createTest) {
  EtchField *test = NULL;
  test = new EtchField();
  EXPECT_TRUE(test != NULL);
  delete test;
}

TEST(EtchFieldTest, getTest) {
  EtchField *test = NULL;
  EtchString type("testType");
  test = new EtchField(5, type);

  EXPECT_TRUE(test->getId() == 5);
  EXPECT_TRUE(test->getName().equals(&type));
  delete test;
}

TEST(EtchFieldTest, hashTest) {
  EtchString str = "test";
  EtchField *field = new EtchField(str);
  EXPECT_EQ(EtchHashEx::Digest(str),field->getId());
  EXPECT_TRUE(field->getName().equals(&str));
  delete field;

}
