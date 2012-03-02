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

#include "common/EtchSocket.h"

const EtchObjectType EtchSocket::TYPE(EOTID_SOCKET, NULL);

EtchSocket::EtchSocket()
: EtchObject(&EtchSocket::TYPE) {
  mSocket = new capu::Socket();
}

EtchSocket::EtchSocket(capu::Socket* soc)
: EtchObject(&EtchSocket::TYPE) {
  if (soc == NULL)
    mSocket = new capu::Socket();
  else
    mSocket = soc;
}

EtchSocket::~EtchSocket() {
  delete mSocket;
}

status_t EtchSocket::close() {
  return mSocket->close();
}

status_t EtchSocket::connect(unsigned char* dest_addr, capu::uint16_t port) {
  return mSocket->connect(dest_addr, port);
}

status_t EtchSocket::receive(unsigned char * buffer, capu::int32_t length, capu::int32_t& numBytes) {
  return mSocket->receive(buffer, length, numBytes);
}

status_t EtchSocket::send(unsigned char* buffer, capu::int32_t length) {
  return mSocket->send(buffer, length);
}

status_t EtchSocket::setBufferSize(capu::int32_t bufferSize) {
  return mSocket->setBufferSize(bufferSize);
}

status_t EtchSocket::setKeepAlive(capu::bool_t keepAlive) {
  return mSocket->setKeepAlive(keepAlive);
}

status_t EtchSocket::setLingerOption(capu::bool_t isLinger, capu::int32_t linger) {
  return mSocket->setLingerOption(isLinger, linger);
}

status_t EtchSocket::setNoDelay(capu::bool_t noDelay) {
  return mSocket->setNoDelay(noDelay);
}

status_t EtchSocket::setTimeout(capu::int32_t timeout) {
  return mSocket->setTimeout(timeout);
}

status_t EtchSocket::getBufferSize(capu::int32_t& bufferSize) {
  return mSocket->getBufferSize(bufferSize);
}

status_t EtchSocket::getKeepAlive(capu::bool_t& keepAlive) {
  return mSocket->getKeepAlive(keepAlive);
}

status_t EtchSocket::getLingerOption(capu::bool_t& isLinger, capu::int32_t& linger) {
  return mSocket->getLingerOption(isLinger, linger);
}

status_t EtchSocket::getNoDelay(capu::bool_t& noDelay) {
  return mSocket->getNoDelay(noDelay);
}

status_t EtchSocket::getTimeout(capu::int32_t& timeout) {
  return mSocket->getTimeout(timeout);
}
