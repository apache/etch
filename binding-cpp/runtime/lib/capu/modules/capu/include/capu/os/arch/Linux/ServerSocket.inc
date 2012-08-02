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

#ifdef SERVER_SOCKET_INC_HEADER
#include <sys/socket.h>
#include <netdb.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#endif

#ifdef SERVER_SOCKET_INC_MEMBER
private:
int32_t mServerSock;
bool_t mIsBound;
#endif

#ifdef SERVER_SOCKET_INC_IMPL

inline ServerSocket::ServerSocket() {
  mServerSock = -1;
  mIsBound = false;
  mServerSock = socket(AF_INET, SOCK_STREAM, 0);
}

inline ServerSocket::~ServerSocket() {
  close();
}

inline Socket* ServerSocket::accept() {
  int32_t clientAddrSize = sizeof (sockaddr_in);
  struct sockaddr_in serverAddr;
  capu::int32_t socketHandle = ::accept(mServerSock, (sockaddr *) & serverAddr, (socklen_t *) & clientAddrSize);
  if (socketHandle < 0) {
    return NULL;
  }
  Socket *s = new Socket(socketHandle);
  s->mServerAddress = serverAddr;
  return s;
}

inline status_t ServerSocket::close() {
  int32_t returnValue = CAPU_OK;
  if (mServerSock != -1) {
    shutdown(mServerSock, SHUT_RDWR);
    if (::close(mServerSock) < 0) {
      returnValue = CAPU_ERROR;
    }
  } else {
    returnValue = CAPU_SOCKET_ESOCKET;
  }
  mServerSock = -1;
  mIsBound = false;
  return returnValue;
}

inline status_t ServerSocket::bind(uint16_t port, const char * addr) {

  sockaddr_in mServerAddress;
  if (port == 0)
    return CAPU_EINVAL;

  if (mIsBound)
    return CAPU_ERROR;

  if (mServerSock == -1)
    return CAPU_SOCKET_ESOCKET;

  int32_t optval = 1;
  setsockopt(mServerSock, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval));
  memset((char *) &mServerAddress, 0x00, sizeof (mServerAddress));
  mServerAddress.sin_family = AF_INET;
  if (addr == NULL)
    mServerAddress.sin_addr.s_addr = INADDR_ANY;
  else if (inet_aton(addr, &mServerAddress.sin_addr) == 0)
    return CAPU_SOCKET_EADDR;
  mServerAddress.sin_port = htons(port);

  int32_t res = ::bind(mServerSock, (sockaddr *) & mServerAddress, sizeof (struct sockaddr_in));
  if (res < 0) {
    return CAPU_SOCKET_EBIND;
  }

  mIsBound = true;
  return CAPU_OK;
}

inline status_t ServerSocket::listen(uint8_t backlog) {
  if (!mIsBound)
    return CAPU_EINVAL;

  if (mServerSock == -1)
    return CAPU_SOCKET_ESOCKET;

  if (::listen(mServerSock, backlog) < 0)
    return CAPU_ERROR;
  return CAPU_OK;
}

#endif
