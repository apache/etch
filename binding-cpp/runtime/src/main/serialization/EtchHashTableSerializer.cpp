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


#include "serialization/EtchHashTableSerializer.h"
#include "support/EtchRuntime.h"

const EtchString& EtchHashTableSerializer::FIELD_NAME() {
  static const EtchString name("keysAndValues");
  return name;
}

EtchHashTableSerializer::EtchHashTableSerializer(EtchRuntime* runtime, EtchType* type, EtchField& field)
: EtchImportExportHelper(runtime), mField(field), mType(type) {
}

EtchHashTableSerializer::~EtchHashTableSerializer() {
}

status_t EtchHashTableSerializer::exportValue(EtchValueFactory* vf, capu::SmartPointer<EtchObject> value, EtchStructValue*& result) {
  if ((value.get() == NULL) || (vf == NULL))
    return ETCH_EINVAL;
  if (value->getObjectType() != EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> >::TYPE())
    return ETCH_EINVAL;

  capu::SmartPointer<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > table = capu::smartpointer_cast<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > (value);

  EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> >::Iterator it = table->begin();
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchObject> > > keysAndValuesArray = new EtchNativeArray<capu::SmartPointer<EtchObject> > (table->count()*2);
  capu::int32_t i = 0;
  EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> >::HashTableEntry entry;

  while (it.hasNext()) {
    it.next(&entry);
    keysAndValuesArray->set(i, entry.key);
    i++;
    keysAndValuesArray->set(i, entry.value);
    i++;
  }

  result = new EtchStructValue(mType, vf);
  if (result->put(mField, keysAndValuesArray) != ETCH_OK) {
    return ETCH_ERROR;
  }
  ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "HashTable has been serialized");
  return ETCH_OK;
}

status_t EtchHashTableSerializer::importValue(EtchStructValue* value, capu::SmartPointer<EtchObject> &result) {
  if (value == NULL) {
    return ETCH_EINVAL;
  } else if (!value->isType(mType)) {
    return ETCH_EINVAL;
  }
  capu::SmartPointer<EtchObject> tmp;
  if (value->get(mField, &tmp) != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "KeysAndValues Field could not be found");
    return ETCH_ERROR;
  }

  EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> >* table = new EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> >();

  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchObject> > > keysAndValuesArray = capu::smartpointer_cast<EtchNativeArray<capu::SmartPointer<EtchObject> > > (tmp);

  capu::SmartPointer<EtchObject> key;
  capu::SmartPointer<EtchObject> hashValue;
  capu::int32_t i = 0;
  while (i < keysAndValuesArray->getLength()) {
    keysAndValuesArray->get(i, &key);
    i++;
    keysAndValuesArray->get(i, &hashValue);
    i++;
    status_t res = table->put(key, hashValue);
    if (res != ETCH_OK)
      return res;
  }
  result = (EtchObject*) table;
  ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "HashTable has been deserialized");
  return ETCH_OK;
}

status_t EtchHashTableSerializer::Init(EtchRuntime* runtime, EtchType* type, EtchClass2TypeMap* class2type) {
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

  type->setComponentType(EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> >::TYPE());

  //set the import export helper
  type->setImportExportHelper(new EtchHashTableSerializer(runtime, type, field));

  //get validator
  capu::SmartPointer<EtchValidator> tmp;
  result = EtchValidatorObject::Get(runtime, 1, tmp);
  if (result != ETCH_OK) {
    return result;
  }
  result = type->putValidator(field, tmp);
  if (result != ETCH_OK) {
    return result;
  }

  type->lock();
  ETCH_LOG_TRACE(runtime->getLogger(), runtime->getLogger().getSerializerContext(), "EtchHashTableSerializer has been initialized");
  return ETCH_OK;
}
