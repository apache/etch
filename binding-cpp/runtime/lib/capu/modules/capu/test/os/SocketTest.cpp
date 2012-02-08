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

class ThreadClientTest {
public:
  //client thread to test data exchange between client and server

  inline void operator()(void * param) {
    capu::int32_t communication_variable;
    capu::int32_t numBytes = 0;
    //ALLOCATION AND SYNCH OF cient and  server
    capu::Socket *cli_socket = new capu::Socket();
    capu::Thread::Sleep(2000);
    //TRY TO CONNECT TO IPV6
    EXPECT_TRUE(cli_socket->connect((unsigned char *) "::1", 5000) == capu::CAPU_SOCKET_EADDR);
    //connects to he given id
    EXPECT_TRUE(cli_socket->connect((unsigned char *) "localhost", 5000) == capu::CAPU_OK);
    capu::int32_t i = 5;
    //send data
    EXPECT_TRUE(cli_socket->send((unsigned char*) &i, sizeof (capu::int32_t)) == capu::CAPU_OK);
    //receive
    EXPECT_TRUE(cli_socket->receive((unsigned char *) &communication_variable, sizeof (capu::int32_t), numBytes) == capu::CAPU_OK);
    //CHECK VALUE
    EXPECT_TRUE(communication_variable == 6);
    //socket close
    EXPECT_TRUE(cli_socket->close() == capu::CAPU_OK);
    //deallocating
    delete cli_socket;
  }
};

class ThreadTimeoutClientTest {
public:
  //timeout test

  inline void operator()(void * param) {
    capu::int32_t communication_variable;
    capu::int32_t numBytes = 0;
    //ALLOCATION AND SYNCH OF cient and  server
    capu::Socket *cli_socket = new capu::Socket();
    capu::Thread::Sleep(2000);
    //timeout is 5 second;
    cli_socket->setTimeout(5);
    //TRY TO CONNECT TO IPV6
    EXPECT_TRUE(cli_socket->connect((unsigned char *) "::1", 5000) == capu::CAPU_SOCKET_EADDR);
    //connects to he given id
    EXPECT_TRUE(cli_socket->connect((unsigned char *) "localhost", 5000) == capu::CAPU_OK);
    capu::int32_t i = 5;
    //send data
    EXPECT_TRUE(cli_socket->send((unsigned char*) &i, sizeof (capu::int32_t)) == capu::CAPU_OK);
    //receive
    EXPECT_TRUE(cli_socket->receive((unsigned char *) &communication_variable, sizeof (capu::int32_t), numBytes) == capu::CAPU_TIMEOUT);
    //socket close
    EXPECT_TRUE(cli_socket->close() == capu::CAPU_OK);
    //deallocating
    delete cli_socket;
  }
};

class ThreadServerTest {
public:
  //SERVER thread to test data exchange between client and server

  inline void operator()(void * param) {
    capu::int32_t communication_variable;
    capu::int32_t numBytes = 0;
    //server socket allocation
    capu::ServerSocket *socket = new capu::ServerSocket();

    //bind to given address
    EXPECT_TRUE(socket->bind(5000, "0.0.0.0") == capu::CAPU_OK);
    //start listening
    EXPECT_TRUE(socket->listen(5) == capu::CAPU_OK);
    //accept connection
    capu::Socket *new_socket = socket->accept();
    //receive data
    EXPECT_TRUE(new_socket->receive((unsigned char *) &communication_variable, sizeof (capu::int32_t), numBytes) != -1);
    //CHECK VALUE
    EXPECT_TRUE(communication_variable == 5);
    //update data
    communication_variable++;
    //send it back
    capu::Thread::Sleep(4000);
    EXPECT_TRUE(new_socket->send((unsigned char *) &communication_variable, sizeof (capu::int32_t)) == capu::CAPU_OK);
    //close session
    EXPECT_TRUE(new_socket->close() == capu::CAPU_OK);
    //deallocate session identifier
    delete new_socket;
    EXPECT_EQ(capu::CAPU_OK, socket->close());
    delete socket;
  }
};

class ThreadTimeoutServerTest {
public:
  //timeout test

  inline void operator()(void * param) {
    capu::int32_t communication_variable;
    capu::int32_t numBytes = 0;
    //server socket allocation
    capu::ServerSocket *socket = new capu::ServerSocket();

    //bind to given address
    EXPECT_TRUE(socket->bind(5000, "0.0.0.0") == capu::CAPU_OK);
    //start listening
    EXPECT_TRUE(socket->listen(5) == capu::CAPU_OK);
    //accept connection
    capu::Socket *new_socket = socket->accept();
    //receive data
    EXPECT_TRUE(new_socket->receive((unsigned char *) &communication_variable, sizeof (capu::int32_t), numBytes) != -1);
    //CHECK VALUE
    EXPECT_TRUE(communication_variable == 5);
    //update data
    communication_variable++;
    //send it back
    capu::Thread::Sleep(9000);
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
  EXPECT_TRUE(socket->connect(NULL, 5) == capu::CAPU_EINVAL);

  EXPECT_TRUE(socket->connect((unsigned char *)"www.test", 5) == capu::CAPU_SOCKET_EADDR);
  
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


  serverSocket->bind(5000, "0.0.0.0");
  serverSocket->listen(3);

  socket->connect((unsigned char *) "127.0.0.1", 5000);

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
  ThreadServerTest server;
  ThreadClientTest client;
  capu::Thread * server_thread = new capu::Thread(&server, NULL);
  capu::Thread * client_thread = new capu::Thread(&client, NULL);
  //Create two threads which will behave like client and server to test functionality
  server_thread->join();
  client_thread->join();

  delete client_thread;
  delete server_thread;
}

TEST(SocketAndServerSocket, TimeoutTest) {
  ThreadTimeoutServerTest server;
  ThreadTimeoutClientTest client;
  capu::Thread * server_thread = new capu::Thread(&server, NULL);
  capu::Thread * client_thread = new capu::Thread(&client, NULL);
  //Create two threads which will behave like client and server to test functionality
  server_thread->join();
  client_thread->join();

  delete client_thread;
  delete server_thread;
}


