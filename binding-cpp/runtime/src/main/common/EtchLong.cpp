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

EtchLong::EtchLong()
: EtchObject(EtchLong::TYPE_ID)
, mValue(0){
}

EtchLong::EtchLong(capu::int64_t value)
: EtchObject(EtchLong::TYPE_ID)
, mValue(value){
}

void EtchLong::set(capu::int64_t value){
  mValue = value;
}

capu::int64_t EtchLong::get(){
  return mValue;
}

capu::uint64_t EtchLong::getHashCode(){
  return static_cast <capu::uint64_t> (mValue);
}

capu::bool_t EtchLong::equals(const EtchObject * other){
  if (other == NULL)
    return false;
  else if (other->getObjectTypeId() != EtchLong::TYPE_ID)
    return false;
  EtchLong * a = (EtchLong*) other;
  return (a->mValue == this->mValue);
}
