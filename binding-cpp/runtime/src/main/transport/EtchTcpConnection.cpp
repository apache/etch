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
#include "capu/os/Debug.h"

EtchTcpConnection::EtchTcpConnection(EtchRuntime* runtime, EtchSocket* socket, EtchURL* uri)
: mRuntime(runtime), mOptions(uri) {
  capu::Debug::Assert(mRuntime != NULL);

  if ((socket == NULL) && (uri != NULL)) {
    mHost = uri->getHost();
    mPort = uri->getPort();
  } else {
    mHost.set("");
    mPort = 0;
  }
  mThread = NULL;
  mIsStarted = false;
  mIsTerminated = false;
  mSession = NULL;
  mSocket = socket;
}

EtchTcpConnection::~EtchTcpConnection() {
  mIsStarted = false;
  close();
  if (mThread != NULL) {
    mThread->join();
    delete mThread;
  }

  if (mSocket != NULL) {
    delete mSocket;
    mSocket = NULL;
  }
}

status_t EtchTcpConnection::send(capu::int8_t* buf, capu::uint32_t off, capu::uint32_t len) {
  if (mSocket != NULL) {
    CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%d byte of data has been transmitted", len);
    return mSocket->send((capu::char_t*)&buf[off], len);
  }
  CAPU_LOG_WARN(mRuntime->getLogger(), "EtchTcpConnection", "%d byte of data has not been transmitted because there is no connection", len);
  return ETCH_ERROR;
}

