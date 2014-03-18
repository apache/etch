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

#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "support/EtchRemoteBase.h"
#include "transport/EtchDefaultDeliveryService.h"
#include "serialization/EtchDefaultValueFactory.h"
#include "support/EtchPlainMailbox.h"
#include "transport/EtchPlainMailboxManager.h"
#include "support/EtchRuntime.h"

class MockSession2 : public EtchSessionMessage {
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

class MockTransport2 : public EtchTransportMessage {
public:

  status_t transportMessage(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchMessage> message) {
    return ETCH_OK;
  }

  MOCK_METHOD0(getSession, EtchSessionMessage* ());

  void setSession(EtchSessionMessage* session) {

  }

  status_t transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result) {
    return ETCH_OK;
  }

  status_t transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
    return ETCH_OK;
  }

  status_t transportNotify(capu::SmartPointer<EtchObject> event) {
    return ETCH_OK;
  }
};

class MockDefaultValueFactory2 {
public:
  EtchTypeMap types;
  EtchClass2TypeMap class2type;
  EtchDefaultValueFactory * factory;

  /**
   * Constructs the MyValueFactory.
   * @param uri
   */
public:

  MockDefaultValueFactory2(EtchRuntime* runtime, EtchString uri) {
    EtchDefaultValueFactory::Init(runtime, &types, &class2type);
    factory = new EtchDefaultValueFactory(uri, &types, &class2type);
  }

  ~MockDefaultValueFactory2() {
    delete factory;
    types.clear();
  }
};

class EtchRemoteBaseTest
  : public ::testing::Test {
protected:
  virtual void SetUp() {
    mRuntime = new EtchRuntime();
    mRuntime->start();
  }

  virtual void TearDown() {
    mRuntime->shutdown();

    delete mRuntime;
    mRuntime = NULL;
  }

  EtchRuntime* mRuntime;
};

TEST_F(EtchRemoteBaseTest, constructorTest) {
  MockSession2 session;
  MockTransport2 *transport = new MockTransport2();
  MockDefaultValueFactory2 *factory;
  EtchString uri("tcp://127.0.0.1:4001");
  factory = new MockDefaultValueFactory2(mRuntime, uri);
  //created value factory
  EtchURL u(uri);
  EtchPlainMailboxManager * manager = NULL;
  manager = new EtchPlainMailboxManager(mRuntime, transport, NULL, NULL);
  EtchDeliveryService * service = NULL;
  service = new EtchDefaultDeliveryService(mRuntime, manager, &u);
  EtchRemoteBase * remote = NULL;
  remote = new EtchRemoteBase(mRuntime, service, factory->factory, NULL);

  EXPECT_TRUE(remote != NULL);
  manager->setSession(service);
  service->setSession(&session);

  delete remote;
  delete transport;
  delete manager;
  delete service;
  delete factory;
}

TEST_F(EtchRemoteBaseTest, newMessageTest) {
  MockSession2 session;
  MockTransport2 *transport = new MockTransport2();
  MockDefaultValueFactory2 *factory;
  EtchString uri("tcp://127.0.0.1:4001");
  factory = new MockDefaultValueFactory2(mRuntime, uri);
  //created value factory
  EtchURL u(uri);

  EtchPlainMailboxManager * manager = NULL;
  manager = new EtchPlainMailboxManager(mRuntime, transport, NULL, NULL);
  EtchDeliveryService * service = NULL;
  service = new EtchDefaultDeliveryService(mRuntime, manager, &u);
  EtchRemoteBase * remote = NULL;
  remote = new EtchRemoteBase(mRuntime, service, factory->factory, NULL);

  EXPECT_TRUE(remote != NULL);
  manager->setSession(service);
  service->setSession(&session);

  //create type
  EtchString typeName("foo");
  EtchType mt(typeName);
  //create new message
  capu::SmartPointer<EtchMessage> msg;
  remote->newMessage(&mt, &msg);

  //check type
  EXPECT_TRUE(msg->isType(&mt));

  //check the assigned factory
  EXPECT_TRUE(factory->factory == msg->getValueFactory());

  delete transport;
  delete remote;
  delete manager;
  delete service;
  delete factory;
}

TEST_F(EtchRemoteBaseTest, sendTest) {
  MockSession2 session;
  MockTransport2 *transport = new MockTransport2();
  MockDefaultValueFactory2 *factory;
  EtchString uri("tcp://127.0.0.1:4001");
  factory = new MockDefaultValueFactory2(mRuntime, uri);
  //created value factory
  EtchURL u(uri);

  EtchPlainMailboxManager * manager = NULL;
  manager = new EtchPlainMailboxManager(mRuntime, transport, NULL, NULL);
  EtchDeliveryService * service = NULL;
  service = new EtchDefaultDeliveryService(mRuntime, manager, &u);
  EtchRemoteBase * remote = NULL;
  remote = new EtchRemoteBase(mRuntime, service, factory->factory, NULL);

  EXPECT_TRUE(remote != NULL);
  manager->setSession(service);
  service->setSession(&session);

  //get the validator
  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorLong::Get(mRuntime, 0, val);
  //create type
  EtchType *mt;
  factory->types.get("add", mt);
  mt->putValidator(EtchDefaultValueFactory::_mf__messageId(), val);
  //create new message
  capu::SmartPointer<EtchMessage> msg;
  remote->newMessage(mt, &msg);

  //check type
  EXPECT_TRUE(msg->isType(mt));
  //remote send
  EXPECT_TRUE(remote->send(msg) == ETCH_OK);

  factory->types.clear();

  delete transport;
  delete remote;
  delete manager;
  delete service;
  delete factory;
}

