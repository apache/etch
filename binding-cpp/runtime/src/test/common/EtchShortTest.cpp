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
#include "common/EtchShort.h"
#include "common/EtchString.h"

// Tests positive input.

TEST(EtchShortTest, Constructor_Default){
  EtchShort* i1 = new EtchShort();
  EXPECT_TRUE(i1->getObjectType()->equals(EtchShort::TYPE()));
  EXPECT_EQ(0, i1->get());
  delete i1;
}

TEST(EtchShortTest, Constructor_Short){
  EtchShort* i1 = new EtchShort(42);
  EXPECT_TRUE(i1->getObjectType()->equals(EtchShort::TYPE()));
  EXPECT_EQ(42, i1->get());
  delete i1;
}

TEST(EtchShortTest, set){
  EtchShort* i1 = new EtchShort();
  i1->set(43);
  EXPECT_EQ(43, i1->get());
  delete i1;
}

TEST(EtchShortTest, get){
  EtchShort* i1 = new EtchShort();
  EXPECT_EQ(0, i1->get());
  i1->set(41);
  EXPECT_EQ(41, i1->get());
  delete i1;
}

TEST(EtchShortTest, equals){
  EtchShort i1, i2;
  EXPECT_FALSE(i1.get());
  i1.set(40);
  i2.set(41);
  EXPECT_FALSE(i1.equals(&i2));
  i2.set(40);
  EXPECT_TRUE(i1.equals(&i2));
}

TEST(EtchShortTest, copyTest) {
  EtchShort o1(2);
  EtchShort o2(o1);
  EtchShort o3 = o2;
  EXPECT_TRUE(o1.equals(&o2));
  EXPECT_TRUE(o2.equals(&o3));
}

TEST(EtchShortTest, isInstanceOf) {
  EtchObject* o1 = new EtchShort(2);
  EXPECT_TRUE(o1->isInstanceOf(EtchObject::TYPE()));
  EXPECT_TRUE(o1->isInstanceOf(EtchShort::TYPE()));
  EXPECT_FALSE(o1->isInstanceOf(EtchString::TYPE()));
  delete o1;
}
