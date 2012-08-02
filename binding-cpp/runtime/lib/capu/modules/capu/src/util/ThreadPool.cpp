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

#include "capu/util/ThreadPool.h"

capu::ThreadPool::ThreadPool() : mSize(5) {
  init();
}

capu::ThreadPool::ThreadPool(capu::uint_t size) : mSize(size) {
  init();
}

capu::status_t capu::ThreadPool::init() {
  mClosed = false;
  mCloseRequested = false;
  mRunnableQueue = new capu::Queue<capu::SmartPointer<capu::Runnable> >();
  mThreadArray = new capu::ThreadPool::PoolWorker*[mSize];
  for (capu::uint32_t i = 0; i<mSize; i++) {
    capu::ThreadPool::PoolWorker *t = new capu::ThreadPool::PoolWorker(this, i);
    mThreadArray[i] = t;
    t->startThread();
  }

  return CAPU_OK;
}

capu::ThreadPool::~ThreadPool() {
  mClosed = true;
  join();
  delete mRunnableQueue;
  for (capu::uint32_t i = 0; i<mSize; i++) {
    capu::ThreadPool::PoolWorker *t = mThreadArray[i];
    delete t;
  }
  delete[] mThreadArray;
}


capu::status_t capu::ThreadPool::add(capu::SmartPointer<capu::Runnable> runnable) {
  if (runnable.get() == NULL || mClosed || mCloseRequested) {
    return CAPU_ERROR;
  }
  mMutex.lock();
  status_t result = mRunnableQueue->add(runnable);
  mCV.signal();
  mMutex.unlock();
  return result;
}

capu::status_t capu::ThreadPool::join() {
  mMutex.lock();
  mCloseRequested = true;
  mCV.broadcast();
  mMutex.unlock();
  status_t result = capu::CAPU_OK;
  for (capu::uint32_t i = 0; i<mSize; i++) {
    capu::ThreadPool::PoolWorker *t = mThreadArray[i];
    result = t->joinThread();
    if (result != capu::CAPU_OK) {
      return result;
    }
  }
  return result;
}

capu::int32_t capu::ThreadPool::getSize() {
  return mSize;
}

capu::bool_t capu::ThreadPool::isClosed() {
  return mClosed;
}




