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

#include "capu/os/NumericLimits.h"
#include "capu/os/Memory.h"
#include "util/EtchCircularQueue.h"

EtchCircularQueue::EtchCircularQueue()
: mItems(NULL), mClosed(false), mCount(0), mSize(10), mHead(0), mTail(0) {
  mItems = new EtchMailbox::EtchElement*[mSize];
  capu::Memory::Set(mItems, 0, sizeof(EtchMailbox::EtchElement*) * mSize);
}

EtchCircularQueue::EtchCircularQueue(capu::uint32_t size)
: mItems(NULL), mClosed(false), mCount(0), mSize(size), mHead(0), mTail(0) {
  mItems = new EtchMailbox::EtchElement*[mSize];
  capu::Memory::Set(mItems, 0, sizeof(EtchMailbox::EtchElement*) * mSize);
}

EtchCircularQueue::~EtchCircularQueue() {
  mMutex.lock();
  delete[] mItems;
  mMutex.unlock();
}

capu::uint32_t EtchCircularQueue::getSize() {
  return mSize;
}

capu::uint32_t EtchCircularQueue::getCount() {
  return mCount;
}

capu::bool_t EtchCircularQueue::isEmpty() {
  return mCount == 0;
}

capu::bool_t EtchCircularQueue::isFull() {
  return mCount == mSize;
}

status_t EtchCircularQueue::get0(EtchMailbox::EtchElement** element) {
  if(element == NULL || mCount < 1) {
    return ETCH_EINVAL;
  }

  EtchMailbox::EtchElement* obj = mItems[mHead];
  if (obj == NULL) {
    return ETCH_EINVAL;
  }

  mItems[mHead] = NULL;

  mCount--;
  mHead++;

  if(mHead == mSize) {
    mHead = 0;
  }

  *element = obj;
  return ETCH_OK;
}

status_t EtchCircularQueue::put0(EtchMailbox::EtchElement* obj) {
  if (obj == NULL || mCount >= mSize || mItems[mTail] != NULL) {
    return ETCH_EINVAL;
  }

  mItems[mTail] = obj;

  mCount++;
  mTail++;

  if (mTail == mSize) {
      mTail = 0;
  }
  return ETCH_OK;
}

status_t EtchCircularQueue::get(EtchMailbox::EtchElement** element) {
  return get(element, 0);
}

status_t EtchCircularQueue::get(EtchMailbox::EtchElement** element, capu::int32_t maxDelay) {
  status_t status;
  
  if(element == NULL) {
    return ETCH_EINVAL;
  }

  mMutex.lock();

  if (!isEmpty()) {
    status = getAndNotify(element);
    mMutex.unlock();
    return status;
  }

  if ((maxDelay < 0)) {
    *element = NULL;
    mMutex.unlock();
    return ETCH_EINVAL;
  }

  // the queue is closed.
  if (isClosed()) {
    *element = NULL;
    mMutex.unlock();
    return ETCH_ERROR;
  }

  // the queue is empty, not closed, and caller has requested a delay...
  capu::int64_t now = capu::Time::GetMilliseconds();
  capu::int64_t end = (maxDelay > 0) ? now + maxDelay : capu::NumericLimits::Max<capu::int64_t>();

  capu::int64_t d;
  while ((d = end - now) > 0) {
    // the queue is empty, not closed, and delay has not run out...
    if(!isEmpty() || isClosed() || d <= 0) {
      *element = NULL;
      mMutex.unlock();
      return ETCH_ERROR;
    }
    // wait
    mCondVar.wait(&mMutex, static_cast<capu::uint32_t>(d));

    if (!isEmpty()) {
      status = getAndNotify(element);
      mMutex.unlock();
      return status;
    }

    // the queue is closed.
    if (isClosed()) {
      *element = NULL;
      mMutex.unlock();
      return ETCH_OK;
    }

    now = capu::Time::GetMilliseconds();
  }
  // the time has been elapsed
  *element = NULL;
  mMutex.unlock();
  return ETCH_TIMEOUT;
}

status_t EtchCircularQueue::put(EtchMailbox::EtchElement* obj) {
  return put(obj, 0);
}

capu::status_t EtchCircularQueue::put(EtchMailbox::EtchElement* obj, capu::int32_t maxDelay) {
  status_t status;

  if(obj == NULL) {
    return ETCH_EINVAL;
  }

  mMutex.lock();

  if (isClosed()) {
    mMutex.unlock();
    return ETCH_ERROR;
  }

  // the queue is not closed.
  if (!isFull()) {
    status = putAndNotify(obj);
    mMutex.unlock();
    return status;
  }

  // the queue is full and not closed.
  if(maxDelay < 0) {
    mMutex.unlock();
    return ETCH_EINVAL;
  }

  // the queue is full, not closed, and the caller has requested a delay...
  capu::int64_t now = capu::Time::GetMilliseconds();
  capu::int64_t end = (maxDelay > 0) ? now + maxDelay : capu::NumericLimits::Max<capu::int64_t>();

  capu::int64_t d;
  while((d = end - now) > 0) {
    // the queue is full, not closed, and delay has not run out...
    if (!isFull() || isClosed() || d <= 0) {
      mMutex.unlock();
      return ETCH_ERROR;
    }
    // wait
    mCondVar.wait(&mMutex, static_cast<capu::uint32_t>(d));

    if(isClosed()) {
      mMutex.unlock();
      return ETCH_ERROR;
    }
    // the queue is not closed.
    if (!isFull()) {
      status = putAndNotify(obj);
      mMutex.unlock();
      return status;
    }
    now = capu::Time::GetMilliseconds();
  }

  mMutex.unlock();
  return ETCH_TIMEOUT;
}

void EtchCircularQueue::close() {
  mMutex.lock();
  if (!mClosed) {
    mClosed = true;
    mCondVar.broadcast();
  }
  mMutex.unlock();
}

capu::bool_t EtchCircularQueue::isClosed() {
  return mClosed;
}

status_t EtchCircularQueue::getAndNotify(EtchMailbox::EtchElement** element) {
  status_t status;

  // the queue is now not empty.

  // putters waiting should be notified if the queue is now full.
  capu::bool_t notify = isFull();

  status = get0(element);

  // getters waiting should be notified if the queue is now not empty.
  notify = notify || !isEmpty();

  if(notify) {
    mCondVar.signal();
  }

  return status;
}

status_t EtchCircularQueue::putAndNotify(EtchMailbox::EtchElement* obj) {
  status_t status;

  // the queue is now not full.

  // getters waiting should be notified if the queue is now empty.
  capu::bool_t notify = isEmpty();

  status = put0(obj);

  // putters waiting should be notified if the queue is now not full.
  notify = notify || !isFull();

  if (notify) {
    mCondVar.signal();
  }
  return status;
}


