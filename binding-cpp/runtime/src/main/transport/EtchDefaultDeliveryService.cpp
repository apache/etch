/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#include "transport/EtchDefaultDeliveryService.h"
#include "support/EtchRuntime.h"
#include "util/EtchLogger.h"

static char* TAG = "EtchDefaultDeliveryService";

const EtchString& EtchDefaultDeliveryService::DISABLE_TIMEOUT() {
  static const EtchString name("DefaultDeliveryService.disableTimeout");
  return name;
}

EtchDefaultDeliveryService::EtchDefaultDeliveryService(EtchRuntime* runtime, EtchMailboxManager* transport, const EtchString& uri)
: mRuntime(runtime), mTransport(transport), mStatus(EtchString("session status"), EtchString("")) {
  EtchURL url(uri);
  init(&url);
}

EtchDefaultDeliveryService::EtchDefaultDeliveryService(EtchRuntime* runtime, EtchMailboxManager* transport, EtchURL* uri)
: mRuntime(runtime), mTransport(transport), mStatus(EtchString("session status"), EtchString("")) {
  init(uri);
}

void EtchDefaultDeliveryService::init(EtchURL* uri) {
  //set session to mailbox manager
  if (mTransport != NULL) {
    mTransport->setSession(this);
  }

  //check for "disable timout" parameter in given uri
  mDisableTimeout = false;
  EtchString str;
  if (uri != NULL) {
    if (uri->getTerms().get(DISABLE_TIMEOUT(), &str) == ETCH_OK) {
      EtchString tmp("0");
      //check if disable timeout is not 0
      if (!str.equals(&tmp)) {
        mDisableTimeout = true;
      }
    }
  }
}

EtchDefaultDeliveryService::~EtchDefaultDeliveryService() {

}

const EtchMailboxManager* EtchDefaultDeliveryService::getTransport() {
  return mTransport;
}

EtchSessionMessage* EtchDefaultDeliveryService::getSession() {
  return mSession;
}

void EtchDefaultDeliveryService::setSession(EtchSessionMessage* session) {
  mSession = session;
}

status_t EtchDefaultDeliveryService::sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result) {
  return mSession->sessionQuery(query, result);
}

status_t EtchDefaultDeliveryService::sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
  return mSession->sessionControl(control, value);
}

status_t EtchDefaultDeliveryService::sessionNotify(capu::SmartPointer<EtchObject> event) {
  EtchString str;
  if (event->equals(&UP())) {
    mStatus.set(UP(), str);
  } else if (event->equals(&DOWN())) {
    mStatus.set(DOWN(), str);
  }
  return mSession->sessionNotify(event);
}

status_t EtchDefaultDeliveryService::sessionMessage(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg) {
  CAPU_LOG_DEBUG(mRuntime->getLogger(), TAG, "Message will be passed to StubBase to execute the respective remote call");
  return mSession->sessionMessage(sender, msg);
}

status_t EtchDefaultDeliveryService::transportMessage(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchMessage> message) {
  CAPU_LOG_DEBUG(mRuntime->getLogger(), TAG, "Result of respective remote call will be passed to Mailbox Manager");
  return mTransport->transportMessage(recipient, message);
}

status_t EtchDefaultDeliveryService::transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result) {
  if (query->getObjectType()->equals(WaitUp::TYPE())) {
    waitUp(((WaitUp*) query.get())->mMaxDelay);
    result = NULL;
    return ETCH_OK;
  } else if (query->getObjectType()->equals(WaitDown::TYPE())) {
    waitDown(((WaitDown*) query.get())->mMaxDelay);
    result = NULL;
    return ETCH_OK;
  }
  return mTransport->transportQuery(query, result);
}

status_t EtchDefaultDeliveryService::transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
  status_t result = ETCH_ERROR;;
  if (control->equals(&START_AND_WAIT_UP())) {
    result = mTransport->transportControl(new EtchString(START()), NULL);
    if (result != ETCH_OK) {
      return result;
    }
    //TODO: check if value is a valid integer
    result = waitUp(((EtchInt32*) value.get())->get());
  } else if (control->equals(&STOP_AND_WAIT_DOWN())) {
    result = mTransport->transportControl(new EtchString(STOP()), NULL);
    if (result != ETCH_OK) {
      return result;
    }
    //TODO: check if value is a valid integer
    result = waitDown(((EtchInt32*) value.get())->get());
  } else {
    return mTransport->transportControl(control, value);
  }
  return result;
}

status_t EtchDefaultDeliveryService::transportNotify(capu::SmartPointer<EtchObject> event) {
  return mTransport->transportNotify(event);
}

status_t EtchDefaultDeliveryService::begincall(capu::SmartPointer<EtchMessage> msg, EtchMailbox*& result) {
  CAPU_LOG_DEBUG(mRuntime->getLogger(), TAG, "Begin call for the message has been initiated");
  return mTransport->transportCall(NULL, msg, result);
}

status_t EtchDefaultDeliveryService::endcall(EtchMailbox* mb, EtchType* responseType, capu::SmartPointer<EtchObject>& result) {

  //get timeout
  capu::uint32_t timeout = mDisableTimeout ? 0 : responseType->getTimeout();

  //get message from mailbox
  EtchMailbox::EtchElement* mbe = NULL;
  CAPU_LOG_DEBUG(mRuntime->getLogger(), TAG, "End call for the message starts");
  status_t res = mb->read(mbe, timeout);
  if (res != ETCH_OK) {
    mb->closeRead();
    CAPU_LOG_ERROR(mRuntime->getLogger(), TAG, "Error on mailbox read, might be caused by timeout");
    //TODO: Add error handling
    return res;
  }
  CAPU_LOG_TRACE(mRuntime->getLogger(), TAG, "We got a message in the mailbox");

  //get reply message and responseType
  capu::SmartPointer<EtchMessage> rmsg = mbe->mMsg;
  if (!rmsg->isType(responseType)) {
    CAPU_LOG_ERROR(mRuntime->getLogger(), TAG, "Error on response Type");
    mb->closeRead();
    rmsg->clear();
    delete mbe;
    return ETCH_ERROR;
  }

  //get response field
  capu::SmartPointer<EtchObject> r;
  EtchField field = responseType->getResponseField();
  status_t err = rmsg->get(field, &r);
  if (err == ETCH_ENOT_EXIST) {
    //void return value
    mb->closeRead();
    CAPU_LOG_DEBUG(mRuntime->getLogger(), TAG, "End call for the message is completed");
    return ETCH_OK;
  } else if (err != ETCH_OK) {
    mb->closeRead();
    rmsg->clear();
    CAPU_LOG_ERROR(mRuntime->getLogger(), TAG, "Error on getting respective field on message structure");
    delete mbe;
    return ETCH_ERROR;
  }
  if (r->getObjectType()->equals(EtchRuntimeException::TYPE())) {
    //TODO: handle error
  }
  delete mbe;
  result = r;
  mb->closeRead();
  CAPU_LOG_DEBUG(mRuntime->getLogger(), TAG, "End call for the message is completed");
  return ETCH_OK;
}

status_t EtchDefaultDeliveryService::waitUp(capu::int32_t maxDelay) {
  return mStatus.waitUntilEq(UP(), maxDelay);
}

status_t EtchDefaultDeliveryService::waitDown(capu::int32_t maxDelay) {
  return mStatus.waitUntilEq(DOWN(), maxDelay);
}
