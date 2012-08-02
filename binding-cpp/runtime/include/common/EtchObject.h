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

#ifndef __ETCHOBJECT_H__
#define __ETCHOBJECT_H__

#include "capu/util/SmartPointer.h"
#include "EtchConfig.h"

class EtchObjectType;

class EtchObject {
public:

  /**
   * EtchObjectType for EtchObject.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructor.
   */
  EtchObject();

  /**
   * Constructor.
   */
  EtchObject(const EtchObjectType* type);

  /**
   * Destructor.
   */
  virtual ~EtchObject();

  /**
  * Returns object type id of this type.
  */
  const EtchObjectType* GetObjectType() const;

  /**
   * Returns object type id of this object.
   */
  const EtchObjectType* getObjectType() const;

  /**
   * Returns hash code
   */
  virtual capu::uint32_t getHashCode() const;

  /**
   * @return true if two object is equal
   *         false otherwise
   */
  virtual capu::bool_t equals(const EtchObject * other) const;

private:

  const EtchObjectType* mType;

};

typedef capu::SmartPointer<EtchObject> EtchObjectPtr;

#endif
