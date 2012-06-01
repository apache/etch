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

#include "serialization/EtchValidatorByte.h"

capu::SmartPointer<EtchValidator> EtchValidatorByte::mValidator[MAX_CACHED];
const EtchObjectType EtchValidatorByte::TYPE(EOTID_VALIDATOR_BYTE, NULL);

EtchValidatorByte::EtchValidatorByte(capu::uint32_t ndim)
: EtchTypeValidator(&EtchValidatorByte::TYPE, &EtchByte::TYPE, &EtchByte::TYPE, ndim) {

}

EtchValidatorByte::~EtchValidatorByte() {

}

capu::bool_t EtchValidatorByte::validate(EtchObject* value) {
  if (value == NULL)
    return false;
  if (mExpectedType == NULL)
    return false;

  if (value->getObjectType()->equals(mExpectedType))
    return true;

  if (value->getObjectType()->equals(&EtchShort::TYPE)) {
    EtchShort *v = (EtchShort *) value;
    return ((v->get() >= capu::NumericLimitMin<capu::int8_t> ()) && (v->get() <= capu::NumericLimitMax<capu::int8_t> ()));
  }

  if (value->getObjectType()->equals(&EtchInt32::TYPE)) {
    EtchInt32 *v = (EtchInt32 *) value;
    return ((v->get() >= capu::NumericLimitMin<capu::int8_t> ()) && (v->get() <= capu::NumericLimitMax<capu::int8_t> ()));
  }

  if (value->getObjectType()->equals(&EtchLong::TYPE)) {
    EtchLong *v = (EtchLong *) value;
    return ((v->get() >= capu::NumericLimitMin<capu::int8_t> ()) && (v->get() <= capu::NumericLimitMax<capu::int8_t> ()));
  }
  //array handling
  if ((value->getObjectType()->isArray()) && (mExpectedType->isArray())) {

    EtchNativeArray<EtchObject*> *array = (EtchNativeArray<EtchObject*> *) value;
    const EtchObjectType* type = array->getContentType();
    const EtchObjectType* type2 = mExpectedType->getObjectComponentType();
    while ((type->getObjectComponentType() != NULL) && (mExpectedType->getObjectComponentType() != NULL)) {
      type = type->getObjectComponentType();
      type2 = type2->getObjectComponentType();
    }

    //both of them should be pointing null
    if (type->getObjectComponentType() != type2->getObjectComponentType()) {
      return false;
    }
    return type->equals(type2);
  }


  return false;
}

status_t EtchValidatorByte::validateValue(EtchObject* value, EtchObject*& result) {
  if (validate(value)) {
    if ((value->getObjectType()->equals(&EtchByte::TYPE)) || (mNDims > 0)) {
      result = value;
      return ETCH_OK;
    } else {
      if (value->getObjectType()->equals(&EtchShort::TYPE)) {
        EtchShort *v = (EtchShort *) value;
        result = (EtchObject*) (new EtchByte((capu::int8_t)v->get()));
        return ETCH_OK;
      }

      if (value->getObjectType()->equals(&EtchLong::TYPE)) {
        EtchByte *v = (EtchByte *) value;
        result = (EtchObject*) (new EtchByte((capu::int8_t)v->get()));
        return ETCH_OK;
      }

      if (value->getObjectType()->equals(&EtchInt32::TYPE)) {
        EtchInt32 *v = (EtchInt32 *) value;
        result = (EtchObject*) (new EtchByte((capu::int8_t)v->get()));
        return ETCH_OK;
      }
      return ETCH_ERROR;
    }
  } else {
    return ETCH_ERROR;
  }
}

status_t EtchValidatorByte::Get(capu::uint32_t ndim, capu::SmartPointer<EtchValidator> &val) {
  if (ndim > MAX_NDIMS) {
    return ETCH_EINVAL;
  }
  if (ndim >= MAX_CACHED) {
    val = new EtchValidatorByte(ndim);
    return ETCH_OK;
  }
  if (mValidator[ndim].get() == NULL) {
    mValidator[ndim] = new EtchValidatorByte(ndim);
  }
  val = mValidator[ndim];
  return ETCH_OK;
}

status_t EtchValidatorByte::getElementValidator(capu::SmartPointer<EtchValidator> &val) {
  return EtchValidatorByte::Get(mNDims - 1, val);
}