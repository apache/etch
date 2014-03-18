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

#ifndef __ETCHDEFAULTDELIVERYSERVICE_H__
#define __ETCHDEFAULTDELIVERYSERVICE_H__

#include "common/EtchBool.h"
#include "common/EtchString.h"
#include "common/EtchInt32.h"
#include "common/EtchRuntimeException.h"
#include "support/EtchDeliveryService.h"
#include "support/EtchMonitor.h"
#include "transport/EtchMailboxManager.h"
#include "util/EtchURL.h"
#include "util/EtchResources.h"

class EtchRuntime;

/**
 * Default implementation of DeliveryService.
 */
class EtchDefaultDeliveryService
: public EtchDeliveryService {

public:
  /**
   * Parameter to Globally Disable Timeout.
   */
  static const EtchString& DISABLE_TIMEOUT();

  /**
   * Constructor
   * @param transport the MailBoxManager implementation
   * @param uri as an EtchString object
   */
  EtchDefaultDeliveryService(EtchRuntime* runtime, EtchMailboxManager* transport, const EtchString& uri);

  /**
   * Constructor
   * @param transport the MailBoxManager implementation
   * @param uri as an EtchURL object
   */
  EtchDefaultDeliveryService(EtchRuntime* runtime, EtchMailboxManager* transport, EtchURL* uri);

  /**
   * Destructor
   */
  virtual ~EtchDefaultDeliveryService();

  /**
   * @return the transport interface
   */
  const EtchMailboxManager* getTransport();

  /**
   * @see EtchSessionMessage
   */
  EtchSessionMessage* getSession();

  /**
   * @see EtchSessionMessage
   */
  void setSession(EtchSessionMessage* session);

  /**
   * @see EtchSessionMessage
   */
  virtual status_t sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result);

  /**
   * @see EtchSessionMessage
   */
  virtual status_t sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);

  /**
   * @see EtchSessionMessage
   */
  virtual status_t sessionNotify(capu::SmartPointer<EtchObject> event);

  /**
   * @see EtchSessionMessage
   */
  status_t sessionMessage(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg);

  /**
   * @see EtchTransportMessage
   */
  virtual status_t transportMessage(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchMessage> message);

  /**
   * @see EtchTransportMessage
   */
  virtual status_t transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result);

  /**
   * @see EtchTransportMessage
   */
  virtual status_t transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);

  /**
   * @see EtchTransportMessage
   */
  virtual status_t transportNotify(capu::SmartPointer<EtchObject> event);

  /**
   * @see EtchDeliveryService
   */
  virtual status_t begincall(capu::SmartPointer<EtchMessage> msg, capu::SmartPointer<EtchMailbox>& result);

  /**
   * @see EtchDeliveryService
   */
  virtual status_t endcall(EtchMailbox* mb, EtchType* responseType, capu::SmartPointer<EtchObject>& result);

private:
  EtchRuntime* mRuntime;
  status_t waitUp(capu::int32_t maxDelay);
  status_t waitDown(capu::int32_t maxDelay);
  void init(EtchURL* url);

  EtchMailboxManager* mTransport;
  EtchSessionMessage* mSession;
  EtchMonitor mStatus;
  capu::bool_t mDisableTimeout;
};


#endif /* __ETCHDEFAULTDELIVERYSERVICE_H__ */
