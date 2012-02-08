/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef __TRAITS_H__
#define __TRAITS_H__

#include "capu/Config.h"

namespace capu {

  template<typename T>
  struct ReferenceType { typedef T &Type; };

  template<typename T>
  struct ReferenceType<T*> { typedef T *Type; };

  template<>
  struct ReferenceType<int8_t> { typedef int8_t Type; };

  template<>
  struct ReferenceType<int16_t> { typedef int16_t Type; };
  
  template<>
  struct ReferenceType<int32_t> { typedef int32_t Type; };
  
  template<>
  struct ReferenceType<int64_t> { typedef int64_t Type; };
  
  template<>
  struct ReferenceType<uint32_t> { typedef uint32_t Type; };
  
  template<>
  struct ReferenceType<uint64_t> { typedef uint64_t Type; };
  
  template<>
  struct ReferenceType<float_t> { typedef float_t Type; };
  
  template<>
  struct ReferenceType<double_t> { typedef double_t Type; };
  
  template<>
  struct ReferenceType<bool_t> { typedef bool_t Type; };

  template<>
  struct ReferenceType<char*> { typedef char* Type; };

  template<>
  struct ReferenceType<const char*> { typedef const char* Type; };
}

#endif /* __TRAITS_H__ */