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
#include <gmock/gmock.h>
#include "serialization/EtchValidatorStructValue.h"
#include "common/EtchInt32.h"
#include "common/EtchBool.h"
#include "common/EtchShort.h"
#include "common/EtchByte.h"
#include "common/EtchLong.h"
#include "common/EtchString.h"
#include "serialization/EtchTypeCodes.h"
#include "serialization/EtchStructValue.h"
#include "capu/os/NumericLimits.h"
#include "support/EtchRuntime.h"

class MockValueFactory8 : public virtual EtchValueFactory {
public:

  ~MockValueFactory8() {

  }
  MOCK_METHOD2(getType, status_t(capu::uint32_t id, EtchType*& result));

  MOCK_METHOD2(getType, status_t(EtchString& name, EtchType*& result));

  MOCK_METHOD1(addType, status_t(EtchType* type));

  MOCK_METHOD0(lockDynamicTypes, status_t());

  MOCK_METHOD0(unlockDynamicTypes, status_t());

  MOCK_METHOD2(getMessageId, status_t(EtchMessage* msg, capu::int64_t &result));

  MOCK_METHOD2(setMessageId, status_t(EtchMessage* msg, capu::int64_t msgid));

  MOCK_METHOD0(get_mf__messageId, EtchField());

  MOCK_METHOD2(getInReplyToMessageId, status_t(EtchMessage* msg, capu::int64_t &result));

  MOCK_METHOD2(setInReplyToMessageId, status_t(EtchMessage* msg, capu::int64_t msgid));

  MOCK_METHOD0(get_mf__inReplyTo, EtchField());

  MOCK_METHOD2(importCustomValue, status_t(EtchStructValue* structValue, capu::SmartPointer<EtchObject> &result));

  MOCK_METHOD2(exportCustomValue, status_t(capu::SmartPointer<EtchObject> value, EtchStructValue*& result));

  MOCK_METHOD0(get_mt__exception, EtchType*());

  MOCK_METHOD2(getCustomStructType, status_t(const EtchObjectType *c, EtchType *& type));

  MOCK_METHOD4(getNativeArrayForComponentType, status_t(const EtchObjectType *objectType,  capu::SmartPointer<EtchNativeArrayBase> &nativeArray, capu::int32_t length, capu::int32_t dim));

  EtchLevel getLevel() {
    return LEVEL_FULL;
  }

  MOCK_METHOD1(setLevel, EtchLevel(EtchLevel level));

};

class EtchValidatorStructValueTest
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

