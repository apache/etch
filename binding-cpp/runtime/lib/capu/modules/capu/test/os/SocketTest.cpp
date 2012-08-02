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
#include "capu/os/ServerSocket.h"
#include "capu/os/Thread.h"
#include "capu/os/Socket.h"
#include "capu/os/Mutex.h"
#include "capu/os/CondVar.h"

capu::Mutex mutex;
capu::CondVar cv;
capu::bool_t cond = false;

class RandomPort
{
public:
  /**
   * Gets a Random Port between 1024 and 10024
   */
  static capu::uint16_t get()
  {
    return (rand() % 10000) + 40000; // 0-1023 = Well Known, 1024-49151 = User, 49152 - 65535 = Dynamic
  }
};

class ThreadClientTest : public capu::Runnable {

capu::int16_t port;
public:
  //client thread to test data exchange between client and server
  ThreadClientTest(capu::int16_t port) : port(port) {}

  void run() {
    capu::int32_t communication_variable;
    capu::int32_t numBytes = 0;
    //ALLOCATION AND SYNCH OF cient and  server
    capu::Socket *cli_socket = new capu::Socket();

    //TRY TO CONNECT TO IPV6
    EXPECT_TRUE(cli_socket->connect((unsigned char *) "::1", port) == capu::CAPU_SOCKET_EADDR);
    //connects to he given id
    capu::status_t result = capu::CAPU_ERROR;
    //wait for server to start up
    mutex.lock();
    while (!cond) { 
      cv.wait(&mutex);
    }
    cond = false;
    mutex.unlock();
    result = capu::CAPU_ERROR;
    capu::int32_t attemps = 0;
    while (result != capu::CAPU_OK && attemps < 10) {
      result = cli_socket->connect((unsigned char *) "localhost", port);
      attemps++;
      capu::Thread::Sleep(50);
    }
    EXPECT_TRUE(result == capu::CAPU_OK);

    capu::int32_t i = 5;
    //send data
    EXPECT_TRUE(cli_socket->send((unsigned char*) &i, sizeof (capu::int32_t)) == capu::CAPU_OK);

    //receive
    capu::status_t res = cli_socket->receive((unsigned char *) &communication_variable, sizeof (capu::int32_t), numBytes);
    EXPECT_EQ(capu::CAPU_OK,res);

    //CHECK VALUE
    EXPECT_EQ(6,communication_variable);
    
    mutex.lock();
    cond = true;
    cv.signal();
    mutex.unlock();

    //socket close
    EXPECT_TRUE(cli_socket->close() == capu::CAPU_OK);
    //deallocating
    delete cli_socket;
  }
};

class ThreadTimeoutClientTest : public capu::Runnable {
  capu::int16_t port;
public:
  //timeout test
  ThreadTimeoutClientTest(capu::int16_t port) : port(port) {}

  void run() {
    capu::int32_t communication_variable;
    capu::int32_t numBytes = 0;
    capu::Socket *cli_socket = new capu::Socket();
    //timeout is 2 second;
    cli_socket->setTimeout(2);
    //connects to he given id
    capu::status_t result = capu::CAPU_ERROR;
    //wait for server to start up
    mutex.lock();
    while (!cond) { 
      cv.wait(&mutex);
    }
    cond = false;
    mutex.unlock();
    result = cli_socket->connect((unsigned char *) "localhost", port);
    EXPECT_TRUE(result == capu::CAPU_OK);

    capu::int32_t i = 5;

    //send data
    EXPECT_TRUE(cli_socket->send((unsigned char*) &i, sizeof (capu::int32_t)) == capu::CAPU_OK);

    //receive
    EXPECT_EQ(capu::CAPU_ETIMEOUT, cli_socket->receive((unsigned char *) &communication_variable, sizeof (capu::int32_t), numBytes));

    //client has received timeout, server can close socket
    mutex.lock();
    cond = true;
    cv.signal();
    mutex.unlock();
    //socket close
    EXPECT_TRUE(cli_socket->close() == capu::CAPU_OK);
    //deallocating
    delete cli_socket;
  }
};

class ThreadServerTest : public capu::Runnable {
  capu::int16_t port;
public:
  //SERVER thread to test data exchange between client and server
  ThreadServerTest(capu::int16_t port) : port(port) {}

