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

#include "serialization/EtchValidatorDouble.h"
#include "support/EtchRuntime.h"

VALIDATOR_CACHE_IMPL(EtchValidatorDouble)

const EtchObjectType* EtchValidatorDouble::TYPE() {
  const static EtchObjectType TYPE(EOTID_VALIDATOR_DOUBLE, NULL);
  return &TYPE;
}

EtchValidatorDouble::EtchValidatorDouble(EtchRuntime* runtime, capu::uint32_t ndim)
: EtchTypeValidator(EtchValidatorDouble::TYPE(), EtchDouble::TYPE(), EtchDouble::TYPE(), ndim), mRuntime(runtime) {
}

EtchValidatorDouble::EtchValidatorDouble(const EtchValidatorDouble& other)
: EtchTypeValidator(other), mRuntime(other.mRuntime) {

}

EtchValidatorDouble::~EtchValidatorDouble() {
}

capu::bool_t EtchValidatorDouble::validate(capu::SmartPointer<EtchObject> value) {
  if (value.get() == NULL)
    return false;

  if (mExpectedType == NULL)
    return false;

  if (value->getObjectType() ->equals(mExpectedType))
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

status_t EtchValidatorDouble::validateValue(capu::SmartPointer<EtchObject> value, capu::SmartPointer<EtchObject>& result) {
  if (validate(value)) {
    result = value;
    ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getValidatorContext(), "Double object validated");
    return ETCH_OK;
  } else {
    ETCH_LOG_WARN(mRuntime->getLogger(), mRuntime->getLogger().getValidatorContext(), "Double object validation failed");
    return ETCH_ERROR;
  }
}

status_t EtchValidatorDouble::getElementValidator(capu::SmartPointer<EtchValidator> &val) {
  return EtchValidatorDouble::Get(mRuntime, mNDims - 1, val);
}

VALIDATOR_GET_IMPL(EtchValidatorDouble)
