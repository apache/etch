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
#include "serialization/EtchValidatorString.h"
#include "capu/util/SmartPointer.h"
#include "common/EtchInt32.h"

TEST(EtchValidatorStringTest, createTest) {
  capu::SmartPointer<EtchValidatorString> ptr = NULL;

  capu::SmartPointer<EtchValidator> val;
  EXPECT_TRUE(EtchValidatorString::Get(0, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorString>(val);

  EXPECT_TRUE(ptr->getExpectedType()->equals(&EtchString::TYPE));
  EXPECT_TRUE(ptr->getNDims() == 0);

  EtchObjectType type1(EOTID_STRING, NULL);
  EtchObjectType type2(EOTID_NATIVE_ARRAY, &type1);
  EtchObjectType type3(EOTID_NATIVE_ARRAY, &type2);
  EXPECT_TRUE(EtchValidatorString::Get(2, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorString>(val);
  EXPECT_TRUE(ptr->getExpectedType()->equals(&type3));
  EXPECT_TRUE(ptr->getNDims() == 2);

}

TEST(EtchValidatorStringTest, validateTest) {
  EtchObject* str = NULL;
  EtchObject* integer = new EtchInt32(4);
  EtchObject* str2 = new EtchString("hello");
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorString::Get(0, ptr) == ETCH_OK);
  EXPECT_TRUE(((EtchTypeValidator*) ptr.get())->getNDims() == 0);
  EXPECT_FALSE(ptr->validate(str));
  EXPECT_FALSE(ptr->validate(integer));
  EXPECT_TRUE(ptr->validate(str2));
  delete integer;
  delete str2;
}

TEST(EtchValidatorStringTest, validateValueTest) {
  EtchObject* str = NULL;
  EtchObject* result;
  EtchObject* integer = new EtchInt32(4);
  EtchObject* str2 = new EtchString("hello");
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorString::Get(0, ptr) == ETCH_OK);
  EXPECT_TRUE(ptr->validateValue(str, result) == ETCH_ERROR);
  EXPECT_TRUE(ptr->validateValue(integer, result) == ETCH_ERROR);
  EXPECT_TRUE(ptr->validateValue(str2, result) == ETCH_OK);
  EXPECT_TRUE(((EtchString*) result)->equals((EtchString*) str2));
  delete integer;
  delete str2;
}

TEST(EtchValidatorStringTest, elementValidatorTest) {
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorString::Get(1, ptr) == ETCH_OK);
  capu::SmartPointer<EtchValidator> elementValidator;
  ptr->getElementValidator(elementValidator);

  EtchObject* str = NULL;
  EtchObject* integer = new EtchInt32(4);
  EtchObject* str2 = new EtchString("hello");

  EXPECT_FALSE(elementValidator->validate(str));
  EXPECT_FALSE(elementValidator->validate(integer));
  EXPECT_TRUE(elementValidator->validate(str2));
  delete integer;
  delete str2;
}
