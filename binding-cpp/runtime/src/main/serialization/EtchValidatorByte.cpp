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
#include "support/EtchRuntime.h"

capu::SmartPointer<EtchValidator>* EtchValidatorByte::Validators(EtchRuntime* runtime) {
  static EtchValidatorCaches validators;
  return validators.get(runtime);
}

const EtchObjectType* EtchValidatorByte::TYPE() {
  const static EtchObjectType TYPE(EOTID_VALIDATOR_BYTE, NULL);
  return &TYPE;
}

EtchValidatorByte::EtchValidatorByte(EtchRuntime* runtime, capu::uint32_t ndim)
: EtchTypeValidator(EtchValidatorByte::TYPE(), EtchByte::TYPE(), EtchByte::TYPE(), ndim)
, mRuntime(runtime) {
}

EtchValidatorByte::EtchValidatorByte(const EtchValidatorByte& other)
: EtchTypeValidator(other)
, mRuntime(other.mRuntime) {
}

EtchValidatorByte::~EtchValidatorByte() {

}

capu::bool_t EtchValidatorByte::validate(capu::SmartPointer<EtchObject> value) {
  if (value.get() == NULL)
    return false;
  if (mExpectedType == NULL)
    return false;

  if (value->getObjectType()->equals(mExpectedType))
    return true;

  if (value->getObjectType()->equals(EtchShort::TYPE())) {
    EtchShort *v = (EtchShort *) value.get();
    return ((v->get() >= capu::NumericLimits::Min<capu::int8_t > ()) && (v->get() <= capu::NumericLimits::Max<capu::int8_t > ()));
  }

  if (value->getObjectType()->equals(EtchInt32::TYPE())) {
    EtchInt32 *v = (EtchInt32 *) value.get();
    return ((v->get() >= capu::NumericLimits::Min<capu::int8_t > ()) && (v->get() <= capu::NumericLimits::Max<capu::int8_t > ()));
  }

  if (value->getObjectType()->equals(EtchLong::TYPE())) {
    EtchLong *v = (EtchLong *) value.get();
    return ((v->get() >= capu::NumericLimits::Min<capu::int8_t > ()) && (v->get() <= capu::NumericLimits::Max<capu::int8_t > ()));
  }
  //array handling
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

status_t EtchValidatorByte::validateValue(capu::SmartPointer<EtchObject> value, capu::SmartPointer<EtchObject>& result) {
  if (validate(value)) {
    if ((value->getObjectType()->equals(EtchByte::TYPE())) || (mNDims > 0)) {
      result = value;
      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getValidatorContext(), "Byte object validation successful");
      return ETCH_OK;
    } else {
      if (value->getObjectType()->equals(EtchShort::TYPE())) {
        EtchShort *v = (EtchShort *) value.get();
        result = new EtchByte((capu::int8_t)v->get());
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getValidatorContext(), "Short has been validated by EtchValidatorByte and converted to EtchByte");

        return ETCH_OK;
      }

      if (value->getObjectType()->equals(EtchLong::TYPE())) {
        EtchLong *v = (EtchLong *) value.get();
        result = new EtchByte((capu::int8_t)v->get());
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getValidatorContext(), "EtchLong has been validated by EtchValidatorByte and converted to EtchByte");

        return ETCH_OK;
      }

      if (value->getObjectType()->equals(EtchInt32::TYPE())) {
        EtchInt32 *v = (EtchInt32 *) value.get();
        result = new EtchByte((capu::int8_t)v->get());
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getValidatorContext(), "EtchInt32 has been validated by EtchValidatorByte and converted to EtchByte");
        return ETCH_OK;
      }
      ETCH_LOG_WARN(mRuntime->getLogger(), mRuntime->getLogger().getValidatorContext(), "Byte object validation failed due to unknown type");
      return ETCH_ERROR;
    }
  } else {
    ETCH_LOG_WARN(mRuntime->getLogger(), mRuntime->getLogger().getValidatorContext(), "Byte object validation failed");
    return ETCH_ERROR;
  }
}

status_t EtchValidatorByte::Get(EtchRuntime* runtime, capu::uint32_t ndim, capu::SmartPointer<EtchValidator> &val) {
  if (ndim > MAX_NDIMS) {
    return ETCH_EINVAL;
  }
  if (ndim >= MAX_CACHED) {
    val = new EtchValidatorByte(runtime, ndim);
    return ETCH_OK;
  }
  if (Validators(runtime)[ndim].get() == NULL) {
    Validators(runtime)[ndim] = new EtchValidatorByte(runtime, ndim);
    ETCH_LOG_TRACE(runtime->getLogger(), runtime->getLogger().getValidatorContext(), "EtchValidatorByte has been created");

  }
  val = Validators(runtime)[ndim];
  return ETCH_OK;
}

status_t EtchValidatorByte::getElementValidator(capu::SmartPointer<EtchValidator> &val) {
  return EtchValidatorByte::Get(mRuntime, mNDims - 1, val);
}
