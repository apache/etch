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


capu::SmartPointer<EtchValidator> EtchValidatorObject::mValidator[MAX_CACHED];

const EtchObjectType* EtchValidatorObject::TYPE() {
  const static EtchObjectType TYPE(EOTID_VALIDATOR_OBJECT, NULL);
  return &TYPE;
}

EtchValidatorObject::EtchValidatorObject(capu::uint32_t ndim)
: EtchTypeValidator(EtchValidatorObject::TYPE(), EtchObject::TYPE(), EtchObject::TYPE(), ndim) {
  mSubclass = true;
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
    return ETCH_OK;
  } else {
    return ETCH_ERROR;
  }
}

status_t EtchValidatorObject::Get(capu::uint32_t ndim, capu::SmartPointer<EtchValidator> &val) {
  if (ndim > MAX_NDIMS)
    return ETCH_EINVAL;
  if (ndim >= MAX_CACHED) {
    val = new EtchValidatorObject(ndim);
    return ETCH_OK;
  }
  if (mValidator[ndim].get() == NULL) {
    mValidator[ndim] = new EtchValidatorObject(ndim);
  }
  val = mValidator[ndim];
  return ETCH_OK;
}

status_t EtchValidatorObject::getElementValidator(capu::SmartPointer<EtchValidator> &val) {
  return EtchValidatorObject::Get(mNDims - 1, val);
}
