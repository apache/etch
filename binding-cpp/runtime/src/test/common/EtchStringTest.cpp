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
#include "common/EtchString.h"
#include "common/EtchInt32.h"

// Tests positive input.

TEST(EtchStringTest, Constructor_Default) {
  EtchString* s1 = new EtchString();
  EXPECT_TRUE(s1->getObjectType()->equals(EtchString::TYPE()));
  EXPECT_EQ(NULL, s1->c_str());
  delete s1;
}

TEST(EtchStringTest, Constructor_Char) {
  EtchString* s1 = new EtchString("test");
  EXPECT_TRUE(s1->getObjectType()->equals(EtchString::TYPE()));
  EXPECT_TRUE(s1->c_str() != NULL);
  EXPECT_TRUE(strcmp(s1->c_str(), "test") == 0);
  delete s1;
}

TEST(EtchStringTest, set) {
  EtchString* s1 = new EtchString("test");
  s1->set("hello");
  EXPECT_TRUE(s1->c_str() != NULL);
  EXPECT_TRUE(strcmp(s1->c_str(), "hello") == 0);
  delete s1;
}

TEST(EtchStringTest, setLen) {
  EtchString* s1 = new EtchString("test");
  s1->set("hello", 3);
  EXPECT_TRUE(s1->c_str() != NULL);
  EXPECT_TRUE(strcmp(s1->c_str(), "hel") == 0);
  delete s1;
}

TEST(EtchStringTest, assign) {
  EtchString *s = new EtchString("hello");
  *s = "hallo2";
  EXPECT_TRUE(strcmp(s->c_str(), "hallo2") == 0);
  EtchString _tmp("hi");
  *s = _tmp;
  EXPECT_TRUE(strcmp(s->c_str(), "hi") == 0);
  delete s;
}

TEST(EtchStringTest, c_str) {
  EtchString *s = new EtchString(NULL);
  EXPECT_TRUE(s->c_str() == NULL);

  *s = "hello";
  EXPECT_TRUE(strcmp(s->c_str(), "hello") == 0);

  EtchString as("hallo2");
  EXPECT_TRUE(strcmp(as.c_str(), "hallo2") == 0);

  delete s;
}

TEST(EtchStringTest, equalsTest) {
  EtchString str1("test");
  EtchString str2("test");
  EtchString str3("test2");

  EXPECT_TRUE(str1.equals(&str2));
  EXPECT_FALSE(str1.equals(&str3));
}

TEST(EtchStringTest, length) {
  EtchString str1("test");
  EtchString str2("");

  EXPECT_EQ(4, str1.length());
  EXPECT_EQ(0, str2.length());
}

TEST(EtchStringTest, leftFindTest) {
  EtchString str1("test1:test2:test3");
  capu::int32_t index = str1.leftFind(':');
  EXPECT_TRUE(index == 5);
  index = str1.leftFind('<');
  EXPECT_TRUE(index == -1);
}

TEST(EtchStringTest, rightFindTest) {
  EtchString str1("test1:test2:test3");
  capu::int32_t index = str1.rightFind(':');
  EXPECT_TRUE(index == 11);
  index = str1.rightFind('<');
  EXPECT_TRUE(index == -1);
}

TEST(EtchStringTest, substringTest) {
  EtchString str1("test1:test2:test3");

  EtchString tmp;
  status_t result;
  result = str1.substring(0, 5, &tmp);
  EXPECT_TRUE(result == ETCH_OK);
  EXPECT_TRUE(strcmp("test1", tmp.c_str()) == 0);

  result = str1.substring(0, 5, NULL);
  EXPECT_TRUE(result == ETCH_EINVAL);

  result = str1.substring(0, 90, &tmp);
  EXPECT_TRUE(result == ETCH_EINVAL);

  result = str1.substring(-5, 7, &tmp);
  EXPECT_TRUE(result == ETCH_EINVAL);

  result = str1.substring(5, -7, &tmp);
  EXPECT_TRUE(result == ETCH_EINVAL);

  result = str1.substring(-5, -7, &tmp);
  EXPECT_TRUE(result == ETCH_EINVAL);

  result = str1.substring(str1.length() - 1, 1, &tmp);
  EXPECT_TRUE(result == ETCH_OK);
  EXPECT_TRUE(strcmp("3", tmp.c_str()) == 0);
}

