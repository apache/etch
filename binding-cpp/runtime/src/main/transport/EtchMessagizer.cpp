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
#include "transport/EtchMessagizer.h"

const EtchString EtchMessagizer::FORMAT("Messagizer.format");
capu::Mutex EtchMessagizer::mutex;

EtchMessagizer::EtchMessagizer(EtchTransportPacket* transport, EtchURL* uri, EtchResources* resources)
: mTransport(transport) {
  EtchString format;
  EtchObject * val;
  resources->get((EtchString &) EtchTransport<EtchSessionMessage>::VALUE_FACTORY, val);

  if (uri->getTerms().get((EtchString &) FORMAT, &format) == ETCH_OK) {

    if (format.equals(&EtchFormat::BINARY)) {
      mTdi = new EtchBinaryTaggedDataInput((EtchValueFactory *) val);
      mTdo = new EtchBinaryTaggedDataOutput((EtchValueFactory *) val, uri);
    } else if (format.equals(&EtchFormat::XML)) {
      //we dont need serialization via xml currently
    }
  } else {
    mTdi = new EtchBinaryTaggedDataInput((EtchValueFactory *) val);
    mTdo = new EtchBinaryTaggedDataOutput((EtchValueFactory *) val, uri);
  }
  transport->setSession(this);
}

EtchMessagizer::~EtchMessagizer() {
  if(mTransport != NULL)
    delete mTransport;
  if (mTdi != NULL)
    delete mTdi;
  if (mTdo != NULL)
    delete mTdo;
}

EtchTransportPacket* EtchMessagizer::getTransport() {
  return mTransport;
}

EtchSessionMessage* EtchMessagizer::getSession() {
  return mSession;
}

void EtchMessagizer::setSession(EtchSessionMessage* session) {
  mSession = session;
}

status_t EtchMessagizer::transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
  return mTransport->transportControl(control, value);
}

status_t EtchMessagizer::transportNotify(capu::SmartPointer<EtchObject> event) {
  return mTransport->transportNotify(event);
}

status_t EtchMessagizer::transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result) {
  return mTransport->transportQuery(query, result);
}

status_t EtchMessagizer::sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
  return mSession->sessionControl(control, value);
}

status_t EtchMessagizer::sessionNotify(capu::SmartPointer<EtchObject> event) {
  return mSession->sessionNotify(event);
}

status_t EtchMessagizer::sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result) {
  return mSession->sessionQuery(query, result);
}

status_t EtchMessagizer::sessionPacket(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchFlexBuffer> buf) {
  status_t result;
  capu::SmartPointer<EtchMessage> message;
  result = mTdi->readMessage(buf, message);
  if (result != ETCH_OK) {
    return result;
  }
  result = mSession->sessionMessage(sender, message);
  if (result != ETCH_OK) {
    EtchString errmsg("Unwanted Message");
    mSession->sessionNotify(new EtchException(errmsg, ETCH_UNWANTED_MESSAGE, EtchException::EXCPTYPE_BUILTIN));
  }
  return ETCH_OK;

}

status_t EtchMessagizer::transportMessage(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchMessage> message) {
  mutex.lock();
  capu::SmartPointer<EtchFlexBuffer> msgBuf = new EtchFlexBuffer();
  status_t result;

  // packetize the message.
  // leave space for the packet header
  result = msgBuf->setLength(mTransport->getHeaderSize());
  if (result != ETCH_OK) {
    mutex.unlock();
    return result;
  }

  result = msgBuf->setIndex(mTransport->getHeaderSize());
  if (result != ETCH_OK) {
    mutex.unlock();
    return result;
  }
  result = mTdo->writeMessage(message, msgBuf);
  if (result != ETCH_OK){
    mutex.unlock();
    return result;
  }
  msgBuf->setIndex(0);
  result = mTransport->transportPacket(recipient, msgBuf);
  if (result != ETCH_OK) {
    mutex.unlock();
    return result;
  }
  mutex.unlock();
  return ETCH_OK;
}
