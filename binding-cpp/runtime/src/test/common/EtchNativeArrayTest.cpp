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

TEST(EtchNativeArrayTest, SetGetResize_Base) {
  EtchNativeArray<EtchInt32>* na = new EtchNativeArray<EtchInt32>(2);
  EXPECT_EQ(ETCH_OK, na->set(0, 4));
  EtchNativeArrayBase* nab = static_cast<EtchNativeArrayBase*>(na);
  capu::SmartPointer<EtchObject> o;
  EXPECT_EQ(ETCH_OK, nab->getBase(0, o));
  EXPECT_EQ(ETCH_OK, nab->setBase(0, o));

  EtchInt32 int32Value;
  EXPECT_EQ(ETCH_OK, na->get(0, &int32Value));
  EXPECT_EQ(4, int32Value.get());

  //test resizing
  EXPECT_EQ(ETCH_OK, na->resize(10));
  EXPECT_EQ(10, na->getLength());
  EtchInt32 value;
  EXPECT_EQ(ETCH_OK, na->get(0,&value));
  EXPECT_EQ(4, value.get());

  EtchNativeArray<EtchInt32*>* na1 = new EtchNativeArray<EtchInt32*>(2);
  EtchInt32* i1 = new EtchInt32(3);
  EXPECT_EQ(ETCH_OK,na1->set(0, i1));
  EtchNativeArrayBase* nab1 = static_cast<EtchNativeArrayBase*>(na1);
  capu::SmartPointer<EtchObject> o1;
  EXPECT_EQ(ETCH_OK, nab1->getBase(0, o1));
  EXPECT_EQ(ETCH_OK, nab1->setBase(0, o1));

  EtchNativeArray<capu::SmartPointer<EtchInt32> >* na2 = new EtchNativeArray<capu::SmartPointer<EtchInt32> >(2);
  EXPECT_EQ(ETCH_OK, na2->set(0, new EtchInt32(3)));
  EtchNativeArrayBase* nab2 = static_cast<EtchNativeArrayBase*>(na2);
  capu::SmartPointer<EtchObject> o2;
  EXPECT_EQ(ETCH_OK, nab2->getBase(0, o2));
  EXPECT_EQ(ETCH_OK, nab2->setBase(0, o2));

  EtchNativeArray<capu::int8_t>* na3 = new EtchNativeArray<capu::int8_t>(2);
  EXPECT_EQ(ETCH_OK, na3->set(0, 4));
  EtchNativeArrayBase* nab3 = static_cast<EtchNativeArrayBase*>(na3);
  capu::SmartPointer<EtchObject> o3;
  EXPECT_EQ(ETCH_OK, nab3->getBase(0, o3));
  EXPECT_EQ(ETCH_OK, nab3->setBase(0, o3));

  capu::int8_t* ni1 = new capu::int8_t(5);
  EtchNativeArray<capu::int8_t*>* na4 = new EtchNativeArray<capu::int8_t*>(2);
  EXPECT_EQ(ETCH_OK, na4->set(0, ni1));
  EtchNativeArrayBase* nab4 = static_cast<EtchNativeArrayBase*>(na4);
  capu::SmartPointer<EtchObject> o4;
  EXPECT_EQ(ETCH_OK, nab4->getBase(0, o4));
  EXPECT_EQ(ETCH_OK, nab4->setBase(0, o4));

  capu::SmartPointer<capu::int8_t> si1 = new capu::int8_t(5);
  EtchNativeArray<capu::SmartPointer<capu::int8_t> >* na5 = new EtchNativeArray<capu::SmartPointer<capu::int8_t> >(2);
  EXPECT_EQ(ETCH_OK, na5->set(0, si1));
  EtchNativeArrayBase* nab5 = static_cast<EtchNativeArrayBase*>(na5);
  capu::SmartPointer<EtchObject> o5;
  EXPECT_EQ(ETCH_OK, nab5->getBase(0, o5));
  EXPECT_EQ(ETCH_OK, nab5->setBase(0, o5));

  delete na;
  delete i1;
  delete ni1;
  delete na1;
  delete na2;
  delete na3;
  delete na4;
  delete na5;
}

