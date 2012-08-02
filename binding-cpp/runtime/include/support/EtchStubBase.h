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

#ifndef __ETCHSTUBBASE_H__
#define __ETCHSTUBBASE_H__

#include "common/EtchTypes.h"
#include "support/EtchDeliveryService.h"
#include "support/EtchStubHelper.h"
#include "support/EtchFreePool.h"
#include "support/EtchQueuedPool.h"
#include "transport/EtchWho.h"
#include "transport/EtchMessage.h"

/**
 * Base class of stub implementations.
 * @param <T> The type of object used to implement stub.
 */
template<typename T>
class EtchStubBase : public EtchSessionMessage {
public:
  template <typename T>
  friend class EtchStubPoolRunnable;

  /**
   * Constructs the StubBase.
   * @param svc the message source.
   * @param obj the target of decoded messages.
   */
  EtchStubBase(EtchDeliveryService* svc, T* obj, EtchQueuedPool* queue,  EtchFreePool* pool);

  /**
   * Destructor
   */
  virtual ~EtchStubBase();

  /**
   *@see EtchSessionMessage
   */
  status_t sessionMessage(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg);

  /**
   *@see EtchSessionMessage
   */
  status_t sessionNotify(capu::SmartPointer<EtchObject> event);

  /**
   *@see EtchSessionMessage
   */
  status_t sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result);

  /**
   *@see EtchSessionMessage
   */
  status_t sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);

protected:

  /**
   * The message source.
   */
  EtchDeliveryService* mSvc;
  EtchFreePool* mFreePool;
  EtchQueuedPool* mQueuedPool;

  /**
   * The object used to implement stub.
   */
  T* mObj;
};

template<typename T>
class EtchStubPoolRunnable : public EtchPoolRunnable {
public:

  /**
   * Constructs the StubPoolRunnable.
   * @param stub the stub of this service
   * @param sender the sender of the message
   * @param msg the message.
   * @param helper the StubHelper which will dispatch the message.
   */
  EtchStubPoolRunnable(EtchStubBase<T>* stub, capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg, EtchStubHelper<T>* helper) 
    : mStub(stub)
    , mSender(sender)
    , mMsg(msg)
    , mHelper(helper) {
  }

  status_t run() {
    status_t status = mHelper->run(mStub, mStub->mSvc, mStub->mObj, mSender, mMsg);
    if(status != ETCH_OK) {
      // e.g exception
      // TODO: handel exception
    }
    return ETCH_OK;
  }

  status_t exception(capu::SmartPointer<EtchException> exception) {
    return ETCH_OK;
  }

private:
  EtchStubBase<T>* mStub;
  capu::SmartPointer<EtchWho> mSender;
  capu::SmartPointer<EtchMessage> mMsg;
  EtchStubHelper<T>* mHelper;
};

template<typename T>
EtchStubBase<T>::EtchStubBase(EtchDeliveryService* svc, T* obj, EtchQueuedPool* queued, EtchFreePool* free)
: mSvc(svc), mObj(obj), mQueuedPool(queued), mFreePool(free) {
  mSvc->setSession(this);
}

template<typename T>
EtchStubBase<T>::~EtchStubBase() {
  delete mObj;
  delete mSvc;
}

template<typename T>
status_t EtchStubBase<T>::sessionMessage(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchMessage> msg) {
  EtchType* type = msg->getType();

  EtchStubHelper<T>* helper = (EtchStubHelper<T>*) type->getStubHelper();
  if (helper == NULL) {
    return ETCH_ERROR;
  }

  capu::status_t res;
  EtchStubPoolRunnable<T>* runnable = NULL;
  switch (type->getAsyncMode()) {
    case QUEUED:
      runnable = new EtchStubPoolRunnable<T>(this, sender, msg, helper);
      res = mQueuedPool->add(runnable);
      if (res != ETCH_OK)
        sessionNotify(NULL); //TODO Exception Handling
      return res;
    case FREE:
      runnable = new EtchStubPoolRunnable<T>(this, sender, msg, helper);
      res = mFreePool->add(runnable);
      if (res != ETCH_OK)
        sessionNotify(NULL); //TODO Exception Handling
      return res;
    case NONE:
      res = helper->run(this, mSvc, mObj, sender, msg);
      if (res != ETCH_OK)
        sessionNotify(NULL); //TODO Exception Handling
      return res;
    default:
      //  throw new IllegalArgumentException("unknown async mode "+type.getAsyncMode());
      sessionNotify(NULL); //TODO Exception Handling
      return ETCH_OK;
  }
}

template<typename T>
status_t EtchStubBase<T>::sessionNotify(capu::SmartPointer<EtchObject> event) {
  //TODO: add implementation
  return ETCH_EUNIMPL;
}

template<typename T>
status_t EtchStubBase<T>::sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
  //TODO: add implementation, check if it is a control for ObjectSession
  return ETCH_EUNIMPL;
}

template<class T>
status_t EtchStubBase<T>::sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result) {
   //TODO: add implementation, check if it is a control for ObjectSession
  return ETCH_EUNIMPL;
}

#endif
