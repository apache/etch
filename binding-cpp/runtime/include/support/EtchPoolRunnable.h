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

#ifndef __ETCHPOOLRUNNABLE_H__
#define __ETCHPOOLRUNNABLE_H__

#include "capu/util/SmartPointer.h"
#include "common/EtchException.h"

/**
 *  A runnable which can throw an exception.
 */
class EtchPoolRunnable
{
public:

   /**
   * Execute the runnable
   * @return error core if some exeception occures
   */
  virtual status_t run() = 0;

  /**
   * Destructor
   */
  virtual ~EtchPoolRunnable() {}

  /**
   * Sets an exception
   * @param exception that occured
   * @return ETCH_OK if no error
   */
  status_t setException(capu::SmartPointer<EtchException> exception) {
    mException = exception;
    return ETCH_OK;
  }

  capu::bool_t hasException() {
    return (mException.get() != NULL);
  }

  /**
   * Gets the exception
   * @param exception that should be returned
   * @return ETCH_OK if no error, ETCH_EINVAL if the given params are invalid
   */
  status_t getException(capu::SmartPointer<EtchException> *exception) {
    if(exception == NULL) {
      return ETCH_EINVAL;
    }
    *exception = mException;
    return ETCH_OK;
  }

  /**
   * Reports an exception thrown by the run method.
   * @param e exception
   */
  virtual status_t exception(capu::SmartPointer<EtchException> exception) = 0;

private:
  capu::SmartPointer<EtchException> mException;
};

#endif
