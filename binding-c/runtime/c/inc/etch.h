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
 * etch.h -- includes common to the entire etch c binding. note that all
 * function signatures are assumed to have a __cdecl calling convention.
 */

#ifndef ETCH_H
#define ETCH_H

#undef  IS_WINDOWS_ETCH  
#ifdef  WIN32
#define IS_WINDOWS_ETCH 32
#endif  /* WIN32 */

#include "etchwarn.h"     /* unicode switch warning */
#include <tchar.h>        /* everything is unicode */
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include "etchdefs.h"     /* constants, #defines, typedefs, etc. */
#include "etchobjtypes.h" /* internal etch object type constants */

unsigned etchhash(const void* key, const int keylen, const unsigned priorhash);  

typedef int (*etch_comparator) (void* myobj, void* otherobj);

#define IS_ETCH_TRANSPORT_LITTLEENDIAN FALSE
#define ETCHCONFIGDEF_EXAMPLE_STRING_MAXLEN 15 

/**
 * etch configuration items 
 */
typedef struct etch_config
{
  int memory_watch_id; /* memory alloc id */
  int default_mailbox_read_waitms;
  int sleepSecondsPriorClientExit; /* for use when stepping thru server */ 
  int max_log_files;   /* max log files in a single log directory */
  int max_log_lines;   /* max lines in a single log file */
  int loglevel;        /* computed 0, 1, 2, 3, 4 */
  unsigned char log_level;      /* X, D, I, W, E */
  unsigned char is_log_to_file;  
  unsigned char is_log_to_console;
  unsigned char is_validate_on_write;
  unsigned char is_validate_on_read;
  unsigned char is_log_memory_leak_detail;
  unsigned char is_destroy_messages_with_mailbox;
  unsigned char calculated_is_client;
  char example_string[ETCHCONFIGDEF_EXAMPLE_STRING_MAXLEN+1];

} etch_config;

etch_config config;  /* global config items */


#endif /* #ifndef ETCH_H */