TEST(EtchNativeArrayTest, Constructor_Array) {

  EtchInt32* int1 = new EtchInt32(42);
  EtchInt32* int2 = new EtchInt32(43);

  EtchNativeArray<EtchInt32*>* na1 = new EtchNativeArray<EtchInt32* >(2);
  EXPECT_TRUE(na1->getObjectType()->equals(EtchNativeArray<EtchInt32* >::TYPE()));

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


  //native type array with dim = 1 and length = 2
  capu::int8_t nativeArray[2];
  nativeArray[0] = 1;
  nativeArray[1] = 2;

  EtchNativeArray<capu::int8_t> *na2 = new EtchNativeArray<capu::int8_t>(2, 1, (capu::int8_t*)nativeArray);

  EXPECT_EQ(1u, na2->getDim());
  EXPECT_EQ(2, na2->getLength());

  capu::int8_t nativeResult;
  EXPECT_EQ(ETCH_OK, na2->get(0,&nativeResult));
  EXPECT_EQ(1, nativeResult);
  EXPECT_EQ(ETCH_OK, na2->get(1,&nativeResult));
  EXPECT_EQ(2, nativeResult);

  delete na2;

  //native type array with dim = 2 and length = 2
  capu::int8_t nativeArray2[2][2];
  nativeArray2[0][0] = 0;
  nativeArray2[0][1] = 1;
  nativeArray2[1][0] = 2;
  nativeArray2[1][1] = 3;

  EtchNativeArray<capu::int8_t> *na3 = new EtchNativeArray<capu::int8_t>(2, 2, (capu::int8_t*)nativeArray2);

  EXPECT_EQ(2u, na3->getDim());
  EXPECT_EQ(2, na3->getLength());

  EXPECT_EQ(ETCH_OK, na3->get(Pos(0,0),&nativeResult));
  EXPECT_EQ(0, nativeResult);
  EXPECT_EQ(ETCH_OK, na3->get(Pos(0,1),&nativeResult));
  EXPECT_EQ(1, nativeResult);
  EXPECT_EQ(ETCH_OK, na3->get(Pos(1,0),&nativeResult));
  EXPECT_EQ(2, nativeResult);
  EXPECT_EQ(ETCH_OK, na3->get(Pos(1,1),&nativeResult));
  EXPECT_EQ(3, nativeResult);

  delete na3;

}

