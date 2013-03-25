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

#include "serialization/EtchStructValue.h"

const EtchObjectType* EtchStructValue::TYPE() {
  const static EtchObjectType TYPE(EOTID_STRUCT_VALUE, NULL);
  return &TYPE;
}

EtchStructValue::EtchStructValue(EtchType* type, EtchValueFactory* vf)
 : mType(type), mLevel(vf->getLevel()), mTable(DEFAULT_SIZE) {
  addObjectType(TYPE());
}

EtchStructValue::EtchStructValue(EtchType* type, EtchValueFactory* vf, capu::uint32_t length)
 : mType(type), mLevel(vf->getLevel()), mTable(length > 0 ? (length * 4 + 2) / 3 : DEFAULT_SIZE) {
  addObjectType(TYPE());
}

EtchStructValue::EtchStructValue(const EtchStructValue& other)
 : EtchObject(other), mType(other.mType), mLevel(other.mLevel), mTable(other.mTable) {
}

EtchStructValue::~EtchStructValue() {

}

status_t EtchStructValue::put(const EtchField &field, capu::SmartPointer<EtchObject> object) {
  if (object.get() == NULL) {
    capu::SmartPointer<EtchObject> tmp;
    status_t result = remove(field, &tmp);
    return result;
  }
  if (mLevel != LEVEL_NONE) {
    capu::SmartPointer<EtchValidator> v;
    if ((mType->getValidator(field, v) != ETCH_OK) && (mLevel == LEVEL_FULL))
      return ETCH_ENOT_EXIST;

    if ((mLevel == LEVEL_FULL) && (v.get() == NULL))
      return ETCH_ENOT_EXIST;

    if ((v.get() != NULL) && (!v->validate(object)))
      return ETCH_EINVAL;
  }
  return mTable.put(field, object);
}

EtchType* EtchStructValue::getType() {
  return mType;
}

EtchLevel EtchStructValue::getLevel() {
  return mLevel;
}

capu::bool_t EtchStructValue::isType(EtchType* otherType) {
  return mType->equals(otherType);
}

status_t EtchStructValue::remove(const EtchField &key, capu::SmartPointer<EtchObject> *value_old) {
  return mTable.remove(key, value_old);
}

capu::uint32_t EtchStructValue::count() {
  return mTable.count();
}

status_t EtchStructValue::clear() {
  mTable.clear();
  return ETCH_OK;
}

status_t EtchStructValue::get(const EtchField &key, capu::SmartPointer<EtchObject>  *value) {
  return mTable.get(key, value);
}

EtchStructValue::Iterator EtchStructValue::begin() const
{
  return mTable.begin();
}

capu::bool_t EtchStructValue::isEmpty()
{
  if(mTable.count() == 0) {
    return true;
  }
  return false;
}
