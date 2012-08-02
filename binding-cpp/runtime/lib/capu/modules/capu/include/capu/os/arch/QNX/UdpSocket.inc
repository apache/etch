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

#ifdef UDPSOCKET_INC_HEADER
#include <netinet/in.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/time.h>
#include <netdb.h>
#include <unistd.h>
#include <errno.h>
#include <arpa/inet.h>
#include "capu/os/StringUtils.h"
#endif

#ifdef UDPSOCKET_INC_MEMBER
public:
private:
  int32_t mSocket;
  int32_t mAddressFamily;
  int32_t mSocketType;
  bool_t mIsBound;
#endif

#ifdef UDPSOCKET_INC_IMPL
inline UdpSocket::UdpSocket() {
  //create the socket which is used to connect the server
  mAddressFamily = AF_INET;
  mSocketType = SOCK_DGRAM;
  mSocket = socket(mAddressFamily, mSocketType, 0);
  if (mSocket != -1) {
    int32_t optval = 1;
    setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof (optval));
  }
  mIsBound = false;
}

inline UdpSocket::~UdpSocket() {
  close();
}

inline status_t UdpSocket::bind(uint16_t port, const char *addr) {
  if (port == 0) {
    return CAPU_EINVAL;
  }

  if (mIsBound) {
    return CAPU_ERROR;
  }

  if (mSocket == -1) {
      return CAPU_SOCKET_ESOCKET;
  }

  struct sockaddr_in mServerAddress;
  memset((char *) &mServerAddress, 0x00, sizeof (mServerAddress));
  mServerAddress.sin_family = AF_INET;
  if (addr == NULL)
    mServerAddress.sin_addr.s_addr = INADDR_ANY;
  else if (inet_aton(addr, &mServerAddress.sin_addr) == 0)
    return CAPU_SOCKET_EADDR;
  mServerAddress.sin_port = htons(port);

  int32_t res = ::bind(mSocket, (sockaddr *) &mServerAddress, sizeof (struct sockaddr_in));
  if (res < 0) {
    printf("%d\n", errno);
    return CAPU_SOCKET_EBIND;
  }

  mIsBound = true;
  return CAPU_OK;
}


inline status_t UdpSocket::send(unsigned char *buffer, int32_t length, const char *receiverAddr, uint16_t receiverPort) {
  if ((buffer == NULL) || (length < 0)) {
    return CAPU_EINVAL;
  }

  if (mSocket == -1) {
    return CAPU_SOCKET_ESOCKET;
  }

  struct hostent *receiverAddrIp = gethostbyname(receiverAddr);
  if (receiverAddrIp == NULL) {
    return CAPU_SOCKET_EADDR;
  }
  struct sockaddr_in receiverSockAddr;
  receiverSockAddr.sin_family = AF_INET;
  memcpy((char *) &receiverSockAddr.sin_addr.s_addr, (char *) receiverAddrIp->h_addr_list[0], receiverAddrIp->h_length);
  receiverSockAddr.sin_port = htons(receiverPort);

  
  int32_t result = sendto(mSocket,(char*)buffer, length, 0, (sockaddr*)&receiverSockAddr, sizeof(receiverSockAddr));
  if(result == -1)
  {
    return CAPU_ERROR;
  }
  return CAPU_OK;
}

inline status_t UdpSocket::send(unsigned char *buffer, int32_t length, SocketAddrInfo& receiverAddr) {
    return send(buffer, length, receiverAddr.addr, receiverAddr.port);
}

inline status_t UdpSocket::receive(unsigned char *buffer, int32_t length, int32_t &numBytes, SocketAddrInfo* sender) {
  if ((buffer == NULL) || (length < 0)) {
    return CAPU_EINVAL;
  }

  if (mSocket == -1) {
    return CAPU_SOCKET_ESOCKET;
  }

  sockaddr remoteSocketAddr;
  socklen_t remoteSocketAddrSize = sizeof(remoteSocketAddr);

  int32_t result = recvfrom(mSocket, (char*)buffer, length, 0, &remoteSocketAddr, &remoteSocketAddrSize);
  if (result == -1) {
    numBytes = 0;
    if (errno == EAGAIN) {
      return CAPU_ETIMEOUT;
    } else {
      return CAPU_ERROR;
    }
  }else{
    if(sender != 0)
    { 
        sender->port = ntohs(((sockaddr_in*)&remoteSocketAddr)->sin_port);
        char* addr = inet_ntoa(((sockaddr_in*)&remoteSocketAddr)->sin_addr);
        StringUtils::Strncpy(sender->addr, sizeof(sender->addr), addr);
    }
  }
  numBytes = result;
  return CAPU_OK;
}

inline status_t UdpSocket::close() {
  if (mSocket == -1) {
    return CAPU_SOCKET_ESOCKET;
  } else {
    if (::close(mSocket) < 0)
      return CAPU_SOCKET_ECLOSE;
    mSocket = -1;
    return CAPU_OK;
  }
}

inline status_t UdpSocket::setBufferSize(int32_t bufferSize) {
  if (bufferSize < 0) {
    return CAPU_EINVAL;
  }
  if (mSocket == -1) {
    return CAPU_SOCKET_ESOCKET;
  }

  if (setsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, sizeof(bufferSize)) == -1) {
    return CAPU_ERROR;
  }

  return CAPU_OK;
}

inline status_t UdpSocket::setTimeout(int32_t timeout) {
  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;

  struct timeval soTimeout;
  soTimeout.tv_sec = timeout + 1;
  soTimeout.tv_usec = 0;

  if (setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&soTimeout, sizeof(soTimeout)) == -1) {
    return CAPU_ERROR;
  }
  if (setsockopt(mSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&soTimeout, sizeof(soTimeout)) == -1) {
    return CAPU_ERROR;
  }

  return CAPU_OK;
}

inline status_t UdpSocket::getBufferSize(int32_t& bufferSize) {
  if (mSocket == -1) {
    return CAPU_SOCKET_ESOCKET;
  }

  socklen_t len = sizeof (bufferSize);
  if (getsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, &len) == -1) {
    return CAPU_ERROR;
  }

  return CAPU_OK;
}

inline status_t UdpSocket::getTimeout(int32_t& timeout) {
  if (mSocket == -1)
    return CAPU_SOCKET_ESOCKET;

  struct timeval soTimeout;
  socklen_t len = sizeof(soTimeout);

  if (getsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&soTimeout, &len) == -1) {
    return CAPU_ERROR;
  }

  timeout = soTimeout.tv_sec;

  return CAPU_OK;
}
#endif
