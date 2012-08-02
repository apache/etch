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
#ifndef __MATH_H__
#define __MATH_H__

#include "capu/Config.h"

#define MATH_INC_HEADER
#include "arch/Math.inc"
#undef MATH_INC_HEADER

namespace capu {

  class Math {
  #define MATH_INC_MEMBER
  #include "arch/Math.inc"
  #undef MATH_INC_MEMBER

  public:
    static float_t Ceil(float_t val);
    static double_t Ceil(double_t val);
    static float_t Floor(float_t val);
    static double_t Floor(double_t val);
    static float_t Abs(float_t val);
    static double_t Abs(double_t val);
    static int_t Abs(int_t val);
    static float_t Sqrt(float_t val);
    static double_t Sqrt(double_t val);
    static float_t Pow2(float_t val);
    static double_t Pow2(double_t val);
    static float_t Pow(float_t val, float_t exponent);
    static double_t Por(double_t val, double_t exponent);
    static float_t Cos(float_t val);
    static double_t Cos(double_t val);
    static float_t Sin(float_t val);
    static double_t Sin(double_t val);
    static float_t Tan(float_t val);
    static double_t Tan(double_t val);
    static float_t ArcCos(float_t val);
    static double_t ArcCos(double_t val);
    static float_t ArcSin(float_t val);
    static double_t ArcSin(double_t val);
    static float_t ArcTan(float_t val);
    static double_t ArcTan(double_t val);
    static float_t Rad2Deg(float_t val);
    static double_t Rad2Deg(double_t val);
    static float_t Deg2Rad(float_t val);
    static double_t Deg2Rad(double_t val);
  };


#define MATH_INC_IMPL
#include "arch/Math.inc"
#undef MATH_INC_IMPL

}

#endif