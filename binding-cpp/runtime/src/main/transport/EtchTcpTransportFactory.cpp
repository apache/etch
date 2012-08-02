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
#include "transport/EtchTcpTransportFactory.h"

const EtchString EtchTcpTransportFactory::SOCKET("TcpTransportFactory.socket");

EtchTcpTransportFactory::EtchTcpTransportFactory()
: mIsSecure(false) {

}

EtchTcpTransportFactory::EtchTcpTransportFactory(capu::bool_t secure)
: mIsSecure(secure) {

}

EtchTcpTransportFactory::~EtchTcpTransportFactory() {

}

status_t EtchTcpTransportFactory::newTransport(EtchString uri, EtchResources* resources, EtchTransportMessage*& result) {
  EtchURL u(uri);

  EtchObject* socket = NULL;
  if (resources->get((EtchString &) SOCKET, socket) != ETCH_OK) {
    return ETCH_ENOT_EXIST;
  }

  EtchTransportData *c = NULL;

  if (mIsSecure) {
    //TODO : secure communication via ssl sockets
    return ETCH_ENOT_EXIST;
  } else {
    c = new EtchTcpConnection((EtchSocket*) socket, &u);
  }

  EtchTransportPacket* p = new EtchPacketizer(c, &u);

  EtchTransportMessage* m = new EtchMessagizer(p, &u, resources);

  //TODO: ADD FILTERS HERE

  EtchObject* obj = NULL;
  EtchString tmp(EtchTransport<EtchSocket>::VALUE_FACTORY);
  if (resources->get(tmp, obj) != ETCH_OK) {
    c->setSession(NULL);
    p->setSession(NULL);
    m->setSession(NULL);
    delete c;
    delete p;
    delete m;
    return ETCH_ENOT_EXIST;
  }
  EtchValueFactory *vf = (EtchValueFactory*) obj;

  vf->lockDynamicTypes();
  result = m;
  return ETCH_OK;
}

status_t EtchTcpTransportFactory::newListener(EtchString uri, EtchResources* resources, EtchTransport<EtchServerFactory>*& result) {
  EtchURL u(uri);

  EtchTransport<EtchSessionListener<EtchSocket> > *l = NULL;
  if (mIsSecure) {
    //TODO secure communication
    return ETCH_ENOT_EXIST;
  } else {
    l = new EtchTcpListener(&u);
  }

  result = new MySessionListener(l, uri, resources, mIsSecure);
  if (result == NULL) {
    return ETCH_ENOT_EXIST;
  }
  return ETCH_OK;
}

EtchTcpTransportFactory::MySessionListener::MySessionListener(EtchTransport<EtchSessionListener<EtchSocket> > *transport, EtchString uri, EtchResources* resources, capu::bool_t secure)
: mTransport(transport), mUri(uri), mResources(resources), mIsSecure(secure) {
  if (transport != NULL) {
    transport->setSession(this);
  }
}

EtchServerFactory* EtchTcpTransportFactory::MySessionListener::getSession() {
  return mSession;
}

void EtchTcpTransportFactory::MySessionListener::setSession(EtchServerFactory* session) {
  mSession = session;
}

status_t EtchTcpTransportFactory::MySessionListener::transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result) {
  return mTransport->transportQuery(query, result);
}

status_t EtchTcpTransportFactory::MySessionListener::transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
  return mTransport->transportControl(control, value);
}

status_t EtchTcpTransportFactory::MySessionListener::transportNotify(capu::SmartPointer<EtchObject> event) {
  return mTransport->transportNotify(event);
}

status_t EtchTcpTransportFactory::MySessionListener::sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result) {
  return mSession->sessionQuery(query, result);
}

status_t EtchTcpTransportFactory::MySessionListener::sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
  return mSession->sessionControl(control, value);
}

status_t EtchTcpTransportFactory::MySessionListener::sessionNotify(capu::SmartPointer<EtchObject> event) {
  return mSession->sessionNotify(event);
}

status_t EtchTcpTransportFactory::MySessionListener::sessionAccepted(EtchSocket* connection) {
  if (connection == NULL) {
    return ETCH_ERROR;
  }

  EtchResources *res = new EtchResources(mResources);
  EtchObject *obj = NULL;
  if (res->put((EtchString &) SOCKET, connection, obj) != ETCH_OK) {
    delete res;
    return ETCH_ERROR;
  }

  EtchValueFactory* vf = NULL;
  if (mSession->newValueFactory(&mUri, vf) != ETCH_OK) {
    delete res;
    return ETCH_ERROR;
  }
  if (res->put((EtchString &) VALUE_FACTORY, vf, obj)) {
    delete res;
    return ETCH_ERROR;
  }

  EtchTransportMessage *t = NULL;
  if (getTransport(mUri, res, t) != ETCH_OK) {
    delete res;
    return ETCH_ERROR;
  }

  return mSession->newServer(t, &mUri, res);
}