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

#include "common/EtchServerSocket.h"
#include "capu/util/SmartPointer.h"

const EtchObjectType* EtchServerSocket::TYPE() {
  const static EtchObjectType TYPE(EOTID_SERVER_SOCKET, NULL);
  return &TYPE;
}

EtchServerSocket::EtchServerSocket() {
  addObjectType(TYPE());
}


EtchServerSocket::EtchServerSocket(const EtchServerSocket& other)
 : EtchObject(other) {
}

EtchServerSocket::~EtchServerSocket() {
  this->close();
}

status_t EtchServerSocket::listen(capu::uint8_t backlog) {
  return mServerSocket.listen(backlog);
}

status_t EtchServerSocket::close() {
  return mServerSocket.close();
}

EtchSocket* EtchServerSocket::accept() {
  capu::TcpSocket *capu_soc = mServerSocket.accept();
  if(capu_soc == NULL)
    return NULL;
  EtchSocket *sock = new EtchSocket(capu_soc);
  return sock;
}

status_t EtchServerSocket::bind(capu::uint16_t port, const char* addr)
{
  return mServerSocket.bind(port,addr);
}
