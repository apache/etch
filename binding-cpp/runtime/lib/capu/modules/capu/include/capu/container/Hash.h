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


#ifndef __HASH_H__
#define __HASH_H__

#include "capu/Config.h"

namespace capu {

  class Hash {
  public:

    template <typename T>
    static uint64_t Digest(const T &key) {
      uint64_t result = 0;
      for (uint64_t i = 0; i < 10; ++i) {
        result = (result + static_cast<uint64_t> (key) * 13);
      }
      return static_cast<uint64_t> (result);
    }

    static uint64_t Digest(char* key) {
      uint64_t result = 0;
      char * keyStart = key;
      char * keyEnd = key + strlen(key);
      while (keyStart != keyEnd) {
        result = (result + static_cast<uint64_t> (*keyStart) * 13);
        ++keyStart;
      }
      return result;
    }

    static uint64_t Digest(const char* key) {
      uint64_t result = 0;
      const char * keyStart = key;
      const char * keyEnd = key + strlen(key);
      while (keyStart != keyEnd) {
        result = (result + static_cast<uint64_t> (*keyStart) * 13);
        ++keyStart;
      }
      return result;
    }

    

  };
}
#endif /* HASH_H */

