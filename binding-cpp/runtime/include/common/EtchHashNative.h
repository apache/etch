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

#ifndef __ETCHHASHNATIVE_H__
#define __ETCHHASHNATIVE_H__
#include "common/EtchConfig.h"
#include "capu/container/Hash.h"
#include "capu/util/Traits.h"

class EtchHashNative {
public:

  template<class T>
  static capu::uint_t Digest(T &key, const capu::uint8_t bitSize) {
    return capu::CapuDefaultHashFunction::Digest<T>(key, bitSize);
  }

  static capu::uint_t Digest(char* key, const capu::uint8_t bitSize) {
    return capu::CapuDefaultHashFunction::Digest(key, bitSize);
  }

  static capu::uint_t Digest(const char* key, const capu::uint8_t bitSize) {
    return capu::CapuDefaultHashFunction::Digest(key, bitSize);
  }

};

#endif