status_t EtchTcpConnection::readSocket() {
  capu::SmartPointer<EtchFlexBuffer> buf = new EtchFlexBuffer(new capu::int8_t[ETCH_DEFAULT_SOCKET_INPUT_BUFFER_SIZE], ETCH_DEFAULT_SOCKET_INPUT_BUFFER_SIZE);

  while (mIsStarted) {
    capu::int32_t n;
    status_t result = mSocket->receive((capu::char_t*)buf->getBuffer(), buf->getSize(), n);
    if (result != ETCH_OK) {
      CAPU_LOG_ERROR(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Receive() failed with error code %d", mHost.c_str(), mPort, result);
      return result;
    }
    if (n <= 0) {
      CAPU_LOG_ERROR(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Connection closed for stack", mHost.c_str(), mPort);
      return ETCH_ERROR;
    }

    buf->setLength(n);
    buf->setIndex(0);
    CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => %d byte data has been received and passed to Packetizer", mHost.c_str(), mPort, n);
    mSession->sessionData(NULL, buf);
  }
  return ETCH_OK;
}

status_t EtchTcpConnection::openSocket(capu::bool_t reconnect) {

  mMutexConnection.lock();
  // if a one time connection from a socket listener, just
  // keep the existing socket.
  if (!reconnect && (mSocket != NULL)) {
    mMutexConnection.unlock();
    return ETCH_OK;
  }
  //temporary socket in a listener
  if (reconnect && (mPort == 0) && (mHost.length() == 0)) {
    mMutexConnection.unlock();
    return ETCH_ERROR;
  }
  // if a reconnect but retries not allowed, then fail.
  if (reconnect && (mOptions.getReconnectDelay() == 0)) {
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
      if (mOptions.getReconnectDelay() == 0) {
        mMutexConnection.unlock();
        return ETCH_ERROR;
      }
      capu::Thread::Sleep(mOptions.getReconnectDelay());

      if (!mIsStarted) {
        mMutexConnection.unlock();
        return ETCH_ERROR;
      }
    }
    // try to open a socket.
    if (mSocket == NULL) {
      mSocket = new EtchSocket();
    }
    if (mSocket->connect((capu::char_t*) mHost.c_str(), mPort) == ETCH_OK) {
      mMutexConnection.unlock();
      CAPU_LOG_TRACE(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Connection established", mHost.c_str(), mPort);
      return ETCH_OK;
    } else {
      mSocket->close();
      delete mSocket;
      mSocket = NULL;
      first = false;
    }
    CAPU_LOG_WARN(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Connection could not be established for stack %d", mHost.c_str(), mPort);
  }

  mMutexConnection.unlock();
  return ETCH_SOCKET_ECONNECT;
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

  if (control->equals(&EtchTcpConnection::START())) {
    mMutex.lock();
    if (mIsStarted) {
      mMutex.unlock();
      CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Start command received and EtchTcpConnection Receiving Thread", mHost.c_str(), mPort);
      return ETCH_OK;
    }
    mIsStarted = true;
    mMutex.unlock();
    mThread = new capu::Thread();
    mThread->start(*this);
    CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Start command received and EtchTcpConnection Receiving Thread has started", mHost.c_str(), mPort);
    return ETCH_OK;
  }

  if (control->equals(&EtchTcpConnection::START_AND_WAIT_UP())) {
    mMutex.lock();
    if (mIsStarted) {
      mMutex.unlock();
      CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Start and wait command received, but already started", mHost.c_str(), mPort);
      return ETCH_OK;
    }
    mIsStarted = true;
    mMutex.unlock();
    mThread = new capu::Thread();
    mThread->start(*this);
    CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Start and wait command received and EtchTcpConnection Receiving Thread has started", mHost.c_str(), mPort);
    return waitUp(((EtchInt32*) value.get())->get());
  }

  if (control->equals(&EtchTcpConnection::STOP())) {
    mMutex.lock();
    if (!mIsStarted) {
      mMutex.unlock();
      CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Stop command received, but already stopped", mHost.c_str(), mPort);

      return ETCH_OK;
    }
    CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Stop command received and Stop flag for receiving thread is set", mHost.c_str(), mPort);
    mIsStarted = false;

    mMutex.unlock();
    close();
    return ETCH_OK;
  }

  if (control->equals(&EtchTcpConnection::STOP_AND_WAIT_DOWN())) {
    mMutex.lock();
    if (!mIsStarted) {
      mMutex.unlock();
      CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Stop and wait command received, but already stopped", mHost.c_str(), mPort);
      return ETCH_OK;
    }
    mIsStarted = false;
    CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Stop and wait command received and Stop flag for receiving thread is set", mHost.c_str(), mPort);
    mMutex.unlock();

    close();
    return waitDown(((EtchInt32*) value.get())->get());
  }

  if (control->equals(&EtchTcpConnection::RESET())) {
    mMutex.lock();
    if (!mIsStarted) {
      mMutex.unlock();
      CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Reset command received, but no connection established yet", mHost.c_str(), mPort);
      return ETCH_OK;
    }
    mIsStarted = false;
    mMutex.unlock();
    close();
    CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Reset command received and the receiving thread has been restarted", mHost.c_str(), mPort);
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
  if (mSocket != NULL) {
    CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Socket has been closed", mHost.c_str(), mPort);
    return mSocket->close();
  } else {
    return ETCH_ERROR;
  }
}

capu::bool_t EtchTcpConnection::isStarted() {
  return mIsStarted;
}

capu::bool_t EtchTcpConnection::isTerminated() {
  return mIsTerminated;
}

void EtchTcpConnection::run() {

  status_t status;
  capu::bool_t first = true;

  while (mIsStarted) {

    status = openSocket(!first);
    if (status != ETCH_OK) {
      CAPU_LOG_ERROR(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Socket has not been successfully opened", mHost.c_str(), mPort);
      break;
    }

    status = setupSocket();
    if (status != ETCH_OK) {
      CAPU_LOG_ERROR(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Socket has not been successfully opened", mHost.c_str(), mPort);
      close();
      break;
    }
    CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Socket has been opened and connection has been successfully established and start reading", mHost.c_str(), mPort);
    fireUp();
    CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => FireUp was send to the Stack", mHost.c_str(), mPort);
    status = readSocket();
    CAPU_LOG_TRACE(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Connection closing", mHost.c_str(), mPort);
    fireDown();
    CAPU_LOG_DEBUG(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => FireDown was send to the Stack", mHost.c_str(), mPort);
    close();
    first = false;
  }
   mIsTerminated = true;
}

status_t EtchTcpConnection::setupSocket() {
  if (mOptions.getBufferSize() != 0) {
    if (mSocket->setBufferSize(mOptions.getBufferSize()) != ETCH_OK) {
      CAPU_LOG_ERROR(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => setupSocket: Error while setting buffer size", mHost.c_str(), mPort);
      return ETCH_ERROR;
    }
  }
  if (mSocket->setKeepAlive((mOptions.getKeepAlive() != 0)) != ETCH_OK) {
    CAPU_LOG_ERROR(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => setupSocket: Error while setting keep alive", mHost.c_str(), mPort);
    return ETCH_ERROR;
  }
  if (mSocket->setLingerOption((mOptions.getLingerTime() >= 0), mOptions.getLingerTime()) != ETCH_OK) {
    CAPU_LOG_ERROR(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => setupSocket: Error while setting linger time", mHost.c_str(), mPort);
    return ETCH_ERROR;
  }
  if (mSocket->setNoDelay((mOptions.getNoDelay() != 0)) != ETCH_OK) {
    CAPU_LOG_ERROR(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => setupSocket: Error while setting delay", mHost.c_str(), mPort);
    return ETCH_ERROR;
  }
  CAPU_LOG_TRACE(mRuntime->getLogger(), "EtchTcpConnection", "%s : %d => Settings for socket has been successfully configured", mHost.c_str(), mPort);
  return ETCH_OK;
}
