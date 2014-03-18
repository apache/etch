/* EtchAsyncResultNone.cpp
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

#include "support/EtchAsyncResultNone.h"

#define MAILBOX_NOTIFY_TIMEOUT 2000

EtchAsyncResultNone::EtchAsyncResultNone(EtchRuntime* runtime, capu::SmartPointer<EtchMailbox> mailbox)
  : mRuntime(runtime), mMailbox(mailbox), mHasMailboxStatus(false), mHasException(false), mException(NULL) {
    if(mailbox.get() != NULL) {
      mMailbox->registerNotify(this, NULL, MAILBOX_NOTIFY_TIMEOUT);
    }
    
}

EtchAsyncResultNone::~EtchAsyncResultNone() {
  if(mMailbox.get() != NULL) {
    mMailbox->unregisterNotify(this);
    mMailbox->closeDelivery(false);
  }
}

capu::bool_t EtchAsyncResultNone::hasException() {
  mMutex.lock();
  while(mMailbox.get() != NULL && !mHasMailboxStatus) {
    mCond.wait(&mMutex);
  }
  mMutex.unlock();
  return mHasException;
}

capu::SmartPointer<EtchException> EtchAsyncResultNone::getException() {
  if (hasException())
    return mException;
  return NULL;
}

void EtchAsyncResultNone::setException(capu::SmartPointer<EtchException> exception) {
  mException = exception;
  mHasException = true;
  setMailboxStatus();
}

void EtchAsyncResultNone::setMailboxStatus() {
  mMutex.lock();
  mHasMailboxStatus = true;
  mCond.signal();
  mMutex.unlock();
}
