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

#include "serialization/EtchRuntimeExceptionSerializer.h"
#include "support/EtchRuntime.h"

const EtchString& EtchRuntimeExceptionSerializer::FIELD_NAME() {
  static const EtchString name("msg");
  return name;
}

// TODO: check signature regarding by value copy

EtchRuntimeExceptionSerializer::EtchRuntimeExceptionSerializer(EtchRuntime* runtime, EtchType* type, EtchField* field)
: EtchImportExportHelper(runtime), mField(*field), mType(type) {
}

EtchRuntimeExceptionSerializer::~EtchRuntimeExceptionSerializer() {

}

status_t EtchRuntimeExceptionSerializer::importValue(EtchStructValue* value, capu::SmartPointer<EtchObject> &result) {
  if (value == NULL) {
    return ETCH_EINVAL;
  } else if (!value->isType(mType)) {
    return ETCH_EINVAL;
  }

  capu::SmartPointer<EtchObject> tmp;
  if (value->get(mField, &tmp) != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Msg Field could not be found");
    return ETCH_ERROR;
  }

  EtchString str = *((EtchString*) tmp.get());
  result = new EtchRuntimeException(str, ETCH_ERROR);
  ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Exception has been deserialized");
  return ETCH_OK;
}

status_t EtchRuntimeExceptionSerializer::exportValue(EtchValueFactory* vf, capu::SmartPointer<EtchObject> value, EtchStructValue *&result) {
  if ((value.get() == NULL) || (vf == NULL))
    return ETCH_EINVAL;
  if (value->getObjectType() != EtchRuntimeException::TYPE())
    return ETCH_EINVAL;
  result = new EtchStructValue(mType, vf);
  capu::SmartPointer<EtchString> str = new EtchString(((EtchRuntimeException*) value.get())->getErrorMessage());

  if (result->put(mField, str) != ETCH_OK)
    return ETCH_ERROR;
  ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Exception has been serialized");
  return ETCH_OK;
}

status_t EtchRuntimeExceptionSerializer::Init(EtchRuntime* runtime, EtchType* type, EtchClass2TypeMap* class2type) {
  status_t result;

  EtchField field;
  result = type->getField(FIELD_NAME(), &field);
  if (result != ETCH_OK) {
    return result;
  }

  result = class2type->put(EtchRuntimeException::TYPE(), type);
  if (result != ETCH_OK) {
    return result;
  }

  type->setComponentType(EtchRuntimeException::TYPE());

  //set the import export helper
  type->setImportExportHelper(new EtchRuntimeExceptionSerializer(runtime, type, &field));

  //get validator
  capu::SmartPointer<EtchValidator> tmp;
  result = EtchValidatorString::Get(runtime, 0, tmp);
  if (result != ETCH_OK) {
    return result;
  }
  result = type->putValidator(field, tmp);
  if (result != ETCH_OK) {
    return result;
  }

  //lock type
  type->lock();
  ETCH_LOG_TRACE(runtime->getLogger(), runtime->getLogger().getSerializerContext(), "EtchRuntimeExceptionSerializer has been initialized");
  return ETCH_OK;
}
