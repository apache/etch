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
 * etch_log.c
 * etch c binding logger
 */

#include "etch_log.h"
#include "etch_runtime.h"
#include "etch_mem.h"
#include "etch_general.h"
#include "etch_thread.h"

#define ETCH_LOG_MAX_APPENDER 5

static const char* etch_log_level_str[] = {
    "XDEBUG",
    "DEBUG",
    "INFO",
    "WARN",
    "ERROR"
};

//
// etch console appender
//
etch_status_t etch_log_appender_console_create(void** appender, etch_config_t* config)
{
    *appender = NULL;
    return ETCH_SUCCESS;
}

etch_status_t etch_log_appender_console_open(void* appender)
{
    return ETCH_SUCCESS;
}

etch_status_t etch_log_appender_console_log(void* appender, etch_log_message_t* logmsg)
{
    struct tm* ts = NULL;
   
    if(logmsg == NULL) {
        return ETCH_EINVAL;
    }
    // log message
    // 23-09-2009 02:52:01 [012345] XDEBUG etch_thread - Located nearest gas station - etch_thread.h, 2223
    ts = localtime(&logmsg->timestamp);
    printf("\r%02d-%02d-%04d %02d:%02d:%02d [%06d] %-6s %-25.25s - %s - %s %d\n", 
        ts->tm_mday,
        ts->tm_mon + 1,
        ts->tm_year +1900,
        ts->tm_hour,
        ts->tm_min,
        ts->tm_sec,
        logmsg->threadid,
        etch_log_level_str[logmsg->level],
        logmsg->category,
        logmsg->message,
        logmsg->file,
        logmsg->line
        );
    return ETCH_SUCCESS;
}

etch_status_t etch_log_appender_console_close(void* appender)
{
    return ETCH_SUCCESS;
}

etch_status_t etch_log_appender_console_destroy(void* appender)
{
    return ETCH_SUCCESS;
}

static struct etch_log_appender_desc etch_log_appender_console_desc = {
    etch_log_appender_console_create,
    etch_log_appender_console_open,
    etch_log_appender_console_log,
    etch_log_appender_console_close,
    etch_log_appender_console_destroy
};

//
// etch file appender
//

struct etch_log_appender_file_t
{
    FILE* file;
};

etch_status_t etch_log_appender_file_create(void** appender, etch_config_t* config)
{
    etch_status_t status = ETCH_SUCCESS;
    // add custome data here
    *appender = etch_malloc(sizeof(struct etch_log_appender_file_t), 0);
    ETCH_ASSERT(*appender != NULL);
    return status;
}

etch_status_t etch_log_appender_file_open(void* appender)
{
    // check if file exists
    // save file inside archiv
    // open logfile
    return ETCH_SUCCESS;
}

etch_status_t etch_log_appender_file_log(void* appender, etch_log_message_t* message)
{
    return ETCH_SUCCESS;
}

etch_status_t etch_log_appender_file_close(void* appender)
{
    return ETCH_SUCCESS;
}

etch_status_t etch_log_appender_file_destroy(void* appender)
{
    if(appender != NULL) {
        etch_free(appender);
    }
    return ETCH_SUCCESS;
}

static struct etch_log_appender_desc etch_log_appender_file_desc = {
    etch_log_appender_file_create,
    etch_log_appender_file_open,
    etch_log_appender_file_log,
    etch_log_appender_file_close,
    etch_log_appender_file_destroy
};

struct etch_log_appender_desc_entry {
    const char*             name;
    etch_log_appender_desc* desc;
};

static struct etch_log_appender_desc_entry appenders[] = {
    {"consoleappender", &etch_log_appender_console_desc},
    {"fileappender", &etch_log_appender_file_desc},
    {NULL, NULL},
    {NULL, NULL},
    {NULL, NULL}
};

struct etch_log_t
{
    etch_log_level    level;
    etch_log_appender appenders[ETCH_LOG_MAX_APPENDER];
};

etch_status_t etch_log_register_appender(const char* name, etch_log_appender_desc* desc)
{
    uint16 i = 0;
    for(i = 0; i < ETCH_LOG_MAX_APPENDER; i++) {
        if(appenders[i].name == NULL) {
            appenders[i].name = name;
            appenders[i].desc = desc;
            return ETCH_SUCCESS;
        }
    }
    return ETCH_ERROR;
}

static etch_status_t etch_log_get_appender(const char* name, etch_log_appender_desc** desc)
{
    uint16 i = 0;

    if(name == NULL || desc == NULL) {
        return ETCH_EINVAL;
    }

    for(i = 0; i < ETCH_LOG_MAX_APPENDER; i++) {
        if(appenders[i].name != NULL && strcmp(appenders[i].name, name) == 0) {
            *desc = appenders[i].desc;
            return ETCH_SUCCESS;
        }
    }
    return ETCH_ERROR;
}


