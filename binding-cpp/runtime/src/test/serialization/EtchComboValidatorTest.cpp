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
#include "gtest/gtest.h"
#include "serialization/EtchComboValidator.h"
#include "serialization/EtchValidatorString.h"
#include "serialization/EtchValidatorInt.h"

class EtchComboValidatorTest
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

TEST_F(EtchComboValidatorTest, createTest) {
  capu::SmartPointer<EtchValidator> ptr;
  capu::SmartPointer<EtchValidator> ptr2;

  EtchValidatorString::Get(mRuntime, 0, ptr);
  EtchValidatorInt::Get(mRuntime, 0, ptr2);

  EtchComboValidator *combo = new EtchComboValidator(ptr, ptr2);
  EXPECT_TRUE(combo != NULL);
  delete combo;
}

TEST_F(EtchComboValidatorTest, validateTest) {
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
  capu::SmartPointer<EtchObject> str = NULL;
  capu::SmartPointer<EtchObject> str2 = new EtchString("hello");
  //
  capu::SmartPointer<EtchObject> byte1 = new EtchByte(capu::NumericLimits::Max<capu::int8_t>());
  capu::SmartPointer<EtchObject> byte2 = new EtchByte(0);
  capu::SmartPointer<EtchObject> byte3 = new EtchByte(capu::NumericLimits::Min<capu::int8_t>());
  capu::SmartPointer<EtchObject> byte4 = new EtchByte(32);

  //create combo validator
  EtchComboValidator *ptr = NULL;
  capu::SmartPointer<EtchValidator> ptr1;
  capu::SmartPointer<EtchValidator> ptr2;
  EtchValidatorInt::Get(mRuntime, 0, ptr1);
  EtchValidatorString::Get(mRuntime, 0, ptr2);
  ptr = new EtchComboValidator(ptr1, ptr2);

  EXPECT_FALSE(ptr->validate(byte));
  EXPECT_FALSE(ptr->validate(longInteger5));
  EXPECT_FALSE(ptr->validate(str)); //STRING VALIDATE
  EXPECT_TRUE(ptr->validate(str2));
  EXPECT_TRUE(ptr->validate(integer)); //INT VALIDATE
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
  delete ptr;
}

TEST_F(EtchComboValidatorTest, validateValueTest) {

  capu::SmartPointer<EtchObject> byte = NULL;
  capu::SmartPointer<EtchObject> result = NULL;
  capu::SmartPointer<EtchObject> integer = new EtchInt32(-128);
  capu::SmartPointer<EtchObject> integer2 = new EtchInt32(5);
  capu::SmartPointer<EtchObject> integer3 = new EtchInt32(127);
  capu::SmartPointer<EtchObject> byte2 = new EtchByte(3);

  //create combo validator
  EtchComboValidator *ptr = NULL;
  capu::SmartPointer<EtchValidator> ptr1;
  capu::SmartPointer<EtchValidator> ptr2;
  EtchValidatorInt::Get(mRuntime, 0, ptr1);
  EtchValidatorString::Get(mRuntime, 0, ptr2);
  ptr = new EtchComboValidator(ptr1, ptr2);
  //INT TESTS
  EXPECT_TRUE(ptr->validateValue(byte, result) == ETCH_ERROR);
  EXPECT_TRUE(ptr->validateValue(integer, result) == ETCH_OK);
  EXPECT_TRUE(((EtchInt32*) result.get())->get() == ((EtchInt32*) integer.get())->get());
  EXPECT_TRUE(ptr->validateValue(integer2, result) == ETCH_OK);
  EXPECT_TRUE(((EtchInt32*) result.get())->get() == ((EtchInt32*) integer2.get())->get());
  EXPECT_TRUE(ptr->validateValue(integer3, result) == ETCH_OK);
  EXPECT_TRUE(((EtchInt32*) result.get())->get() == ((EtchInt32*) integer3.get())->get());
  EXPECT_TRUE(ptr->validateValue(byte2, result) == ETCH_OK);
  EXPECT_TRUE(((EtchInt32*) result.get())->get() == ((EtchByte*) byte2.get())->get());

  capu::SmartPointer<EtchObject> str = NULL;
  capu::SmartPointer<EtchObject> str2 = new EtchString("hello");
  //STRING TESTS
  EXPECT_TRUE(ptr->validateValue(str, result) == ETCH_ERROR);
  EXPECT_TRUE(ptr->validateValue(str2, result) == ETCH_OK);
  delete ptr;
}

TEST_F(EtchComboValidatorTest, elementValidatorTest) {
  //create combo validator
  EtchComboValidator *ptr = NULL;
  capu::SmartPointer<EtchValidator> ptr1;
  capu::SmartPointer<EtchValidator> ptr2;
  EtchValidatorInt::Get(mRuntime, 1, ptr1);
  EtchValidatorString::Get(mRuntime, 1, ptr2);
  ptr = new EtchComboValidator(ptr1, ptr2);
  capu::SmartPointer<EtchValidator> element_validator;
  ptr->getElementValidator(element_validator);


  capu::SmartPointer<EtchObject> str = NULL;
  capu::SmartPointer<EtchObject> str2 = new EtchString("hello");

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

  capu::SmartPointer<EtchObject> byte1 = new EtchByte(capu::NumericLimits::Max<capu::int8_t>());
  capu::SmartPointer<EtchObject> byte2 = new EtchByte(0);
  capu::SmartPointer<EtchObject> byte3 = new EtchByte(capu::NumericLimits::Min<capu::int8_t>());
  capu::SmartPointer<EtchObject> byte4 = new EtchByte(32);
  //INT TEST
  EXPECT_FALSE(element_validator->validate(longInteger5));
  EXPECT_TRUE(element_validator->validate(integer));
  EXPECT_TRUE(element_validator->validate(integer2));
  EXPECT_TRUE(element_validator->validate(integer3));
  EXPECT_TRUE(element_validator->validate(longInteger));
  EXPECT_TRUE(element_validator->validate(longInteger2));
  EXPECT_TRUE(element_validator->validate(longInteger3));
  EXPECT_TRUE(element_validator->validate(shortInteger));
  EXPECT_TRUE(element_validator->validate(shortInteger2));
  EXPECT_TRUE(element_validator->validate(shortInteger3));
  EXPECT_TRUE(element_validator->validate(byte1));
  EXPECT_TRUE(element_validator->validate(byte2));
  EXPECT_TRUE(element_validator->validate(byte3));
  EXPECT_TRUE(element_validator->validate(byte4));
  EXPECT_TRUE(element_validator->validate(integer4));
  EXPECT_TRUE(element_validator->validate(longInteger4));
  EXPECT_TRUE(element_validator->validate(shortInteger4));
  //STRING TEST
  EXPECT_FALSE(element_validator->validate(str));
  EXPECT_TRUE(element_validator->validate(str2));
  delete ptr;
}
