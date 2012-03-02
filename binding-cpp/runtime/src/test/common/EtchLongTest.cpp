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
#include "common/EtchLong.h"

// Tests positive input.

TEST(EtchLongTest, Constructor_Default){
  EtchLong* i1 = new EtchLong();
  EXPECT_TRUE(i1->getObjectType()->equals(&EtchLong::TYPE));
  EXPECT_TRUE(i1->get() == 0);
  delete i1;
}

TEST(EtchLongTest, Constructor_Long){
  EtchLong* i1 = new EtchLong(42);
  EXPECT_TRUE(i1->getObjectType()->equals(&EtchLong::TYPE));
  EXPECT_TRUE(i1->get() == 42);
  delete i1;
}

TEST(EtchLongTest, set){
  EtchLong* i1 = new EtchLong();
  i1->set(43000000);
  EXPECT_TRUE(i1->get() == 43000000);
  delete i1;
}

TEST(EtchLongTest, get){
  EtchLong* i1 = new EtchLong();
  EXPECT_TRUE(i1->get() == 0);
  i1->set(41);
  EXPECT_TRUE(i1->get() == 41);
  delete i1;
}

TEST(EtchLongTest, equals){
  EtchLong i1, i2;
  EXPECT_TRUE(i1.get() == false);
  i1.set(40);
  i2.set(41);
  EXPECT_TRUE(i1.equals(&i2) == false);
  i2.set(40);
  EXPECT_TRUE(i1.equals(&i2) == true);
}
