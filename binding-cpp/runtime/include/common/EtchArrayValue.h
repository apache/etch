/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef __ETCHARRAYVALUE_H__
#define __ETCHARRAYVALUE_H__

#include "common/EtchObject.h"
#include "common/EtchNativeArray.h"
#include "serialization/EtchType.h"

/**
 * An array of values, where each value is of arbitrary type
 * chosen from the basic types boolean, byte, short, int,
 * long, float, double, String, an array of those, the extended
 * types ArrayValue and StructValue, and specific types supported
 * by ValueFactory.
 *
 * ArrayValue is not protected against concurrent access.
 */
class EtchArrayValue : public EtchObject {
public:

  /**
   * TypeId for ArrayValue.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructs the ArrayValue.
   * @param array
   * @param size
   * @param dim
   * @param customStructType
   * @param typeCode
   *
   */
  EtchArrayValue(capu::SmartPointer<EtchNativeArrayBase> array, capu::int32_t size, capu::int8_t typeCode, EtchType* customStructType, capu::int32_t dim);

  /**
   * Constructs an array value with no type info. This is used perhaps
   * by extern struct serializers.
   * @param array
   * @param size
   */
  EtchArrayValue(capu::SmartPointer<EtchNativeArrayBase> array, capu::int32_t size);

  /**
   * Constructs an array value copy with no type info. This is used perhaps
   * by extern struct serializers.
   * @param other Array Value
   */
  EtchArrayValue(const EtchArrayValue& other);

  virtual ~EtchArrayValue();

  /**
   * @return the TypeCode for this array value.
   * For example, if the array is int[][], then
   * the type would be TypeCode.INT4.
   */
  capu::int8_t getTypeCode();

  /**
   * @return a struct type if a custom type code.
   */
  EtchType* getCustomStructType();

  /**
   * @return the dimsionality of the array.
   * For example, if the array is int[][], the
   * dimensionality would be 2.
   */
  capu::int32_t getDim();

  /**
   * @return the capacity of the array.
   */
  capu::uint32_t getSize();

  /**
   * @return the number of elements in the array.
   */
  capu::int32_t getIndex();

  /**
   * @return the number of elements in the array.
   */
  void setIndex(capu::int32_t val);

  /**
   * @return true if two object is equal
   *         false otherwise
   */
  virtual capu::bool_t equals(const EtchObject * other) const;

  /**
   * @param index
   * @return the element at the specified index.
   */
  status_t get(capu::uint32_t index, capu::SmartPointer<EtchObject> &result);

  /**
   * Adds the value to the end of the array, making more space
   * available if needed.
   * @param value
   */
  status_t add(capu::SmartPointer<EtchObject> value);

  /**
   * @return the array value.
   */
  capu::SmartPointer<EtchNativeArrayBase> getArray();

private:
  capu::SmartPointer<EtchNativeArrayBase> mArray;
  capu::int8_t mTypeCode;
  EtchType* mCustomStructType;
  capu::int32_t mDim;
  capu::int32_t mAddIndex;
  capu::int32_t mSize;
};
#endif	/* ETCHARRAYVALUE_H */

