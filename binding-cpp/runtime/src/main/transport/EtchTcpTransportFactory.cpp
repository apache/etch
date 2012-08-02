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

const EtchString& EtchTcpTransportFactory::SOCKET() {
  static const EtchString name("TcpTransportFactory.socket");
  return name;
}

EtchTcpTransportFactory::EtchTcpTransportFactory(EtchRuntime* runtime)
: mRuntime(runtime)
, mIsSecure(false) {
}

EtchTcpTransportFactory::EtchTcpTransportFactory(EtchRuntime* runtime, capu::bool_t secure)
: mRuntime(runtime)
, mIsSecure(secure) {

}

EtchTcpTransportFactory::~EtchTcpTransportFactory() {
}

status_t EtchTcpTransportFactory::newTransport(EtchString uri, EtchResources* resources, EtchTransportMessage*& result) {
  EtchURL u(uri);

  status_t status;
  EtchObject* socket = NULL;
  status = resources->get(SOCKET(), socket);

  EtchTransportData *c = NULL;

  if (mIsSecure) {
    //TODO : secure communication via ssl sockets
    return ETCH_EUNIMPL;
  } else {
    // TODO add runtime
    c = new EtchTcpConnection(NULL, (EtchSocket*) socket, &u);
  }

  EtchTransportPacket* p = new EtchPacketizer(c, &u);

  EtchTransportMessage* m = new EtchMessagizer(p, &u, resources);

  //TODO: ADD FILTERS HERE

  EtchObject* obj = NULL;

  if (resources->get(EtchTransport<EtchSocket>::VALUE_FACTORY(), obj) != ETCH_OK) {
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
    return ETCH_EUNIMPL;
  } else {
    l = new EtchTcpListener(&u);
  }

  result = new MySessionListener(mRuntime, l, uri, resources, mIsSecure);
  if (result == NULL) {
    return ETCH_ERROR;
  }
  return ETCH_OK;
}

EtchTcpTransportFactory::MySessionListener::MySessionListener(EtchRuntime* runtime, EtchTransport<EtchSessionListener<EtchSocket> > *transport, EtchString uri, EtchResources* resources, capu::bool_t secure)
: mRuntime(runtime), mTransport(transport), mUri(uri), mResources(resources), mIsSecure(secure) {
  if (mTransport != NULL) {
    mTransport->setSession(this);
  }
}

EtchServerFactory* EtchTcpTransportFactory::MySessionListener::getSession() {
  return mSession;
}

EtchTcpTransportFactory::MySessionListener::~MySessionListener() {
  if(mTransport != NULL) {
    delete mTransport;
  }
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

  // put socket to the resources
  EtchObject *obj = NULL;
  if (res->put(SOCKET(), connection, obj) != ETCH_OK) {
    delete res;
    return ETCH_ERROR;
  }

  // create value vatory and put it to the resources
  EtchValueFactory* vf = NULL;
  if(mSession->newValueFactory(mUri, vf) != ETCH_OK) {
    delete res;
    return ETCH_ERROR;
  }
  if(res->put(EtchTransport<EtchSocket>::VALUE_FACTORY(), vf, obj) != ETCH_OK)
  {
    delete vf;
    delete res;
    return ETCH_ERROR;
  }

  EtchURL u = EtchURL(mUri);

  EtchObject* socket = NULL;
  if (res->get(SOCKET(), socket) != ETCH_OK) {
    return ETCH_ENOT_EXIST;
  }
  // TODO check if we should register a new stack to the runtime
  
  EtchTransportData *c = NULL;
  if (mIsSecure) {
    //TODO : secure communication via ssl sockets
    return ETCH_EUNIMPL;
  } else {
    c = new EtchTcpConnection(mRuntime, (EtchSocket*) socket, &u);
  }

  EtchTransportPacket* p = new EtchPacketizer(c, &u);

  EtchTransportMessage* m = new EtchMessagizer(p, &u, res);

  //TODO: ADD FILTERS HERE

  if (res->get(EtchTransport<EtchSocket>::VALUE_FACTORY(), obj) != ETCH_OK) {
    c->setSession(NULL);
    p->setSession(NULL);
    m->setSession(NULL);
    delete c;
    delete p;
    delete m;
    return ETCH_ENOT_EXIST;
  }
  vf->lockDynamicTypes();

  return mSession->newServer(mRuntime, m, mUri, res);
}
