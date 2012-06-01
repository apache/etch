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

TEST(EtchComboValidatorTest, createTest) {
  capu::SmartPointer<EtchValidator> ptr;
  capu::SmartPointer<EtchValidator> ptr2;

  EtchValidatorString::Get(0, ptr);
  EtchValidatorInt::Get(0, ptr2);

  EtchComboValidator *combo = new EtchComboValidator(ptr, ptr2);
  EXPECT_TRUE(combo != NULL);
  delete combo;
}

TEST(EtchComboValidatorTest, validateTest) {
  EtchObject* byte = NULL;

  EtchObject* integer = new EtchInt32(capu::NumericLimitMin<capu::int32_t>());
  EtchObject* integer2 = new EtchInt32(0);
  EtchObject* integer3 = new EtchInt32(capu::NumericLimitMax<capu::int32_t>());
  EtchObject* integer4 = new EtchInt32(897);

  EtchObject* longInteger = new EtchLong(capu::NumericLimitMin<capu::int32_t>());
  EtchObject* longInteger2 = new EtchLong(0);
  EtchObject* longInteger3 = new EtchLong(capu::NumericLimitMax<capu::int32_t>());
  EtchObject* longInteger4 = new EtchLong(897);
  //exceed limits of integer
  EtchObject* longInteger5 = new EtchLong((capu::int64_t)capu::NumericLimitMax<capu::int32_t>() + (capu::int64_t)2);

  EtchObject* shortInteger = new EtchShort(capu::NumericLimitMin<capu::int16_t>());
  EtchObject* shortInteger2 = new EtchShort(0);
  EtchObject* shortInteger3 = new EtchShort(capu::NumericLimitMax<capu::int16_t>());
  EtchObject* shortInteger4 = new EtchShort();

  //incompatible type
  EtchObject* str = NULL;
  EtchObject* str2 = new EtchString("hello");
  //
  EtchObject* byte1 = new EtchByte(capu::NumericLimitMax<capu::int8_t>());
  EtchObject* byte2 = new EtchByte(0);
  EtchObject* byte3 = new EtchByte(capu::NumericLimitMin<capu::int8_t>());
  EtchObject* byte4 = new EtchByte(32);

  //create combo validator
  EtchComboValidator *ptr = NULL;
  capu::SmartPointer<EtchValidator> ptr1;
  capu::SmartPointer<EtchValidator> ptr2;
  EtchValidatorInt::Get(0, ptr1);
  EtchValidatorString::Get(0, ptr2);
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

  delete integer;
  delete integer2;
  delete integer3;
  delete integer4;

  delete longInteger;
  delete longInteger2;
  delete longInteger3;
  delete longInteger4;
  delete longInteger5;

  delete shortInteger;
  delete shortInteger2;
  delete shortInteger3;
  delete shortInteger4;

  delete str2;
  delete byte1;
  delete byte2;
  delete byte3;
  delete byte4;
}

TEST(EtchComboValidatorTest, validateValueTest) {

  EtchObject* byte = NULL;
  EtchObject* result = NULL;
  EtchObject* integer = new EtchInt32(-128);
  EtchObject* integer2 = new EtchInt32(5);
  EtchObject* integer3 = new EtchInt32(127);
  EtchObject* byte2 = new EtchByte(3);

  //create combo validator
  EtchComboValidator *ptr = NULL;
  capu::SmartPointer<EtchValidator> ptr1;
  capu::SmartPointer<EtchValidator> ptr2;
  EtchValidatorInt::Get(0, ptr1);
  EtchValidatorString::Get(0, ptr2);
  ptr = new EtchComboValidator(ptr1, ptr2);
  //INT TESTS
  EXPECT_TRUE(ptr->validateValue(byte, result) == ETCH_ERROR);
  EXPECT_TRUE(ptr->validateValue(integer, result) == ETCH_OK);
  EXPECT_TRUE(((EtchInt32*) result)->get() == ((EtchInt32*) integer)->get());
  EXPECT_TRUE(ptr->validateValue(integer2, result) == ETCH_OK);
  EXPECT_TRUE(((EtchInt32*) result)->get() == ((EtchInt32*) integer2)->get());
  EXPECT_TRUE(ptr->validateValue(integer3, result) == ETCH_OK);
  EXPECT_TRUE(((EtchInt32*) result)->get() == ((EtchInt32*) integer3)->get());
  EXPECT_TRUE(ptr->validateValue(byte2, result) == ETCH_OK);
  EXPECT_TRUE(((EtchInt32*) result)->get() == ((EtchByte*) byte2)->get());
  delete result;
  EtchObject* str = NULL;
  EtchObject* str2 = new EtchString("hello");
  //STRING TESTS
  EXPECT_TRUE(ptr->validateValue(str, result) == ETCH_ERROR);
  EXPECT_TRUE(ptr->validateValue(str2, result) == ETCH_OK);
  delete result;
  delete ptr;
  
  delete byte2;
  delete integer;
  delete integer2;
  delete integer3;
}

TEST(EtchComboValidatorTest, elementValidatorTest) {
  //create combo validator
  EtchComboValidator *ptr = NULL;
  capu::SmartPointer<EtchValidator> ptr1;
  capu::SmartPointer<EtchValidator> ptr2;
  EtchValidatorInt::Get(1, ptr1);
  EtchValidatorString::Get(1, ptr2);
  ptr = new EtchComboValidator(ptr1, ptr2);
  capu::SmartPointer<EtchValidator> element_validator;
  ptr->getElementValidator(element_validator);


  EtchObject* str = NULL;
  EtchObject* str2 = new EtchString("hello");

  EtchObject* integer = new EtchInt32(capu::NumericLimitMin<capu::int32_t>());
  EtchObject* integer2 = new EtchInt32(0);
  EtchObject* integer3 = new EtchInt32(capu::NumericLimitMax<capu::int32_t>());
  EtchObject* integer4 = new EtchInt32(897);

  EtchObject* longInteger = new EtchLong(capu::NumericLimitMin<capu::int32_t>());
  EtchObject* longInteger2 = new EtchLong(0);
  EtchObject* longInteger3 = new EtchLong(capu::NumericLimitMax<capu::int32_t>());
  EtchObject* longInteger4 = new EtchLong(897);
  //exceed limits of integer
  EtchObject* longInteger5 = new EtchLong((capu::int64_t)capu::NumericLimitMax<capu::int32_t>() + (capu::int64_t)2);

  EtchObject* shortInteger = new EtchShort(capu::NumericLimitMin<capu::int16_t>());
  EtchObject* shortInteger2 = new EtchShort(0);
  EtchObject* shortInteger3 = new EtchShort(capu::NumericLimitMax<capu::int16_t>());
  EtchObject* shortInteger4 = new EtchShort();

  EtchObject* byte1 = new EtchByte(capu::NumericLimitMax<capu::int8_t>());
  EtchObject* byte2 = new EtchByte(0);
  EtchObject* byte3 = new EtchByte(capu::NumericLimitMin<capu::int8_t>());
  EtchObject* byte4 = new EtchByte(32);
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

  delete integer;
  delete integer2;
  delete integer3;
  delete integer4;

  delete longInteger;
  delete longInteger2;
  delete longInteger3;
  delete longInteger4;
  delete longInteger5;

  delete shortInteger;
  delete shortInteger2;
  delete shortInteger3;
  delete shortInteger4;

  delete str2;
  delete byte1;
  delete byte2;
  delete byte3;
  delete byte4;
}