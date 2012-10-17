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
#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include "serialization/EtchDefaultValueFactory.h"
#include "serialization/EtchBinaryTaggedData.h"
#include "serialization/EtchBinaryTaggedDataInput.h"
#include "serialization/EtchValidatorShort.h"
#include "transport/EtchMessagizer.h"
#include "transport/EtchTcpConnection.h"
#include "transport/EtchPacketizer.h"
#include "transport/EtchSessionListener.h"
#include "transport/EtchTcpListener.h"


class MockListener11 : public virtual EtchSessionListener<EtchSocket> {
public:

  MockListener11(EtchTransport<EtchSessionListener<EtchSocket> >* transport)
    : mTransport(transport) {
    if(mTransport != NULL) {
      mTransport->setSession(this);
    }
  }

  virtual ~MockListener11() {
    if(mTransport != NULL) {
      delete mTransport;
    }
  }

  //This method is called
  status_t sessionAccepted(EtchSocket* connection) {
    delete connection;
    return ETCH_OK;
  }

  MOCK_METHOD2(sessionQuery, status_t(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result));

  MOCK_METHOD2(sessionControl, status_t(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value));

  status_t sessionNotify(capu::SmartPointer<EtchObject> event) {
    return ETCH_OK;
  }

private:
 EtchTransport<EtchSessionListener<EtchSocket> >* mTransport;
};

class MockMailboxManager : public EtchSessionMessage {
public:

  status_t sessionMessage(capu::SmartPointer<EtchWho> receipent, capu::SmartPointer<EtchMessage> buf) {
    EXPECT_TRUE(buf->count() == 1);
    buf->clear();
    return ETCH_OK;
  }

  MOCK_METHOD2(sessionQuery, status_t(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result));

  MOCK_METHOD2(sessionControl, status_t(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value));

  status_t sessionNotify(capu::SmartPointer<EtchObject> event) {
    return ETCH_OK;
  }
};

class EtchMessagizerTest
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

TEST_F(EtchMessagizerTest, constructorTest) {
  EtchTypeMap types;
  EtchClass2TypeMap class2type;
  EtchDefaultValueFactory * factory;
  EtchDefaultValueFactory::Init(mRuntime, &types, &class2type);
  EtchString uri("tcp://127.0.0.1:4001");
  factory = new EtchDefaultValueFactory(uri, &types, &class2type);
  //created value factory
  EtchURL u("tcp://127.0.0.1:4001");
  EtchResources r;
  EtchObject *out;
  r.put(EtchTransport<EtchSessionMessage>::VALUE_FACTORY(), factory, out);
  EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
  EtchTransportPacket* pac = new EtchPacketizer(mRuntime, conn, &u);
  EtchSessionPacket* mes = new EtchMessagizer(mRuntime, pac, &u, &r);

  //Delete created stack
  delete conn;
  delete mes;
  delete pac;
  delete factory;
  types.clear();
}

TEST_F(EtchMessagizerTest, TransportControlTest) {
  EtchTypeMap types;
  EtchClass2TypeMap class2type;
  EtchDefaultValueFactory * factory;
  MockMailboxManager manager;
  EtchDefaultValueFactory::Init(mRuntime, &types, &class2type);
  EtchString uri("tcp://127.0.0.1:4001");
  factory = new EtchDefaultValueFactory(uri, &types, &class2type);
  //created value factory
  EtchURL u("tcp://127.0.0.1:4001");
  EtchResources r;
  EtchObject *out;
  r.put(EtchTransport<EtchSessionMessage>::VALUE_FACTORY(), factory, out);
  EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
  EtchPacketizer* pac = new EtchPacketizer(mRuntime, conn, &u);
  EtchMessagizer* mess = new EtchMessagizer(mRuntime, pac, &u, &r);
  mess->setSession(&manager);

  EtchTcpListener* transport = new EtchTcpListener(mRuntime, &u);
  EtchSessionListener<EtchSocket>* mSessionListener = new MockListener11(transport);

  transport->transportControl(new EtchString(EtchTcpListener::START_AND_WAIT_UP()), new EtchInt32(1000));

  mess->transportControl(new EtchString(EtchPacketizer::START_AND_WAIT_UP()), new EtchInt32(1000));
  //test transport commands
  mess->transportControl(new EtchString(EtchPacketizer::STOP_AND_WAIT_DOWN()), new EtchInt32(1000));

  transport->transportControl(new EtchString(EtchTcpListener::STOP_AND_WAIT_DOWN()), new EtchInt32(1000));

  delete conn;
  delete mSessionListener;
  delete pac;
  delete mess;
  delete factory;
  types.clear();
}

