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

#ifndef __MEMORY_H__
#define __MEMORY_H__

#include "capu/Config.h"
#include "capu/Error.h"

#define MEMORY_INC_HEADER
#include "arch/Memory.inc"
#undef MEMORY_INC_HEADER

namespace capu
{
    class Memory
    {
     #define MEMORY_INC_MEMBER
         #include "arch/Memory.inc"
     #undef MEMORY_INC_MEMBER

    public:
      inline static void Set (void* dst, int32_t val    , uint_t size);
      inline static void Copy(void* dst, const void* src, uint_t size);
      inline static void Move(void* dst, const void* src, uint_t size);
      inline static int32_t Compare(const void* ptr1, const void* ptr2, uint_t num);

    };

#define MEMORY_INC_IMPL
    #include "arch/Memory.inc"
#undef MEMORY_INC_IMPL

}
#endif

