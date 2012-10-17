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

#include "serialization/EtchDateSerializer.h"
#include "support/EtchRuntime.h"

static char* TAG = "EtchDateSerializer";

const EtchString& EtchDateSerializer::FIELD_NAME() {
  static const EtchString name("dateTime");
  return name;
}

EtchDateSerializer::EtchDateSerializer(EtchRuntime* runtime, EtchType* type, EtchField* field)
: EtchImportExportHelper(runtime), mField(*field), mType(type) {
}

EtchDateSerializer::~EtchDateSerializer() {
}

status_t EtchDateSerializer::importValue(EtchStructValue* value, capu::SmartPointer<EtchObject> &result) {
  if (value == NULL) {
    return ETCH_EINVAL;
  } else if (!value->isType(mType)) {
    return ETCH_EINVAL;
  }

  capu::SmartPointer<EtchObject> tmp;
  if (value->get(mField, &tmp) != ETCH_OK) {
    CAPU_LOG_ERROR(mRuntime->getLogger(), TAG, "DateTime Field could not be found");
    return ETCH_ERROR;
  }

  EtchLong date = *((EtchLong*) tmp.get());
  result = new EtchDate((capu::time_t)date.get());
  CAPU_LOG_TRACE(mRuntime->getLogger(), TAG, "Date has been deserialized");
  return ETCH_OK;
}

status_t EtchDateSerializer::exportValue(EtchValueFactory* vf, capu::SmartPointer<EtchObject> value, EtchStructValue*& result) {
  if ((value.get() == NULL) || (vf == NULL))
    return ETCH_EINVAL;
  if (value->getObjectType() != EtchDate::TYPE())
    return ETCH_EINVAL;
  result = new EtchStructValue(mType, vf);
  EtchLong *date = new EtchLong(((EtchDate*) value.get())->get());
  if (result->put(mField, date) != ETCH_OK)
    return ETCH_ERROR;
  CAPU_LOG_TRACE(mRuntime->getLogger(), TAG, "Date has been serialized");
  return ETCH_OK;
}

status_t EtchDateSerializer::Init(EtchRuntime* runtime, EtchType* type, EtchClass2TypeMap* class2type) {

  status_t result;

  EtchField field;
  result = type->getField(FIELD_NAME(), &field);
  if (result != ETCH_OK) {
    return result;
  }

  result = class2type->put(EtchDate::TYPE(), type);
  if (result != ETCH_OK) {
    return result;
  }

  type->setComponentType(EtchDate::TYPE());

  //set the import export helper
  type->setImportExportHelper(new EtchDateSerializer(runtime, type, &field));

  //get validator
  capu::SmartPointer<EtchValidator> val;
  result = EtchValidatorLong::Get(runtime, 0, val);
  if (result != ETCH_OK) {
    return result;
  }
  result = type->putValidator(field, val);
  if (result != ETCH_OK) {
    return result;
  }

  type->lock();
  CAPU_LOG_TRACE(runtime->getLogger(), TAG, "EtchDateSerializer has been initialized");
  return ETCH_OK;
}
