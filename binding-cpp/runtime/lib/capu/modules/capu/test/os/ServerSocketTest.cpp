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

class RandomPort
{
public:
  /**
   * Gets a Random Port between 1024 and 10024
   */
  static capu::uint16_t get()
  {
    return (rand() % 10000) + 20000; // 0-1023 = Well Known, 1024-49151 = User, 49152 - 65535 = Dynamic
  }
};

TEST(ServerSocket, CloseTest) {
  capu::ServerSocket *socket = new capu::ServerSocket();
  //try to close serversocket
  EXPECT_TRUE(socket->close() == capu::CAPU_OK);
  //try to close already closed socket
  EXPECT_TRUE(socket->close() == capu::CAPU_SOCKET_ESOCKET);
  delete socket;
}

TEST(ServerSocket, BindTest) {
  capu::uint16_t port = RandomPort::get();
  capu::ServerSocket *socket = new capu::ServerSocket();
  //try to bind on a invalid port
  EXPECT_TRUE(socket->bind(0, "0.0.0.0") == capu::CAPU_EINVAL);

  //try to bind on a valid port and address
  EXPECT_TRUE(socket->bind(port) == capu::CAPU_OK);
  //call bind twice
  EXPECT_TRUE(socket->bind(port, "127.0.0.1") == capu::CAPU_ERROR);

  delete socket;
  capu::uint16_t port2 = RandomPort::get() + 10000;
  socket = new capu::ServerSocket();
  //try to bind 5 digit port
  EXPECT_TRUE(socket->bind(port2, "0.0.0.0") == capu::CAPU_OK);
  EXPECT_TRUE(socket->close() == capu::CAPU_OK);
  delete socket;

  socket = new capu::ServerSocket();
  //expect true if address is wrong
  EXPECT_TRUE(socket->bind(port, "0.0.0.600") == capu::CAPU_SOCKET_EADDR);
  EXPECT_TRUE(socket->close() == capu::CAPU_OK);
  //deallocate socket
  delete socket;
}

TEST(ServerSocket, ListenTest) {
  capu::uint16_t port = RandomPort::get();
  capu::ServerSocket *socket = new capu::ServerSocket();

  //try to start listening on a unbound socket
  EXPECT_TRUE(socket->listen(3) == capu::CAPU_EINVAL);

  //faulty bind and listen
  EXPECT_TRUE(socket->bind(port, "0.0.0.2564") == capu::CAPU_SOCKET_EADDR);
  EXPECT_TRUE(socket->listen(3) == capu::CAPU_EINVAL);
  //bind and listen in a correct way
  EXPECT_TRUE(socket->bind(port, "0.0.0.0") == capu::CAPU_OK);
  EXPECT_TRUE(socket->listen(3) == capu::CAPU_OK);
  delete socket;
}
