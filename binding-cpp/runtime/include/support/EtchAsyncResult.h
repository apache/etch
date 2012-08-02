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

#ifndef __ETCHASYNCRESULT_H__
#define __ETCHASYNCRESULT_H__

#include "support/EtchAsyncResultNone.h"

/**
 * Base class of AsyncResult implementations.
 * @param <T> The type of object used to implement AsyncResult.
 */
template<typename T>
class EtchAsyncResult : public EtchAsyncResultNone {
public:

  /**
   * Constructor
   * @param runtime
   * @param mailbox
   */
  EtchAsyncResult(EtchRuntime* runtime = NULL, EtchMailbox* mailbox = NULL)
   : EtchAsyncResultNone(runtime, mailbox), mResult(NULL), mHasResult(false) {
  }

  /**
   * Destructor
   */
  virtual ~EtchAsyncResult() {
  }

  /**
   * Check if a result is available
   */
  capu::bool_t hasResult() {
    mMutex.lock();
    while(!mHasMailboxStatus) {
      // TODO wait
      break;
      mCond.wait(&mMutex);
    }
    mMutex.unlock();
    return mHasResult;
  }

  /**
   * Sets the result
   */
  status_t setResult(capu::SmartPointer<T> result) {
    mResult = result;
    mHasResult = true;
    
    mMutex.lock();
    mHasMailboxStatus = true;
    mCond.signal();
    mMutex.unlock();
    return ETCH_OK;
  }

  /**
   * Returns the result
   */
  status_t getResult(capu::SmartPointer<T> &result) {
    if(hasException()) {
      return ETCH_ERROR;
    }
    if(hasResult()) {
      result = mResult;
      return ETCH_OK;
    }
    return ETCH_OK;
  }

private:
   capu::bool_t mHasResult;
   capu::SmartPointer<T> mResult;
};

#endif
