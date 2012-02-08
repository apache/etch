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
#include "capu/Error.h"

TEST(ServerSocket, CloseTest) {
  capu::ServerSocket *socket = new capu::ServerSocket();
  //try to close serversocket
  EXPECT_TRUE(socket->close() == capu::CAPU_OK);
  //try to close already closed socket
  EXPECT_TRUE(socket->close() == capu::CAPU_SOCKET_ESOCKET);
  delete socket;
}

TEST(ServerSocket, BindTest) {
  capu::ServerSocket *socket = new capu::ServerSocket();
  //try to bind on a invalid port
  EXPECT_TRUE(socket->bind(0, "0.0.0.0") == capu::CAPU_EINVAL);

  //try to bind on a valid port and address
  EXPECT_TRUE(socket->bind(5000) == capu::CAPU_OK);
  //call bind twice
  EXPECT_TRUE(socket->bind(5000, "127.0.0.1") == capu::CAPU_ERROR);

  delete socket;

  socket = new capu::ServerSocket();
  //try to bind 5 digit port
  EXPECT_TRUE(socket->bind(15000, "0.0.0.0") == capu::CAPU_OK);
  EXPECT_TRUE(socket->close() == capu::CAPU_OK);
  delete socket;

  socket = new capu::ServerSocket();
  //expect true if address is wrong
  EXPECT_TRUE(socket->bind(5000, "0.0.0.600") == capu::CAPU_SOCKET_EADDR);
  EXPECT_TRUE(socket->close() == capu::CAPU_OK);
  //deallocate socket
  delete socket;
}

TEST(ServerSocket, ListenTest) {
  capu::ServerSocket *socket = new capu::ServerSocket();

  //try to start listening on a unbound socket
  EXPECT_TRUE(socket->listen(3) == capu::CAPU_EINVAL);

  //faulty bind and listen
  EXPECT_TRUE(socket->bind(3000, "0.0.0.2564") == capu::CAPU_SOCKET_EADDR);
  EXPECT_TRUE(socket->listen(3) == capu::CAPU_EINVAL);
  //bind and listen in a correct way
  EXPECT_TRUE(socket->bind(3000, "0.0.0.0") == capu::CAPU_OK);
  EXPECT_TRUE(socket->listen(3) == capu::CAPU_OK);
  delete socket;
}
