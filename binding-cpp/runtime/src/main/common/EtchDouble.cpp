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

#include "common/EtchDouble.h"

EtchDouble::EtchDouble()
: EtchObject(EtchDouble::TYPE_ID)
, mValue(0.0L){
}

EtchDouble::EtchDouble(capu::double_t value)
: EtchObject(EtchDouble::TYPE_ID)
, mValue(value){
}

void EtchDouble::set(capu::double_t value){
  mValue = value;
}

capu::double_t EtchDouble::get(){
  return mValue;
}

capu::uint64_t EtchDouble::getHashCode(){
   capu::uint64_t result = 0;
  memcpy(&result, &mValue, sizeof(capu::double_t));
  return result;
}

capu::bool_t EtchDouble::equals(const EtchObject * other){
  if (other == NULL)
    return false;
  else if (other->getObjectTypeId() != EtchDouble::TYPE_ID)
    return false;
  EtchDouble * a = (EtchDouble *) other;
  return (a->mValue == this->mValue);
}