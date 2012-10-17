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
#include "common/EtchBool.h"
#include "common/EtchString.h"

// Tests positive input.

TEST(EtchBoolTest, Constructor_Default){
  EtchBool* i1 = new EtchBool();
  EXPECT_TRUE(i1->getObjectType()->equals(EtchBool::TYPE()));
  EXPECT_FALSE(i1->get());
  delete i1;
}

TEST(EtchBoolTest, Constructor_Bool){
  EtchBool* i1 = new EtchBool(true);
  EXPECT_TRUE(i1->getObjectType()->equals(EtchBool::TYPE()));
  EXPECT_TRUE(i1->get());
  delete i1;
}

TEST(EtchBoolTest, set){
  EtchBool* i1 = new EtchBool();
  i1->set(true);
  EXPECT_TRUE(i1->get());
  delete i1;
}

TEST(EtchBoolTest, get){
  EtchBool* i1 = new EtchBool();
  EXPECT_FALSE(i1->get());
  i1->set(true);
  EXPECT_TRUE(i1->get());
  delete i1;
}

TEST(EtchBoolTest, equals){
  EtchBool i1, i2;
  EXPECT_FALSE(i1.get());
  i1.set(true);
  i2.set(false);
  EXPECT_FALSE(i1.equals(&i2));
  i2.set(true);
  EXPECT_TRUE(i1.equals(&i2));
}

TEST(EtchBoolTest, copyTest) {
  EtchBool o1(true);
  EtchBool o2(o1);
  EtchBool o3 = o2;
  EXPECT_TRUE(o1.equals(&o2));
  EXPECT_TRUE(o2.equals(&o3));
}

TEST(EtchBoolTest, isInstanceOf) {
  EtchObject* o1 = new EtchBool(true);
  EXPECT_TRUE(o1->isInstanceOf(EtchObject::TYPE()));
  EXPECT_TRUE(o1->isInstanceOf(EtchBool::TYPE()));
  EXPECT_FALSE(o1->isInstanceOf(EtchString::TYPE()));
  delete o1;
}

