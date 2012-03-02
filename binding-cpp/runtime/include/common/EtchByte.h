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

#include "common/EtchObject.h"
#include "common/EtchError.h"

class EtchByte :
public EtchObject {

public:

  /**
   * TypeId for EtchByte.
   */
  static const EtchObjectType TYPE;

  /**
   * Constructs a EtchByte object.
   */
  EtchByte();

  /**
   * Constructs a EtchByte object with given value.
   */
  EtchByte(capu::int8_t value);

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
  capu::uint64_t getHashCode() const;

private:

  capu::int8_t mValue;

};

#endif
