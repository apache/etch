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

#include "serialization/EtchSetSerializer.h"

const EtchString EtchSetSerializer::FIELD_NAME("keys");

EtchSetSerializer::EtchSetSerializer(EtchType* type, EtchField* field)
: mField(*field), mType(type) {

}

EtchSetSerializer::~EtchSetSerializer() {

}

status_t EtchSetSerializer::exportValue(EtchValueFactory* vf, capu::SmartPointer<EtchObject> value, EtchStructValue*& result) {
  if ((value.get() == NULL) || (vf == NULL))
    return ETCH_EINVAL;
  if (value->getObjectType() != EtchHashSet<capu::SmartPointer<EtchObject> >::TYPE())
    return ETCH_EINVAL;

  capu::SmartPointer<EtchHashSet<capu::SmartPointer<EtchObject> > > set = capu::smartpointer_cast<EtchHashSet<capu::SmartPointer<EtchObject> > > (value);

  EtchHashSet<capu::SmartPointer<EtchObject> >::Iterator it = set->begin();
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchObject> > > array = new EtchNativeArray<capu::SmartPointer<EtchObject> > (1, set->count());
  capu::int32_t i = 0;
  capu::SmartPointer<EtchObject> ptr;

  while (it.hasNext()) {
    it.next(&ptr);
    array->set(i, ptr);
    i++;
  }

  result = new EtchStructValue(mType, vf);
  if (result->put(mField, array) != ETCH_OK)
    return ETCH_ERROR;

  return ETCH_OK;
}

status_t EtchSetSerializer::importValue(EtchStructValue* value, capu::SmartPointer<EtchObject> &result) {
  if (value == NULL) {
    return ETCH_EINVAL;
  } else if (!value->isType(mType)) {
    return ETCH_EINVAL;
  }
  capu::SmartPointer<EtchObject> tmp;
  if (value->get(mField, &tmp) != ETCH_OK)
    return ETCH_ERROR;

  EtchHashSet<capu::SmartPointer<EtchObject> >* set = new EtchHashSet<capu::SmartPointer<EtchObject> >();

  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchObject> > > array = capu::smartpointer_cast<EtchNativeArray<capu::SmartPointer<EtchObject> > > (tmp);

  capu::SmartPointer<EtchObject> ptr;
  for (capu::int32_t i = 0; i < array->getLength(); i++) {
    array->get(i, &ptr);
    status_t res = set->put(ptr);
    if (res != ETCH_OK)
      return res;
  }
  result = (capu::SmartPointer<EtchObject>) set;
  return ETCH_OK;
}

status_t EtchSetSerializer::Init(EtchType* type, EtchClass2TypeMap* class2type) {
  status_t result;
  EtchField field_ptr;
  result = type->getField(FIELD_NAME, &field_ptr);
  if (result != ETCH_OK)
    return result;
  class2type->put(EtchNativeArray<capu::SmartPointer<EtchObject> >::TYPE(), type);
  type->setComponentType(EtchHashSet<capu::SmartPointer<EtchObject> >::TYPE());
  //set the import export helper
  type->setImportExportHelper(new EtchSetSerializer(type, &field_ptr));
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
