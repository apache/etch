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

#include "common/EtchConfig.h"
#include "serialization/EtchTypeMap.h"

// TODO: Check memory handling

EtchTypeMap::EtchTypeMap()
: mById(ETCH_DEFAULT_TYPEMAP_HASH_BIT_SIZE), mByName(ETCH_DEFAULT_TYPEMAP_HASH_BIT_SIZE), mLocked(false) {
}

EtchTypeMap::~EtchTypeMap() {
  EtchHashTable<capu::int32_t, EtchType*, EtchComparatorNative, EtchHashNative>::Iterator it = mById.begin();
  while (it.hasNext()) {
    EtchHashTable<capu::int32_t, EtchType*, EtchComparatorNative, EtchHashNative>::HashTableEntry entry;
    it.next(&entry);
    if (entry.value != NULL) {
      delete entry.value;
    }
  }
  mById.clear();
  mByName.clear();
}

status_t EtchTypeMap::add(EtchType *type) {
  if (mLocked)
    return ETCH_EINVAL;
  EtchType *tmp = NULL;
  // check id collision
  if (mById.get(type->getId(), &tmp) != ETCH_ENOT_EXIST)
    return ETCH_ERROR;
  // check name collision
  if (mByName.get(type->getName(), &tmp) != ETCH_ENOT_EXIST)
    return ETCH_ERROR;

  if (mById.put(type->getId(), type) != ETCH_OK)
    return ETCH_ERROR;
  if (mByName.put(type->getName(), type) != ETCH_OK)
    return ETCH_ERROR;

  return ETCH_OK;
}

void EtchTypeMap::lock() {
  mLocked = true;
};

capu::int32_t EtchTypeMap::size() {
  return mById.count();
}

status_t EtchTypeMap::get(EtchString name, EtchType *&type) {
  status_t result = mByName.get(name, &type);
  if (result == ETCH_ENOT_EXIST) {
    if (mLocked)
      return ETCH_EINVAL;
    EtchType *tmp = new EtchType(name);
    result = add(tmp);
    if (result == ETCH_OK) {
      type = tmp;
    } else {
      delete tmp;
    }
  }
  return result;
}

status_t EtchTypeMap::get(capu::int32_t id, EtchType *&type) {
  return mById.get(id, &type);
}

status_t EtchTypeMap::getAll(EtchHashSet<EtchType*>* set) {
  if (set == NULL)
    return ETCH_EINVAL;
  EtchHashTable<EtchString, EtchType* >::Iterator it = mByName.begin();
  EtchHashTable<EtchString, EtchType* >::HashTableEntry entry;
  while (it.hasNext()) {
    it.next(&entry);
    set->put(entry.value);
  }
  return ETCH_OK;
}

void EtchTypeMap::clear() {
  EtchHashTable<EtchString, EtchType* >::Iterator it = mByName.begin();
  EtchHashTable<EtchString, EtchType* >::HashTableEntry entry;
  while (it.hasNext()) {
    it.next(&entry);
    delete entry.value;
  }
  mByName.clear();
  mById.clear();
}
