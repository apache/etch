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
#include "serialization/EtchValidatorCustom.h"
#include "capu/util/SmartPointer.h"
#include "common/EtchString.h"
#include "support/EtchRuntime.h"

class MockGeneratedClass : public EtchObject {
public:

  MockGeneratedClass() {
    addObjectType(MockGeneratedClass::TYPE());
  }

  virtual ~MockGeneratedClass() {
  }

  /**
   * EtchObjectType for EtchString.
   */
  static const EtchObjectType* TYPE();
};

const EtchObjectType* MockGeneratedClass::TYPE() {
  const static EtchObjectType TYPE(99, NULL);
  return &TYPE;
}

class EtchValidatorCustomTest
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

TEST_F(EtchValidatorCustomTest, createTest) {
  EtchValidatorCustom *ptr = NULL;

  status_t status;
  capu::SmartPointer<EtchValidator> val;
  status = EtchValidatorCustom::Get(mRuntime, 0, MockGeneratedClass::TYPE(), true, val);
  EXPECT_EQ(ETCH_OK, status);

  ptr = (EtchValidatorCustom*)val.get();
  EXPECT_TRUE(ptr != NULL);
  EXPECT_TRUE(ptr->getExpectedType()->equals(MockGeneratedClass::TYPE()));
  EXPECT_TRUE(ptr->getNDims() == 0);

  EtchObjectType type1(99, NULL);
  EtchObjectType type2(EOTID_NATIVE_ARRAY, &type1);
  status = EtchValidatorCustom::Get(mRuntime, 2, MockGeneratedClass::TYPE(), true, val);
  EXPECT_EQ(ETCH_OK, status);

  ptr = (EtchValidatorCustom*) val.get();
  EXPECT_TRUE(ptr != NULL);
  EXPECT_TRUE(ptr->getExpectedType()->equals(&type2));
  EXPECT_TRUE(ptr->getNDims() == 2);
}

TEST_F(EtchValidatorCustomTest, validateTest) {
  status_t status;
  capu::SmartPointer<EtchObject> byte = NULL;

  capu::SmartPointer<EtchObject> integer = new EtchInt32(-128);
  capu::SmartPointer<EtchObject> integer2 = new EtchInt32(5);
  capu::SmartPointer<EtchObject> integer3 = new EtchInt32(127);
  //exceed limit of byte
  capu::SmartPointer<EtchObject> integer4 = new EtchInt32(897);
  capu::SmartPointer<EtchObject> longInteger = new EtchLong(-128);
  capu::SmartPointer<EtchObject> longInteger2 = new EtchLong(5);
  capu::SmartPointer<EtchObject> longInteger3 = new EtchLong(127);
  capu::SmartPointer<EtchObject> longInteger4 = new EtchLong(897);
  //exceed limit of byte
  capu::SmartPointer<EtchObject> shortInteger = new EtchShort(-128);
  capu::SmartPointer<EtchObject> shortInteger2 = new EtchShort(5);
  capu::SmartPointer<EtchObject> shortInteger3 = new EtchShort(127);
  //Exceed limit of byte
  capu::SmartPointer<EtchObject> shortInteger4 = new EtchShort(897);
  capu::SmartPointer<EtchObject> str = new EtchString();
  capu::SmartPointer<EtchObject> byte2 = new EtchByte(3);

  capu::SmartPointer<EtchObject> generatedClass = new MockGeneratedClass();

  capu::SmartPointer<EtchValidator> val;
  status = EtchValidatorCustom::Get(mRuntime, 0, MockGeneratedClass::TYPE(), true, val);
  EXPECT_EQ(ETCH_OK, status);

  EXPECT_FALSE(val->validate(byte));
  EXPECT_FALSE(val->validate(integer));
  EXPECT_FALSE(val->validate(integer2));
  EXPECT_FALSE(val->validate(integer3));
  EXPECT_FALSE(val->validate(longInteger));
  EXPECT_FALSE(val->validate(longInteger2));
  EXPECT_FALSE(val->validate(longInteger3));
  EXPECT_FALSE(val->validate(shortInteger));
  EXPECT_FALSE(val->validate(shortInteger2));
  EXPECT_FALSE(val->validate(shortInteger3));
  EXPECT_FALSE(val->validate(byte2));
  EXPECT_FALSE(val->validate(integer4));
  EXPECT_FALSE(val->validate(longInteger4));
  EXPECT_FALSE(val->validate(shortInteger4));
  EXPECT_FALSE(val->validate(str));
  EXPECT_TRUE(val->validate(generatedClass));
}

