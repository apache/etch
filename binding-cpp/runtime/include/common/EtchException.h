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

#ifndef __ETCHEXCEPTION_H__
#define __ETCHEXCEPTION_H__

#include "common/EtchObject.h"
#include "common/EtchString.h"

class EtchException : public EtchObject {
public:

  typedef capu::uint32_t EtchExceptionType;

  enum {
    EXCPTYPE_NONE = 0x0,
    EXCPTYPE_BUILTIN = 0x1,
    EXCPTYPE_USERDEFINED = 0x2
  };


  //Constructor
  EtchException();

  /**
   * EtchException
   * @param msg Exception Message
   */
  EtchException(EtchString msg, status_t errcode, EtchExceptionType type);

  //destructor
  virtual ~EtchException();

  //overridden
  virtual capu::bool_t equals(const EtchObject * other);

  /**
   *
   * @return Message of Exception
   */
  EtchString getErrorMessage();

  /**
   *
   * @return Error Code of Exception
   */
  status_t getErrorCode();

  /**
   *
   * @return Type of Exception
   */
  EtchExceptionType getExceptionType();

  static const EtchObjectType* TYPE();

protected:

  EtchString mErrorMessage;
  status_t mErrorcode;
  EtchExceptionType mExcptype;

  /**
   * @param msg Message of exception
   * @param errcode Error Code such as ETCH_EINVAL etc.
   * @param type    Exception Type such as EXCPTYPE_USERDEFINED etc.
   * @param type_id Typeid of Exception
   */
  EtchException(EtchString msg, status_t errcode, EtchExceptionType type, const EtchObjectType* type_id);
};

#endif /* ETCHEXCEPTION_H */

