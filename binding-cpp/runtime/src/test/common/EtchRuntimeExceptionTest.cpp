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
#include "common/EtchRuntimeException.h"

TEST(EtchRuntimeExceptionTest, createTest) {
  EtchString str("test message");
  EtchRuntimeException *test = NULL;
  test = new EtchRuntimeException(str, ETCH_ERROR);
  EXPECT_TRUE(test != NULL);
  delete test;
}

TEST(EtchRuntimeExceptionTest, getTests) {
  EtchString str("test message");
  EtchRuntimeException *test = NULL;
  test = new EtchRuntimeException(str, ETCH_ERROR);
  EXPECT_TRUE(test != NULL);
  EXPECT_TRUE(test->getErrorMessage().equals(&str));
  EXPECT_TRUE(test->getErrorCode() == ETCH_ERROR);
  EXPECT_TRUE(test->getExceptionType() == EtchException::EXCPTYPE_BUILTIN);
  delete test;
}

TEST(EtchRuntimeExceptionTest, equalsTest) {
  EtchString str("test message");
  EtchString str2("test2 message");
  EtchRuntimeException *test = NULL;
  test = new EtchRuntimeException(str, ETCH_ERROR);
  EtchRuntimeException *test2 = NULL;
  test2 = new EtchRuntimeException(str2, ETCH_EINVAL);
  EtchRuntimeException *test3 = NULL;
  test3 = new EtchRuntimeException(str, ETCH_ERROR);

  EXPECT_TRUE(test != NULL);
  EXPECT_TRUE(test2 != NULL);
  EXPECT_TRUE(test3 != NULL);

  EXPECT_TRUE(test->equals(test3));
  EXPECT_FALSE(test->equals(test2));

  delete test;
  delete test2;
  delete test3;
}

TEST(EtchRuntimeExceptionTest, copyTest) {
  EtchRuntimeException o1(EtchString("Message"), ETCH_ERROR);
  EtchRuntimeException o2(o1);
  EtchRuntimeException o3 = o2;
  EXPECT_TRUE(o1.equals(&o2));
  EXPECT_TRUE(o2.equals(&o3));
}

TEST(EtchRuntimeExceptionTest, isInstanceOf) {
  EtchObject* o1 = new EtchRuntimeException(EtchString("Message"), ETCH_ERROR);
  EXPECT_TRUE(o1->isInstanceOf(EtchObject::TYPE()));
  EXPECT_TRUE(o1->isInstanceOf(EtchException::TYPE()));
  EXPECT_TRUE(o1->isInstanceOf(EtchRuntimeException::TYPE()));
  EXPECT_FALSE(o1->isInstanceOf(EtchString::TYPE()));
  delete o1;
}
