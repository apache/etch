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

#ifndef __STRINGUTILS_H__
#define __STRINGUTILS_H__

#include "capu/Config.h"
#include "capu/Error.h"

#define STRINGUTILS_INC_HEADER
#include "arch/StringUtils.inc"
#undef STRINGUTILS_INC_HEADER

namespace capu
{
  class StringUtils
  {
#define STRINGUTILS_INC_MEMBER
#include "arch/StringUtils.inc"
#undef STRINGUTILS_INC_MEMBER

  public:
    /**
    * Static method to copy a String of length dstSize from src to dst
    * @param dst destination buffer
    * @param dstSize number of chars to be copied
    * @param src source buffer
    */
    inline static void Strncpy(char* dst, uint32_t dstSize, const char* src);

    /**
    * Static method to write a C string according to the given format into the array pointed by buffer.
    * After the format parameter, the function expects at least as many additional arguments as specified in format.
    * @param buffer which contains the string
    * @param bufferSize size of the buffer
    * @param format the format of the string
    * @param arguments 
    */
    inline static void Sprintf(char* buffer, uint32_t bufferSize, const char* format, ...);
    
    /**
    * Static method to write a C string according to the given format into the array pointed by buffer.
    * The arguments specified in the format have to be passed by the values parameter.
    * @param buffer which contains the string
    * @param bufferSize size of the buffer
    * @param format the format of the string
    * @param values arguments for the format 
    */
    inline static void Vsprintf(char* buffer, uint32_t bufferSize, const char* format, va_list values);

    /**
    * Static method to return the length of the given String
    * @param str the string
    * @return length of the string
    */
    inline static uint32_t Strlen(const char* str); 

    /**
    * Static method to compare two C strings.
    * @param str1 first string
    * @param str2 second string
    * @return 0 if both strings are equal
    *         > 0 if the first character which does not match has a greater value in str1
    *         < 0 otherwise
    */
    inline static int32_t Strcmp(const char* str1, const char* str2);

  };

#define STRINGUTILS_INC_IMPL
#include "arch/StringUtils.inc"
#undef STRINGUTILS_INC_IMPL

}
#endif