TEST(EtchStringTest, UTF8_Constructor_Default) {
  const capu::uint8_t utf8 [] = {0xF0, 0xA4, 0xAD, 0xA2, 0xE2, 0x82, 0xAC, 0xC2, 0xA2, 0x24};
  EtchString* s1 = new EtchString((capu::int8_t*)utf8, 10, "utf-8");
  EXPECT_TRUE(s1->getObjectType()->equals(EtchString::TYPE()));
  EXPECT_TRUE(s1->c_str() != NULL);
  delete s1;
}

TEST(EtchStringTest, UTF8_len_test) {
  const capu::uint8_t utf8 [] = {0xF0, 0xA4, 0xAD, 0xA2, 0xE2, 0x82, 0xAC, 0xC2, 0xA2, 0x24};
  EtchString *s = new EtchString((capu::int8_t*)utf8, 10, "utf-8");
  EXPECT_TRUE(s->getNumBytes() == 10);
  EXPECT_TRUE(s->length() == 4);
  delete s;
}

TEST(EtchStringTest, UTF8_equalsTest) {
  const capu::uint8_t utf8_1 [] = {0xF0, 0xA4, 0xAD, 0xA2, 0xE2, 0x82, 0xAC, 0xC2, 0xA2, 0x24};
  const capu::uint8_t utf8_2 [] = {0xF0, 0xA4, 0xAD, 0xA2, 0xE2, 0x82, 0xAC, 0xC2, 0xA2, 0x24};
  const capu::uint8_t utf8_3 [] = {0xF0, 0xA4, 0xAD, 0xA2, 0xE2, 0x82, 0xAC, 0xC2, 0xA2, 0x25};
  EtchString str1((capu::int8_t*)utf8_1, 10, "utf-8");
  EtchString str2((capu::int8_t*)utf8_2, 10, "utf-8");
  EtchString str3((capu::int8_t*)utf8_3, 10, "utf-8");

  EXPECT_TRUE(str1.equals(&str2));
  EXPECT_FALSE(str1.equals(&str3));
}

TEST(EtchStringTest, UTF8_leftFindTest) {
  //not supported
  const capu::uint8_t utf8_1 [] = {0xF0, 0xA4, 0xAD, 0xA2, 0xE2, 0x82, 0xAC, 0xC2, 0xA2, 0x24};
  EtchString str1((capu::int8_t*)utf8_1, 10, "utf-8");
  capu::int32_t index = str1.leftFind(':');
  EXPECT_TRUE(index == -1);
}

TEST(EtchStringTest, UTF8_rightFindTest) {
  const capu::uint8_t utf8_1 [] = {0xF0, 0xA4, 0xAD, 0xA2, 0xE2, 0x82, 0xAC, 0xC2, 0xA2, 0x24};
  EtchString str1((capu::int8_t*)utf8_1, 10, "utf-8");
  capu::int32_t index = str1.rightFind(':');
  EXPECT_TRUE(index == -1);
}

TEST(EtchStringTest, UTF8_substringTest) {
  const capu::uint8_t utf8_1 [] = {0xF0, 0xA4, 0xAD, 0xA2, 0xE2, 0x82, 0xAC, 0xC2, 0xA2, 0x24};
  EtchString str1((capu::int8_t*)utf8_1, 10, "utf-8");

  EtchString tmp;
  status_t result;
  result = str1.substring(0, 5, &tmp);
  EXPECT_TRUE(result == ETCH_EUNIMPL);
}
TEST(EtchStringTest, copyTest) {
  EtchString o1("2");
  EtchString o2(o1);
  EtchString o3 = o2;
  EXPECT_TRUE(o1.equals(&o2));
  EXPECT_TRUE(o2.equals(&o3));
}

TEST(EtchStringTest, isInstanceOf) {
  EtchObject* o1 = new EtchString("2");
  EXPECT_TRUE(o1->isInstanceOf(EtchObject::TYPE()));
  EXPECT_TRUE(o1->isInstanceOf(EtchString::TYPE()));
  EXPECT_FALSE(o1->isInstanceOf(EtchInt32::TYPE()));
  delete o1;
}
