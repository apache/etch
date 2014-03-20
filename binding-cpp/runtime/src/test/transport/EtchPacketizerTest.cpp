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
#include "transport/EtchPacketizer.h"
#include "common/EtchString.h"
#include "common/EtchError.h"
#include "util/EtchURL.h"
#include "transport/EtchTcpConnection.h"
#include "capu/util/SmartPointer.h"
#include "common/EtchSocket.h"
#include "transport/EtchSessionListener.h"
#include "transport/EtchTcpListener.h"
#include "transport/EtchSessionPacket.h"
#include "transport/EtchFlexBuffer.h"

using testing::_;
using testing::Invoke;

class MockListener3 : public virtual EtchSessionListener<EtchSocket> {
public:

  MockListener3(EtchTransport<EtchSessionListener<EtchSocket> >* transport) :
    mTransport(transport) {
    if(mTransport != NULL) {
      mTransport->setSession(this);
    }
  }

  virtual ~MockListener3() {
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

class MockMessagizer : public EtchSessionPacket {
public:

  MOCK_METHOD2(sessionPacket, status_t(capu::SmartPointer<EtchWho> receipent, capu::SmartPointer<EtchFlexBuffer> buf));

  MOCK_METHOD2(sessionQuery, status_t(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result));

  MOCK_METHOD2(sessionControl, status_t(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value));

  status_t sessionNotify(capu::SmartPointer<EtchObject> event) {
    return ETCH_OK;
  }
};

class EtchPacketizerTest
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

TEST_F(EtchPacketizerTest, constructorTest) {
  EtchURL u("tcp://127.0.0.1:4001");
  EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
  EtchSessionData* packetizer = new EtchPacketizer(mRuntime, conn, &u);

  delete conn;
  delete packetizer;

}

TEST_F(EtchPacketizerTest, TransportControlTest) {
  EtchURL u("tcp://127.0.0.1:4001");
  EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
  EtchPacketizer* packetizer = new EtchPacketizer(mRuntime, conn, &u);
  MockMessagizer mes;
  packetizer->setSession(&mes);

  EtchTcpListener* transport = new EtchTcpListener(mRuntime, &u);
  EtchSessionListener<EtchSocket>* listener = new MockListener3(transport);

  //Start the mock listener
  transport->transportControl(new EtchString(EtchTcpListener::START_AND_WAIT_UP()), new EtchInt32(1000));

  packetizer->transportControl(new EtchString(EtchPacketizer::START_AND_WAIT_UP()), new EtchInt32(1000));
  packetizer->transportControl(new EtchString(EtchPacketizer::STOP_AND_WAIT_DOWN()), new EtchInt32(1000));

  transport->transportControl(new EtchString(EtchTcpListener::STOP_AND_WAIT_DOWN()), new EtchInt32(1000));

  delete conn;
  delete packetizer;
  delete listener;
}

TEST_F(EtchPacketizerTest, TransportPacketTest) {
  EtchURL u("tcp://127.0.0.1:4001");
  EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
  EtchPacketizer* packetizer = new EtchPacketizer(mRuntime, conn, &u);
  MockMessagizer mes;
  packetizer->setSession(&mes);
  capu::SmartPointer<EtchFlexBuffer> buffer = new EtchFlexBuffer();

  //A packet is try to transmit data through not started transport
  buffer->put((capu::int8_t *)"_header_", packetizer->getHeaderSize());
  buffer->put((capu::int8_t *)"test", 4);
  buffer->setIndex(0);

  EXPECT_TRUE(packetizer->transportPacket(NULL, buffer) == ETCH_ERROR);

  delete conn;
  delete packetizer;
}

class MessagizerCorruptedCheck : public EtchSessionPacket {
public:
    
    status_t sessionPacket(capu::SmartPointer<EtchWho> receipent, capu::SmartPointer<EtchFlexBuffer> buf)
    {
        capu::uint32_t val;

        buf->getInteger(val);
        EXPECT_EQ(11u, val);

        buf->getInteger(val);
        EXPECT_EQ(1122u, val);

        buf->getInteger(val);
        EXPECT_EQ(112233u, val);

        buf->getInteger(val);
        EXPECT_EQ(11223344u, val);
        return ETCH_OK;
    }
    
    status_t sessionQuery (capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result)
    {
        return ETCH_OK;
    }
    
    status_t sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value)
    {
        return ETCH_OK;
    }
    
    status_t sessionNotify(capu::SmartPointer<EtchObject> event) {
        return ETCH_OK;
    }
};

class MockMessagizerCorrupted : public EtchSessionPacket {
public:
    MockMessagizerCorrupted() {
        // By default, all calls are delegated to the real object.
        ON_CALL(*this, sessionPacket(_, _)).WillByDefault(Invoke(&real_, &MessagizerCorruptedCheck::sessionPacket));
    }
    MOCK_METHOD2(sessionPacket, status_t(capu::SmartPointer<EtchWho> receipent, capu::SmartPointer<EtchFlexBuffer> buf));
    
