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

#include "serialization/EtchTypeValidator.h"

EtchTypeValidator::EtchTypeValidator(const EtchObjectType * type, const EtchObjectType* scalarClass, const EtchObjectType* arrayClass, capu::uint32_t nDims, capu::bool_t subclass)
: EtchValidator(type),  mNDims(-1), mExpectedType(NULL), mArrayComponentType(NULL), mSubclass(subclass){
  if (checkDimension(nDims) == ETCH_OK)
    mNDims = nDims;

  if (mNDims == 0)
    mExpectedType = new EtchObjectType(scalarClass->getTypeId(),scalarClass->getObjectComponentType());
  else if (mNDims > 0){
    EtchObjectType *type = new EtchObjectType(EOTID_NATIVE_ARRAY, scalarClass);
    mExpectedType = type;
    mArrayComponentType = scalarClass;
  }

}

EtchTypeValidator::EtchTypeValidator(const EtchTypeValidator& other)
 : EtchValidator(other), mNDims(other.mNDims), mExpectedType(other.mExpectedType),
   mArrayComponentType(other.mArrayComponentType), mSubclass(other.mSubclass) {
}

EtchTypeValidator::~EtchTypeValidator(){
  delete mExpectedType;
}

capu::uint32_t EtchTypeValidator::getNDims(){
  return mNDims;
}

const EtchObjectType* EtchTypeValidator::getExpectedType(){
  return mExpectedType;
}

status_t EtchTypeValidator::checkDimension(capu::uint32_t ndim){
  if (ndim < EtchValidator::MAX_NDIMS)
    return ETCH_OK;
  else
    return ETCH_ERROR;
}
