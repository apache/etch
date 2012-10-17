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

#ifndef __ETCHRESOURCES_H__
#define __ETCHRESOURCES_H__
#include "common/EtchHashTable.h"
#include "common/EtchObject.h"
#include "capu/util/SmartPointer.h"
#include "common/EtchString.h"

class EtchResources {
public:

  /**
   * Constructs the Resources.
   */
  EtchResources();

  /**
   * Constructs the Resources.
   * @param related another EtchResources which has
   */
  EtchResources(EtchResources * related);

  /**
   * Destructor
   */
  ~EtchResources();

  /**
   * Returns true if this Resources has the key.
   * @param key the string name of a resource.
   * @return true if this Resources has the key
   *         false otherwise.
   */
  capu::bool_t containsKey(const EtchString& key);

  /**
   * Returns the named resource from this Resources
   * @param key the string name of a resource.
   *
   * @return a pointer to the object with given key
   *         if there is no element with given key return a NULL pointer
   */
  status_t get(const EtchString& key, EtchObject*& result);

  /**
   * Puts the named resource into this Resources.
   * @param key the string name of a resource.
   * @param value the value of the resource.
   * @return a pointer to the overwritten element
   *         if there is no existing element with same key returns a NULL pointer
   */
  status_t put(const EtchString& key, EtchObject *value, EtchObject*& result);

  /**
   * Removes the named resource from this Resources only.
   * @param key the string name of a resource.
   * @return a pointer to the removed element
   *         if there is no element with given key return a NULL pointer
   */
  status_t remove(const EtchString& key, EtchObject*& result);

  /**
   * @return related EtchResources
   */
  EtchResources* getRelated();
private:

  EtchResources *mRelated;
  EtchHashTable <EtchString, EtchObject*> mRes;

};

#endif /* ETCHRESOURCES_H */

