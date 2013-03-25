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

#ifndef __ETCHTYPEMAP_H__
#define __ETCHTYPEMAP_H__

#include "common/EtchString.h"
#include "common/EtchHashTable.h"
#include "common/EtchHashSet.h"
#include "common/EtchComparatorNative.h"
#include "common/EtchHashNative.h"
#include "serialization/EtchType.h"

class EtchTypeMap {
public:

  /**
   * Constructor
   */
  EtchTypeMap();

  /**
   * Destructor
   */
  virtual ~EtchTypeMap();

  /**
   * Gets the EtchType subclass which corresponds to the specified id.
   * @param id the id of an Type.
   * @param type  type which is found in collection with respect to id.
   * @return ETCH_OK if get is successful performed
   *         ETCH_EINVAL if type is null
   *         ETCH_ENOT_EXIST if there is no pair with specified id
   */
  status_t get(capu::int32_t id, EtchType *&type);

  /**
   * Gets the EtchType subclass which corresponds to the specified
   * name, or creates it if it isn't found and if this map is not
   * locked.
   * @param name the name of an Type.
   * @param type type which is found in collection with respect to name or created.
   * @return ETCH_OK if get is successful performed
   *         ETCH_EINVAL if type is null
   *         ETCH_ENOT_EXIST if there is no pair with specified id
   */
  status_t get(EtchString name, EtchType *&type);

  /**
   * Adds the EtchType subclass to the map.
   *
   * @param t the EtchType subclass to add.
   *
   * @return ETCH_EINVAL if it is locked
   *         ETCH_ERROR  if there is an error
   *         ETCH_OK otherwise
   *
   * existing entry by id or name.
   */
  status_t add(EtchType *type);

  /**
   * Locks the map preventing further changes.
   */
  void lock();

  /**
   * @return the number of values in the map.
   */
  capu::int32_t size();

  /**
   * all of types element added to collection
   * @param set hashset where the values will be put
   * @return ETCH_OK if successfully completed
   *         ETCH_EINVAL if set is null
   */
  status_t getAll(EtchHashSet<EtchType*> *set);


  //clear deallocates every element in type collection
  void clear();

private:

  EtchHashTable<capu::int32_t, EtchType*, EtchComparatorNative, EtchHashNative > mById;

  EtchHashTable<EtchString, EtchType* > mByName;

  capu::bool_t mLocked;

};


#endif /* ETCHTYPEMAP_H */

