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

#ifndef __ETCHBYTE_H__
#define __ETCHBYTE_H__

#include "common/EtchNumber.h"
#include "common/EtchObjectType.h"
#include "common/EtchError.h"

class EtchByte :
public EtchNumber {

public:

  /**
   * EtchObjectType for EtchByte.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructs a EtchByte object.
   */
  EtchByte();

  /**
   * Constructs a EtchByte object with given value.
   */
  EtchByte(capu::int8_t value);

  /**
   * Constructs a copy of EtchByte.
   */
  EtchByte(const EtchByte &value);

  /**
   * Sets byte value.
   */
  void set(capu::int8_t value);

  /**
   * Returns byte value.
   */
  capu::int8_t get();

  /**
   * @return true if two object is equal
   *         false otherwise
   */
  capu::bool_t equals(const EtchObject * other) const;

  /**
   * Returns hash code
   */
  capu::uint32_t getHashCode() const;

  /**
   * Operator= for numeric Values
   */
  capu::int8_t& operator=(capu::int8_t const& other);

  /**
   * Operator= for EtchByte Objects
   */
  EtchByte& operator=(EtchByte const& other);

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

  capu::int8_t mValue;

};

typedef capu::SmartPointer<EtchByte> EtchBytePtr;

#endif
