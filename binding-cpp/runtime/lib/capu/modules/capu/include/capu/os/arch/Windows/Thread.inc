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
#ifdef THREAD_INC_HEADER
#include <windows.h>
#endif

#ifdef THREAD_INC_MEMBER
private:
  class ThreadRunnable {
  public:
    Thread* thread;
    Runnable* runnable;
  };

  DWORD  mThreadId;
  HANDLE mThreadHandle;
  ThreadState mState;
  ThreadRunnable *mRunnable;

  /**
  * sets the current thread state
  */
  void setState(ThreadState state);

  static DWORD WINAPI run(LPVOID arg)
  {
    ThreadRunnable* tr = (ThreadRunnable*) arg;
    tr->thread->setState(TS_RUNNING);
    if (tr->runnable != NULL) {
      tr->runnable->run();
    }
    tr->thread->setState(TS_TERMINATED);
    return NULL;
  }


#endif

#ifdef THREAD_INC_IMPL



inline Thread::Thread(Runnable *runnable) : mState(TS_NEW) {
  mRunnable = new ThreadRunnable();
  mRunnable->thread = this;
  mRunnable->runnable = runnable;
}

inline Thread::~Thread()
{
  join();
  delete mRunnable;
}

inline status_t Thread::start()
{
  if (mRunnable == NULL) {
    return CAPU_EINVAL;
  }

  mThreadHandle = CreateThread(NULL, 0, Thread::run, mRunnable, 0, &mThreadId);
  //TODO: check thread handle and return appropriate error code
  if (mThreadHandle == NULL) {
    return CAPU_ERROR;
  }
  
  return CAPU_OK;
}

inline status_t Thread::join()
{
  if (mThreadHandle && WaitForSingleObject(mThreadHandle,INFINITE) == 0) {
    return CAPU_OK;
  } else {
    return CAPU_ERROR;
  }
}

inline ThreadState Thread::getState() {
  return mState;
}

inline void Thread::setState(ThreadState state) {
  mState = state;
}

inline status_t Thread::Sleep(uint32_t millis)
{
  ::Sleep(millis);
  return CAPU_OK;
}



#endif
