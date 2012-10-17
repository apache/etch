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

#ifndef __ETCHDOUBLE_H__
#define __ETCHDOUBLE_H__

#include "common/EtchObject.h"

class EtchDouble :
public EtchObject {

public:

  /**
   * EtchObjectType for EtchDouble.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructs a EtchDouble object.
   */
  EtchDouble();

  /**
   * Constructs a EtchDouble object with given value.
   */
  EtchDouble(capu::double_t value);

  /**
   * Constructs a copy of EtchDouble.
   */
  EtchDouble(const EtchDouble& other);

  /**
   * Sets double value.
   */
  void set(capu::double_t value);

  /**
   * Returns double value.
   */
  capu::double_t get();

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

  capu::double_t mValue;

};

typedef capu::SmartPointer<EtchDouble> EtchDoublePtr;

#endif
