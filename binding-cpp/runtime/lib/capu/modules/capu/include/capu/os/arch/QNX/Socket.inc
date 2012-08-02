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

#ifdef SOCKET_INC_HEADER
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/tcp.h>
#include <arpa/inet.h>
#include <stdio.h>
#endif

#ifdef SOCKET_INC_MEMBER
private:
int32_t mSocket;
struct sockaddr_in mServerAddress;
struct hostent *mServer;

public:
friend class ServerSocket;

private:
//private constructor needed in ServerSocket
inline Socket(int32_t socket);
#endif

#ifdef SOCKET_INC_IMPL

inline Socket::Socket() {
  mSocket = -1;
  mSocket = socket(AF_INET, SOCK_STREAM, 0);
}

inline Socket::Socket(int32_t socket) {
  mSocket = socket;
}

inline Socket::~Socket() {
  close();
}

inline status_t Socket::send(unsigned char * buffer, int32_t length) {
  if ((buffer == NULL) || (length < 0)) {
    return CAPU_EINVAL;
  }
  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;

  int32_t res = ::send(mSocket, buffer, length, 0);

  if (res < 0) {
    return CAPU_ERROR;
  }
  return CAPU_OK;
}

inline status_t Socket::receive(unsigned char * buffer, int32_t length, int32_t & numBytes) {
  if ((buffer == NULL) || (length < 0)) {
    return CAPU_EINVAL;
  }
  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;

  int32_t res = ::recv(mSocket, buffer, length, 0);

  if (res == -1) {
    numBytes = 0;
    if (errno == EAGAIN)
      return CAPU_ETIMEOUT;
    else
      return CAPU_ERROR;
  }
  numBytes = res;

  return CAPU_OK;
}

inline status_t Socket::close() {
  if (mSocket == -1) {
    return CAPU_SOCKET_ESOCKET;
  } else {
    if (::close(mSocket) < 0)
      return CAPU_SOCKET_ECLOSE;
    mSocket = -1;
    return CAPU_OK;
  }
}

inline status_t Socket::connect(unsigned char * dest_addr, uint16_t port) {
  if ((dest_addr == NULL) || (port == 0))
    return CAPU_EINVAL;

  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;

  mServer = gethostbyname((const char *) dest_addr);
  if (mServer == NULL)
    return CAPU_SOCKET_EADDR;

  // for padding
  memset((char *) &mServerAddress, 0x00, sizeof (mServerAddress));
  mServerAddress.sin_family = AF_INET;

  //destination ip is set
  memcpy((char *) &mServerAddress.sin_addr.s_addr, (char *) mServer->h_addr_list[0], mServer->h_length);

  //port is set
  mServerAddress.sin_port = htons(port);

  int32_t res = ::connect(mSocket, (const sockaddr *) &mServerAddress, sizeof (mServerAddress));

  if (res == -1)
    return CAPU_SOCKET_ECONNECT;
  return CAPU_OK;
}

inline status_t Socket::setBufferSize(int32_t bufferSize) {
  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;
  if (bufferSize < 0)
    return CAPU_EINVAL;
  if (setsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, &bufferSize, sizeof (bufferSize)) < 0)
    return CAPU_ERROR;

  return CAPU_OK;
}

inline status_t Socket::setLingerOption(bool_t isLinger, int32_t linger) {
  if (linger < 0) {
    return CAPU_EINVAL;
  }
  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;

  struct linger so_linger;
  if (isLinger) {
    so_linger.l_onoff = 1;
    so_linger.l_linger = linger;
  } else {
    so_linger.l_onoff = 0;
    so_linger.l_linger = 0;
  }

  if (setsockopt(mSocket, SOL_SOCKET, SO_LINGER, &so_linger, sizeof (so_linger)) < 0)
    return CAPU_ERROR;
  return CAPU_OK;
}

inline status_t Socket::setNoDelay(bool_t noDelay) {
  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;
  int32_t opt;
  if (noDelay)
    opt = 1;
  else
    opt = 0;
  if (setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, &opt, sizeof (opt)) < 0)
    return CAPU_ERROR;
  return CAPU_OK;
}

inline status_t Socket::setKeepAlive(bool_t keepAlive) {
  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;
  int32_t opt;
  if (keepAlive)
    opt = 1;
  else
    opt = 0;
  if (setsockopt(mSocket, SOL_SOCKET, SO_KEEPALIVE, &opt, sizeof (opt)) < 0)
    return CAPU_ERROR;
  return CAPU_OK;
}

inline status_t Socket::setTimeout(int32_t timeout) {
  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;

  struct timeval _timeout;
  _timeout.tv_sec = timeout + 1;
  _timeout.tv_usec = 0;

  if (setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, &_timeout, sizeof (_timeout)) < 0)
    return CAPU_ERROR;
  if (setsockopt(mSocket, SOL_SOCKET, SO_SNDTIMEO, &_timeout, sizeof (_timeout)) < 0)
    return CAPU_ERROR;

  return CAPU_OK;
}

inline status_t Socket::getBufferSize(int32_t& bufferSize) {
  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;
  int32_t size;
  socklen_t len = sizeof (size);
  if (getsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, &size, &len) < 0)
    return CAPU_ERROR;
  bufferSize = size;
  return CAPU_OK;
}

inline status_t Socket::getLingerOption(bool_t& isLinger, int32_t& _linger) {
  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;
  struct linger so_linger;

  socklen_t len = sizeof (so_linger);
  if (getsockopt(mSocket, SOL_SOCKET, SO_LINGER, &so_linger, &len) < 0)
    return CAPU_ERROR;
  _linger = so_linger.l_linger;
  if (so_linger.l_onoff > 0)
    isLinger = true;
  else
    isLinger = false;
	
  return CAPU_OK;
}

inline status_t Socket::getNoDelay(bool_t& noDelay) {
  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;
  int32_t opt;
  socklen_t len = sizeof (opt);
  if (getsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, &opt, &len) < 0)
    return CAPU_ERROR;
  
  if (opt > 0)
    noDelay = true;
  else
    noDelay = false;

  return CAPU_OK;
}

inline status_t Socket::getKeepAlive(bool_t& keepAlive) {
  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;

  int32_t opt;

  socklen_t len = sizeof (opt);
  if (getsockopt(mSocket, SOL_SOCKET, SO_KEEPALIVE, &opt, &len) < 0)
    return CAPU_ERROR;

  if (opt > 0)
    keepAlive = true;
  else
    keepAlive = false;

  return CAPU_OK;
}

inline status_t Socket::getTimeout(int32_t& timeout) {
  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;

  struct timeval _timeout;

  socklen_t len = sizeof (_timeout);

  if (getsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, &_timeout, &len) < 0)
    return CAPU_ERROR;
  timeout = _timeout.tv_sec;

  return CAPU_OK;
}
#endif


