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

#ifndef __ETCHTRANSPORTFACTORY_H__
#define __ETCHTRANSPORTFACTORY_H__

#include "common/EtchTypes.h"
#include "support/EtchServerFactory.h"
#include "transport/EtchTransport.h"
#include "transport/EtchTransportMessage.h"
#include "util/EtchResources.h"
#include "util/EtchURL.h"

/**
 * Interface to transport factory.
 */
class EtchTransportFactory {
public:

  /**
   * Query term on the transport uri which defines a set of filters which
   * process messages as they move up and down the transport stack. Filter
   * names are separated by one or more of these characters: ",:;| \t\r\n".
   */
  const static EtchString& FILTER();

  /**
   * Constructs a new Transport stack topped by a TransportMessage
   * which is used by the remote service implementations to send
   * messages.
   * @param uri transport configuration parameters.
   * @param resources additional resources needed by the stack.
   * @param result the TransportMessage topping the transport stack.
   * @return ETCH_ENOT_EXIST if the required transport method is not available or not implememted
             ETCH_OK otherwise
   */
  static status_t getTransport(EtchRuntime* runtime, EtchString uri, EtchResources* resources, EtchTransportMessage*& result);

  /**
   * Constructs a new Transport Listener which is used to construct
   * server sessions.
   * @param uri listener configuration parameters.
   * @param resources additional resources needed by the listener.
   * @param result an out-of-band source which may be used to control the listener.
   * @return ETCH_ENOT_EXIST if the required listener is not available or not implememted
             ETCH_OK otherwise
   */
  static status_t getListener(EtchRuntime* runtime, EtchString uri, EtchResources* resources, EtchTransport<EtchServerFactory>*& result);


protected:
  /**
   * Destructor
   */
  virtual ~EtchTransportFactory();

  /**
   * Constructs a new Transport stack topped by a TransportMessage
   * which is used by the remote service implementations to send
   * messages.
   * @param uri transport configuration parameters.
   * @param resources additional resources needed by the stack.
   * @param result the TransportMessage topping the transport stack.
   * @return ETCH_ENOT_EXIST if the required transport method is not available or not implememted
             ETCH_OK otherwise
   */
  virtual status_t newTransport(EtchString uri, EtchResources* resources, EtchTransportMessage*& result) = 0;


  /**
   * Constructs a new Transport Listener which is used to construct
   * server sessions.
   * @param uri listener configuration parameters.
   * @param resources additional resources needed by the listener.
   * @param result an out-of-band source which may be used to control the listener.
   * @return ETCH_ENOT_EXIST if the required listener is not available or not implememted
             ETCH_OK otherwise
   */
  virtual status_t newListener(EtchString uri, EtchResources* resources, EtchTransport<EtchServerFactory>*& result) = 0;

  /**
   * Adds any message filters specified on the uri. They are added in order
   * from transport to session. The first filter is the session for Messagizer,
   * the second is the session for the first, etc. The last filter added is
   * returned, and becomes the TransportMessage for what follows.
   * @param transport
   * @param uri
   * @param resources
   * @param filter the newly added filter
   */
  status_t addFilters(EtchTransportMessage* transport, EtchURL* uri, EtchResources* resources, EtchTransportMessage*& filter);

private:

  /**
   * Adds any message filters specified on the uri. They are added in order
   * from transport to session. The first filter is the session for Messagizer,
   * the second is the session for the first, etc. The last filter added is
   * returned, and becomes the TransportMessage for what follows.
   * @param transport
   * @param uri
   * @param resources
   * @param filter the newly added filter
   * @return status
   */
  status_t addFilter(EtchTransportMessage* transport, EtchString* name, EtchURL* uri, EtchResources* resources, EtchTransportMessage*& filter);

  /**
   * Gets the named transport factory.
   * @param name the name of a configured transport factory.
   * @param the named transport factory.
   */
   static status_t getTransportFactory(EtchRuntime* runtime, const EtchString& name, EtchTransportFactory*& result);
};
#endif /* __ETCHTRANSPORTFACTORY_H__ */
