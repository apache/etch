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
#include "common/EtchFloat.h"
#include "common/EtchString.h"

// Tests positive input.

TEST(EtchFloatTest, Constructor_Default){
  EtchFloat* i1 = new EtchFloat();
  EXPECT_TRUE(i1->getObjectType()->equals(EtchFloat::TYPE()));
  EXPECT_EQ(0.0f, i1->get());
  delete i1;
}

TEST(EtchFloatTest, Constructor_Float){
  EtchFloat* i1 = new EtchFloat(42.0f);
  EXPECT_TRUE(i1->getObjectType()->equals(EtchFloat::TYPE()));
  EXPECT_EQ(42.0f, i1->get());
  delete i1;
}

TEST(EtchFloatTest, set){
  EtchFloat* i1 = new EtchFloat();
  i1->set(43.33f);
  EXPECT_EQ(43.33f, i1->get());
  delete i1;
}

TEST(EtchFloatTest, get){
  EtchFloat* i1 = new EtchFloat();
  EXPECT_EQ(0.0f, i1->get());
  i1->set(41.0f);
  EXPECT_EQ(41.0f, i1->get());
  delete i1;
}

TEST(EtchFloatTest, equals){
  EtchFloat i1, i2;
  EXPECT_EQ(0.0f, i1.get());
  i1.set(40);
  i2.set(41);
  EXPECT_FALSE(i1.equals(&i2));
  i2.set(40);
  EXPECT_TRUE(i1.equals(&i2));
}

TEST(EtchFloatTest, copyTest) {
  EtchFloat o1(static_cast<capu::float_t>(2.123));
  EtchFloat o2(o1);
  EtchFloat o3 = o2;
  EXPECT_TRUE(o1.equals(&o2));
  EXPECT_TRUE(o2.equals(&o3));
}

TEST(EtchFloatTest, isInstanceOf) {
  EtchObject* o1 = new EtchFloat(static_cast<capu::float_t>(2.123));
  EXPECT_TRUE(o1->isInstanceOf(EtchObject::TYPE()));
  EXPECT_TRUE(o1->isInstanceOf(EtchFloat::TYPE()));
  EXPECT_FALSE(o1->isInstanceOf(EtchString::TYPE()));
  delete o1;
}
