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
 * etchlog.h 
 * logger for the c binding
 */
#ifndef ETCHLOG_H
#define ETCHLOG_H

#include "etch_config.h"  

 typedef enum etchlog_level 
 {  ETCHLOG_XDEBUG = 0, /* extended debug for low-level or voluminous detail */
    ETCHLOG_DEBUG,      /* traces of connection open close send receive etc */
    ETCHLOG_INFO,       /* display server and client open and close etc */
    ETCHLOG_WARNING,    /* messages that should be noted regardless */
    ETCHLOG_ERROR,      /* failures */
 } etchlog_level;
 
 /**
  * etchlog(), etchlogw()
  * write a log message to console/file/system log depending on configuration.
  * @param cat the category name for this message. should be four characters,
  * upper case, identifying the module originating the log message.
  * @param level logging level of the message. 
  * @param fmt the format string ala printf.
  * @... a variable length argument list ala printf.
  */
void  etchlog (char *cat, etchlog_level level, const char *fmt, ...);

void  etchlogw (wchar_t *comp, etchlog_level level, const wchar_t *fmt, ...);

int   etchlog_open (const int is_client);
int   etchlog_openx(char* filepath);
int   etchlog_open_client();
int   etchlog_open_server();
int   etchlog_close();
int   etchlog_write(char*);
int   etchlog_get_logfile_count();
void  etchlog_set_logfile_count(const int);
char* etchlog_get_dirpath();


#endif /* ETCHLOG_H */
 