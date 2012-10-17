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

#ifndef ETCHSTACK_H
#define ETCHSTACK_H

#include "support/EtchStubBase.h"
#include "transport/EtchTransportData.h"
#include "transport/EtchTransportPacket.h"
#include "transport/EtchTransportMessage.h"
#include "transport/EtchPlainMailboxManager.h"
#include "transport/EtchDefaultDeliveryService.h"
#include "serialization/EtchValueFactory.h"
#include "util/EtchResources.h"

/* *
 * This class serves as a container for all layers created during the setup of a new stack (connection, packetizer, messagizer, ...).
 */
class EtchStack : public EtchObject {
public:

  /**
   * EtchObjectType for EtchStack.
   */
  static const EtchObjectType* TYPE();

  /**
   * Resource identifier
   */
  const static EtchString& STACK();

  /**
   * Default Constructor
   */
  EtchStack();

  /**
   * Default Constructor
   */
  EtchStack(const EtchStack& other);


  /**
   * Destructor which cleans the whole stack
   */
  virtual ~EtchStack();

  /**
   * sets the transport interface
   */
  void setTransportData(EtchTransportData* transportData);

  /**
   * sets the packet interface
   */
  void setTransportPacket(EtchTransportPacket* transportPacket);

  /**
   * sets the message interface
   */
  void setTransportMessage(EtchTransportMessage* transportMessage);

  /**
   * sets the resources
   */
  void setResources(EtchResources* resources);

  /**
   * sets the value factory
   */
  void setValueFactory(EtchValueFactory* valuefactory);

  /**
   * sets the mailbox manager
   */
  void setMailboxManager(EtchMailboxManager* mailboxmanager);

  /**
   * sets the delivery service
   */
  void setDeliveryService(EtchDeliveryService* deliveryservice);

  /**
   * sets the stub implemenation
   */
  void setStub(EtchSessionMessage* stub);

  /**
   * returns the transport interface
   */
  EtchTransportData* getTransportData();

  /**
   * returns the packet interface
   */
  EtchTransportPacket* getTransportPacket();

  /**
   * returns the message interface
   */
  EtchTransportMessage* getTransportMessage();

  /**
   * returns the resources
   */
  EtchResources* getResources();

  /**
   * returns the value factory
   */
  EtchValueFactory* getValueFactory();

  /**
   * returns the mailbox manager
   */
  EtchMailboxManager* getMailboxManager();

  /**
   * returns the delivery service
   */
  EtchDeliveryService* getDeliveryService();

  /**
   * returns the stub implemenatation
   */
  EtchSessionMessage* getStub();

private:
  EtchTransportData* mTransportData;
  EtchTransportPacket* mTransportPacket;
  EtchTransportMessage* mTransportMessage;
  EtchResources* mResources;
  EtchValueFactory* mValueFactory;
  EtchMailboxManager* mMailboxManager;
  EtchDeliveryService* mDeliveryService;
  EtchSessionMessage* mStub;

};

#endif /* ETCHSTACK_H */
