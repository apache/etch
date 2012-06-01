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


const EtchString EtchHashTableSerializer::FIELD_NAME("keysAndValues");

EtchHashTableSerializer::EtchHashTableSerializer(EtchType* type, EtchField* field)
: mField(*field), mType(type) {

}

EtchHashTableSerializer::~EtchHashTableSerializer() {

}

status_t EtchHashTableSerializer::exportValue(EtchValueFactory* vf, capu::SmartPointer<EtchObject> value, EtchStructValue*& result) {
  if ((value.get() == NULL) || (vf == NULL))
    return ETCH_EINVAL;
  if (value->getObjectType() != EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> >::TYPE())
    return ETCH_EINVAL;

  capu::SmartPointer<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > table = capu::smartpointer_cast<EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> > > (value);

  EtchHashTable<capu::SmartPointer<EtchObject> , capu::SmartPointer<EtchObject> >::Iterator it = table->begin();
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchObject> > > keysAndValuesArray = new EtchNativeArray<capu::SmartPointer<EtchObject> > (table->count()*2);
  capu::int32_t i = 0;
  EtchHashTable<capu::SmartPointer<EtchObject> , capu::SmartPointer<EtchObject> >::Pair ptr;

  while (it.hasNext()) {
    it.next(&ptr);
    keysAndValuesArray->set(i, ptr.first);
    i++;
    keysAndValuesArray->set(i, ptr.second);
    i++;
  }

  result = new EtchStructValue(mType, vf);
  if (result->put(mField, keysAndValuesArray) != ETCH_OK) {
    return ETCH_ERROR;
  }

  return ETCH_OK;
}

status_t EtchHashTableSerializer::importValue(EtchStructValue* value, capu::SmartPointer<EtchObject> &result) {
  if (value == NULL) {
    return ETCH_EINVAL;
  } else if (!value->isType(mType)) {
    return ETCH_EINVAL;
  }
  capu::SmartPointer<EtchObject> tmp;
  if (value->get(mField, &tmp) != ETCH_OK)
    return ETCH_ERROR;

  EtchHashTable<capu::SmartPointer<EtchObject> , capu::SmartPointer<EtchObject> >* table = new EtchHashTable<capu::SmartPointer<EtchObject> , capu::SmartPointer<EtchObject> >();

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
  return ETCH_OK;
}

status_t EtchHashTableSerializer::Init(EtchType* type, EtchClass2TypeMap* class2type) {
  status_t result;
  EtchField field_ptr;
  result = type->getField(FIELD_NAME, &field_ptr);
  if (result != ETCH_OK)
    return result;
  class2type->put(EtchNativeArray<capu::SmartPointer<EtchObject> >::TYPE(), type);
  type->setComponentType(EtchHashTable<capu::SmartPointer<EtchObject>, capu::SmartPointer<EtchObject> >::TYPE());
  //set the import export helper
  type->setImportExportHelper(new EtchHashTableSerializer(type, &field_ptr));
  capu::SmartPointer<EtchValidator> tmp;
  result = EtchValidatorObject::Get(1, tmp);
  if (result != ETCH_OK)
    return result;
  result = type->putValidator(field_ptr, tmp);
  if (result != ETCH_OK)
    return result;
  type->lock();
  return ETCH_OK;
}