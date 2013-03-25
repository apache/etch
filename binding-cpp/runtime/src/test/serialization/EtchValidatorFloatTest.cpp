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
#include "capu/os/NumericLimits.h"
#include "capu/util/SmartPointer.h"
#include "serialization/EtchValidatorFloat.h"
#include "common/EtchString.h"
#include "support/EtchRuntime.h"

class EtchValidatorFloatTest
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

TEST_F(EtchValidatorFloatTest, createTest) {
  capu::SmartPointer<EtchValidatorFloat> ptr = NULL;

  capu::SmartPointer<EtchValidator> val;
  EXPECT_TRUE(EtchValidatorFloat::Get(mRuntime, 0, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorFloat>(val);

  EXPECT_TRUE(ptr->getExpectedType()->equals(EtchFloat::TYPE()));
  EXPECT_TRUE(ptr->getNDims() == 0);

  EtchObjectType type1(EOTID_FLOAT, NULL);
  EtchObjectType type2(EOTID_NATIVE_ARRAY, &type1);

  EXPECT_TRUE(EtchValidatorFloat::Get(mRuntime, 2, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorFloat>(val);
  EXPECT_TRUE(ptr->getExpectedType()->equals(&type2));
  EXPECT_TRUE(ptr->getNDims() == 2);
}

TEST_F(EtchValidatorFloatTest, validateTest) {
  capu::SmartPointer<EtchObject> byte = NULL;

  capu::SmartPointer<EtchObject> floatTmp = new EtchFloat(capu::NumericLimits::Min<capu::float_t>());
  capu::SmartPointer<EtchObject> floatTmp1 = new EtchFloat(0);
  capu::SmartPointer<EtchObject> floatTmp2 = new EtchFloat(capu::NumericLimits::Max<capu::float_t>());
  capu::SmartPointer<EtchObject> floatTmp3 = new EtchFloat(static_cast<capu::float_t>(897.12));

  //incompatible type
  capu::SmartPointer<EtchObject> str = new EtchString();

  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorFloat::Get(mRuntime, 0, ptr) == ETCH_OK);
  EXPECT_TRUE((capu::smartpointer_cast<EtchTypeValidator>(ptr))->getNDims() == 0);

  EXPECT_FALSE(ptr->validate(byte));
  EXPECT_FALSE(ptr->validate(str));

  EXPECT_TRUE(ptr->validate(floatTmp));
  EXPECT_TRUE(ptr->validate(floatTmp1));
  EXPECT_TRUE(ptr->validate(floatTmp2));
  EXPECT_TRUE(ptr->validate(floatTmp3));
}

TEST_F(EtchValidatorFloatTest, validateValueTest) {
  capu::SmartPointer<EtchObject> byte = NULL;
  capu::SmartPointer<EtchObject> result;
  capu::SmartPointer<EtchObject> floatTmp = new EtchFloat(static_cast<capu::float_t>(-128.12));
  capu::SmartPointer<EtchObject> floatTmp2 = new EtchFloat(static_cast<capu::float_t>(5.13));
  capu::SmartPointer<EtchObject> floatTmp3 = new EtchFloat(static_cast<capu::float_t>(127.54));
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorFloat::Get(mRuntime, 0, ptr) == ETCH_OK);
  EXPECT_TRUE(ptr->validateValue(byte, result) == ETCH_ERROR);
  EXPECT_TRUE(ptr->validateValue(floatTmp, result) == ETCH_OK);
  EXPECT_TRUE(((EtchFloat*) result.get())->get() == ((EtchFloat*) floatTmp.get())->get());
  EXPECT_TRUE(ptr->validateValue(floatTmp2, result) == ETCH_OK);
  EXPECT_TRUE(((EtchFloat*) result.get())->get() == ((EtchFloat*) floatTmp2.get())->get());
  EXPECT_TRUE(ptr->validateValue(floatTmp3, result) == ETCH_OK);
  EXPECT_TRUE(((EtchFloat*) result.get())->get() == ((EtchFloat*) floatTmp3.get())->get());
}

TEST_F(EtchValidatorFloatTest, elementValidatorTest) {
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorFloat::Get(mRuntime, 1, ptr) == ETCH_OK);
  capu::SmartPointer<EtchValidator> elementValidator;
  ptr->getElementValidator(elementValidator);

  capu::SmartPointer<EtchObject> floatTmp = new EtchFloat(capu::NumericLimits::Min<capu::float_t>());
  capu::SmartPointer<EtchObject> floatTmp1 = new EtchFloat(0);
  capu::SmartPointer<EtchObject> floatTmp2 = new EtchFloat(capu::NumericLimits::Max<capu::float_t>());
  capu::SmartPointer<EtchObject> floatTmp3 = new EtchFloat(static_cast<capu::float_t>(897.12));

  //incompatible type
  capu::SmartPointer<EtchObject> str = new EtchString();

  EXPECT_FALSE(elementValidator->validate(str));
  EXPECT_TRUE(elementValidator->validate(floatTmp));
  EXPECT_TRUE(elementValidator->validate(floatTmp1));
  EXPECT_TRUE(elementValidator->validate(floatTmp2));
  EXPECT_TRUE(elementValidator->validate(floatTmp3));
}


