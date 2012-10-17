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

#include "common/EtchBool.h"

const EtchObjectType* EtchBool::TYPE() {
  const static EtchObjectType TYPE(EOTID_BOOL, NULL);
  return &TYPE;
}

EtchBool::EtchBool()
: mValue(false){
  addObjectType(TYPE());
}

EtchBool::EtchBool(capu::bool_t value)
 : mValue(value){
  addObjectType(TYPE());
}

EtchBool::EtchBool(const EtchBool& other)
 : EtchObject(other), mValue(other.mValue) {
}


void EtchBool::set(capu::bool_t value){
  mValue = value;
}

capu::bool_t EtchBool::get(){
  return mValue;
}

capu::bool_t EtchBool::equals(const EtchObject * other) const{
  if (other == NULL)
    return false;
  else if (!other->getObjectType()->equals(EtchBool::TYPE()))
    return false;
  EtchBool* a = (EtchBool *) other;
  return (a->mValue == this->mValue);
}

capu::uint32_t EtchBool::getHashCode() const{
  if (mValue)
    return 0;
  return 1;
}
