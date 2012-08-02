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
#include "capu/container/Array.h"
#include "capu/Error.h"
#include "capu/Config.h"

TEST(Array, Constructor) {
  capu::Array<capu::uint32_t> emptyArray;
  EXPECT_EQ(0u, emptyArray.size());

  capu::Array<capu::uint32_t> *myArray = NULL;
  myArray = new capu::Array<capu::uint32_t>(10);
  EXPECT_TRUE(myArray != NULL);
  EXPECT_EQ(10u,myArray->size());
  delete myArray;
}

TEST(Array, SetAndGetValues) {
  capu::Array<capu::uint32_t> myArray(10);
  myArray.set(5);
  for (capu::int32_t i = 0; i< 10; i++) {
    EXPECT_EQ(5u,myArray[i]);
  }

  EXPECT_EQ(capu::CAPU_OK, myArray.set(3,0,3));
  EXPECT_EQ(3u,myArray[0]);
  EXPECT_EQ(3u,myArray[1]);
  EXPECT_EQ(3u,myArray[2]);

  EXPECT_EQ(capu::CAPU_OK, myArray.set(7,7,3));
  EXPECT_EQ(7u,myArray[7]);
  EXPECT_EQ(7u,myArray[8]);
  EXPECT_EQ(7u,myArray[9]);

  EXPECT_EQ(capu::CAPU_OK, myArray.set(4,4,3));
  EXPECT_EQ(4u,myArray[4]);
  EXPECT_EQ(4u,myArray[5]);
  EXPECT_EQ(4u,myArray[6]);

  capu::Array<capu::uint32_t> myArray2(20);
  myArray2.set(5);
  for (capu::int32_t i = 0; i< 20; i++) {
    EXPECT_EQ(5u,myArray2[i]);
  }

  EXPECT_EQ(capu::CAPU_OK,myArray2.set(3,0,3));
  EXPECT_EQ(3u,myArray2[0]);
  EXPECT_EQ(3u,myArray2[1]);
  EXPECT_EQ(3u,myArray2[2]);

  EXPECT_EQ(capu::CAPU_OK,myArray2.set(7,17,3));
  EXPECT_EQ(7u,myArray2[17]);
  EXPECT_EQ(7u,myArray2[18]);
  EXPECT_EQ(7u,myArray2[19]);

  EXPECT_EQ(capu::CAPU_OK,myArray2.set(4,5,12));
  EXPECT_EQ(5u,myArray2[4]);
  EXPECT_EQ(4u,myArray2[5]);
  EXPECT_EQ(4u,myArray2[6]);
  EXPECT_EQ(4u,myArray2[7]);
  EXPECT_EQ(4u,myArray2[8]);
  EXPECT_EQ(4u,myArray2[9]);
  EXPECT_EQ(4u,myArray2[10]);
  EXPECT_EQ(4u,myArray2[11]);
  EXPECT_EQ(4u,myArray2[12]);
  EXPECT_EQ(4u,myArray2[13]);
  EXPECT_EQ(4u,myArray2[14]);
  EXPECT_EQ(4u,myArray2[15]);
  EXPECT_EQ(4u,myArray2[16]);
  EXPECT_EQ(7u,myArray2[17]);

  myArray2[0] = 1;
  myArray2[1] = 2;

  EXPECT_EQ(1u,myArray2[0]);
  EXPECT_EQ(2u,myArray2[1]);

  EXPECT_EQ(capu::CAPU_OK,myArray2.set(4,0,20));
  EXPECT_EQ(capu::CAPU_ERANGE,myArray2.set(4,0,21));
  EXPECT_EQ(capu::CAPU_ERANGE,myArray2.set(4,1,20));
}

TEST(Array, Swap) {
  capu::Array<capu::uint32_t> myArray(10);
  myArray.set(5);
  myArray[3] = 10;
  myArray[7] = 15;
  EXPECT_EQ(10u,myArray[3]);
  EXPECT_EQ(15u,myArray[7]);

  EXPECT_EQ(capu::CAPU_OK,myArray.swap(3,7));
  EXPECT_EQ(15u,myArray[3]);
  EXPECT_EQ(10u,myArray[7]);

  EXPECT_EQ(capu::CAPU_ERANGE,myArray.swap(13,17));
  EXPECT_EQ(capu::CAPU_ERANGE,myArray.swap(3,17));
  EXPECT_EQ(capu::CAPU_ERANGE,myArray.swap(13,7));
}

