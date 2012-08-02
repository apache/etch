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

 TEST(AtomicOperation,Add_NoOverflow_X86_64) {
  capu::uint32_t val = 4294967295u;
  capu::AtomicOperation::AtomicAdd32(val, 3);
  EXPECT_EQ((capu::uint32_t) 4294967298u,val);
}

//disabled until *64 methods are implemented
/*TEST(AtomicOperation,Add64_Overflow_X86_64) {
  capu::uint64_t val = 18446744073709551615u;
  capu::AtomicOperation::AtomicAdd64(val, 3);
  EXPECT_EQ((capu::uint32_t) 2,val);
}

TEST(AtomicOperation,Sub64_Overflow_X86_64) {
  capu::uint64_t val = 0;
  capu::AtomicOperation::AtomicSub64(val, 5);
  EXPECT_EQ((capu::uint32_t) 18446744073709551610u,val);
}
*/
