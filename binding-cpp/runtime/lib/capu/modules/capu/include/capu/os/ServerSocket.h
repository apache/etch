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

#ifndef __SERVERSOCKET_H__
#define __SERVERSOCKET_H__

#define SERVER_SOCKER_INC_HEADER
#include "arch/ServerSocket.inc"
#undef SERVER_SOCKER_INC_HEADER

#include "capu/os/Socket.h"
#include "capu/Error.h"

namespace capu {

  class ServerSocket {
#define SERVER_SOCKET_INC_MEMBER
#include "arch/ServerSocket.inc"
#undef SERVER_SOCKET_INC_MEMBER

  public:

    /**
     * Constructor of ServerSocket
     */
    inline ServerSocket();

    /**
     * Destructor of Server Socket
     */
    inline ~ServerSocket();

    /**
     * The program flow will be blocked until a connection arrives
     * Programmer is responsible for deallocating memory of returning socket.
     * @return Socket if a connection is accepted
     *         NULL otherwise
     */
    inline Socket* accept();

    /**
     * Close the socket which is used for accepting connection
     * @return CAPU_OK if the socket is successfully closed
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     */
    inline status_t close();

    /**
     *
     * @param port indicates port number
     * @param address to bind if it is not given it accepts all connection from any address
     * @return CAPU_OK  if the server socket is successfully bound
     *         CAPU_SOCKET_EADDR if the addr is faulty
     *         CAPU_ERROR  if the socket is already bound
     *         CAPU_EINVAL if the addr is NULL or port is equal to 0
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     */
    inline status_t bind(uint16_t port, const char * addr = NULL);

    /**
     * 
     * @param backlog (maximum length of the queue of pending connections)
     * @return CAPU_OK if the listen is successful
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     *         CAPU_ERROR otherwise
     */
    inline status_t listen(uint8_t backlog);

  };

#define SERVER_SOCKET_INC_IMPL
#include "arch/ServerSocket.inc"
#undef SERVER_SOCKET_INC_IMPL
}
#endif

