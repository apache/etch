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

#include "util/EtchResources.h"

EtchResources::EtchResources() {

}

EtchResources::~EtchResources() {

}

capu::bool_t EtchResources::containsKey(const EtchString& key) {
  EtchObject* ptr = NULL;
  if (res.get(key, &ptr) == ETCH_OK)
    return true;
  else
    return false;
}

status_t EtchResources::get(const EtchString& key, EtchObject** result) {
  return res.get(key, result);
}

status_t EtchResources::put(const EtchString& key, EtchObject* value, EtchObject** result) {
  return res.put(key, value, result);
}

status_t EtchResources::remove(const EtchString& key, EtchObject** result) {

  return res.remove(key, result);
}