TEST(EtchNativeArrayTest, setGetSingleValue) {
  EtchInt32* int1 = new EtchInt32(42);
  EtchInt32* int2 = new EtchInt32(43);
  EtchNativeArray<EtchInt32* >* na1 = new EtchNativeArray<EtchInt32* >(2);

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

  EtchNativeArray<EtchInt32* > *na = new EtchNativeArray<EtchInt32* > (6);

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
  EtchNativeArray<capu::int8_t> *na = new EtchNativeArray<capu::int8_t> (2);
  capu::int8_t arr1[2];
  capu::int8_t arr2[2];
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
  EtchInt32 *int00  = new EtchInt32(1);
  EtchInt32 *int01  = new EtchInt32(2);
  EtchInt32 *int10  = new EtchInt32(3);
  EtchInt32 *int11  = new EtchInt32(4);
  EtchInt32 *int000 = new EtchInt32(1);
  EtchInt32 *int001 = new EtchInt32(2);
  EtchInt32 *int010 = new EtchInt32(3);
  EtchInt32 *int011 = new EtchInt32(4);
  EtchInt32 *int113 = new EtchInt32(5);
  capu::SmartPointer<EtchInt32> temp  = NULL;

  EtchNativeArray<capu::SmartPointer<EtchInt32> >* main1 = new EtchNativeArray<capu::SmartPointer<EtchInt32> >(2, 2);

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

  //test setBase and getBase of EtchNativeArrayBase
  EtchNativeArrayBase* main1b = (EtchNativeArrayBase*)main1;
  capu::SmartPointer<EtchObject> o1;
  EXPECT_EQ(ETCH_OK, main1b->getBase(0, o1));
  capu::SmartPointer<EtchObject> o2;
  EtchNativeArrayBase* sub1b = (EtchNativeArrayBase*)o1.get();
  EXPECT_EQ(ETCH_OK, sub1b->getBase(0,o2));
  capu::SmartPointer<EtchInt32> o3 = capu::smartpointer_cast<EtchInt32>(o2);
  EXPECT_EQ(1,o3->get());

  //test resizing
  EXPECT_EQ(ETCH_OK, main1->resize(10));
  EXPECT_EQ(10, main1->getLength());

  //check data after resizing
  ret = main1->get(Pos(1, 1), &temp);
  EXPECT_EQ(ETCH_OK, ret);
  EXPECT_EQ(4, temp->get());

  EtchNativeArray<capu::SmartPointer<EtchInt32> >* main2 = new EtchNativeArray<capu::SmartPointer<EtchInt32> >(2, 3);

  // create dim2
  main2->createArray(Pos(0), 2);
  main2->createArray(Pos(1), 2);

  // create dim3
  main2->createArray(Pos(0, 0), 2);
  main2->createArray(Pos(0, 1), 2);
  main2->createArray(Pos(1, 0), 4);
  main2->createArray(Pos(1, 1), 4);

  main2->set(Pos(0, 0, 0), int000);
  main2->set(Pos(0, 0, 1), int001);
  main2->set(Pos(0, 1, 0), int010);
  main2->set(Pos(0, 1, 1), int011);
  main2->set(Pos(1, 1, 3), int113);

  ret = main2->get(Pos(1, 1), &temp);
  EXPECT_EQ(ETCH_ERANGE, ret);

  ret = main2->get(Pos(1, 1, 3), &temp);
  EXPECT_EQ(ETCH_OK, ret);
  EXPECT_EQ(5, temp->get());

  delete main1;
  delete main2;

}


TEST(EtchNativeArrayTest, getSubarray) {
  status_t ret;
  EtchInt32* int1 = new EtchInt32(1);
  EtchInt32* int2 = new EtchInt32(2);
  EtchInt32* int3 = new EtchInt32(3);
  EtchInt32* int4 = new EtchInt32(4);
  EtchInt32* temp = NULL;

  EtchNativeArray<EtchInt32*>* mainArray = new EtchNativeArray<EtchInt32*>(2, 3);
  capu::SmartPointer<EtchNativeArray<EtchInt32*> > subArray = NULL;

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

  EXPECT_EQ(subArray->getObjectType()->getObjectComponentType()->getTypeId(),mainArray->getObjectType()->getObjectComponentType()->getTypeId());

  delete mainArray;
  //check data again to make sure is has not been deleted by mainArray
  subArray->get(Pos(1,3),&temp);
  EXPECT_EQ(4, temp->get());

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

  capu::int8_t nativeTypesArray[2][2][2];
  nativeTypesArray[0][0][0] = 1;
  nativeTypesArray[0][0][1] = 2;
  nativeTypesArray[0][1][0] = 3;
  nativeTypesArray[0][1][1] = 4;
  nativeTypesArray[1][0][0] = 5;
  nativeTypesArray[1][0][1] = 6;
  nativeTypesArray[1][1][0] = 7;
  nativeTypesArray[1][1][1] = 8;

  EtchNativeArray<capu::int8_t> *na = new EtchNativeArray<capu::int8_t>(2,3);
  na->createArray(0,2);
  na->createArray(1,2);
  na->createArray(Pos(0,0),2);
  na->createArray(Pos(0,1),2);
  na->createArray(Pos(1,0),2);
  na->createArray(Pos(1,1),2);

  EXPECT_EQ(ETCH_OK, na->set(Pos(0,0), (capu::int8_t*) nativeTypesArray[0][0], 2, 0, 2));
  capu::int8_t returnValue;
  EXPECT_EQ(ETCH_OK, na->get(Pos(0,0,0), &returnValue));
  EXPECT_EQ(1, returnValue);
  EXPECT_EQ(ETCH_OK, na->get(Pos(0,0,1), &returnValue));
  EXPECT_EQ(2, returnValue);

  delete na;


}
