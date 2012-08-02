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

  template<typename T>
  class HashCalculator;

  template<>
  class HashCalculator<uint32_t>
  {
  private:
    static const uint32_t prime = 16777619UL;
    static const uint32_t offset_base = 2166136261UL;
  public:
    static uint32_t Hash(void* key, uint32_t len)
    {
      uint32_t result = offset_base;
      uint8_t* ptr = static_cast<uint8_t*>(key);
      do
      {
        result = (result^*ptr) * prime;
        ++ptr;
      }while(--len);

      return result;
    }

    static uint32_t Hash(char* key)
    {
      uint32_t result = offset_base;
      do
      {
        result = (result^*key) * prime;
      }while(*(++key));

      return result;
    }

    static uint32_t Hash(const char* key)
    {
      uint32_t result = offset_base;
      do
      {
        result = (result^*key) * prime;
      }while(*(++key));

      return result;
    }

  };

  template<>
  class HashCalculator<uint64_t>
  {
  private:
    static const uint64_t prime = 1099511628211ULL;
    static const uint64_t offset_base = 14695981039346656037ULL;
  public:
    static uint64_t Hash(void* key, uint32_t len)
    {
      uint64_t result = offset_base;
      uint8_t* ptr = static_cast<uint8_t*>(key);
      do
      {
        result = (result^*ptr) * prime;
        ++ptr;
      }while(--len);

      return result;
    }  

    static uint64_t Hash(char* key)
    {
      uint64_t result = offset_base;
      do
      {
        result = (result^*key) * prime;
      }while(*(++key));

      return result;
    }

    static uint64_t Hash(const char* key)
    {
      uint64_t result = offset_base;
      do
      {
        result = (result^*key) * prime;
      }while(*(++key));

      return result;
    }
 
  };

  class Hash
  {
  public:

    static uint_t Digest(char* key)
    {
      return HashCalculator<uint_t>::Hash(key);
    }

    static uint_t Digest(const char* key)
    {
      return HashCalculator<uint_t>::Hash(key);
    }
    
    template<typename T>
    static uint_t Digest(T key)
    {
      return HashCalculator<uint_t>::Hash(&key, sizeof(T));
    }
  };

}
#endif /* HASH_H */