TEST_F(EtchValidatorStructValueTest, createTest) {
  EtchString str("typename");
  EtchType *type = new EtchType(str);
  capu::SmartPointer<EtchValidatorStructValue> ptr = NULL;

  capu::SmartPointer<EtchValidator> val = NULL;
  EXPECT_TRUE(EtchValidatorStructValue::Get(mRuntime, type, 0, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorStructValue>(val);

  EXPECT_TRUE(ptr->getExpectedType()->equals(EtchStructValue::TYPE()));
  EXPECT_TRUE(ptr->getNDims() == 0);

  EtchObjectType type1(EOTID_STRUCT_VALUE, NULL);
  EtchObjectType type2(EOTID_NATIVE_ARRAY, &type1);

  EXPECT_TRUE(EtchValidatorStructValue::Get(mRuntime, type, 2, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorStructValue>(val);
  EXPECT_TRUE(ptr->getExpectedType()->equals(&type2));
  EXPECT_TRUE(ptr->getNDims() == 2);

  EXPECT_TRUE(EtchValidatorStructValue::Get(mRuntime, type, 12, val) == ETCH_EINVAL);
  delete type;
}

TEST_F(EtchValidatorStructValueTest, elementValidator) {
  EtchString str("typename");
  EtchType *type = new EtchType(str);
  capu::SmartPointer<EtchValidator> elementValidator;
  capu::SmartPointer<EtchValidator> val;
  MockValueFactory8 *fac = new MockValueFactory8();

  EtchObjectType type1(EOTID_STRUCT_VALUE, NULL);
  EtchObjectType type2(EOTID_NATIVE_ARRAY, &type1);
  EXPECT_TRUE(EtchValidatorStructValue::Get(mRuntime, type, 1, val) == ETCH_OK);
  EXPECT_TRUE(val.get() != NULL);
  EXPECT_TRUE((capu::smartpointer_cast<EtchTypeValidator>(val))->getExpectedType()->equals(&type2));
  EXPECT_TRUE((capu::smartpointer_cast<EtchTypeValidator>(val))->getNDims() == 1);

  val->getElementValidator(elementValidator);


  capu::SmartPointer<EtchObject> integer = new EtchInt32(capu::NumericLimits::Min<capu::int16_t>());
  capu::SmartPointer<EtchObject> integer2 = new EtchInt32(0);
  capu::SmartPointer<EtchObject> integer3 = new EtchInt32(capu::NumericLimits::Max<capu::int16_t>());
  capu::SmartPointer<EtchObject> integer4 = new EtchInt32(897);

  //exceed limits of integer
  capu::SmartPointer<EtchObject> integer5 = new EtchInt32(capu::NumericLimits::Max<capu::int16_t>() + 2);

  capu::SmartPointer<EtchObject> longInteger = new EtchLong(capu::NumericLimits::Min<capu::int16_t>());
  capu::SmartPointer<EtchObject> longInteger2 = new EtchLong(0);
  capu::SmartPointer<EtchObject> longInteger3 = new EtchLong(capu::NumericLimits::Max<capu::int16_t>());
  capu::SmartPointer<EtchObject> longInteger4 = new EtchLong(897);
  //exceed limits of integer
  capu::SmartPointer<EtchObject> longInteger5 = new EtchLong((capu::int64_t)capu::NumericLimits::Max<capu::int16_t>() + (capu::int64_t)2);

  capu::SmartPointer<EtchObject> shortInteger = new EtchShort(capu::NumericLimits::Min<capu::int16_t>());
  capu::SmartPointer<EtchObject> shortInteger2 = new EtchShort(0);
  capu::SmartPointer<EtchObject> shortInteger3 = new EtchShort(capu::NumericLimits::Max<capu::int16_t>());
  capu::SmartPointer<EtchObject> shortInteger4 = new EtchShort();
  //incompatible type
  capu::SmartPointer<EtchObject> str2 = new EtchString();

  capu::SmartPointer<EtchObject> byte1 = new EtchByte(capu::NumericLimits::Max<capu::int8_t>());
  capu::SmartPointer<EtchObject> byte2 = new EtchByte(0);
  capu::SmartPointer<EtchObject> byte3 = new EtchByte(capu::NumericLimits::Min<capu::int8_t>());
  capu::SmartPointer<EtchObject> byte4 = new EtchByte(32);
  capu::SmartPointer<EtchObject> structValueval = new EtchStructValue(type, fac);

  EXPECT_FALSE(elementValidator->validate(str2));
  EXPECT_FALSE(elementValidator->validate(longInteger5));
  EXPECT_FALSE(elementValidator->validate(integer5));
  EXPECT_FALSE(elementValidator->validate(integer));
  EXPECT_FALSE(elementValidator->validate(integer2));
  EXPECT_FALSE(elementValidator->validate(integer3));
  EXPECT_FALSE(elementValidator->validate(longInteger));
  EXPECT_FALSE(elementValidator->validate(longInteger2));
  EXPECT_FALSE(elementValidator->validate(longInteger3));
  EXPECT_FALSE(elementValidator->validate(shortInteger));
  EXPECT_FALSE(elementValidator->validate(shortInteger2));
  EXPECT_FALSE(elementValidator->validate(shortInteger3));
  EXPECT_FALSE(elementValidator->validate(byte1));
  EXPECT_FALSE(elementValidator->validate(byte2));
  EXPECT_FALSE(elementValidator->validate(byte3));
  EXPECT_FALSE(elementValidator->validate(byte4));
  EXPECT_FALSE(elementValidator->validate(integer4));
  EXPECT_FALSE(elementValidator->validate(longInteger4));
  EXPECT_FALSE(elementValidator->validate(shortInteger4));
  EXPECT_FALSE(elementValidator->validate(NULL));
  EXPECT_TRUE(elementValidator->validate(structValueval));
  delete fac;
  delete type;
}

TEST_F(EtchValidatorStructValueTest, validateTest) {

  EtchString str("typename");
  EtchType *type = new EtchType(str);
  capu::SmartPointer<EtchValidator> ptr = NULL;
  MockValueFactory8 *fac = new MockValueFactory8();

  EXPECT_TRUE(EtchValidatorStructValue::Get(mRuntime, type, 0, ptr) == ETCH_OK);

  capu::SmartPointer<EtchObject> byte = NULL;

  capu::SmartPointer<EtchObject> integer = new EtchInt32(capu::NumericLimits::Min<capu::int16_t>());
  capu::SmartPointer<EtchObject> integer2 = new EtchInt32(0);
  capu::SmartPointer<EtchObject> integer3 = new EtchInt32(capu::NumericLimits::Max<capu::int16_t>());
  capu::SmartPointer<EtchObject> integer4 = new EtchInt32(897);

  //exceed limits of integer
  capu::SmartPointer<EtchObject> integer5 = new EtchInt32(capu::NumericLimits::Max<capu::int16_t>() + 2);

  capu::SmartPointer<EtchObject> longInteger = new EtchLong(capu::NumericLimits::Min<capu::int16_t>());
  capu::SmartPointer<EtchObject> longInteger2 = new EtchLong(0);
  capu::SmartPointer<EtchObject> longInteger3 = new EtchLong(capu::NumericLimits::Max<capu::int16_t>());
  capu::SmartPointer<EtchObject> longInteger4 = new EtchLong(897);
  //exceed limits of integer
  capu::SmartPointer<EtchObject> longInteger5 = new EtchLong((capu::int64_t)capu::NumericLimits::Max<capu::int16_t>() + (capu::int64_t)2);

  capu::SmartPointer<EtchObject> shortInteger = new EtchShort(capu::NumericLimits::Min<capu::int16_t>());
  capu::SmartPointer<EtchObject> shortInteger2 = new EtchShort(0);
  capu::SmartPointer<EtchObject> shortInteger3 = new EtchShort(capu::NumericLimits::Max<capu::int16_t>());
  capu::SmartPointer<EtchObject> shortInteger4 = new EtchShort();

  //incompatible type
  capu::SmartPointer<EtchObject> str2 = new EtchString();

  capu::SmartPointer<EtchObject> byte1 = new EtchByte(capu::NumericLimits::Max<capu::int8_t>());
  capu::SmartPointer<EtchObject> byte2 = new EtchByte(0);
  capu::SmartPointer<EtchObject> byte3 = new EtchByte(capu::NumericLimits::Min<capu::int8_t>());
  capu::SmartPointer<EtchObject> byte4 = new EtchByte(32);

  EtchStructValue * value = new EtchStructValue(type, fac);


  EXPECT_FALSE(ptr->validate(byte));
  EXPECT_FALSE(ptr->validate(str2));
  EXPECT_FALSE(ptr->validate(longInteger5));
  EXPECT_FALSE(ptr->validate(integer5));
  EXPECT_FALSE(ptr->validate(integer));
  EXPECT_FALSE(ptr->validate(integer2));
  EXPECT_FALSE(ptr->validate(integer3));
  EXPECT_FALSE(ptr->validate(longInteger));
  EXPECT_FALSE(ptr->validate(longInteger2));
  EXPECT_FALSE(ptr->validate(longInteger3));
  EXPECT_FALSE(ptr->validate(shortInteger));
  EXPECT_FALSE(ptr->validate(shortInteger2));
  EXPECT_FALSE(ptr->validate(shortInteger3));
  EXPECT_FALSE(ptr->validate(byte1));
  EXPECT_FALSE(ptr->validate(byte2));
  EXPECT_FALSE(ptr->validate(byte3));
  EXPECT_FALSE(ptr->validate(byte4));
  EXPECT_FALSE(ptr->validate(integer4));
  EXPECT_FALSE(ptr->validate(longInteger4));
  EXPECT_FALSE(ptr->validate(shortInteger4));
  EXPECT_FALSE(ptr->validate(NULL));
  EXPECT_TRUE(ptr->validate(value));
  delete fac;
  delete type;
}

TEST_F(EtchValidatorStructValueTest, validateValueTest) {
  EtchString str("typename");
  EtchType *type = new EtchType(str);
  capu::SmartPointer<EtchValidator> ptr = NULL;
  MockValueFactory8 *fac = new MockValueFactory8();

  EXPECT_TRUE(EtchValidatorStructValue::Get(mRuntime, type, 0, ptr) == ETCH_OK);
  EXPECT_TRUE(((EtchTypeValidator*)ptr.get())->getExpectedType()->equals(EtchStructValue::TYPE()));
  EXPECT_TRUE(((EtchTypeValidator*)ptr.get())->getNDims() == 0);

  capu::SmartPointer<EtchObject> byte = NULL;
  capu::SmartPointer<EtchObject> result;
  capu::SmartPointer<EtchObject> integer = new EtchInt32(-128);
  capu::SmartPointer<EtchObject> byte2 = new EtchByte(3);
  capu::SmartPointer<EtchObject> structValue = new EtchStructValue(type, fac);

  EXPECT_TRUE(ptr->validateValue(byte, result) == ETCH_ERROR);

  EXPECT_TRUE(ptr->validateValue(integer, result) == ETCH_ERROR);

  EXPECT_TRUE(ptr->validateValue(byte2, result) == ETCH_ERROR);

  EXPECT_TRUE(ptr->validateValue(structValue, result) == ETCH_OK);

  delete type;
  delete fac;
}
