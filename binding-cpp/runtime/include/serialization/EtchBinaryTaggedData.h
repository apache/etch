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

#ifndef __ETCHBINARYTAGGEDDATA_H__
#define __ETCHBINARYTAGGEDDATA_H__

#include "common/EtchTypes.h"
#include "serialization/EtchTypeCodes.h"
#include "serialization/EtchTaggedData.h"

class EtchBinaryTaggedData : public EtchTaggedData {
public:

  /**
   * This is the current version of the protocol.
   */
  const static capu::int8_t& VERSION()
  {
    static const capu::int8_t version(3);
    return version;
  }

  /**
   * sentinel that marks the end of array or struct element
   */
  static capu::SmartPointer<EtchObject>& NONE();

  /**
   * Constructs the BinaryTaggedData.
   *
   * @param vf ValueFactory
   */
  EtchBinaryTaggedData(EtchValueFactory* vf);

  /**
   * Destructor
   */
  virtual ~EtchBinaryTaggedData();

  /**
   * @param value
   * @return return a type code for the specified value.
   */
  capu::int8_t checkValue(capu::SmartPointer<EtchObject> value);

  /**
   * Returns the type code for the specified class. This
   * is needed when we have an array and we have determine
   * the base type and now we're fixing to serialize it.
   * We use the base type code to reconstitute it on the
   * other side. So we don't return a perfect code the way
   * checkValue does, but rather just something that let's
   * us recreate the appropriate array type on import.
   * @param c
   * @return a type code for the specified class.
   */
  capu::int8_t getNativeTypeCode(const EtchObjectType *c);

  /**
   * @param c           typeid
   * @param result      out buffet
   */
  status_t getCustomStructType(const EtchObjectType* c, EtchType*& result);

  /**
   * @param type
   * @return the class for a specified (native) type code.
   */
  const EtchObjectType* getNativeType(capu::int8_t type);

private:

  static capu::int8_t checkByte(capu::int8_t v);
  static capu::int8_t checkShort(capu::int16_t v);
  static capu::int8_t checkInteger(capu::int32_t v);
  static capu::int8_t checkLong(capu::int64_t v);

};

#endif /* ETCHBINARYTAGGEDDATA_H */

