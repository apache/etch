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

  MOCK_METHOD2(sessionQuery, status_t(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> result));

  MOCK_METHOD2(sessionControl, status_t(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value));

  MOCK_METHOD1(sessionNotify, status_t(capu::SmartPointer<EtchObject> event));

};

class MockListener : public virtual EtchSessionListener<EtchSocket> {
public:

  //Communication Test Between Peers

  EtchResources resources;

  status_t sessionAccepted(EtchSocket* connection) {
    EtchString _socket("socket");
    EtchObject *tmp;
    resources.put(_socket, connection, tmp);
    connection->send((unsigned char *) "mock", 4);
    capu::Thread::Sleep(1000);
    delete connection;
    return ETCH_OK;
  }

  MOCK_METHOD2(sessionQuery, status_t(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> result));

  MOCK_METHOD2(sessionControl, status_t(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value));

  MOCK_METHOD1(sessionNotify, status_t(capu::SmartPointer<EtchObject> event));

};

TEST(EtchTcpConnection, constructorTest) {
  EtchURL url("tcp://127.0.0.1:4001");
  EtchResources resources;
  EtchTcpConnection * conn = new EtchTcpConnection(NULL, &url);
  EXPECT_TRUE(conn != NULL);
  delete conn;
}

TEST(EtchTcpConnection, isStartedTest) {
  EtchURL url("tcp://127.0.0.1:4001");
  EtchTcpConnection * conn = new EtchTcpConnection(NULL, &url);
  EtchTcpListener *listener = new EtchTcpListener(&url);
  EXPECT_FALSE(conn->isStarted());
  EtchSessionListener<EtchSocket>* mSessionListener = new MockListener();
  EtchSessionData* mPacketizer = new MockPacketizer();
  //START THE LISTENER
  listener->setSession(mSessionListener);
  listener->transportControl(new EtchString(EtchTcpListener::START_AND_WAIT_UP), new EtchInt32(1000));
  //START THE TRANSPORT
  conn->setSession(mPacketizer);
  conn->transportControl(new EtchString(EtchTcpConnection::START_AND_WAIT_UP), new EtchInt32(1000));
  EXPECT_TRUE(conn->isStarted());
  //STOP THE TRANSPORT
  conn->transportControl(new EtchString(EtchTcpConnection::STOP_AND_WAIT_DOWN), new EtchInt32(1000));
  //STOP THE LISTENER
  listener->transportControl(new EtchString(EtchTcpListener::STOP_AND_WAIT_DOWN), new EtchInt32(1000));
  EXPECT_FALSE(conn->isStarted());
  listener->setSession(NULL);
  conn->setSession(NULL);

  delete mSessionListener;
  delete mPacketizer;
  delete listener;
  delete conn;
}

TEST(EtchTcpConnectionAndListener, SessionAcceptTest) {
  EtchURL url("tcp://127.0.0.1:4001");
  EtchTcpConnection * conn = new EtchTcpConnection(NULL, &url);
  EtchSessionListener<EtchSocket>* mSessionListener = new MockListener();
  EtchSessionData* mPacketizer = new MockPacketizer();
  EtchTcpListener listener(&url);

  //START THE LISTENER
  listener.setSession(mSessionListener);
  listener.transportControl(new EtchString(EtchTcpListener::START_AND_WAIT_UP), new EtchInt32(1000));

  //START THE TRANSPORT
  conn->setSession(mPacketizer);
  conn->transportControl(new EtchString(EtchTcpConnection::START_AND_WAIT_UP), new EtchInt32(1000));

  conn->transportControl(new EtchString(EtchTcpConnection::STOP_AND_WAIT_DOWN), new EtchInt32(1000));
  //STOP THE LISTENER
  listener.transportControl(new EtchString(EtchTcpListener::STOP_AND_WAIT_DOWN), new EtchInt32(1000));

  listener.setSession(NULL);
  conn->setSession(NULL);

  delete mSessionListener;
  delete mPacketizer;
  delete conn;
}