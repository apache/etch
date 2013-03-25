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

#include "common/EtchHashTable.h"
#include "common/EtchConfig.h"
#include "serialization/EtchType.h"
#include "serialization/EtchClass2TypeMap.h"

EtchClass2TypeMap::EtchClass2TypeMap()
: mC2T(ETCH_DEFAULT_C2TYPEMAP_HASH_BIT_SIZE), mLocked(false) {
}

EtchClass2TypeMap::~EtchClass2TypeMap() {
}

status_t EtchClass2TypeMap::get(const EtchObjectType* classType, EtchType **result) {
  return mC2T.get(classType, result);
}

status_t EtchClass2TypeMap::put(const EtchObjectType* classType, EtchType *type) {

  if (mLocked) {
    return ETCH_EINVAL;
  }

  EtchType* tmp = NULL;
  if (mC2T.get(classType, &tmp) == ETCH_OK) {
    return ETCH_OK;
  }

  return mC2T.put(classType, type);
}

void EtchClass2TypeMap::lock() {
  mLocked = true;
}
