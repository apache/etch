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

#ifndef __ETCHBINARYTAGGEDDATAINPUT_H__
#define __ETCHBINARYTAGGEDDATAINPUT_H__

#include "capu/os/StringUtils.h"
#include "serialization/EtchTaggedDataInput.h"
#include "serialization/EtchBinaryTaggedData.h"
#include "serialization/EtchValidatorInt.h"
#include "serialization/EtchValidatorString.h"
#include "serialization/EtchValidatorObject.h"

class EtchRuntime;

class EtchBinaryTaggedDataInput : public EtchBinaryTaggedData, public EtchTaggedDataInput {
public:

  /**
   * Constructs the BinaryTaggedDataInput with a null buffer.
   *
   * @param vf the value factory for the service.
   */
  EtchBinaryTaggedDataInput(EtchRuntime* runtime, EtchValueFactory *vf);

  /**
   * Destructor
   */
  virtual ~EtchBinaryTaggedDataInput();

  /////////////////////////////
  // TaggedDataInput methods //
  /////////////////////////////

  //overriden method
  status_t readMessage(capu::SmartPointer<EtchFlexBuffer> buf, capu::SmartPointer<EtchMessage> &message);

private:
  EtchRuntime* mRuntime;
  capu::SmartPointer<EtchFlexBuffer> mBuffer;
  capu::int32_t mLengthBudget;
  capu::SmartPointer<EtchValidator> mIntOrStrValidator;
  capu::SmartPointer<EtchValidator> mIntValidator;

  status_t readStruct(capu::SmartPointer<EtchStructValue> &result);
  status_t readArray(capu::SmartPointer<EtchValidator> v, EtchArrayValue *& result);
  status_t readKeysAndValues(capu::SmartPointer<EtchStructValue> sv);
  status_t readValues(EtchArrayValue *av, capu::SmartPointer<EtchValidator> v);
  status_t startMessage(capu::SmartPointer<EtchMessage> &result);
  status_t endMessage(capu::SmartPointer<EtchMessage> msg);
  status_t startStruct(capu::SmartPointer<EtchStructValue> &result);
  status_t endStruct(capu::SmartPointer<EtchStructValue> result);
  status_t startArray(EtchArrayValue *& result);
  status_t endArray(EtchArrayValue *array);
  status_t readType(EtchType *&type);
  status_t readField(EtchType *type, EtchField& field);
  status_t readLength(capu::int32_t& result);
  status_t readIntegerValue(capu::int32_t& result);

  ///////////////////////////
  // LOCAL UTILITY METHODS //
  ///////////////////////////
  status_t validate(capu::SmartPointer<EtchValidator> v, capu::SmartPointer<EtchObject> value);
  status_t validateValue(capu::SmartPointer<EtchValidator> v, capu::SmartPointer<EtchObject> value, capu::SmartPointer<EtchObject>& result);
  status_t validateValue(capu::SmartPointer<EtchValidator> v, capu::bool_t noneOk, capu::SmartPointer<EtchObject> value, capu::SmartPointer<EtchObject>& result);
  status_t readValue(capu::SmartPointer<EtchValidator> v, capu::SmartPointer<EtchObject>& result);
  status_t readValue(capu::SmartPointer<EtchValidator> v, capu::bool_t noneOk, capu::SmartPointer<EtchObject>& result);
  status_t readBytes(capu::int8_t*& array, capu::uint32_t &length);
};

#endif
