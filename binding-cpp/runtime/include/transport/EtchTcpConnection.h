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

#ifndef __ETCHTCPCONNECTION_H__
#define __ETCHTCPCONNECTION_H__

#include "capu/util/Runnable.h"
#include "common/EtchString.h"
#include "common/EtchSocket.h"
#include "support/EtchRuntime.h"
#include "transport/EtchConnection.h"
#include "transport/EtchTransportData.h"
#include "transport/EtchFlexBuffer.h"
#include "transport/EtchSessionData.h"
#include "transport/EtchTcpOption.h"
#include "util/EtchURL.h"
#include "util/EtchResources.h"
#include "util/EtchLogger.h"


/**
 * Implementation of connection which handles a tcp connection.
 */
class EtchTcpConnection : public EtchConnection<EtchSessionData>, public EtchTransportData, public capu::Runnable {
public:

  /**
   * Constructs the Connection from a uri.
   * @param socket
   * @param uri
   * @param resources
   */
  EtchTcpConnection(EtchRuntime* runtime, EtchSocket* socket, EtchURL* uri);

  /**
   * Destructor
   */
  virtual ~EtchTcpConnection();

  /**
   * @see TransportData
   */
  status_t transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);


  /**
   * @see TransportData
   */
  status_t transportData(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchFlexBuffer> buf);

  /**
   * @see TransportData
   */
  status_t transportNotify(capu::SmartPointer<EtchObject> event);

  /**
   * @see TransportData
   */
  status_t transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result);

  /**
   * @return Session
   */
  EtchSessionData* getSession();

  /**
   * Sets session
   */
  void setSession(EtchSessionData* session);


  /**
   * @see EtchConnection
   */
  capu::bool_t isStarted();

  /**
   * @see EtchConnection
   */
  capu::bool_t isTerminated();

  /**
   * @see capu::thread
   */
  void run();

  /**
   * @see EtchConnection
   */
  status_t close();

  /**
   * @see EtchConnection
   */
  status_t setupSocket();

  /**
   * Sends some data to the remote end. If the connection is buffered, the
   * data is saved until the buffer is full or the buffer is flushed. If the
   * connection is not buffered, the data is sent to the operating system
   * immediately. The operating system may or may not transmit the data
   * immediately, depending upon the noDelay setting.
   * @param buf the bytes to be sent.
   * @param off the offset into buf of the first byte to send.
   * @param len the number of bytes to send.
   * @return ETCH_OK if the sent is successful
   *         ETCH_EINVAL if the buffer is NULL or socket is not opened
   *         ETCH_ERROR otherwise
   */
  status_t send(capu::int8_t* buf, capu::uint32_t off, capu::uint32_t len);

  /**
   * Sets the number of connection attempts
   * @param numAttempts
   */
  void setNumAttempts(capu::uint8_t numAttempts);

private:
  EtchRuntime *mRuntime;
  EtchString mHost;
  capu::uint16_t mPort;
  EtchSocket* mSocket;
  EtchTcpOption mOptions;
  capu::uint8_t mNumAttempts;

protected:

  status_t readSocket();

  status_t openSocket(capu::bool_t reconnect);

};

#endif /* ETCHTCPCONNECTION_H */

