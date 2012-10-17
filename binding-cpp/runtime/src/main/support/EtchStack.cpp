/* $Id$
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

#include "support/EtchStack.h"
#include "support/EtchTransportHelper.h"

const EtchObjectType* EtchStack::TYPE() {
   const static EtchObjectType TYPE(EOTID_STACK, NULL);
   return &TYPE;
}

const EtchString& EtchStack::STACK() {
  static const EtchString cmd("STACK");
  return cmd;
}

EtchStack::EtchStack() : EtchObject()
  , mTransportData(NULL)
  , mTransportPacket(NULL)
  , mTransportMessage(NULL)
  , mResources(NULL)
  , mValueFactory(NULL)
  , mMailboxManager(NULL)
  , mDeliveryService(NULL)
  , mStub(NULL)
  {
    addObjectType(EtchStack::TYPE());
}

EtchStack::EtchStack(const EtchStack& other) : EtchObject(other)
, mTransportData(other.mTransportData)
, mTransportPacket(other.mTransportPacket)
, mTransportMessage(other.mTransportMessage)
, mResources(other.mResources)
, mValueFactory(other.mValueFactory)
, mMailboxManager(other.mMailboxManager)
, mDeliveryService(other.mDeliveryService)
, mStub(other.mStub) {
}

EtchStack::~EtchStack() {
  if (mTransportMessage != NULL) {
    delete mTransportMessage;
  }
  if (mTransportPacket != NULL) {
    delete mTransportPacket;
  }
  if (mTransportData != NULL) {
    delete mTransportData;
  }
  if (mResources != NULL) {
    delete mResources;
  }
  if (mValueFactory != NULL) {
    delete mValueFactory;
  }
  if (mMailboxManager != NULL) {
    delete mMailboxManager;
  }
  if (mDeliveryService != NULL) {
    delete mDeliveryService;
  }

  if (mStub != NULL) {
    delete mStub;
  }
}

void EtchStack::setTransportData(EtchTransportData* transportData) {
  mTransportData = transportData;
}

void EtchStack::setTransportPacket(EtchTransportPacket* transportPacket) {
  mTransportPacket = transportPacket;
}

void EtchStack::setTransportMessage(EtchTransportMessage* transportMessage) {
  mTransportMessage = transportMessage;
}

void EtchStack::setResources(EtchResources* resources) {
  mResources = resources;
}

void EtchStack::setValueFactory(EtchValueFactory* valuefactory) {
  mValueFactory = valuefactory;
}

void EtchStack::setMailboxManager(EtchMailboxManager* mailboxmanager) {
  mMailboxManager = mailboxmanager;
}

void EtchStack::setDeliveryService(EtchDeliveryService* deliveryservice) {
  mDeliveryService = deliveryservice;
}

void EtchStack::setStub(EtchSessionMessage* stub) {
  mStub = stub;
}

EtchTransportData* EtchStack::getTransportData() {
  return mTransportData;
}

EtchTransportPacket* EtchStack::getTransportPacket() {
  return mTransportPacket;
}

EtchTransportMessage* EtchStack::getTransportMessage() {
  return mTransportMessage;
}

EtchResources* EtchStack::getResources() {
  return mResources;
}

EtchValueFactory* EtchStack::getValueFactory() {
  return mValueFactory;
}

EtchMailboxManager* EtchStack::getMailboxManager() {
  return mMailboxManager;
}

EtchDeliveryService* EtchStack::getDeliveryService() {
  return mDeliveryService;
}

EtchSessionMessage* EtchStack::getStub() {
  return mStub;
}

