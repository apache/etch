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

const EtchObjectType EtchStructValue::TYPE(EOTID_STRUCT_VALUE, NULL);

EtchStructValue::EtchStructValue(EtchType* type, EtchValueFactory* vf)
: EtchObject(&EtchStructValue::TYPE), mType(type), mLevel(vf->getLevel()), mTable(DEFAULT_SIZE) {

}

EtchStructValue::EtchStructValue(EtchType* type, EtchValueFactory* vf, capu::uint32_t length)
: EtchObject(&EtchStructValue::TYPE), mType(type), mLevel(vf->getLevel()), mTable(length > 0 ? (length * 4 + 2) / 3 : DEFAULT_SIZE) {

}

EtchStructValue::~EtchStructValue() {

}

status_t EtchStructValue::put(EtchField &field, capu::SmartPointer<EtchObject> object, capu::SmartPointer<EtchObject> *old_value) {
  if (object.get() == NULL) {
    capu::SmartPointer<EtchObject> tmp;
    status_t result = remove(field, &tmp);
    if (old_value != NULL)
      *old_value = tmp;
    return result;
  }
  if (mLevel != LEVEL_NONE) {
    capu::SmartPointer<EtchValidator> v;
    if ((mType->getValidator(field, v) != ETCH_OK) && (mLevel == LEVEL_FULL))
      return ETCH_ENOT_EXIST;

    if ((mLevel == LEVEL_FULL) && (v.get() == NULL))
      return ETCH_ENOT_EXIST;

    if ((v.get() != NULL) && (!v->validate(object.get())))
      return ETCH_EINVAL;
  }
  return mTable.put(field, object, old_value);
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

status_t EtchStructValue::remove(EtchField &key, capu::SmartPointer<EtchObject> *value_old) {
  return mTable.remove(key, value_old);
}

capu::uint64_t EtchStructValue::count() {
  return mTable.count();
}

status_t EtchStructValue::clear() {
  EtchHashTable<EtchField, capu::SmartPointer<EtchObject> >::Iterator it = mTable.begin();
  EtchHashTable<EtchField, capu::SmartPointer<EtchObject> >::Pair pair;
  while (it.hasNext()) {
    it.next(&pair);
    pair.second = NULL;
  }
  return ETCH_OK;
}

status_t EtchStructValue::get(EtchField &key, capu::SmartPointer<EtchObject>  *value) {
  return mTable.get(key, value);
}

EtchStructValue::Iterator EtchStructValue::begin()
{
  return mTable.begin();
}