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
  EtchNativeArray<EtchInt32*>* na = new EtchNativeArray<EtchInt32*>(2);
  delete na;
}

TEST(EtchNativeArrayTest, Constructor_Array) {

  EtchInt32* int1 = new EtchInt32(42);
  EtchInt32* int2 = new EtchInt32(43);

  EtchNativeArray<EtchInt32*>* na1 = new EtchNativeArray<EtchInt32*>(2);
  EXPECT_TRUE(na1->getObjectType()->equals(EtchNativeArray<EtchInt32*>::TYPE()));

  na1->set(0, int1);
  na1->set(1, int2);

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

TEST(EtchNativeArrayTest, setGetSingleValue) {
  EtchInt32* int1 = new EtchInt32(42);
  EtchInt32* int2 = new EtchInt32(43);
  EtchNativeArray<EtchInt32*>* na1 = new EtchNativeArray<EtchInt32*>(2);

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

  delete na1;
  delete int1;
  delete int2;
  delete newValue;
}

TEST(EtchNativeArrayTest, setGetArray) {
  status_t ret;
  EtchInt32** arr1 = new EtchInt32*[6];
  EtchInt32** arr2 = new EtchInt32*[4];
  arr1[0] = new EtchInt32(1);
  arr1[1] = new EtchInt32(2);
  arr1[2] = new EtchInt32(3);
  arr1[3] = new EtchInt32(4);
  arr1[4] = new EtchInt32(5);
  arr1[5] = new EtchInt32(6);
  EtchInt32* int2 = new EtchInt32(10);
  EtchInt32* temp = NULL;
  capu::int32_t elementsWritten = 0;
  capu::int32_t elementsRead = 0;

  EtchNativeArray<EtchInt32*> *na = new EtchNativeArray<EtchInt32*> (6);

  //test invalids
  //data == NULL
  ret = na->set(0,NULL,0,0,0,&elementsWritten);
  EXPECT_EQ(ETCH_EINVAL, ret);
  
  //data == NULL
  ret = na->get(0,NULL,0,0,0,&elementsRead);
  EXPECT_EQ(ETCH_EINVAL, ret);

  //buffer size smaller then length
  ret = na->set(0,arr2, 2, 0, 4, &elementsWritten);
  EXPECT_EQ(ETCH_ERANGE, ret);

  //offset + length out of range
  ret = na->set(0,arr2, 4, 3, 4, &elementsWritten);
  EXPECT_EQ(ETCH_ERANGE, ret);

  //buffer size smaller then length
  ret = na->get(0, arr2, 2, 0, 4, &elementsRead);
  EXPECT_EQ(ETCH_ERANGE, ret);

  //offset + length out of range
  ret  = na->get(0, arr2, 4, 3, 4, &elementsRead);
  EXPECT_EQ(ETCH_ERANGE, ret);

  //set Array
  ret = na->set(0, arr1, 6, 0, 6, &elementsWritten);
  EXPECT_EQ(ret, ETCH_OK);
  EXPECT_EQ(elementsWritten, 6);
  
  na->get(0,&temp);
  EXPECT_EQ(temp->get(), 1);
  na->set(0, int2);
  na->get(0,&temp);
  EXPECT_EQ(temp->get(), int2->get());

  //get Array
  ret = na->get(0, arr2, 4, 0, 4, &elementsRead);
  EXPECT_EQ(ret, ETCH_OK);
  EXPECT_EQ(elementsRead, 4);

  EXPECT_EQ(arr2[3]->get(), 4);

  delete na;
  delete arr1[0];
  delete arr1[1];
  delete arr1[2];
  delete arr1[3];
  delete arr1[4];
  delete arr1[5];
  delete[] arr1;
  delete int2;
  delete[] arr2;

}

TEST(EtchNativeArrayTest, nativeTypes) {

  status_t ret;
  //Test setget of block of native type
  EtchNativeArray<int> *na = new EtchNativeArray<int> (2);
  capu::int32_t arr1[2];
  capu::int32_t arr2[2];
  capu::int32_t elementsWritten = 0;
  capu::int32_t elementsRead = 0;

  arr1[0] = 3;
  arr1[1] = 5;

  ret = na->set(0, arr1,2, 0, 2, &elementsWritten);
  EXPECT_EQ(ETCH_OK, ret);

  ret = na->get(0, arr2, 2, 0, 2, &elementsRead);
  EXPECT_EQ(ETCH_OK, ret);

  EXPECT_EQ(3, arr2[0]);
  EXPECT_EQ(5, arr2[1]);

  delete na;
}

TEST(EtchNativeArrayTest, multiDimensionalArrays) {

  status_t ret;
  EtchInt32* int00 = new EtchInt32(1);
  EtchInt32* int01 = new EtchInt32(2);
  EtchInt32* int10 = new EtchInt32(3);
  EtchInt32* int11 = new EtchInt32(4);
  EtchInt32* temp  = NULL;

  EtchNativeArray<EtchInt32*>* main1 = new EtchNativeArray<EtchInt32*>(2, 2);

  main1->createArray(Pos(0), 2);
  main1->createArray(Pos(1), 2);

  main1->set(Pos(0, 0), int00);
  main1->set(Pos(0, 1), int01);
  main1->set(Pos(1, 0), int10);
  main1->set(Pos(1, 1), int11);

  ret = main1->get(Pos(1), &temp);
  EXPECT_EQ(ETCH_ERANGE, ret);

  ret = main1->get(Pos(1, 1), &temp);
  EXPECT_EQ(ETCH_OK, ret);
  EXPECT_EQ(4, temp->get());

  delete main1;

  EtchNativeArray<EtchInt32*>* main2 = new EtchNativeArray<EtchInt32*>(2, 3);

  // create dim2
  main2->createArray(Pos(0), 2);
  main2->createArray(Pos(1), 2);

  // create dim3
  main2->createArray(Pos(0, 0), 2);
  main2->createArray(Pos(0, 1), 2);
  main2->createArray(Pos(1, 0), 4);
  main2->createArray(Pos(1, 1), 4);

  main2->set(Pos(0, 0, 0), int00);
  main2->set(Pos(0, 0, 1), int01);
  main2->set(Pos(0, 1, 0), int10);
  main2->set(Pos(0, 1, 1), int11);
  main2->set(Pos(1, 1, 3), int11);

  ret = main2->get(Pos(1, 1), &temp);
  EXPECT_EQ(ETCH_ERANGE, ret);

  ret = main2->get(Pos(1, 1, 3), &temp);
  EXPECT_EQ(ETCH_OK, ret);
  EXPECT_EQ(4, temp->get());

  delete main2;

  delete int00;
  delete int01;
  delete int10;
  delete int11;

}

TEST(EtchNativeArrayTest, getSubarray) {
  status_t ret;
  EtchInt32* int1 = new EtchInt32(1);
  EtchInt32* int2 = new EtchInt32(2);
  EtchInt32* int3 = new EtchInt32(3);
  EtchInt32* int4 = new EtchInt32(4);
  EtchInt32* temp = NULL;

  EtchNativeArray<EtchInt32*>* mainArray = new EtchNativeArray<EtchInt32*>(2, 3);
  EtchNativeArray<EtchInt32*>* subArray = NULL;

  // create dim2
  mainArray->createArray(Pos(0), 2);
  mainArray->createArray(Pos(1), 2);

  // create dim3
  mainArray->createArray(Pos(0, 0), 2);
  mainArray->createArray(Pos(0, 1), 2);
  mainArray->createArray(Pos(1, 0), 4);
  mainArray->createArray(Pos(1, 1), 4);

  mainArray->set(Pos(0, 0, 0), int1);
  mainArray->set(Pos(0, 0, 1), int2);
  mainArray->set(Pos(0, 1, 0), int3);
  mainArray->set(Pos(0, 1, 1), int4);
  mainArray->set(Pos(1, 1, 3), int4);

  //get data element, must return error
  ret = mainArray->get(Pos(1, 1, 3), subArray);
  EXPECT_EQ(ETCH_ERANGE, ret);

  //get 2-dim subarray
  ret = mainArray->get(Pos(1), subArray);
  EXPECT_EQ(ETCH_OK, ret);
  //check data
  subArray->get(Pos(1,3),&temp);
  EXPECT_EQ(4, temp->get());

  delete mainArray;
  //check data again to make sure is has not been deleted by mainArray
  subArray->get(Pos(1,3),&temp);
  EXPECT_EQ(4, temp->get());

  delete subArray;

  delete int1;
  delete int2;
  delete int3;
  delete int4;
}

TEST(EtchNativeArrayTest, setSubarray) {
  status_t ret;
  EtchInt32* int1 = new EtchInt32(1);
  EtchInt32* int2 = new EtchInt32(2);
  EtchInt32* int3 = new EtchInt32(3);
  EtchInt32* int4 = new EtchInt32(4);
  EtchInt32* temp = NULL;

  EtchNativeArray<EtchInt32*>* mainArray = new EtchNativeArray<EtchInt32*>(2, 3);
  EtchNativeArray<EtchInt32*>* subArray = new EtchNativeArray<EtchInt32*>(2,1);
  EtchNativeArray<EtchInt32*>* subArrayDim2 = new EtchNativeArray<EtchInt32*>(2,2);

  // create dim2
  mainArray->createArray(Pos(0), 2);
  mainArray->createArray(Pos(1), 2);

  // create dim3
  mainArray->createArray(Pos(0, 0), 2);
  mainArray->createArray(Pos(0, 1), 2);
  mainArray->createArray(Pos(1, 0), 4);
  mainArray->createArray(Pos(1, 1), 4);

  //fill main array
  mainArray->set(Pos(0, 0, 0), int1);
  mainArray->set(Pos(0, 0, 1), int2);
  
  //fill sub array
  subArray->set(Pos(0),int3);
  subArray->set(Pos(0),int4);
  
  //set sub array in data array, must return error
  ret = mainArray->set(Pos(1, 0, 0), subArray);
  EXPECT_EQ(ETCH_ERANGE, ret);

  //set sub array with invalid dim
  ret = mainArray->set(Pos(1, 0), subArrayDim2);
  EXPECT_EQ(ETCH_ERANGE, ret);

  //set sub array with valid dim
  ret = mainArray->set(Pos(1, 0), subArray);
  EXPECT_EQ(ETCH_OK, ret);

  //check data
  ret = mainArray->get(Pos(1, 0, 0), &temp);
  EXPECT_EQ(ETCH_OK, ret);
  EXPECT_EQ(4, temp->get());

  delete mainArray;
  delete subArray;
  delete subArrayDim2;

  delete int1;
  delete int2;
  delete int3;
  delete int4;

}