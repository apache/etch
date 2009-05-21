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
 * etch_config.c
 * config items and config file parse.
 * config file is formatted as a java-style properties file expected as ansi or utf-8.
 */
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <ctype.h>

#pragma warning(disable:4996) /* "unsafe" function warning */
#include "etch_config.h"  
#include "etchlog.h"

char* ETCHCFIG = "CFIG";

#define ETCHMAXCONFIGLINESIZE 240
#define ETCHCONFIG_TOKEN_FOUND 0
#define ETCHCONFIG_LINE_IS_BLANK_OR_COMMENT 1

#define ETCHCONFIG_DATATYPE_INT   1
#define ETCHCONFIG_DATATYPE_FIXED 2
#define ETCHCONFIG_DATATYPE_CHAR  3

#define NOTWHITESPACE(p) (*p != ' '  && *p != '\t')
#define ISWHITESPACE(p)  (*p == ' '  || *p == '\t')
#define NOTENDOFLINE(p)  (*p != '\0' && *p != '\n' && *p != '\r')
#define ISENDOFLINE(p)   (*p == '\0' || *p == '\n' || *p == '\r')

#define ETCHSERVERDEFAULTCONFIGFILEPATH  "../etch-c-server.properties"
#define ETCHCLIENTDEFAULTCONFIGFILEPATH  "../etch-c-client.properties"

typedef struct ndx {int id; char* key;} ndx; 

typedef struct etchconfig_work
{ int  keycount, item, datatype, length, value, is_bool;
  char *beg, *end, *endobj, *begkey, *equal, *begval, *endval;
  double fixedval;
} etchconfig_work;

int etchconfig_identify_value (etchconfig_work*);
int etchconfig_assign_value   (const etchconfig_work*); 
int etchconfig_identify_item  (etchconfig_work*, char* line, ndx* keys);  


/**
 * get_etch_server_configfile_path()
 * return the path to the server config file. 
 */
char* get_etch_server_configfile_path()
{
    static char* path = ETCHSERVERDEFAULTCONFIGFILEPATH;
    return path;
}


/**
 * get_etch_client_configfile_path()
 * return the path to the client config file. 
 */
char* get_etch_client_configfile_path()
{
    static char* path = ETCHCLIENTDEFAULTCONFIGFILEPATH;
    return path;
}


/**
 * open_etch_configfile() 
 */
int open_etch_configfile (const int is_client, FILE** f)
{
    int result = 0;   
    char* path = is_client? get_etch_client_configfile_path(): get_etch_server_configfile_path();

    if (NULL == (*f = fopen(path, "r")))
    {   path = is_client?  ETCHCLIENTDEFAULTCONFIGFILEPATH: ETCHSERVERDEFAULTCONFIGFILEPATH;

        if (NULL == (*f = fopen(path, "r")))   
        {  etchlog(ETCHCFIG, ETCHLOG_WARNING, "could not open config %s - using defaults\n", path);
           result = -1;
        }
    }

    return result;
}


/**
 * etch_reset_config()
 * initialize global config items with etch defaults.  
 * @remarks whenever a new config item is added, a setter for it must be coded 
 * herein in order to ensure that a default value exists for the item, and thus
 * it will have an appropriate value if either no config file entry is coded for
 * it, or if a config file can't be located and read.
 */
int etch_reset_config (const int is_client)
{
    memset(&config, 0, sizeof(struct etch_config));
    config.memory_watch_id = 0;
    config.log_level = ETCHCONFIGDEF_LOG_LEVEL;
    config.loglevel  = get_etch_loglevel(ETCHCONFIGDEF_LOG_LEVEL);
    config.max_log_files = ETCHCONFIGDEF_MAX_LOGFILES;
    config.max_log_lines = ETCHCONFIGDEF_MAX_LOGLINES;
    config.is_validate_on_read  = ETCHCONFIGDEF_IS_VALIDATE_ON_READ;
    config.is_validate_on_write = ETCHCONFIGDEF_IS_VALIDATE_ON_WRITE;
    config.is_log_to_file = ETCHCONFIGDEF_IS_LOG_TO_FILE;
    config.is_log_to_console = ETCHCONFIGDEF_IS_LOG_TO_CONSOLE;
    config.is_log_memory_leak_detail = ETCHCONFIGDEF_IS_LOG_MEMORY_LEAK_DETAIL;
    config.is_destroy_messages_with_mailbox = ETCHCONFIGDEF_IS_DESTROY_MESSAGES_WITH_MAILBOX;
    config.default_mailbox_read_waitms = ETCHCONFIGDEF_DEFAULT_MAILBOX_READ_WAITMS;
    config.sleepSecondsPriorClientExit = ETCHCONFIGDEF_SLEEP_SECONDS_PRIOR_CLIENT_EXIT;
    strncpy(config.example_string, ETCHCONFIGDEF_EXAMPLE_STRING_VALUE, ETCHCONFIGDEF_EXAMPLE_STRING_MAXLEN);

    return 0;
}


