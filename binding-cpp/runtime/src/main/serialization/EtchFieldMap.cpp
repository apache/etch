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
#include "serialization/EtchFieldMap.h"

EtchFieldMap::EtchFieldMap()
: mById(ETCH_DEFAULT_FIELDMAP_HASH_BIT_SIZE), mByName(ETCH_DEFAULT_FIELDMAP_HASH_BIT_SIZE), mLocked(false) {

}

EtchFieldMap::~EtchFieldMap() {

}

status_t EtchFieldMap::add(const EtchField t) {
  if (mLocked)
    return ETCH_EINVAL;
  EtchField field;
  if (mById.get(t.getId(), &field) != ETCH_ENOT_EXIST)
    return ETCH_ERROR;
  if (mByName.get(t.getName(), &field) != ETCH_ENOT_EXIST)
    return ETCH_ERROR;

  if (mById.put(t.getId(), t) != ETCH_OK)
    return ETCH_ERROR;
  if (mByName.put(t.getName(), t) != ETCH_OK)
    return ETCH_ERROR;

  return ETCH_OK;
}

void EtchFieldMap::lock() {
  mLocked = true;
};

capu::int32_t EtchFieldMap::size() {
  return mById.count();
}

status_t EtchFieldMap::get(EtchString name, EtchField* field) {
  status_t result = mByName.get(name, field);
  if (result == ETCH_ENOT_EXIST) {
    if (mLocked)
      return ETCH_EINVAL;
    EtchField tmp(name);
    if (add(tmp) == ETCH_OK) {
      *field = tmp;
      result = ETCH_OK;
    }
  }
  return result;
}

status_t EtchFieldMap::get(capu::int32_t id, EtchField* field) {
  return mById.get(id, field);
}
