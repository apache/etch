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
#define WIN32_LEAN_AND_MEAN
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include "capu/os/StringUtils.h"
#endif

#ifdef UDPSOCKET_INC_MEMBER
public:
private:
  SOCKET mSocket;
  WSADATA mWsaData;
  int32_t mAddressFamily;
  int32_t mSocketType;
  int32_t mProtocol;
  bool_t mIsBound;
#endif

#ifdef UDPSOCKET_INC_IMPL
inline UdpSocket::UdpSocket() {
  //Initialize Winsock
  int32_t result = WSAStartup(MAKEWORD(2,2), &mWsaData);
  if (result == 0) {
    //create the socket which is used to connect the server
    mAddressFamily = AF_INET;
    mSocketType = SOCK_DGRAM;
    mProtocol = IPPROTO_UDP;
    mSocket = socket(mAddressFamily, mSocketType, mProtocol);
    mIsBound = false;
    if (mSocket == INVALID_SOCKET) {
      WSACleanup();
    } else {
      int32_t optVal = 1;
      setsockopt(mSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optVal, sizeof(optVal));
    }
  } else {
    mSocket = INVALID_SOCKET;
  }
}

inline UdpSocket::~UdpSocket() {
  close();
}

inline status_t UdpSocket::bind(uint16_t port, const char *addr) {
  if (port == 0) {
    return CAPU_EINVAL;
  }

  //check if the address is valid

  if (mIsBound) {
    return CAPU_ERROR;
  }

  if (mSocket == INVALID_SOCKET)
    return CAPU_SOCKET_ESOCKET;

  sockaddr_in socketAddr;
  memset((char*) &socketAddr, 0x00, sizeof(socketAddr));
  socketAddr.sin_family = AF_INET;
  if (addr == NULL) {
    socketAddr.sin_addr.s_addr = INADDR_ANY;
  } else if (inet_addr(addr) == INADDR_NONE) {
    return CAPU_SOCKET_EADDR;
  } else {
    socketAddr.sin_addr.s_addr = inet_addr(addr);
  }
  socketAddr.sin_port = htons(port);

  int32_t result = ::bind(mSocket, (sockaddr*) &socketAddr, sizeof(socketAddr));
  if (result == SOCKET_ERROR) {
    return CAPU_SOCKET_EBIND;
  }

  mIsBound = true;
  return CAPU_OK;
}


inline status_t UdpSocket::send(unsigned char *buffer, int32_t length, const char* receiverAddr, uint16_t receiverPort) {
  if ((buffer == NULL) || (length < 0)) {
    return CAPU_EINVAL;
  }

  if (mSocket == INVALID_SOCKET) {
    return CAPU_SOCKET_ESOCKET;
  }

  struct hostent* serverHost = gethostbyname((const char *) receiverAddr);
  if (serverHost == NULL) {
    return CAPU_SOCKET_EADDR;
  }

  struct sockaddr_in receiverSockAddr;
  memset((char *) &receiverSockAddr, 0x00, sizeof (receiverSockAddr));

  receiverSockAddr.sin_family = AF_INET;
  memcpy((char *) &receiverSockAddr.sin_addr.s_addr, (char *) serverHost->h_addr_list[0], serverHost->h_length);
  receiverSockAddr.sin_port = htons(receiverPort);

  int32_t result = sendto(mSocket,(char*)buffer, length, 0, (sockaddr*) &receiverSockAddr, sizeof(receiverSockAddr));
  if (result == SOCKET_ERROR) {
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

  if (mSocket == INVALID_SOCKET) {
    return CAPU_SOCKET_ESOCKET;
  }

  sockaddr remoteSocketAddr;
  int32_t remoteSocketAddrSize = sizeof(remoteSocketAddr);

  int32_t result = recvfrom(mSocket, (char*)buffer, length, 0, &remoteSocketAddr, &remoteSocketAddrSize);
  if (result == SOCKET_ERROR) {
    numBytes = 0;
    result = WSAGetLastError();
    if (result == WSAETIMEDOUT) {
      return CAPU_ETIMEOUT;
    }
    else{
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
  int32_t returnValue = CAPU_OK;
  if (mSocket == INVALID_SOCKET) {
    returnValue = CAPU_SOCKET_ESOCKET;
  } else {
    int32_t result = closesocket(mSocket);
    if (result != 0) {
      result = WSAGetLastError();
      if (result != WSANOTINITIALISED){ //socket has already been closed
        returnValue = CAPU_SOCKET_ECLOSE;
      }
    }
  }
  mSocket = INVALID_SOCKET;
  WSACleanup();
  return returnValue;
}

inline status_t UdpSocket::setBufferSize(int32_t bufferSize) {
  if (bufferSize < 0) {
    return CAPU_EINVAL;
  }
  if (mSocket == INVALID_SOCKET) {
    return CAPU_SOCKET_ESOCKET;
  }

  if (setsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, sizeof(bufferSize)) == SOCKET_ERROR) {
    return CAPU_ERROR;
  }

  return CAPU_OK;
}

inline status_t UdpSocket::setTimeout(int32_t timeout) {
  if (mSocket == INVALID_SOCKET)
    return CAPU_SOCKET_ESOCKET;

  struct timeval soTimeout;
  soTimeout.tv_sec = timeout;
  soTimeout.tv_usec = 0;

  if (setsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&soTimeout, sizeof(soTimeout)) == SOCKET_ERROR) {
    return CAPU_ERROR;
  }
  if (setsockopt(mSocket, SOL_SOCKET, SO_SNDTIMEO, (char*)&soTimeout, sizeof(soTimeout)) == SOCKET_ERROR) {
    return CAPU_ERROR;
  }

  return CAPU_OK;
}

inline status_t UdpSocket::getBufferSize(int32_t& bufferSize) {
  if (mSocket == INVALID_SOCKET) {
    return CAPU_SOCKET_ESOCKET;
  }

  socklen_t len = sizeof (bufferSize);
  if (getsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, &len) == SOCKET_ERROR) {
    return CAPU_ERROR;
  }

  return CAPU_OK;
}

inline status_t UdpSocket::getTimeout(int32_t& timeout) {
  if (mSocket == INVALID_SOCKET)
    return CAPU_SOCKET_ESOCKET;

  struct timeval soTimeout;
  socklen_t len = sizeof(soTimeout);

  if (getsockopt(mSocket, SOL_SOCKET, SO_RCVTIMEO, (char*)&soTimeout, &len) == SOCKET_ERROR) {
    return CAPU_ERROR;
  }

  timeout = soTimeout.tv_sec;

  return CAPU_OK;
}
#endif
