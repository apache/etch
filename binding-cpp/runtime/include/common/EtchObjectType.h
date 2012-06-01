/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef __ETCHOBJECTTYPE_H__
#define __ETCHOBJECTTYPE_H__
#include "EtchConfig.h"

enum EtchObjectTypeIds {

  EOTID_INT32 = 0,
  EOTID_BOOL,
  EOTID_BYTE,
  EOTID_SHORT,
  EOTID_LONG,
  EOTID_DOUBLE,
  EOTID_FLOAT,
  EOTID_STRING,
  EOTID_DATE,
  EOTID_LIST,
  EOTID_NATIVE_ARRAY,
  EOTID_HASHTABLE,
  EOTID_SET,
  EOTID_SOCKET,
  EOTID_SERVER_SOCKET,
  EOTID_VALIDATOR_BOOLEAN,
  EOTID_VALIDATOR_BYTE,
  EOTID_VALIDATOR_INT,
  EOTID_VALIDATOR_SHORT,
  EOTID_VALIDATOR_FLOAT,
  EOTID_VALIDATOR_DOUBLE,
  EOTID_VALIDATOR_LONG,
  EOTID_VALIDATOR_STRING,
  EOTID_VALIDATOR_COMBO,
  EOTID_VALIDATOR_OBJECT,
  EOTID_FIELD,
  EOTID_OBJECT,

  EOTID_NATIVE_INT8,
  EOTID_NATIVE_INT16,
  EOTID_NATIVE_INT32,
  EOTID_NATIVE_INT64,
  EOTID_NATIVE_DOUBLE,
  EOTID_NATIVE_FLOAT,
  EOTID_NATIVE_LONG,
  EOTID_NATIVE_SHORT,
  EOTID_NATIVE_BOOL,
  EOTID_NATIVE_BYTE,
};

class EtchObjectType {
public:
   /**
    * TypeIds for native types
    */
  static const EtchObjectType NATIVE_INT8;
  static const EtchObjectType NATIVE_INT16;
  static const EtchObjectType NATIVE_INT32;
  static const EtchObjectType NATIVE_INT64;
  static const EtchObjectType NATIVE_DOUBLE;
  static const EtchObjectType NATIVE_FLOAT;
  static const EtchObjectType NATIVE_BOOL;

  /**
   * Constructor
   */
  EtchObjectType(capu::int32_t typeId, const EtchObjectType* componentTypes);

  /**
   * to get the type ids
   * @return
   */
  template<class T>
  static const EtchObjectType * getType() {
    return __Wrapper__<T>::getType();
  }

  /**
   * get object type
   * @return object type
   */
  capu::int32_t getTypeId() const;

  /**
   * check if the given object is an array or not
   * @return true if it is array
   *         false otherwise
   */
  capu::bool_t isArray() const;

  /**
   * @return NULL if this is not a collection type
   *         otherwise component type of collection such as NativeArray
   */
  const EtchObjectType* getObjectComponentType() const;

  /**
   * @return true if two type is equal
   *         false otherwise
   */
  capu::bool_t equals(const EtchObjectType* type) const;

private:
  capu::int32_t mTypeId;
  const EtchObjectType* mComponentType;

  template <class T>
  struct __Wrapper__ {

    static const EtchObjectType * getType() {
      return &T::TYPE;
    }
  };

  template <class T>
  struct __Wrapper__<T*> {

    static const EtchObjectType * getType() {
      return &T::TYPE;
    }
  };

};

 template <>
 struct EtchObjectType::__Wrapper__<capu::int32_t> {

    static const EtchObjectType * getType() {
      return &EtchObjectType::NATIVE_INT32;
    }
  };

  template <>
  struct EtchObjectType::__Wrapper__<capu::int16_t> {

    static const EtchObjectType * getType() {
      return &EtchObjectType::NATIVE_INT16;
    }
  };

  template <>
  struct EtchObjectType::__Wrapper__<capu::int8_t> {

    static const EtchObjectType * getType() {
      return &EtchObjectType::NATIVE_INT8;
    }
  };

  template <>
  struct EtchObjectType::__Wrapper__<capu::int64_t> {

    static const EtchObjectType * getType() {
      return &EtchObjectType::NATIVE_INT64;
    }
  };

  template <>
  struct EtchObjectType::__Wrapper__<capu::float_t> {

    static const EtchObjectType * getType() {
      return &EtchObjectType::NATIVE_FLOAT;
    }
  };

  template <>
  struct EtchObjectType::__Wrapper__<capu::double_t> {

    static const EtchObjectType * getType() {
      return &EtchObjectType::NATIVE_DOUBLE;
    }
  };

  template <>
  struct EtchObjectType::__Wrapper__<capu::bool_t> {

    static const EtchObjectType * getType() {
      return &EtchObjectType::NATIVE_BOOL;
    }
  };

  template <>
  struct EtchObjectType::__Wrapper__<capu::int32_t*> {

    static const EtchObjectType * getType() {
      return &EtchObjectType::NATIVE_INT32;
    }
  };

  template <>
  struct EtchObjectType::__Wrapper__<capu::int16_t*> {

    static const EtchObjectType * getType() {
      return &EtchObjectType::NATIVE_INT16;
    }
  };

  template <>
  struct EtchObjectType::__Wrapper__<capu::int8_t*> {

    static const EtchObjectType * getType() {
      return &EtchObjectType::NATIVE_INT8;
    }
  };

  template <>
  struct EtchObjectType::__Wrapper__<capu::int64_t*> {

    static const EtchObjectType * getType() {
      return &EtchObjectType::NATIVE_INT64;
    }
  };

  template <>
  struct EtchObjectType::__Wrapper__<capu::float_t*> {

    static const EtchObjectType * getType() {
      return &EtchObjectType::NATIVE_FLOAT;
    }
  };

  template <>
  struct EtchObjectType::__Wrapper__<capu::double_t*> {

    static const EtchObjectType * getType() {
      return &EtchObjectType::NATIVE_DOUBLE;
    }
  };

  template <>
  struct EtchObjectType::__Wrapper__<capu::bool_t*> {

    static const EtchObjectType * getType() {
      return &EtchObjectType::NATIVE_BOOL;
    }
  };

#endif /* ETCHOBJECTTYPE_H */

