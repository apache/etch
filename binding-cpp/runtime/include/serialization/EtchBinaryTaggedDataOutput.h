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

#ifndef __ETCHBINARYTAGGEDDATAOUTPUT_H__
#define __ETCHBINARYTAGGEDDATAOUTPUT_H__

#include "util/EtchURL.h"
#include "serialization/EtchTaggedDataOutput.h"
#include "serialization/EtchBinaryTaggedData.h"
#include "serialization/EtchValidatorInt.h"
#include "serialization/EtchValidatorString.h"
#include "serialization/EtchValidatorNone.h"

class EtchRuntime;

class EtchBinaryTaggedDataOutput : public EtchTaggedDataOutput, public EtchBinaryTaggedData {
public:

  /**
   * Name of uri parameter which controls whether we write ints or strings
   * for types and fields.
   */
  static const EtchString& STRING_TYPE_AND_FIELD();

  /**
   * Constructs the BinaryTaggedDataInput with a null buffer.
   *
   * @param vf the value factory for the service.
   * @param uri.
   */
  EtchBinaryTaggedDataOutput(EtchRuntime* runtime, EtchValueFactory *vf, EtchURL* uri);

  /**
   * Destructor
   */
  virtual ~EtchBinaryTaggedDataOutput();

  //////////////////////////////
  // TaggedDataOutput methods //
  //////////////////////////////
  //overriden method
  status_t writeMessage(capu::SmartPointer<EtchMessage> msg, capu::SmartPointer<EtchFlexBuffer> buf);

private:
  EtchRuntime* mRuntime;
  EtchLevel mLevel;
  capu::SmartPointer<EtchFlexBuffer> mBuffer;
  capu::bool_t mStringTypeAndField;
  capu::SmartPointer<EtchValidator> mIntValidator;
  capu::SmartPointer<EtchValidator> mStringValidator;
  capu::SmartPointer<EtchValidator> mNoneValidator;

  status_t writeStruct(capu::SmartPointer<EtchStructValue> sv);
  status_t writeArray(EtchArrayValue* av, EtchValidator* v);
  status_t writeKeysAndValues(capu::SmartPointer<EtchStructValue> sv);
  status_t writeValues(EtchArrayValue* av, EtchValidator* v);

  /////////////////////////
  // Main output methods //
  /////////////////////////

  status_t startMessage(capu::SmartPointer<EtchMessage> msg);
  status_t endMessage(capu::SmartPointer<EtchMessage> msg);
  status_t startStruct(capu::SmartPointer<EtchStructValue> _struct);
  status_t endStruct(capu::SmartPointer<EtchStructValue> _struct);
  status_t startArray(EtchArrayValue* array);
  status_t endArray(EtchArrayValue* array);
  status_t writeType(EtchType* type);
  status_t writeField(EtchField* field);
  status_t writeIntValue(capu::int32_t value);
  status_t writeStringValue(capu::SmartPointer<EtchString> value);
  status_t writeNoneValue();
  status_t writeValue(capu::SmartPointer<EtchValidator> v, capu::SmartPointer<EtchObject> value);
  status_t writeBytes( capu::int8_t* value, capu::int32_t size);
};

#endif /* ETCHBINARYTAGGEDDATAOUTPUT_H */

