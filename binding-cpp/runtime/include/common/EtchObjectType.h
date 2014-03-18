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
#include "capu/util/SmartPointer.h"

enum EtchObjectTypeIds {
  EOTID_OBJECT = 0,
  EOTID_NUMBER,
  EOTID_INT32,
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
  EOTID_VALIDATOR_RUNTIME_EXCEPTION,
  EOTID_VALIDATOR_STRUCT_VALUE,
  EOTID_VALIDATOR_NONE,
  EOTID_FIELD,
  EOTID_OBJECT_TYPE,
  EOTID_VALUE_FACTORY,
  EOTID_TYPE,
  EOTID_STRUCT_VALUE,
  EOTID_EXCEPTION,
  EOTID_AUTH_EXCEPTION,
  EOTID_RUNTIME_EXCEPTION,
  EOTID_ARRAY_VALUE,
  EOTID_POOL,
  EOTID_FREEPOOL,
  EOTID_QUEUEDPOOL,
  EOTID_WAIT_UP,
  EOTID_WAIT_DOWN,
  EOTID_STUBHELPER,
  EOTID_VALIDATOR_CUSTOM,
  EOTID_VALIDATOR_CUSTOM_KEY,
  EOTID_STACK,
  EOTID_STACK_CLIENT,
  EOTID_STACK_SERVER,
  EOTID_DYNAMIC_START
};

class EtchObjectType {
public:
  /**
   * TypeIds for native types
   */
  static const EtchObjectType& NATIVE_INT8();
  static const EtchObjectType& NATIVE_INT16();
  static const EtchObjectType& NATIVE_INT32();
  static const EtchObjectType& NATIVE_INT64();
  static const EtchObjectType& NATIVE_DOUBLE();
  static const EtchObjectType& NATIVE_FLOAT();
  static const EtchObjectType& NATIVE_BOOL();

  /**
   * TypeTraits
   */
  enum EtchObjectTypeTrait {NONE, VALUE, POINTER, SMART_POINTER};

  /**
   * EtchObjectType.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructor
   */
  EtchObjectType(capu::int32_t typeId, const EtchObjectType* componentType, const EtchObjectTypeTrait componentTypeTrait = NONE);

  /**
   * to get the type ids
   * @return
   */
  template<class T>
  static const EtchObjectType* getType() {
    return __Wrapper__<T>::getType();
  }

  /**
   * to get the type trait
   * @return
   */
  template<class T>
  static EtchObjectTypeTrait getTypeTrait() {
    return __Wrapper__<T>::getTypeTrait();
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
     * returns the hash code of the object
     * @return hash code
     */
  capu::uint32_t getHashCode() const;

  /**
   * @return NULL if this is not a collection type
   *         otherwise component type of collection such as NativeArray
   */
  const EtchObjectType* getObjectComponentType() const;

  /**
   * @return NONE if this is not a collection type trait
   *         otherwise component type trait of collection such VALUE, POINTER, SMART_POINTER
   */
  EtchObjectTypeTrait getObjectComponentTypeTrait() const;

  /**
   * @return true if two type is equal
   *         false otherwise
   */
  capu::bool_t equals(const EtchObjectType* type) const;

private:
  capu::int32_t mTypeId;
  const EtchObjectType* mComponentType;
  const EtchObjectTypeTrait mComponentTypeTrait;


  template <class T>
  struct __Wrapper__ {

    static const EtchObjectType* getType() {
      return T::TYPE();
    }

    static EtchObjectTypeTrait getTypeTrait() {
      return EtchObjectType::VALUE;
    }

  };

  template <class T>
  struct __Wrapper__<T*> {

    static const EtchObjectType* getType() {
      return T::TYPE();
    }

    static EtchObjectTypeTrait getTypeTrait() {
      return EtchObjectType::POINTER;
    }
  };

  template <class T>
  struct __Wrapper__<capu::SmartPointer<T> > {

    static const EtchObjectType * getType() {
      return EtchObjectType::getType<T>();
    }

    static EtchObjectTypeTrait getTypeTrait() {
      return EtchObjectType::SMART_POINTER;
    }
  };

};

template <>
struct EtchObjectType::__Wrapper__<capu::int32_t> {

  static const EtchObjectType* getType() {
    return &EtchObjectType::NATIVE_INT32();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::VALUE;
  }
};

template <>
struct EtchObjectType::__Wrapper__<capu::int16_t> {

