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
#include <pthread.h>
#include <unistd.h>
#endif

#ifdef THREAD_INC_MEMBER
private:
  class ThreadRunnable {
  public:
    Thread* thread;
    Runnable* runnable;
  };

  pthread_t mThread;
  pthread_attr_t mAttr;
  ThreadState mState;
  ThreadRunnable *mRunnable;

  /**
  * sets the current thread state
  */
  void setState(ThreadState state);

  static void * run(void* arg)
  {
    ThreadRunnable* tr = (ThreadRunnable*) arg;
    tr->thread->setState(TS_RUNNING);
    if (tr->runnable != NULL) {
      tr->runnable->run();
    }
    tr->thread->setState(TS_TERMINATED);
    pthread_exit(NULL);
  }


#endif

#ifdef THREAD_INC_IMPL

inline Thread::Thread(Runnable *runnable) : mState(TS_NEW)
{
  pthread_attr_init(&mAttr);
  pthread_attr_setdetachstate(&mAttr,PTHREAD_CREATE_JOINABLE);
  mRunnable = new ThreadRunnable();
  mRunnable->thread = this;
  mRunnable->runnable = runnable;
  
}

inline Thread::~Thread()
{
  join();
  pthread_attr_destroy(&mAttr);
  delete mRunnable;
}

inline status_t Thread::start()
{
  if (mRunnable == NULL) {
    return CAPU_EINVAL;
  }

  int32_t result = pthread_create(&mThread, &mAttr, Thread::run, mRunnable);
  if (result != 0) {
    return CAPU_ERROR;
  }   
  return CAPU_OK;
}


inline status_t Thread::join ()
{
  if (mThread == 0) {
    return CAPU_ERROR;
  }
  if(pthread_join(mThread, NULL) == 0) {
    mThread = 0;
    return CAPU_OK;
  }
  return CAPU_ERROR;
}

inline status_t Thread::Sleep(uint32_t millis)
{
  if(usleep(millis*1000)==0)
    return CAPU_OK;
  return CAPU_ERROR;
}

inline ThreadState Thread::getState() {
  return mState;
}

inline void Thread::setState(ThreadState state) {
  mState = state;
}

#endif
