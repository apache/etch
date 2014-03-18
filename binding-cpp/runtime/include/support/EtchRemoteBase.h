
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

#ifndef __ETCHREMOTBASE_H__
#define __ETCHREMOTBASE_H__

#include "common/EtchTypes.h"
#include "support/EtchDeliveryService.h"
#include "support/EtchStack.h"
#include "serialization/EtchValueFactory.h"

class EtchRuntime;

/**
 * Base class for call to message translators.
 */
class EtchRemoteBase {
public:
  /**
   * Constructs the RemoteBase.
   *
   * @param svc the delivery service used to send messages.
   * @param vf the value factory used to create messages and encode/decode types.
   */
  EtchRemoteBase(EtchRuntime* runtime, EtchDeliveryService* svc, EtchValueFactory* vf, EtchStack* stack);

  /**
   * Destructor
   */
  virtual ~EtchRemoteBase();

  /**
   * Constructs a new message to send using {@link #send(Message)} or
   * {@link #begincall(Message)};
   * @param type the type of the message.
   * @param message a new message
   * @return error if something goes wrong
   */
  capu::status_t newMessage(EtchType* type, capu::SmartPointer<EtchMessage> *message);

  /**
   * Sends the message to the recipient, but does not wait for any response.
   *
   * @param msg the message to send
   * @return error if something goes wrong
   */
  status_t send(capu::SmartPointer<EtchMessage> msg);

  /**
   * Sends the message which begins a call sequence.
   *
   * @param msg the message to send.
   * @return a mailbox which can be used to read the response, using
   * {@link #endcall(Mailbox, Type)}.
   */
  status_t begincall(capu::SmartPointer<EtchMessage> msg, capu::SmartPointer<EtchMailbox> &result);

  /**
   * Finishes a call sequence by waiting for the response message.
   *
   * @param mb a mailbox which will be used to read the response, returned by
   * {@link #begincall(Message)}.
   * @param responseType the type of the expected response.
   * @param result the value of the response field if it isn't an exception.
   * @return an error if there is a problem sending or a timeout waiting or
   *         if the result value was an exception.
   */
  status_t endcall(EtchMailbox* mb, EtchType* responseType, capu::SmartPointer<EtchObject> &result);

  /**
   * Gets a configuration or operational value from the source. The
   * request is passed down the chain of sources until some source
   * recognizes the query, whereupon it returns the requested value.
   *
   * @param query an object representing a query, which could be as
   * simple as a string, integer, or enum, or more complex such as
   * a class with instance variables for query terms.
   *
   * @param result the requested value, or null if not defined.
   *
   * @return an error if there is a problem
   */
  status_t transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result);

  /**
   * Sets a configuration or operational value in the source. The
   * request is passed down the chain of sources until some source
   * recognizes the control, whereupon it stores the specified value
   * and returns.
   *
   * @param control an object representing a control, which could be as
   * simple as a string, integer, or enum, or more complex such as
   * a class with instance variables for control terms.
   *
   * @param value the value to set.
   *
   * @return an error if there is a problem
   */
  status_t transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);

  /**
   * Notifies the chain of sources of the specified event. Unlike query
   * and control operations above, events are always passed down to the
   * bottom to allow all sources to notice them.
   *
   * @param event a class which represents the event, possibly with
   * parameters. The simplest event could be a string, integer,
   * or enum, but any class instance will do (as long as some source
   * in the chain expects it).
   *
   * @return an error if there is a problem
   */
  status_t transportNotify(capu::SmartPointer<EtchObject> event);

  ////////////////////////////
  // Convenience operations //
  ////////////////////////////

  /**
   * Starts the transport.
   * @see transportControl
   * @return an error if there is a problem
   */
  status_t start();

  /**
   * Waits for the transport to come up.
   * @param maxDelay max delay in milliseconds.
   * @return an error if there is a problem
   * @see transportQuery
   * @return an error if there is a problem
   */
  status_t waitUp(capu::int32_t maxDelay);

  /**
   * Starts the transport and waits for it to come up.
   * @param maxDelay max delay in milliseconds.
   * @return an error if there is a problem
   * @see start
   * @return an error if there is a problem
   */
  status_t startAndWaitUp(capu::int32_t maxDelay);

  /**
   * Stops the transport.
   * @return an error if there is a problem
   * @see transportControl
   * @return an error if there is a problem
   */
  status_t stop();

  /**
   * Waits for the transport to go down.
   * @param maxDelay max delay in milliseconds.
   * @return an error if there is a problem
   * @see transportQuery
   * @see WaitDown
   * @return an error if there is a problem
   */
  status_t waitDown(capu::int32_t maxDelay);

  /**
   * Stops the transport and waits for it to go down.
   * @param maxDelay max delay in milliseconds.
   * @return an error if there is a problem
   * @see start
   * @see waitDown
   * @return an error if there is a problem
   */
  status_t stopAndWaitDown(capu::int32_t maxDelay);

private:
  EtchRuntime* mRuntime;
  EtchDeliveryService * mSvc;
  EtchValueFactory* mVf;
  EtchStack* mStack;
};

#endif /* __ETCHREMOTBASE_H__ */
