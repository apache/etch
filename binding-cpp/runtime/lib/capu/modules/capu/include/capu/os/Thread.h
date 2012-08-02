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
#include "capu/util/Runnable.h"

namespace capu
{

  /**
   * Thread states
   */
  enum ThreadState {
    TS_NEW,
    TS_RUNNING,
    TS_TERMINATED
  };


  class Thread
  {

#define THREAD_INC_MEMBER
#include "arch/Thread.inc"
#undef THREAD_INC_MEMBER

  public:

    /**
    * Default constructor
    */
    inline Thread() {}

    /**
    * Constructor
    * Creates a thread which will execute the given function
    * @param Runnable object that will be executed
    * @param arguments
    */
    inline Thread(Runnable* runnable);

    /**
    * Destructor
    */
    inline ~Thread();

    /**
    * Starts the thread
    * @param Runnable object that will be executed
    * @return CAPU_OK if thread has been started successfully
    *         CAPU_ERROR otherwise
    */
    inline status_t start();

    /**
    * Waits the thread completeness
    * @return CAPU_OK if thread is currently waiting for completeness
    *         CAPU_ERROR otherwise
    */
    inline status_t join();

    /**
    * Return the current thread state
    * @return state of the thread
    */
    inline ThreadState getState();

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
