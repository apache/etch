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
#include "transport/EtchPlainMailboxManager.h"
#include "support/EtchRuntime.h"
#include "capu/os/Debug.h"

EtchPlainMailboxManager::EtchPlainMailboxManager(EtchRuntime* runtime, EtchTransportMessage* transport, const EtchString& uri, EtchResources* resources)
: mRuntime(runtime), mSession(NULL), mTransport(transport), mUp(false), mMailboxes(ETCH_DEFAULT_MAILBOXMANAGER_HASH_BIT_SIZE) {
  capu::Debug::Assert(mRuntime != NULL);
  mTransport->setSession(this);
}

EtchPlainMailboxManager::~EtchPlainMailboxManager() {
  mMailboxesMutex.lock();

  EtchHashTable<EtchLong, capu::SmartPointer<EtchMailbox> >::Iterator it = mMailboxes.begin();
  EtchHashTable<EtchLong, capu::SmartPointer<EtchMailbox> >::HashTableEntry entry;

  while (it.hasNext()) {
    it.next(&entry);
    entry.value->closeDelivery();
  }

  mMailboxesMutex.unlock();
}

EtchTransportMessage* EtchPlainMailboxManager::getTransport() {
  return mTransport;
}

status_t EtchPlainMailboxManager::redeliver(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg) {
  return mSession->sessionMessage(sender, msg);
}

status_t EtchPlainMailboxManager::registerMailbox(capu::SmartPointer<EtchMailbox> mb) {
  if(mb.get() == NULL) {
    return ETCH_EINVAL;
  }

  const EtchLong msgid = mb->getMessageId();

  if (!mUp) {
    return ETCH_EINVAL;
  }

  capu::SmartPointer<EtchMailbox> tmp = NULL;
  if (mMailboxes.get(msgid, &tmp) != ETCH_ENOT_EXIST) {
    return ETCH_EINVAL;
  }

  mMailboxes.put(msgid, mb);
  ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getMailboxContext(), "A new mailbox is registered");
  return ETCH_OK;
}

status_t EtchPlainMailboxManager::unregisterMailbox(EtchLong mailboxId) {
  capu::SmartPointer<EtchMailbox> tmp = NULL;
  mMailboxesMutex.lock();
  mMailboxes.remove(mailboxId, &tmp);
  mMailboxesMutex.unlock();
  return ETCH_OK;
}

status_t EtchPlainMailboxManager::getMailbox(EtchLong msgid, capu::SmartPointer<EtchMailbox>& result) {
  status_t status = mMailboxes.get(msgid, &result);
  return status;
}

status_t EtchPlainMailboxManager::sessionMessage(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg) {
  capu::int64_t msgid;
  if(msg->getInReplyToMessageId(msgid) == ETCH_OK) {
    capu::SmartPointer<EtchMailbox> mb = NULL;
    ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getMailboxContext(), "A message has been received as answer to message identified by msgid " << msgid);
	mMailboxesMutex.lock();
    if (getMailbox(msgid, mb) != ETCH_OK) {
	  mMailboxesMutex.unlock();
      ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getMailboxContext(), "Mailbox for Message with msgid " << msgid << "has already been closed and removed. Dropping message.");
      return ETCH_ERROR;
    }
    mMailboxesMutex.unlock();
    ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getMailboxContext(), "Message has been sent to respective mailbox");
	status_t status = mb->message(sender, msg);
	
    return status;
  }
  // no msgid - pass off to session
  ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getMailboxContext(), "Message has been sent to upper layer directly as no msgid was given");
  return mSession->sessionMessage(sender, msg);
}

status_t EtchPlainMailboxManager::transportCall(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchMessage> msg, capu::SmartPointer<EtchMailbox>& result) {
  capu::int64_t tmp;
  if (msg->getMessageId(tmp) == ETCH_OK) {
    // message has already been sent
    return ETCH_ERROR;
  }

  if (msg->getInReplyToMessageId(tmp) == ETCH_OK) {
    // message is marked as a reply
    return ETCH_ERROR;
  }

  capu::int64_t msgid = mIdGen.next();
  if (msg->setMessageId(msgid) != ETCH_OK) {
    return ETCH_ERROR;
  }

  ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getMailboxContext(), "A mailbox has been created for msgid " << msgid);
  capu::SmartPointer<EtchMailbox> mb = new EtchPlainMailbox(this, msgid);

  mMailboxesMutex.lock();
  if (registerMailbox(mb) != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getMailboxContext(), "Mailbox registration failed");
    mMailboxesMutex.unlock();
    return ETCH_ERROR;
  }

  ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getMailboxContext(), "Message sending to Messagizer and registering a respective mailbox");
  if (mTransport->transportMessage(recipient, msg) == ETCH_OK) {
    result = mb;
    mMailboxesMutex.unlock();
    return ETCH_OK;
  } else {
    mb->closeDelivery();
    mMailboxesMutex.unlock();
    return ETCH_ERROR;
  }
}

status_t EtchPlainMailboxManager::transportMessage(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchMessage> message) {
  capu::int64_t tmp;
  if (message->getMessageId(tmp) == ETCH_OK) {
    // message has already been sent
    return ETCH_ERROR;
  }

  if(message->setMessageId(mIdGen.next()) != ETCH_OK) {
    return ETCH_ERROR;
  }
  ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getMailboxContext(), "Message has been sent to Messagizer without registering a mailbox");
  return mTransport->transportMessage(recipient, message);
}

status_t EtchPlainMailboxManager::sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result) {
  status_t status;
  if(mSession != NULL) {
    status = mSession->sessionQuery(query, result);
  } else {
    // TODO: add log message
    status = ETCH_ERROR;
  }
  return status;
}

status_t EtchPlainMailboxManager::sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
  status_t status;
  if(mSession != NULL) {
    status = mSession->sessionControl(control, value);
  } else {
    // TODO: add log message
    status = ETCH_ERROR;
  }
  return status;
}

status_t EtchPlainMailboxManager::sessionNotify(capu::SmartPointer<EtchObject> event) {
  mMailboxesMutex.lock();
  if (event->equals(&EtchSession::UP())) {
    mUp = true;
    ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getMailboxContext(), "Connection is up");
  } else if (event->equals(&EtchSession::DOWN())) {
    mUp = false;
    EtchHashTable<EtchLong, capu::SmartPointer<EtchMailbox> >::Iterator it = mMailboxes.begin();
    EtchHashTable<EtchLong, capu::SmartPointer<EtchMailbox> >::HashTableEntry entry;
    while (it.hasNext()) {
      it.next(&entry);
      entry.value->closeDelivery();
    }
    ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getMailboxContext(), "Connection is down");
  }
  mMailboxesMutex.unlock();

  status_t status;
  if(mSession != NULL) {
    status = mSession->sessionNotify(event);
  } else {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getMailboxContext(), "Session of PlainMailBoxManager is null");
    status = ETCH_ERROR;
  }
  return status;
}

status_t EtchPlainMailboxManager::transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result) {
  return mTransport->transportQuery(query, result);
}

status_t EtchPlainMailboxManager::transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
  return mTransport->transportControl(control, value);
}

status_t EtchPlainMailboxManager::transportNotify(capu::SmartPointer<EtchObject> event) {
  return mTransport->transportNotify(event);
}

EtchSessionMessage* EtchPlainMailboxManager::getSession() {
  return mSession;
}

void EtchPlainMailboxManager::setSession(EtchSessionMessage* session) {
  mSession = session;
}

capu::uint64_t EtchPlainMailboxManager::count() {
  return mMailboxes.count();
}
