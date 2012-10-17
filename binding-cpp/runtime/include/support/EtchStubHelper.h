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

#include "capu/util/SmartPointer.h"
#include "support/EtchDeliveryService.h"
#include "support/EtchRuntime.h"

template<typename T>
class EtchStubBase;

/**
 * Generated code to dispatch method from message.
 * @param <T> the service interface type.
 */
template <typename T>
class EtchStubHelper
  : public EtchObject {
public:

  /**
   * EtchObjectType for EtchStubHelper.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructs a StubHelper which uses synchronous mode to dispatch
   * method from message.
   */
  EtchStubHelper(EtchRuntime* runtime)
  : mRuntime(runtime) {
    addObjectType(TYPE());
  }

  EtchStubHelper(const EtchStubHelper& other)
  : EtchObject(other), mRuntime(other.mRuntime) {
  }

  /**
   * Dispatches the method from message.
   * @param svc
   * @param obj
   * @param sender
   * @param msg
   */
  virtual status_t run(EtchStubBase<T>* ref, EtchDeliveryService* svc, T* obj, capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg ) = 0;


protected:
  EtchRuntime* mRuntime;

};

template<typename T>
const EtchObjectType* EtchStubHelper<T>::TYPE() {
  const static EtchObjectType TYPE(EOTID_STUBHELPER, NULL);
  return &TYPE;
}

#endif
