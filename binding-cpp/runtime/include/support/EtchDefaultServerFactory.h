/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef __ETCHDEFAULTSERVERFACTORY_H__
#define __ETCHDEFAULTSERVERFACTORY_H__


#include "support/EtchServerFactory.h"

/**
 * Default implementation of ServerFactory. Used by Etch generated Helper files
 * to provide listeners with backstop implementations of Session methods which
 * forward to user's implementation factory.
 */
class EtchDefaultServerFactory
  : public EtchServerFactory {
public:
  /**
   * Constructs the DefaultServerFactory.
   * @param listener
   * @param implFactory
   */
  EtchDefaultServerFactory(EtchTransport<EtchServerFactory>* listener, EtchSession* implFactory);

  /**
   * Destructor
   */
  virtual ~EtchDefaultServerFactory();

  /**
   * @see EtchSession
   */
  virtual status_t sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result);

  /**
   * @see EtchSession
   */
  virtual status_t sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);

  /**
   * @see EtchSession
   */
  virtual status_t sessionNotify(capu::SmartPointer<EtchObject> event );

  /**
   * @see EtchTransport<EtchSession>
   */
  virtual EtchSession* getSession();

  /**
   * @see EtchTransport<EtchSession>
   */
  virtual void setSession( EtchSession* session );

  /**
   * @see EtchTransport<EtchSession>
   */
  virtual status_t transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);

  /**
   * @see EtchTransport<EtchSession>
   */
  virtual status_t transportNotify(capu::SmartPointer<EtchObject> event);

  /**
   * @see EtchTransport<EtchSession>
   */
  virtual status_t transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result);

private:
  EtchTransport<EtchServerFactory>* mListener;
  EtchSession* mSession;
};

#endif /* __ETCHDEFAULTSERVERFACTORY_H__*/
