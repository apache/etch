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

const EtchObjectType* EtchSocket::TYPE() {
  const static EtchObjectType TYPE(EOTID_SOCKET, NULL);
  return &TYPE;
}

EtchSocket::EtchSocket() {
  addObjectType(TYPE());
  mSocket = new capu::TcpSocket();
}

EtchSocket::EtchSocket(capu::TcpSocket* soc) {
  addObjectType(TYPE());
  if (soc == NULL)
    mSocket = new capu::TcpSocket();
  else
    mSocket = soc;
}

EtchSocket::EtchSocket(const EtchSocket& other)
 : EtchObject(other), mSocket(other.mSocket) {
}

EtchSocket::~EtchSocket() {
  delete mSocket;
}

status_t EtchSocket::close() {
  return mSocket->close();
}

status_t EtchSocket::connect(const char* dest_addr, capu::uint16_t port) {
  return mSocket->connect(dest_addr, port);
}

status_t EtchSocket::receive(char * buffer, capu::int32_t length, capu::int32_t& numBytes) {
  return mSocket->receive(buffer, length, numBytes);
}

status_t EtchSocket::send(const char* buffer, capu::int32_t length) {
  capu::int32_t numBytes = 0;
  return mSocket->send(buffer, length, numBytes);
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

status_t EtchSocket::getRemoteAddress( capu::char_t** address )
{
  return mSocket->getRemoteAddress(address);
}
