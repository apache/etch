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

#ifndef __ETCHSHORT_H__
#define __ETCHSHORT_H__

#include "common/EtchNumber.h"

class EtchShort :
public EtchNumber {

public:

  /**
   * EtchObjectType for EtchShort.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructs a EtchShort object.
   */
  EtchShort();

  /**
   * Constructs a EtchShort object with given value.
   */
  EtchShort(capu::int16_t value);

  /**
   * Constructs a copy of EtchShort.
   */
  EtchShort(const EtchShort& value);

  /**
   * Sets short value.
   */
  void set(capu::int16_t value);

  /**
   * Returns short value.
   */
  capu::int16_t get();

  /**
   * Returns hash code
   */
  capu::uint32_t getHashCode() const;

  /**
   * @return true if two object is equal
   *         false otherwise
   */
  capu::bool_t equals(const EtchObject * other) const;

  /**
   * @see EtchNumber
   */
  capu::int64_t getLongValue() const;

  /**
   * @see EtchNumber
   */
  capu::int32_t getInt32Value() const;

  /**
   * @see EtchNumber
   */
  capu::int16_t getShortValue() const;

  /**
   * @see EtchNumber
   */
  capu::int8_t getByteValue() const;

private:

  capu::int16_t mValue;

};

typedef capu::SmartPointer<EtchShort> EtchShortPtr;

#endif
