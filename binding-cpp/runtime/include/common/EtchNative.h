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

#ifndef __ETCHNATIVE_H__
#define __ETCHNATIVE_H__

#include "common/EtchObject.h"
#include "common/EtchError.h"

class EtchNative {
 public:

  /**
   * TypeIds for native types
   */
  static const int32_t INT8 = EOTID_NATIVE_INT8;
  static const int32_t INT16 = EOTID_NATIVE_INT16;
  static const int32_t INT32 = EOTID_NATIVE_INT32;
  static const int32_t INT64 = EOTID_NATIVE_INT64;
  static const int32_t DOUBLE = EOTID_NATIVE_DOUBLE;
  static const int32_t FLOAT = EOTID_NATIVE_FLOAT;
  static const int32_t LONG = EOTID_NATIVE_LONG;
  static const int32_t SHORT = EOTID_NATIVE_SHORT;
  static const int32_t BOOL = EOTID_NATIVE_BOOL;
  static const int32_t BYTE = EOTID_NATIVE_BYTE;
 
};

#endif
