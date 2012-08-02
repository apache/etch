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


#ifndef __ETCHSTUBHELPER_H__
#define __ETCHSTUBHELPER_H__

#include "support/EtchDeliveryService.h"

/**
* Generated code to dispatch method from message.
* @param <T> the service interface type.
*/

template <typename T>
class EtchStubHelper
{
 /**
  * Constructs a StubHelper which uses synchronous mode to dispatch
  * method from message.
  */
public:
  EtchStubHelper()
  {
    // nothing to do.
  }

 /**
  * Dispatches the method from message.
  * @param svc
  * @param obj
  * @param sender
  * @param msg
  */
  virtual status_t run(capu::SmartPointer<EtchDeliveryService> svc, capu::SmartPointer<T> obj, capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg);
};

#endif
