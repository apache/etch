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

/* 
 * etch_log.h 
 * logger for the c binding
 */
#ifndef _ETCH_LOG_H_
#define _ETCH_LOG_H_

#include "etch.h"
#include "etch_config.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * etch_log macros
 */
#define ETCH_LOG(category, level, fmt, ...) etch_log_log(NULL, category, level, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define ETCH_LOGW(category, level, fmt, ...) etch_log_logw(NULL, category, level, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

/**
 * etch_log_t
 */
typedef struct etch_log_t etch_log_t;

/**
 * etch_log_levels
 */
typedef enum etch_log_level
{
    /**
     * extended debug for low-level
     * or voluminous detail
     */
    ETCH_LOG_XDEBUG = 0,

    /**
     * traces of connection open
     * close send receive etc
     */
    ETCH_LOG_DEBUG,

    /**
     * display server and client
     * open and close etc
     */
    ETCH_LOG_INFO,

    /**
     * messages that should be
     * noted regardless
     */
    ETCH_LOG_WARN,

    /**
     * failures
     */
    ETCH_LOG_ERROR

} etch_log_level;

/**
 * etch_log_message_t
 */
typedef struct etch_log_message_t
{
    const char*     category;
    time_t          timestamp;
    etch_log_level  level;
    uint32          threadid;
    const char*     file;
    uint32          line;
    char*           message;
} etch_log_message_t;

/*
 * log appender definitions.
 */
typedef etch_status_t etch_log_appender_create(void** appender, etch_config_t* config);
typedef etch_status_t etch_log_appender_open(void* appender);
typedef etch_status_t etch_log_appender_log(void* appender, etch_log_message_t* message);
typedef etch_status_t etch_log_appender_close(void* appender);
typedef etch_status_t etch_log_appender_destroy(void* appender);

typedef struct etch_log_appender_desc {
    etch_log_appender_create*   create;
    etch_log_appender_open*     open;
    etch_log_appender_log*      log;
    etch_log_appender_close*    close;
    etch_log_appender_destroy*  destroy;
} etch_log_appender_desc;

typedef struct etch_log_appender {
    struct etch_log_appender_desc* desc;
    void*                          data;
} etch_log_appender;

/**
 * register a new log appender.
 * @param name of the appender
 * @param desc of log appender.
 * @return status.
 */
etch_status_t etch_log_register_appender(const char* name, etch_log_appender_desc* desc);

/**
 * create a new logger.
 * @param logger that will be created.
 * @return status.
 */
etch_status_t etch_log_create(etch_log_t** logger, etch_config_t* config);

/**
 * etch_log_log()
 * write a log message to console/file/system log depending on configuration.
 * @param logger logger instance
 * @param category the category name for this message.
 * @param level logging level of the message.
 * @param file filename where the log function is called.
 * @param line line number where the log function is called.
 * @param fmt the format string ala printf.
 * @...   a variable length argument list ala printf.
 * @return void
 */
etch_status_t etch_log_log(etch_log_t* logger, const char* category, etch_log_level level, const char* file, int line, const char* fmt, ...);

/**
 * etch_log_logw()
 * write a log message to console/file/system log depending on configuration.
 * @param logger logger instance
 * @param category the category name for this message.
 * @param level logging level of the message.
 * @param file filename where the log function is called.
 * @param line line number where the log function is called.
 * @param fmt the format wide string ala wprintf.
 * @...   a variable length argument list ala wprintf.
 * @return void
 */
etch_status_t etch_log_logw(etch_log_t* logger, const char* category, etch_log_level level, const char* file, int line, const wchar_t* fmt, ...);

/**
 * destroy the logger instance.
 * @config handle.
 */
etch_status_t etch_log_destroy(etch_log_t* logger);

#ifdef __cplusplus
}
#endif

#endif /* ETCH_LOG_H */