  void run() {
    capu::int32_t communication_variable;
    capu::int32_t numBytes = 0;
    //server socket allocation
    capu::ServerSocket *socket = new capu::ServerSocket();

    //bind to given address
    EXPECT_TRUE(socket->bind(port, "0.0.0.0") == capu::CAPU_OK);
    //start listening
    EXPECT_TRUE(socket->listen(5) == capu::CAPU_OK);
    //accept connection

    //server is ready to accept clients
    mutex.lock();
    cond = true;
    cv.signal();
    mutex.unlock();
    capu::Socket *new_socket = socket->accept();

    //receive data
    capu::status_t result = capu::CAPU_ERROR;

    result = new_socket->receive((unsigned char *) &communication_variable, sizeof (capu::int32_t), numBytes);
    EXPECT_TRUE(result == capu::CAPU_OK);
    //CHECK VALUE
    EXPECT_TRUE(communication_variable == 5);
    //update data
    communication_variable++;

    //send it back
    EXPECT_TRUE(new_socket->send((unsigned char *) &communication_variable, sizeof (capu::int32_t)) == capu::CAPU_OK);

    //wait with close until client has received data
    mutex.lock();
    while (!cond) { 
      cv.wait(&mutex);
    }
    cond = false;
    mutex.unlock();
    //close session
    EXPECT_TRUE(new_socket->close() == capu::CAPU_OK);
    //deallocate session identifier
    delete new_socket;
    EXPECT_EQ(capu::CAPU_OK, socket->close());
    delete socket;
  }
};

class ThreadTimeoutServerTest : public capu::Runnable {
  capu::int16_t port;

public:
  //timeout test
  ThreadTimeoutServerTest(capu::int16_t port) : port(port) {}
  inline void run() {
    capu::int32_t communication_variable;
    capu::int32_t numBytes = 0;
    //server socket allocation
    capu::ServerSocket *socket = new capu::ServerSocket();

    //bind to given address
    EXPECT_TRUE(socket->bind(port, "0.0.0.0") == capu::CAPU_OK);
    //start listening
    EXPECT_TRUE(socket->listen(5) == capu::CAPU_OK);

    //server is ready to accept clients
    mutex.lock();
    cond = true;
    cv.signal();
    mutex.unlock();
    //accept connection
    capu::Socket *new_socket = socket->accept();
    capu::status_t result = capu::CAPU_ERROR;
    
    result = new_socket->receive((unsigned char *) &communication_variable, sizeof (capu::int32_t), numBytes);
    EXPECT_EQ(capu::CAPU_OK, result);
    //CHECK VALUE
    EXPECT_TRUE(communication_variable == 5);
    
    //wait for timeout on client side
    mutex.lock();
    while (!cond) { 
    cv.wait(&mutex);
    }
    cond = false;
    mutex.unlock();

    //close session
    EXPECT_TRUE(new_socket->close() == capu::CAPU_OK);
    //deallocate session identifier
    delete new_socket;
    EXPECT_EQ(capu::CAPU_OK, socket->close());
    delete socket;
  }
};

TEST(Socket, ConnectTest) {
  capu::Socket *socket = new capu::Socket();
  //pass null
  capu::uint16_t port = RandomPort::get();
  EXPECT_TRUE(socket->connect(NULL, port) == capu::CAPU_EINVAL);

  EXPECT_TRUE(socket->connect((unsigned char *)"www.test", port) == capu::CAPU_SOCKET_EADDR);

  delete socket;
}

TEST(Socket, CloseReceiveAndSendTest) {
  capu::Socket *socket = new capu::Socket();
  capu::int32_t i = 0;
  capu::int32_t numBytes = 0;
  EXPECT_TRUE(socket->close() == capu::CAPU_OK);
  //try to send data via closed socket
  EXPECT_TRUE(socket->send((unsigned char *) "asda", 4) == capu::CAPU_SOCKET_ESOCKET);
  //try to receive data from closed socket
  EXPECT_TRUE(socket->receive((unsigned char *) &i, 4, numBytes) == capu::CAPU_SOCKET_ESOCKET);
  //Deallocation of socket
  delete socket;
}