TEST_F(EtchValidatorCustomTest, validateValueTest) {
  status_t status;
  capu::SmartPointer<EtchObject> byte = NULL;
  capu::SmartPointer<EtchObject> result;
  capu::SmartPointer<EtchObject> integer = new EtchInt32(-128);
  capu::SmartPointer<EtchObject> integer2 = new EtchInt32(5);
  capu::SmartPointer<EtchObject> integer3 = new EtchInt32(127);
  capu::SmartPointer<EtchObject> byte2 = new EtchByte(3);

  capu::SmartPointer<EtchValidator> ptr = NULL;
  capu::SmartPointer<EtchObject> generatedClass = new MockGeneratedClass();

  capu::SmartPointer<EtchValidator> val;
  status = EtchValidatorCustom::Get(mRuntime, 0, MockGeneratedClass::TYPE(), true, val);
  EXPECT_EQ(ETCH_OK, status);

  EXPECT_TRUE(val->validateValue(byte, result) == ETCH_ERROR);
  EXPECT_TRUE(val->validateValue(integer, result) == ETCH_ERROR);
  EXPECT_TRUE(val->validateValue(integer2, result) == ETCH_ERROR);
  EXPECT_TRUE(val->validateValue(integer3, result) == ETCH_ERROR);
  EXPECT_TRUE(val->validateValue(byte2, result) == ETCH_ERROR);
  EXPECT_TRUE(val->validateValue(generatedClass, result) == ETCH_OK);
}



TEST_F(EtchValidatorCustomTest, elementValidatorTest) {
  status_t status;
  capu::SmartPointer<EtchValidator> ptr = NULL;

  status = EtchValidatorCustom::Get(mRuntime, 1, MockGeneratedClass::TYPE(), true, ptr);
  EXPECT_EQ(ETCH_OK, status);

  capu::SmartPointer<EtchValidator> element_validator;
  ptr->getElementValidator(element_validator);

  capu::SmartPointer<EtchObject> integer = new EtchInt32(-128);
  capu::SmartPointer<EtchObject> integer2 = new EtchInt32(5);
  capu::SmartPointer<EtchObject> integer3 = new EtchInt32(127);
  //exceed limit of byte
  capu::SmartPointer<EtchObject> integer4 = new EtchInt32(897);
  capu::SmartPointer<EtchObject> longInteger = new EtchLong(-128);
  capu::SmartPointer<EtchObject> longInteger2 = new EtchLong(5);
  capu::SmartPointer<EtchObject> longInteger3 = new EtchLong(127);
  //exceed limit of byte
  capu::SmartPointer<EtchObject> longInteger4 = new EtchLong(897);
  capu::SmartPointer<EtchObject> shortInteger = new EtchShort(-128);
  capu::SmartPointer<EtchObject> shortInteger2 = new EtchShort(5);
  capu::SmartPointer<EtchObject> shortInteger3 = new EtchShort(127);
  //Exceed limit of byte
  capu::SmartPointer<EtchObject> shortInteger4 = new EtchShort(897);
  capu::SmartPointer<EtchObject> str = new EtchString();
  capu::SmartPointer<EtchObject> byte2 = new EtchByte(3);
  capu::SmartPointer<EtchObject> generatedClass = new MockGeneratedClass();

  EXPECT_FALSE(element_validator->validate(integer));
  EXPECT_FALSE(element_validator->validate(integer2));
  EXPECT_FALSE(element_validator->validate(integer3));
  EXPECT_FALSE(element_validator->validate(longInteger));
  EXPECT_FALSE(element_validator->validate(longInteger2));
  EXPECT_FALSE(element_validator->validate(longInteger3));
  EXPECT_FALSE(element_validator->validate(shortInteger));
  EXPECT_FALSE(element_validator->validate(shortInteger2));
  EXPECT_FALSE(element_validator->validate(shortInteger3));
  EXPECT_FALSE(element_validator->validate(byte2));
  EXPECT_FALSE(element_validator->validate(integer4));
  EXPECT_FALSE(element_validator->validate(longInteger4));
  EXPECT_FALSE(element_validator->validate(shortInteger4));
  EXPECT_FALSE(element_validator->validate(str));
  EXPECT_TRUE(element_validator->validate(generatedClass));

}
