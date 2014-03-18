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

#ifndef __ETCHINT32_H__
#define __ETCHINT32_H__

#include "common/EtchNumber.h"

class EtchInt32 :
public EtchNumber {

public:

  /**
   * EtchObjectType for EtchString.
   */
  static const EtchObjectType* TYPE();


  /**
   * Constructs a EtchInt32 object.
   */
  EtchInt32();

  /**
   * Constructs a EtchInt32 object with given value.
   */
  EtchInt32(capu::int32_t value);

  /**
   * Constructs a EtchInt32 copy
   */
  EtchInt32(const EtchInt32& other);


  /**
   * Destructor.
   */
  virtual ~EtchInt32();

  /**
   * Sets int32 value.
   */
  void set(capu::int32_t value);

  /**
   * Returns int32 value.
   */
  capu::int32_t get() const;

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
   * assigns an integer number to the stored value
   */
  capu::int32_t& operator=(capu::int32_t const& other);

  /**
   * assigns an EtchInt32 to an EtchInt32
   */
  EtchInt32& operator=(EtchInt32 const& other);

  /**
   * increments and assigns the stored integer value
   */
  capu::int32_t& operator++();

  /**
   * assigns and increments the stored integer value
   */
  capu::int32_t operator++(int);

  /**
   * decrement and assigns the stored integer value
   */
  capu::int32_t& operator--();

  /**
   * assigns and decrement the stored integer value
   */
  capu::int32_t operator--(int);

  /**
   * returns true if two objects are equal
   */
  capu::bool_t operator==(const EtchObject& other) const;

  /**
   * returns true if two objects are not equal
   */
  capu::bool_t operator!=(const EtchObject& other) const;

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
  capu::int32_t mValue;

};

typedef capu::SmartPointer<EtchInt32> EtchInt32Ptr;

#endif
