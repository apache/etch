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

#ifndef __ETCHMAILBOXMANAGER_H__
#define __ETCHMAILBOXMANAGER_H__

#include "transport/EtchSessionMessage.h"
#include "transport/EtchTransportMessage.h"
#include "support/EtchMailbox.h"

/**
 * Interface to the mailbox manager as needed by {@link DeliveryService} and
 * {@link Mailbox} implementors.
 */
class EtchMailboxManager : public EtchSessionMessage, public EtchTransportMessage {
public:
  /**
   * Sends a message which begins a call after allocating a Mailbox to
   * receive any responses.
   * @param recipient specifies the recipient when there is the possibility
   * of more than one.
   * @param msg the message which begins the call.
   * @param result the mailbox which will receive responses to this call.
   * @return status code
   */
  virtual status_t transportCall(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchMessage> msg, capu::SmartPointer<EtchMailbox>& result) = 0;

  /**
   * Removes the mailbox from the set of mailboxes receiving responses to
   * messages.
   * @param mb a mailbox as returned by {@link #transportCall(Who, Message)}.
   * @return status code
   */
  virtual status_t unregisterMailbox(EtchLong mailboxId) = 0;

  /**
   * Re-delivers dead letter messages from a closed mailbox.
   * @param sender
   * @param msg
   * @return status code
   */
  virtual status_t redeliver(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg) = 0;
};

#endif /* __ETCHMAILBOXMANAGER_H__ */
