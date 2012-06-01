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

TEST(EtchValidatorDoubleTest, createTest) {
  capu::SmartPointer<EtchValidatorDouble> ptr = NULL;

  capu::SmartPointer<EtchValidator> val;
  EXPECT_TRUE(EtchValidatorDouble::Get(0, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorDouble>(val);

  EXPECT_TRUE(ptr->getExpectedType()->equals(&EtchDouble::TYPE));
  EXPECT_TRUE(ptr->getNDims() == 0);

  EtchObjectType type1(EOTID_DOUBLE, NULL);
  EtchObjectType type2(EOTID_NATIVE_ARRAY, &type1);
  EtchObjectType type3(EOTID_NATIVE_ARRAY, &type2);

  EXPECT_TRUE(EtchValidatorDouble::Get(2, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorDouble>(val);
  EXPECT_TRUE(ptr->getExpectedType()->equals(&type3));
  EXPECT_TRUE(ptr->getNDims() == 2);
}

TEST(EtchValidatorDoubleTest, validateTest) {
  EtchObject* byte = NULL;

  EtchObject* doubleTmp = new EtchDouble(capu::NumericLimitMax<capu::float_t>());
  EtchObject* doubleTmp1 = new EtchDouble(0);
  EtchObject* doubleTmp2 = new EtchDouble(capu::NumericLimitMin<capu::float_t>());
  EtchObject* doubleTmp3 = new EtchDouble(897.12);
  //exceed limits of Float
  EtchObject* doubleTmp4_true = new EtchDouble((capu::double_t)capu::NumericLimitMax<capu::float_t>() + (capu::double_t)2.12);
  //incompatible type
  EtchObject* str = new EtchString();

  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorDouble::Get(0, ptr) == ETCH_OK);
  EXPECT_TRUE((capu::smartpointer_cast<EtchTypeValidator>(ptr))->getNDims() == 0);

  EXPECT_FALSE(ptr->validate(byte));
  EXPECT_FALSE(ptr->validate(str));

  EXPECT_TRUE(ptr->validate(doubleTmp));
  EXPECT_TRUE(ptr->validate(doubleTmp1));
  EXPECT_TRUE(ptr->validate(doubleTmp2));
  EXPECT_TRUE(ptr->validate(doubleTmp3));
  EXPECT_TRUE(ptr->validate(doubleTmp4_true));

  delete doubleTmp;
  delete doubleTmp1;
  delete doubleTmp2;
  delete doubleTmp3;
  delete doubleTmp4_true;

  delete str;
}

TEST(EtchValidatorDoubleTest, validateValueTest) {
  EtchObject* byte = NULL;
  EtchObject* result;
  EtchObject* doubleTmp = new EtchDouble(-128.12);
  EtchObject* doubleTmp2 = new EtchDouble(5.13);
  EtchObject* doubleTmp3 = new EtchDouble(127.54);
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorDouble::Get(0, ptr) == ETCH_OK);
  EXPECT_TRUE(ptr->validateValue(byte, result) == ETCH_ERROR);
  EXPECT_TRUE(ptr->validateValue(doubleTmp, result) == ETCH_OK);
  EXPECT_TRUE(((EtchDouble*) result)->get() == ((EtchDouble*) doubleTmp)->get());
  EXPECT_TRUE(ptr->validateValue(doubleTmp2, result) == ETCH_OK);
  EXPECT_TRUE(((EtchDouble*) result)->get() == ((EtchDouble*) doubleTmp2)->get());
  EXPECT_TRUE(ptr->validateValue(doubleTmp3, result) == ETCH_OK);
  EXPECT_TRUE(((EtchDouble*) result)->get() == ((EtchDouble*) doubleTmp3)->get());

  delete doubleTmp;
  delete doubleTmp2;
  delete doubleTmp3;
}

TEST(EtchValidatorDoubleTest, elementValidatorTest) {
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorDouble::Get(1, ptr) == ETCH_OK);
  capu::SmartPointer<EtchValidator> elementValidator;
  ptr->getElementValidator(elementValidator);

  EtchObject* doubleTmp = new EtchDouble(capu::NumericLimitMax<capu::float_t>());
  EtchObject* doubleTmp1 = new EtchDouble(0);
  EtchObject* doubleTmp2 = new EtchDouble(capu::NumericLimitMin<capu::float_t>());
  EtchObject* doubleTmp3 = new EtchDouble(897.12);
  //exceed limits of Float
  EtchObject* doubleTmp4_true = new EtchDouble((capu::double_t)capu::NumericLimitMax<capu::float_t>() + (capu::double_t)2.12);

  //incompatible type
  EtchObject* str = new EtchString();

  EXPECT_FALSE(elementValidator->validate(str));
  EXPECT_TRUE(elementValidator->validate(doubleTmp4_true));
  EXPECT_TRUE(elementValidator->validate(doubleTmp));
  EXPECT_TRUE(elementValidator->validate(doubleTmp1));
  EXPECT_TRUE(elementValidator->validate(doubleTmp2));
  EXPECT_TRUE(elementValidator->validate(doubleTmp3));

  delete doubleTmp;
  delete doubleTmp1;
  delete doubleTmp2;
  delete doubleTmp3;
  delete doubleTmp4_true;

  delete str;
}


