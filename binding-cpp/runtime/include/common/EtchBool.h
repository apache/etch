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

#ifndef __ETCHBOOL_H__
#define __ETCHBOOL_H__

#include "common/EtchObject.h"

class EtchBool :
public EtchObject {

public:

  /**
   * EtchObjectType for EtchBool.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructs a EtchBool object.
   */
  EtchBool();

  /**
   * Constructs a EtchBool object with given value.
   */
  EtchBool(capu::bool_t value);

  /**
   * Constructs a copy of EtchBool
   */
  EtchBool(const EtchBool& other);

  /**
   * Sets bool value.
   */
  void set(capu::bool_t value);

  /**
   * Returns bool value.
   */
  capu::bool_t get();

  /**
   * @return true if two object is equal
   *         false otherwise
   */
  capu::bool_t equals(const EtchObject * other) const;

  /**
   * Returns hash code
   */
  capu::uint32_t getHashCode() const;

private:

  capu::bool_t mValue;
};

typedef capu::SmartPointer<EtchBool> EtchBoolPtr;

#endif
