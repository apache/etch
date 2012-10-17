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

#ifndef __ETCHMESSAGIZER_H__
#define __ETCHMESSAGIZER_H__

#include "capu/os/Mutex.h"
#include "common/EtchException.h"
#include "serialization/EtchBinaryTaggedDataInput.h"
#include "serialization/EtchBinaryTaggedDataOutput.h"
#include "transport/EtchFormat.h"
#include "transport/EtchSessionPacket.h"
#include "transport/EtchSessionMessage.h"
#include "transport/EtchTransportMessage.h"
#include "transport/EtchTransportPacket.h"
#include "util/EtchResources.h"
#include "util/EtchURL.h"

class EtchRuntime;

class EtchMessagizer : public EtchSessionPacket, public EtchTransportMessage {
public:

  /**
   * Name of format name in uri or resources. The value is "Messagizer.format".
   * The result of looking up this name should be a String.
   *
   * @see #Messagizer(TransportPacket, String, Resources)
   */
  const static EtchString& FORMAT();

  /**
   * Constructs the Messagizer with null handler and tagged data format
   * constructed from the format factory.
   * @param transport used to deliver packets to the peer
   * @param uri the uri being used to configure the transport stack.
   * @param resources the associated set of resources for this service.
   */
  EtchMessagizer(EtchRuntime* runtime, EtchTransportPacket* transport, EtchURL* uri, EtchResources* resources);

  /**
   * Destructor
   */
  virtual ~EtchMessagizer();

  /**
   * @return Transport layer for messagizer
   */
  EtchTransportPacket* getTransport();

  /**
   * @return Session layer for messagizer
   */
  EtchSessionMessage* getSession();

  /**
   * sets the session layer used in messagizer
   */
  void setSession(EtchSessionMessage* session);

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
   * @see EtchSession
   */
  status_t sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result);

  /**
   * @see EtchSession
   */
  status_t sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);

  /**
   * @see EtchSession
   */
  status_t sessionNotify(capu::SmartPointer<EtchObject> event);

  /**
   * @see EtchTransportMessage
   */
  status_t transportMessage(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchMessage> message);

  /**
   * @see EtchSessionPacket
   */
  status_t sessionPacket(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchFlexBuffer> buf);

private:
  EtchRuntime* mRuntime;
  static capu::Mutex mutex;
  EtchTransportPacket* mTransport;
  EtchTaggedDataInput* mTdi;
  EtchTaggedDataOutput* mTdo;
  EtchSessionMessage* mSession;

};

#endif

