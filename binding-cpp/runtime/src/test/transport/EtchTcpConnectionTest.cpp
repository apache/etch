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
#include "transport/EtchTcpConnection.h"
#include "util/EtchResources.h"
#include "util/EtchURL.h"
#include "common/EtchError.h"
#include "common/EtchSocket.h"
#include "common/EtchString.h"
#include "capu/util/SmartPointer.h"
#include "transport/EtchTcpListener.h"
#include "transport/EtchSessionData.h"
#include "transport/EtchSessionListener.h"

class MockPacketizer : public virtual EtchSessionData {
public:

  //Communication Test Between Peers

  status_t sessionData(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchFlexBuffer> buf) {
    EXPECT_TRUE(memcmp(buf->getBuffer(), "mock", buf->getLength()) == 0);
    return ETCH_OK;
  }

  MOCK_METHOD2(sessionQuery, status_t(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result));

  MOCK_METHOD2(sessionControl, status_t(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value));

  status_t sessionNotify(capu::SmartPointer<EtchObject> event) {
    return ETCH_OK;
  }

};

class MockListener : public virtual EtchSessionListener<EtchSocket> {
public:
  MockListener() : socket(NULL) {}

  ~MockListener() {
    delete socket;
  }

  EtchResources resources;
  EtchSocket* socket;

  //Communication Test Between Peers
  status_t sessionAccepted(EtchSocket* connection) {
    EtchString _socket("socket");
    EtchObject *tmp;
    resources.put(_socket, connection, tmp);
    connection->send("mock", 4);
    socket = connection;
    return ETCH_OK;
  }

  MOCK_METHOD2(sessionQuery, status_t(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result));

  MOCK_METHOD2(sessionControl, status_t(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value));

  status_t sessionNotify(capu::SmartPointer<EtchObject> event) {
    return ETCH_OK;
  }

};

class EtchTcpConnectionTest
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

TEST_F(EtchTcpConnectionTest, constructorTest) {
  EtchURL url("tcp://127.0.0.1:4001");
  EtchTcpConnection * conn = new EtchTcpConnection(mRuntime, NULL, &url);
  EXPECT_TRUE(conn != NULL);
  delete conn;
}

TEST_F(EtchTcpConnectionTest, isStartedTest) {
  EtchURL url("tcp://127.0.0.1:4001");
  EtchTcpConnection * conn = new EtchTcpConnection(mRuntime, NULL, &url);
  EtchTcpListener *listener = new EtchTcpListener(mRuntime, &url);
  EXPECT_FALSE(conn->isStarted());
  EtchSessionListener<EtchSocket>* mSessionListener = new MockListener();
  EtchSessionData* mPacketizer = new MockPacketizer();
  //START THE LISTENER
  listener->setSession(mSessionListener);
  EXPECT_EQ(ETCH_OK, listener->transportControl(new EtchString(EtchTcpListener::START_AND_WAIT_UP()), new EtchInt32(400000)));
  //START THE TRANSPORT
  conn->setSession(mPacketizer);
  EXPECT_EQ(ETCH_OK, conn->transportControl(new EtchString(EtchTcpConnection::START_AND_WAIT_UP()), new EtchInt32(400000)));
  EXPECT_TRUE(conn->isStarted());
  //STOP THE TRANSPORT
  EXPECT_EQ(ETCH_OK, conn->transportControl(new EtchString(EtchTcpConnection::STOP_AND_WAIT_DOWN()), new EtchInt32(400000)));
  //STOP THE LISTENER
  EXPECT_EQ(ETCH_OK, listener->transportControl(new EtchString(EtchTcpListener::STOP_AND_WAIT_DOWN()), new EtchInt32(400000)));
  EXPECT_FALSE(conn->isStarted());
  conn->setSession(NULL);

  delete listener;
  delete conn;
  delete mSessionListener;
  delete mPacketizer;
}


TEST_F(EtchTcpConnectionTest, SessionAcceptTest) {
  EtchURL url("tcp://127.0.0.1:4001");
  EtchTcpConnection * conn = new EtchTcpConnection(mRuntime, NULL, &url);
  EtchSessionListener<EtchSocket>* mSessionListener = new MockListener();
  EtchSessionData* mPacketizer = new MockPacketizer();
  EtchTcpListener *listener = new EtchTcpListener(mRuntime, &url);

  //START THE LISTENER
  listener->setSession(mSessionListener);
  listener->transportControl(new EtchString(EtchTcpListener::START_AND_WAIT_UP()), new EtchInt32(1000));

  //START THE TRANSPORT
  conn->setSession(mPacketizer);
  conn->transportControl(new EtchString(EtchTcpConnection::START_AND_WAIT_UP()), new EtchInt32(1000));

  conn->transportControl(new EtchString(EtchTcpConnection::STOP_AND_WAIT_DOWN()), new EtchInt32(1000));
  //STOP THE LISTENER
  listener->transportControl(new EtchString(EtchTcpListener::STOP_AND_WAIT_DOWN()), new EtchInt32(1000));
  conn->setSession(NULL);

  delete conn;
  delete listener;
  delete mSessionListener;
  delete mPacketizer;
}
