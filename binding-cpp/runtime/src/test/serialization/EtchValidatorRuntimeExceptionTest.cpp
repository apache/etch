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
#include "serialization/EtchValidatorRuntimeException.h"
#include "capu/util/SmartPointer.h"
#include "common/EtchInt32.h"
#include "common/EtchRuntimeException.h"
#include "support/EtchRuntime.h"

class EtchValidatorRuntimeExceptionTest
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

TEST_F(EtchValidatorRuntimeExceptionTest, createTest) {
  capu::SmartPointer<EtchValidatorRuntimeException> ptr = NULL;
  capu::SmartPointer<EtchValidator> val;
  EXPECT_TRUE(EtchValidatorRuntimeException::Get(mRuntime, val) == ETCH_OK);
  ptr = capu::smartpointer_cast<EtchValidatorRuntimeException>(val);
}

TEST_F(EtchValidatorRuntimeExceptionTest, validateTest) {
  EtchString excepmess("test");
  capu::SmartPointer<EtchObject> boolean = NULL;
  capu::SmartPointer<EtchObject> integer = new EtchInt32(4);
  capu::SmartPointer<EtchObject> exc = new EtchRuntimeException(excepmess, ETCH_ERROR);
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorRuntimeException::Get(mRuntime, ptr) == ETCH_OK);
  EXPECT_FALSE(ptr->validate(boolean));
  EXPECT_FALSE(ptr->validate(integer));
  EXPECT_TRUE(ptr->validate(exc));

}

TEST_F(EtchValidatorRuntimeExceptionTest, validateValueTest) {
  EtchString excepmess("test");

  capu::SmartPointer<EtchObject> boolean = NULL;
  capu::SmartPointer<EtchObject> result;
  capu::SmartPointer<EtchObject> integer = new EtchInt32(4);
  capu::SmartPointer<EtchObject> exc = new EtchRuntimeException(excepmess, ETCH_ERROR);
  capu::SmartPointer<EtchValidator> ptr = NULL;
  EXPECT_TRUE(EtchValidatorRuntimeException::Get(mRuntime, ptr) == ETCH_OK);
  EXPECT_TRUE(ptr->validateValue(boolean, result) == ETCH_ERROR);
  EXPECT_TRUE(ptr->validateValue(integer, result) == ETCH_ERROR);
  EXPECT_TRUE(ptr->validateValue(exc, result) == ETCH_OK);
}
