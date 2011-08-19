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
    CRITICAL_SECTION     m_lock;
#endif

#ifdef MUTEX_INC_IMPL
inline Mutex::Mutex()
{
	InitializeCriticalSection(&m_lock);
}
inline Mutex::~Mutex()
{
	DeleteCriticalSection(&m_lock);
}
inline status_t Mutex::lock()
{
	EnterCriticalSection(&m_lock);
	return CAPU_OK;
}
inline status_t Mutex::unlock()
{
	LeaveCriticalSection(&m_lock);
	return CAPU_OK;
}
inline bool Mutex::trylock()
{
	TryEnterCriticalSection(&m_lock);
	return true;
}
#endif
