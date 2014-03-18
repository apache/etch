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

#include "common/EtchDate.h"

const EtchObjectType* EtchDate::TYPE() {
  const static EtchObjectType TYPE(EOTID_DATE, NULL);
  return &TYPE;
}

EtchDate::EtchDate()
: mValue(0) {
  addObjectType(TYPE());
}

EtchDate::EtchDate(capu::int64_t value)
 : mValue(value) {
  addObjectType(TYPE());
}


EtchDate::EtchDate(const EtchDate& other)
 : EtchObject(other), mValue(other.mValue) {
}

EtchDate::~EtchDate() {

}

capu::bool_t EtchDate::equals(const EtchObject* other) const{
  if (other == NULL) {
    return false;
  } else if (other->getObjectType() != EtchDate::TYPE()) {
    return false;
  }
  EtchDate *tmp = (EtchDate *) other;
  return tmp->mValue == this->mValue;
}

capu::int64_t EtchDate::get() {
  return mValue;
}

void EtchDate::set(capu::int64_t value) {
  mValue = value;
}

capu::uint32_t EtchDate::getHashCode() const{
  return static_cast<capu::uint32_t>(mValue ^ mValue >> 32);
}
