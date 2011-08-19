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
#include <pthread.h>
#endif

#ifdef MUTEX_INC_MEMBER
public:
    friend class CondVar;

private:
    pthread_mutex_t     m_lock;
    pthread_mutexattr_t m_lock_attr;
#endif

#ifdef MUTEX_INC_IMPL
inline Mutex::Mutex()
{
    pthread_mutexattr_init(&m_lock_attr);
    pthread_mutex_init(&m_lock,&m_lock_attr);
}
inline Mutex::~Mutex()
{
    pthread_mutex_destroy(&m_lock);
    pthread_mutexattr_destroy(&m_lock_attr);
}
inline status_t Mutex::lock()
{
    if(pthread_mutex_lock(& m_lock)==0)
        return CAPU_OK;
    else
        return CAPU_ERROR;
}
inline status_t Mutex::unlock()
{
    if(pthread_mutex_unlock(& m_lock)==0)
        return CAPU_OK;
    else
        return CAPU_ERROR;
}
inline bool Mutex::trylock()
{
    if(pthread_mutex_trylock(& m_lock)==0)
        return true;
    else
        return false;
}
#endif
