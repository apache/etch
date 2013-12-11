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

#include "common/EtchRuntimeException.h"

const EtchObjectType* EtchRuntimeException::TYPE() {
  const static EtchObjectType TYPE(EOTID_RUNTIME_EXCEPTION, NULL);
  return &TYPE;
}


EtchRuntimeException::EtchRuntimeException(EtchString msg, status_t errcode)
: EtchException(msg, errcode, EtchException::EXCPTYPE_BUILTIN) {
  addObjectType(TYPE());
}

EtchRuntimeException::EtchRuntimeException(const EtchRuntimeException& other)
: EtchException(other) {
}

EtchRuntimeException::~EtchRuntimeException() {

}

capu::bool_t EtchRuntimeException::equals(const EtchObject* other) const {
  if (other == NULL)
    return false;
  else if (other->getObjectType() != EtchRuntimeException::TYPE())
    return false;
  EtchRuntimeException * ptr = (EtchRuntimeException*) other;
  return ((ptr->mErrorcode == this->mErrorcode)
          && (ptr->mExcptype == this->mExcptype)
          && (ptr->mErrorMessage.equals(&this->mErrorMessage)));
}
