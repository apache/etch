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

#ifndef __ETCHERROR_H__
#define __ETCHERROR_H__

#include "EtchConfig.h"

typedef capu::int32_t status_t;

enum {

  //RANGE BETWEEN 0X0000 AND 0XFFFF are shared with CAPU
    ETCH_OK = 0,
    ETCH_EUNIMPL = 1,
    ETCH_ERANGE = 2,
    ETCH_EINVAL = 3,
    ETCH_ERROR = 4,
    ETCH_SOCKET_EBIND = 5,
    ETCH_SOCKET_ESOCKET = 6,
    ETCH_SOCKET_ECONNECT = 7,
    ETCH_SOCKET_ELISTEN = 8,
    ETCH_SOCKET_ECLOSE = 9,
    ETCH_SOCKET_EADDR = 10,
    ETCH_ENO_MEMORY = 11,
    ETCH_TIMEOUT = 12,
    ETCH_ENOT_EXIST = 13,
    ETCH_ENOT_SUPPORTED = 14,
    ETCH_EIO = 15,

  //RANGE BETWEEN 0X10000 TO OXFFFFFFF ARE ERROR CODES FOR ONLY ETCH
    ETCH_UNWANTED_MESSAGE = 0X10000,
    ETCH_UKNOWN_OP,
    ETCH_UNSUPP_OP
};

#endif
