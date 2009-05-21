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
 * etch_config.h -- configuration items and file
 */
 
#ifndef ETCHCONFIG_H
#define ETCHCONFIG_H

#include "etch.h"

#if(0)
_________________________________________________________________________________
example of adding a config entry to etch:
1)  decide on an external config item name, e.g., myMaxClients.
2)  add an internal name for the new entry to the etch_config struct below, e.g., 
    int my_max_clients;
3)  add a default value for the item to the defaults section below, e.g.,
    #define ETCHCONFIGDEF_MY_MAX_CLIENTS 8
4)  using the internal name from (2), and the default value from (3), add a 
    default setter for the item to etch_reset_config() in etch_config.c;  e.g., 
    config.my_max_clients = ETCHCONFIGDEF_MY_MAX_CLIENTS;
5)  add a key for the item to the end of the ordinals section below, e.g.,
    #define ETCHCONFIGKEY_MY_MAX_CLIENTS 21  /* value will be previous key + 1 */
6)  using the key from (5) and the external name from (1), ad an entry to the  
    config_keys table found in etch_read_configfile() in etch_config.c; e.g.,
    {ETCHCONFIGKEY_MY_MAX_CLIENTS, "myMaxClients"},
7.  using the key from (5) and the internal name from (2), add an edit and 
    assignment for the item to etchconfig_assign_value() in etch_config.c; e.g.,
    case ETCHCONFIGKEY_MY_MAX_CLIENTS:
         if (datatype != ETCHCONFIG_DATATYPE_INT) result = -1;
         else config.my_max_clients = ivalue;
         break;
8.  add the external name from (1), plus a value, to etch-c.config, e.g.,
    myMaxClients = 4    # your optional comment here
9.  optionally add some code to etch to put the configured value to use, e.g.,
    if (etch_clients_count < config.my_max_clients)
        etch_clients_count++;
    else etchlog(ETCHETCH, ETCHLOG_ERROR, "max clients exceeded\n");
10. build the etch runtime, and link an etch executable.
11. start up the executable from (10), and test.
_________________________________________________________________________________
#endif


/* default values for each config item */
#define ETCHCONFIGDEF_LOG_LEVEL  'X'
#define ETCHCONFIGDEF_MAX_LOGFILES 20
#define ETCHCONFIGDEF_MAX_LOGLINES 4000
#define ETCHCONFIGDEF_LOGCOUNT_GRACE  10
#define ETCHCONFIGDEF_IS_LOG_TO_FILE  TRUE
#define ETCHCONFIGDEF_IS_LOG_TO_CONSOLE  TRUE
#define ETCHCONFIGDEF_IS_VALIDATE_ON_READ  TRUE
#define ETCHCONFIGDEF_IS_VALIDATE_ON_WRITE TRUE
#define ETCHCONFIGDEF_IS_DESTROY_MESSAGES_WITH_MAILBOX  FALSE
#define ETCHCONFIGDEF_IS_LOG_MEMORY_LEAK_DETAIL  TRUE
#define ETCHCONFIGDEF_DEFAULT_MAILBOX_READ_WAITMS  3000
#define ETCHCONFIGDEF_SLEEP_SECONDS_PRIOR_CLIENT_EXIT 0
#define ETCHCONFIGDEF_EXAMPLE_STRING_VALUE  "examplevalue" 


/* unique ordinal key for each config file item. each configured item
 * (but not computed items) must have an entry here. 
 */
#define ETCHCONFIGKEY_TEST_STRING           1  /* internal testing use */
#define ETCHCONFIGKEY_TEST_INT              2  /* internal testing use */
#define ETCHCONFIGKEY_TEST_BOOL             3  /* internal testing use */
#define ETCHCONFIGKEY_TEST_FIXED            4  /* internal testing use */
#define ETCHCONFIGKEY_LOG_LEVEL             5
#define ETCHCONFIGKEY_ETCH_WATCH_ID         6
#define ETCHCONFIGKEY_IS_LOG_TO_FILE        7
#define ETCHCONFIGKEY_IS_LOG_TO_CONSOLE     8
#define ETCHCONFIGKEY_IS_VALIDATE_ON_READ   9
#define ETCHCONFIGKEY_IS_VALIDATE_ON_WRITE 10
#define ETCHCONFIGKEY_MAX_LOGFILES         11
#define ETCHCONFIGKEY_MAX_LOGLINES         12
#define ETCHCONFIGKEY_IS_DESTROY_MESSAGES_WITH_MAILBOX 13
#define ETCHCONFIGKEY_IS_DISPLAY_MEMORY_LEAK_DETAIL    14
#define ETCHCONFIGKEY_DEFAULT_MAILBOX_READ_WAITMS      15
#define ETCHCONFIGKEY_SLEEP_SECONDS_PRIOR_CLIENT_EXIT  16


/**
 * configuration items 
 * the config struct and global are defined in etch.h - copied here for reference
 */
#if(0)

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
  char example_string[ETCHCONFIGDEF_EXAMPLE_STRING_MAXLEN+1];

} etch_config;

etch_config config;  /* global config items */

#endif


int etch_read_configfile();
int etch_reset_config (const int is_client);
int get_etch_loglevel (const unsigned char);


#endif /* #ifndef ETCHCONFIG_H */ 