TEST(Socket, SetAndGetPropertiesTest) {
  capu::Socket *socket = new capu::Socket();
  capu::uint16_t port = RandomPort::get();
  capu::ServerSocket *serverSocket = new capu::ServerSocket();
  //TRY TO CHANGE THE PROPERTIES OF NOT CONNECTED SOCKET
  EXPECT_TRUE(socket->setBufferSize(1024) == capu::CAPU_OK);
  EXPECT_TRUE(socket->setKeepAlive(true) == capu::CAPU_OK);
  EXPECT_TRUE(socket->setLingerOption(true, 90) == capu::CAPU_OK);
  EXPECT_TRUE(socket->setNoDelay(false) == capu::CAPU_OK);
  EXPECT_TRUE(socket->setTimeout(90) == capu::CAPU_OK);

  capu::int32_t int_tmp;
  capu::bool_t bool_tmp;

  //CHECK THE PROPERTIES ARE CORRECTLY SET
  EXPECT_TRUE(socket->getBufferSize(int_tmp) == capu::CAPU_OK);


  //On Linux the kernel adjust the buffer size and set it to doubles of given size (at least)
  //therefore we have to check here for >=
  EXPECT_TRUE(int_tmp >= 1024);
  EXPECT_TRUE(socket->getKeepAlive(bool_tmp) == capu::CAPU_OK);
  EXPECT_TRUE(bool_tmp == true);
  EXPECT_TRUE(socket->getLingerOption(bool_tmp, int_tmp) == capu::CAPU_OK);
  EXPECT_TRUE(int_tmp == 90);
  EXPECT_TRUE(bool_tmp == true);
  EXPECT_TRUE(socket->getNoDelay(bool_tmp) == capu::CAPU_OK);
  EXPECT_TRUE(bool_tmp == false);
  EXPECT_TRUE(socket->getTimeout(int_tmp) == capu::CAPU_OK);
  EXPECT_TRUE(int_tmp == 90);


  serverSocket->bind(port, "0.0.0.0");
  serverSocket->listen(3);

  socket->connect((unsigned char *) "127.0.0.1", port);

  //TRY TO CHANGE THE PROPERTIES OF CONNECTED SOCKET
  EXPECT_TRUE(socket->setBufferSize(2024) == capu::CAPU_OK);
  EXPECT_TRUE(socket->setKeepAlive(false) == capu::CAPU_OK);

  EXPECT_TRUE(socket->setLingerOption(false, 0) == capu::CAPU_OK);
  EXPECT_TRUE(socket->setNoDelay(true) == capu::CAPU_OK);
  EXPECT_TRUE(socket->setTimeout(92) == capu::CAPU_OK);

  //CHECK THE PROPERTIES ARE CORRECTLY SET
  EXPECT_TRUE(socket->getBufferSize(int_tmp) == capu::CAPU_OK);
  //kernel adjust the buffer size and set it to doubles of given size (at least)

  EXPECT_TRUE(int_tmp >= 2024);
  EXPECT_TRUE(socket->getKeepAlive(bool_tmp) == capu::CAPU_OK);
  EXPECT_TRUE(bool_tmp == false);
  EXPECT_TRUE(socket->getLingerOption(bool_tmp, int_tmp) == capu::CAPU_OK);
  EXPECT_TRUE(bool_tmp == false);
  EXPECT_TRUE(socket->getNoDelay(bool_tmp) == capu::CAPU_OK);
  EXPECT_TRUE(bool_tmp == true);
  EXPECT_TRUE(socket->getTimeout(int_tmp) == capu::CAPU_OK);
  EXPECT_TRUE(int_tmp == 92);

  socket->close();
  //TRY TO CHANGE THE PROPERTIES OF CLOSED SOCKET
  EXPECT_TRUE(socket->setBufferSize(1024) == capu::CAPU_SOCKET_ESOCKET);
  EXPECT_TRUE(socket->setKeepAlive(true) == capu::CAPU_SOCKET_ESOCKET);
  EXPECT_TRUE(socket->setLingerOption(true, 90) == capu::CAPU_SOCKET_ESOCKET);
  EXPECT_TRUE(socket->setNoDelay(false) == capu::CAPU_SOCKET_ESOCKET);
  EXPECT_TRUE(socket->setTimeout(90) == capu::CAPU_SOCKET_ESOCKET);
  //TRY TO GET PROPERTIES OF CLOSED SOCKET
  EXPECT_TRUE(socket->getBufferSize(int_tmp) == capu::CAPU_SOCKET_ESOCKET);
  EXPECT_TRUE(socket->getKeepAlive(bool_tmp) == capu::CAPU_SOCKET_ESOCKET);
  EXPECT_TRUE(socket->getLingerOption(bool_tmp, int_tmp) == capu::CAPU_SOCKET_ESOCKET);
  EXPECT_TRUE(socket->getNoDelay(bool_tmp) == capu::CAPU_SOCKET_ESOCKET);
  EXPECT_TRUE(socket->getTimeout(int_tmp) == capu::CAPU_SOCKET_ESOCKET);

  delete socket;
  delete serverSocket;
}

TEST(SocketAndServerSocket, CommunicationTest) {
  cond = false;
  capu::uint16_t port = RandomPort::get();
  ThreadServerTest server(port);
  ThreadClientTest client(port);
  capu::Thread * server_thread = new capu::Thread(&server);
  server_thread->start();
  capu::Thread * client_thread = new capu::Thread(&client);
  client_thread->start();

  //Create two threads which will behave like client and server to test functionality
  server_thread->join();
  client_thread->join();

  delete client_thread;
  delete server_thread;
}

TEST(SocketAndServerSocket, TimeoutTest) {
  cond = false;
  capu::uint16_t port = RandomPort::get();
  ThreadTimeoutServerTest server(port);
  ThreadTimeoutClientTest client(port);
  capu::Thread * server_thread = new capu::Thread(&server);
  server_thread->start();
  capu::Thread * client_thread = new capu::Thread(&client);
  client_thread->start();
  //client_thread two threads which will behave like client and server to test functionality
  server_thread->join();
  client_thread->join();

  delete client_thread;
  delete server_thread;
}


