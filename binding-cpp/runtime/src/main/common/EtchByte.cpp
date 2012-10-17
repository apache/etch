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

#include "common/EtchByte.h"

const EtchObjectType* EtchByte::TYPE() {
  const static EtchObjectType TYPE(EOTID_BYTE, NULL);
  return &TYPE;
}

EtchByte::EtchByte()
: mValue(0) {
  addObjectType(TYPE());
}

EtchByte::EtchByte(capu::int8_t value)
: mValue(value) {
  addObjectType(TYPE());
}

EtchByte::EtchByte(const EtchByte &other)
 : mValue(other.mValue) {
  addObjectType(TYPE());
}

void EtchByte::set(capu::int8_t value){
  mValue = value;
}

capu::int8_t EtchByte::get(){
  return mValue;
}

capu::bool_t EtchByte::equals(const EtchObject * other) const{
  if (other == NULL)
    return false;
  else if (!other->getObjectType()->equals(EtchByte::TYPE()))
    return false;
  EtchByte * a = (EtchByte*) other;
  return (a->mValue == this->mValue);
}

capu::int8_t& EtchByte::operator=(capu::int8_t const& other)
{
  if(mValue != other)
  {
    mValue = other;
  }
  return mValue;
}

EtchByte& EtchByte::operator=(EtchByte const& other)
{
  if(mValue != other.mValue)
  {
    mValue = other.mValue;
  }
  return *this;
}

capu::uint32_t EtchByte::getHashCode() const{
  return static_cast<capu::uint32_t>(mValue);
}

capu::int64_t EtchByte::getLongValue() const {
  return static_cast<capu::int64_t>(mValue);
}

capu::int32_t EtchByte::getInt32Value() const {
  return static_cast<capu::int32_t>(mValue);
}

capu::int16_t EtchByte::getShortValue() const {
  return static_cast<capu::int16_t>(mValue);
}

capu::int8_t EtchByte::getByteValue() const {
  return mValue;
}
