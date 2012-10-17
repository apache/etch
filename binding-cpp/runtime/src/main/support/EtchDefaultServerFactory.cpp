
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


#include "support/EtchDefaultServerFactory.h"

/**
* Constructs the DefaultServerFactory.
* @param listener
* @param implFactory
*/
EtchDefaultServerFactory::EtchDefaultServerFactory( EtchTransport<EtchServerFactory>* listener, EtchSession* implFactory )
  : mListener(listener), mSession(NULL) {
    if(implFactory != NULL) {
      setSession( implFactory );
    }
    if(listener != NULL) {
      listener->setSession(this);
    }
}

EtchDefaultServerFactory::~EtchDefaultServerFactory() {
  if(mListener != NULL) {
    delete mListener;
  }
}

status_t EtchDefaultServerFactory::sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result) {
  if(mSession != NULL) {
    return mSession->sessionQuery(query, result);
  }
  return ETCH_UNSUPP_OP;
}

status_t EtchDefaultServerFactory::sessionControl( capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value ) {
  if(mSession != NULL) {
    return mSession->sessionControl( control, value );
  }
  return ETCH_UNSUPP_OP;
}

status_t EtchDefaultServerFactory::sessionNotify( capu::SmartPointer<EtchObject> event ) {
  if(mSession != NULL) {
    return mSession->sessionNotify( event );
  }
  return ETCH_UNSUPP_OP;
}

EtchSession* EtchDefaultServerFactory::getSession() {
  return mSession;
}

void EtchDefaultServerFactory::setSession( EtchSession* session ) {
  if(session != NULL) {
    this->mSession = session;
  }
}

status_t EtchDefaultServerFactory::transportControl( capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value ) {
  if(mListener == NULL) {
    return ETCH_ERANGE;
  }
  return mListener->transportControl(control, value);
}

status_t EtchDefaultServerFactory::transportNotify( capu::SmartPointer<EtchObject> event ) {
  if(mListener == NULL) {
    return ETCH_ERANGE;
  }
  return mListener->transportNotify(event);
}

status_t EtchDefaultServerFactory::transportQuery( capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result ) {
  if(mListener == NULL) {
    return ETCH_ERANGE;
  }
  return mListener->transportQuery(query, result);
}
