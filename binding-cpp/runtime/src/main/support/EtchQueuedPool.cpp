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
#include "support/EtchQueuedPool.h"

#include "capu/os/Thread.h"
#include "capu/util/ThreadPool.h"
#include "capu/os/Memory.h"
#include "capu/util/Runnable.h"

class EtchQueuedPoolRunnable : public capu::Runnable {
public:

  /**
   * Create a new instance of EtchFreePoolRunnable class.
   */
  EtchQueuedPoolRunnable(EtchQueuedPool* pool, capu::SmartPointer<EtchPoolRunnable> runnable)
    : mRunnable(runnable) {
  }

  /**
   * Destructor
   */
  virtual ~EtchQueuedPoolRunnable() {
  }

  /**
   * Runnable
   */
  void run() {
    if(mRunnable.get() != NULL) {
      if(ETCH_OK != mRunnable->run()) {
        //TODO: Log exception
        if(mRunnable->hasException()) {
          capu::SmartPointer<EtchException> exception;
          mRunnable->getException(&exception);
          mRunnable->exception(exception);
        }
      }
    }
  }

private:
  capu::SmartPointer<EtchPoolRunnable> mRunnable;
};

const EtchObjectType* EtchQueuedPool::TYPE() {
  const static EtchObjectType TYPE(EOTID_QUEUEDPOOL, NULL);
  return &TYPE;
}

EtchQueuedPool::EtchQueuedPool(capu::int32_t size)
: mSizeMax(size), mIsOpen(true) {
  addObjectType(TYPE());
  mPool = new capu::ThreadPool(1);
}

EtchQueuedPool::~EtchQueuedPool() {
  delete mPool;
}

status_t EtchQueuedPool::close() {
  mIsOpen = false;
  return ETCH_OK;
}

status_t EtchQueuedPool::join() {
  mPool->close();
  return ETCH_OK;
}

status_t EtchQueuedPool::add(capu::SmartPointer<EtchPoolRunnable> runnable) {
  if(!mIsOpen) {
    return ETCH_EINVAL;
  }
  if(mPool->getSize() + 1 > mSizeMax)
    return ETCH_ENOT_SUPPORTED;

  EtchQueuedPoolRunnable* pr = new EtchQueuedPoolRunnable(this, runnable);
  capu::status_t status = mPool->add(pr);
  if(status != capu::CAPU_OK) {
    return ETCH_ERROR;
  }
  return ETCH_OK;
}
