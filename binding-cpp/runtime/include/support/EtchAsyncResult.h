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
  EtchAsyncResult(EtchRuntime* runtime = NULL, capu::SmartPointer<EtchMailbox> mailbox = NULL)
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

    setMailboxStatus();
    return ETCH_OK;
  }

  /**
   * Returns the result
   */
  capu::SmartPointer<T> getResult() {
    if(hasException()) {
      return NULL;
    } else
    if(hasResult()) {
      return mResult;
    }
    return NULL;
  }

private:
  capu::SmartPointer<T> mResult;
  capu::bool_t mHasResult;
};

#endif
