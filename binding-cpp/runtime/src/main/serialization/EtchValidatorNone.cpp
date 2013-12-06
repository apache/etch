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

#include "serialization/EtchValidatorNone.h"
#include "support/EtchRuntime.h"

capu::SmartPointer<EtchValidator>* EtchValidatorNone::Validators(EtchRuntime* runtime) {
  static EtchValidatorCaches validators;
  return validators.get(runtime);
}

const EtchObjectType* EtchValidatorNone::TYPE() {
  static const EtchObjectType TYPE(EOTID_VALIDATOR_NONE, NULL);
  return &TYPE;
}

EtchValidatorNone::EtchValidatorNone(EtchRuntime* runtime)
: EtchValidator(EtchValidatorNone::TYPE()), mRuntime(runtime) {
}

EtchValidatorNone::EtchValidatorNone(const EtchValidatorNone& other)
: EtchValidator(other), mRuntime(other.mRuntime) {

}

EtchValidatorNone::~EtchValidatorNone() {
}

capu::bool_t EtchValidatorNone::validate(capu::SmartPointer<EtchObject> value) {
  return value->equals(EtchBinaryTaggedData::NONE().get());
}

status_t EtchValidatorNone::validateValue(capu::SmartPointer<EtchObject> value, capu::SmartPointer<EtchObject>& result) {
  if (validate(value)) {
    result = value;
    ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getValidatorContext(), "NONE type object has been validated");
    return ETCH_OK;
  } else {
    ETCH_LOG_WARN(mRuntime->getLogger(), mRuntime->getLogger().getValidatorContext(), "NONE type object validation failed");
    return ETCH_ERROR;
  }
}

status_t EtchValidatorNone::Get(EtchRuntime* runtime, capu::SmartPointer<EtchValidator> &val) {
  if (Validators(runtime)[0].get() == NULL) {
    Validators(runtime)[0] = new EtchValidatorNone(runtime);
    ETCH_LOG_TRACE(runtime->getLogger(), runtime->getLogger().getValidatorContext(), "EtchValidatorNone has been created");
  }
  val = Validators(runtime)[0];
  return ETCH_OK;
}

status_t EtchValidatorNone::getElementValidator(capu::SmartPointer<EtchValidator> &val) {
  return ETCH_EUNIMPL;
}
