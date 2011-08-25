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
  DWORD  mThreadId;
  HANDLE mThreadHandle;

  template <typename T>
  static DWORD WINAPI run(LPVOID arg);
  
  template <class K>
  class ThreadParam
  {
  public:
    K* object;
    void* param;
  };
#endif

#ifdef THREAD_INC_IMPL
  template <typename T>
  static DWORD WINAPI Thread::run(LPVOID param)
  {
    ThreadParam<T>* args= (ThreadParam<T>*) param;
    (*args->object)(args->param);
    delete args;
    return NULL;
  }

template <typename T>
inline Thread::Thread(T *object,void* args)
{ 
  if (object == NULL)
  {
    return;
  }
  ThreadParam<T> *param = new ThreadParam<T>();
  param->object=object;
  param->param=args;
  mThreadHandle = CreateThread(NULL, 0, Thread::run<T>, param, 0, &mThreadId);
}

inline Thread::~Thread()
{
  join();
}

inline status_t Thread::join()
{
  if (mThreadHandle && WaitForSingleObject(mThreadHandle,INFINITE) == 0) {
    return CAPU_OK;
  } else {
    return CAPU_ERROR;
  }
}

inline status_t Thread::Sleep(uint32_t millis)
{
  ::Sleep(millis);
  return CAPU_OK;
}

#endif
