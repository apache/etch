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

#include "transport/EtchMessage.h"
#include "support/EtchRuntime.h"

EtchMessage::EtchMessage(EtchType* type, EtchValueFactory* vf)
: EtchStructValue(type, vf), mVf(vf) {

}

EtchMessage::EtchMessage(EtchType* type, EtchValueFactory* vf, capu::uint32_t length)
: EtchStructValue(type, vf, length), mVf(vf) {

}

EtchMessage::EtchMessage(const EtchMessage& other)
 : EtchStructValue(other), mVf(other.mVf) {
}

EtchMessage::~EtchMessage() {

}

EtchValueFactory* EtchMessage::getValueFactory() {
  return mVf;
}

status_t EtchMessage::createReplyMessage(capu::SmartPointer<EtchMessage> &message) {
  message = new EtchMessage(getType()->getResult(), mVf);
  capu::int64_t tmp;
  getMessageId(tmp);
  message->setInReplyToMessageId(tmp);
  if (message.get() != NULL)
    return ETCH_OK;
  else
    return ETCH_ERROR;
}

status_t EtchMessage::createReplyMessage(EtchType* rType, capu::SmartPointer<EtchMessage> &message) {
  message = new EtchMessage(rType, mVf);
  capu::int64_t tmp;
  getMessageId(tmp);
  message->setInReplyToMessageId(tmp);
  if (message.get() != NULL)
    return ETCH_OK;
  else
    return ETCH_ERROR;
}

status_t EtchMessage::getMessageId(capu::int64_t &result) {
  return mVf->getMessageId(this, result);
}

status_t EtchMessage::setMessageId(capu::int64_t msgid) {
  return mVf->setMessageId(this, msgid);
}

status_t EtchMessage::getInReplyToMessageId(capu::int64_t &result) {
  return mVf->getInReplyToMessageId(this, result);
}

void EtchMessage::setInReplyToMessageId(capu::int64_t msgid) {
  mVf->setInReplyToMessageId(this, msgid);
}

