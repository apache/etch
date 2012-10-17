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

#ifndef __ETCHSESSION_H__
#define __ETCHSESSION_H__

#include "common/EtchString.h"

class EtchSession {
public:

  virtual ~EtchSession() {
  }

  /**
   * Gets a configuration or operational value from the handler. The
   * request is passed up the chain of sessions until some session
   * recognizes the query, whereupon it returns the requested value.
   *
   * @param query an object representing a query, which could be as
   * simple as a string, integer, or enum, or more complex such as
   * a class with instance variables for query terms.
   *
   * @param result contains the result of given query
   * @return ETCH_OK if requested operational value is successfully get
   *         ETCH_ERROR otherwise
   */
  virtual status_t sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result) = 0;

  /**
   * Sets a configuration or operational value in the session. The
   * request is passed up the chain of sessions until some session
   * recognizes the control, whereupon it stores the specified value
   * and returns.
   *
   * @param control an object representing a control, which could be as
   * simple as a string, integer, or enum, or more complex such as
   * a class with instance variables for control terms.
   *
   * @param value the value to set.
   * @return ETCH_OK if requested operational value is successfully set
   *         ETCH_ERROR otherwise
   */
  virtual status_t sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) = 0;

  /**
   * Notifies the chain of sessions of the specified event. Unlike query
   * and control operations above, events are always passed up to the
   * top to allow all sessions to notice them.
   *
   * @param event a class which represents the event, possibly with
   * parameters. The simplest event could be a string, integer,
   * or enum, but any class instance will do (as long as some session
   * in the chain expects it).
   * @return ETCH_OK if event is notified successfully
   *         ETCH_ERROR otherwise
   *
   */
  virtual status_t sessionNotify(capu::SmartPointer<EtchObject> event) = 0;

  ///////////////////////
  // Well-known events //
  ///////////////////////

  /**
   * Session event reporting the transport is up.
   */
  const static EtchString& UP();

  /**
   * Session event reporting the transport is down.
   */
  const static EtchString& DOWN();

};

#endif /* ETCHSESSION_H */

