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
#include "common/EtchException.h"

const EtchObjectType* EtchException::TYPE() {
  const static EtchObjectType TYPE(EOTID_EXCEPTION, NULL);
  return &TYPE;
}

EtchException::EtchException()
: EtchObject(), mErrorMessage(""), mErrorcode(0), mExcptype(0) {
  addObjectType(EtchException::TYPE());
}

EtchException::EtchException(EtchString msg, status_t errcode, EtchExceptionType type)
: EtchObject(), mErrorMessage(msg), mErrorcode(errcode), mExcptype(type) {
  addObjectType(EtchException::TYPE());
}

EtchException::EtchException(const EtchException& other)
 : EtchObject(other), mErrorMessage(other.mErrorMessage), mErrorcode(other.mErrorcode), mExcptype(other.mExcptype) {
}

EtchException::~EtchException() {

}

capu::bool_t EtchException::equals(const EtchObject* other) const {
  if (other == NULL)
    return false;
  if (other->getObjectType() != EtchException::TYPE())
    return false;
  EtchException * object = (EtchException *) other;

  return ((object->mErrorcode == this->mErrorcode)
          && (object->mExcptype == this->mExcptype)
          && (object->mErrorMessage.equals(&this->mErrorMessage)));

}

EtchString EtchException::getErrorMessage() {
  return mErrorMessage;
}

status_t EtchException::getErrorCode() {
  return mErrorcode;
}

EtchException::EtchExceptionType EtchException::getExceptionType() {
  return mExcptype;
}
