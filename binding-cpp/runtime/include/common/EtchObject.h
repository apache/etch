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

#include "capu/container/List.h"
#include "capu/container/Hash.h"

#include "common/EtchError.h"
#include "common/EtchObjectType.h"

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
   * Copy Constructor.
   */
  EtchObject(const EtchObject& other);

  /**
   * Destructor.
   */
  virtual ~EtchObject();

  /**
   */
  virtual const EtchObjectType* getObjectType() const;

  /**
   * Returns true if the type hierarchy of this instance
   * is from the given type.
   * @param type to check
   * @return true if the instance is compatible to the given type
   */
  virtual capu::bool_t isInstanceOf(const EtchObjectType* type) const;

  /**
   * Returns hash code
   */
  virtual capu::uint32_t getHashCode() const;

  /**
   * @return true if two object is equal
   *         false otherwise
   */
  virtual capu::bool_t equals(const EtchObject * other) const;

protected:
  /**
   * Adds the object type to the type hierachy of this instance.
   * @param type that should be added to type hierachy of this instance.
   */
  virtual status_t addObjectType(const EtchObjectType* type);

private:
  capu::List<const EtchObjectType*> mTypes;

};

typedef capu::SmartPointer<EtchObject> EtchObjectPtr;

#endif
