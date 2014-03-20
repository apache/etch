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
#include "support/EtchRuntime.h"



EtchTcpListener::ConnectionChecker::ConnectionChecker(EtchTcpListener* listener) : mListener(listener){ }

EtchTcpListener::ConnectionChecker::~ConnectionChecker() {}

void EtchTcpListener::ConnectionChecker::run() {
  status_t result;
  while (mListener->mIsStarted) {
    if (mListener->mSession != NULL) {
      result = mListener->mSession->sessionNotify(new EtchString(EtchTcpListener::CONNECTION_CHECK()));
      if (result != ETCH_OK) {
        //TODO handle error
      }
      capu::Thread::Sleep(1000);
    }
  }
}

const EtchString& EtchTcpListener::BACKLOG() {
  static const EtchString name("TcpTransportFactory.socket");
  return name;
}

const EtchString& EtchTcpListener::CONNECTION_CHECK() {
  static const EtchString name("connection.check");
  return name;
}

EtchTcpListener::EtchTcpListener(EtchRuntime* runtime, EtchURL *url)
: mRuntime(runtime), mPort(url->getPort()) {
  EtchString str;
  if (url->getTerms().get(BACKLOG(), &str) != ETCH_OK) {
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
  mIsTerminated = false;
  mThread = NULL;
  mSocket = NULL;
  mConnectionCheckerThread = NULL;
  mConnectionChecker = NULL;
}

EtchTcpListener::~EtchTcpListener() {
  if (mThread != NULL) {
    mThread->join();
    delete mThread;
  }
  if (mConnectionCheckerThread != NULL) {
    mConnectionCheckerThread->join();
    delete mConnectionCheckerThread;
  }
  if (mSocket != NULL) {
    mSocket->close();
    delete mSocket;
    mSocket = NULL;
  }

  delete mConnectionChecker;

}

capu::bool_t EtchTcpListener::isStarted() {
  return mIsStarted;
}

capu::bool_t EtchTcpListener::isTerminated() {
  return mIsTerminated;
}

status_t EtchTcpListener::close() {
  if (mSocket != NULL) {
    return mSocket->close();
  }
  return ETCH_ERROR;
}

status_t EtchTcpListener::openSocket(capu::bool_t reconnect) {
  // if a one time connection from a server socket listener, just
  // return the existing socket.
  mMutexConnection.lock();
  if (reconnect && (mSocket != NULL)) {
    mMutexConnection.unlock();
    return ETCH_OK;
  }
  while (mIsStarted) {
    // if reconnect is false and first is true, this is our
    // very first attempt to connect. otherwise, we are trying
    // to reconnect a broken link or establish a link where we
    // have already failed at least once.
    // try to open a socket.
    if (mSocket == NULL) {
      mSocket = new EtchServerSocket();
    }
    if (mSocket->bind(mPort) == ETCH_OK) {
      if (mSocket->listen(mBackLog) == ETCH_OK) {
        ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Listener socket opened successfully on port " << mPort);
        mMutexConnection.unlock();
        return ETCH_OK;
      } else {
        mSocket->close();
        delete mSocket;
        mSocket = NULL;
        ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Listening on specified port " <<  mPort << " has failed");
      }
    } else {
      delete mSocket;
      mSocket = NULL;
      ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Binding to specified port " << mPort << " has failed");
    }

  }
  mMutexConnection.unlock();
  return ETCH_ERROR;
}

status_t EtchTcpListener::readSocket() {
  EtchSocket* s = NULL;
  while (mIsStarted) {
    s = mSocket->accept();
    if (s == NULL)
      break;
    if (mSession != NULL) {
      capu::char_t* remoteAddress = NULL;
      s->getRemoteAddress(&remoteAddress);
      ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "A new connection from " << remoteAddress << " has been accepted");
      delete[] remoteAddress;
      mSession->sessionAccepted(s);
    } else {
      delete s;
      return ETCH_ERROR;
    }
  }
  return ETCH_OK;
}

status_t EtchTcpListener::transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {

  if (control->equals(&EtchTcpListener::START())) {
    if (mIsStarted) {
      ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Ignoring START command received as listener is already started - port is " << mPort);
      return ETCH_OK;
    }
    mMutex.lock();
    mIsStarted = true;
    mMutex.unlock();
    mThread = new capu::Thread();
    mThread->start(*this);
    mConnectionChecker = new ConnectionChecker(this);
    mConnectionCheckerThread = new capu::Thread();
    mConnectionCheckerThread->start(*mConnectionChecker);
    ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Start command received and EtchTcpListener starts listening on port " << mPort);
    return ETCH_OK;
  }

  if (control->equals(&EtchTcpListener::START_AND_WAIT_UP())) {
    if (mIsStarted) {
      ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Ignoring START_AND_WAIT_UP command received as listener is already started - port is " << mPort);
      return ETCH_OK;
    }
    mMutex.lock();
    mIsStarted = true;
    mMutex.unlock();
    mThread = new capu::Thread();
    mThread->start(*this);
    mConnectionChecker = new ConnectionChecker(this);
    mConnectionCheckerThread = new capu::Thread();
    mConnectionCheckerThread->start(*mConnectionChecker);
    ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Start and wait up command received and EtchTcpListener starts listening on port " << mPort);
    return waitUp(((EtchInt32*) value.get())->get());
  }

  if (control->equals(&EtchTcpListener::STOP())) {
    if (!mIsStarted) {
      ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Ignoring STOP command as listener is already stopped");
      return ETCH_OK;
    }
    mMutex.lock();
    ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Stop command received and EtchTcpListener sets the stop flag");
    mIsStarted = false;
    mMutex.unlock();
    close();
    return ETCH_OK;
  }

  if (control->equals(&EtchTcpListener::STOP_AND_WAIT_DOWN())) {
    if (!mIsStarted) {
     ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Stop and wait command received and EtchTcpListener, but already stopped");
    }
    mMutex.lock();
    mIsStarted = false;
    mMutex.unlock();
    close();
    ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Stop and wait command received and EtchTcpListener sets the stop flag");
    status_t ret = waitDown(((EtchInt32*) value.get())->get());
    return ret;
  }

  if (control->equals(&EtchTcpListener::RESET())) {
    if (!mIsStarted) {
      ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Reset command received - but listener is not started yet.");
      return ETCH_OK;
    }
    mMutex.lock();
    mIsStarted = false;
    mMutex.unlock();
    close();
    mThread->join();
    mIsStarted = true;
    mThread = new capu::Thread();
    mThread->start(*this);
    ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Reset command received and EtchTcpListener has been restarted the stop flag");
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

    if (openSocket(first) != ETCH_OK) {
      ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Socket cannot be opened");
      break;
    }
    ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "Socket has been created");
    fireUp();
    ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "FireUp was send to the Stack");
    if (readSocket() != ETCH_OK) {
      close();
      break;
    }
    fireDown();
    ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getTransportContext(), "FireDown was send to the Stack");
    close();
    first = false;
  }
  mIsStarted = false;
  mIsTerminated = true;
}

status_t EtchTcpListener::setupSocket() {
  return ETCH_EUNIMPL;
}

