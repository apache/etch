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
#include <windows.h>
#endif

#ifdef CONDVAR_INC_MEMBER
private:
  HANDLE mSemaphore;
  Mutex mLockWaitCounter;
  uint32_t mWaitCounter;
  uint32_t mGeneration;
#endif

#ifdef CONDVAR_INC_IMPL

  inline CondVar::CondVar()
  {
    mSemaphore = CreateSemaphore(NULL,0,INT_MAX,NULL);
    mWaitCounter = 0;
    mGeneration = 0;
  }

  inline CondVar::~CondVar() {
    CloseHandle(mSemaphore);
  }

  inline status_t CondVar::signal()
  {
    bool releaseSemaphore = false;
    mLockWaitCounter.lock();
    if (mWaitCounter > 0) {
      releaseSemaphore = true;
      ++mGeneration;
    }
    mLockWaitCounter.unlock();
    if (releaseSemaphore){
      ReleaseSemaphore(mSemaphore, 1, NULL);
    }
    return CAPU_OK;
  }

  inline status_t CondVar::broadcast()
  {
    bool releaseSemaphore = false;
     if (mWaitCounter > 0) {
      releaseSemaphore = true;
      ++mGeneration;
    }
    mLockWaitCounter.unlock();
    if (releaseSemaphore){
      ReleaseSemaphore(mSemaphore, mWaitCounter, NULL);
    }
    return CAPU_OK;
  }

  inline status_t CondVar::wait(Mutex *mutex, uint32_t millisec)
  {
    uint32_t myGeneration = mGeneration;
    if (mutex == NULL) {
      return CAPU_EINVAL;
    } else {
      if (mLockWaitCounter.lock() == CAPU_OK) {
        ++mWaitCounter;
        mLockWaitCounter.unlock();
        mutex->unlock();
        DWORD waitResult;
        while (true) {
            if(millisec == 0) {
              // regular wait
              waitResult = WaitForSingleObject(mSemaphore, INFINITE);
            } else {
              // timed wait
              waitResult = WaitForSingleObject(mSemaphore, millisec);
            }

            if (waitResult == WAIT_OBJECT_0) {
              if (myGeneration != mGeneration) {
                mLockWaitCounter.lock();
                --mWaitCounter;
                mLockWaitCounter.unlock();

                mutex->lock();
                return CAPU_OK;
              } else {
                ReleaseSemaphore(mSemaphore, 1, NULL);
              }
            } else
            if (waitResult == WAIT_TIMEOUT) {
              mLockWaitCounter.lock();
              --mWaitCounter;
              mLockWaitCounter.unlock();

              mutex->lock();
              return CAPU_ETIMEOUT;
            } else {
              mutex->lock();
              return CAPU_ERROR;
            }
        }
      } else {
        return CAPU_ERROR;
      }
    }
  }
#endif
