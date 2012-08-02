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

#ifndef __ETCHASYNCRESULTBASE_H__
#define __ETCHASYNCRESULTBASE_H__

#include "common/EtchException.h"

/**
* Base class of AsyncResult implementations.
* @param <T> The type of object used to implement AsyncResult.
*/

template<typename T>
class EtchAsyncResultBase {
protected:
  capu::SmartPointer<EtchException> mException;
  capu::SmartPointer<T> mResult;
  capu::bool_t mHasException, mHasResult;

  EtchAsyncResultBase() : mException(NULL), mResult(NULL), mHasException(false), mHasResult(false) { }

  virtual ~EtchAsyncResultBase() { }

public:
  EtchException* getException()
  {
    return mException;
  }
  void setException(EtchException* exception)
  {
    mHasException = true;
    mException = exception;
  }
  virtual capu::bool_t hasException() = 0;

  capu::SmartPointer<T> getResult()
  {
    return mResult;
  }
  void setResult(capu::SmartPointer<T> value)
  {
    mHasResult = true;
    mResult = value;
  }
  capu::bool_t hasResult()
  {
    return mHasResult;
  }
};

#endif
