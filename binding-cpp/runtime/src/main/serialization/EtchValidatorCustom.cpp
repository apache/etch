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

#include "serialization/EtchValidatorCustom.h"


const EtchObjectType* EtchValidatorCustomKey::TYPE() {
  const static EtchObjectType TYPE(EOTID_VALIDATOR_CUSTOM_KEY, NULL);
  return &TYPE;
}

EtchValidatorCustomKey::EtchValidatorCustomKey()
: EtchObject(EtchValidatorCustomKey::TYPE())
, mType(NULL)
, mDims(0)
, mSubclassOk(false) {
}

EtchValidatorCustomKey::EtchValidatorCustomKey(const EtchObjectType * type, capu::int32_t dim, capu::bool_t sub)
: EtchObject(EtchValidatorCustomKey::TYPE())
, mType(type)
, mDims(dim)
, mSubclassOk(sub) {
}

EtchValidatorCustomKey::~EtchValidatorCustomKey() {
}

capu::bool_t EtchValidatorCustomKey::equals(const EtchObject * other) const {
  if (other == NULL)
    return false;
  else if (!other->getObjectType()->equals(EtchValidatorCustomKey::TYPE()))
    return false;
  EtchValidatorCustomKey * a = (EtchValidatorCustomKey *) other;
  return ((a->mDims == this->mDims) && (a->mType->equals(this->mType)) && (a->mSubclassOk == this->mSubclassOk));
}

capu::uint32_t EtchValidatorCustomKey::getHashCode() const {
  return ((capu::uint32_t) mType->getTypeId()) ^ mDims * 9131 ^ (mSubclassOk ? 21357 : 8547);
}

const EtchObjectType* EtchValidatorCustom::TYPE() {
  const static EtchObjectType TYPE(EOTID_VALIDATOR_CUSTOM, NULL);
  return &TYPE;
}

EtchValidatorCustom::EtchValidatorCustom(const EtchObjectType *type, capu::uint32_t ndim, capu::bool_t sub)
  : EtchTypeValidator(EtchValidatorCustom::TYPE(), type, type, ndim) {
  // TODO refacotor type hierarchy
  mSubclass = sub;
}

EtchValidatorCustom::~EtchValidatorCustom() {
}

capu::bool_t EtchValidatorCustom::validate(capu::SmartPointer<EtchObject> value) {
  if(value.get() == NULL) {
    return false;
  }
  if(mExpectedType == NULL) {
    return false;
  }
  if(value->getObjectType()->equals(mExpectedType)) {
    return true;
  }
  // return ((dynamic_cast<T*> (value.get()) != NULL) && (mSubclass));
  // TODO: add object hierarchy fix

  return false;
}

status_t EtchValidatorCustom::validateValue(capu::SmartPointer<EtchObject> value, capu::SmartPointer<EtchObject>& result) {
  if (validate(value)) {
    result = value;
    return ETCH_OK;
  } else {
    return ETCH_ERROR;
  }
}

status_t EtchValidatorCustom::Get(capu::uint32_t ndim, const EtchObjectType *type, capu::bool_t sub, capu::SmartPointer<EtchValidator> &val) {
  if (ndim > MAX_NDIMS) {
    return ETCH_EINVAL;
  }

  EtchValidatorCustomKey key(type, ndim, sub);
  if (mValidators.get(key, &val) == ETCH_ENOT_EXIST) {
    val = new EtchValidatorCustom(type, ndim, sub);
    mValidators.put(key, val);
  }
  return ETCH_OK;
}

status_t EtchValidatorCustom::getElementValidator(capu::SmartPointer<EtchValidator> &val) {
  return EtchValidatorCustom::Get(mNDims - 1, mExpectedType->getObjectComponentType(), mSubclass, val);
}

EtchHashTable<EtchValidatorCustomKey, capu::SmartPointer<EtchValidator> > EtchValidatorCustom::mValidators;
