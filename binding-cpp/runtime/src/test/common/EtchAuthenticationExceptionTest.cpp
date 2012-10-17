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
#include "common/EtchAuthenticationException.h"

TEST(EtchAuthenticationExceptionTest, createTest) {

  EtchString str("test message");
  EtchAuthenticationException *test = NULL;
  test = new EtchAuthenticationException(str);
  EXPECT_TRUE(test != NULL);
  delete test;
}

TEST(EtchAuthenticationExceptionTest, getTests) {
  EtchString str("test message");
  EtchAuthenticationException *test = NULL;
  test = new EtchAuthenticationException(str);
  EXPECT_TRUE(test != NULL);
  EXPECT_TRUE(test->getErrorMessage().equals(&str));
  EXPECT_TRUE(test->getErrorCode() == ETCH_ERROR);
  EXPECT_TRUE(test->getExceptionType() == EtchException::EXCPTYPE_BUILTIN);
  delete test;
}

TEST(EtchAuthenticationExceptionTest, equalsTest) {
  EtchString str("test message");
  EtchString str2("test2 message");
  EtchAuthenticationException *test = NULL;
  test = new EtchAuthenticationException(str);
  EtchAuthenticationException *test2 = NULL;
  test2 = new EtchAuthenticationException(str2);
  EtchAuthenticationException *test3 = NULL;
  test3 = new EtchAuthenticationException(str);

  EXPECT_TRUE(test != NULL);
  EXPECT_TRUE(test2 != NULL);
  EXPECT_TRUE(test3 != NULL);

  EXPECT_TRUE(test->equals(test3));
  EXPECT_FALSE(test->equals(test2));

  delete test;
  delete test2;
  delete test3;
}

TEST(EtchAuthenticationExceptionTest, copyTest) {
  EtchString str("test message");
  EtchAuthenticationException o1(str);
  EtchAuthenticationException o2(o1);
  EtchAuthenticationException o3 = o2;
  EXPECT_TRUE(o1.equals(&o2));
  EXPECT_TRUE(o2.equals(&o3));
}

TEST(EtchAuthenticationExceptionTest, isInstanceOf) {
  EtchString str("test message");
  EtchObject* o1 = new EtchAuthenticationException(str);
  EXPECT_TRUE(o1->isInstanceOf(EtchObject::TYPE()));
  EXPECT_TRUE(o1->isInstanceOf(EtchException::TYPE()));
  EXPECT_TRUE(o1->isInstanceOf(EtchAuthenticationException::TYPE()));
  EXPECT_FALSE(o1->isInstanceOf(EtchString::TYPE()));
  delete o1;
}
