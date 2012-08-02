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

#include "support/EtchTransportHelper.h"
#include "support/EtchFreePool.h"
#include "support/EtchQueuedPool.h"

EtchString EtchTransportHelper::QUEUED_POOL("QUEUED_POOL");

EtchString EtchTransportHelper::FREE_POOL("FREE_POOL");

EtchString EtchTransportHelper::BINARY("binary");

EtchString EtchTransportHelper::XML("xml");

status_t EtchTransportHelper::initResources(EtchResources* resources, EtchResources** result) {
  if(result == NULL) {
    return ETCH_EINVAL;
  }
  if(resources == NULL) {
    resources = new EtchResources();
  } else {
    resources = new EtchResources(*resources);
  }

  EtchObject* obj = NULL;
  EtchObject* objOld = NULL;
  // Queued Pool
  if(!resources->containsKey(EtchTransportHelper::QUEUED_POOL)) {
    obj = new EtchQueuedPool();
    // TODO: change interface to give NULL as return value
    resources->put(EtchTransportHelper::QUEUED_POOL, obj, objOld );
  }
  // Free Pool
  if(!resources->containsKey(EtchTransportHelper::FREE_POOL)) {
    obj = new EtchFreePool();
    // TODO: change interface to give NULL as return value
    resources->put( EtchTransportHelper::FREE_POOL, obj, objOld);
  }
  *result = resources;
  return ETCH_OK;
}
