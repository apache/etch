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
  while (!mQueue.isEmpty()) {
    EtchMailbox::EtchElement* element = NULL;
    if(ETCH_OK == mQueue.get(&element)) {
      delete element;
    }
  }
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

  mMutex.lock();
  status = mQueue.put(element, -1);
  mMutex.unlock();

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

  mMutex.lock();
  n = mNotify;
  s = mState;
  c = mQueue.isClosed();
  
  mMutex.unlock();

  if (n != NULL) {
    n->mailboxStatus(this, s, c);
  }
}

status_t EtchPlainMailbox::read(EtchMailbox::EtchElement*& result) {
  status_t status = mQueue.get(&result);
  if(ETCH_OK == status) {
    return ETCH_OK;
  }
  // TODO: logging
  return ETCH_ERROR;
}

status_t EtchPlainMailbox::read(EtchMailbox::EtchElement *& result, capu::int32_t maxDelay) {
  status_t status = mQueue.get(&result, maxDelay);
  if(status == ETCH_OK) {
    return ETCH_OK;
  }
  return ETCH_ERROR;
}

status_t EtchPlainMailbox::closeDelivery() {

  mMutex.lock();
  if(mQueue.isClosed()) {
    mMutex.unlock();
    return ETCH_EINVAL;
  }

  mMailboxManager->unregisterMailbox(this);
  mQueue.close();
  mMutex.unlock();

  fireNotify();
  return ETCH_OK;
}

status_t EtchPlainMailbox::closeRead() {
  if (closeDelivery() == ETCH_OK) {
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

  mMutex.lock();

  if(mNotify != NULL) {
    mMutex.unlock();
    return ETCH_EINVAL;
  }

  mNotify = notify;
  mState = state;

  isNotEmptyOrIsClosed = !mQueue.isEmpty() || mQueue.isClosed();

  mMutex.unlock();

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

  mMutex.lock();

  if(mNotify !=  notify) {
    mMutex.unlock();
    return ETCH_EINVAL;
  }

  mNotify = NULL;
  mState = NULL;

  mMutex.unlock();

  return ETCH_OK;
}

capu::bool_t EtchPlainMailbox::isEmpty() {
  capu::bool_t res = mQueue.isEmpty();
  return res;
}

capu::bool_t EtchPlainMailbox::isClosed() {
  capu::bool_t res = mQueue.isClosed();
  return res;
}

capu::bool_t EtchPlainMailbox::isFull() {
  capu::bool_t res = mQueue.isFull();
  return res;
}
