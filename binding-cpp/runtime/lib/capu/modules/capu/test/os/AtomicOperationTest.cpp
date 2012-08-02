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
#include "capu/os/AtomicOperation.h"

//include tests depending on architecture
#include "arch/AtomicOperation.inc"

TEST(AtomicOperation,Add) {
  capu::uint32_t val = 100;
  capu::uint32_t ret = capu::AtomicOperation::AtomicAdd32(val, 3);
  EXPECT_EQ(103u,val);
  EXPECT_EQ(100u, ret);
}

TEST(AtomicOperation,Sub) {
  capu::uint32_t val = 13;
  capu::uint32_t ret = capu::AtomicOperation::AtomicSub32(val, 5);
  EXPECT_EQ(8u,val);
  EXPECT_EQ(13u, ret);
}

TEST(AtomicOperation,Inc) {
  capu::uint32_t val = 1;
  capu::uint32_t ret = capu::AtomicOperation::AtomicInc32(val);
  EXPECT_EQ(2u,val);
  EXPECT_EQ(1u, ret);
}

TEST(AtomicOperation,Dec) {
  capu::uint32_t val = 3;
  capu::uint32_t ret = capu::AtomicOperation::AtomicDec32(val);
  EXPECT_EQ(2u,val);
  EXPECT_EQ(3u, ret);
}
