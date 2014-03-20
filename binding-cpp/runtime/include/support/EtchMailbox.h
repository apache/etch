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

#ifndef __ETCHMAILBOX_H__
#define __ETCHMAILBOX_H__

#include "common/EtchBool.h"
#include "common/EtchLong.h"
#include "transport/EtchWho.h"
#include "transport/EtchMessage.h"

/**
 * Adapter between remote and message source.
 */
class EtchMailbox {
public:
  class EtchElement;
  class EtchNotify;

  /**
   * Destructure
   */
  virtual ~EtchMailbox() {
  }

  /**
   * @return the message id of this mailbox.
   */
  virtual EtchLong getMessageId() = 0;

  /**
   * Queues the message to the mailbox.
   * @param sender
   * @param msg
   */

  virtual status_t message(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg) = 0;
  /**
   * @param the next message to be read from the mailbox, or null if
   * the mailbox is empty and closed. Wait forever for such a message
   * to be delivered.
   */
  virtual status_t read(EtchElement *&result) = 0;

  /**
   * @param maxDelay the maximum amount of time in milliseconds to
   * wait to read a message from an empty mailbox. 0 means wait
   * forever, -1 means don't wait at all.
   * @param the message read from the mailbox, or null if the mailbox
   * is empty and closed, or if the time limit was exceeeded.
   */
  virtual status_t read(EtchElement *&result, capu::int32_t maxDelay) = 0;

  /**
   * Closes the mailbox so that no more messages can be delivered.
   * Queued messages remain to be read.
   */
  virtual status_t closeDelivery(capu::bool_t withNotification = true) = 0;

  /**
   * Closes the mailbox so that no more messages will be delivered or
   * read. Any remaining queued messages are delivered to a default
   * handler.
   */
  virtual status_t closeRead() = 0;
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
   * @throws IllegalStateException notify already registered.
   */
  virtual status_t registerNotify(EtchNotify* notify, EtchObject* state, capu::int32_t maxDelay) = 0;

  /**
   * Unregisters a Notify interface implementation from receiving a callback
   * when a mailbox's status is changed. Cancels any timeout.
   * @param notify a Notify interface implementation which was previously
   * registered.
   */
  virtual status_t unregisterNotify(EtchNotify* notify) = 0;

  /**
   * @return true if the mailbox is empty.
   */
  virtual capu::bool_t isEmpty() = 0;

  /**
   * @return true if the mailbox is closed.
   */
  virtual capu::bool_t isClosed() = 0;

  /**
   * @return true if the mailbox is full.
   */
  virtual capu::bool_t isFull() = 0;

  /**
   * The message as queued, including src and sender.
   */
  class EtchElement {
  public:

    /**
     * @param sender the message sender.
     * @param msg the message.
     */
    EtchElement(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg)
    : mSender(sender), mMsg(msg) {
    }

    /**
     * Destructor
     */
    virtual ~EtchElement() {
    }

    /**
     * The message sender.
     */
    capu::SmartPointer<EtchWho> mSender;
    /**
     * the message.
     */
    capu::SmartPointer<EtchMessage> mMsg;
  };

  /**
   * Interface used to notify of mailbox status.
   */
  class EtchNotify {
  public:
    /**
     * Destructor
     */
    virtual ~EtchNotify() {
    }

    /**
     * Notifies of mailbox status change.
     * @param mb the mailbox whose status has changed.
     * @param state the state object passed in the register
     * method.
     * @param closed true if the mailbox timeout has expired and
     * the mailbox is now closed to delivery, false if a message
     * has arrived.
     */
    virtual status_t mailboxStatus(EtchMailbox* mb, EtchObject* state, capu::bool_t closed) = 0;
  };
};

#endif
