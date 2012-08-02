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

#include "transport/EtchTcpListener.h"

const EtchString EtchTcpListener::BACKLOG("TcpTransportFactory.socket");

EtchTcpListener::EtchTcpListener(EtchURL *url)
: mPort(url->getPort()) {
  EtchString str;
  if (url->getTerms().get(BACKLOG, &str) != ETCH_OK) {
    mBackLog = 0;
  } else {
    const char *c_str = str.c_str();
    mBackLog = atoi(c_str);
    if (mBackLog < 0) {
      mBackLog = 0;
    }
  }
  mSession = NULL;
  mIsStarted = false;
  mThread = NULL;
  mSocket = NULL;
}

EtchTcpListener::~EtchTcpListener() {
  if (mThread != NULL) {
    delete mThread;
  }
  if (mSocket != NULL) {
    mSocket->close();
    delete mSocket;
    mSocket = NULL;
  }
}

capu::bool_t EtchTcpListener::isStarted() {
  return mIsStarted;
}

status_t EtchTcpListener::close() {
  if (mSocket != NULL)
    return mSocket->close();
  return ETCH_ERROR;
}

status_t EtchTcpListener::openSocket(capu::bool_t reconnect) {
  // if a one time connection from a server socket listener, just
  // return the existing socket.
  mMutexConnection.lock();
  if (!reconnect && (mSocket != NULL)) {
    mMutexConnection.unlock();
    return ETCH_OK;
  }
  while (isStarted()) {
    // if reconnect is false and first is true, this is our
    // very first attempt to connect. otherwise, we are trying
    // to reconnect a broken link or establish a link where we
    // have already failed at least once.
    // try to open a socket.

    mSocket = new EtchServerSocket();
    if (mSocket->bind(mPort) == ETCH_OK) {
      if (mSocket->listen(mBackLog) == ETCH_OK) {
        mMutexConnection.unlock();
        return ETCH_OK;
      } else {
        mSocket->close();
        delete mSocket;
        mSocket = NULL;
      }
    }
    //LOGGER IS NEEDED
  }
  mMutexConnection.unlock();
  return ETCH_ERROR;
}

status_t EtchTcpListener::readSocket() {
  EtchSocket* s = NULL;
  while (isStarted()) {
    s = mSocket->accept();
    if (s == NULL)
      break;
    if (mSession != NULL) {
      mSession->sessionAccepted(s);
    } else {
      delete s;
      return ETCH_ERROR;
    }
  }
  return ETCH_OK;
}

status_t EtchTcpListener::transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {

  if (control->equals(&EtchTcpListener::START)) {
    if (mIsStarted)
      return ETCH_OK;
    mMutex.lock();
    mIsStarted = true;
    mMutex.unlock();
    mThread = new capu::Thread(this);
    mThread->start();
    return ETCH_OK;
  }

  if (control->equals(&EtchTcpListener::START_AND_WAIT_UP)) {
    if (mIsStarted)
      return ETCH_OK;
    mMutex.lock();
    mIsStarted = true;
    mMutex.unlock();
    mThread = new capu::Thread(this);
    mThread->start();
    waitUp(((EtchInt32*) value.get())->get());
    //TODO: Wait handling in one of the next releases
    return ETCH_OK;
  }

  if (control->equals(&EtchTcpListener::STOP)) {
    if (!mIsStarted)
      return ETCH_OK;
    mMutex.lock();
    mIsStarted = false;
    mMutex.unlock();
    close();
    return ETCH_OK;
  }

  if (control->equals(&EtchTcpListener::STOP_AND_WAIT_DOWN)) {
    if (!mIsStarted)
      return ETCH_OK;
    mMutex.lock();
    mIsStarted = false;
    mMutex.unlock();
    close();
    waitDown(((EtchInt32*) value.get())->get());
    //TODO: Wait handling in one of the next releases
    return ETCH_OK;
  }

  if (control->equals(&EtchTcpListener::RESET)) {
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

status_t EtchTcpListener::transportNotify(capu::SmartPointer<EtchObject> event) {
  return ETCH_OK;
}

status_t EtchTcpListener::transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result) {
  //Nothing to do
  return ETCH_EUNIMPL;
}

EtchSessionListener<EtchSocket>* EtchTcpListener::getSession() {
  return mSession;
}

void EtchTcpListener::setSession(EtchSessionListener<EtchSocket>* session) {
  mSession = session;
}

void EtchTcpListener::run() {

  capu::bool_t first = true;

  while (mIsStarted) {

    if (openSocket(!first) != ETCH_OK) {
      break;
    }
    fireUp();
    if (readSocket() != ETCH_OK) {
      close();
      break;
    }

    fireDown();
    close();
    first = false;
  }
  mIsStarted = false;
}

status_t EtchTcpListener::setupSocket() {
  return ETCH_EUNIMPL;
}

