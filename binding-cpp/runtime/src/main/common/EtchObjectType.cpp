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
#include "common/EtchObject.h"

const EtchObjectType EtchObjectType::NATIVE_INT8(EOTID_BYTE, NULL);
const EtchObjectType EtchObjectType::NATIVE_INT16(EOTID_SHORT, NULL);
const EtchObjectType EtchObjectType::NATIVE_INT32(EOTID_INT32, NULL);
const EtchObjectType EtchObjectType::NATIVE_INT64(EOTID_LONG, NULL);
const EtchObjectType EtchObjectType::NATIVE_DOUBLE(EOTID_DOUBLE, NULL);
const EtchObjectType EtchObjectType::NATIVE_FLOAT(EOTID_FLOAT, NULL);
const EtchObjectType EtchObjectType::NATIVE_BOOL(EOTID_BOOL, NULL);

EtchObjectType::EtchObjectType(capu::int32_t typeId, const EtchObjectType* componentType)
: mTypeId(typeId), mComponentType(componentType) {

}

const EtchObjectType* EtchObjectType::getObjectComponentType() const {
  return mComponentType;
}

capu::bool_t EtchObjectType::isArray() const {
  return mTypeId == EOTID_NATIVE_ARRAY;
}

capu::int32_t EtchObjectType::getTypeId() const {
  return mTypeId;
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