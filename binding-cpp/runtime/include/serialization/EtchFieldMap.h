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

#ifndef __ETCHFIELDMAP_H__
#define __ETCHFIELDMAP_H__

#include "common/EtchString.h"
#include "common/EtchHashTable.h"
#include "common/EtchComparatorNative.h"
#include "common/EtchHashNative.h"
#include "serialization/EtchField.h"

/**
 * Mapping of id and name to EtchField
 */
class EtchFieldMap {
public:

  /**
   * Constructor
   */
  EtchFieldMap();

  /**
   * Destructor
   */
  virtual ~EtchFieldMap();

  /**
   * Gets the EtchField subclass which corresponds to the specified id.
   * @param id the id of an EtchField.
   * @param field Field that will be get
   * @return  ETCH_OK if get is successful performed
   *          ETCH_EINVAL if field is null
   *          ETCH_ENOT_EXIST if there is no pair with specified id
   */
  status_t get(capu::int32_t id, EtchField* field);

  /**
   * Gets the EtchField subclass which corresponds to the specified
   * name, or creates it if it isn't found and if this map is not
   * locked.
   * @param name the name of an EtchField.
   * @param field Field that will be get
   * @return ETCH_OK if get is successful performed
   *         ETCH_EINVAL if field is null
   *         ETCH_ENOT_EXIST if there is no pair with specified name
   */
  status_t get(EtchString name, EtchField* field);

  /**
   * Adds the EtchField subclass to the map.
   *
   * @param t the EtchField subclass to add.
   *
   * @return ETCH_EINVAL if it is locked
   *         ETCH_ERROR  if there is an error
   *         ETCH_OK otherwise
   *
   * existing entry by id or name.
   */
  status_t add(EtchField t);

  /**
   * Locks the map preventing further changes.
   */
  void lock();

  /**
   * @return the number of values in the map.
   */
  capu::int32_t size();


  /**
   * @return the iterator
   */
  EtchHashTable<EtchString, EtchField>::Iterator begin();

private:

  EtchHashTable<capu::int32_t, EtchField, EtchComparatorNative, EtchHashNative> mById;

  EtchHashTable<EtchString, EtchField> mByName;

  capu::bool_t mLocked;
};

#endif /* ETCHFIELDCOLLECTION_H */
