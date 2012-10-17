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

#include "common/EtchShort.h"

const EtchObjectType* EtchShort::TYPE() {
  const static EtchObjectType TYPE(EOTID_SHORT, NULL);
  return &TYPE;
}

EtchShort::EtchShort()
 : mValue(0) {
  addObjectType(TYPE());
}

EtchShort::EtchShort(capu::int16_t value)
 : mValue(value) {
  addObjectType(TYPE());
}

EtchShort::EtchShort(const EtchShort& other)
 : mValue(other.mValue) {
  addObjectType(TYPE());
}

void EtchShort::set(capu::int16_t value) {
  mValue = value;
}

capu::int16_t EtchShort::get() {
  return mValue;
}

capu::uint32_t EtchShort::getHashCode() const {
  return static_cast<capu::uint32_t>(mValue);
}

capu::bool_t EtchShort::equals(const EtchObject * other) const {
  if (other == NULL)
    return false;
  else if (!other->getObjectType()->equals(EtchShort::TYPE()))
    return false;
  EtchShort * a = (EtchShort*) other;
  return (a->mValue == this->mValue);
}

capu::int64_t EtchShort::getLongValue() const {
  return static_cast<capu::int64_t>(mValue);
}

capu::int32_t EtchShort::getInt32Value() const {
  return static_cast<capu::int32_t>(mValue);
}

capu::int16_t EtchShort::getShortValue() const {
  return mValue;
}

capu::int8_t EtchShort::getByteValue() const {
  return static_cast<capu::int8_t>(mValue);
}
