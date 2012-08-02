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
#include "capu/os/Memory.h"

TEST(Memory,compare)
{
  char string1[30] = "This is a String";
  char string2[30] = "This is a String";
  char string3[30] = "This is another String";

  EXPECT_EQ(0,capu::Memory::Compare(string1,string2,30));
  EXPECT_FALSE(0 == capu::Memory::Compare(string2,string3,30));
}

TEST(Memory,set)
{
  char string[] = "Hello World";
  capu::Memory::Set(string,'m',5);
  EXPECT_EQ(0,capu::Memory::Compare("mmmmm World",string,strlen(string)));
}

TEST(Memory,copy)
{
  char string1[30] = "This is a String";
  char string2[30];
  capu::Memory::Copy(string2,string1,strlen(string1)+1);
  EXPECT_EQ(0,capu::Memory::Compare(string1,string2,strlen(string1)));
}


TEST(Memory,move)
{
  char string1[35] = "This is a boring String";
  capu::Memory::Move(string1+17,string1+10,13);
  char string2[35] = "This is a boring boring String";
  EXPECT_EQ(0,capu::Memory::Compare(string2,string1,35));
}

