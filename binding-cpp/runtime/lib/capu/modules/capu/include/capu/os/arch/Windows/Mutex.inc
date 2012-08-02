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

#ifdef MUTEX_INC_HEADER
#include <windows.h>
#endif

#ifdef MUTEX_INC_MEMBER
public:
  friend class CondVar;

private:
  HANDLE mLock;
#endif

#ifdef MUTEX_INC_IMPL
  inline Mutex::Mutex()
  {
    mLock = CreateMutex( 
      NULL,              // default security attributes
      FALSE,             // initially not owned
      NULL);             // unnamed mutex  
  }

  inline Mutex::~Mutex()
  {
    CloseHandle(mLock);
  }

  inline status_t Mutex::lock()
  {
    DWORD waitResult = WaitForSingleObject(mLock, INFINITE);
    if (waitResult == WAIT_OBJECT_0) {
      return CAPU_OK;
    } else {
      return CAPU_ERROR;
    }
  }

  inline status_t Mutex::unlock()
  {
    if (ReleaseMutex(mLock)) {
      return CAPU_OK;
    } else {
      return CAPU_ERROR;
    }
  }

  inline bool Mutex::trylock()
  {
    DWORD waitResult = WaitForSingleObject(mLock, 0);
    if (waitResult == WAIT_OBJECT_0) {
      return true;
    } else {
      return false;
    }
  }
#endif