/**
 * etch_read_configfile()
 * read and validate config file, populate global config items. 
 * @param is_client boolean indicating if reading config on client side.
 * @param is_reset boolean indicating if global config items should be reset
 * to default values prior to reading the config file.
 * @return 0 success, -1 failure.
 * @remarks whenever a new config item is added, a mapping from that item's 
 * key to its external config file name must be coded herein.
 */
int etch_read_configfile (const int is_client, const int is_reset)
{
    int  result = 0, linenum = 0, itemcount = 0;
    char line [ETCHMAXCONFIGLINESIZE+1];  
    etchconfig_work work; 
    FILE* f = 0;

    ndx config_keys[] =                     
    { {ETCHCONFIGKEY_LOG_LEVEL,  "logLevel"},
      {ETCHCONFIGKEY_MAX_LOGFILES, "maxLogFiles"},
      {ETCHCONFIGKEY_MAX_LOGLINES, "maxLogLines"},
      {ETCHCONFIGKEY_ETCH_WATCH_ID, "memoryWatchID"},
      {ETCHCONFIGKEY_IS_LOG_TO_FILE,  "isLogToFile"},
      {ETCHCONFIGKEY_IS_LOG_TO_CONSOLE,  "isLogToConsole"},
      {ETCHCONFIGKEY_IS_VALIDATE_ON_READ,   "isValidateOnRead"},
      {ETCHCONFIGKEY_IS_VALIDATE_ON_WRITE,  "isValidateOnWrite"},
      {ETCHCONFIGKEY_IS_DESTROY_MESSAGES_WITH_MAILBOX, "isDestroyMessagesWithMailbox"},
      {ETCHCONFIGKEY_IS_DISPLAY_MEMORY_LEAK_DETAIL,    "isDisplayMemoryLeakDetail"},
      {ETCHCONFIGKEY_DEFAULT_MAILBOX_READ_WAITMS  ,    "defaultMailboxReadWaitms"},
      {ETCHCONFIGKEY_SLEEP_SECONDS_PRIOR_CLIENT_EXIT,  "sleepSecondsPriorClientExit"},
      {ETCHCONFIGKEY_TEST_STRING, "testString"},  /* for testing only */
      {ETCHCONFIGKEY_TEST_INT,    "testInt"},     /* for testing only */
      {ETCHCONFIGKEY_TEST_BOOL,   "testBool"},    /* for testing only */
      {ETCHCONFIGKEY_TEST_FIXED,  "testFixed"},   /* for testing only */
    };

    memset(&work, 0, sizeof(etchconfig_work));
    work.keycount = sizeof(config_keys) / sizeof(ndx);

    if (is_reset)  /* reset config entries to etch defaults */
        etch_reset_config (is_client);
  
    if (-1 == open_etch_configfile (is_client, &f))  /* open config file */
        return -1;

    while(1) /* while not eof on config file ... */
    {
        if (NULL == fgets (line, ETCHMAXCONFIGLINESIZE, f)) break;
        work.item = 0;
        linenum++;

        result = etchconfig_identify_item (&work, line, config_keys);
        if (ETCHCONFIG_LINE_IS_BLANK_OR_COMMENT == result) continue; 

        if (-1 == result) 
        {   etchlog(ETCHCFIG, ETCHLOG_ERROR, "at line %d: name not recognized\n", linenum);
            continue;
        } 

        if((-1 == etchconfig_identify_value(&work)) || (-1 == etchconfig_assign_value(&work)))
        {
            etchlog(ETCHCFIG, ETCHLOG_ERROR, "at line %d: invalid item value\n", linenum);
            continue;
        }

        if (*work.endval == ';') /* warning that a semicolon may have been typed in error */
            etchlog(ETCHCFIG, ETCHLOG_WARNING, "at line %d: value ends with semicolon\n", linenum);

        itemcount++;
    }
       
    fclose(f);
    etchlog(ETCHCFIG, ETCHLOG_DEBUG, "%d config entries read\n", itemcount);
    return itemcount;
}


