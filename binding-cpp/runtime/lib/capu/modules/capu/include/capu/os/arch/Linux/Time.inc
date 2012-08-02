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
#ifdef TIME_INC_HEADER
#include <time.h>
#endif

#ifdef TIME_INC_MEMBER
#endif

#ifdef TIME_INC_IMPL
inline
uint32_t Time::GetMilliseconds()
{
    timespec time;
    clock_gettime(CLOCK_REALTIME, &time);
    return static_cast<uint32_t>(((static_cast<uint64_t>(time.tv_sec) * 1000000000) + time.tv_nsec) / 1000000);
}


#endif