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

#ifndef __LOGGER_H__
#define __LOGGER_H__

#include "capu/Error.h"
#include "capu/Config.h"
#include "capu/os/StringUtils.h"
#include <stdarg.h>

#if CAPU_LOGGING_ENABLED
#define CAPU_LOG(logger, level, tag, format, ...) if(logger != NULL) logger->log(level, tag, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define CAPU_LOG_TRACE(logger, tag, format, ...) if(logger != NULL) logger->log(capu::CLL_TRACE, tag, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define CAPU_LOG_DEBUG(logger, tag, format, ...) if(logger != NULL) logger->log(capu::CLL_DEBUG, tag, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define CAPU_LOG_INFO(logger, tag, format, ...) if(logger != NULL) logger->log(capu::CLL_INFO, tag, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define CAPU_LOG_WARN(logger, tag, format, ...) if(logger != NULL) logger->log(capu::CLL_WARN, tag, __FILE__, __LINE__, format, ##__VA_ARGS__)
#define CAPU_LOG_ERROR(logger, tag, format, ...) if(logger != NULL) logger->log(capu::CLL_ERROR, tag, __FILE__, __LINE__, format, ##__VA_ARGS__)
#else
#define CAPU_LOG(logger, level, tag, format, ...)
#define CAPU_LOG_TRACE(logger, tag, format, ...)
#define CAPU_LOG_DEBUG(logger, tag, format, ...)
#define CAPU_LOG_INFO(logger, tag, format, ...)
#define CAPU_LOG_WARN(logger, tag, format, ...)
#define CAPU_LOG_ERROR(logger, tag, format, ...)
#endif

namespace capu {

  class Appender;

  /**
   * Logger levels
   */
  enum LoggerLevel {
    CLL_INVALID,
    CLL_TRACE,
    CLL_DEBUG,
    CLL_INFO,
    CLL_WARN,
    CLL_ERROR
  };


  class LoggerMessage {
  public:
    /**
     * creats a new LoggerMessage instance
     */
    LoggerMessage();

    // id
    void setId(int32_t id);
    int32_t getId();

    // timestamp
    void setTimestamp(int32_t timestamp);
    int32_t getTimestamp();

    // threadId
    void setThreadId(const char *threadId);
    const char* getThreadId();

    // level
    void setLevel(LoggerLevel level);
    LoggerLevel getLevel();

    // tag
    void setTag(const char *tag);
    const char* getTag();

    // filename
    void setFile(const char *file);
    const char* getFile();

    // line
    void setLine(int32_t line);
    int32_t getLine();

    // message
    void setMessage(const char* message);
    const char* getMessage();

    /**
     * cleanup current message
     */
    ~LoggerMessage();

  private:
    int32_t mId;
    int32_t mTimestamp;
    char *mThreadId;
    LoggerLevel mLevel;
    char *mTag;
    char *mFile;
    int32_t mLine;
    char *mMessage;
  };

  // id
  inline void LoggerMessage::setId(int32_t id) {
    mId = id;
  }
  inline int32_t LoggerMessage::getId() {
    return mId;
  }

  // timestamp
  inline void LoggerMessage::setTimestamp(int32_t timestamp) {
    mTimestamp = timestamp;
  }
  inline int32_t LoggerMessage::getTimestamp() {
    return mTimestamp;
  }

  // threadId
  inline void LoggerMessage::setThreadId(const char *threadId) {
    if(threadId != NULL) {
      if(mThreadId != NULL) {
        delete mThreadId;
        mThreadId = NULL;
      }
      uint_t length = StringUtils::Strlen(threadId);
      mThreadId = new char[length + 1];
      StringUtils::Strncpy(mThreadId, length + 1, threadId);
    }
  }
  inline const char* LoggerMessage::getThreadId() {
    return mThreadId;
  }

  // level
  inline void LoggerMessage::setLevel(LoggerLevel level) {
    mLevel = level;
  }
  inline LoggerLevel LoggerMessage::getLevel() {
    return mLevel;
  }

  // tag
  inline void LoggerMessage::setTag(const char *tag) {
    if(tag != NULL) {
      if(mTag != NULL) {
        delete mTag;
        mTag = NULL;
      }
      uint_t length = StringUtils::Strlen(tag);
      mTag = new char[length + 1];
      StringUtils::Strncpy(mTag, length + 1, tag);
    }
  }
  inline const char* LoggerMessage::getTag() {
    return mTag;
  }

  // filename
  inline void LoggerMessage::setFile(const char *file) {
    if(file != NULL) {
      if(mFile != NULL) {
        delete mFile;
        mFile = NULL;
      }
      uint_t length = StringUtils::Strlen(file);
      mFile = new char[length + 1];
      StringUtils::Strncpy(mFile, length + 1, file);
    }
  }
  inline const char* LoggerMessage::getFile() {
    return mFile;
  }

  // line
  inline void LoggerMessage::setLine(int32_t line) {
    mLine = line;
  }
  inline int32_t LoggerMessage::getLine() {
    return mLine;
  }

