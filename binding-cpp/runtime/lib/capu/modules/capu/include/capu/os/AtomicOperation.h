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

#ifndef __ATOMIC_OPERATION_H__
#define __ATOMIC_OPERATION_H__

#include "capu/Config.h"

#define ATOMIC_OPERATION_INC_HEADER
#include "arch/AtomicOperation.inc"
#undef ATOMIC_OPERATION_INC_HEADER

namespace capu
{
  class AtomicOperation { 
  public:

    /**
    * atomically add 'summand' to an uint32_t
    * @param mem reference to the object
    * @param summand amount to add
    */
    static void AtomicAdd32(volatile uint32_t &mem, uint32_t summand);  

    /**
    * atomically subtract 'substrahend' from an uint32_t
    * @param mem reference to the object
    * @param subtrahend amount to subtract
    */
    static void AtomicSub32(volatile uint32_t &mem, uint32_t subtrahend);

    /**
    * atomically increment an uint32_t
    * @param mem reference to the object
    */
    static void AtomicInc32(volatile uint32_t &mem);  

    /**
    * atomically decrement  an uint32_t
    * @param mem reference to the object
    */
    static void AtomicDec32(volatile uint32_t &mem);
  };

#define ATOMIC_OPERATION_INC_IMPL
#include "arch/AtomicOperation.inc"
#undef ATOMIC_OPERATION_INC_IMPL
}

#endif
