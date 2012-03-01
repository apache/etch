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

#define CAPU_PRIMITIVE 1
#define CAPU_CLASS     0

  //is CAPU_PRIMITIVE
  template<typename T> struct is_CAPU_PRIMITIVE { enum { Value = CAPU_CLASS }; };
  template<> struct is_CAPU_PRIMITIVE<int8_t  > { enum { Value = CAPU_PRIMITIVE }; };
  template<> struct is_CAPU_PRIMITIVE<int16_t > { enum { Value = CAPU_PRIMITIVE }; };
  template<> struct is_CAPU_PRIMITIVE<int32_t > { enum { Value = CAPU_PRIMITIVE }; };
  template<> struct is_CAPU_PRIMITIVE<int64_t > { enum { Value = CAPU_PRIMITIVE }; };
  template<> struct is_CAPU_PRIMITIVE<uint32_t> { enum { Value = CAPU_PRIMITIVE }; };
  template<> struct is_CAPU_PRIMITIVE<uint64_t> { enum { Value = CAPU_PRIMITIVE }; };
  template<> struct is_CAPU_PRIMITIVE<float_t > { enum { Value = CAPU_PRIMITIVE }; };
  template<> struct is_CAPU_PRIMITIVE<double_t> { enum { Value = CAPU_PRIMITIVE }; };
  template<> struct is_CAPU_PRIMITIVE<bool_t  > { enum { Value = CAPU_PRIMITIVE }; };

  //References Helper (T &)
  template<typename T, int TYPE> struct _ReferenceType {};
  template<typename T> struct _ReferenceType<T, CAPU_CLASS     > { typedef T &Type; };
  template<typename T> struct _ReferenceType<T, CAPU_PRIMITIVE > { typedef T Type;  };
  
  //ConstReferences Helper (const T &)
  template<typename T, int TYPE> struct _ConstReferenceType {};
  template<typename T> struct _ConstReferenceType<T, CAPU_CLASS      > { typedef const T &Type; };
  template<typename T> struct _ConstReferenceType<T, CAPU_PRIMITIVE  > { typedef const T Type;  };

  //References (T &)
  template<typename T>
  struct ReferenceType
  {
   typedef typename _ReferenceType<T, is_CAPU_PRIMITIVE<T>::Value >::Type Type;
  };
    
  //ConstReferences (const T &)
  template<typename T>
  struct ConstReferenceType
  {
   typedef typename _ConstReferenceType<T, is_CAPU_PRIMITIVE<T>::Value >::Type Type;
  };

}

#endif /* __TRAITS_H__ */