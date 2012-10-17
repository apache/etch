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
#include "common/EtchDate.h"
#include "common/EtchString.h"

TEST(EtchDateTest, Constructor) {
  EtchDate* i1 = new EtchDate();
  EXPECT_TRUE(i1->getObjectType() == EtchDate::TYPE());
  delete i1;
}

TEST(EtchDateTest, set) {
  EtchDate* i1 = new EtchDate();
  i1->set(1234567);
  EXPECT_TRUE(i1->get() == 1234567);
  delete i1;
}

TEST(EtchDateTest, get) {
  EtchDate* i1 = new EtchDate();
  time_t _time = 1234567;
  i1->set(_time);
  EXPECT_TRUE(i1->get() == _time);
  delete i1;
}

TEST(EtchDateTest, equals) {
  EtchDate i1, i2;
  time_t _time = 1234567;
  i1.set(_time);
  i2.set(41);
  EXPECT_TRUE(i1.equals(&i2) == false);
  i2.set(_time);
  EXPECT_TRUE(i1.equals(&i2) == true);
}


TEST(EtchDateTest, copyTest) {
  EtchDate o1;
  time_t _time = 1234567;
  o1.set(_time);
  EtchDate o2(o1);
  EtchDate o3 = o2;
  EXPECT_TRUE(o1.equals(&o2));
  EXPECT_TRUE(o2.equals(&o3));
}

TEST(EtchDateTest, isInstanceOf) {
  EtchObject* o1 = new EtchDate();
  EXPECT_TRUE(o1->isInstanceOf(EtchObject::TYPE()));
  EXPECT_TRUE(o1->isInstanceOf(EtchDate::TYPE()));
  EXPECT_FALSE(o1->isInstanceOf(EtchString::TYPE()));
  delete o1;
}
