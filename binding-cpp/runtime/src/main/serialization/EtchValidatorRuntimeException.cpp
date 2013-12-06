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

#include "serialization/EtchValidatorRuntimeException.h"
#include "support/EtchRuntime.h"

capu::SmartPointer<EtchValidator>* EtchValidatorRuntimeException::Validators(EtchRuntime* runtime) {
  static EtchValidatorCaches validators;
  return validators.get(runtime);
}

const EtchObjectType* EtchValidatorRuntimeException::TYPE() {
  const static EtchObjectType TYPE(EOTID_VALIDATOR_RUNTIME_EXCEPTION, NULL);
  return &TYPE;
}

EtchValidatorRuntimeException::EtchValidatorRuntimeException(EtchRuntime* runtime)
: EtchValidator(EtchValidatorRuntimeException::TYPE()), mRuntime(runtime) {
}

EtchValidatorRuntimeException::EtchValidatorRuntimeException(const EtchValidatorRuntimeException& other)
: EtchValidator(other), mRuntime(other.mRuntime) {

}

EtchValidatorRuntimeException::~EtchValidatorRuntimeException() {
}

capu::bool_t EtchValidatorRuntimeException::validate(capu::SmartPointer<EtchObject> value) {
  if (value.get() == NULL)
    return false;
  return value->getObjectType()->equals(EtchRuntimeException::TYPE());
}

status_t EtchValidatorRuntimeException::validateValue(capu::SmartPointer<EtchObject> value, capu::SmartPointer<EtchObject>& result) {
  if (validate(value)) {
    result = value;
    ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getValidatorContext(), "EtchRuntimeException validated");
    return ETCH_OK;
  } else {
    ETCH_LOG_WARN(mRuntime->getLogger(), mRuntime->getLogger().getValidatorContext(), "EtchRuntimeException object validation failed");
    return ETCH_ERROR;
  }
}

status_t EtchValidatorRuntimeException::Get(EtchRuntime* runtime, capu::SmartPointer<EtchValidator> &val) {
  if (Validators(runtime)[0].get() == NULL) {
    Validators(runtime)[0] = new EtchValidatorRuntimeException(runtime);
    ETCH_LOG_TRACE(runtime->getLogger(), runtime->getLogger().getValidatorContext(),"EtchValidatorRuntimeException has been created");
  }
  val = Validators(runtime)[0];
  return ETCH_OK;
}

status_t EtchValidatorRuntimeException::getElementValidator(capu::SmartPointer<EtchValidator> &val) {
  return ETCH_EUNIMPL;
}
