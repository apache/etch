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

#ifndef __THREAD_H__
#define __THREAD_H__

#define THREAD_INC_HEADER
#include "arch/Thread.inc"
#undef THREAD_INC_HEADER

#include "capu/Error.h"

namespace capu
{

  class Thread
  {

#define THREAD_INC_MEMBER
#include "arch/Thread.inc"
#undef THREAD_INC_MEMBER

  public:
    /**
    * Constructor
    * Creates a thread which will execute the given function
    * @param func function object that will be executed
    * @param arguments
    */
    template <typename T>
    inline Thread(T* object,void* args=NULL);

    /**
    * Destructor
    */
    inline ~Thread();

    /**
    * Waits the thread completeness
    * @return CAPU_OK if thread is currently waiting for completeness
    *         CAPU_ERROR otherwise
    */
    inline status_t join();

    /**
    * Suspend the current thread for specific amount of time
    * @return CAPU_OK if thread is currently suspended
    *         CAPU_ERROR otherwise
    */
    static inline status_t Sleep(uint32_t millis);

  };

#define THREAD_INC_IMPL
    #include "arch/Thread.inc"
#undef THREAD_INC_IMPL

}

#endif /* Thread_H */
