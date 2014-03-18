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

  MockSession1(EtchTransportMessage* transport)
    : mTransport(transport) {
    if(mTransport != NULL) {
      mTransport->setSession(this);
    }
  }

  virtual ~MockSession1() {
    if(mTransport != NULL) {
      delete mTransport;
    }
  }

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
private:
  EtchTransportMessage* mTransport;
};

class MockTransport1 : public EtchTransportMessage {
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

class MockDefaultValueFactory1 {
public:
  EtchTypeMap types;
  EtchClass2TypeMap class2type;
  EtchDefaultValueFactory * factory;
  /**
   * Constructs the MyValueFactory.
   * @param uri
   */
public:

  MockDefaultValueFactory1(EtchRuntime* runtime, EtchString uri) {
  EtchDefaultValueFactory::Init(runtime, &types, &class2type);
  factory = new EtchDefaultValueFactory(uri, &types, &class2type);
  }

  ~MockDefaultValueFactory1() {
    delete factory;
    types.clear();
  }
};

class EtchDefaultDeliveryServiceTest
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

TEST_F(EtchDefaultDeliveryServiceTest, constructorTest) {

  EtchString uri("tcp://127.0.0.1:4001");

// create mock layer for transport
  MockTransport1* transport = new MockTransport1();

  EtchPlainMailboxManager* mailboxManager = new EtchPlainMailboxManager(mRuntime, transport, uri, NULL);
  EtchDefaultDeliveryService* deliveryService = new EtchDefaultDeliveryService(mRuntime, mailboxManager, uri);

  // create mock layer for session
  MockSession1* session = new MockSession1(deliveryService);

  delete transport;
  delete mailboxManager;
  delete session;
}

TEST_F(EtchDefaultDeliveryServiceTest, beginCallTest) {
  EtchString uri("tcp://127.0.0.1:4001");

  // create value factory
  MockDefaultValueFactory1* factory = new MockDefaultValueFactory1(mRuntime, uri);
  //set type and corresponding validator
  EtchType * type = NULL;
  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorLong::Get(mRuntime, 0, val);

  factory->types.get("add", type);
  type->putValidator(EtchDefaultValueFactory::_mf__messageId(), val);

  //create message of type
  capu::SmartPointer<EtchMessage> message = new EtchMessage(type, factory->factory);

  capu::int64_t id;
  EXPECT_TRUE(ETCH_OK != message->getMessageId(id));
  EXPECT_TRUE(ETCH_OK != message->getInReplyToMessageId(id));

  // create mock layer for transport
  MockTransport1* transport = new MockTransport1();

  EtchPlainMailboxManager* mailboxManager = new EtchPlainMailboxManager(mRuntime, transport, uri, NULL);
  EtchDefaultDeliveryService* deliveryService = new EtchDefaultDeliveryService(mRuntime, mailboxManager, uri);

  // create mock layer for session
  MockSession1* session = new MockSession1(deliveryService);

  //put the stack up
  mailboxManager->sessionNotify(new EtchString(EtchSession::UP()));

  //test begincall
  capu::SmartPointer<EtchMailbox> mail = NULL;
  EXPECT_TRUE(ETCH_OK == deliveryService->begincall(message, mail));
  EXPECT_EQ(1u, mailboxManager->count());
  EXPECT_TRUE(ETCH_OK == message->getMessageId(id));
  EXPECT_TRUE(ETCH_OK != message->getInReplyToMessageId(id));

  EXPECT_TRUE(ETCH_OK == mailboxManager->getMailbox(id, mail));
  message->clear();

  //put the stack down
  mailboxManager->sessionNotify(new EtchString(EtchSession::DOWN()));

  delete transport;
  delete mailboxManager;
  delete session;
  delete factory;
}

TEST_F(EtchDefaultDeliveryServiceTest, endCallTest) {
  status_t status;
  EtchString uri("tcp://127.0.0.1:4001");

  // create value factory
  MockDefaultValueFactory1* factory = new MockDefaultValueFactory1(mRuntime, uri);

  //set type and return type and the corresponding validators
  EtchType * type = NULL;
  EtchType * replyType = NULL;
  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorLong::Get(mRuntime, 0, val);

  factory->types.get("add_result", replyType);
  factory->types.get("add", type);
  type->putValidator(EtchDefaultValueFactory::_mf__messageId(), val);
  replyType->putValidator(EtchDefaultValueFactory::_mf__inReplyTo(), val);
  replyType->putValidator(EtchDefaultValueFactory::_mf__messageId(), val);
  replyType->putValidator(EtchDefaultValueFactory::_mf_result(), val);
  replyType->setResponseField(EtchDefaultValueFactory::_mf_result());

  //Create messsage that will be sent
  capu::SmartPointer<EtchMessage> message = new EtchMessage(type, factory->factory);
  capu::int64_t id;
  EXPECT_TRUE(ETCH_OK != message->getMessageId(id));
  EXPECT_TRUE(ETCH_OK != message->getInReplyToMessageId(id));

  // create mock layer for transport
  MockTransport1* transport = new MockTransport1();

  EtchPlainMailboxManager* mailboxManager = new EtchPlainMailboxManager(mRuntime, transport, uri, NULL);
  EtchDefaultDeliveryService* deliveryService = new EtchDefaultDeliveryService(mRuntime, mailboxManager, uri);

  // create mock layer for session
  MockSession1* session = new MockSession1(deliveryService);

  //put the stack up
  mailboxManager->sessionNotify(new EtchString(EtchSession::UP()));

  //performed the call
  capu::SmartPointer<EtchMailbox> mail;
  EXPECT_TRUE(ETCH_OK == deliveryService->begincall(message, mail));
  EXPECT_EQ(1u, mailboxManager->count());
  EXPECT_TRUE(ETCH_OK == message->getMessageId(id));
  EXPECT_TRUE(ETCH_OK != message->getInReplyToMessageId(id));
  EXPECT_TRUE(ETCH_OK == mailboxManager->getMailbox(id, mail));

  //create a reply message
  //simulate the response
  capu::SmartPointer<EtchMessage> replymessage;
  message->createReplyMessage(replyType, replymessage);
  EXPECT_TRUE(ETCH_OK == replymessage->getInReplyToMessageId(id));
  capu::SmartPointer<EtchObject> data = new EtchLong(123);
  capu::SmartPointer<EtchObject> old;
  EtchField field = replyType->getResponseField();
  replymessage->put(field, data);

  //call the sessionMessage of mailbox manager as if it is called from messagizer to deliver data from
  status = mailboxManager->sessionMessage(NULL, replymessage);
  EXPECT_EQ(ETCH_OK, status);

  //wait for the response
  capu::SmartPointer<EtchObject> result;
  status = deliveryService->endcall(mail.get(), replyType, result);
  EXPECT_EQ(ETCH_OK, status);

  //check the result
  EXPECT_TRUE(result->equals(data.get()));

  delete transport;
  delete mailboxManager;
  delete session;
  delete factory;
}
