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


#ifndef __ETCHTCPLISTENER_H__
#define __ETCHTCPLISTENER_H__

#include "capu/os/TcpServerSocket.h"
#include "capu/os/TcpSocket.h"
#include "capu/util/Runnable.h"

#include "common/EtchError.h"
#include "common/EtchObject.h"
#include "common/EtchSocket.h"
#include "common/EtchServerSocket.h"
#include "transport/EtchTransport.h"
#include "transport/EtchSessionListener.h"
#include "transport/EtchConnection.h"
#include "util/EtchURL.h"
#include "util/EtchResources.h"

class EtchRuntime;

/**
 * Implementation of a connection which handles a socket listener.
 */
class EtchTcpListener
: public EtchConnection<EtchSessionListener<EtchSocket> >,
public virtual EtchTransport<EtchSessionListener<EtchSocket> >, public capu::Runnable {
public:

  /**
   * Default Constructor
   */
  EtchTcpListener(EtchRuntime* runtime, EtchURL *url);

  /**
   * Destructor
   */
  virtual ~EtchTcpListener();

  /**
   * @see EtchConnection
   */
  virtual capu::bool_t isStarted();

  /**
   * @see EtchConnection
   */
  virtual capu::bool_t isTerminated();

  virtual status_t close();

  /**
   * @see TransportData
   */
  virtual status_t transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);

  /**
   * @see TransportData
   */
  virtual status_t transportNotify(capu::SmartPointer<EtchObject> event);

  /**
   * @see TransportData
   */
  virtual status_t transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result);

  /**
   * @return Session
   */
  virtual EtchSessionListener<EtchSocket>* getSession();

  /**
   * Sets session
   */
  virtual void setSession(EtchSessionListener<EtchSocket>* session);

  virtual void run();

  /**
   * @see EtchConnection
   */
  virtual status_t setupSocket();

protected:

  /**
   * @see EtchConnection
   */
  virtual status_t openSocket(capu::bool_t reconnect);

  /**
   * @see EtchConnection
   */
  virtual status_t readSocket();

public:
  /**
   * Query term for URI to specify backlog value to ServerSocket. The value
   * is "TcpListener.backlog". The backlog must be >= 0 and somewhat
   * reasonable. The value 0 specifies the system default, the same as if you
   * leave the term off altogether.
   *
   * Example: tcp://host:port?TcpListener.backlog=250
   *
   * @see #TcpListener(String, Resources)
   */
  const static EtchString& BACKLOG();

  const static EtchString& CONNECTION_CHECK();

private:
  class ConnectionChecker : public capu::Runnable {
  public:
    /**
     * Constructor
     * @param instance of TcpListener
     */
    ConnectionChecker(EtchTcpListener* listener);

    /**
     * Destructor
     */
    virtual ~ConnectionChecker();

    /**
     * @see Runnable
     */
    void run();
  private:
    EtchTcpListener *mListener;
  };

  friend class ConnectionChecker;

  EtchRuntime* mRuntime;
  EtchServerSocket* mSocket;
  capu::uint16_t mPort;
  capu::uint8_t mBackLog;
  capu::Thread* mConnectionCheckerThread;
  ConnectionChecker* mConnectionChecker;

};

#endif /* ETCHTCPLISTENER_H */