  // message
  inline void LoggerMessage::setMessage(const char* message) {
    if(message != NULL) {
      if(mMessage != NULL) {
        delete mMessage;
        mMessage = NULL;
      }
      uint_t length = StringUtils::Strlen(message);
      mMessage = new char[length + 1];
      StringUtils::Strncpy(mMessage, length + 1, message);
    }
  }
  inline const char* LoggerMessage::getMessage() {
    return mMessage;
  }

  class Logger {
  public:

    /**
     * creats a new Logger
     * @param id of the logger
     */
    Logger(int32_t id = 0);

    /**
     * destroy logger instance
     */
    ~Logger();

    /**
     * sets an appender for the logger
     */
    status_t setAppender(Appender *appender);

    /**
     * removes the appender identified by name
     */
    status_t removeAppender(Appender *appender);

    /**
     * opens the logger
     */
    status_t open();

    /**
     * print trace message to the logger.
     * @param tag that will be logged
     * @param message that will be logged
     * @param file name of the log call
     * @param line number of the log call
     * @param msgFormat of the log message
     */
    status_t trace(const char *tag, const char *file, int32_t line,const char *msgFormat, ...);

    /**
     * print debug message to the logger.
     * @param tag that will be logged
     * @param message that will be logged
     * @param file name of the log call
     * @param line number of the log call
     * @param msgFormat of the log message
     */
    status_t debug(const char *tag, const char *file, int32_t line,const char *msgFormat, ...);

    /**
     * print info message to the logger.
     * @param tag that will be logged
     * @param message that will be logged
     * @param file name of the log call
     * @param line number of the log call
     * @param msgFormat of the log message
     */
    status_t info(const char *tag, const char *file, int32_t line,const char *msgFormat, ...);

    /**
     * print warn message to the logger.
     * @param tag that will be logged
     * @param message that will be logged
     * @param file name of the log call
     * @param line number of the log call
     * @param msgFormat of the log message
     */
    status_t warn(const char *tag, const char *file, int32_t line,const char *msgFormat, ...);

    /**
     * print error message to the logger.
     * @param tag that will be logged
     * @param message that will be logged
     * @param file name of the log call
     * @param line number of the log call
     * @param msgFormat of the log message
     */
    status_t error(const char *tag, const char *file, int32_t line,const char *msgFormat, ...);

    /**
     * print log message with given level to the logger.
     * @param level of the lov message
     * @param tag that will be logged
     * @param message that will be logged
     * @param file name of the log call
     * @param line number of the log call
     * @param msgFormat of the log message
     */
    status_t log(LoggerLevel level, const char *tag, const char *file, int32_t line,const char *msgFormat, ...);

    /**
     * close the logger
     */
    status_t close();

  private:
    /**
     * print log message with given level to the logger.
     * @param level of the lov message
     * @param tag that will be logged
     * @param message that will be logged
     * @param file name of the log call
     * @param line number of the log call
     * @param msgFormat of the log message
     * @param args variable argument list
     */
    status_t vlog(LoggerLevel level, const char *tag, const char *file, int32_t line,const char *msgFormat, va_list args);

  private:
    int32_t mId;
    Appender* mAppenders[LOGGER_APPENDER_MAX];
    bool_t mOpen;
  };

  inline status_t Logger::trace(const char *tag, const char *file, int32_t line, const char *msgFormat, ...) {
    va_list args;
    va_start(args, msgFormat);
    status_t status = vlog(CLL_TRACE, tag, file, line, msgFormat, args);
    va_end(args);
    return status;
  }

  inline status_t Logger::debug(const char *tag, const char *file, int32_t line,const char *msgFormat, ...) {
    va_list args;
    va_start(args, msgFormat);
    status_t status = vlog(CLL_DEBUG, tag, file, line, msgFormat, args);
    va_end(args);
    return status;
  }

  inline status_t Logger::info(const char *tag, const char *file, int32_t line,const char *msgFormat, ...) {
    va_list args;
    va_start(args, msgFormat);
    status_t status = vlog(CLL_INFO, tag, file, line, msgFormat, args);
    va_end(args);
    return status;
  }

  inline status_t Logger::warn(const char *tag, const char *file, int32_t line,const char *msgFormat, ...) {
    va_list args;
    va_start(args, msgFormat);
    status_t status = vlog(CLL_WARN, tag, file, line, msgFormat, args);
    return status;
  }

  inline status_t Logger::error(const char *tag, const char *file, int32_t line,const char *msgFormat, ...) {
    va_list args;
    va_start(args, msgFormat);
    status_t status = vlog(CLL_ERROR, tag, file, line, msgFormat, args);
    va_end(args);
    return status;
  }

  inline status_t Logger::log(LoggerLevel level, const char *tag, const char *file, int32_t line,const char *msgFormat, ...) {
    va_list args;
    va_start(args, msgFormat);
    status_t status = vlog(level, tag, file, line, msgFormat, args);
    va_end(args);
    return status;
  }

}

#endif /* __LOGGER_H__ */
