/* $Id$
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

#include "common/EtchError.h"
#include "common/EtchRuntimeException.h"
#include "support/EtchRuntime.h"
#include "support/EtchTransportHelper.h"
#include "util/EtchLogger.h"

//TODO remove this after refactoring
EtchRuntime* EtchRuntime::sRuntime = NULL;

EtchRuntime::EtchRuntime()
  : mIsClosed(false)
  , mLogger(NULL) {
  mMutex.lock();
  mId = getNextId();
  mMutex.unlock();

  //TODO remove this after refactoring
  sRuntime = this;
}

EtchRuntime::~EtchRuntime() {
}

capu::uint64_t EtchRuntime::getId() {
  return mId;
}

capu::bool_t EtchRuntime::isClosed() {
  return mIsClosed;
}

status_t EtchRuntime::registerListener(EtchRuntimeListener* listener) {
  if(listener == NULL) {
    return ETCH_EINVAL;
  }

  mMutex.lock();
  mListeners.add(listener);
  mMutex.unlock();
  return ETCH_OK;
}

status_t EtchRuntime::unregisterListener(EtchRuntimeListener* listener) {
  status_t status;
  mMutex.lock();
  capu::int_t index = mListeners.find(listener);
  if(index != -1) {
    mListeners.removeAt(index);
    status = ETCH_OK;
  } else {
    status = ETCH_ERROR;
  }
  mMutex.unlock();
  return status;
}

status_t EtchRuntime::start() {
  return ETCH_OK;
}

status_t EtchRuntime::shutdown() {
  if(!mIsClosed) {
    mIsClosed = true;
    fireOnRuntimeChanged();
  }
  return ETCH_OK;
}

status_t EtchRuntime::fireOnRuntimeChanged() {
  mMutex.lock();

  capu::List<EtchRuntimeListener*>::Iterator iter = mListeners.begin();
  while(iter.hasNext()) {
    EtchRuntimeListener* listener = NULL;
    iter.next(&listener);
    listener->onRuntimeChanged(this);
  }
  mMutex.unlock();
  return ETCH_OK;
}

capu::uint64_t EtchRuntime::getNextId() {
  static capu::uint64_t sId = 0;
  return sId++;
}

status_t EtchRuntime::setLogger(EtchLogger* logger) {
  if(logger == NULL) {
    return ETCH_EINVAL;
  }
  mLogger = logger;
  return ETCH_OK;
}

EtchLogger* EtchRuntime::getLogger() {
  return mLogger;
}

EtchRuntime* EtchRuntime::getRuntime() {
  return sRuntime;
}