/**
 * etchconfig_identify_item()
 * parse out and identify object token   
 */
int etchconfig_identify_item (etchconfig_work* work, char* line, ndx* keys)  
{
    int  i;
    ndx  *keyi = keys;
    char csave, *q, *p = line;

    while(ISWHITESPACE(p)) p++;  /* find first noblank character */
    if (ISENDOFLINE(p)) return ETCHCONFIG_LINE_IS_BLANK_OR_COMMENT;
    work->beg = p;               /* mark initial character */ 

    while(*p && *p != '#' && *p != '=') p++;  /* find equal sign */              
    if (*p == '#')      return ETCHCONFIG_LINE_IS_BLANK_OR_COMMENT;
    if (ISENDOFLINE(p)) return -1;

    work->equal = p;             /* mark equal sign */

    q = work->equal; q--;
    while(ISWHITESPACE(q))  q--; /* strip trailing whitespace */

    work->end = q;               /* mark final character */
    q++;                         /* point after item name */
    csave = *q;                  /* save character at that position */
    *q = '\0';                   /* and terminate item name string */
    
    for(i = 0, work->item = 0; i < (const int) work->keycount; i++, keyi++)
    {   /* compare found item name with expected item names */
        if (stricmp(keyi->key, work->beg) == 0)
        {   work->item = keyi->id;
            break;
        }
    }
 
    *q = csave;  /* restore character replaced above */
    return work->item? ETCHCONFIG_TOKEN_FOUND: -1;
}


/**
 * etchconfig_identify_item()
 * parse out item value and determine its data type.   
 */                                            
int etchconfig_identify_value (etchconfig_work* work)             
{
    char *p = NULL, *q = NULL;
    int  i = 0, digits = 0, expecteddigits = 0, dots = 0, isnegative = 0; 
    work->length = work->datatype = work->is_bool = 0;
    work->begval = work->endval = NULL; 

    p = work->equal; p++;                     /* start after equal sign */
    while(*p && ISWHITESPACE(p)) p++;         /* find first noblank character */
    if (ISENDOFLINE(p) || *p == '#') return -1;

    q = p;
    while(NOTENDOFLINE(q) && *q != '#') q++;  /* find end of line or comment */
    q--;
    while(ISWHITESPACE(q)) q--;               /* strip trailing whitespace */
              
    /* calculate length of value */                              
    work->length = (int) ((size_t) q - (size_t) p + 1);    
    if (work->length < 1) return -1; 

    if (*p == '\"')                 /* if quoted string ... */
    {   if (*q != '\"') return -1;  /* test for close quote */
        p++; q--;                   /* remove quotes */
        work->datatype = ETCHCONFIG_DATATYPE_CHAR;
    }

    work->begval = p;   /* set value boundaries */
    work->endval = q;
    *++q = '\0';        /* add terminator for logging */

    if (work->datatype) /* data type now known? */
        return ETCHCONFIG_TOKEN_FOUND;
                     
    /* determine data type of value */   
    expecteddigits = work->length; 
        
    for(; i < (const int) work->length; i++, p++)
    {
      if  (isdigit(*p))
           digits++;
      else
      if  (*p == '-' && i == 0)  /* negative sign? */
           isnegative = TRUE;
      else                       /* decimal point? */
      if  (*p == '.')
           dots++;      
    }

    if (digits > 0)
    {   if (isnegative) expecteddigits--;
        if (dots == 1)  expecteddigits--;
    }

    if (digits == expecteddigits && dots == 0)
    {    /* integral value e.g. 314159 */
         work->datatype = ETCHCONFIG_DATATYPE_INT;
         work->value = atoi(work->begval);
         if (work->value == 0 ||work->value == 1)
             work->is_bool = TRUE;  /* integer is boolean-valued */
    }
    else
    if  (digits == expecteddigits && dots == 1)
    {    /* fixed decimal value e.g. 3.14159 */
         work->datatype = ETCHCONFIG_DATATYPE_FIXED;
         work->fixedval = strtod(work->begval, &q);
    }    /* unquoted string */
    else work->datatype = ETCHCONFIG_DATATYPE_CHAR;

    return ETCHCONFIG_TOKEN_FOUND;
}


/**
 * get_etch_loglevel()
 * convert log level from character supplied in config to ordinal used by logger. 
 */  