    MOCK_METHOD2(sessionQuery, status_t(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result));
    
    MOCK_METHOD2(sessionControl, status_t(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value));
    
    status_t sessionNotify(capu::SmartPointer<EtchObject> event) {
        return ETCH_OK;
    }
private:
    MessagizerCorruptedCheck real_;
};


TEST_F(EtchPacketizerTest, SinglePackagesTest) {
    EtchURL u("tcp://127.0.0.1:4001");
    EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
    EtchPacketizer* packetizer = new EtchPacketizer(mRuntime, conn, &u);
    capu::SmartPointer<EtchFlexBuffer> buffer = new EtchFlexBuffer();
    EtchSessionPacket* mSessionPacker = new MockMessagizerCorrupted();
    packetizer->setSession(mSessionPacker);
    
    EXPECT_CALL(*(MockMessagizerCorrupted*)mSessionPacker, sessionPacket(_, _)).Times(1); //Expect two Clean packages

    buffer->putInt(packetizer->SIG());
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 24);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    packetizer->setSession(NULL);
    delete mSessionPacker;
    delete packetizer;
    delete conn;
}

TEST_F(EtchPacketizerTest, MutliplePackagesTest) {
    EtchURL u("tcp://127.0.0.1:4001");
    EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
    EtchPacketizer* packetizer = new EtchPacketizer(mRuntime, conn, &u);
    capu::SmartPointer<EtchFlexBuffer> buffer = new EtchFlexBuffer();
    EtchSessionPacket* mSessionPacker = new MockMessagizerCorrupted();
    packetizer->setSession(mSessionPacker);
    
    EXPECT_CALL(*(MockMessagizerCorrupted*)mSessionPacker, sessionPacket(_, _)).Times(3); //Expect two Clean packages
    
    //Build up a Correct Package and Attach a damaged Header behind it, which should lead to a bad SIG in the next loop
    buffer->putInt(packetizer->SIG());
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    buffer->putInt(packetizer->SIG());
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    buffer->putInt(packetizer->SIG());
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 72);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    packetizer->setSession(NULL);
    delete mSessionPacker;
    delete packetizer;
    delete conn;
}

TEST_F(EtchPacketizerTest, SplitPackagesTest) {
    EtchURL u("tcp://127.0.0.1:4001");
    EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
    EtchPacketizer* packetizer = new EtchPacketizer(mRuntime, conn, &u);
    capu::SmartPointer<EtchFlexBuffer> buffer = new EtchFlexBuffer();
    EtchSessionPacket* mSessionPacker = new MockMessagizerCorrupted();
    packetizer->setSession(mSessionPacker);
    
    EXPECT_CALL(*(MockMessagizerCorrupted*)mSessionPacker, sessionPacket(_, _)).Times(3); //Expect two Clean packages
    
    //Build up a Correct Package and Attach a damaged Header behind it, which should lead to a bad SIG in the next loop
    buffer->putInt(packetizer->SIG());
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    buffer->putInt(packetizer->SIG());
    buffer->putInt(16); //size
    buffer->putInt(11);

  
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 36);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    buffer->reset();
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    buffer->putInt(packetizer->SIG());
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 36);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    packetizer->setSession(NULL);
    delete mSessionPacker;
    delete packetizer;
    delete conn;
}

