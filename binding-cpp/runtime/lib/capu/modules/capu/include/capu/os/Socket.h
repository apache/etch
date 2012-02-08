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

#ifndef __SOCKET_H__
#define __SOCKET_H__

#define SOCKET_INC_HEADER
#include "arch/Socket.inc"
#undef SOCKET_INC_HEADER

#include "capu/Error.h"

namespace capu {

  class Socket {
#define SOCKET_INC_MEMBER
#include "arch/Socket.inc"
#undef SOCKET_INC_MEMBER
  public:

    /**
     * Default Constructor
     */
    inline Socket();

    /**
     * Destructor
     */
    inline ~Socket();

    /**
     * Send the messages
     * @param buffer    the content of message that will be sent to destination
     * @param length    the length of message that will be sent to destination
     * @return CAPU_OK if the sent is successful
     *         CAPU_EINVAL if the buffer is NULL
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     *         CAPU_ERROR otherwise
     */
    inline status_t send(unsigned char * buffer, int32_t length);

    /**
     * Receive message
     * @param buffer    buffer that will be used to store incoming message
     * @param length    buffer size
     * @param numBytes  number of bytes on socket
     * @return CAPU_OK if the receive is successfully executed
     *         CAPU_TIMEOUT if there has been a timeout
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     *         CAPU_ERROR otherwise
     */
    inline status_t receive(unsigned char * buffer, int32_t length, int32_t& numBytes);

    /**
     * close the socket
     * @return CAPU_OK if the socket is correctly closed
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     *         CAPU_ERROR otherwise
     */
    inline status_t close();

    /**
     * connect to the given address
     * @param dest_addr destination address of server
     * @param port      port number of service
     * @return  CAPU_OK if correctly connects to specified address
     *          CAPU_SOCKET_ECONNECT if the connection is not successful
     *          CAPU_SOCKET_EADDR if the given address is not resolved.
     *          CAPU_EINVAL if the dest_addr is NULL
     *          CAPU_SOCKET_ESOCKET if the socket is not created
     */
    inline status_t connect(unsigned char * dest_addr, uint16_t port);

    /**
     * Sets the maximum socket buffer in bytes. The kernel doubles this value (to allow space for bookkeeping overhead)
     * Set the receive buffer size
     * Sets buffer size information.
     * @return CAPU_OK if the buffer is successfully set for both receive and send operations
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     *         CAPU_ERROR otherwise
     */
    inline status_t setBufferSize(int32_t bufferSize);

    /**
     * Set the linger option for socket
     * Specifies whether the socket lingers on close() if data is present.
     * @return CAPU_OK if the LingerOption is successfully set
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     *         CAPU_ERROR otherwise
     */
    inline status_t setLingerOption(bool_t isLinger, int32_t linger);


    /**
     * Set no delay option
     * Specifies whether the Nagle algorithm used by TCP for send coalescing is to be disabled.
     * @return CAPU_OK if the NoDelay is successfully set
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     *         CAPU_ERROR otherwise
     */
    inline status_t setNoDelay(bool_t noDelay);

    /**
     * Set Keep Alive option
     * Keeps connections active by enabling periodic transmission of messages, if this is supported by the protocol.
     * @return CAPU_OK if the KeepAlive is successfully set
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     *         CAPU_ERROR otherwise
     */
    inline status_t setKeepAlive(bool_t keepAlive);

    /**
     * Set Timeout
     * Sets the timeout value that specifies the maximum amount of time an input function waits until it completes
     * @return CAPU_OK if the Timeout for receive operation is successfully set
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     *         CAPU_ERROR otherwise
     */
    inline status_t setTimeout(int32_t timeout);

    /**
     * get the send and receive buffer size
     * gets buffer size information.
     * @return CAPU_OK if the buffer is successfully set for both receive and send operations
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     *         CAPU_ERROR otherwise
     */
    inline status_t getBufferSize(int32_t& bufferSize);

    /**
     * get the linger option for socket
     * @return CAPU_OK if the LingerOption is successfully set
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     *         CAPU_ERROR otherwise
     */
    inline status_t getLingerOption(bool_t& isLinger, int32_t& linger);

    /**
     * Get no delay option
     * @return CAPU_OK if the NoDelay is successfully set
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     *         CAPU_ERROR otherwise
     */
    inline status_t getNoDelay(bool_t& noDelay);

    /**
     * Get Keep Alive option
     * @return CAPU_OK if the KeepAlive is successfully set
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     *         CAPU_ERROR otherwise
     */
    inline status_t getKeepAlive(bool_t& keepAlive);


    /**
     * Get Timeout
     * @return CAPU_OK if the Timeout for receive operation is successfully set
     *         CAPU_SOCKET_ESOCKET if the socket is not created
     *         CAPU_ERROR otherwise
     */
    inline status_t getTimeout(int32_t& timeout);
  };

#define SOCKET_INC_IMPL
#include "arch/Socket.inc"
#undef SOCKET_INC_IMPL
}

#endif /* Socket_H */

