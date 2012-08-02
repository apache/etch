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

#ifdef ATOMIC_OPERATION_INC_HEADER
#endif

#ifdef ATOMIC_OPERATION_INC_MEMBER
#endif

#ifdef ATOMIC_OPERATION_INC_IMPL
inline uint32_t AtomicOperation::AtomicAdd32(volatile uint32_t &mem, uint32_t summand) {
//This code comes from the Apache APR project (apache-apr/1.4.2/atomic/unix/ia32.c)
  asm volatile (
    "lock; xaddl %0,%1"
  : "=r" (summand), "=m" (mem)
  : "0" (summand), "m" (mem)
  : "memory", "cc");

  return summand;

}

inline uint32_t AtomicOperation::AtomicSub32(volatile uint32_t &mem, uint32_t subtrahend) {
//This code comes from the Apache APR project (apache-apr/1.4.2/atomic/unix/ia32.c)
  int32_t summand = subtrahend * -1;
  asm volatile (
    "lock; xaddl %0,%1"
  : "=r" (summand), "=m" (mem)
  : "0" (summand), "m" (mem)
  : "memory", "cc");

    return summand;
}

inline uint32_t AtomicOperation::AtomicInc32(volatile uint32_t &mem) {
  return AtomicAdd32(mem, 1);
}

inline uint32_t AtomicOperation::AtomicDec32(volatile uint32_t &mem) { 
  return AtomicSub32(mem, 1);
}
#endif
