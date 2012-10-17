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

#include "common/EtchLong.h"

const EtchObjectType* EtchLong::TYPE() {
  const static EtchObjectType TYPE(EOTID_LONG, NULL);
  return &TYPE;
}

EtchLong::EtchLong()
 : mValue(0) {
  addObjectType(TYPE());
}

EtchLong::EtchLong(capu::int64_t value)
 : mValue(value) {
  addObjectType(TYPE());
}

EtchLong::EtchLong(const EtchLong& other)
 : mValue(other.mValue) {
  addObjectType(TYPE());
}

void EtchLong::set(capu::int64_t value) {
  mValue = value;
}

capu::int64_t EtchLong::get() {
  return mValue;
}

capu::uint32_t EtchLong::getHashCode() const {
  //first 32 bit xor second 32 bit of long
  return static_cast<capu::uint32_t>(mValue ^ mValue >> 32);
}

capu::bool_t EtchLong::equals(const EtchObject * other) const {
  if (other == NULL)
    return false;
  else if (!other->getObjectType()->equals(EtchLong::TYPE()))
    return false;
  EtchLong * a = (EtchLong*) other;
  return (a->mValue == this->mValue);
}

capu::int64_t EtchLong::getLongValue() const {
  return mValue;
}

capu::int32_t EtchLong::getInt32Value() const {
  return static_cast<capu::int32_t>(mValue);
}

capu::int16_t EtchLong::getShortValue() const {
  return static_cast<capu::int16_t>(mValue);
}

capu::int8_t EtchLong::getByteValue() const {
  return static_cast<capu::int8_t>(mValue);
}
