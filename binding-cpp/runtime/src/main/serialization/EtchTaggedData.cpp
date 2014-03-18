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

#include "serialization/EtchTaggedData.h"
#include "common/EtchTypes.h"

EtchTaggedData::EtchTaggedData(EtchValueFactory* vf)
: mVf(vf) {

}

EtchTaggedData::~EtchTaggedData() {

}

EtchValueFactory* EtchTaggedData::getValueFactory() {
  return mVf;
}

const EtchObjectType* EtchTaggedData::getComponentType(capu::int8_t typeCode, EtchType* customStructType, capu::int32_t dim) {
  const EtchObjectType* c;
  if (typeCode == EtchTypeCode::CUSTOM || typeCode == EtchTypeCode::STRUCT) {
    c = customStructType->getComponentType();
    //0 is default value
    if (c == NULL) {
      c = EtchStructValue::TYPE();
    }
  } else {
    c = getNativeType(typeCode);
  }
  return c;
}

status_t EtchTaggedData::allocNativeArray(capu::int8_t typeCode, EtchType* customStructType, capu::int32_t dim, capu::int32_t length, capu::SmartPointer<EtchNativeArrayBase> &result) {
  const EtchObjectType* componentType = getComponentType(typeCode, customStructType, dim - 1);

  if (!componentType) {
    //unknown type code
    return ETCH_ERROR;
  }

  switch (componentType->getTypeId()) {
    case EOTID_INT32:
      result = new EtchNativeArray<capu::SmartPointer<EtchInt32> >(length, dim);
      break;
    case EOTID_BOOL:
      result = new EtchNativeArray<capu::SmartPointer<EtchBool> >(length, dim);
      break;
    case EOTID_BYTE:
      result = new EtchNativeArray<capu::SmartPointer<EtchByte> >(length, dim);
      break;
    case EOTID_SHORT:
      result = new EtchNativeArray<capu::SmartPointer<EtchShort> >(length, dim);
      break;
    case EOTID_LONG:
      result = new EtchNativeArray<capu::SmartPointer<EtchLong> >(length, dim);
      break;
    case EOTID_DOUBLE:
      result = new EtchNativeArray<capu::SmartPointer<EtchDouble> >(length, dim);
      break;
    case EOTID_FLOAT:
      result = new EtchNativeArray<capu::SmartPointer<EtchFloat> >(length, dim);
      break;
    case EOTID_STRING:
      result = new EtchNativeArray<capu::SmartPointer<EtchString> >(length, dim);
      break;
    case EOTID_DATE:
      result = new EtchNativeArray<capu::SmartPointer<EtchDate> >(length, dim);
      break;
    case EOTID_LIST:
      result = new EtchNativeArray<capu::SmartPointer<EtchList<EtchObject> > >(length, dim);
      break;
    case EOTID_HASHTABLE:
      result = new EtchNativeArray<capu::SmartPointer<EtchHashTable<EtchObject, EtchObject> > >(length, dim);
      break;
    case EOTID_SET:
      result = new EtchNativeArray<capu::SmartPointer<EtchHashSet<EtchObject> > >(length, dim);
      break;
    case EOTID_OBJECT:
      result = new EtchNativeArray<capu::SmartPointer<EtchObject> >(length, dim);
      break;

    default:
      status_t status = mVf->getNativeArrayForComponentType(componentType, result, length, dim);
      if (status != ETCH_OK) {
        return ETCH_ERROR;
      }

      break;
  }
  return ETCH_OK;
}

status_t EtchTaggedData::fromArrayValue(EtchArrayValue* av, capu::SmartPointer<EtchObject> &result) {
  if (av == NULL)
    return ETCH_EINVAL;
  result = av->getArray();
  return ETCH_OK;
}

status_t EtchTaggedData::toArrayValue(capu::SmartPointer<EtchObject> value, EtchValidator* v, EtchArrayValue *&result) {

  const EtchObjectType* c = value->getObjectType()->getObjectComponentType();
  capu::SmartPointer<EtchNativeArrayBase> array = capu::smartpointer_cast<EtchNativeArrayBase>(value);
  capu::int32_t dim = array->getDim();

  // now we want the type code for c, and if the type code is custom,
  // we'll also want the struct type code.
  capu::int8_t typeCode = getNativeTypeCode(c);

  EtchType* customStructType = NULL;
  if (typeCode == EtchTypeCode::CUSTOM || typeCode == EtchTypeCode::STRUCT) {
    getCustomStructType(c, customStructType);

    if (customStructType == NULL && c == EtchStructValue::TYPE()) {
      EtchValidatorStructValue* x;
      findStructValueValidator(v, x);
      if (findStructValueValidator(v, x) == ETCH_OK) {
        customStructType = x->getType();
      }
    }

    if (customStructType == NULL) {
      return ETCH_ERROR;
    }
  } else {
    customStructType = NULL;
  }

  //not owned here
  result = new EtchArrayValue(array, array->getLength(), typeCode, customStructType, dim);
  return ETCH_OK;
}

status_t EtchTaggedData::findStructValueValidator(EtchValidator* v, EtchValidatorStructValue*& result) {
  if (v->getObjectType() == EtchValidatorStructValue::TYPE()) {
    result = (EtchValidatorStructValue*) v;
    return ETCH_OK;
  }

  if (v->getObjectType() == EtchComboValidator::TYPE()) {
    EtchComboValidator* x = (EtchComboValidator*) v;

    if (findStructValueValidator(x->getFirst().get(), result) == ETCH_ERROR) {
      return findStructValueValidator(x->getSecond().get(), result);
    } else {
      return ETCH_OK;
    }
  }
  return ETCH_ERROR;
}
