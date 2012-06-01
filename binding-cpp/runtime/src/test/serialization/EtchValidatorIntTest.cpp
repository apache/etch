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
#include "serialization/EtchValidatorInt.h"
#include "capu/util/SmartPointer.h"
#include "common/EtchString.h"

TEST(EtchValidatorIntTest, createTest) {
  capu::SmartPointer<EtchValidatorInt> ptr = NULL;
  capu::SmartPointer<EtchValidator> val;
  EXPECT_TRUE(EtchValidatorInt::Get(0, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorInt>(val);
  EXPECT_TRUE(ptr->getExpectedType()->equals(EtchInt32::TYPE()));
  EXPECT_TRUE(ptr->getNDims() == 0);

  EtchObjectType type1(EOTID_INT32, NULL);
  EtchObjectType type2(EOTID_NATIVE_ARRAY, &type1);
  EtchObjectType type3(EOTID_NATIVE_ARRAY, &type2);
  EXPECT_TRUE(EtchValidatorInt::Get(2, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorInt>(val);
  EXPECT_TRUE(ptr->getExpectedType()->equals(&type3));
  EXPECT_TRUE(ptr->getNDims() == 2);

}

TEST(EtchValidatorIntTest, validateTest) {
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
  EtchObject* str = new EtchString();

  EtchObject* byte1 = new EtchByte(capu::NumericLimitMax<capu::int8_t>());
  EtchObject* byte2 = new EtchByte(0);
  EtchObject* byte3 = new EtchByte(capu::NumericLimitMin<capu::int8_t>());
  EtchObject* byte4 = new EtchByte(32);

  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorInt::Get(0, ptr) == ETCH_OK);
  EXPECT_TRUE((capu::smartpointer_cast<EtchTypeValidator>(ptr))->getNDims() == 0);
  EXPECT_FALSE(ptr->validate(byte));
  EXPECT_FALSE(ptr->validate(str));
  EXPECT_FALSE(ptr->validate(longInteger5));
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

  delete str;
  delete byte1;
  delete byte2;
  delete byte3;
  delete byte4;

}

TEST(EtchValidatorIntTest, validateValueTest) {
  EtchObject* byte = NULL;
  EtchObject* result;
  EtchObject* integer = new EtchInt32(-128);
  EtchObject* integer2 = new EtchInt32(5);
  EtchObject* integer3 = new EtchInt32(127);
  EtchObject* byte2 = new EtchByte(3);
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorInt::Get(0, ptr) == ETCH_OK);
  EXPECT_TRUE(ptr->validateValue(byte, result) == ETCH_ERROR);
  EXPECT_TRUE(ptr->validateValue(integer, result) == ETCH_OK);
  EXPECT_TRUE(((EtchInt32*) result)->get() == ((EtchInt32*) integer)->get());
//  delete result;
  EXPECT_TRUE(ptr->validateValue(integer2, result) == ETCH_OK);
  EXPECT_TRUE(((EtchInt32*) result)->get() == ((EtchInt32*) integer2)->get());
//  delete result;
  EXPECT_TRUE(ptr->validateValue(integer3, result) == ETCH_OK);
  EXPECT_TRUE(((EtchInt32*) result)->get() == ((EtchInt32*) integer3)->get());
//  delete result;
  EXPECT_TRUE(ptr->validateValue(byte2, result) == ETCH_OK);
  EXPECT_TRUE(((EtchInt32*) result)->get() == ((EtchByte*) byte2)->get());
  delete result;
  delete integer;
  delete integer2;
  delete integer3;
  delete byte2;

}

TEST(EtchValidatorIntTest, elementValidatorTest) {
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorInt::Get(1, ptr) == ETCH_OK);
  capu::SmartPointer<EtchValidator> elementValidator;
  ptr->getElementValidator(elementValidator);

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
  EtchObject* str = new EtchString();

  EtchObject* byte1 = new EtchByte(capu::NumericLimitMax<capu::int8_t>());
  EtchObject* byte2 = new EtchByte(0);
  EtchObject* byte3 = new EtchByte(capu::NumericLimitMin<capu::int8_t>());
  EtchObject* byte4 = new EtchByte(32);

  EXPECT_FALSE(elementValidator->validate(str));
  EXPECT_FALSE(elementValidator->validate(longInteger5));
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

  delete str;
  delete byte1;
  delete byte2;
  delete byte3;
  delete byte4;
}


