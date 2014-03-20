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
#include "support/EtchTransportHelper.h"
#include "support/EtchStackServer.h"
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
  status_t status;

  if (resources == NULL) {
    return ETCH_ERROR;
  }

  EtchStack* stack = NULL;
  status = resources->get(EtchStack::STACK(), (EtchObject*&)stack);
  if (status != ETCH_OK) {
    return status;
  }

  EtchURL u(uri);

  EtchObject* socket = NULL;
  status = resources->get(SOCKET(), socket);

  EtchTransportData *c = NULL;

  if (mIsSecure) {
    //TODO : secure communication via ssl sockets
    return ETCH_EUNIMPL;
  } else {
    c = new EtchTcpConnection(mRuntime, (EtchSocket*) socket, &u);
  }
  stack->setTransportData(c);

  EtchTransportPacket* p = new EtchPacketizer(mRuntime, c, &u);
  stack->setTransportPacket(p);

  EtchTransportMessage* m = new EtchMessagizer(mRuntime, p, &u, resources);
  stack->setTransportMessage(m);

  //TODO: ADD FILTERS HERE

  EtchObject* obj = NULL;

  if (resources->get(EtchTransport<EtchSocket>::VALUE_FACTORY(), obj) != ETCH_OK) {
    delete stack;
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
    l = new EtchTcpListener(mRuntime, &u);
  }

  result = new MySessionListener(mRuntime, this, l, uri, resources);
  if (result == NULL) {
    return ETCH_ERROR;
  }
  return ETCH_OK;
}

EtchTcpTransportFactory::MySessionListener::MySessionListener(EtchRuntime* runtime, EtchTcpTransportFactory* factory, EtchTransport<EtchSessionListener<EtchSocket> > *transport, EtchString uri, EtchResources* resources)
: mRuntime(runtime), mFactory(factory), mTransport(transport), mUri(uri), mResources(resources) {
  if (mTransport != NULL) {
    mTransport->setSession(this);
  }
  mConnectionStacks = new EtchList<EtchStack*>();
}

EtchServerFactory* EtchTcpTransportFactory::MySessionListener::getSession() {
  return mSession;
}

EtchTcpTransportFactory::MySessionListener::~MySessionListener() {
  
  if(mTransport != NULL) {
    delete mTransport;
  }
  if(mFactory != NULL) {
    delete mFactory;
  }
  if (mResources != NULL) {
    EtchTransportHelper::DestroyResources(mRuntime, mResources);
  }

  EtchList<EtchStack*>::Iterator it = mConnectionStacks->begin();
  while (it.hasNext()) {
    EtchStack* st = NULL;
    it.next(&st);
    if (st != NULL) {
      delete st;
    }
  }
  delete mConnectionStacks;
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
  if (event->equals(&EtchTcpListener::CONNECTION_CHECK())) {
    //go through the list of connection and check if the connection is still dead and we have to clean the stack up
    EtchList<EtchStack*>::Iterator it = mConnectionStacks->begin();
    while (it.hasNext()) {
      EtchStack* stack = NULL;
      status_t res = it.current(stack);
      if (res == ETCH_OK) {
        EtchTcpConnection* con = (EtchTcpConnection*) stack->getTransportData();
        if (con != NULL) {
          if (con->isTerminated()) {
            //delete all instances for this stack
            delete stack;
            //remote stack from list
            mConnectionStacks->removeAt(it);
          }
        }
      }
      it.next();
    }
    return ETCH_OK;
  }

  return mSession->sessionNotify(event);

}

status_t EtchTcpTransportFactory::MySessionListener::sessionAccepted(EtchSocket* connection) {
  if (connection == NULL) {
    return ETCH_ERROR;
  }

  status_t status;

  EtchStackServer *stack = new EtchStackServer();

  EtchResources *res = new EtchResources(mResources);
  stack->setResources(res);

  // put socket to the resources
  EtchObject *obj = NULL;
  status = res->put(SOCKET(), connection, obj);
  if (status != ETCH_OK) {
    delete stack;
    return ETCH_ERROR;
  }

  // create value vatory and put it to the resources
  EtchValueFactory* vf = NULL;
  status = mSession->newValueFactory(mUri, vf);
  if(status != ETCH_OK) {
    delete stack;
    return ETCH_ERROR;
  }
  stack->setValueFactory(vf);

  status = res->put(EtchTransport<EtchSocket>::VALUE_FACTORY(), vf, obj);
  if(status != ETCH_OK) {
    delete stack;
    return ETCH_ERROR;
  }

  EtchObject* old = NULL;
  status = res->put(EtchStack::STACK(), stack, old);
  if (status != ETCH_OK) {
    delete stack;
    return ETCH_ERROR;
  }
  if (old != NULL) {
    delete old;
  }

  //add stack container to list
  mConnectionStacks->add(stack);

  EtchTransportMessage *m = NULL;
  status = mFactory->newTransport(mUri, res, m);
  if (status != ETCH_OK) {
    delete stack;
    return status;
  }
  ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getMailboxContext(), "New communication stack for the accepted connection has been created");

  return mSession->newServer(mRuntime, m, mUri, res);
}
