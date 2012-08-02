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


#ifdef CONDVAR_INC_HEADER
#include <pthread.h>
#include <time.h>
#include <errno.h>
#endif

#ifdef CONDVAR_INC_MEMBER
private:
  pthread_cond_t mCond;
  pthread_condattr_t mCondAttr;
#endif

#ifdef CONDVAR_INC_IMPL
  inline CondVar::CondVar()
  {
    pthread_condattr_init(&mCondAttr);
    pthread_cond_init(&mCond,&mCondAttr);
  }
  inline CondVar::~CondVar()
  {
    pthread_cond_destroy(&mCond);
    pthread_condattr_destroy(&mCondAttr);
  }
  inline status_t CondVar::signal()
  {
    if(pthread_cond_signal(&mCond) == 0)
    {
      return CAPU_OK;
    }
    else
    {
      return CAPU_ERROR;
    }
  }
  inline status_t CondVar::broadcast()
  {
    if(pthread_cond_broadcast(&mCond) == 0)
    {
      return CAPU_OK;
    }
    else
    {
      return CAPU_ERROR;
    }
  }
  inline status_t CondVar::wait(Mutex *mutex, uint32_t millisec)
  {
    if(mutex == NULL)
    {
      return CAPU_EINVAL;
    }

    if(millisec != 0)
    {
      timespec _time;
      if(clock_gettime(CLOCK_REALTIME, &_time) < 0)
      {
        return CAPU_ERROR;
      }
      _time.tv_sec += (millisec/1000);
      _time.tv_nsec += ((millisec%1000)* 1000000);
      int32_t ret = pthread_cond_timedwait(&mCond, &mutex->mLock, &_time);

      switch(ret)
      {
        case 0:
          return CAPU_OK;
        case ETIMEDOUT:
          return CAPU_ETIMEOUT;
        default:
          return CAPU_ERROR;
      }
    }
    else
    {
      if(pthread_cond_wait(&mCond, &mutex->mLock) == 0)
      {
        return CAPU_OK;
      }
      else
      {
        return CAPU_ERROR;
      }
    }
  }
#endif
