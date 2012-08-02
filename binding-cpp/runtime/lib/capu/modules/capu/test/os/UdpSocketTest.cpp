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
#include "capu/os/UdpSocket.h"
#include "capu/os/Thread.h"
#include "capu/os/Mutex.h"
#include "capu/os/CondVar.h"

capu::Mutex mutex2;
capu::CondVar cv2;
capu::bool_t cond2;

class RandomPort
{
public:
  /**
   * Gets a Random Port between 1024 and 10024
   */
  static capu::uint16_t get()
  {
    return (rand() % 10000) + 30000; // 0-1023 = Well Known, 1024-49151 = User, 49152 - 65535 = Dynamic
  }
};


class ThreadClientUdpTest : public capu::Runnable {
  capu::uint16_t port;
public:
  //client thread to test data exchange between client and server
  ThreadClientUdpTest(capu::uint16_t port) : port(port) {}
  void operator()(void * param) {
    capu::int32_t communication_variable;
    capu::int32_t numBytes = 0;
    capu::UdpSocket *clientSocket = new capu::UdpSocket();

    capu::int32_t i = 5;
    //try to connect to ipv6
    EXPECT_TRUE(clientSocket->send((unsigned char*) &i, sizeof (capu::int32_t), "::1", port) == capu::CAPU_SOCKET_EADDR);

    //wait for other side to start up
    mutex2.lock();
    while (!cond2) { 
      cv2.wait(&mutex2);
    }
    cond2 = false;
    mutex2.unlock();
    //send data
    EXPECT_TRUE(clientSocket->send((unsigned char*) &i, sizeof (capu::int32_t), "127.0.0.1", port) == capu::CAPU_OK);

    //receive
    capu::status_t result = capu::CAPU_ERROR;

    result = clientSocket->receive((unsigned char *) &communication_variable, sizeof (capu::int32_t), numBytes, 0);
    EXPECT_EQ(capu::CAPU_OK,result);

    //check value
    EXPECT_EQ(6,communication_variable);

    mutex2.lock();
    cond2 = true;
    cv2.signal();
    mutex2.unlock();

    //socket close
    EXPECT_EQ(capu::CAPU_OK, clientSocket->close());

    delete clientSocket;
  }
};

class ThreadTimeoutClientUdpTest : public capu::Runnable {
  capu::uint16_t port;
public:
  //timeout test
  ThreadTimeoutClientUdpTest(capu::uint16_t port) : port(port) {}
  void operator()(void * param) {
    capu::int32_t communication_variable;
    capu::int32_t numBytes = 0;
    capu::Thread::Sleep(1000);
    //ALLOCATION AND SYNCH OF cient and  server
    capu::UdpSocket *cli_socket = new capu::UdpSocket();
    //timeout is 2 second;
    cli_socket->setTimeout(2);
    capu::int32_t i = 5;
    
    //wait for other side to start up
    mutex2.lock();
    while (!cond2) { 
      cv2.wait(&mutex2);
    }
    cond2 = false;
    mutex2.unlock();
    
    //send data
    EXPECT_EQ(capu::CAPU_OK, cli_socket->send((unsigned char*) &i, sizeof (capu::int32_t), "localhost", port));
    
    //receive
    EXPECT_EQ(capu::CAPU_ETIMEOUT, cli_socket->receive((unsigned char *) &communication_variable, sizeof (capu::int32_t), numBytes, 0));

    mutex2.lock();
    cond2 = true;
    cv2.signal();
    mutex2.unlock();

    //socket close
    EXPECT_TRUE(cli_socket->close() == capu::CAPU_OK);
    //deallocating
    delete cli_socket;
  }
};

class ThreadServerUdpTest : public capu::Runnable {
  capu::uint16_t port;
public:
  //SERVER thread to test data exchange between client and server
  ThreadServerUdpTest(capu::uint16_t port) : port(port) {}
  void operator()(void * param) {
    capu::int32_t communication_variable;
    capu::int32_t numBytes = 0;
    //server socket allocation
    capu::UdpSocket *serverSocket = new capu::UdpSocket();

    //bind to given address
    EXPECT_TRUE(serverSocket->bind(port, "127.0.0.1") == capu::CAPU_OK);

    //receive data
    capu::UdpSocket::SocketAddrInfo remoteSocket;
    //receive
    capu::status_t result = capu::CAPU_ERROR;
    
    //send signal that server is ready to receive
    mutex2.lock();
    cond2 = true;
    cv2.signal();
    mutex2.unlock();
    result = serverSocket->receive((unsigned char *) &communication_variable, sizeof (capu::int32_t), numBytes, &remoteSocket);
    EXPECT_TRUE(result == capu::CAPU_OK);

    EXPECT_STREQ("127.0.0.1", remoteSocket.addr);

    //check value
    EXPECT_TRUE(communication_variable == 5);

    //update data
    communication_variable++;

    //send it back
    EXPECT_TRUE(serverSocket->send((unsigned char*) &communication_variable, sizeof (capu::int32_t), remoteSocket) == capu::CAPU_OK);

    mutex2.lock();
    while (!cond2) { 
      cv2.wait(&mutex2);
    }
    cond2 = false;
    mutex2.unlock();

    //close session
    EXPECT_TRUE(serverSocket->close() == capu::CAPU_OK);
    delete serverSocket;
  }
};

