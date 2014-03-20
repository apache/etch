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

#ifndef __ETCHPLAINMAILBOX_H__
#define __ETCHPLAINMAILBOX_H__

#include "capu/os/Mutex.h"
#include "support/EtchMailbox.h"
#include "util/EtchCircularQueue.h"
#include "transport/EtchMailboxManager.h"

class EtchPlainMailbox : public EtchMailbox{
public:

  /**
   * Default Constructor
   * @param mailboxManager
   * @param messageId
   */

  EtchPlainMailbox(EtchMailboxManager* mailboxManager, EtchLong messageId);

  /**
   * Destructor
   */
  virtual ~EtchPlainMailbox();

  /**
   * @return the message id of this mailbox.
   */
  EtchLong getMessageId();

  /**
   * @See EtchMailbox
   */
  status_t message(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg);

  /**
   * @param the next message to be read from the mailbox, or null if
   * the mailbox is empty and closed. Wait forever for such a message
   * to be delivered.
   */
  status_t read(EtchMailbox::EtchElement*& result);

  /**
   * @param maxDelay the maximum amount of time in milliseconds to
   * wait to read a message from an empty mailbox. 0 means wait
   * forever, -1 means don't wait at all.
   * @param the message read from the mailbox, or null if the mailbox
   * is empty and closed, or if the time limit was exceeeded.
   */
  status_t read(EtchMailbox::EtchElement *& result, capu::int32_t maxDelay);

  /**
   * Closes the mailbox so that no more messages can be delivered.
   * Queued messages remain to be read. Reading an empty closed
   * mailbox returns null.
   * @param true if this call closed the mailbox (that is, if action was
   * taken), false if the mailbox was already closed.
   */
  status_t closeDelivery(capu::bool_t withNotification = true);

  /**
   * Closes the mailbox so that no more messages will be delivered or
   * read. Any remaining queued messages are delivered to a default
   * handler.
   * @return ETCH_OK if this call closed the mailbox (that is, if action was
   * taken), ETCH_ERROR if the mailbox was already closed.
   */
  status_t closeRead();

  /**
   * Registers a Notify interface implementation to receive a callback
   * when a mailbox's status is changed.
   * @param notify a Notify interface implementation to report the
   * delivery status to.
   * @param state a state value to pass thru to the Notify interface
   * implementation.
   * @param maxDelay the maximum amount of time in milliseconds to
   * wait for delivery of a message to the mailbox. 0 means wait
   * forever. The mailbox is closed upon timeout.
   * @return ETCH_OK registration was successfully, ETCH_ERROR otherwise
   */
  status_t registerNotify(EtchMailbox::EtchNotify* notify, EtchObject* state, capu::int32_t maxDelay);

  /**
   * Unregisters a Notify interface implementation from receiving a callback
   * when a mailbox's status is changed. Cancels any timeout.
   * @param notify a Notify interface implementation which was previously
   * registered.
   * @return ETCH_OK unregistration was successfully, ETCH_ERROR otherwise
   */
  status_t unregisterNotify(EtchMailbox::EtchNotify* notify);

  /**
   * @return true if the mailbox is empty.
   */
  capu::bool_t isEmpty();

  /**
   * @return true if the mailbox is closed.
   */
  capu::bool_t isClosed();

  /**
   * @return true if the mailbox is full.
   */
  capu::bool_t isFull();

  /**
   *
   * @return mailbox manager
   */
  EtchMailboxManager* getMailboxManager();

private:
  EtchMailboxManager* mMailboxManager;
  EtchMailbox::EtchNotify* mNotify;
  EtchObject* mState;
  EtchLong mMessageId;
  EtchCircularQueue mQueue;
  capu::Mutex mQueueMutex;

  void fireNotify();
};

#endif /* ETCHPLAINMAILBOX_H */