TEST_F(EtchRemoteBaseTest, beginCallTest) {
  MockSession2 session;
  MockTransport2 *transport = new MockTransport2();
  MockDefaultValueFactory2 *factory;
  EtchString uri("tcp://127.0.0.1:4001");
  factory = new MockDefaultValueFactory2(mRuntime, uri);
  //created value factory
  EtchURL u(uri);

  EtchPlainMailboxManager * manager = NULL;
  manager = new EtchPlainMailboxManager(mRuntime, transport, NULL, NULL);
  EtchDeliveryService * service = NULL;
  service = new EtchDefaultDeliveryService(mRuntime, manager, &u);
  EtchRemoteBase * remote = NULL;
  remote = new EtchRemoteBase(mRuntime, service, factory->factory, NULL);

  EXPECT_TRUE(remote != NULL);
  manager->setSession(service);
  service->setSession(&session);

  //get the validator
  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorLong::Get(mRuntime, 0, val);
  //create type
  EtchType *mt;
  factory->types.get("add", mt);
  mt->putValidator(EtchDefaultValueFactory::_mf__messageId(), val);

  //create new message
  capu::SmartPointer<EtchMessage> msg;
  remote->newMessage(mt, &msg);

  //check type
  EXPECT_TRUE(msg->isType(mt));

  manager->sessionNotify(new EtchString(EtchSession::UP()));

  capu::SmartPointer<EtchMailbox> mail = NULL;
  EXPECT_TRUE(remote->begincall(msg, mail) == ETCH_OK);
  EXPECT_TRUE(mail.get() != NULL);

  factory->types.clear();

  delete transport;
  delete remote;
  delete manager;
  delete service;
  delete factory;
}

TEST_F(EtchRemoteBaseTest, endCallTest) {
  MockSession2 session;
  MockTransport2 *transport = new MockTransport2();
  MockDefaultValueFactory2 *factory;
  EtchString uri("tcp://127.0.0.1:4001");
  factory = new MockDefaultValueFactory2(mRuntime, uri);
  //created value factory
  EtchURL u(uri);

  EtchPlainMailboxManager * manager = NULL;
  manager = new EtchPlainMailboxManager(mRuntime, transport, NULL, NULL);
  EtchDeliveryService * service = NULL;
  service = new EtchDefaultDeliveryService(mRuntime, manager, &u);
  EtchRemoteBase * remote = NULL;
  remote = new EtchRemoteBase(mRuntime, service, factory->factory, NULL);

  EXPECT_TRUE(remote != NULL);
  manager->setSession(service);
  service->setSession(&session);

  //create type
  EtchType * type;
  EtchType * replyType;
  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorLong::Get(mRuntime, 0, val);

  factory->types.get("add_result", replyType);
  factory->types.get("add", type);
  type->putValidator(EtchDefaultValueFactory::_mf__messageId(), val);
  replyType->putValidator(EtchDefaultValueFactory::_mf__inReplyTo(), val);
  replyType->putValidator(EtchDefaultValueFactory::_mf__messageId(), val);
  replyType->putValidator(EtchDefaultValueFactory::_mf_result(), val);
  replyType->setResponseField(EtchDefaultValueFactory::_mf_result());

  //create new message
  capu::SmartPointer<EtchMessage> replyMessage;
  capu::SmartPointer<EtchMessage> message;
  remote->newMessage(type, &message);

  manager->sessionNotify(new EtchString(EtchSession::UP()));

  capu::SmartPointer<EtchMailbox> mail = NULL;
  EXPECT_TRUE(remote->begincall(message, mail) == ETCH_OK);

  EXPECT_TRUE(mail.get() != NULL);

  capu::int64_t id;
  message->getMessageId(id);
  //create a reply message
  message->createReplyMessage(replyType, replyMessage);
  EXPECT_TRUE(ETCH_OK == replyMessage->getInReplyToMessageId(id));
  capu::SmartPointer<EtchMessage> replymess = replyMessage;
  capu::SmartPointer<EtchObject> data = new EtchLong(123);
  EtchField field = replyType->getResponseField();
  replyMessage->put(field, data);
  //call the sessionMessage of mailbox manager as if it is called from messagizer to deliver data from
  EXPECT_TRUE(ETCH_OK == manager->sessionMessage(NULL, replymess));
  capu::SmartPointer<EtchObject> result;
  EXPECT_TRUE(remote->endcall(mail.get(), replyType, result) == ETCH_OK);
  EXPECT_TRUE(mail.get() != NULL);
  EXPECT_TRUE(result == data);

  factory->types.clear();

  delete transport;
  delete remote;
  delete manager;
  delete service;
  delete factory;
}
