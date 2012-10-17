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
#include "common/EtchInt32.h"
#include "common/EtchString.h"

// Tests positive input.

TEST(EtchInt32Test, Constructor_Default){
  EtchInt32* i1 = new EtchInt32();
  EtchInt32(i1->getObjectType()->equals(EtchInt32::TYPE()));
  EtchInt32(i1->get() == 0);
  delete i1;
}

TEST(EtchInt32Test, Constructor_Int){
  EtchInt32* i1 = new EtchInt32(42);
  EXPECT_TRUE(i1->getObjectType()->equals(EtchInt32::TYPE()));
  EXPECT_EQ(42, i1->get());
  delete i1;
}

TEST(EtchInt32Test, set){
  EtchInt32* i1 = new EtchInt32();
  i1->set(42);
  EXPECT_EQ(42, i1->get());
  delete i1;
}

TEST(EtchInt32Test, get){
  EtchInt32* i1 = new EtchInt32();
  EXPECT_EQ(0, i1->get());
  i1->set(42);
  EXPECT_EQ(42, i1->get());
  delete i1;
}

TEST(EtchInt32Test, equals){
  EtchInt32 i1, i2;
  EXPECT_FALSE(i1.get());
  i1.set(40);
  i2.set(41);
  EXPECT_FALSE(i1.equals(&i2));
  i2.set(40);
  EXPECT_TRUE(i1.equals(&i2));
}

TEST(EtchInt32Test, operator_equal){
  EtchInt32 a = 0;
  EXPECT_EQ(0, a.get());
  a = 5;
  EXPECT_EQ(5, a.get());
  EtchInt32 b(12);
  a = b;
  EXPECT_EQ(12, a.get());
}

TEST(EtchInt32Test, operator_post_plusplus){
  EtchInt32 a = 0;
  for(int i = 1; i <= 10; i++)
  {
    a++;
    EXPECT_EQ(i, a.get());
  }
  EXPECT_EQ(10, a++);
  EXPECT_EQ(11, a.get());
}

TEST(EtchInt32Test, operator_pre_plusplus){
  EtchInt32 a = 0;
  for(int i = 1; i <= 10; i++)
  {
    EXPECT_EQ(i, ++a);
  }
  EXPECT_EQ(10, a.get());
  EXPECT_EQ(11, ++a);
}

TEST(EtchInt32Test, operator_post_minusminus){
  EtchInt32 a = 11;
  for(int i = 10; i > 0; i--)
  {
    a--;
    EXPECT_EQ(i, a.get());
  }
  EXPECT_EQ(1, a--);
  EXPECT_EQ(0, a.get());
}

TEST(EtchInt32Test, operator_pre_minusminus){
  EtchInt32 a = 11;
  for(int i = 10; i > 0; i--)
  {
    EXPECT_EQ(i, --a);
  }
  EXPECT_EQ(1, a.get());
  EXPECT_EQ(0, --a);
}

TEST(EtchInt32Test, operator_equalequal){
  EtchInt32 a(12);
  EtchInt32 b(12);
  EXPECT_TRUE(a == b);
  ++b;
  EXPECT_FALSE(a == b);
}

TEST(EtchInt32Test, operator_unequal){
  EtchInt32 a(12);
  EtchInt32 b(11);
  EXPECT_TRUE(a != b);
  ++b;
  EXPECT_FALSE(a != b);
}

TEST(EtchInt32Test, copyTest) {
  EtchInt32 o1(2);
  EtchInt32 o2(o1);
  EtchInt32 o3 = o2;
  EXPECT_TRUE(o1.equals(&o2));
  EXPECT_TRUE(o2.equals(&o3));
}

TEST(EtchInt32Test, isInstanceOf) {
  EtchObject* o1 = new EtchInt32(2);
  EXPECT_TRUE(o1->isInstanceOf(EtchObject::TYPE()));
  EXPECT_TRUE(o1->isInstanceOf(EtchInt32::TYPE()));
  EXPECT_FALSE(o1->isInstanceOf(EtchString::TYPE()));
  delete o1;
}
