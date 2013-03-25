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

#ifndef __ETCHSERVERSOCKET_H__
#define __ETCHSERVERSOCKET_H__

#include "capu/os/TcpServerSocket.h"
#include "capu/os/TcpSocket.h"
#include "common/EtchSocket.h"

class EtchServerSocket : public EtchObject {
public:

  /**
   * EtchObjectType for EtchServerSocket.
   */
  static const EtchObjectType* TYPE();

  /**
   * Default Constructor
   */
  EtchServerSocket();

  /**
   * Copy Constructor
   */
  EtchServerSocket(const EtchServerSocket& other);

  /**
   * Destructor
   */
  virtual ~EtchServerSocket();

  /**
   * The program flow will be blocked until a connection arrives
   * Programmer is responsible for deallocating memory of returning socket.
   * @return Socket if a connection is accepted
   *         NULL otherwise
   */
  EtchSocket* accept();

  /**
   * Close the socket which is used for accepting connection
   * @return ETCH_OK if the socket is successfully closed
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   */
  status_t close();

  /**
   *
   * @param port indicates port number
   * @param address to bind if it is not given it accepts all connection from any address
   * @return ETCH_OK  if the server socket is successfully bound
   *         ETCH_SOCKET_EADDR if the addr is faulty
   *         ETCH_ERROR  if the socket is already bound
   *         ETCH_EINVAL if the addr is NULL or port is equal to 0
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   */
  status_t bind(capu::uint16_t port, const char * addr = NULL);

  /**
   *
   * @param backlog (maximum length of the queue of pending connections)
   * @return ETCH_OK if the listen is successful
   *         ETCH_SOCKET_ESOCKET if the socket is not created
   *         ETCH_ERROR otherwise
   */
  status_t listen(capu::uint8_t backlog);

private:
  capu::TcpServerSocket mServerSocket;

};

#endif /* ETCHSERVERSOCKET_H */