int get_etch_loglevel (const unsigned char loglvl)
{
    int thislevel = -1;
    switch(toupper(loglvl))
    { case 'D': thislevel = ETCHLOG_DEBUG;   break;   
      case 'I': thislevel = ETCHLOG_INFO;    break;  
      case 'X': thislevel = ETCHLOG_XDEBUG;  break;   
      case 'E': thislevel = ETCHLOG_ERROR;   break;   
      case 'W': thislevel = ETCHLOG_WARNING; break;   
    }
    return thislevel;
}


/**
 * etchconfig_assign_value()
 * edit value from config file and assign to internal config item. 
 * @remarks whenever a new config item is added, its key, plus code 
 * to validate the value, must be added to the switch herein.
 */  
int etchconfig_assign_value (const etchconfig_work* work)                   
{
    const int datatype = work->datatype; 
    const int ivalue   = work->value, is_bool_value = work->is_bool;
    const char* cvalue = work->begval;
    int result = 0, n = 0;

    switch(work->item)
    { 
      case ETCHCONFIGKEY_LOG_LEVEL:
           if (datatype != ETCHCONFIG_DATATYPE_CHAR) 
               result = -1; 
           else  /* translate character level to ordinal */
           if (-1 == (n = get_etch_loglevel (*cvalue)))
               result = -1;
           else 
           {  config.loglevel = n;   /* ordinal level */
              config.log_level = *cvalue;  /* character level */
           }
           break;

      case ETCHCONFIGKEY_MAX_LOGFILES:
           if (datatype != ETCHCONFIG_DATATYPE_INT) result = -1;
           else config.max_log_files = ivalue;
           break;

      case ETCHCONFIGKEY_MAX_LOGLINES:
           if (datatype != ETCHCONFIG_DATATYPE_INT) result = -1;
           else config.max_log_lines = ivalue;
           break;

      case ETCHCONFIGKEY_ETCH_WATCH_ID:
           if (datatype != ETCHCONFIG_DATATYPE_INT) result = -1;
           else config.memory_watch_id = ivalue;
           break;

      case ETCHCONFIGKEY_DEFAULT_MAILBOX_READ_WAITMS:
           if (datatype != ETCHCONFIG_DATATYPE_INT || ivalue < (-1)) 
               result = -1;
           else config.default_mailbox_read_waitms = ivalue;
           break;

      case ETCHCONFIGKEY_IS_LOG_TO_FILE:
           if (!is_bool_value) result = -1; 
           else config.is_log_to_file = ivalue;
           break;

      case ETCHCONFIGKEY_IS_LOG_TO_CONSOLE:
           if (!is_bool_value) result = -1; 
           else config.is_log_to_console = ivalue;
           break;

      case ETCHCONFIGKEY_IS_VALIDATE_ON_READ:
           if (!is_bool_value) result = -1; 
           else config.is_validate_on_read = ivalue;
           break;

      case ETCHCONFIGKEY_IS_VALIDATE_ON_WRITE:
           if (!is_bool_value) result = -1; 
           else config.is_validate_on_write = ivalue;
           break;

      case ETCHCONFIGKEY_IS_DESTROY_MESSAGES_WITH_MAILBOX:
           if (!is_bool_value) result = -1; 
           else config.is_destroy_messages_with_mailbox = ivalue;
           break;

      case ETCHCONFIGKEY_IS_DISPLAY_MEMORY_LEAK_DETAIL:
           if (!is_bool_value) result = -1; 
           else config.is_log_memory_leak_detail = ivalue;
           break;

      case ETCHCONFIGKEY_SLEEP_SECONDS_PRIOR_CLIENT_EXIT:
           if (datatype != ETCHCONFIG_DATATYPE_INT || ivalue < 0) 
               result = -1;
           else config.sleepSecondsPriorClientExit = ivalue;
           break;

      /* test items for testing config file parsing */
      case ETCHCONFIGKEY_TEST_STRING: /* example for a string config entry */
           if (datatype != ETCHCONFIG_DATATYPE_CHAR) result = -1;
           else if (strlen(cvalue) > ETCHCONFIGDEF_EXAMPLE_STRING_MAXLEN) result = -1; 
           else strcpy(config.example_string, cvalue);
           break;

      case ETCHCONFIGKEY_TEST_INT:
           if (datatype != ETCHCONFIG_DATATYPE_INT) result = -1;
           break;

      case ETCHCONFIGKEY_TEST_BOOL:
           if (!is_bool_value) result = -1; 
           break;

      case ETCHCONFIGKEY_TEST_FIXED:
           if (datatype != ETCHCONFIG_DATATYPE_FIXED) result = -1;
           break;

      default: result = -1; 
    }

    return result;
}