TEST(Array, Move) {
  capu::Array<capu::uint32_t> myArray(10);
  myArray.set(5);

  myArray[0] = 10;
  myArray[1] = 15;
  myArray[2] = 20;

  EXPECT_EQ(capu::CAPU_OK,myArray.move(0,3,7));
  EXPECT_EQ(10u,myArray[7]);
  EXPECT_EQ(15u,myArray[8]);
  EXPECT_EQ(20u,myArray[9]);

  EXPECT_EQ(capu::CAPU_ERANGE,myArray.move(13,3,7));
  EXPECT_EQ(capu::CAPU_ERANGE,myArray.move(0,13,7));
  EXPECT_EQ(capu::CAPU_ERANGE,myArray.move(0,3,13));
}

TEST(Array, CopyConstructor)
{

    capu::Array<capu::uint32_t> myArray(10);
    myArray.set(5);

    capu::Array<capu::uint32_t> myArray2(myArray);

    EXPECT_EQ(5u, myArray2[0]);
    EXPECT_EQ(5u, myArray2[4]);
    EXPECT_EQ(5u, myArray2[9]);
}

TEST(Array, Assignment)
{
    capu::Array<capu::uint32_t> myEmptyArray;

    capu::Array<capu::uint32_t> myArray(10);
    myArray.set(5);

    myEmptyArray = myArray;

    EXPECT_EQ(5u, myEmptyArray[0]);
    EXPECT_EQ(5u, myEmptyArray[4]);
    EXPECT_EQ(5u, myEmptyArray[9]);

    capu::Array<capu::uint32_t> myArray2(10);
    myArray2 = myArray;

    EXPECT_EQ(5u, myArray2[0]);
    EXPECT_EQ(5u, myArray2[4]);
    EXPECT_EQ(5u, myArray2[9]);
}

TEST(Array, CopyMethodNormal)
{
    capu::Array<capu::uint32_t> original(5);
    capu::uint32_t newVals[5];

    // fill with default values
    for(capu::uint32_t i=0;i<original.size();i++)
    {
        original[i] = i;
        newVals[i] = i+1;
    }

    capu::status_t status = original.copy(newVals, original.size());
    EXPECT_EQ(capu::CAPU_OK, status);

    for(capu::uint32_t i=0;i<original.size();i++)
    {
        EXPECT_EQ(i+1, original[i]);
    }
}

TEST(Array, CopyMethodNormalWithBiggerArray)
{
    capu::Array<capu::uint32_t> original(5);
    capu::uint32_t newVals[50]; // this one's bigger

    // fill with default values
    for(capu::uint32_t i=0;i<original.size();i++)
    {
        original[i] = i;
    }
    for(capu::uint32_t i=0;i<50;i++)
    {
        newVals[i] = i+1;
    }

    capu::status_t status = original.copy(newVals, original.size()); // only copy the first 5 values
    EXPECT_EQ(capu::CAPU_OK, status);

    for(capu::uint32_t i=0;i<original.size();i++)
    {
        EXPECT_EQ(i+1, original[i]);
    }
}

TEST(Array, CopyMethodInvalidSize)
{
    capu::Array<capu::uint32_t> original(5);
    for(capu::uint32_t i=0;i<original.size();i++)
    {
        original[i] = i;
    }
    capu::uint32_t newVals[3];

    capu::status_t status = original.copy(newVals, 3); // give invalid size
    EXPECT_EQ(capu::CAPU_ERANGE, status);

    // no changes in original array
    for(capu::uint32_t i=0;i<original.size();i++)
    {
        EXPECT_EQ(i, original[i]);
    }
}

TEST(Array, CopyMethodZeroSizeNormal)
{
    capu::Array<capu::uint32_t> original(0);
    capu::uint32_t newVals[1];

    capu::status_t status = original.copy(newVals, original.size()); // copy with zero, no error expected
    EXPECT_EQ(capu::CAPU_OK, status);
}

TEST(Array, CopyMethodZeroSizeInvalidSize)
{
    capu::Array<capu::uint32_t> original(0);
    capu::uint32_t newVals[1];

    capu::status_t status = original.copy(newVals, 1);
    EXPECT_EQ(capu::CAPU_ERANGE, status);
}

TEST(Array, SetRawData)
{
    capu::Array<capu::uint32_t> array(3u);

    array.setRawData(0);

    EXPECT_EQ(0u, array[0]);
    EXPECT_EQ(0u, array[1]);
    EXPECT_EQ(0u, array[2]);
}
