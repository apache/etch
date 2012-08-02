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
    //TODO: Make this thread-safe
    uint32_t oldValue = mem;
  __asm__ volatile (
   "1: ldrex r0, [%0]      \n\t" //load mem into r0
   "add      r0,  r0,  %1  \n\t" //add summand to r0 and store result in r0
   "strex    r1,  r0, [%0] \n\t" //store result from register back to mem
   "cmp      r1,  #0       \n\t" //check if we have had exclusive access
   "bne      1b"                 //if there was no exclusive access, try it again
   : /* no output register needed */  
   : "r"   (&mem), "r"(summand)
   : "r0"  , "r1"
  );

   return oldValue;
}

inline uint32_t AtomicOperation::AtomicSub32(volatile uint32_t &mem, uint32_t subtrahend) {
  //TODO: Make this thread-safe
  uint32_t oldValue = mem;

  __asm__ volatile (
   "2: ldrex r0, [%0]      \n\t" //load mem into r0
   "sub      r0,  r0,  %1  \n\t" //subs subthrahend from r0 and store result in r0
   "strex    r1,  r0, [%0] \n\t" //store result from register back to mem
   "cmp      r1,  #0       \n\t" //check if we have had exclusive access
   "bne      2b"                 //if there was no exclusive access, try it again
   : /* no output register needed */
   : "r"   (&mem), "r"(subtrahend)
   : "r0"  , "r1"
  );

   return oldValue;
}

inline uint32_t AtomicOperation::AtomicInc32(volatile uint32_t &mem){
  return AtomicAdd32(mem, 1);
}

inline uint32_t AtomicOperation::AtomicDec32(volatile uint32_t &mem) {
  return AtomicSub32(mem, 1);
}
#endif
