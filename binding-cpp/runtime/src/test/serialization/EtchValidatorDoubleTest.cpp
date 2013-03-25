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
#include "serialization/EtchValidatorDouble.h"
#include "capu/util/SmartPointer.h"
#include "common/EtchString.h"
#include "support/EtchRuntime.h"

class EtchValidatorDoubleTest
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

TEST_F(EtchValidatorDoubleTest, createTest) {
  capu::SmartPointer<EtchValidatorDouble> ptr = NULL;

  capu::SmartPointer<EtchValidator> val;
  EXPECT_TRUE(EtchValidatorDouble::Get(mRuntime, 0, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorDouble>(val);

  EXPECT_TRUE(ptr->getExpectedType()->equals(EtchDouble::TYPE()));
  EXPECT_TRUE(ptr->getNDims() == 0);

  EtchObjectType type1(EOTID_DOUBLE, NULL);
  EtchObjectType type2(EOTID_NATIVE_ARRAY, &type1);

  EXPECT_TRUE(EtchValidatorDouble::Get(mRuntime, 2, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorDouble>(val);
  EXPECT_TRUE(ptr->getExpectedType()->equals(&type2));
  EXPECT_TRUE(ptr->getNDims() == 2);
}

TEST_F(EtchValidatorDoubleTest, validateTest) {
  capu::SmartPointer<EtchObject> byte = NULL;

  capu::SmartPointer<EtchObject> doubleTmp = new EtchDouble(capu::NumericLimits::Max<capu::float_t>());
  capu::SmartPointer<EtchObject> doubleTmp1 = new EtchDouble(0);
  capu::SmartPointer<EtchObject> doubleTmp2 = new EtchDouble(capu::NumericLimits::Min<capu::float_t>());
  capu::SmartPointer<EtchObject> doubleTmp3 = new EtchDouble(897.12);
  //exceed limits of Float
  capu::SmartPointer<EtchObject> doubleTmp4_true = new EtchDouble((capu::double_t)capu::NumericLimits::Max<capu::float_t>() + (capu::double_t)2.12);
  //incompatible type
  capu::SmartPointer<EtchObject> str = new EtchString();

  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorDouble::Get(mRuntime, 0, ptr) == ETCH_OK);
  EXPECT_TRUE((capu::smartpointer_cast<EtchTypeValidator>(ptr))->getNDims() == 0);

  EXPECT_FALSE(ptr->validate(byte));
  EXPECT_FALSE(ptr->validate(str));

  EXPECT_TRUE(ptr->validate(doubleTmp));
  EXPECT_TRUE(ptr->validate(doubleTmp1));
  EXPECT_TRUE(ptr->validate(doubleTmp2));
  EXPECT_TRUE(ptr->validate(doubleTmp3));
  EXPECT_TRUE(ptr->validate(doubleTmp4_true));
}

TEST_F(EtchValidatorDoubleTest, validateValueTest) {
  capu::SmartPointer<EtchObject> byte = NULL;
  capu::SmartPointer<EtchObject> result;
  capu::SmartPointer<EtchObject> doubleTmp = new EtchDouble(-128.12);
  capu::SmartPointer<EtchObject> doubleTmp2 = new EtchDouble(5.13);
  capu::SmartPointer<EtchObject> doubleTmp3 = new EtchDouble(127.54);
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorDouble::Get(mRuntime, 0, ptr) == ETCH_OK);
  EXPECT_TRUE(ptr->validateValue(byte, result) == ETCH_ERROR);
  EXPECT_TRUE(ptr->validateValue(doubleTmp, result) == ETCH_OK);
  EXPECT_TRUE(((EtchDouble*) result.get())->get() == ((EtchDouble*) doubleTmp.get())->get());
  EXPECT_TRUE(ptr->validateValue(doubleTmp2, result) == ETCH_OK);
  EXPECT_TRUE(((EtchDouble*) result.get())->get() == ((EtchDouble*) doubleTmp2.get())->get());
  EXPECT_TRUE(ptr->validateValue(doubleTmp3, result) == ETCH_OK);
  EXPECT_TRUE(((EtchDouble*) result.get())->get() == ((EtchDouble*) doubleTmp3.get())->get());
}

TEST_F(EtchValidatorDoubleTest, elementValidatorTest) {
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorDouble::Get(mRuntime, 1, ptr) == ETCH_OK);
  capu::SmartPointer<EtchValidator> elementValidator;
  ptr->getElementValidator(elementValidator);

  capu::SmartPointer<EtchObject> doubleTmp = new EtchDouble(capu::NumericLimits::Max<capu::float_t>());
  capu::SmartPointer<EtchObject> doubleTmp1 = new EtchDouble(0);
  capu::SmartPointer<EtchObject> doubleTmp2 = new EtchDouble(capu::NumericLimits::Min<capu::float_t>());
  capu::SmartPointer<EtchObject> doubleTmp3 = new EtchDouble(897.12);
  //exceed limits of Float
  capu::SmartPointer<EtchObject> doubleTmp4_true = new EtchDouble((capu::double_t)capu::NumericLimits::Max<capu::float_t>() + (capu::double_t)2.12);

  //incompatible type
  capu::SmartPointer<EtchObject> str = new EtchString();

  EXPECT_FALSE(elementValidator->validate(str));
  EXPECT_TRUE(elementValidator->validate(doubleTmp4_true));
  EXPECT_TRUE(elementValidator->validate(doubleTmp));
  EXPECT_TRUE(elementValidator->validate(doubleTmp1));
  EXPECT_TRUE(elementValidator->validate(doubleTmp2));
  EXPECT_TRUE(elementValidator->validate(doubleTmp3));
}


