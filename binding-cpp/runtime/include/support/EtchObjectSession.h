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

#ifndef __ETCHOBJECTSESSION_H__
#define __ETCHOBJECTSESSION_H__

#include "common/EtchObject.h"

/**
* Out-of-band query, control, and notification interface for sessions.
* This is exactly the same as EtchSession except the names are changed
* to prevent collisions with service defined names.
* @see EtchSession
*/
class EtchObjectSession
{
public:

  virtual ~EtchObjectSession() {}

  /**
  * Gets a configuration or operational value from the handler. The
  * request is passed up the chain of sessions until some session
  * recognizes the query, whereupon it returns the requested value.
  *
  * @param query an object representing a query, which could be as
  * simple as a string, integer, or enum, or more complex such as
  * a class with instance variables for query terms.
  *
  * @param result the requested value, or null if not defined.
  */
  virtual status_t _sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result ) = 0;

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
  *
  *
  */
  virtual status_t _sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value ) = 0;

  /**
  * Notifies the chain of sessions of the specified event. Unlike query
  * and control operations above, events are always passed up to the
  * top to allow all sessions to notice them.
  *
  * @param event a class which represents the event, possibly with
  * parameters. The simplest event could be a string, integer,
  * or enum, but any class instance will do (as long as some session
  * in the chain expects it).
  *
  */
  virtual status_t _sessionNotify(capu::SmartPointer<EtchObject> event ) = 0;
};

#endif /*__ETCHOBJECTSESSION_H__*/
