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
 
#ifndef __MUTEX_H__
#define __MUTEX_H__

#define MUTEX_INC_HEADER
#include "arch/Mutex.inc"
#undef MUTEX_INC_HEADER

#include "capu/Error.h"

namespace capu
{
    class Mutex
    {

    #define MUTEX_INC_MEMBER
      #include "arch/Mutex.inc"
    #undef MUTEX_INC_MEMBER

    public:

        /**
         * Constructor
         */
        inline Mutex();

        /**
         * Destructor
         */
        inline ~Mutex();

        /**
         * used for locking if lock is not currently available, then wait until the lock is captured
         * @return CAPU_OK if the locking is successful
         *         CAPU_ERROR otherwise
         */
        inline status_t lock();

         /**
         * it will attempt to lock a mutex. However if the mutex is already locked, the routine will return false immediately
         * @return true if the mutex is successfully locked
         *         false if the mutex is already locked
         */
        inline bool trylock();
        
         /**
         *release the lock
         *@return CAPU_OK if the unlocking is successful
         *        CAPU_ERROR otherwise 
         */
        inline status_t unlock();

        

    };

#define MUTEX_INC_IMPL
    #include "arch/Mutex.inc"
#undef MUTEX_INC_IMPL
}

#endif

