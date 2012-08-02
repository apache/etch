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

EtchRemoteBase::EtchRemoteBase(EtchDeliveryService* svc, EtchValueFactory* vf )
 : mSvc(svc)
 , mVf(vf) {
}

status_t EtchRemoteBase::newMessage( EtchType* type, capu::SmartPointer<EtchMessage> *message) {
  if(type == NULL || message == NULL) {
    return ETCH_EINVAL;
  }
  *message = new EtchMessage(type, mVf);
  return ETCH_OK;
}

status_t EtchRemoteBase::send(capu::SmartPointer<EtchMessage> msg) {
  return ETCH_EUNIMPL;
}


status_t EtchRemoteBase::beginCall(EtchMessage* msg, EtchMailbox** result) {
  return ETCH_EUNIMPL;
}

status_t EtchRemoteBase::endCall(EtchMailbox* mb, EtchType* responseType, EtchObject** result) {
  return ETCH_EUNIMPL;
}

status_t EtchRemoteBase::transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result) {
  return ETCH_EUNIMPL;
}

status_t EtchRemoteBase::transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
  return ETCH_EUNIMPL;
}

status_t EtchRemoteBase::transportNotify(capu::SmartPointer<EtchObject> event ) {
  return ETCH_EUNIMPL;
}

////////////////////////////
// Convenience operations //
////////////////////////////

status_t EtchRemoteBase::start() {
  return ETCH_EUNIMPL;
}

status_t EtchRemoteBase::waitUp(capu::int32_t maxDelay) {
  return ETCH_EUNIMPL;
}

status_t EtchRemoteBase::startAndWaitUp(capu::int32_t maxDelay) {
  return ETCH_EUNIMPL;
}

status_t EtchRemoteBase::stop() {
  return ETCH_EUNIMPL;
}

status_t EtchRemoteBase::waitDown(capu::int32_t maxDelay) {
  return ETCH_EUNIMPL;
}

status_t EtchRemoteBase::stopAndWaitDown(capu::int32_t maxDelay) {
  return ETCH_EUNIMPL;
}
