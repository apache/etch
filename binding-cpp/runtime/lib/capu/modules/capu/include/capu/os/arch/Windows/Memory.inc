/* $Id$
* Licensed to the Apache Software Foundation (ASF) under one or more
*
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

#ifdef MEMORY_INC_HEADER
#include <memory.h>
#include "capu/Config.h"
#endif

#ifdef MEMORY_INC_MEMBER
#endif

#ifdef MEMORY_INC_IMPL

inline
void Memory::Set(void* dst, int32_t val, uint_t size)
{
  memset(dst, val, size);
}

inline
void Memory::Copy(void* dst, const void* src, uint_t size)
{
  memcpy(dst, src, size);
}

inline
void Memory::Move(void* dst, const void* src, uint_t size)
{
  memmove(dst, src, size);
}

int32_t Memory::Compare(const void* ptr1, const void* ptr2, uint_t num)
{
  return memcmp(ptr1, ptr2, num);
}
#endif
