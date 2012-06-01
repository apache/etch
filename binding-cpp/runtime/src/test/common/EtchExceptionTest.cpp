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
#include "common/EtchException.h"

TEST(EtchExceptionTest, createTest) {
  EtchString str("test message");
  EtchException *test = NULL;
  test = new EtchException(str, ETCH_ERROR, EtchException::EXCPTYPE_BUILTIN);
  EXPECT_TRUE(test != NULL);
  delete test;
}

TEST(EtchExceptionTest, getTests) {
  EtchString str("test message");
  EtchException *test = NULL;
  test = new EtchException(str, ETCH_ERROR, EtchException::EXCPTYPE_BUILTIN);
  EXPECT_TRUE(test != NULL);
  EXPECT_TRUE(test->getErrorMessage().equals(&str));
  EXPECT_TRUE(test->getErrorCode() == ETCH_ERROR);
  EXPECT_TRUE(test->getExceptionType() == EtchException::EXCPTYPE_BUILTIN);
  delete test;
}

TEST(EtchExceptionTest, equalsTest) {
  EtchString str("test message");
  EtchString str2("test2 message");
  EtchException *test = NULL;
  test = new EtchException(str, ETCH_ERROR, EtchException::EXCPTYPE_BUILTIN);
  EtchException *test2 = NULL;
  test2 = new EtchException(str2, ETCH_EINVAL, EtchException::EXCPTYPE_BUILTIN);
  EtchException *test3 = NULL;
  test3 = new EtchException(str, ETCH_ERROR, EtchException::EXCPTYPE_BUILTIN);

  EXPECT_TRUE(test != NULL);
  EXPECT_TRUE(test2 != NULL);
  EXPECT_TRUE(test3 != NULL);

  EXPECT_TRUE(test->equals(test3));
  EXPECT_FALSE(test->equals(test2));

  delete test;
  delete test2;
  delete test3;
}
