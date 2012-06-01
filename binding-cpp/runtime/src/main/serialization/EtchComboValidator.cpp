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

#include "serialization/EtchComboValidator.h"

const EtchObjectType* EtchComboValidator::TYPE() {
  const static EtchObjectType TYPE(EOTID_VALIDATOR_COMBO, NULL);
  return &TYPE;
}

EtchComboValidator::EtchComboValidator(capu::SmartPointer<EtchValidator> a, capu::SmartPointer<EtchValidator> b)
: EtchValidator(EtchComboValidator::TYPE()), mFirst(a), mSecond(b) {

}

EtchComboValidator::~EtchComboValidator() {

}

capu::SmartPointer<EtchValidator> EtchComboValidator::getFirst() {
  return mFirst;
}

capu::SmartPointer<EtchValidator> EtchComboValidator::getSecond() {
  return mSecond;
}

capu::bool_t EtchComboValidator::validate(capu::SmartPointer<EtchObject> value) {
  return mFirst->validate(value) || mSecond->validate(value);
}

status_t EtchComboValidator::validateValue(capu::SmartPointer<EtchObject> value, capu::SmartPointer<EtchObject>& result) {
  if (mFirst->validateValue(value, result) == ETCH_OK) {
    return ETCH_OK;
  }
  return mSecond->validateValue(value, result);
}

status_t EtchComboValidator::getElementValidator(capu::SmartPointer<EtchValidator> &val) {
  capu::SmartPointer<EtchValidator> na;

  status_t a = mFirst->getElementValidator(na);

  capu::SmartPointer<EtchValidator> nb;

  status_t b = mSecond->getElementValidator(nb);

  if ((a != ETCH_OK) && (b != ETCH_OK))
    return ETCH_EINVAL;

  if (a != ETCH_OK) {
    val = nb;
    return ETCH_OK;
  }
  if (b != ETCH_OK) {
    val = na;
    return ETCH_OK;
  }

  val = new EtchComboValidator(na, nb);
  return ETCH_OK;
}
