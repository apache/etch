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

#ifndef __ETCHTAGGEDDATA_H__
#define __ETCHTAGGEDDATA_H__
#include "serialization/EtchValueFactory.h"
#include "serialization/EtchValidatorStructValue.h"
#include "serialization/EtchTypeCodes.h"
#include "common/EtchArrayValue.h"
#include "common/EtchNativeArray.h"

class EtchTaggedData {
protected:

  /**
   * The value factory to use for tagged input and output.
   */
  EtchValueFactory* mVf;

public:

  //Constructor
  EtchTaggedData(EtchValueFactory* vf);

  //Destructor
  virtual ~EtchTaggedData();

  /**
   * @return the value factory to use for tagged input and output.
   */

  EtchValueFactory* getValueFactory();

protected:

  /**
   * @param typeCode
   * @param customStructType
   * @param dim
   * @param result the allocated array value.
   * @return ETCH_OK if array is successfully allocated
   *         ETCH_ENO_MEMORY if mem allocation fails
   *
   */
  status_t allocNativeArray(capu::int8_t typeCode, EtchType* customStructType, capu::int32_t dim, capu::int32_t length, capu::SmartPointer<EtchNativeArrayBase> &result);

  /**
   * @param av
   * @param result native array converted from an array value.
   * @return ETCH_OK if successfully converted
   *         ETCH_EINVAL if av is NULL
   */

  status_t fromArrayValue(EtchArrayValue* av, capu::SmartPointer<EtchObject> &result);


  /**
   * @param value
   * @param v
   * @param result an ArrayValue which models the given array.
   * @return ETCH_OK if successfully converted
   *         ETCH_EINVAL if value or v is NULL
   *         ETCH_ERROR otherwise
   */
  status_t toArrayValue(capu::SmartPointer<EtchObject> value, EtchValidator* v, EtchArrayValue*& result);

private:

  const EtchObjectType* getComponentType(capu::int8_t typeCode, EtchType* customStructType,
          capu::int32_t dim);

  status_t findStructValueValidator(EtchValidator* v, EtchValidatorStructValue*& result);

public:
  /**
   * @param value
   * @return return a type code for the specified value.
   */
  virtual capu::int8_t checkValue(capu::SmartPointer<EtchObject> value) = 0;

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
  virtual capu::int8_t getNativeTypeCode(const EtchObjectType *c) = 0;

  /**
   * @param c           typeid
   * @param result      out buffet
   */
  virtual status_t getCustomStructType(const EtchObjectType* c, EtchType*& result) = 0;

  /**
   * @param type
   * @return the class for a specified (native) type code.
   */
  virtual const EtchObjectType* getNativeType(capu::int8_t type) = 0;

};

#endif /* ETCHTAGGEDDATA_H */

