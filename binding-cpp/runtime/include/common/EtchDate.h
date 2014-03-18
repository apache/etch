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

#ifndef ETCHDATE_H
#define ETCHDATE_H

#include "common/EtchObject.h"

class EtchDate : public EtchObject {
public:

  /**
   * EtchObjectType for EtchDate.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructor
   */
  EtchDate();

  /**
   * Constructor
   * @param value the time
   */
  EtchDate(capu::int64_t value);

  /**
   * Constructs a copy of EtchDate
   */
  EtchDate(const EtchDate& other);

  //Destructor
  virtual ~EtchDate();

  //overridden
  capu::bool_t equals(const EtchObject* other) const;

  /**
   * Sets time value.
   */
  void set(capu::int64_t value);

  /**
   * Returns time value.
   */
  capu::int64_t get();

  /**
   * Returns hash code
   */
  capu::uint32_t getHashCode() const;

private:

  capu::int64_t mValue;

};

typedef capu::SmartPointer<EtchDate> EtchDatePtr;

#endif /* ETCHDATE_H */

