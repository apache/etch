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

#ifndef __ERROR_H__
#define __ERROR_H__

#include "Config.h"

namespace capu {
  typedef int32_t status_t;

  enum Errors{
    CAPU_OK = 0,
    CAPU_EUNIMPL = 1,
    CAPU_ERANGE = 2,
    CAPU_EINVAL = 3,
    CAPU_ERROR = 4,
    CAPU_SOCKET_EBIND = 5,
    CAPU_SOCKET_ESOCKET = 6,
    CAPU_SOCKET_ECONNECT = 7,
    CAPU_SOCKET_ELISTEN = 8,
    CAPU_SOCKET_ECLOSE = 9,
    CAPU_SOCKET_EADDR = 10,
    CAPU_ENO_MEMORY = 11,
    CAPU_ETIMEOUT = 12,
    CAPU_ENOT_EXIST = 13,
    CAPU_ENOT_SUPPORTED = 14,
    CAPU_EIO = 15,
    CAPU_EOF = 16
  };
}
#endif

