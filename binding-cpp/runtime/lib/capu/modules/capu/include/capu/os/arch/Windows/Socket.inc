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
#define _WINSOCKAPI_
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#ifdef SOCKET_INC_MEMBER
public:
  friend class ServerSocket;
private:
  SOCKET mSocket;
  WSADATA mWsaData;

  //private constructor needed in ServerSocket
  inline Socket(SOCKET socket);
#endif

#ifdef SOCKET_INC_IMPL

inline Socket::Socket() {
  //Initialize Winsock
  int32_t result = WSAStartup(MAKEWORD(2,2), &mWsaData);
  if (result == 0) {
    //create the socket which is used to connect the server
    mSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mSocket == INVALID_SOCKET) {
      WSACleanup();
    }
  } else {
    mSocket = INVALID_SOCKET;
  }
}

inline Socket::Socket(SOCKET socket) {
  //Initialize Winsock
  int32_t result = WSAStartup(MAKEWORD(2,2), &mWsaData);
  if (result == 0) {
    //create the socket which is used to connect the server
    mSocket = socket;
    if (mSocket == INVALID_SOCKET) {
      WSACleanup();
    }
  } else {
    mSocket = INVALID_SOCKET;
  }
}

inline Socket::~Socket() {
  close();
}

inline status_t Socket::send(unsigned char *buffer, int32_t length) {
  if ((buffer == NULL) || (length < 0)) {
    return CAPU_EINVAL;
  }

  if (mSocket == INVALID_SOCKET) {
    return CAPU_SOCKET_ESOCKET;
  }

  int32_t result;
  result = ::send(mSocket, (const char*)buffer, length, 0);
  if (result == SOCKET_ERROR) {
    close();
    return CAPU_ERROR;
  }
  return CAPU_OK;
}

inline status_t Socket::receive(unsigned char *buffer, int32_t length, int32_t &numBytes) {
  if ((buffer == NULL) || (length < 0)) {
    return CAPU_EINVAL;
  }

  if (mSocket == INVALID_SOCKET) {
    return CAPU_SOCKET_ESOCKET;
  }

  int32_t result = recv(mSocket, (char*)buffer, length, 0);
  if (result == SOCKET_ERROR) {
    numBytes = 0;
    result = WSAGetLastError();
    if (result == WSAETIMEDOUT) {
      return CAPU_ETIMEOUT;
    }
    else{
      return CAPU_ERROR;
    }
  }
  numBytes = result;
  return CAPU_OK;
}

inline status_t Socket::close() {
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

inline status_t Socket::connect(unsigned char *dest_addr, uint16_t port) {

  //check parameters
  if ((dest_addr == NULL) || (port == 0)) {
    return CAPU_EINVAL;
  }

  if (mSocket == INVALID_SOCKET) {
    return CAPU_SOCKET_ESOCKET;
  }

  struct hostent* serverHost = gethostbyname((const char *) dest_addr);
  if (serverHost == NULL) {
    return CAPU_SOCKET_EADDR;
  }

  struct sockaddr_in serverAddress;
  memset((char *) &serverAddress, 0x00, sizeof (serverAddress));

  serverAddress.sin_family = AF_INET;
  memcpy((char *) &serverAddress.sin_addr.s_addr, (char *) serverHost->h_addr_list[0], serverHost->h_length);
  serverAddress.sin_port = htons(port);

  int32_t result = ::connect(mSocket, (sockaddr *) &serverAddress, sizeof (serverAddress));
  if (result == SOCKET_ERROR) {
    int32_t errorCode = WSAGetLastError();
    close();
    return CAPU_SOCKET_ECONNECT;
  }
  return CAPU_OK;
}

inline status_t Socket::setBufferSize(int32_t bufferSize) {
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

inline status_t Socket::setLingerOption(bool_t isLinger, int32_t linger) {
  if (linger < 0) {
    return CAPU_EINVAL;
  }
  if (mSocket == INVALID_SOCKET) {
    return CAPU_SOCKET_ESOCKET;
  }

  struct linger soLinger;
  if (isLinger) {
    soLinger.l_onoff = 1;
    soLinger.l_linger = linger;
  } else {
    soLinger.l_onoff = 0;
    soLinger.l_linger = 0;
  }

  if (setsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)&soLinger, sizeof(soLinger)) == SOCKET_ERROR) {
    return CAPU_ERROR;
  }
  return CAPU_OK;
}

inline status_t Socket::setNoDelay(bool_t noDelay) {
  if (mSocket == INVALID_SOCKET)
    return CAPU_SOCKET_ESOCKET;
  int32_t opt;
  if (noDelay) {
    opt = 1;
  } else {
    opt = 0;
  }
  if (setsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
    return CAPU_ERROR;
  }
  return CAPU_OK;
}

inline status_t Socket::setKeepAlive(bool_t keepAlive) {
  if (mSocket == INVALID_SOCKET)
    return CAPU_SOCKET_ESOCKET;
  int32_t opt;
  if (keepAlive) {
    opt = 1;
  } else {
    opt = 0;
  }
  if (setsockopt(mSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&opt, sizeof(opt)) == SOCKET_ERROR) {
    return CAPU_ERROR;
  }
  return CAPU_OK;
}

inline status_t Socket::setTimeout(int32_t timeout) {
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

inline status_t Socket::getBufferSize(int32_t& bufferSize) {
  if (mSocket == INVALID_SOCKET) {
    return CAPU_SOCKET_ESOCKET;
  }

  socklen_t len = sizeof (bufferSize);
  if (getsockopt(mSocket, SOL_SOCKET, SO_RCVBUF, (char*)&bufferSize, &len) == SOCKET_ERROR) {
    return CAPU_ERROR;
  }

  return CAPU_OK;
}

inline status_t Socket::getLingerOption(bool_t& isLinger, int32_t& _linger) {
  if (mSocket == INVALID_SOCKET) {
    return CAPU_SOCKET_ESOCKET;
  }

  struct linger soLinger;
  int32_t len = sizeof (soLinger);

  if (getsockopt(mSocket, SOL_SOCKET, SO_LINGER, (char*)&soLinger, &len) == SOCKET_ERROR) {
    return CAPU_ERROR;
  }

  _linger = soLinger.l_linger;

  if (soLinger.l_onoff == 1) {
    isLinger = true;
  } else {
    isLinger = false;
  }

  return CAPU_OK;
}

inline status_t Socket::getNoDelay(bool_t& noDelay) {
  if (mSocket == INVALID_SOCKET) {
    return CAPU_SOCKET_ESOCKET;
  }

  int32_t opt = 0;
  socklen_t len = sizeof(opt);

  if (getsockopt(mSocket, IPPROTO_TCP, TCP_NODELAY, (char*)&opt, &len) == SOCKET_ERROR) {
    return CAPU_ERROR;
  }

  if (opt == 1) {
    noDelay = true;
  } else {
    noDelay = false;
  }

  return CAPU_OK;
}

inline status_t Socket::getKeepAlive(bool_t& keepAlive) {
  if (mSocket == INVALID_SOCKET) {
    return CAPU_SOCKET_ESOCKET;
  }

  int32_t opt = 0;
  int32_t len = sizeof (opt);

  if (getsockopt(mSocket, SOL_SOCKET, SO_KEEPALIVE, (char*)&opt, &len) == SOCKET_ERROR) {
    return CAPU_ERROR;
  }

  if (opt == 1) {
    keepAlive = true;
  } else {
    keepAlive = false;
  }

  return CAPU_OK;
}

inline status_t Socket::getTimeout(int32_t& timeout) {
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
