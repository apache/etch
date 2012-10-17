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

#ifndef __ETCHTRANSPORT_H__
#define __ETCHTRANSPORT_H__

#include "common/EtchString.h"

/**
 * Out-of-band query, control, and notification interface for transports.
 * @param <S> the subclass of Session for this Transport.
 */
template <class S>
class EtchTransport {
public:

  /**
   * Destructor
   */
  virtual ~EtchTransport() {
  }

  /**
   * Returns the session for this transport.
   * @return the session for this transport.
   */
  virtual S* getSession() = 0;

  /**
   * Sets the session for this transport.
   * @param session session to receive data from the transport.
   */
  virtual void setSession(S* session) = 0;

  /**
   * Gets a configuration or operational value from the transport. The
   * request is passed down the chain of transports until some transport
   * recognizes the query, whereupon it returns the requested value.
   *
   * @param query an object representing a query, which could be as
   * simple as a string, integer, or enum, or more complex such as
   * a class with instance variables for query terms.
   * @param result contains the result of given query
   * @return ETCH_EUNIMPL if unimplemented
   *
   */
  virtual status_t transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result) = 0;

  /**
   * Sets a configuration or operational value in the transport. The
   * request is passed down the chain of transports until some transport
   * recognizes the control, whereupon it stores the specified value
   * and returns.
   *
   * @param control an object representing a control, which could be as
   * simple as a string, integer, or enum, or more complex such as
   * a class with instance variables for control terms.
   *
   * @param value the value to set.
   *
   */
  virtual status_t transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) = 0;

  /**
   * Notifies the chain of transports of the specified event. Unlike query
   * and control operations above, events are always passed down to the
   * bottom to allow all transports to notice them (though it is possible
   * that an event might be delayed for a bit).
   *
   * @param event a class which represents the event, possibly with
   * parameters. The simplest event could be a string, integer, or enum,
   * but any class instance will do (as long as some transport in the
   * chain expects it).
   *
   */
  virtual status_t transportNotify(capu::SmartPointer<EtchObject> event) = 0;

  /**
   * Name of value factory in resources.
   */
  const static EtchString& VALUE_FACTORY();

  ///////////////////////////////////////////////
  // Well-known queries, controls, and events. //
  ///////////////////////////////////////////////

  /**
   * Transport control which starts the transport stack. The argument
   * should be null.
   */
  const static EtchString& START();

  /**
   * Transport control which starts the transport stack and waits
   * for it to come up. The argument should be the integer number of
   * milliseconds to wait before giving up.
   */
  const static EtchString& START_AND_WAIT_UP();

  /**
   * Transport control which stops the transport stack. The argument
   * should be null, or a Boolean reset (true for instant close,
   * false for a nicer, gentler close).
   */
  const static EtchString& STOP();

  /**
   * Transport control which stops the transport stack and waits
   * for it to go down. The argument should be the integer number of
   * milliseconds to wait before giving up.
   */
  const static EtchString& STOP_AND_WAIT_DOWN();

  /**
   * Transport control which resets the transport stack (e.g., closes the
   * socket) without stopping it. If it is set to reconnect, then it will do
   * that. Only meaningful for connection oriented transports. Others types
   * will ignore this.
   */
  static const EtchString& RESET();

  /**
   * Transport query which asks is this a listener initiated connection or
   * is this a client initiated connection.
   */
  static const EtchString& IS_SERVER();
};

/**
 * Class for transport query which waits for the transport stack to come up.
 */
class WaitUp : public EtchObject {
public:
  /**
   * EtchObjectType for WaitUp
   */
  static const EtchObjectType* TYPE() {
    const static EtchObjectType TYPE(EOTID_WAIT_UP, NULL);
    return &TYPE;
  }

  /**
   * @param maxDelay max delay in milliseconds.
   */
  WaitUp(capu::int32_t maxDelay)
    : mMaxDelay(maxDelay) {
    addObjectType(TYPE());
  }

  /**
   * Copy Constructor
   */
  WaitUp(const WaitUp& other)
   : EtchObject(other), mMaxDelay(other.mMaxDelay) {
  }

  /**
   * max delay in milliseconds.
   */
  const capu::int32_t mMaxDelay;
};

/**
 * Class for transport query which waits for the transport stack to go down.
 */
class WaitDown : public EtchObject {
public:

  /**
   * EtchObjectType for WaitUp
   */
  static const EtchObjectType* TYPE() {
    const static EtchObjectType TYPE(EOTID_WAIT_DOWN, NULL);
    return &TYPE;
  }

  /**
   * Copy Constructor
   */
  WaitDown(const WaitDown& other)
   : EtchObject(other), mMaxDelay(other.mMaxDelay) {
  }

  /**
   * @param maxDelay max delay in milliseconds.
   */
  WaitDown(capu::int32_t maxDelay)
    : mMaxDelay(maxDelay) {
    addObjectType(TYPE());
  }

  /**
   * max delay in milliseconds.
   */
  const capu::int32_t mMaxDelay;
};

template <class S>
const EtchString& EtchTransport<S>::VALUE_FACTORY() {
  static const EtchString vf("Transport.valueFactory");
  return vf;
}

template <class S>
const EtchString& EtchTransport<S>::IS_SERVER() {
  static const EtchString isServer("IS_SERVER");
  return isServer;
}

template <class S>
const EtchString& EtchTransport<S>::RESET() {
  static const EtchString cmd("RESET");
  return cmd;
}

template <class S>
const EtchString& EtchTransport<S>::STOP_AND_WAIT_DOWN() {
  static const EtchString cmd("STOP_AND_WAIT_DOWN");
  return cmd;
}

template <class S>
const EtchString& EtchTransport<S>::STOP() {
  static const EtchString cmd("STOP");
  return cmd;
}

template <class S>
const EtchString& EtchTransport<S>::START() {
  static const EtchString cmd("START");
  return cmd;
}

template <class S>
const EtchString& EtchTransport<S>::START_AND_WAIT_UP() {
  static const EtchString cmd("START_AND_WAIT_UP");
  return cmd;
}

#endif
