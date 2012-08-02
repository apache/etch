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

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "serialization/EtchDefaultValueFactory.h"
#include "support/EtchPlainMailbox.h"
#include "transport/EtchDefaultDeliveryService.h"
#include "transport/EtchPlainMailboxManager.h"

class MockSession1 : public EtchSessionMessage {
public:

  status_t sessionMessage(capu::SmartPointer<EtchWho> receipent, capu::SmartPointer<EtchMessage> buf) {
    return ETCH_OK;
  }

  status_t sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result) {
    return ETCH_OK;
  }

  status_t sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
    return ETCH_OK;
  }

  status_t sessionNotify(capu::SmartPointer<EtchObject> event) {
    return ETCH_OK;
  }
};

class MockTransport1 : public EtchTransportMessage {
public:

  status_t transportMessage(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchMessage> message) {
    return ETCH_OK;
  }

  MOCK_METHOD0(getSession, EtchSessionMessage* ());

  void setSession(EtchSessionMessage* session) {

  }

  status_t transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result) {
    return ETCH_OK;
  }

  status_t transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
    return ETCH_OK;
  }

  status_t transportNotify(capu::SmartPointer<EtchObject> event) {
    return ETCH_OK;
  }
};

class MockDefaultValueFactory1 {
public:
  EtchTypeMap types;
  EtchClass2TypeMap class2type;
  EtchDefaultValueFactory * factory;
  /**
   * Constructs the MyValueFactory.
   * @param uriString
   */
public:

  MockDefaultValueFactory1(EtchString uriString) {
    EtchDefaultValueFactory::Init(&types, &class2type);
    factory = new EtchDefaultValueFactory(uriString, &types, &class2type);
  }

  ~MockDefaultValueFactory1() {
    delete factory;
    types.clear();
  }
};

TEST(EtchDeliveryServiceTest, constructorTest) {
  MockTransport1 transport;
  MockSession1 session;
  MockDefaultValueFactory1 *factory;
  EtchString uriString("tcp://127.0.0.1:4001");
  factory = new MockDefaultValueFactory1(uriString);
  //created value factory
  EtchURL uri(uriString);
  EtchPlainMailboxManager *manager = new EtchPlainMailboxManager(&transport, &uri, NULL);
  EtchDefaultDeliveryService *service = new EtchDefaultDeliveryService(manager, uriString);
  EtchDefaultDeliveryService *service2 = new EtchDefaultDeliveryService(manager, &uri);

  EXPECT_TRUE(service != NULL);
  manager->setSession(service);
  service->setSession(&session);

  EXPECT_TRUE(service2 != NULL);
  manager->setSession(service2);
  service2->setSession(&session);

  delete manager;
  delete service;
  delete service2;
  delete factory;
}

TEST(EtchDeliveryServiceTest, beginCallTest) {
  EtchString uriString("tcp://127.0.0.1:4001");
  EtchURL uri(uriString);
  MockDefaultValueFactory1 *factory = new MockDefaultValueFactory1(uriString);

  //set type and corresponding validator
  EtchType * type;
  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorLong::Get(0, val);

  factory->types.get("add", type);
  type->putValidator(EtchDefaultValueFactory::_mf__messageId, val);

  //create message of type
  capu::SmartPointer<EtchMessage> message = new EtchMessage(type, factory->factory);

  capu::int64_t id;
  EXPECT_TRUE(ETCH_OK != message->getMessageId(id));
  EXPECT_TRUE(ETCH_OK != message->getInReplyToMessageId(id));

  //create transport and session layers
  MockTransport1 transport;
  MockSession1 session;

  //create mailbox manager
  EtchPlainMailboxManager * manager = new EtchPlainMailboxManager(&transport, &uri, NULL);

  //create delivery service and set respective transport and session intefaces
  EtchDefaultDeliveryService * service = new EtchDefaultDeliveryService(manager, &uri);
  manager->setSession(service);
  service->setSession(&session);

  //put the stack up
  manager->sessionNotify(new EtchString(EtchSession::UP));

  //test begincall
  EtchMailbox *mail = NULL;
  EXPECT_TRUE(ETCH_OK == service->begincall(message, mail));
  EXPECT_EQ(1, manager->count());
  EXPECT_TRUE(ETCH_OK == message->getMessageId(id));
  EXPECT_TRUE(ETCH_OK != message->getInReplyToMessageId(id));

  EXPECT_TRUE(ETCH_OK == manager->getMailbox(id, mail));

  message->clear();
  delete service;
  delete manager;
  delete factory;
}

TEST(EtchDeliveryServiceTest, endCallTest) {
  EtchString uriString("tcp://127.0.0.1:4001");
  EtchURL uri(uriString);
  MockDefaultValueFactory1 *factory = new MockDefaultValueFactory1(uriString);

  //set type and return type and the corresponding validators
  EtchType * type;
  EtchType * replyType;
  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorLong::Get(0, val);

  factory->types.get("add_result", replyType);
  factory->types.get("add", type);
  type->putValidator(EtchDefaultValueFactory::_mf__messageId, val);
  replyType->putValidator(EtchDefaultValueFactory::_mf__inReplyTo, val);
  replyType->putValidator(EtchDefaultValueFactory::_mf__messageId, val);
  replyType->putValidator(EtchDefaultValueFactory::_mf_result, val);
  replyType->setResponseField(EtchDefaultValueFactory::_mf_result);

  //Create messsage that will be sent
  capu::SmartPointer<EtchMessage> message = new EtchMessage(type, factory->factory);
  capu::int64_t id;
  EXPECT_TRUE(ETCH_OK != message->getMessageId(id));
  EXPECT_TRUE(ETCH_OK != message->getInReplyToMessageId(id));

  MockTransport1 transport;
  MockSession1 session;

  //mailbox manager
  EtchPlainMailboxManager *manager = new EtchPlainMailboxManager(&transport, &uri, NULL);
  //delivery service
  EtchDefaultDeliveryService *service = new EtchDefaultDeliveryService(manager, &uri);

  manager->setSession(service);
  service->setSession(&session);
  manager->sessionNotify(new EtchString(EtchSession::UP));

  //performed the call
  EtchMailbox *mail;
  EXPECT_TRUE(ETCH_OK == service->begincall(message, mail));
  EXPECT_EQ(1, manager->count());
  EXPECT_TRUE(ETCH_OK == message->getMessageId(id));
  EXPECT_TRUE(ETCH_OK != message->getInReplyToMessageId(id));
  EXPECT_TRUE(ETCH_OK == manager->getMailbox(id, mail));
  capu::SmartPointer<EtchObject> result;

  //simulate the response
  capu::SmartPointer<EtchMessage> replymessage;

  //create a reply message
  message->createReplyMessage(replyType, replymessage);
  EXPECT_TRUE(ETCH_OK == replymessage->getInReplyToMessageId(id));
  capu::SmartPointer<EtchMessage> replymess = replymessage;
  capu::SmartPointer<EtchObject> data = new EtchLong(123);
  capu::SmartPointer<EtchObject> old;
  EtchField field = replyType->getResponseField();
  replymessage->put(field, data, &old);

  //call the sessionMessage of mailbox manager as if it is called from messagizer to deliver data from 
  EXPECT_TRUE(ETCH_OK == manager->sessionMessage(NULL, replymess));

  //wait for the response
  EXPECT_TRUE(ETCH_OK == service->endcall(mail, replyType, result));

  //check the result
  EXPECT_TRUE(result->equals(data.get()));

  replymessage->clear();
  message->clear();
  delete mail;
  delete service;
  delete manager;
  delete factory;
}