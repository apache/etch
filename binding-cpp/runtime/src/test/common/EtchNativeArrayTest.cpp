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
#include "common/EtchNativeArray.h"

TEST(EtchNativeArrayTest, Constructor_Default) {
  EtchNativeArray<EtchInt32*>* na =
          new EtchNativeArray<EtchInt32*>(1, 2);
  delete na;
}

TEST(EtchNativeArrayTest, Constructor_Array) {

  EtchInt32* int1 = new EtchInt32(42);
  EtchInt32* int2 = new EtchInt32(43);

  EtchNativeArray<EtchInt32*>* na1 = new EtchNativeArray<EtchInt32*>(1, 2);

  na1->set(0, int1);
  na1->set(1, int2);

  EXPECT_TRUE(na1->getObjectType()->equals(EtchNativeArray<EtchInt32*>::TYPE()));

  EtchInt32* value = NULL;
  capu::int32_t i = 0;
  for (i = 0; i < 2; i++) {
    status_t ret = na1->get(i, &value);
    ASSERT_EQ(ETCH_OK, ret);
    ASSERT_EQ(42 + i, value->get());
  }

  delete na1;
  delete int1;
  delete int2;
}

TEST(EtchNativeArrayTest, setget) {

  EtchInt32* int1 = new EtchInt32(42);
  EtchInt32* int2 = new EtchInt32(43);
  EtchNativeArray<EtchInt32*>* na1 =
          new EtchNativeArray<EtchInt32*>(1, 2);
  EXPECT_TRUE(na1->getObjectType()->equals(EtchNativeArray<EtchInt32*>::TYPE()));

  na1->set(0, int1);
  na1->set(1, int2);

  EtchInt32* value = NULL;
  int i = 0;
  for (i = 0; i < 2; i++) {
    status_t ret = na1->get(i, &value);
    ASSERT_EQ(ETCH_OK, ret);
    ASSERT_EQ(42 + i, value->get());
  }

  EtchInt32* newValue = new EtchInt32(44);

  na1->set(1, newValue);

  status_t ret = na1->get(1, &value);
  ASSERT_EQ(ETCH_OK, ret);
  ASSERT_EQ(44, newValue->get());

  //Test setget of block
  EtchNativeArray<EtchInt32*> *native_array = new EtchNativeArray<EtchInt32*> (1, 2);
  EtchInt32** int3 = new EtchInt32*[2];
  int3[0] = new EtchInt32();
  int3[1] = new EtchInt32();

  int3[0]->set(2);
  int3[1]->set(4);

  ret = native_array->set(0, 0, NULL, 0);
  EXPECT_TRUE(ret == ETCH_EINVAL);

  ret = native_array->get(0, 0, NULL, 0);
  EXPECT_TRUE(ret == ETCH_EINVAL);

  ret = native_array->set(0, 2, int3, 0);
  EXPECT_TRUE(ret == ETCH_OK);

  ret = native_array->get(0, &value);
  EXPECT_TRUE(value->get() == 2);

  EtchInt32** int6 = new EtchInt32*[2];
  ret = native_array->get(0, 2, int6, 0);
  EXPECT_TRUE(ret == ETCH_OK);

  EXPECT_TRUE(int6[0]->get() == 2);
  EXPECT_TRUE(int6[1]->get() == 4);

  //Test setget of block of native type
  EtchNativeArray<int> *native_array2 = new EtchNativeArray<int> (1, 2);
  capu::int32_t int4[2];
  capu::int32_t int5[2];
  int4[0] = 3;
  int4[1] = 5;

  ret = native_array2->set(0, 0, NULL, 0);
  EXPECT_TRUE(ret == ETCH_EINVAL);

  ret = native_array2->get(0, 0, NULL, 0);
  EXPECT_TRUE(ret == ETCH_EINVAL);

  ret = native_array2->set(0, 2, (int*) int4, 0);
  EXPECT_TRUE(ret == ETCH_OK);

  ret = native_array2->get(0, 2, (int*) int5, 0);
  EXPECT_TRUE(ret == ETCH_OK);

  EXPECT_TRUE(int5[0] == 3);
  EXPECT_TRUE(int5[1] == 5);

  //buffer size smaller then count
  ret = native_array2->set(0, 1, (int*) int4, 2);
  EXPECT_TRUE(ret == ETCH_ERANGE);

  //offset + count out of range
  ret = native_array2->set(0, 2, (int*) int4, 2);
  EXPECT_TRUE(ret == ETCH_ERANGE);

  //buffer size smaller then count
  ret = native_array2->get(0, 1, (int*) int5, 2);
  EXPECT_TRUE(ret == ETCH_ERANGE);

  //offset + count out of range
  ret = native_array2->set(0, 2, (int*) int5, 2);
  EXPECT_TRUE(ret == ETCH_ERANGE);

  delete int3[0];
  delete int3[1];
  delete[] int3;
  delete[] int6;
  delete na1;
  delete int1;
  delete int2;
  delete newValue;
  delete native_array;
  delete native_array2;
}

TEST(EtchNativeArrayTest, multiDimensionalArrays) {

  EtchInt32* int1 = new EtchInt32(1);
  EtchInt32* int2 = new EtchInt32(2);
  EtchInt32* int3 = new EtchInt32(3);
  EtchInt32* int4 = new EtchInt32(4);

  EtchNativeArray<EtchInt32*>* mainArray = new EtchNativeArray<EtchInt32*>(2, 2);

  const EtchObjectType *ot = mainArray->getObjectType();
  //get type of main array
  EXPECT_TRUE(ot->equals(EtchNativeArray<EtchInt32*>::TYPE()));

  EtchInt32* int5;
  mainArray->createArray(Pos(0), 1, 2);
  mainArray->createArray(Pos(1), 1, 2);
  mainArray->set(Pos(0, 0), int1);
  mainArray->set(Pos(0, 1), int2);
  mainArray->set(Pos(1, 0), int3);
  mainArray->set(Pos(1, 1), int4);
  mainArray->get(Pos(0, 0), &int5);
  EXPECT_EQ(1, int5->get());

  delete int1;
  delete int2;
  delete int3;
  delete int4;

  delete mainArray;
}