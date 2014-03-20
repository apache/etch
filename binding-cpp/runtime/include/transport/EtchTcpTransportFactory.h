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


#ifndef __ETCHTCPTRANSPORTFACTORY_H__
#define __ETCHTCPTRANSPORTFACTORY_H__

#include "common/EtchString.h"
#include "common/EtchConfig.h"
#include "support/EtchServerFactory.h"
#include "support/EtchStack.h"
#include "transport/EtchTransportMessage.h"
#include "transport/EtchTransportData.h"
#include "transport/EtchTransportPacket.h"
#include "transport/EtchPacketizer.h"
#include "transport/EtchTcpConnection.h"
#include "transport/EtchTcpListener.h"
#include "transport/EtchMessagizer.h"
#include "transport/EtchTransportFactory.h"
#include "util/EtchResources.h"

class EtchTcpTransportFactory
 : public EtchTransportFactory {
public:

  friend class MySessionListener;
  /**
   * Constructor
   * @param runtime
   */
  EtchTcpTransportFactory(EtchRuntime* runtime);

  /**
   * Constructor
   * @param runtime
   * @param secure indicates whether the connection should be secured by SSL or not
   */
  EtchTcpTransportFactory(EtchRuntime* runtime, capu::bool_t secure);

  /**
   * Destructor
   */
  virtual ~EtchTcpTransportFactory();

  /**
   * @see EtchTransportFactory
   */
  status_t newTransport(EtchString uri, EtchResources* resources, EtchTransportMessage*& result);

  /**
   * @see EtchTransportFactory
   */
  status_t newListener(EtchString uri, EtchResources* resources, EtchTransport<EtchServerFactory>*& result);

private:
  static const EtchString& SOCKET();
  EtchRuntime* mRuntime;
  const capu::bool_t mIsSecure;

  class MySessionListener : public EtchTransport<EtchServerFactory>, public EtchSessionListener<EtchSocket> {
  public:

    /**
     * Creates a new Session Listener
     * @param runtime
     * @param transport
     * @param uri
     * @param resources
     */
    MySessionListener(EtchRuntime* runtime, EtchTcpTransportFactory* factory, EtchTransport<EtchSessionListener<EtchSocket> > *transport,
            EtchString uri, EtchResources* resources);

     /**
      * Destructor
      */
    virtual ~MySessionListener();

    /**
     * @see EtchTransport
     */
    EtchServerFactory* getSession();

    /**
     * @see EtchTransport
     */
    void setSession(EtchServerFactory* session);

    /**
     * @see EtchTransport
     */
    status_t transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result);

    /**
     * @see EtchTransport
     */
    status_t transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);

    /**
     * @see EtchTransport
     */
    status_t transportNotify(capu::SmartPointer<EtchObject> event);

    /**
     * @see EtchSessionListener
     */
    status_t sessionAccepted(EtchSocket* connection);

    /**
     * @see EtchSessionListener
     */
    status_t sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result);

    /**
     * @see EtchSessionListener
     */
    status_t sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);

    /**
     * @see EtchSessionListener
     */
    status_t sessionNotify(capu::SmartPointer<EtchObject> event);

  private:
    EtchRuntime* mRuntime;
    EtchTcpTransportFactory* mFactory;
    EtchTransport<EtchSessionListener<EtchSocket> > *mTransport;
    EtchString mUri;
    EtchResources* mResources;
    EtchServerFactory* mSession;
    EtchList<EtchStack*>* mConnectionStacks;
  };

};

#endif

