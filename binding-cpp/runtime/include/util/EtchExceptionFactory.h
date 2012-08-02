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

#ifndef __ETCHEXCEPTIONFACTORY_H__
#define __ETCHEXCEPTIONFACTORY_H__

#include "common/EtchError.h"
#include "common/EtchRuntimeException.h"
#include "common/EtchAuthenticationException.h"

class EtchExceptionFactory
{
public:
  static EtchException* getException(status_t EtchError)
  {
    switch(EtchError)
    {
      case ETCH_EUNIMPL:
          return new EtchRuntimeException(EtchString("Unimplemented function was called"), ETCH_EUNIMPL);
      case ETCH_ERANGE:
          return new EtchRuntimeException(EtchString("Invalid Range of given parameters"), ETCH_ERANGE);
      case ETCH_EINVAL:
          return new EtchRuntimeException(EtchString("Wrong input Value"), ETCH_EINVAL);
      case ETCH_ENO_MEMORY:
          return new EtchRuntimeException(EtchString("Out of Memory"), ETCH_ENO_MEMORY);
      case ETCH_TIMEOUT:
          return new EtchRuntimeException(EtchString("Timeout occured"), ETCH_TIMEOUT);
      case ETCH_ENOT_EXIST:
        return new EtchRuntimeException(EtchString("Value does not exist"), ETCH_ENOT_EXIST);
      case ETCH_ENOT_SUPPORTED:
        return new EtchRuntimeException(EtchString("Operation is not supported"), ETCH_ENOT_SUPPORTED);
      case ETCH_UNWANTED_MESSAGE:
        return new EtchRuntimeException(EtchString("Unwanted Message was received"), ETCH_UNWANTED_MESSAGE);
      case ETCH_UKNOWN_OP:
          return new EtchRuntimeException(EtchString("Unknown Operation was called"), ETCH_UKNOWN_OP);
      case ETCH_UNSUPP_OP:
          return new EtchRuntimeException(EtchString("Unsupported Operation"), ETCH_UNSUPP_OP);
      case ETCH_SOCKET_EBIND:
          return new EtchRuntimeException(EtchString("Socket can not be bind"), ETCH_SOCKET_EBIND);
      case ETCH_SOCKET_ESOCKET:
          return new EtchRuntimeException(EtchString("Unknown socket expcetion"), ETCH_SOCKET_ESOCKET);
      case ETCH_SOCKET_ECONNECT:
          return new EtchRuntimeException(EtchString("Socket can not connect"), ETCH_SOCKET_ECONNECT);
      case ETCH_SOCKET_ELISTEN:
          return new EtchRuntimeException(EtchString("Error while trying to listen on given socket"), ETCH_SOCKET_ELISTEN);
      case ETCH_SOCKET_ECLOSE:
          return new EtchRuntimeException(EtchString("Socket is already closed"), ETCH_SOCKET_ECLOSE);
      case ETCH_SOCKET_EADDR:
          return new EtchRuntimeException(EtchString("Invalid Socket Address"), ETCH_SOCKET_EADDR);
      case ETCH_ERROR:
      default:
        return new EtchRuntimeException(EtchString("Undefined Exception occurred!"), ETCH_ERROR);
    }
  }

};

#endif
