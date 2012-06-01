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

#ifndef __ETCHHASH_H__
#define __ETCHHASH_H__
#include "common/EtchConfig.h"
#include "common/EtchString.h"

class EtchHashEx {
public:

  static capu::uint32_t Digest(const EtchString& value) {
    capu::uint32_t hash = 5381;
    capu::uint32_t n = value.length();
    const char *str = value.c_str();
    for (capu::uint32_t i = 0; i < n; i++) {
      char c = str[i];
      capu::uint32_t h6 = hash << 6;
      hash = (h6 << 10) + h6 - hash + c;
    }
    return hash;
  }
};

#endif