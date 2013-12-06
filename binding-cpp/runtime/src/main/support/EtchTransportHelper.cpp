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

#include "support/EtchFreePool.h"
#include "support/EtchQueuedPool.h"
#include "support/EtchTransportHelper.h"
#include "support/EtchRuntime.h"
#include "util/EtchLogger.h"


const EtchString& EtchTransportHelper::QUEUED_POOL() {
  static const EtchString pool("QUEUED_POOL");
  return pool;
}

const EtchString& EtchTransportHelper::FREE_POOL() {
  static const EtchString pool("FREE_POOL");
  return pool;
}

const EtchString& EtchTransportHelper::BINARY() {
  static const EtchString type("binary");
  return type;
}
const EtchString& EtchTransportHelper::XML() {
  static const EtchString type("xml");
  return type;
}

status_t EtchTransportHelper::InitResources(EtchRuntime* runtime, EtchResources* resources, EtchResources*& result) {
  if(resources == NULL) {
    resources = new EtchResources();
  } else {
    resources = new EtchResources(*resources);
  }

  EtchObject* obj = NULL;
  EtchObject* objOld = NULL;
  // Queued Pool
  if(!resources->containsKey(EtchTransportHelper::QUEUED_POOL())) {
    obj = new EtchQueuedPool();
    // TODO: change interface to give NULL as return value
    resources->put(EtchTransportHelper::QUEUED_POOL(), obj, objOld );
  }
  // Free Pool
  if(!resources->containsKey(EtchTransportHelper::FREE_POOL())) {
    obj = new EtchFreePool();
    // TODO: change interface to give NULL as return value
    resources->put( EtchTransportHelper::FREE_POOL(), obj, objOld);
  }
  result = resources;
  ETCH_LOG_TRACE(runtime->getLogger(), runtime->getLogger().getRuntimeContext(), "Resources and pools have been allocated initialized");

  return ETCH_OK;
}

status_t EtchTransportHelper::DestroyResources(EtchRuntime* runtime, EtchResources* resources) {
  status_t result;
  if (resources == NULL) {
    return ETCH_EINVAL;
  } else {

    EtchObject* returnValue = NULL;

    //get queued pool and delete it
    result = resources->get(EtchTransportHelper::QUEUED_POOL(), returnValue);
    if(result == ETCH_OK && returnValue != NULL) {
      //TODO: Add queue handling before deleting such as join()
      delete returnValue;
    } else {
      return result;
    }

    //get free pool and delete it
    returnValue = NULL;
    result = resources->get(EtchTransportHelper::FREE_POOL(), returnValue);
    if(result == ETCH_OK && returnValue != NULL) {
      //TODO: Add queue handling before deleting  such as join()
      delete returnValue;
    } else {
      return result;
    }

    //delete resource
    delete resources;
    ETCH_LOG_TRACE(runtime->getLogger(), runtime->getLogger().getRuntimeContext(), "Resources and pools have been deallocated");
    return ETCH_OK;
  }
}
