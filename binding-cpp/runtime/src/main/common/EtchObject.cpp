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

#include "common/EtchObject.h"
#include "common/EtchObjectType.h"

const EtchObjectType* EtchObject::TYPE() {
  const static EtchObjectType TYPE(EOTID_OBJECT, NULL);
  return &TYPE;
}

EtchObject::EtchObject()
  : mType(TYPE()) {
  addObjectType(TYPE());
}

EtchObject::EtchObject(const EtchObjectType* type){
  mType = type;
}

EtchObject::~EtchObject() {
}

const EtchObjectType* EtchObject::getObjectType() const{
  return mType;
}

capu::bool_t EtchObject::isInstanceOf(const EtchObjectType* type) const {
  capu::List<const EtchObjectType*>::Iterator iter = mTypes.begin();
  while(iter.hasNext()) {
    const EtchObjectType* t = NULL;
    iter.next(&t);
    if(t->equals(type)) {
      return true;
    }
  }
  return false;
}

capu::uint32_t EtchObject::getHashCode() const{
  return (capu::uint32_t) this;
}

capu::bool_t EtchObject::equals(const EtchObject* other) const{
  return (other == this);
}

status_t EtchObject::addObjectType(const EtchObjectType* type) {
  mTypes.add(type);
  return ETCH_OK;
}

status_t EtchObject::setObjectType(const EtchObjectType* type) {
  mType = type;
  return ETCH_OK;
}

