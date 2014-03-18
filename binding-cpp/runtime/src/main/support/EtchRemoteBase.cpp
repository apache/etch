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

#include "support/EtchRemoteBase.h"
#include "transport/EtchTransportData.h"
#include "support/EtchRuntime.h"
#include "util/EtchLogger.h"
#include "support/EtchStackClient.h"

EtchRemoteBase::EtchRemoteBase(EtchRuntime* runtime, EtchDeliveryService* svc, EtchValueFactory* vf, EtchStack* stack)
  : mRuntime(runtime), mSvc(svc), mVf(vf), mStack(stack) {
}

EtchRemoteBase::~EtchRemoteBase() {
  if (mStack != NULL) {
    if (mStack->isInstanceOf(EtchStackClient::TYPE())) {
      delete mStack;
    }
  }
}

status_t EtchRemoteBase::newMessage( EtchType* type, capu::SmartPointer<EtchMessage> *message) {
  if(type == NULL || message == NULL) {
    return ETCH_EINVAL;
  }
  *message = new EtchMessage(type, mVf);
  return ETCH_OK;
}

status_t EtchRemoteBase::send(capu::SmartPointer<EtchMessage> msg) {
  return mSvc->transportMessage(NULL, msg);
}

status_t EtchRemoteBase::begincall(capu::SmartPointer<EtchMessage> msg, capu::SmartPointer<EtchMailbox> &result) {
  ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getDeliveryServiceContext(), "Begin call for the message is initiated");
  return mSvc->begincall(msg, result);
}

status_t EtchRemoteBase::endcall(EtchMailbox* mb, EtchType* responseType, capu::SmartPointer<EtchObject> &result) {
  ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getDeliveryServiceContext(), "End call for the message is initiated");
  return mSvc->endcall(mb, responseType, result);
}

status_t EtchRemoteBase::transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result) {
  return mSvc->transportQuery(query, result);
}

status_t EtchRemoteBase::transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
  return mSvc->transportControl(control, value);
}

status_t EtchRemoteBase::transportNotify(capu::SmartPointer<EtchObject> event) {
  return mSvc->transportNotify(event);
}

status_t EtchRemoteBase::start() {
  return transportControl(new EtchString(EtchTransportData::START()), NULL);
}

status_t EtchRemoteBase::waitUp(capu::int32_t maxDelay) {
  return transportQuery(new WaitUp(maxDelay), NULL);
}

status_t EtchRemoteBase::startAndWaitUp(capu::int32_t maxDelay) {
  status_t res = start();
  if (res != ETCH_OK)
    return res;
  return waitUp(maxDelay);
}

status_t EtchRemoteBase::stop() {
  return transportControl(new EtchString(EtchTransportData::STOP()), NULL);
}

status_t EtchRemoteBase::waitDown(capu::int32_t maxDelay) {
  return transportQuery(new WaitDown(maxDelay), NULL);
}

status_t EtchRemoteBase::stopAndWaitDown(capu::int32_t maxDelay) {
  status_t res = stop();
  if (res != ETCH_OK)
    return res;
  return waitDown(maxDelay);
}
