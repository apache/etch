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
#define _WINSOCKAPI_
// Need to link with Ws2_32.lib, Mswsock.lib, and Advapi32.lib
#pragma comment (lib, "Ws2_32.lib")
#pragma comment (lib, "Mswsock.lib")
#pragma comment (lib, "AdvApi32.lib")
#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#endif

#ifdef SERVER_SOCKET_INC_MEMBER
  SOCKET mServerSocket;
  WSADATA mWsaData;
  bool_t mIsBound;
#endif

#ifdef SERVER_SOCKET_INC_IMPL
inline ServerSocket::ServerSocket()
{
  mServerSocket = INVALID_SOCKET;
  mIsBound = false;
  //Initialize Winsock
  int32_t result = WSAStartup(MAKEWORD(2,2), &mWsaData);
  if (result == 0) {
    //create the socket which is used to connect the server
    mServerSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
    if (mServerSocket != INVALID_SOCKET) {
      int32_t optVal = 1;
      setsockopt(mServerSocket, SOL_SOCKET, SO_REUSEADDR, (char*)&optVal, sizeof(optVal));
    }
  } else {
    WSACleanup();
    mServerSocket = INVALID_SOCKET;
  }
}

inline ServerSocket::~ServerSocket()
{
  close();
}

inline Socket* ServerSocket::accept()
{
  SOCKET handle = ::accept(mServerSocket, NULL, NULL);
  if (handle == INVALID_SOCKET) {
    return NULL;
  }
  Socket *clientSocket = new Socket(handle);
  return clientSocket;
}

inline status_t ServerSocket::close()
{
  int32_t returnValue = CAPU_OK;
  if (mServerSocket == INVALID_SOCKET) {
    returnValue = CAPU_SOCKET_ESOCKET;
  } else {
    if (closesocket(mServerSocket) != 0) {
      int32_t result = WSAGetLastError();
      if (result != WSANOTINITIALISED){ //socket has already been closed
        returnValue = CAPU_SOCKET_ECLOSE;
      }
    }
  }
  mServerSocket = INVALID_SOCKET;
  mIsBound = false;
  WSACleanup();
  return returnValue;
}


inline status_t ServerSocket::bind(uint16_t port, const char *addr) {
  if (port == 0) {
    return CAPU_EINVAL;
  }

  //check if the address is valid

  if (mIsBound) {
    return CAPU_ERROR;
  }

  if (mServerSocket == INVALID_SOCKET)
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

  int32_t result = ::bind(mServerSocket, (sockaddr*) &socketAddr, sizeof(socketAddr));
  if (result == SOCKET_ERROR) {
    return CAPU_SOCKET_EBIND;
  }

  mIsBound = true;
  return CAPU_OK;
}

inline status_t ServerSocket::listen(uint8_t backlog) {
  if (!mIsBound) {
    return CAPU_EINVAL;
  }

  if (mServerSocket == -1) {
    return CAPU_SOCKET_ESOCKET;
  }

  int32_t result = ::listen(mServerSocket, backlog);

  if (result == SOCKET_ERROR) {
    return CAPU_ERROR;
  }

  return CAPU_OK;
}
#endif
