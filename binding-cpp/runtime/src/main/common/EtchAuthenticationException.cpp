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
#include "common/EtchAuthenticationException.h"

const EtchObjectType* EtchAuthenticationException::TYPE() {
  const static EtchObjectType TYPE(EOTID_AUTH_EXCEPTION, NULL);
  return &TYPE;
}

EtchAuthenticationException::EtchAuthenticationException(EtchString& msg)
: EtchException(msg, ETCH_ERROR, EXCPTYPE_BUILTIN) {
  addObjectType(TYPE());
}

EtchAuthenticationException::EtchAuthenticationException(const EtchAuthenticationException& other)
: EtchException(other) {
}

EtchAuthenticationException::~EtchAuthenticationException() {

}

capu::bool_t EtchAuthenticationException::equals(const EtchObject* other) const {
  if (other == NULL)
    return false;
  else if (other->getObjectType() != EtchAuthenticationException::TYPE())
    return false;
  EtchAuthenticationException * ptr = (EtchAuthenticationException*) other;
  return ((ptr->mErrorcode == this->mErrorcode)
          && (ptr->mExcptype == this->mExcptype)
          && (ptr->mErrorMessage.equals(&this->mErrorMessage)));
}

