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

#include "serialization/EtchField.h"
#include "util/EtchHash.h"
#include "common/EtchObjectHash.h"

const EtchObjectType* EtchField::TYPE() {
  const static EtchObjectType TYPE(EOTID_FIELD, NULL);
  return &TYPE;
}

EtchField::EtchField(capu::uint32_t id, EtchString& name)
 : mId(id), mName(name) {
  addObjectType(EtchField::TYPE());
}

EtchField::EtchField(const EtchString& name)
 : mId(0), mName(name) {
  addObjectType(EtchField::TYPE());
  mId = EtchHashEx::Digest(name);
}

EtchField::EtchField()
 : mId(0), mName("") {
  addObjectType(EtchField::TYPE());
}

EtchField::~EtchField() {
}

EtchField::EtchField(const EtchField& other)
 : EtchObject(other), mId(other.mId), mName(other.mName) {
}

capu::uint32_t EtchField::getHashCode() const{
  return mId.getHashCode() ^ mName.getHashCode();
}

capu::bool_t EtchField::equals(const EtchObject * object) const{
  if (object == NULL)
    return false;
  else if (object->getObjectType() != EtchField::TYPE())
    return false;
  EtchField * a = (EtchField*) object;
  return ((a->mId.equals(&(this->mId))) && (a->mName.equals(&this->mName)));
}

capu::uint32_t EtchField::getId() const {
  return mId.get();
}

EtchString EtchField::getName() const {
  return mName;
}