etch_status_t etch_log_create(etch_log_t** logger, etch_config_t* config)
{
    etch_status_t   status    = ETCH_SUCCESS;
    etch_log_t*     newlogger = NULL;
    char*           propvalue = NULL;

    if(logger == NULL || config == NULL) {
        return ETCH_EINVAL;
    }

    newlogger = etch_malloc(sizeof(etch_log_t), 0);
    if(newlogger == NULL) {
        return ETCH_ENOMEM;
    }
    memset(newlogger, 0, sizeof(etch_log_t));

    status = etch_config_get_property_string(config, "etch.log", &propvalue);
    if(propvalue != NULL) {
        // parse log configuration
        char* token  = NULL;
        char* buffer = NULL;
        unsigned char  i      = 0;
        
        buffer = etch_malloc(strlen(propvalue) + 1, 0);
        strcpy(buffer, propvalue);
        
        // read log level
        token = strtok(buffer, ",");
        if(strcmp(token, "xdebug") == 0) {
            newlogger->level = ETCH_LOG_XDEBUG;
        } else
        if(strcmp(token, "debug") == 0) {
            newlogger->level = ETCH_LOG_DEBUG;
        } else
        if(strcmp(token, "info") == 0) {
            newlogger->level = ETCH_LOG_INFO;
        } else
        if(strcmp(token, "warn") == 0) {
            newlogger->level = ETCH_LOG_WARN;
        } else
        if(strcmp(token, "error") == 0) {
            newlogger->level = ETCH_LOG_ERROR;
        } else {
            newlogger->level = ETCH_LOG_WARN;
        }

        // read appenders
        do {
            token = strtok(NULL, ",");
            token = strtrim(token);
            if(token != NULL) {
                etch_log_appender_desc* appender_desc = NULL;
                status = etch_log_get_appender(token, &appender_desc);
                if(status == ETCH_SUCCESS && i < ETCH_LOG_MAX_APPENDER) {
                    // add new appender
                    newlogger->appenders[i].desc = appender_desc;
		    newlogger->appenders[i].desc->create(&newlogger->appenders[i].data, config);
                    newlogger->appenders[i].desc->open(&newlogger->appenders[i].data);
                    i++;
                } else {
                    // WARN appender not found or max appender count reached
                }
            }
        } while(token != NULL);

        // release buffer
        etch_free(buffer);
    }

    *logger = newlogger;

    return ETCH_SUCCESS;
}

etch_status_t etch_log_log(etch_log_t* logger, const char* category, etch_log_level level, const char* file, int line, const char* fmt, ...)
{
    etch_status_t       status     = ETCH_SUCCESS;
    uint16              i          = 0;
    etch_log_message_t* logmsg     = NULL;
    va_list             args;
    size_t              msg_length = 0;
    
    status = etch_runtime_get_logger(&logger);
    if(! logger || status != ETCH_SUCCESS) {
        // error: no logger available
        return ETCH_ERROR;
    }

    if(!(level >= logger->level)) {
        return ETCH_SUCCESS;
    }

    // create message and log to registered appenders
    logmsg = etch_malloc(sizeof(etch_log_message_t), 0);
    ETCH_ASSERT(logmsg != NULL);
    memset(logmsg, 0, sizeof(etch_log_message_t));

    logmsg->category  = category;
    logmsg->level     = level;
    logmsg->file      = file;
    logmsg->line      = line;
    logmsg->threadid  = etch_thread_current_id();
    time(&logmsg->timestamp);
    // format message
    va_start(args, fmt);
    msg_length = apr_vsnprintf(NULL, 0, fmt, args);
    logmsg->message = etch_malloc(msg_length + 1, 0);
    memset(logmsg->message, 0, msg_length + 1);
    apr_vsnprintf(logmsg->message, msg_length + 1, fmt, args);
    va_end(args);

    // TODO: add message to log runner / separate thread
    for(i = 0; i < ETCH_LOG_MAX_APPENDER; i++) {
        if(logger->appenders[i].desc != NULL) {
            logger->appenders[i].desc->log(&logger->appenders[i].data, logmsg);
        }
    }
    etch_free(logmsg->message);
    etch_free(logmsg);
    return ETCH_SUCCESS;
}

etch_status_t etch_log_logw(etch_log_t* logger, const char* category, etch_log_level level, const char* file, int line, const wchar_t* fmt, ...)
{
    //TODO: encode wchar_t to asci or utf-8 string
    return ETCH_ENOTIMPL;
}

etch_status_t etch_log_destroy(etch_log_t* logger)
{
    uint16 i = 0;

    if(logger != NULL) {
        // free appenders
        for(i = 0; i < ETCH_LOG_MAX_APPENDER; i++) {
            if(logger->appenders[i].desc != NULL) {
                logger->appenders[i].desc->close(&logger->appenders[i].data);
                logger->appenders[i].desc->destroy(&logger->appenders[i].data);
                logger->appenders[i].desc = NULL;
                logger->appenders[i].data = NULL;
            }
        }
        etch_free(logger);
    }
    return ETCH_SUCCESS;
}
