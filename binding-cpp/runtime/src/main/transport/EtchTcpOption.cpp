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

#include "transport/EtchTcpOption.h"

const EtchString& EtchTcpOption::BUFFER_SIZE() {
  static const EtchString name("TcpTransport.bufferSize");
  return name;
}

const EtchString& EtchTcpOption::KEEP_ALIVE() {
  static const EtchString name("TcpTransport.keepAlive");
  return name;
}

const EtchString& EtchTcpOption::LINGER_TIME() {
  static const EtchString name("TcpTransport.lingerTime");
  return name;
}

const EtchString& EtchTcpOption::NO_DELAY() {
  static const EtchString name("TcpTransport.noDelay");
  return name;
}

const EtchString& EtchTcpOption::RECONNECT_DELAY() {
  static const EtchString name("TcpTransport.reconnectDelay");
  return name;
}

EtchTcpOption::EtchTcpOption(EtchURL *url)
: mBufferSize(0), mLingerTime(30), mReconnectDelay(10), mKeepAlive(0), mNoDelay(1) {
  EtchString result;
  if (url->getTerms().get(BUFFER_SIZE(), &result) != ETCH_ENOT_EXIST) {
    const char * str = result.c_str();

    if (str != NULL) {
      capu::int32_t len = capu::StringUtils::Strlen(str);
      if (isValidNumber(str, len))
        mBufferSize = atoi(str);
      str = NULL;
    }
  }

  if (url->getTerms().get(KEEP_ALIVE(), &result) != ETCH_ENOT_EXIST) {
    const char * str = result.c_str();
    if (str != NULL) {
      capu::int32_t len = capu::StringUtils::Strlen(str);
      if (isValidNumber(str, len))
        mKeepAlive = atoi(str);
      str = NULL;
    }
  }

  if (url->getTerms().get(LINGER_TIME(), &result) != ETCH_ENOT_EXIST) {
    const char * str = result.c_str();
    if (str != NULL) {
      capu::int32_t len = capu::StringUtils::Strlen(str);
      //check if the string contains only digits
      if (isValidNumber(str, len))
        mLingerTime = atoi(str);
    }
  }

  if (url->getTerms().get(NO_DELAY(), &result) != ETCH_ENOT_EXIST) {
    const char * str = result.c_str();
    if (str != NULL) {

      capu::int32_t len = capu::StringUtils::Strlen(str);

      if (isValidNumber(str, len))
        mNoDelay = atoi(str);
      delete[] str;
    }
  }

  if (url->getTerms().get(RECONNECT_DELAY(), &result) != ETCH_ENOT_EXIST) {
    const char * str = result.c_str();
    if (str != NULL) {
      //check if the string contains only digits
      capu::int32_t len = capu::StringUtils::Strlen(str);

      if (isValidNumber(str, len))
        mReconnectDelay = atoi(str);
      str = NULL;
    }
  }

  if (checkBufferSize(mBufferSize) == false)
    mBufferSize = 0;

  if (checkLingerTime(mLingerTime) == false)
    mLingerTime = 30;

  if (checkReconnectDelay(mReconnectDelay) == false)
    mReconnectDelay = 10;
}

EtchTcpOption::~EtchTcpOption() {

}

capu::bool_t EtchTcpOption::checkBufferSize(capu::int32_t size) {
  if ((size < 0) || (size > 65536))
    return false;
  return true;
}

capu::bool_t EtchTcpOption::checkLingerTime(capu::int32_t lingertime) {
  if ((lingertime <= -1) || (lingertime > 240))
    return false;
  return true;
}

capu::bool_t EtchTcpOption::checkReconnectDelay(capu::int32_t delay) {

  if (delay < 0)
    return false;
  return true;
}

capu::bool_t EtchTcpOption::isValidNumber(const char * str, capu::int32_t len) {
  for (capu::int32_t i = 0; i < len; i++) {
    if ((str[i] > 57) || (str[i] < 48)) {
      return false;
    }
  }
  return true;
}


capu::int32_t EtchTcpOption::getBufferSize() {
  return mBufferSize;
}

capu::int32_t EtchTcpOption::getLingerTime() {
  return mLingerTime;
}

capu::int32_t EtchTcpOption::getReconnectDelay()  {
  return mReconnectDelay;
}

capu::int32_t EtchTcpOption::getKeepAlive() {
  return mKeepAlive;
}

capu::int32_t EtchTcpOption::getNoDelay() {
  return mNoDelay;
}
