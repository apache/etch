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

#ifndef __ETCHLONG_H__
#define __ETCHLONG_H__

#include "common/EtchNumber.h"

class EtchLong :
public EtchNumber {

public:

  /**
   * EtchObjectType for EtchLong.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructs a EtchLong object.
   */
  EtchLong();

  /**
   * Constructs a EtchLong object with given value.
   */
  EtchLong(capu::int64_t value);

  /**
   * Constructs a copy of EtchLong.
   */
  EtchLong(const EtchLong& other);


  /**
   * Sets long value.
   */
  void set(capu::int64_t value);

  /**
   * Returns long value.
   */
  capu::int64_t get();

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

  capu::int64_t mValue;

};

typedef capu::SmartPointer<EtchLong> EtchLongPtr;

#endif
