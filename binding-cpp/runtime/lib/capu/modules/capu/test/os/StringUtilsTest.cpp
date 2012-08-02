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
#include "capu/os/StringUtils.h"


TEST(StringUtils,Strcmp)
{
  char string1[] = "My String";
  char string2[] = "My String";
  char string3[] = "Another String";
  EXPECT_EQ(0,capu::StringUtils::Strcmp(string1,string2));
  EXPECT_TRUE(0 <= capu::StringUtils::Strcmp(string1,string3));
}

TEST(StringUtils,Strncpy)
{
  char string1[20] = "My String";
  char string2[20];
  capu::StringUtils::Strncpy(string2,20,string1);
  EXPECT_EQ(0,capu::StringUtils::Strcmp(string1,string2));
}

TEST(StringUtils,Strlen)
{
  char string1[20] = "My String";
  EXPECT_TRUE(9 == capu::StringUtils::Strlen(string1));
}

TEST(StringUtils,Sprintf)
{
  char string1[20];
  capu::StringUtils::Sprintf(string1,20,"%d",12345);
  EXPECT_EQ(0,capu::StringUtils::Strcmp("12345",string1));
  capu::StringUtils::Sprintf(string1,20,"%d %f",12345,12.45);
  EXPECT_EQ(0,capu::StringUtils::Strcmp("12345 12.450000",string1));
}

class VscprintfTest {
  public:
    static capu::int32_t Vscprintf(const char* format, ...) {
      capu::int32_t length = 0;
      va_list args;
      va_start(args, format);
      length = capu::StringUtils::Vscprintf("This is a test! %d", args);
      va_end(args);
      return length;
    }
};

TEST(StringUtils,Vscprintf)
{
  capu::int32_t length = VscprintfTest::Vscprintf("This is a test! %d", 12345);
  EXPECT_EQ(21, length);
}

