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

#ifndef ETCHFIELD_H
#define ETCHFIELD_H
#include "capu/Config.h"
#include "common/EtchString.h"
#include "common/EtchInt32.h"

class EtchField : public EtchObject {
public:

  /**
   * EtchObjectType for EtchField.
   */
  static const EtchObjectType* TYPE();

  /**
   * Default Constructor
   */
  EtchField();

  /**
   * Constructor
   * @param name
   */
  EtchField(const EtchString & name);


  /**
   * Constructor
   * @param id
   * @param name
   */
  EtchField(capu::uint32_t id, EtchString & name);

  /**
   * Copy Constructor
   * @param other
   */
  EtchField(const EtchField& other);

  /**
   * Destructor
   */
  virtual ~EtchField();

  /**
   * returns the hash code
   * @return hash value
   */
  capu::uint32_t getHashCode() const;

  /**
   * Compares two fields
   * @return true if two fields are equal
   *         false otherwise
   */
  capu::bool_t equals(const EtchObject * object) const;

  /**
   * @return id of the field
   */
  capu::uint32_t getId() const;

  /**
   * @return name of the field
   */
  EtchString getName() const;

private:
  EtchInt32 mId;
  EtchString mName;
};

#endif /* ETCHFIELD_H */

