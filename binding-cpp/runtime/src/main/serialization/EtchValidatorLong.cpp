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

#include "serialization/EtchValidatorLong.h"

capu::SmartPointer<EtchValidator> EtchValidatorLong::mValidator[MAX_CACHED];

const EtchObjectType* EtchValidatorLong::TYPE() {
  const static EtchObjectType TYPE(EOTID_VALIDATOR_LONG, NULL);
  return &TYPE;
}

EtchValidatorLong::EtchValidatorLong(capu::uint32_t ndim)
: EtchTypeValidator(EtchValidatorLong::TYPE(), EtchLong::TYPE(), EtchLong::TYPE(), ndim) {

}

EtchValidatorLong::~EtchValidatorLong() {

}

capu::bool_t EtchValidatorLong::validate(capu::SmartPointer<EtchObject> value) {
  if (value.get() == NULL)
    return false;

  if (mExpectedType == NULL)
    return false;

  if (value->getObjectType()->equals(mExpectedType))
    return true;

  if ((value->getObjectType()->equals(EtchInt32::TYPE())) ||
          (value->getObjectType()->equals(EtchShort::TYPE())) ||
          (value->getObjectType()->equals(EtchByte::TYPE())))
    return true;

  //handle array
  if ((value->getObjectType()->isArray()) && (mExpectedType->isArray())) {
    EtchNativeArray<EtchObject*> *array = (EtchNativeArray<EtchObject*> *) value.get();
    if (array->getDim() != mNDims) {
      return false;
    }
    const EtchObjectType* type = array->getObjectType()->getObjectComponentType();
    const EtchObjectType* type2 = mExpectedType->getObjectComponentType();
    return type->equals(type2);
  }
  return false;
}

status_t EtchValidatorLong::validateValue(capu::SmartPointer<EtchObject> value, capu::SmartPointer<EtchObject>& result) {
  if (validate(value)) {
    if ((value->getObjectType()->equals(EtchLong::TYPE())) || (mNDims > 0)) {
      result = value;
      return ETCH_OK;
    } else {
      if (value->getObjectType()->equals(EtchShort::TYPE())) {
        EtchShort *v = (EtchShort *) value.get();
        result = new EtchLong((capu::int64_t)v->get());
        return ETCH_OK;
      }

      if (value->getObjectType()->equals(EtchByte::TYPE())) {
        EtchByte *v = (EtchByte *) value.get();
        result = new EtchLong((capu::int64_t)v->get());
        return ETCH_OK;
      }

      if (value->getObjectType()->equals(EtchInt32::TYPE())) {
        EtchInt32 *v = (EtchInt32 *) value.get();
        result = new EtchLong((capu::int64_t)v->get());
        return ETCH_OK;
      }
      return ETCH_ERROR;
    }
  } else {
    return ETCH_ERROR;
  }
}

status_t EtchValidatorLong::Get(capu::uint32_t ndim, capu::SmartPointer<EtchValidator> &val) {
  if (ndim > MAX_NDIMS) {
    return ETCH_EINVAL;
  }
  if (ndim >= MAX_CACHED) {
    val = new EtchValidatorLong(ndim);
    return ETCH_OK;
  }
  if (mValidator[ndim].get() == NULL) {
    mValidator[ndim] = new EtchValidatorLong(ndim);
  }
  val = mValidator[ndim];
  return ETCH_OK;
}

status_t EtchValidatorLong::getElementValidator(capu::SmartPointer<EtchValidator> &val) {
  return EtchValidatorLong::Get(mNDims - 1, val);
}