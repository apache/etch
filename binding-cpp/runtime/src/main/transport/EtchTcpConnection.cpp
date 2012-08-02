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

#include "transport/EtchTcpConnection.h"

EtchTcpConnection::EtchTcpConnection(EtchSocket* socket, EtchURL* uri) : mOptions(uri) {
  if ((socket == NULL) && (uri != NULL)) {
    mHost = uri->getHost();
    mPort = uri->getPort();
  } else {
    mHost.set("");
    mPort = 0;
  }
  mThread = NULL;
  mIsStarted = false;
  mSession = NULL;
  mSocket = socket;
}

EtchTcpConnection::~EtchTcpConnection() {


  if (mThread != NULL) {
    mThread->join();
    delete mThread;
  }

  if (mSocket != NULL) {
    mSocket->close();
    delete mSocket;
    mSocket = NULL;
    mIsStarted = false;
  }
}

status_t EtchTcpConnection::send(capu::int8_t* buf, capu::uint32_t off, capu::uint32_t len) {
  if (mSocket != NULL)
    return mSocket->send((unsigned char *) &buf[off], len);
  return ETCH_ERROR;
}

status_t EtchTcpConnection::readSocket() {
  capu::SmartPointer<EtchFlexBuffer> buf = new EtchFlexBuffer(new capu::int8_t[8192], 8192);

  while (mIsStarted) {
    capu::int32_t n;
    status_t result = mSocket->receive((unsigned char *) buf->getBuffer(), buf->getSize(), n);
    if (result != ETCH_OK)
      return result;
    if (n <= 0)
      return ETCH_ERROR;

    buf->setLength(n);
    buf->setIndex(0);
    mSession->sessionData(NULL, buf);
  }
  return ETCH_OK;
}

status_t EtchTcpConnection::openSocket(capu::bool_t reconnect) {

  mMutexConnection.lock();
  // if a one time connection from a server socket listener, just
  // return the existing socket.
  if (!reconnect && (mSocket != NULL)) {
    mMutexConnection.unlock();
    return ETCH_OK;
  }
  //temporary socket in a listener
  if ((reconnect == false) && (mPort == 0) && (mHost.length() == 0)) {
    mMutexConnection.unlock();
    return ETCH_ERROR;
  }
  // if a reconnect but no retries allowed, then bail.
  if (reconnect && (mOptions.mReconnectDelay == 0)) {
    mMutexConnection.unlock();
    return ETCH_ERROR;
  }

  capu::bool_t first = true;
  while (mIsStarted) {
    // if reconnect is false and first is true, this is our
    // very first attempt to connect. otherwise, we are trying
    // to reconnect a broken link or establish a link where we
    // have already failed at least once.

    if (reconnect || !first) {
      if (mOptions.mReconnectDelay == 0) {
        mMutexConnection.unlock();
        return ETCH_ERROR;
      }
      capu::Thread::Sleep(mOptions.mReconnectDelay);

      if (!mIsStarted) {
        mMutexConnection.unlock();
        return ETCH_ERROR;
      }
    }
    // try to open a socket.
    mSocket = new EtchSocket();
    if (mSocket->connect((unsigned char *) mHost.c_str(), mPort) == ETCH_OK) {
      mMutexConnection.unlock();
      return ETCH_OK;
    } else {
      mSocket->close();
      delete mSocket;
      mSocket = NULL;
    }

    //LOGGER IS NEEDED
  }

  mMutexConnection.unlock();
  return ETCH_ERROR;
}

status_t EtchTcpConnection::transportData(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchFlexBuffer> buf) {
  return this->send(buf->getBuffer(), buf->getIndex(), buf->getAvailableBytes());
}

status_t EtchTcpConnection::transportNotify(capu::SmartPointer<EtchObject> event) {
  return ETCH_OK;
}

status_t EtchTcpConnection::transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result) {
  //Nothing to do
  return ETCH_EUNIMPL;
}

status_t EtchTcpConnection::transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {

  if (control->equals(&EtchTcpConnection::START)) {
    if (mIsStarted)
      return ETCH_OK;
    mMutex.lock();
    mIsStarted = true;
    mMutex.unlock();
    mThread = new capu::Thread(this);
    mThread->start();
    return ETCH_OK;
  }

  if (control->equals(&EtchTcpConnection::START_AND_WAIT_UP)) {
    if (mIsStarted)
      return ETCH_OK;
    mMutex.lock();
    mIsStarted = true;
    mMutex.unlock();
    mThread = new capu::Thread(this);
    mThread->start();
    capu::Thread::Sleep(((EtchInt32*) value.get())->get());
    //TODO: Wait handling in one of the next releases
    return ETCH_OK;
  }

  if (control->equals(&EtchTcpConnection::STOP)) {
    if (!mIsStarted)
      return ETCH_OK;
    mMutex.lock();
    mIsStarted = false;
    mMutex.unlock();
    close();
    return ETCH_OK;
  }

  if (control->equals(&EtchTcpConnection::STOP_AND_WAIT_DOWN)) {
    if (!mIsStarted)
      return ETCH_OK;
    mMutex.lock();
    mIsStarted = false;
    mMutex.unlock();
    close();
    capu::Thread::Sleep(((EtchInt32*) value.get())->get());
    //TODO: Wait handling in one of the next releases
    return ETCH_OK;
  }

  if (control->equals(&EtchTcpConnection::RESET)) {
    if (!mIsStarted)
      return ETCH_OK;
    mMutex.lock();
    mIsStarted = false;
    mMutex.unlock();
    close();
    return ETCH_OK;
  }
  return ETCH_ENOT_SUPPORTED;
}

EtchSessionData* EtchTcpConnection::getSession() {
  return mSession;
}

void EtchTcpConnection::setSession(EtchSessionData* session) {
  mSession = session;
}

status_t EtchTcpConnection::close() {
  if (mSocket != NULL)
    return mSocket->close();
  else
    return ETCH_ERROR;
}

capu::bool_t EtchTcpConnection::isStarted() {
  return mIsStarted;
}

void EtchTcpConnection::run() {

  capu::bool_t first = true;

  while (mIsStarted) {

    if (openSocket(!first) != ETCH_OK) {
      break;
    }
    status_t res = setupSocket();

    if (res != ETCH_OK) {
      close();
      break;
    }

    if (readSocket() != ETCH_OK) {
      close();
      break;
    }
    close();
    first = false;
  }
  mIsStarted = false;
}

status_t EtchTcpConnection::setupSocket() {
  if (mSocket->setBufferSize(mOptions.mBufferSize) != ETCH_OK)
    return ETCH_ERROR;
  if (mSocket->setKeepAlive((mOptions.mKeepAlive != 0)) != ETCH_OK)
    return ETCH_ERROR;
  if (mSocket->setLingerOption((mOptions.mLingerTime >= 0), mOptions.mLingerTime) != ETCH_OK)
    return ETCH_ERROR;
  if (mSocket->setNoDelay((mOptions.mNoDelay != 0)) != ETCH_OK)
    return ETCH_ERROR;
  return ETCH_OK;
}
