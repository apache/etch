/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "common/EtchArrayValue.h"
#include "common/EtchObject.h"
#include "common/EtchInt32.h"
#include "common/EtchBool.h"
#include "common/EtchByte.h"
#include "common/EtchShort.h"
#include "common/EtchLong.h"
#include "common/EtchDouble.h"
#include "common/EtchFloat.h"
#include "common/EtchString.h"
#include "common/EtchDate.h"
#include "common/EtchList.h"
#include "common/EtchHashTable.h"
#include "common/EtchHashSet.h"

const EtchObjectType EtchArrayValue::TYPE(EOTID_ARRAY_VALUE, NULL);

EtchArrayValue::EtchArrayValue(capu::SmartPointer<EtchNativeArrayBase> array, capu::int32_t size, capu::int8_t typeCode, EtchType* customStructType, capu::int32_t dim)
: EtchObject(&EtchArrayValue::TYPE), mArray(array), mTypeCode(typeCode),
mCustomStructType(customStructType), mDim(dim), mAddIndex(size), mSize(size) {
}

EtchArrayValue::EtchArrayValue(capu::SmartPointer<EtchNativeArrayBase> array, capu::int32_t size)
: EtchObject(&EtchArrayValue::TYPE), mArray(array), mTypeCode(0),
mCustomStructType(NULL), mDim(1), mAddIndex(size), mSize(size) {
}

EtchArrayValue::~EtchArrayValue() {

}

capu::int32_t EtchArrayValue::getDim() {
  return mDim;
}

capu::uint32_t EtchArrayValue::getSize() {
  return mSize;
}

capu::int8_t EtchArrayValue::getTypeCode() {
  return mTypeCode;
}

capu::SmartPointer<EtchNativeArrayBase> EtchArrayValue::getArray() {
  return mArray;
}

capu::int32_t EtchArrayValue::getIndex() {
  return mAddIndex;
}

void EtchArrayValue::setIndex(capu::int32_t val) {
  mAddIndex = val;
}

EtchType * EtchArrayValue::getCustomStructType() {
  return mCustomStructType;
}

status_t EtchArrayValue::get(capu::uint32_t index, capu::SmartPointer<EtchObject> &result) {
  if ((index >= getSize()) || (mArray.get() == NULL)) {
    return ETCH_EINVAL;
  }
  return mArray->getBase(index,result);
}

status_t EtchArrayValue::add(capu::SmartPointer<EtchObject> value) {
  if (value.get() == NULL) {
    return ETCH_EINVAL;
  }
  
  //check if we have to resize the array
  capu::int32_t n = mSize;
  if (mAddIndex >= n) {
    if (n == 0) {
      n = 8;
    } else {
      n *= 2;
    }
    mArray->resize(n);
    mSize = n;
  }
  return mArray->setBase(mAddIndex++, value);
}
