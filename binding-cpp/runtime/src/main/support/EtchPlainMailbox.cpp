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
#include "support/EtchPlainMailbox.h"

EtchPlainMailbox::EtchPlainMailbox(EtchMailboxManager* mailboxManager, EtchLong messageId)
: mMailboxManager(mailboxManager), mNotify(NULL), mState(NULL), mMessageId(messageId), mQueue(1) {
}

EtchPlainMailbox::~EtchPlainMailbox() {
  mQueueMutex.lock();
  while (!mQueue.isEmpty()) {
    EtchMailbox::EtchElement* element = NULL;
    if(ETCH_OK == mQueue.get(&element)) {
      delete element;
    }
  }
  mQueueMutex.unlock();
}

EtchMailboxManager* EtchPlainMailbox::getMailboxManager() {
  return mMailboxManager;
}

EtchLong EtchPlainMailbox::getMessageId() {
  return mMessageId;
}

status_t EtchPlainMailbox::message(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg) {
  status_t status;
  EtchMailbox::EtchElement* element = new EtchMailbox::EtchElement(sender, msg);

  mQueueMutex.lock();
  status = mQueue.put(element, -1);
  mQueueMutex.unlock();

  if(status == ETCH_OK) {
     fireNotify();
     return ETCH_OK;
  } else {
    // TODO: log error if message could not be put to the mailbox
    delete element;
    return ETCH_ERROR;
  }
}

void EtchPlainMailbox::fireNotify() {

  EtchMailbox::EtchNotify *n;
  EtchObject* s;
  capu::bool_t c;

  mQueueMutex.lock();
  n = mNotify;
  s = mState;
  c = mQueue.isClosed();
  
  if (n != NULL) {
    n->mailboxStatus(this, s, c);
  }
  mQueueMutex.unlock();
}

status_t EtchPlainMailbox::read(EtchMailbox::EtchElement*& result) {
  mQueueMutex.lock();
  status_t status = mQueue.get(&result);
  mQueueMutex.unlock();
  if(ETCH_OK == status) {
    return ETCH_OK;
  }
  // TODO: logging
  return ETCH_ERROR;
}

status_t EtchPlainMailbox::read(EtchMailbox::EtchElement *& result, capu::int32_t maxDelay) {
  mQueueMutex.lock();
  status_t status = mQueue.get(&result, maxDelay);
  mQueueMutex.unlock();
  if(status == ETCH_OK) {
    return ETCH_OK;
  }
  return ETCH_ERROR;
}

status_t EtchPlainMailbox::closeDelivery(capu::bool_t withNotification) {

  mQueueMutex.lock();
  if(mQueue.isClosed()) {
    mQueueMutex.unlock();
    return ETCH_EINVAL;
  }

  mMailboxManager->unregisterMailbox(getMessageId());
  mQueue.close();
  mQueueMutex.unlock();

  if (withNotification) {
    fireNotify();
  }

  return ETCH_OK;
}

status_t EtchPlainMailbox::closeRead() {
  if (closeDelivery(false) == ETCH_OK) {
    EtchMailbox::EtchElement* mbe = NULL;
    while ((read(mbe)) == ETCH_OK) {
      mMailboxManager->redeliver(mbe->mSender, mbe->mMsg);
      delete mbe;
    }
    return ETCH_OK;
  }
  return ETCH_ERROR;
}

status_t EtchPlainMailbox::registerNotify(EtchMailbox::EtchNotify* notify, EtchObject* state, capu::int32_t maxDelay) {
  if(notify == NULL) {
    return ETCH_EINVAL;
  }

  if(maxDelay < 0) {
    return ETCH_EINVAL;
  }

  capu::bool_t isNotEmptyOrIsClosed;

  mQueueMutex.lock();

  if(mNotify != NULL) {
    mQueueMutex.unlock();
    return ETCH_EINVAL;
  }

  mNotify = notify;
  mState = state;

  isNotEmptyOrIsClosed = !mQueue.isEmpty() || mQueue.isClosed();

  mQueueMutex.unlock();

  if(isNotEmptyOrIsClosed) {
    fireNotify();
  }

  return ETCH_OK;
}

status_t EtchPlainMailbox::unregisterNotify(EtchMailbox::EtchNotify* notify) {

  if(mNotify == NULL) {
    return ETCH_EINVAL;
  }

  if(notify == NULL) {
    return ETCH_OK;
  }

  mQueueMutex.lock();

  if(mNotify !=  notify) {
    mQueueMutex.unlock();
    return ETCH_EINVAL;
  }

  mNotify = NULL;
  mState = NULL;

  mQueueMutex.unlock();

  return ETCH_OK;
}

capu::bool_t EtchPlainMailbox::isEmpty() {
  mQueueMutex.lock();
  capu::bool_t res = mQueue.isEmpty();
  mQueueMutex.unlock();
  return res;
}

capu::bool_t EtchPlainMailbox::isClosed() {
  mQueueMutex.lock();
  capu::bool_t res = mQueue.isClosed();
  mQueueMutex.unlock();
  return res;
}

capu::bool_t EtchPlainMailbox::isFull() {
  mQueueMutex.lock();
  capu::bool_t res = mQueue.isFull();
  mQueueMutex.unlock();
  return res;
}
