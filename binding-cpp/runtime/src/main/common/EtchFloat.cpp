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

#include "common/EtchFloat.h"

const EtchObjectType* EtchFloat::TYPE() {
  const static EtchObjectType TYPE(EOTID_FLOAT, NULL);
  return &TYPE;
}

EtchFloat::EtchFloat()
 : mValue(0.0f) {
  addObjectType(TYPE());
}

EtchFloat::EtchFloat(capu::float_t value)
 : mValue(value) {
  addObjectType(TYPE());
}

EtchFloat::EtchFloat(const EtchFloat& other)
 : EtchObject(other), mValue(other.mValue) {
}

void EtchFloat::set(capu::float_t value){
  mValue = value;
}

capu::float_t EtchFloat::get(){
  return mValue;
}

capu::uint32_t EtchFloat::getHashCode() const{
  capu::uint32_t result = 0;
  memcpy(&result, &mValue, sizeof(capu::float_t));
  return result;
}

capu::bool_t EtchFloat::equals(const EtchObject * other) const{
  if (other == NULL)
    return false;
  else if (!other->getObjectType()->equals(EtchFloat::TYPE()))
    return false;
  EtchFloat * a = (EtchFloat*) other;
  return (a->mValue == this->mValue);
}