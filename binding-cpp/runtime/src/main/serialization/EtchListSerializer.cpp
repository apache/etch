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

#include "serialization/EtchListSerializer.h"
#include "support/EtchRuntime.h"

const EtchString& EtchListSerializer::FIELD_NAME() {
  static const EtchString name("values");
  return name;
}

// TODO: check signature regarding by value copy

EtchListSerializer::EtchListSerializer(EtchRuntime* runtime, EtchType* type, EtchField* field)
: EtchImportExportHelper(runtime), mField(*field), mType(type) {
}

EtchListSerializer::~EtchListSerializer() {
}

status_t EtchListSerializer::exportValue(EtchValueFactory* vf, capu::SmartPointer<EtchObject> value, EtchStructValue *&result) {
  if ((vf == NULL) || (value.get() == NULL))
    return ETCH_EINVAL;

  if (!value->getObjectType()->equals(EtchList<capu::SmartPointer<EtchObject> >::TYPE()))
    return ETCH_EINVAL;

  capu::SmartPointer<EtchList<capu::SmartPointer<EtchObject> > > list = capu::smartpointer_cast<EtchList<capu::SmartPointer<EtchObject> > >(value);

  EtchList<capu::SmartPointer<EtchObject> >::Iterator it = list->begin();
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchObject> > > values = new EtchNativeArray<capu::SmartPointer<EtchObject> > (list->size(), 1);
  capu::int32_t i = 0;
  capu::SmartPointer<EtchObject> tmp = NULL;
  while (it.hasNext()) {
    it.next(&tmp);
    values->set(Pos(i), tmp);
    i++;
  }
  result = new EtchStructValue(mType, vf);
  if (result->put(mField, values) != ETCH_OK)
    return ETCH_ERROR;
  ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "List has been serialized");
  return ETCH_OK;
}

status_t EtchListSerializer::importValue(EtchStructValue* value, capu::SmartPointer<EtchObject> &result) {
  if (value == NULL) {
    return ETCH_EINVAL;
  }
  if (!value->isType(mType)) {
    return ETCH_EINVAL;
  }

  capu::SmartPointer<EtchObject> tmp = NULL;
  if (value->get(mField, &tmp) != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Values Field could not be found");
    return ETCH_ERROR;
  }

  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchObject> > > values = capu::smartpointer_cast<EtchNativeArray<capu::SmartPointer<EtchObject> > >(tmp);

  EtchList<capu::SmartPointer<EtchObject> > *list = new EtchList<capu::SmartPointer<EtchObject> >();
  // TODO improve this
  for (capu::int32_t i = 0; i < values->getLength(); i++) {
    capu::SmartPointer<EtchObject> item = NULL;
    values->get(i, &item);
    status_t res = list->add(item);
    if (res != ETCH_OK)
      return res;
  }
  result = (EtchObject*) list;
  ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "List has been deserialized");
  return ETCH_OK;
}

status_t EtchListSerializer::Init(EtchRuntime* runtime, EtchType* type, EtchClass2TypeMap * class2type) {
  status_t result;

  EtchField field;
  result = type->getField(FIELD_NAME(), &field);
  if (result != ETCH_OK) {
    return result;
  }

  result = class2type->put(EtchNativeArray<capu::SmartPointer<EtchObject> >::TYPE(), type);
  if (result != ETCH_OK) {
    return result;
  }

  type->setComponentType(EtchList<capu::SmartPointer<EtchObject> >::TYPE());

  //set the import export helper
  // TODO memory EtchField
  type->setImportExportHelper(new EtchListSerializer(runtime, type, &field));

  //get validator
  capu::SmartPointer<EtchValidator> validator;
  result = EtchValidatorObject::Get(runtime, 1, validator);
  if (result != ETCH_OK) {
    return result;
  }
  result = type->putValidator(field, validator);
  if (result != ETCH_OK) {
    return result;
  }
  type->lock();

  ETCH_LOG_TRACE(runtime->getLogger(), runtime->getLogger().getSerializerContext(), "EtchListSerializer has been initialized");
  return ETCH_OK;
}
