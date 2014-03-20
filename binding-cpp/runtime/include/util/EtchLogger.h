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

#ifndef __ETCHLOGGER_H__
#define __ETCHLOGGER_H__

#include "capu/util/Logger.h"
#include "capu/util/ConsoleLogAppender.h"
#include "capu/util/LogContext.h"
#include "capu/util/ILogAppender.h"

typedef capu::ILogAppender IEtchLogAppender;
typedef capu::ELogLevel EtchLogLevel;
typedef capu::ConsoleLogAppender EtchConsoleLogAppender;
typedef capu::LogContext EtchLogContext;

#define ETCH_LOG_TRACE(logger, context, message) \
  LOG_TRACE_EXT(logger, (context), message);

#define ETCH_LOG_DEBUG(logger, context, message) \
  LOG_DEBUG_EXT(logger, (context), message);

#define ETCH_LOG_INFO(logger, context, message) \
  LOG_INFO_EXT(logger, (context), message);

#define ETCH_LOG_WARN(logger, context, message) \
  LOG_WARN_EXT(logger, (context), message);

#define ETCH_LOG_ERROR(logger, context, message) \
  LOG_ERROR_EXT(logger, (context), message);

class EtchLogger: public capu::Logger {
public:
  EtchLogger(IEtchLogAppender& logAppender) : capu::Logger(logAppender)
    , mSerializerContext(createContext("etch.runtime.messagizer.serializer"))
    , mDeliveryServiceContext(createContext("etch.runtime.deliveryservice"))
    , mTransportContext(createContext("etch.runtime.transport"))
    , mPacketizerContext(createContext("etch.runtime.packetizer"))
    , mMessagizerContext(createContext("etch.runtime.messagizer"))
    , mValidatorContext(createContext("etch.runtime.messagizer.validator"))
    , mMailboxContext(createContext("etch.runtime.mailbox"))
    , mRuntimeContext(createContext("etch.runtime")) {
  }

  virtual ~EtchLogger() {

  }
  EtchLogContext& getSerializerContext() {
    return mSerializerContext;
  }
  EtchLogContext& getDeliveryServiceContext() {
    return mDeliveryServiceContext;
  }
  EtchLogContext& getTransportContext() {
    return mTransportContext;
  }
  EtchLogContext& getPacketizerContext() {
    return mPacketizerContext;
  }
  EtchLogContext& getMessagizerContext() {
    return mMessagizerContext;
  }
  EtchLogContext& getValidatorContext() {
    return mValidatorContext;
  }
  EtchLogContext& getMailboxContext() {
    return mMailboxContext;
  }
  EtchLogContext& getRuntimeContext() {
    return mRuntimeContext;
  }
private:
  EtchLogContext& mSerializerContext;
  EtchLogContext& mDeliveryServiceContext;
  EtchLogContext& mTransportContext;
  EtchLogContext& mPacketizerContext;
  EtchLogContext& mMessagizerContext;
  EtchLogContext& mValidatorContext;
  EtchLogContext& mMailboxContext;
  EtchLogContext& mRuntimeContext;

};

#endif /* ETCHLOGGER_H */