class ThreadTimeoutServerUdpTest : public capu::Runnable {
  capu::uint16_t port;
public:
  //timeout test
  ThreadTimeoutServerUdpTest(capu::uint16_t port) : port(port) {}
  void operator()(void * param) {
    capu::int32_t communication_variable;
    capu::int32_t numBytes = 0;
    //server socket allocation
    capu::UdpSocket *serverSocket = new capu::UdpSocket();

    //bind to given address
    EXPECT_TRUE(serverSocket->bind(port) == capu::CAPU_OK);

    //receive data
    capu::status_t result = capu::CAPU_ERROR;
    
    //send signal that server is ready to receive
    mutex2.lock();
    cond2 = true;
    cv2.signal();
    mutex2.unlock();
    result = serverSocket->receive((unsigned char *) &communication_variable, sizeof (capu::int32_t), numBytes, NULL);
    EXPECT_TRUE(result == capu::CAPU_OK);

    //check value
    EXPECT_TRUE(communication_variable == 5);

    mutex2.lock();
    while (!cond2) { 
      cv2.wait(&mutex2);
    }
    cond2 = false;
    mutex2.unlock();

    //close session
    EXPECT_TRUE(serverSocket->close() == capu::CAPU_OK);

    delete serverSocket;
  }
};


TEST(UdpSocket, CloseReceiveAndSendTest) {
  capu::uint16_t port = RandomPort::get();
  capu::UdpSocket *socket = new capu::UdpSocket();
  capu::int32_t i = 0;
  capu::int32_t numBytes = 0;
  EXPECT_TRUE(socket->close() == capu::CAPU_OK);
  //try to send data via closed socket
  EXPECT_TRUE(socket->send((unsigned char *) "asda", 4, "localhost", port) == capu::CAPU_SOCKET_ESOCKET);
  //try to receive data from closed socket
  EXPECT_TRUE(socket->receive((unsigned char *) &i, 4, numBytes, NULL) == capu::CAPU_SOCKET_ESOCKET);
  //Deallocation of socket
  delete socket;
}

TEST(UdpSocket, SetAndGetPropertiesTest) {
  capu::UdpSocket *socket = new capu::UdpSocket();
  //TRY TO CHANGE THE PROPERTIES OF NOT CONNECTED SOCKET
  EXPECT_TRUE(socket->setBufferSize(1024) == capu::CAPU_OK);
  EXPECT_TRUE(socket->setTimeout(90) == capu::CAPU_OK);

  capu::int32_t int_tmp;

  //CHECK THE PROPERTIES ARE CORRECTLY SET
  EXPECT_TRUE(socket->getBufferSize(int_tmp) == capu::CAPU_OK);
  //On Linux the kernel adjust the buffer size and set it to doubles of given size (at least)
  //therefore we have to check here for >=
  EXPECT_TRUE(int_tmp >= 1024);

  EXPECT_TRUE(socket->getTimeout(int_tmp) == capu::CAPU_OK);
  EXPECT_TRUE(int_tmp == 90);

  socket->close();
  //TRY TO CHANGE THE PROPERTIES OF CLOSED SOCKET
  EXPECT_TRUE(socket->setBufferSize(1024) == capu::CAPU_SOCKET_ESOCKET);
  EXPECT_TRUE(socket->setTimeout(90) == capu::CAPU_SOCKET_ESOCKET);
  //TRY TO GET PROPERTIES OF CLOSED SOCKET
  EXPECT_TRUE(socket->getBufferSize(int_tmp) == capu::CAPU_SOCKET_ESOCKET);
  EXPECT_TRUE(socket->getTimeout(int_tmp) == capu::CAPU_SOCKET_ESOCKET);

  delete socket;
}

TEST(UdpSocketAndUdpServerSocket, CommunicationTest) {
  cond2 = false;
  capu::uint16_t port = RandomPort::get();
  ThreadServerUdpTest server(port);
  ThreadClientUdpTest client(port);
  capu::Thread * server_thread = new capu::Thread(&server, NULL);
  capu::Thread * client_thread = new capu::Thread(&client, NULL);
  //Create two threads which will behave like client and server to test functionality
  server_thread->join();
  client_thread->join();

  delete client_thread;
  delete server_thread;
}

TEST(UdpSocketAndUdpServerSocket, TimeoutTest) {
  cond2 = false;
  capu::uint16_t port = RandomPort::get();
  ThreadTimeoutServerUdpTest server(port);
  ThreadTimeoutClientUdpTest client(port);
  capu::Thread * server_thread = new capu::Thread(&server, NULL);
  capu::Thread * client_thread = new capu::Thread(&client, NULL);
  //Create two threads which will behave like client and server to test functionality
  server_thread->join();
  client_thread->join();

  delete client_thread;
  delete server_thread;
}


