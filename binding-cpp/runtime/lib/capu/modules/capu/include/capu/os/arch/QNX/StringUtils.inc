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

#ifdef STRINGUTILS_INC_HEADER
#include "capu/Config.h"
#include <string.h>
#include <stdarg.h>
#endif

#ifdef STRINGUTILS_INC_MEMBER
private:
#endif

#ifdef STRINGUTILS_INC_IMPL

  inline void StringUtils::Strncpy(char* dst, uint_t dstSize, const char* src)
  {
    strncpy(dst, src, dstSize);
  }

  inline uint_t StringUtils::Strlen(const char* str)
  {
    return strlen(str);
  }

  inline int_t StringUtils::Strcmp(const char* str1, const char* str2)
  {
    return strcmp(str1, str2);
  }

  inline void StringUtils::Vsprintf(char* buffer, uint_t bufferSize,const char* format, va_list values)
  {
    vsnprintf(buffer, bufferSize, format, values);
  }
  
  inline void StringUtils::Sprintf(char* buffer, uint_t bufferSize, const char* format, ...) 
  {
    va_list argptr;
    va_start(argptr,format);
    StringUtils::Vsprintf(buffer, bufferSize, format, argptr);
    va_end(argptr);
  }


#endif
