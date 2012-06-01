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

capu::SmartPointer<EtchValidator> EtchValidatorRuntimeException::mValidator(NULL);
const EtchObjectType* EtchValidatorRuntimeException::TYPE() {
  const static EtchObjectType TYPE(EOTID_VALIDATOR_RUNTIME_EXCEPTION, NULL);
  return &TYPE;
}

EtchValidatorRuntimeException::EtchValidatorRuntimeException()
: EtchValidator(EtchValidatorRuntimeException::TYPE()) {

}

EtchValidatorRuntimeException::~EtchValidatorRuntimeException() {

}

capu::bool_t EtchValidatorRuntimeException::validate(EtchObject* value) {
  if (value == NULL)
    return false;
  return value->getObjectType()->equals(EtchRuntimeException::TYPE());
}

status_t EtchValidatorRuntimeException::validateValue(EtchObject* value, EtchObject*& result) {
  if (validate(value)) {
    result = value;
    return ETCH_OK;
  } else {
    return ETCH_ERROR;
  }
}

status_t EtchValidatorRuntimeException::Get(capu::SmartPointer<EtchValidator> &val) {
  if (mValidator.get() == NULL)
    mValidator = new EtchValidatorRuntimeException();
  val = mValidator;
  return ETCH_OK;
}

status_t EtchValidatorRuntimeException::getElementValidator(capu::SmartPointer<EtchValidator> &val) {
  return ETCH_EUNIMPL;
}
