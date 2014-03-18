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

#include "common/EtchInt32.h"

const EtchObjectType* EtchInt32::TYPE() {
   const static EtchObjectType TYPE(EOTID_INT32, NULL);
   return &TYPE;
}

EtchInt32::EtchInt32()
 : mValue(0) {
  addObjectType(TYPE());
}

EtchInt32::EtchInt32(capu::int32_t value)
 : mValue(value) {
  addObjectType(TYPE());
}

EtchInt32::EtchInt32(const EtchInt32& other)
 : mValue(other.mValue) {
  addObjectType(TYPE());
}

EtchInt32::~EtchInt32() {
}


void EtchInt32::set(capu::int32_t value){
  mValue = value;
}

capu::int32_t EtchInt32::get() const{
  return mValue;
}

capu::uint32_t EtchInt32::getHashCode() const{
  return static_cast <capu::uint32_t> (mValue);
}

capu::bool_t EtchInt32::equals(const EtchObject * other) const{
  if (other == NULL)
    return false;
  else if (!other->getObjectType()->equals(EtchInt32::TYPE()))
    return false;
  EtchInt32 *a = (EtchInt32*) other;
  return (a->mValue == this->mValue);
}

capu::int32_t& EtchInt32::operator=(capu::int32_t const& other)
{
  if(mValue != other)
  {
    mValue = other;
  }
  return mValue;
}

EtchInt32& EtchInt32::operator=(EtchInt32 const& other)
{
  if(mValue != other.mValue)
  {
    mValue = other.mValue;
  }
  return *this;
}

capu::int32_t& EtchInt32::operator++() //pre increment
{
  ++mValue;
  return mValue;
}
capu::int32_t EtchInt32::operator++(int) //post increment
{
  capu::int32_t tmp(mValue);
  ++mValue;
  return tmp;
}

capu::int32_t& EtchInt32::operator--() //pre increment
{
  --mValue;
  return mValue;
}

capu::int32_t EtchInt32::operator--(int) //post increment
{
  capu::int32_t tmp(mValue);
  --mValue;
  return tmp;
}

capu::bool_t EtchInt32::operator==(const EtchObject& other) const
{
  return this->equals(&other);
}

capu::bool_t EtchInt32::operator!=(const EtchObject& other) const
{
  return !(*this == other);
}

capu::int64_t EtchInt32::getLongValue() const {
  return static_cast<capu::int64_t>(mValue);
}

capu::int32_t EtchInt32::getInt32Value() const {
  return mValue;
}

capu::int16_t EtchInt32::getShortValue() const {
  return static_cast<capu::int16_t>(mValue);
}

capu::int8_t EtchInt32::getByteValue() const {
  return static_cast<capu::int8_t>(mValue);
}