TEST_F(EtchMessagizerTest, TransportMessageTest) {
  EtchTypeMap types;
  EtchClass2TypeMap class2type;
  EtchDefaultValueFactory * factory;
  MockMailboxManager manager;
  EtchDefaultValueFactory::Init(mRuntime, &types, &class2type);
  EtchString uri("tcp://127.0.0.1:4001");
  factory = new EtchDefaultValueFactory(uri, &types, &class2type);
  //default value factory
  EtchURL u("tcp://127.0.0.1:4001");
  EtchResources r;
  EtchObject *out;
  //add to the resource
  r.put(EtchTransport<EtchSessionMessage>::VALUE_FACTORY(), factory, out);
  EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
  EtchPacketizer* pac = new EtchPacketizer(mRuntime, conn, &u);
  EtchMessagizer* mess = new EtchMessagizer(mRuntime, pac, &u, &r);
  mess->setSession(&manager);
  //creation of example message which will be serialized
  EtchType *mt_foo = NULL;
  EtchField mf_x("x");
  EtchString str("foo");
  factory->getType(str, mt_foo);
  capu::SmartPointer<EtchValidator> v;
  EtchValidatorShort::Get(mRuntime, 0, v);
  mt_foo->putValidator(mf_x, v);
  capu::SmartPointer<EtchShort> data = new EtchShort(10000);
  capu::SmartPointer<EtchMessage> msg = new EtchMessage(mt_foo, factory);
  msg->put(mf_x, data);

  EXPECT_TRUE(mess->transportMessage(NULL, msg) == ETCH_ERROR);

  delete conn;
  delete mess;
  delete pac;
  delete factory;
  types.clear();
}

TEST_F(EtchMessagizerTest, SessionDataTest) {
  //creation of an example message to compare the deserialized messageMyValueFactory vf("tcp:");
  EtchTypeMap types;
  EtchClass2TypeMap class2type;
  EtchString name1("a");
  EtchString name2("b");
  EtchType *mt_foo = new EtchType(1, name1);
  EtchField mf_x(2, name2);
  capu::SmartPointer<EtchValidator> v;
  EtchValidatorShort::Get(mRuntime, 0, v);
  mt_foo->putValidator(mf_x, v);
  types.add(mt_foo);
  capu::SmartPointer<EtchShort> data = new EtchShort(10000);
  //default value factory
  EtchDefaultValueFactory * factory;
  EtchDefaultValueFactory::Init(mRuntime, &types, &class2type);
  EtchString uri("tcp://127.0.0.1:4001");


  factory = new EtchDefaultValueFactory(uri, &types, &class2type);
  capu::SmartPointer<EtchMessage> msg = new EtchMessage(mt_foo, factory);
  msg->put(mf_x, data);


  EtchURL u(uri);
  EtchResources r;
  EtchObject *out;
  //add the value factory to the resources
  r.put(EtchTransport<EtchSessionMessage>::VALUE_FACTORY(), factory, out);
  //create stack
  EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
  EtchPacketizer* pac = new EtchPacketizer(mRuntime, conn, &u);
  EtchMessagizer* mess = new EtchMessagizer(mRuntime, pac, &u, &r);
  capu::SmartPointer<EtchFlexBuffer> buffer = new EtchFlexBuffer();

  //A packet is created
  capu::int8_t byte_pos[] = {3, 1, 1, 2, -123, 39, 16, -127};
  buffer->setByteRepresentation(ETCH_BIG_ENDIAN);
  buffer->put((capu::int8_t *)"_header_", pac->getHeaderSize());
  buffer->put((capu::int8_t *)byte_pos, 8);
  buffer->setIndex(0);
  capu::int32_t pktsize = buffer->getLength() - pac->getHeaderSize();
  buffer->put((capu::int8_t *) & pac->SIG, sizeof (capu::int32_t));
  buffer->put((capu::int8_t *) & pktsize, sizeof (capu::int32_t));
  buffer->setIndex(pac->getHeaderSize());
  EXPECT_TRUE(buffer->getLength() == 16);

  //Simulate call with fake package
  EtchSessionMessage* mMailboxManager = new MockMailboxManager();
  mess->setSession(mMailboxManager);
  EXPECT_TRUE(mess->sessionPacket(NULL, buffer) == ETCH_OK);

  mess->setSession(NULL);
  types.clear();
  delete conn;
  delete mMailboxManager;
  delete pac;
  delete mess;
  delete factory;
}
