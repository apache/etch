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

#include "serialization/EtchValidatorObject.h"
#include "support/EtchRuntime.h"

capu::SmartPointer<EtchValidator>* EtchValidatorObject::Validators(EtchRuntime* runtime) {
  static EtchValidatorCaches validators;
  return validators.get(runtime);
}

const EtchObjectType* EtchValidatorObject::TYPE() {
  const static EtchObjectType TYPE(EOTID_VALIDATOR_OBJECT, NULL);
  return &TYPE;
}

EtchValidatorObject::EtchValidatorObject(EtchRuntime* runtime, capu::uint32_t ndim)
: EtchTypeValidator(EtchValidatorObject::TYPE(), EtchObject::TYPE(), EtchObject::TYPE(), ndim), mRuntime(runtime) {
  mSubclass = true;
}

EtchValidatorObject::EtchValidatorObject(const EtchValidatorObject& other)
: EtchTypeValidator(other), mRuntime(other.mRuntime) {

}

EtchValidatorObject::~EtchValidatorObject() {
}

capu::bool_t EtchValidatorObject::validate(capu::SmartPointer<EtchObject> value) {
  if (value.get() == NULL)
    return false;
  if (mExpectedType == NULL)
    return false;
  if (value->getObjectType()->equals(mExpectedType))
    return true;
  return ((dynamic_cast<EtchObject*> (value.get()) != NULL) && (mSubclass));
}

status_t EtchValidatorObject::validateValue(capu::SmartPointer<EtchObject> value, capu::SmartPointer<EtchObject>& result) {
  if (validate(value)) {
    result = value;
    ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getValidatorContext(), "Object validated");
    return ETCH_OK;
  } else {
    ETCH_LOG_WARN(mRuntime->getLogger(), mRuntime->getLogger().getValidatorContext(), "Object validation failed");
    return ETCH_ERROR;
  }
}

status_t EtchValidatorObject::Get(EtchRuntime* runtime, capu::uint32_t ndim, capu::SmartPointer<EtchValidator> &val) {
  if (ndim > MAX_NDIMS)
    return ETCH_EINVAL;
  if (ndim >= MAX_CACHED) {
    val = new EtchValidatorObject(runtime, ndim);
    return ETCH_OK;
  }
  //TODO thread safety
  if (Validators(runtime)[ndim].get() == NULL) {
    ETCH_LOG_TRACE(runtime->getLogger(), runtime->getLogger().getValidatorContext(), "EtchValidatorObject has been created");
    Validators(runtime)[ndim] = new EtchValidatorObject(runtime, ndim);
  }
  val = Validators(runtime)[ndim];
  return ETCH_OK;
}

status_t EtchValidatorObject::getElementValidator(capu::SmartPointer<EtchValidator> &val) {
  if (mNDims > 0) {
    return EtchValidatorObject::Get(mRuntime, mNDims - 1, val);
  } else {
    return EtchValidatorObject::Get(mRuntime, 0, val);
  }
}
