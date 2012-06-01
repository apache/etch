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
#include "serialization/EtchValidatorBoolean.h"
#include "capu/util/SmartPointer.h"
#include "common/EtchInt32.h"
#include "common/EtchNativeArray.h"

TEST(EtchValidatorBooleanTest, createTest) {
  capu::SmartPointer<EtchValidator> val2;
  EXPECT_TRUE(EtchValidatorBoolean::Get(0, val2) == ETCH_OK);

  capu::SmartPointer<EtchValidatorBoolean> val = capu::smartpointer_cast<EtchValidatorBoolean>(val2);
  EXPECT_TRUE(val.get() != NULL);
  EXPECT_TRUE(val->getExpectedType()->equals(EtchBool::TYPE()));
  EXPECT_TRUE(val->getNDims() == 0);

  EtchObjectType type1(EOTID_BOOL, NULL);
  EtchObjectType type2(EOTID_NATIVE_ARRAY, &type1);
  EtchObjectType type3(EOTID_NATIVE_ARRAY, &type2);

  EXPECT_TRUE(EtchValidatorBoolean::Get(2, val2) == ETCH_OK);
  val = capu::smartpointer_cast<EtchValidatorBoolean>(val2);
  EXPECT_TRUE(val.get() != NULL);
  EXPECT_TRUE(val->getExpectedType()->equals(&type3));
  EXPECT_TRUE(val->getNDims() == 2);

}

TEST(EtchValidatorBooleanTest, validateTest) {
  EtchObject* boolean = NULL;
  EtchObject* integer = new EtchInt32(4);
  EtchObject* boolean2 = new EtchBool(false);
  capu::SmartPointer<EtchValidator> val2;
  EXPECT_TRUE(EtchValidatorBoolean::Get(0, val2) == ETCH_OK);
  capu::SmartPointer<EtchValidatorBoolean> val = capu::smartpointer_cast<EtchValidatorBoolean>(val2);
  EXPECT_FALSE(val->validate(boolean));
  EXPECT_FALSE(val->validate(integer));
  EXPECT_TRUE(val->validate(boolean2));
  delete integer;
  delete boolean2;

}

TEST(EtchValidatorBooleanTest, validateValueTest) {
  EtchObject* boolean = NULL;
  EtchObject* result;
  EtchObject* integer = new EtchInt32(4);
  EtchObject* boolean2 = new EtchBool(false);
  capu::SmartPointer<EtchValidator> val2;
  EXPECT_TRUE(EtchValidatorBoolean::Get(0, val2) == ETCH_OK);
  capu::SmartPointer<EtchValidatorBoolean> val = capu::smartpointer_cast<EtchValidatorBoolean>(val2);
  EXPECT_TRUE(val->validateValue(boolean, result) == ETCH_ERROR);
  EXPECT_TRUE(val->validateValue(integer, result) == ETCH_ERROR);
  EXPECT_TRUE(val->validateValue(boolean2, result) == ETCH_OK);
  delete integer;
  delete boolean2;
}

TEST(EtchValidatorBooleanTest, elementValidatorTest) {
  capu::SmartPointer<EtchValidator> val2;
  EXPECT_TRUE(EtchValidatorBoolean::Get(1, val2) == ETCH_OK);
  capu::SmartPointer<EtchValidatorBoolean> val = capu::smartpointer_cast<EtchValidatorBoolean>(val2);

  capu::SmartPointer<EtchValidator> element_validator;
  val->getElementValidator(element_validator);

  EtchObject* boolean = NULL;
  EtchObject* integer = new EtchInt32(4);
  EtchObject* boolean2 = new EtchBool(false);

  EXPECT_FALSE(element_validator->validate(boolean));
  EXPECT_FALSE(element_validator->validate(integer));
  EXPECT_TRUE(element_validator->validate(boolean2));
  delete integer;
  delete boolean2;
}