  static const EtchObjectType* getType() {
    return &EtchObjectType::NATIVE_INT16();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::VALUE;
  }

};

template <>
struct EtchObjectType::__Wrapper__<capu::int8_t> {

  static const EtchObjectType* getType() {
    return &EtchObjectType::NATIVE_INT8();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::VALUE;
  }
};

template <>
struct EtchObjectType::__Wrapper__<capu::int64_t> {

  static const EtchObjectType* getType() {
    return &EtchObjectType::NATIVE_INT64();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::VALUE;
  }
};

template <>
struct EtchObjectType::__Wrapper__<capu::float_t> {

  static const EtchObjectType* getType() {
    return &EtchObjectType::NATIVE_FLOAT();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::VALUE;
  }

};

template <>
struct EtchObjectType::__Wrapper__<capu::double_t> {

  static const EtchObjectType* getType() {
    return &EtchObjectType::NATIVE_DOUBLE();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::VALUE;
  }

};

template <>
struct EtchObjectType::__Wrapper__<capu::bool_t> {

  static const EtchObjectType* getType() {
    return &EtchObjectType::NATIVE_BOOL();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::VALUE;
  }

};

template <>
struct EtchObjectType::__Wrapper__<capu::int32_t*> {

  static const EtchObjectType* getType() {
    return &EtchObjectType::NATIVE_INT32();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::POINTER;
  }

};

template <>
struct EtchObjectType::__Wrapper__<capu::int16_t*> {

  static const EtchObjectType* getType() {
    return &EtchObjectType::NATIVE_INT16();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::POINTER;
  }

};

template <>
struct EtchObjectType::__Wrapper__<capu::int8_t*> {

  static const EtchObjectType* getType() {
    return &EtchObjectType::NATIVE_INT8();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::POINTER;
  }

};

template <>
struct EtchObjectType::__Wrapper__<capu::int64_t*> {

  static const EtchObjectType* getType() {
    return &EtchObjectType::NATIVE_INT64();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::POINTER;
  }

};

template <>
struct EtchObjectType::__Wrapper__<capu::float_t*> {

  static const EtchObjectType* getType() {
    return &EtchObjectType::NATIVE_FLOAT();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::POINTER;
  }

};

template <>
struct EtchObjectType::__Wrapper__<capu::double_t*> {

  static const EtchObjectType* getType() {
    return &EtchObjectType::NATIVE_DOUBLE();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::POINTER;
  }

};

template <>
struct EtchObjectType::__Wrapper__<capu::bool_t*> {

  static const EtchObjectType* getType() {
    return &EtchObjectType::NATIVE_BOOL();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::POINTER;
  }

};

template <>
struct EtchObjectType::__Wrapper__<capu::SmartPointer<capu::int32_t> > {

  static const EtchObjectType * getType() {
    return &EtchObjectType::NATIVE_INT32();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::SMART_POINTER;
  }
};

template <>
struct EtchObjectType::__Wrapper__<capu::SmartPointer<capu::int16_t> > {

  static const EtchObjectType * getType() {
    return &EtchObjectType::NATIVE_INT16();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::SMART_POINTER;
  }

};

template <>
struct EtchObjectType::__Wrapper__<capu::SmartPointer<capu::int8_t> > {

  static const EtchObjectType * getType() {
    return &EtchObjectType::NATIVE_INT8();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::SMART_POINTER;
  }
};

template <>
struct EtchObjectType::__Wrapper__<capu::SmartPointer<capu::int64_t> > {

  static const EtchObjectType * getType() {
    return &EtchObjectType::NATIVE_INT64();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::SMART_POINTER;
  }
};

template <>
struct EtchObjectType::__Wrapper__<capu::SmartPointer<capu::float_t> > {

  static const EtchObjectType * getType() {
    return &EtchObjectType::NATIVE_FLOAT();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::SMART_POINTER;
  }

};

template <>
struct EtchObjectType::__Wrapper__<capu::SmartPointer<capu::double_t> > {

  static const EtchObjectType * getType() {
    return &EtchObjectType::NATIVE_DOUBLE();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::SMART_POINTER;
  }

};

template <>
struct EtchObjectType::__Wrapper__<capu::SmartPointer<capu::bool_t> > {

  static const EtchObjectType * getType() {
    return &EtchObjectType::NATIVE_BOOL();
  }

  static EtchObjectTypeTrait getTypeTrait() {
    return EtchObjectType::SMART_POINTER;
  }

};

#endif /* ETCHOBJECTTYPE_H */

