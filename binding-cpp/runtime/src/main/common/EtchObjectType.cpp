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
#include "common/EtchObjectType.h"
#include "capu/container/Hash.h"

const EtchObjectType& EtchObjectType::NATIVE_INT8() {
  static const EtchObjectType type(EOTID_BYTE, NULL);
  return type;
}
const EtchObjectType& EtchObjectType::NATIVE_INT16() {
  static const EtchObjectType type(EOTID_SHORT, NULL);
  return type;
}
const EtchObjectType& EtchObjectType::NATIVE_INT32() {
  static const EtchObjectType type(EOTID_INT32, NULL);
  return type;
}
const EtchObjectType& EtchObjectType::NATIVE_INT64() {
  static const EtchObjectType type(EOTID_LONG, NULL);
  return type;
}
const EtchObjectType& EtchObjectType::NATIVE_DOUBLE() {
  static const EtchObjectType type(EOTID_DOUBLE, NULL);
  return type;
}
const EtchObjectType& EtchObjectType::NATIVE_FLOAT() {
  static const EtchObjectType type(EOTID_FLOAT, NULL);
  return type;
}
const EtchObjectType& EtchObjectType::NATIVE_BOOL() {
  static const EtchObjectType type(EOTID_BOOL, NULL);
  return type;
}

const EtchObjectType* EtchObjectType::TYPE() {
  const static EtchObjectType TYPE(EOTID_OBJECT_TYPE, NULL);
  return &TYPE;
}

EtchObjectType::EtchObjectType(capu::int32_t typeId, const EtchObjectType* componentType, const EtchObjectTypeTrait componentTypeTrait)
: mTypeId(typeId), mComponentType(componentType), mComponentTypeTrait(componentTypeTrait) {

}

const EtchObjectType* EtchObjectType::getObjectComponentType() const {
  return mComponentType;
}

EtchObjectType::EtchObjectTypeTrait EtchObjectType::getObjectComponentTypeTrait() const {
  return mComponentTypeTrait;
}

capu::bool_t EtchObjectType::isArray() const {
  return mTypeId == EOTID_NATIVE_ARRAY;
}

capu::int32_t EtchObjectType::getTypeId() const {
  return mTypeId;
}

capu::uint32_t EtchObjectType::getHashCode() const {
  return capu::CapuDefaultHashFunction::Digest(this);
}

capu::bool_t EtchObjectType::equals(const EtchObjectType* type) const {
  if (this->mTypeId == type->mTypeId) {
    if ((mComponentType == NULL) && (type->mComponentType == NULL)) {
      return true;
    } else if ((mComponentType != NULL) && (type->mComponentType != NULL)) {
      return mComponentType->equals(type->mComponentType);
    } else {
      return false;
    }
  } else {
    return false;
  }
}
