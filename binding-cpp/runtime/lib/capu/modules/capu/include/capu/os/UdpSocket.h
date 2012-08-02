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

#ifndef __UDPSOCKET_H__
#define __UDPSOCKET_H__

#define UDPSOCKET_INC_HEADER
#include "arch/UdpSocket.inc"
#undef UDPSOCKET_INC_HEADER

#include "capu/Error.h"

namespace capu {

  class UdpSocket{
#define UDPSOCKET_INC_MEMBER
#include "arch/UdpSocket.inc"
#undef UDPSOCKET_INC_MEMBER

  public:

    struct SocketAddrInfo
    {
      uint16_t port;
      char     addr[16];
    };

    /**
     * Default Constructor
     */
    inline UdpSocket();

    /**
     * Destructor
     */
    inline ~UdpSocket();

    /**
     *
     * @param port indicates port number
     * @param address to bind if it is not given it accepts all connection from any address
     * @return CAPU_OK  if the server socket is successfully bound
     *         CAPU_SOCKET_EADDR if the addr is faulty
     *         CAPU_ERROR  if the socket is already bound
     *         CAPU_EINVAL if the port is equal to 0
     *         CAPU_SOCKET_ESOCKET if the socket has not been created successfully
     */
    inline status_t bind(uint16_t port, const char *addr = NULL);

    /**
     * Send the messages
     * @param buffer          the content of message that will be sent to destination
     * @param length          the length of message that will be sent to destination
     * @param receiverAddr    the destination
     * @return CAPU_OK if the sent is successful
     *         CAPU_EINVAL if the buffer is NULL
     *         CAPU_SOCKET_EADDR if the given address is not resolved.
     *         CAPU_SOCKET_ESOCKET if the socket has not been created successfully
     *         CAPU_ERROR otherwise
     */
    inline status_t send(unsigned char *buffer, int32_t length, SocketAddrInfo& receiverAddr);

    /**
     * Send the messages
     * @param buffer          the content of message that will be sent to destination
     * @param length          the length of message that will be sent to destination
     * @param receiverAddr    the destination address
     * @param receiverPort    the destination port
     * @return CAPU_OK if the sent is successful
     *         CAPU_EINVAL if the buffer is NULL
     *         CAPU_SOCKET_EADDR if the given address is not resolved.
     *         CAPU_SOCKET_ESOCKET if the socket has not been created successfully
     *         CAPU_ERROR otherwise
     */
    inline status_t send(unsigned char *buffer, int32_t length, const char *receiverAddr, uint16_t receiverPort);

    /**
     * Receive message
     * @param buffer    buffer that will be used to store incoming message
     * @param length    buffer size
     * @param numBytes  number of bytes on socket
     * @param sender    out parameter for the socket address of the sender
     * @return CAPU_OK if the receive is successfully executed
     *         CAPU_TIMEOUT if there has been a timeout
     *         CAPU_SOCKET_ESOCKET if the socket has not been created successfully
     *         CAPU_ERROR otherwise
     */
    inline status_t receive(unsigned char *buffer, int32_t length, int32_t& numBytes, SocketAddrInfo* sender);

    /**
     * close the socket
     * @return CAPU_OK if the socket is correctly closed
     *         CAPU_SOCKET_ESOCKET if the socket has not been created successfully
     *         CAPU_ERROR otherwise
     */
    inline status_t close();


    /**
     * Sets the maximum socket buffer in bytes. The kernel doubles this value (to allow space for bookkeeping overhead)
     * Set the receive buffer size
     * Sets buffer size information.
     * @return CAPU_OK if the buffer is successfully set for both receive and send operations
     *         CAPU_SOCKET_ESOCKET if the socket has not been created successfully
     *         CAPU_ERROR otherwise
     */
    inline status_t setBufferSize(int32_t bufferSize);

    /**
     * Set Timeout
     * Sets the timeout value that specifies the maximum amount of time an input function waits until it completes
     * @return CAPU_OK if the Timeout for receive operation is successfully set
     *         CAPU_SOCKET_ESOCKET if the socket has not been created successfully
     *         CAPU_ERROR otherwise
     */
    inline status_t setTimeout(int32_t timeout);

    /**
     * get the send and receive buffer size
     * gets buffer size information.
     * @return CAPU_OK if the buffer is successfully set for both receive and send operations
     *         CAPU_SOCKET_ESOCKET if the socket has not been created successfully
     *         CAPU_ERROR otherwise
     */
    inline status_t getBufferSize(int32_t& bufferSize);

    /**
     * Get Timeout
     * @return CAPU_OK if the Timeout for receive operation is successfully set
     *         CAPU_SOCKET_ESOCKET if the socket has not been created successfully
     *         CAPU_ERROR otherwise
     */
    inline status_t getTimeout(int32_t& timeout);
  };


#define UDPSOCKET_INC_IMPL
#include "arch/UdpSocket.inc"
#undef UDPSOCKET_INC_IMPL
}

#endif /* __UDPSOCKET_H__ */

