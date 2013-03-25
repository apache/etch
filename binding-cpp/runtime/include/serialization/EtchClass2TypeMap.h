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


#ifndef __ETCHCLASS2TYPEMAP_H__
#define __ETCHCLASS2TYPEMAP_H__

#include "common/EtchInt32.h"
#include "common/EtchHashNative.h"
#include "serialization/EtchType.h"

/**
 * Mapping between Class and Type.
 */
class EtchClass2TypeMap {
public:

  /**
   * Constructor
   */
  EtchClass2TypeMap();

  /**
   * Destructor
   */
  virtual ~EtchClass2TypeMap();

  /**
   * @param classType
   * @param result
   * @return  ETCH_OK if get is successful performed
   *          ETCH_EINVAL if field is null
   *          ETCH_ENOT_EXIST if there is no pair with specified id
   */
  status_t get(const EtchObjectType* classType, EtchType **result);

  /**
   * Adds a map entry from classType to type.
   * @param classType
   * @param type
   * @return ETCH_EINVAL if it is locked
   *         ETCH_ERANGE  if there is duplicate element on the collection
   *         ETCH_OK if successfully added
   *         ETCH_ERROR otherwise
   */
  status_t put(const EtchObjectType* classType, EtchType *type);

  /**
   * Locks the map, disallowing any more changes.
   */
  void lock();

  /**
   * Hashing Functions for the EtchClass2TypeMap
   */
  class Hash {
  public:
  static capu::uint32_t Digest(const EtchObjectType &key) {
    return key.getHashCode();
    }

    static capu::uint32_t Digest(const EtchObjectType* key) {
    return key->getHashCode();
    }

    static capu::uint32_t Digest(const capu::SmartPointer<EtchObjectType>& key) {
    return key->getHashCode();
    }
  };

  /**
   * Comperator Functions for the EtchClass2TypeMap
   */
  template <class T>
  class Comparator {
  public:
    inline capu::bool_t operator() (const EtchObjectType &first, const EtchObjectType &second) const {
    return first.equals(&second);
    }
  };
  template <class T>
  class Comparator <T*> {
  public:
    inline capu::bool_t operator() (const EtchObjectType* first, const EtchObjectType* second) const {
      return first->equals(second);
    }
  };
  template <class T>
  class Comparator <capu::SmartPointer<T> > {
  public:
    inline capu::bool_t operator() (const capu::SmartPointer<EtchObjectType>& first, const capu::SmartPointer<EtchObjectType>& second) const {
      return first->equals(second.get());
    }
  };
private:
  EtchHashTable<const EtchObjectType*, EtchType*, EtchClass2TypeMap::Comparator<const EtchObjectType*>,EtchHashNative > mC2T;

  capu::bool_t mLocked;

};

#endif /* __ETCHCLASS2TYPEMAP_H__ */

