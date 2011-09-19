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

#ifndef __CONDVAR_H__
#define __CONDVAR_H__

#include "capu/Config.h"
#include "capu/Error.h"
#include "capu/os/Mutex.h"

#define CONDVAR_INC_HEADER
#include "arch/CondVar.inc"
#undef CONDVAR_INC_HEADER

namespace capu
{
    class CondVar
    {
     #define CONDVAR_INC_MEMBER
         #include "arch/CondVar.inc"
     #undef CONDVAR_INC_MEMBER

     public:

        inline CondVar();

        ~CondVar();

        /**
         * Wake up single thread that is waiting for this condition variable
         * @return CAPU_OK if the condition variable is correctly signaled
         *         CAPU_ERROR otherwise
         */
        inline status_t signal();

         /**
         * Wait for a condition variable
         * @param  mutex
         * @return CAPU_OK if the condition variable is correctly waited
         *         CAPU_EINVAL if the given mutex is NULL
         *         CAPU_ERROR otherwise
         */
        inline status_t wait(Mutex *mutex);

        /**
         * Wake up all threads that is waiting for this condition variable
         * @return CAPU_OK if the condition variable is correctly broadcasted
         *         CAPU_ERROR otherwise
         */
        inline status_t broadcast();

    };

#define CONDVAR_INC_IMPL
    #include "arch/CondVar.inc"
#undef CONDVAR_INC_IMPL

}
#endif

