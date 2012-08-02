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

#include "transport/EtchTransportFactory.h"
#include "transport/EtchTcpTransportFactory.h"

const EtchString EtchTransportFactory::FILTER("filter");


EtchTransportFactory::~EtchTransportFactory() {

}

status_t EtchTransportFactory::addFilters(EtchTransportMessage* transport, EtchURL* uri, EtchResources* resources, EtchTransportMessage *& filter) {
  //TODO: Add implementation
  return ETCH_EUNIMPL;
}

status_t EtchTransportFactory::addFilter(EtchTransportMessage* transport, EtchString* name, EtchURL* uri, EtchResources* resources, EtchTransportMessage *& filter) {
  //TODO: Add implementation
  return ETCH_EUNIMPL;
}

status_t EtchTransportFactory::getTransport(EtchString uri, EtchResources* resources, EtchTransportMessage*& result) {
  EtchURL u(uri);
  EtchTransportFactory* f = NULL;
  if (getTransportFactory(u.getScheme(), f) != ETCH_OK) {
    return ETCH_ENOT_EXIST;
  }
  if (f == NULL) {
    return ETCH_ENOT_EXIST;
  }
  return f->newTransport(uri, resources, result);
}

status_t EtchTransportFactory::getListener(EtchString uri, EtchResources* resources, EtchTransport<EtchServerFactory>*& result) {
  EtchURL u(uri);
  EtchTransportFactory* f = NULL;
  if (getTransportFactory(u.getScheme(), f) != ETCH_OK) {
    return ETCH_ENOT_EXIST;
  }
  if (f == NULL) {
    return ETCH_ENOT_EXIST;
  }
  return f->newListener(uri, resources, result);
}

status_t EtchTransportFactory::getTransportFactory(const EtchString& name, EtchTransportFactory*& result) {
  EtchString tmp("tcp");
  if (name.equals(&tmp)) {
    result = new EtchTcpTransportFactory();
    return ETCH_OK;
  }
  //TODO: result = new EtchUdpTransprtFactory();
  return ETCH_ENOT_EXIST;

}