TEST_F(EtchPacketizerTest, SplitHeaderTest) {
    EtchURL u("tcp://127.0.0.1:4001");
    EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
    EtchPacketizer* packetizer = new EtchPacketizer(mRuntime, conn, &u);
    capu::SmartPointer<EtchFlexBuffer> buffer = new EtchFlexBuffer();
    EtchSessionPacket* mSessionPacker = new MockMessagizerCorrupted();
    packetizer->setSession(mSessionPacker);
    
    EXPECT_CALL(*(MockMessagizerCorrupted*)mSessionPacker, sessionPacket(_, _)).Times(3); //Expect two Clean packages
    
    //Build up a Correct Package and Attach a damaged Header behind it, which should lead to a bad SIG in the next loop
    buffer->putInt(packetizer->SIG());
    
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 4);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    buffer->reset();
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    buffer->putInt(packetizer->SIG());
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    buffer->putInt(packetizer->SIG());
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 68);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    packetizer->setSession(NULL);
    delete mSessionPacker;
    delete packetizer;
    delete conn;
}

TEST_F(EtchPacketizerTest, SplitMultiplePackagesTest) {
    EtchURL u("tcp://127.0.0.1:4001");
    EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
    EtchPacketizer* packetizer = new EtchPacketizer(mRuntime, conn, &u);
    capu::SmartPointer<EtchFlexBuffer> buffer = new EtchFlexBuffer();
    EtchSessionPacket* mSessionPacker = new MockMessagizerCorrupted();
    packetizer->setSession(mSessionPacker);
    
    EXPECT_CALL(*(MockMessagizerCorrupted*)mSessionPacker, sessionPacket(_, _)).Times(3); //Expect two Clean packages
    
    //Build up a Correct Package and Attach a damaged Header behind it, which should lead to a bad SIG in the next loop
    buffer->putInt(packetizer->SIG());
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    buffer->putInt(packetizer->SIG());
    buffer->putInt(16); //size
    buffer->putInt(11);
    
    
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 36);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    buffer->reset();
    buffer->putInt(1122);
    buffer->putInt(112233);
    
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 8);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    buffer->reset();
    buffer->putInt(11223344);
    buffer->putInt(packetizer->SIG());
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 28);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    packetizer->setSession(NULL);
    delete mSessionPacker;
    delete packetizer;
    delete conn;
}



TEST_F(EtchPacketizerTest, CorruptedPackagesTest_OnePacket) {
    EtchURL u("tcp://127.0.0.1:4001");
    EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
    EtchPacketizer* packetizer = new EtchPacketizer(mRuntime, conn, &u);
    capu::SmartPointer<EtchFlexBuffer> buffer = new EtchFlexBuffer();
    EtchSessionPacket* mSessionPacker = new MockMessagizerCorrupted();
    packetizer->setSession(mSessionPacker);

    EXPECT_CALL(*(MockMessagizerCorrupted*)mSessionPacker, sessionPacket(_, _)).Times(2); //Expect two Clean packages
    
    //Build up a Correct Package and Attach a damaged Header behind it, which should lead to a bad SIG in the next loop
    buffer->putInt(packetizer->SIG());
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    //CorruptedPackage Header
    buffer->putShort(0xBEEFu); //SIG = DEADBEEF
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 26);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    buffer->reset();
    buffer->putShort(0xDEADu); //Header SIG is now BEEFDEAD => wrong
    buffer->put((capu::int8_t*)"123456789123456789123456789", 27); //some bad Data
    buffer->putInt(packetizer->SIG()); //a fine package behind the corrupted one
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 53);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    packetizer->setSession(NULL);
    delete mSessionPacker;
    delete packetizer;
    delete conn;
}


