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

#ifndef __ETCHSOCKET_H__
#define __ETCHSOCKET_H__

#include "capu/os/TcpSocket.h"
#include "common/EtchObject.h"

class EtchSocket : public EtchObject {
public:

  /**
   * EtchObjectType for EtchSocket.
   */
  static const EtchObjectType* TYPE();

  /**
   * Default Constructor
   */
  EtchSocket();

  /**
   * Copy Constructor
   */
  EtchSocket(const EtchSocket& other);


  /**
   * Destructor
   */
  ~EtchSocket();

  /**
   * Send the messages
   * @param buffer    the content of message that will be sent to destination
   * @param length    the length of message that will be sent to destination
   * @return ETCH_OK if the sent is successful
   *         ETCH_EINVAL if the buffer is NULL
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  status_t send(const char * buffer, capu::int32_t length);

  /**
   * Receive message
   * @param buffer    buffer that will be used to store incoming message
   * @param length    buffer size
   * @param numBytes  number of bytes on socket
   * @return ETCH_OK if the receive is successfully executed
   *         ETCH_TIMEOUT if there has been a timeout
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  status_t receive(char * buffer, capu::int32_t length, capu::int32_t& numBytes);

  /**
   * close the socket
   * @return ETCH_OK if the socket is correctly closed
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  status_t close();

  /**
   * connect to the given address
   * @param dest_addr destination address of server
   * @param port      port number of service
   * @return  ETCH_OK if correctly connects to specified address
   *          ETCH_SOCKET_ECONNECT if the connection is not successful
   *          ETCH_SOCKET_EADDR if the given address is not resolved.
   *          ETCH_EINVAL if the dest_addr is NULL
   *          ETCH_SOCKET_ESOCKET if the socket is not created
   */
  status_t connect(const char * dest_addr, capu::uint16_t port);

  /**
   * Sets the maximum socket buffer in bytes. The kernel doubles this value (to allow space for bookkeeping overhead)
   * Set the receive buffer size
   * Sets buffer size information.
   * @return ETCH_OK if the buffer is successfully set for both receive and send operations
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  status_t setBufferSize(capu::int32_t bufferSize);

  /**
   * Set the linger option for socket
   * Specifies whether the socket lingers on close() if data is present.
   * @return ETCH_OK if the LingerOption is successfully set
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  status_t setLingerOption(capu::bool_t isLinger, capu::int32_t linger);


  /**
   * Set no delay option
   * Specifies whether the Nagle algorithm used by TCP for send coalescing is to be disabled.
   * @return ETCH_OK if the NoDelay is successfully set
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  status_t setNoDelay(capu::bool_t noDelay);

  /**
   * Set Keep Alive option
   * Keeps connections active by enabling periodic transmission of messages, if this is supported by the protocol.
   * @return ETCH_OK if the KeepAlive is successfully set
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  status_t setKeepAlive(capu::bool_t keepAlive);

  /**
   * Set Timeout
   * Sets the timeout value that specifies the maximum amount of time an input function waits until it completes
   * @return ETCH_OK if the Timeout for receive operation is successfully set
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  status_t setTimeout(capu::int32_t timeout);

  /**
   * get the send and receive buffer size
   * gets buffer size information.
   * @return ETCH_OK if the buffer is successfully set for both receive and send operations
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  status_t getBufferSize(capu::int32_t& bufferSize);

  /**
   * get the linger option for socket
   * @return ETCH_OK if the LingerOption is successfully set
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  status_t getLingerOption(capu::bool_t& isLinger, capu::int32_t& linger);

  /**
   * Get no delay option
   * @return ETCH_OK if the NoDelay is successfully set
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  status_t getNoDelay(capu::bool_t& noDelay);

  /**
   * Get Keep Alive option
   * @return ETCH_OK if the KeepAlive is successfully set
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  status_t getKeepAlive(capu::bool_t& keepAlive);


  /**
   * Get Timeout
   * @return ETCH_OK if the Timeout for receive operation is successfully set
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  status_t getTimeout(capu::int32_t& timeout);

  /**
   * Get address from remote
   * @return ETCH_OK if the address was successfully obtained
   *         ETCH_SOCKET_ESOCKET if the socket is not valid
   *         ETCH_ERROR otherwise
   */
  status_t getRemoteAddress(capu::char_t** address);

  friend class EtchServerSocket;

private:
  capu::TcpSocket *mSocket;

  /**
   * Constructor
   */
  EtchSocket(capu::TcpSocket* soc);

};

#endif /* ETCHSOCKET_H */

