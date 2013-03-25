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
#include "serialization/EtchValidatorLong.h"
#include "capu/util/SmartPointer.h"
#include "common/EtchString.h"
#include "support/EtchRuntime.h"

class EtchValidatorLongTest
  : public ::testing::Test {
protected:
  virtual void SetUp() {
    mRuntime = new EtchRuntime();
    mRuntime->start();
  }

  virtual void TearDown() {
    mRuntime->shutdown();
    delete mRuntime;
    mRuntime = NULL;
  }

  EtchRuntime* mRuntime;
};

TEST_F(EtchValidatorLongTest, createTest) {
  capu::SmartPointer<EtchValidatorLong> ptr = NULL;

  capu::SmartPointer<EtchValidator> val;
  EXPECT_TRUE(EtchValidatorLong::Get(mRuntime, 0, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorLong>(val);

  EXPECT_TRUE(ptr->getExpectedType()->equals(EtchLong::TYPE()));
  EXPECT_TRUE(ptr->getNDims() == 0);

  EtchObjectType type1(EOTID_LONG, NULL);
  EtchObjectType type2(EOTID_NATIVE_ARRAY, &type1);
  EXPECT_TRUE(EtchValidatorLong::Get(mRuntime, 2, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorLong>(val);
  EXPECT_TRUE(ptr->getExpectedType()->equals(&type2));
  EXPECT_TRUE(ptr->getNDims() == 2);

}

TEST_F(EtchValidatorLongTest, validateTest) {
  capu::SmartPointer<EtchObject> byte = NULL;

  capu::SmartPointer<EtchObject> integer = new EtchInt32(capu::NumericLimits::Min<capu::int32_t>());
  capu::SmartPointer<EtchObject> integer2 = new EtchInt32(0);
  capu::SmartPointer<EtchObject> integer3 = new EtchInt32(capu::NumericLimits::Max<capu::int32_t>());
  capu::SmartPointer<EtchObject> integer4 = new EtchInt32(897);

  capu::SmartPointer<EtchObject> longInteger = new EtchLong(capu::NumericLimits::Min<capu::int32_t>());
  capu::SmartPointer<EtchObject> longInteger2 = new EtchLong(0);
  capu::SmartPointer<EtchObject> longInteger3 = new EtchLong(capu::NumericLimits::Max<capu::int32_t>());
  capu::SmartPointer<EtchObject> longInteger4 = new EtchLong(897);
  //exceed limits of integer
  capu::SmartPointer<EtchObject> longInteger5 = new EtchLong((capu::int64_t)capu::NumericLimits::Max<capu::int32_t>() + (capu::int64_t)2);

  capu::SmartPointer<EtchObject> shortInteger = new EtchShort(capu::NumericLimits::Min<capu::int16_t>());
  capu::SmartPointer<EtchObject> shortInteger2 = new EtchShort(0);
  capu::SmartPointer<EtchObject> shortInteger3 = new EtchShort(capu::NumericLimits::Max<capu::int16_t>());
  capu::SmartPointer<EtchObject> shortInteger4 = new EtchShort();

  //incompatible type
  capu::SmartPointer<EtchObject> str = new EtchString();

  capu::SmartPointer<EtchObject> byte1 = new EtchByte(capu::NumericLimits::Max<capu::int8_t>());
  capu::SmartPointer<EtchObject> byte2 = new EtchByte(0);
  capu::SmartPointer<EtchObject> byte3 = new EtchByte(capu::NumericLimits::Min<capu::int8_t>());
  capu::SmartPointer<EtchObject> byte4 = new EtchByte(32);
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorLong::Get(mRuntime, 0, ptr) == ETCH_OK);
  EXPECT_TRUE((capu::smartpointer_cast<EtchTypeValidator>(ptr))->getNDims() == 0);
  EXPECT_FALSE(ptr->validate(byte));
  EXPECT_FALSE(ptr->validate(str));
  EXPECT_TRUE(ptr->validate(longInteger5));
  EXPECT_TRUE(ptr->validate(integer));
  EXPECT_TRUE(ptr->validate(integer2));
  EXPECT_TRUE(ptr->validate(integer3));
  EXPECT_TRUE(ptr->validate(longInteger));
  EXPECT_TRUE(ptr->validate(longInteger2));
  EXPECT_TRUE(ptr->validate(longInteger3));
  EXPECT_TRUE(ptr->validate(shortInteger));
  EXPECT_TRUE(ptr->validate(shortInteger2));
  EXPECT_TRUE(ptr->validate(shortInteger3));
  EXPECT_TRUE(ptr->validate(byte1));
  EXPECT_TRUE(ptr->validate(byte2));
  EXPECT_TRUE(ptr->validate(byte3));
  EXPECT_TRUE(ptr->validate(byte4));
  EXPECT_TRUE(ptr->validate(integer4));
  EXPECT_TRUE(ptr->validate(longInteger4));
  EXPECT_TRUE(ptr->validate(shortInteger4));
}

TEST_F(EtchValidatorLongTest, validateValueTest) {
  capu::SmartPointer<EtchObject> byte = NULL;
  capu::SmartPointer<EtchObject> result;
  capu::SmartPointer<EtchObject> integer = new EtchInt32(-128);
  capu::SmartPointer<EtchObject> integer2 = new EtchInt32(5);
  capu::SmartPointer<EtchObject> integer3 = new EtchInt32(127);
  capu::SmartPointer<EtchObject> byte2 = new EtchByte(3);
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorLong::Get(mRuntime, 0, ptr) == ETCH_OK);
  EXPECT_TRUE(ptr->validateValue(byte, result) == ETCH_ERROR);
  EXPECT_TRUE(ptr->validateValue(integer, result) == ETCH_OK);
  EXPECT_TRUE(((EtchLong*) result.get())->get() == ((EtchInt32*) integer.get())->get());
  EXPECT_TRUE(ptr->validateValue(integer2, result) == ETCH_OK);
  EXPECT_TRUE(((EtchLong*) result.get())->get() == ((EtchInt32*) integer2.get())->get());
  EXPECT_TRUE(ptr->validateValue(integer3, result) == ETCH_OK);
  EXPECT_TRUE(((EtchLong*) result.get())->get() == ((EtchInt32*) integer3.get())->get());
  EXPECT_TRUE(ptr->validateValue(byte2, result) == ETCH_OK);
  EXPECT_TRUE(((EtchLong*) result.get())->get() == ((EtchByte*) byte2.get())->get());
}

TEST_F(EtchValidatorLongTest, elementValidatorTest) {
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorLong::Get(mRuntime, 1, ptr) == ETCH_OK);
  capu::SmartPointer<EtchValidator> elementValidator;
  ptr->getElementValidator(elementValidator);

  capu::SmartPointer<EtchObject> integer = new EtchInt32(capu::NumericLimits::Min<capu::int32_t>());
  capu::SmartPointer<EtchObject> integer2 = new EtchInt32(0);
  capu::SmartPointer<EtchObject> integer3 = new EtchInt32(capu::NumericLimits::Max<capu::int32_t>());
  capu::SmartPointer<EtchObject> integer4 = new EtchInt32(897);

  capu::SmartPointer<EtchObject> longInteger = new EtchLong(capu::NumericLimits::Min<capu::int32_t>());
  capu::SmartPointer<EtchObject> longInteger2 = new EtchLong(0);
  capu::SmartPointer<EtchObject> longInteger3 = new EtchLong(capu::NumericLimits::Max<capu::int32_t>());
  capu::SmartPointer<EtchObject> longInteger4 = new EtchLong(897);
  //exceed limits of integer
  capu::SmartPointer<EtchObject> longInteger5 = new EtchLong((capu::int64_t)capu::NumericLimits::Max<capu::int32_t>() + (capu::int64_t)2);

  capu::SmartPointer<EtchObject> shortInteger = new EtchShort(capu::NumericLimits::Min<capu::int16_t>());
  capu::SmartPointer<EtchObject> shortInteger2 = new EtchShort(0);
  capu::SmartPointer<EtchObject> shortInteger3 = new EtchShort(capu::NumericLimits::Max<capu::int16_t>());
  capu::SmartPointer<EtchObject> shortInteger4 = new EtchShort();

  //incompatible type
  capu::SmartPointer<EtchObject> str = new EtchString();

  capu::SmartPointer<EtchObject> byte1 = new EtchByte(capu::NumericLimits::Max<capu::int8_t>());
  capu::SmartPointer<EtchObject> byte2 = new EtchByte(0);
  capu::SmartPointer<EtchObject> byte3 = new EtchByte(capu::NumericLimits::Min<capu::int8_t>());
  capu::SmartPointer<EtchObject> byte4 = new EtchByte(32);

  EXPECT_FALSE(elementValidator->validate(str));
  EXPECT_TRUE(elementValidator->validate(longInteger5));
  EXPECT_TRUE(elementValidator->validate(integer));
  EXPECT_TRUE(elementValidator->validate(integer2));
  EXPECT_TRUE(elementValidator->validate(integer3));
  EXPECT_TRUE(elementValidator->validate(longInteger));
  EXPECT_TRUE(elementValidator->validate(longInteger2));
  EXPECT_TRUE(elementValidator->validate(longInteger3));
  EXPECT_TRUE(elementValidator->validate(shortInteger));
  EXPECT_TRUE(elementValidator->validate(shortInteger2));
  EXPECT_TRUE(elementValidator->validate(shortInteger3));
  EXPECT_TRUE(elementValidator->validate(byte1));
  EXPECT_TRUE(elementValidator->validate(byte2));
  EXPECT_TRUE(elementValidator->validate(byte3));
  EXPECT_TRUE(elementValidator->validate(byte4));
  EXPECT_TRUE(elementValidator->validate(integer4));
  EXPECT_TRUE(elementValidator->validate(longInteger4));
  EXPECT_TRUE(elementValidator->validate(shortInteger4));
}


