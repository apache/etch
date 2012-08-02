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

#ifndef __NUMERIC_LIMITS_H__
#define __NUMERIC_LIMITS_H__

#include "capu/Config.h"

#define NUMERIC_LIMITS_INC_HEADER
#include "arch/NumericLimits.inc"
#undef NUMERIC_LIMITS_INC_HEADER

namespace capu {

  template<typename T>
  inline T NumericLimitMax() {
    //not important
    return 0;
  }

  template<typename T>
  inline T NumericLimitMin() {
    //not important
    return 0;
  }

  template<>
  inline capu::int32_t NumericLimitMax<capu::int32_t>() {
    return 0x7fffffff;
  }

  template<>
  inline capu::int32_t NumericLimitMin<capu::int32_t>() {
    return 0x80000000;
  }

  template<>
  inline capu::uint32_t NumericLimitMax<capu::uint32_t>() {
    return 0xFFFFFFFF;
  }

  template<>
  inline capu::uint32_t NumericLimitMin<capu::uint32_t>() {
    return 0x0;
  }

  template<>
  inline capu::int16_t NumericLimitMin<capu::int16_t>() {
    return -32768;
  }

  template<>
  inline capu::int16_t NumericLimitMax<capu::int16_t>() {
    return 32767;
  }
  
  template<>
  inline capu::int8_t NumericLimitMin<capu::int8_t>() {
    return -128;
  }

  template<>
  inline capu::int8_t NumericLimitMax<capu::int8_t>() {
    return 127;
  }
  
  template<>
  inline capu::float_t NumericLimitMin<capu::float_t>() {
    return FLT_MIN;
  }

  template<>
  inline capu::float_t NumericLimitMax<capu::float_t>() {
    return FLT_MAX;
  }
}

#endif

