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
#include <serialization/EtchType.h>
#include "serialization/EtchValidatorInt.h"
#include "serialization/EtchValidatorLong.h"
#include "serialization/EtchValidatorByte.h"

TEST(EtchTypeTest, createTest) {
  EtchString str("one");
  EtchType *type = new EtchType(1, str);
  EXPECT_TRUE(type != NULL);
  delete type;
}

TEST(EtchTypeTest, putValidatorTest) {
  EtchString str1("one");
  EtchString str2("two");
  EtchString str3("three");
  EtchType t(1, str1);
  EtchField f(str2);
  EtchField g(str3);
  capu::SmartPointer<EtchValidator> v1;
  EtchValidatorInt::Get(0, v1);
  capu::SmartPointer<EtchValidator> v2;
  EtchValidatorLong::Get(0, v2);
  capu::SmartPointer<EtchValidator> v3;
  EtchValidatorByte::Get(0, v3);
  EXPECT_EQ(t.putValidator(f, v1), ETCH_OK);
  EXPECT_EQ(t.putValidator(g, v2), ETCH_OK);
  EXPECT_EQ(t.putValidator(g, v3), ETCH_OK); //makes a combo generator
  EXPECT_EQ(t.getValidator(f, v2), ETCH_OK);
  EXPECT_EQ(v2, v1);
}

TEST(EtchTypeTest, getValidatorTest) {
  EtchString str1("one");
  EtchString str2("two");
  EtchString str3("three");
  EtchType t(1, str1);
  EtchField f(str2);
  EtchField g(str3);
  capu::SmartPointer<EtchValidator> v;
  EtchValidatorInt::Get(0, v);
  capu::SmartPointer<EtchValidator> result = NULL;
  t.putValidator(f, v);
  EXPECT_EQ(t.getValidator(f, result), ETCH_OK);
  EXPECT_EQ(v, result);
  EXPECT_EQ(t.getValidator(g, result), ETCH_ENOT_EXIST);
}

TEST(EtchTypeTest, clearTest) {
  EtchString str1("one");
  EtchString str2("two");
  EtchString str3("three");
  EtchType t(1, str1);
  EtchField f(str2);
  EtchField g(str3);
  capu::SmartPointer<EtchValidator> v1;
  EtchValidatorInt::Get(0, v1);
  capu::SmartPointer<EtchValidator> v2;
  EtchValidatorLong::Get(0, v2);
  EXPECT_EQ(t.putValidator(f, v1), ETCH_OK);
  EXPECT_EQ(t.putValidator(g, v2), ETCH_OK);
  EXPECT_EQ(t.clearValidator(f), ETCH_OK);
  EXPECT_EQ(t.clearValidator(g), ETCH_OK);
  EXPECT_EQ(t.clearValidator(g), ETCH_ERANGE);
}

TEST(EtchTypeTest, setAndGetTest) {
  EtchString str1("one");
  EtchString str2("two");
  EtchString str3("three");
  EtchType t(1, str1);
  EtchField f(str2);
  EtchField g(str3);
  t.setAsyncMode(NONE);
  t.setDirection(BOTH);
  t.setResponseField(f);
  t.setResult(NULL);
  t.setSuperType(NULL);
  t.setTimeout(30);
  EXPECT_TRUE(t.getAsyncMode() == NONE);
  EXPECT_TRUE(t.getDirection() == BOTH);
  EXPECT_TRUE(t.getTimeout() == 30);
  EXPECT_TRUE(t.getResult() == NULL);
  EXPECT_TRUE(t.getResponseField().equals(&f));
  EXPECT_TRUE(t.getSuperType() == NULL);
}

TEST(EtchTypeTest, lockTest) {
  EtchString str1("one");
  EtchString str2("two");
  EtchString str3("three");
  EtchType t(1, str1);
  EtchField f(str2);
  EtchField g(str3);
  t.setAsyncMode(NONE);
  t.setDirection(BOTH);
  t.setResponseField(f);
  t.setResult(NULL);
  t.setSuperType(NULL);
  t.setTimeout(30);
  EXPECT_TRUE(t.getAsyncMode() == NONE);
  EXPECT_TRUE(t.getDirection() == BOTH);
  EXPECT_TRUE(t.getTimeout() == 30);
  EXPECT_TRUE(t.getResult() == NULL);
  EXPECT_TRUE(t.getResponseField().equals(&f));
  EXPECT_TRUE(t.getSuperType() == NULL);
  //after it is locked
  EtchType* t2 = new EtchType(1, str1);
  t.lock();
  t.setAsyncMode(QUEUED);
  t.setDirection(SERVER);
  t.setResponseField(g);

  t.setResult(t2);
  t.setSuperType(t2);
  t.setTimeout(50);
  //it should not change
  EXPECT_TRUE(t.getAsyncMode() == NONE);
  EXPECT_TRUE(t.getDirection() == BOTH);
  EXPECT_TRUE(t.getTimeout() == 30);
  EXPECT_TRUE(t.getResult() == NULL);
  EXPECT_TRUE(t.getResponseField().equals(&f));
  EXPECT_TRUE(t.getSuperType() == NULL);
  delete t2;
}
