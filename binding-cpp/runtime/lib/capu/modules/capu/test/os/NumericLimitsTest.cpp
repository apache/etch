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
#include "capu/os/NumericLimits.h"

TEST(NumericLimits,testLimits)
{
  EXPECT_EQ(capu::NumericLimitMax<capu::int32_t>(), static_cast<capu::int32_t>(0x7fffffff));
  EXPECT_EQ(capu::NumericLimitMin<capu::int32_t>(), static_cast<capu::int32_t>(0x80000000));
  EXPECT_EQ(capu::NumericLimitMax<capu::int16_t>(), static_cast<capu::int32_t>(32767));
  EXPECT_EQ(capu::NumericLimitMin<capu::int16_t>(), static_cast<capu::int32_t>(-32768));
  EXPECT_EQ(capu::NumericLimitMax<capu::int8_t>(), static_cast<capu::int32_t>(127));
  EXPECT_EQ(capu::NumericLimitMin<capu::int8_t>(), static_cast<capu::int32_t>(-128));
}
