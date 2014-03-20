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

#include "capu/os/Thread.h"
#include "capu/util/Runnable.h"
#include "capu/os/Memory.h"
#include "capu/util/Runnable.h"


class EtchFreePoolRunnable : public capu::Runnable {
public:

  /**
   * Create a new instance of EtchFreePoolRunnable class.
   */
  EtchFreePoolRunnable(EtchFreePool* pool, capu::SmartPointer<EtchPoolRunnable> runnable)
    : mRunnable(runnable) {
  }

  /**
   * Destructor
   */
  virtual ~EtchFreePoolRunnable() {
  }

  /**
   * Runnable
   */
  void run() {
    if(mRunnable.get() != NULL) {
      if(ETCH_OK != mRunnable->run()) {
        // Log exception
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

const EtchObjectType* EtchFreePool::TYPE() {
  const static EtchObjectType TYPE(EOTID_FREEPOOL, NULL);
  return &TYPE;
}

EtchFreePool::EtchFreePool(capu::int32_t size)
 : mSize(0), mSizeMax(size), mIsOpen(true) {
  addObjectType(TYPE());
  mThreads = new capu::Thread*[mSizeMax];
  capu::Memory::Set(mThreads, 0, sizeof(capu::Thread*)*mSizeMax);
  mRunnables = new EtchFreePoolRunnable*[mSizeMax];
  capu::Memory::Set(mRunnables, 0, sizeof(EtchFreePoolRunnable*)*mSizeMax);
}

EtchFreePool::~EtchFreePool() {
  delete[] mThreads;
  delete[] mRunnables;
}

status_t EtchFreePool::close() {
  mIsOpen = false;
  return ETCH_OK;
}

capu::int32_t EtchFreePool::getSize() {
  capu::int32_t size;
  mMutex.lock();
  check();
  size = mSize;
  mMutex.unlock();
  return size;
}

status_t EtchFreePool::join() {
  mMutex.lock();
  close();
  for(capu::int32_t i = 0; i < mSizeMax; i++) {
    if(mThreads[i] != NULL) {
      mThreads[i]->join();
      delete mThreads[i];
      delete mRunnables[i];
      mThreads[i] = NULL;
      mRunnables[i] = NULL;
    }
  }
  mMutex.unlock();
  return ETCH_OK;
}

status_t EtchFreePool::add(capu::SmartPointer<EtchPoolRunnable> runnable) {

  // TODO: improve thread group handling and clean up e.g. with
  // a free list that could be cleaned if the tread terminated

  if(!mIsOpen) {
    return ETCH_ERROR;
  }

  mMutex.lock();

  // clean thread list
  check();

  if(mSize >= mSizeMax) {
    mMutex.unlock();
    return ETCH_ERROR;
  }

  EtchFreePoolRunnable *tmp = new EtchFreePoolRunnable(this, runnable);
  capu::Thread* thread = new capu::Thread();
  thread->start(*tmp);

  for(capu::int32_t i = 0; i < mSizeMax; i++) {
    if(mThreads[i] == NULL) {
      mThreads[i] = thread;
      mRunnables[i] = tmp;
      mSize++;
      mMutex.unlock();
      return ETCH_OK;
    }
  }
  mMutex.unlock();

  return ETCH_ERROR;
}

status_t EtchFreePool::check() {
  for(capu::int32_t i = 0; i < mSizeMax; i++) {
    if(mThreads[i] != NULL) {
      capu::ThreadState state = mThreads[i]->getState();
      // clean old threads
      if(capu::TS_TERMINATED == state) {
        delete mThreads[i];
        mThreads[i] = NULL;

        if (mRunnables[i] != NULL) {
          delete mRunnables[i];
          mRunnables[i] = NULL;
        }
        mSize--;
      }
    }
  }
  return ETCH_OK;
}
