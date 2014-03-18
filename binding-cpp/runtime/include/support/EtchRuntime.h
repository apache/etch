/* $Id$
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
#ifndef __ETCHRUNTIME_H__
#define __ETCHRUNTIME_H__

#include "capu/os/Mutex.h"
#include "capu/container/List.h"
#include "common/EtchError.h"
#include "util/EtchLogger.h"

class EtchRuntime;

class EtchRuntimeListener {
public:

  virtual ~EtchRuntimeListener() {}

  /**
   * Called if the runtime changed its state
   */
  virtual status_t onRuntimeChanged(EtchRuntime* runtime) = 0;
};

/**
 * EtchRuntime class
 */
class EtchRuntime {
public:

  /**
   * Create a new instance of the EtchRuntime class
   * It uses the default Console Log Appender for logging with log level WARN.
   */
  EtchRuntime();

  /**
   * Create a new instance of the EtchRuntime class
   * It uses the default Console Log Appender for logging with the given log level.
   * @param logLevel the log level
   */
  EtchRuntime(EtchLogLevel logLevel);

  /**
   * Create a new instance of the EtchRuntime class
   * @param logAppender the log appender
   * @param logLevel the log level
   */
  EtchRuntime(IEtchLogAppender& logAppender, EtchLogLevel logLevel);

  /**
   * Destructor
   */
  virtual ~EtchRuntime();

  /**
   * Unique runtime id
   * @return unique runtime id
   */
  capu::uint64_t getId();

  /**
   * Current runtime state
   * @return true if runtime was closed
   */
  capu::bool_t isClosed();

  /**
   * Register a runtime listener
   */
  status_t registerListener(EtchRuntimeListener* listener);

  /**
   * Unregister the runtime listener
   */
  status_t unregisterListener(EtchRuntimeListener* listener);

  /**
   * Starts the runtime
   * @return ETCH_OK if runtime was successfully started
   */
  status_t start();

  /**
   * Shutdown the runtime
   * @return ETCH_OK if runtime successfully ended
   */
  status_t shutdown();

  /**
   * Gets Logger
   * @return logger if exists
   */
  EtchLogger& getLogger();

private:
  capu::bool_t mIsClosed;
  EtchLogger mLogger;
  EtchConsoleLogAppender mDefaultLogAppender;
  static capu::uint64_t sId;
  capu::uint64_t mId;
  capu::Mutex mMutex;
  capu::List<EtchRuntimeListener*> mListeners;


  /**
   * Assigns an unique id to the runtime
   */
  void generateRuntimeId();

  /**
   * Return the next id
   */
  static capu::uint64_t getNextId();

  /**
   * Fire onRuntimeChanged callback
   */
  status_t fireOnRuntimeChanged();

};
#endif /* ETCHRUNTIME_H */