TEST_F(EtchPacketizerTest, CorruptedPackagesTest_MultiplePacket) {
    EtchURL u("tcp://127.0.0.1:4001");
    EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
    EtchPacketizer* packetizer = new EtchPacketizer(mRuntime, conn, &u);
    capu::SmartPointer<EtchFlexBuffer> buffer = new EtchFlexBuffer();
    EtchSessionPacket* mSessionPacker = new MockMessagizerCorrupted();
    packetizer->setSession(mSessionPacker);
    
    EXPECT_CALL(*(MockMessagizerCorrupted*)mSessionPacker, sessionPacket(_, _)).Times(2); //Expect two Clean packages
    
    //Build up a Correct Package and Attach a damaged Header behind it, which should lead to a bad SIG in the next loop
    buffer->putInt(packetizer->SIG());
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    //CorruptedPackage Header
    buffer->putShort(0xBEEFu); //SIG = DEADBEEF
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 26);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    buffer->reset();
    buffer->putShort(0xDEADu); //Header SIG is now BEEFDEAD => wrong
    buffer->put((capu::int8_t*)"123456789123456789123456789", 27); //some bad Data
    buffer->putInt(0xBEEFDEAD);
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 33);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    buffer->reset();
    buffer->putShort(0xDEADu); //Header SIG is now DEADDEAD => wrong
    buffer->put((capu::int8_t*)"123456789123456789123456789", 27); //some bad Data
    buffer->putInt(0xBEEFDEAD); //Half the Header "DEAD" = the beginning of the correct Header
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 33);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    buffer->reset();
    buffer->putShort(0xBEEFu); //Fine with previous data
    buffer->putInt(16); //size
    buffer->putInt(11);
    buffer->putInt(1122);
    buffer->putInt(112233);
    buffer->putInt(11223344);
    buffer->setIndex(0);
    EXPECT_TRUE(buffer->getLength() == 22);
    EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);
    
    packetizer->setSession(NULL);
    delete mSessionPacker;
    delete packetizer;
    delete conn;
}


TEST_F(EtchPacketizerTest, PacketToLargeTest) {
  EtchURL u("tcp://127.0.0.1:4001?Packetizer.maxPktSize=18");
  EtchTransportData* conn = new EtchTcpConnection(mRuntime, NULL, &u);
  EtchPacketizer* packetizer = new EtchPacketizer(mRuntime, conn, &u);

  capu::SmartPointer<EtchFlexBuffer> buffer = new EtchFlexBuffer();
  EtchSessionPacket* mSessionPacker = new MockMessagizerCorrupted();
  packetizer->setSession(mSessionPacker);
  
  EXPECT_CALL(*(MockMessagizerCorrupted*)mSessionPacker, sessionPacket(_, _)).Times(2); //Expect two Clean packages
  
  //Build up a Correct Package and Attach a damaged Header behind it, which should lead to a bad SIG in the next loop
  buffer->putInt(packetizer->SIG());
  buffer->putInt(16); //size
  buffer->putInt(11);
  buffer->putInt(1122);
  buffer->putInt(112233);
  buffer->putInt(11223344);
  //CorruptedPackage Header
  buffer->putInt(packetizer->SIG());
  buffer->putInt(20); //size
  buffer->putInt(11);
  buffer->putInt(1122);
  buffer->putInt(112233);
  buffer->putInt(11223344);
  buffer->putInt(1122334455);
  buffer->putInt(packetizer->SIG());
  buffer->putInt(16); //size
  buffer->putInt(11);
  buffer->putInt(1122);
  buffer->putInt(112233);
  buffer->putInt(11223344);
  buffer->setIndex(0);
  EXPECT_TRUE(buffer->getLength() == 76);
  EXPECT_TRUE(packetizer->sessionData(NULL, buffer) == ETCH_OK);

  
  packetizer->setSession(NULL);
  delete mSessionPacker;
  delete packetizer;
  delete conn;
}
