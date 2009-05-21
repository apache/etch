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
 * etchlog.c
 * etch c binding logger
 */

#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <stdio.h>
#include <time.h> 
#include <direct.h>
#include <sys/stat.h>

#if IS_WINDOWS_ETCH
#pragma warning(disable:4996) /* potentially unsafe function warning */
#endif

#include "etchthread.h"  /* for mutex */
#include "etchlog.h"
#include "etchutl.h"

void etchlog_timestamp (char*    buf, const char*    component, etchlog_level lvl);  
void etchlogw_timestamp(wchar_t* buf, const wchar_t* component, etchlog_level lvl);  
int  etchlog_copy_logfile(const int); 
#define ETCHLOG_STACKBUFSIZE 256
#define ETCHLOG_PREAMBLESIZE 19
#define ETCHLOG_STAMPLEN 12
#define ETCHLOG_CBUFLEN  4
char*   ETCHLOGX = "LOGX";

char* etchlog_level_str[] = 
{
    "X","D","I","W","E"
};

wchar_t* etchlogw_level_str[] = 
{
    L"X",L"D",L"I",L"W",L"E"
};

char  etchlogbuf[ETCHLOG_STACKBUFSIZE];
int   etch_loglines, etch_logfiles;
int   is_etch_logfile_open, is_etchlog_shutdown, is_etchlog_suspended;
FILE* flog;
char* logpath;
#define ETCHCLIENTLOGDIR  "../logcli"
#define ETCHCLIENTLOGPATH "../logcli/etchclient.log"
#define ETCHSERVERLOGDIR  "../logsrv" 
#define ETCHSERVERLOGPATH "../logsrv/etchserver.log"
#define IS_ETCH_LOG_FILE    TRUE
#define IS_ETCH_LOG_CONSOLE TRUE


/**
 * etchlog_open()
 * log file open.
 */
int etchlog_open (const int is_client)
{
    return is_client? etchlog_open_client(): etchlog_open_server();
}


/**
 * etchlog_open_server()
 * log file open.
 */
int etchlog_open_server()
{
    config.calculated_is_client = FALSE;
    return etchlog_openx(ETCHSERVERLOGPATH);
}


/**
 * etchlog_open_client()
 * client log file lazy open.
 */
int etchlog_open_client()
{
    config.calculated_is_client = TRUE;
    return etchlog_openx(ETCHCLIENTLOGPATH);  
}


/**
 * etchlog_openx()
 * log file lazy open.
 * note that the etch .exe directory will be different when the etch is started
 * in the debugger, than when it is started from the command line, and so the log
 * directories, being relative to the .exe directory, will change accordingly.
 */
int etchlog_openx (char* filepath)
{
    int result = 0;

    if (is_etch_logfile_open);
    else
    if (is_etchlog_shutdown)  
        result = -1;
    else  
    {   const char* dirpath = config.calculated_is_client? ETCHCLIENTLOGDIR: ETCHSERVERLOGDIR;
        mkdir (dirpath); /* create the log directory if it does not already exist */
        logpath = filepath; 

        #if(0) /* code to identify effective directory */
        { const char* t = "it works!", *fn = "etchtest.txt";  
          FILE* f = fopen(fn,"w"); size_t n = fwrite(t,1,strlen(t),f); fclose(f);
          printf("*** etchlog_openx wrote %d to %s\n", n, fn);  
        }
        #endif

        etchlog_copy_logfile (FALSE); /* back up etchlog.log */

        if (flog = fopen(filepath, "w")) 
        {   is_etch_logfile_open = TRUE;
            etch_loglines = 0;
        }
        else 
        {   printf("could not open log file %s\n", filepath);
            is_etchlog_shutdown = TRUE;
            result = -1;
        }
    }

    return result;
}


/**
 * etchlog_close()
 * currently can't be reopened.
 */
int etchlog_close()
{
    if (is_etch_logfile_open); 
        fclose(flog);
    is_etch_logfile_open = FALSE;
    is_etchlog_shutdown  = TRUE;
    return 0;
}
 

/**
 * etchlog_write()
 * write specified ansi string to log.
 * @return count of bytes written.
 */
int etchlog_write(char* buf)
{
    int bytecount = 0, bytesout = 0, result = 0;
    if (is_etchlog_suspended) return 0;

    if (buf && (0 > (bytecount = (int) strlen(buf)))) 
        if (config.is_log_to_file && is_etch_logfile_open && !is_etchlog_suspended)
            if (0 < (bytesout = (int) fwrite(buf, 1, bytecount, flog)))
            {   fflush(flog);  /* linecount assumes each line has a line feed */
                if (++etch_loglines >= config.max_log_lines)  
                {   result = etchlog_copy_logfile(TRUE);   /* copy and reopen */
                    assert(0 == result);
                }
            }

    return bytesout;
}


/**
 * etchlog()
 * write ansi string to log.
 */
void etchlog (char *comp, etchlog_level level, const char *fmt, ...)
{
    char *buf;
    va_list args;
    int result = 0, arglen, totallen, charcount;

    if (level < config.loglevel) return;

    loglock->acquire(loglock); /* todo use a queue instead of serializing? */

    do {

    va_start(args, fmt);
    if (0 > (arglen = _vscprintf (fmt, args) + 1)) break;
   
    if ((totallen = arglen + ETCHLOG_PREAMBLESIZE) > ETCHLOG_STACKBUFSIZE)
         buf = malloc(totallen);  /* eschew etch_malloc since immediately freed */
    else buf = etchlogbuf;
    charcount = totallen - 1;  
    memset(buf,' ', charcount); buf[charcount] = '\0'; 

    if (strlen(comp) > 4) comp[4] = '\0';     /* module id exactly 4 characters */

    etchlog_timestamp(buf, comp, level);

    vsprintf(buf + ETCHLOG_PREAMBLESIZE, fmt, args);
    va_end(fmt);

    #if IS_ETCH_LOG_CONSOLE      /* log to console, unless configured otherwise */
    if (config.is_log_to_console)
        printf(buf);  
    #endif /* IS_ETCH_LOG_CONSOLE */

    #if IS_ETCH_LOG_FILE            /* log to file, unless configured otherwise */
    if (config.is_log_to_file && is_etch_logfile_open && !is_etchlog_suspended)
        if (fwrite(buf, 1, charcount, flog) > 0)
        {   fflush(flog);    /* linecount assumes each line has a line feed */
            if (++etch_loglines >= config.max_log_lines) 
            {   result = etchlog_copy_logfile(TRUE);     /* copy and reopen */
                if (-1 == result) printf("*** LOGFILE COPY FAILED\n");
                assert(0 == result);
            }
        }
    #endif /* IS_ETCH_LOG_FILE */ 

    if (totallen > ETCHLOG_STACKBUFSIZE)
        free(buf);   /* see etch_malloc comment above */

    } while(0);

    loglock->release(loglock); /* todo use a queue instead of serializing */
}


/**
 * etchlogw()
 * write unicode string to log.
 */
void etchlogw (wchar_t *comp, etchlog_level level, const wchar_t *fmt, ...)
{
    wchar_t *buf;
    va_list args;
    int argcharlen, totalcharlen, bytelen;

    if (level < config.loglevel) return;

    loglock->acquire(loglock); /* todo use a queue instead of serializing? */

    do {

    va_start(args, fmt);
    if (0 > (argcharlen = _vscwprintf(fmt, args) + 1)) break;

    totalcharlen = argcharlen + ETCHLOG_PREAMBLESIZE;
    bytelen = totalcharlen * sizeof(wchar_t);
   
    if  (bytelen > ETCHLOG_STACKBUFSIZE)
         buf  = malloc(bytelen); /* eschew etch_malloc since immediately freed */
    else buf  = (wchar_t*) etchlogbuf;

    memset (buf, 0, bytelen);  
    wmemset(buf, L' ', totalcharlen);      

    if (wcslen(comp) > 4) comp[4] = L'\0';   /* module id exactly 4 characters */

    etchlogw_timestamp(buf, comp, level);

    /* note that offsets are specified in characters not bytes */
    vswprintf(buf + ETCHLOG_PREAMBLESIZE, totalcharlen, fmt, args);
    va_end((void*) fmt);
     

    #if IS_ETCH_LOG_CONSOLE     /* log to console, unless configured otherwise */
    if (config.is_log_to_console)
        wprintf(buf); 
    #endif /* IS_ETCH_LOG_CONSOLE */


    #if IS_ETCH_LOG_FILE        /* log to file, unless configured otherwise */
    if (config.is_log_to_file)
        if (0 == etchlog_open_server())
            if (fwrite(buf, 1, bytelen, flog) > 0)
            {   fflush(flog);
                if (++etch_loglines >= config.max_log_lines) 
                {   const int result = etchlog_copy_logfile(TRUE);  
                    assert(0 == result);
                }
            }
    #endif /* IS_ETCH_LOG_FILE */

    if (bytelen > ETCHLOG_STACKBUFSIZE)
        free(buf);   /* see etch_malloc comment above */

    } while(0);

    loglock->release(loglock); /* todo use a queue instead of serializing */
}


/** 
 * etchlog_timestamp()
 * sets preamble to supplied buffer which must be at least 20 bytes. 
 * string length of preamble is always 19.
 */
void etchlog_timestamp(char* buf, const char* component, etchlog_level lvl)   
{   /* 012345678901234567 */
    /* MMDD HHMMSS CCCC L */
    static const char* TIMESTAMPMASK = "%02d%02d %02d%02d%02d "; 
    char cbuf[ETCHLOG_CBUFLEN+1];
    struct tm* t;
    time_t l; 

    time(&l);                                
    t = localtime(&l);
    memset (cbuf,' ',ETCHLOG_CBUFLEN+1);
    memcpy (cbuf, component, strlen(component));

    sprintf(buf, TIMESTAMPMASK, /* t->tm_year+1900, */
            t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

    memcpy(buf+12, cbuf, ETCHLOG_CBUFLEN);
    *(buf+17) = *etchlog_level_str[lvl];
    *(buf+19) = '\0'; /* caller will overwrite term */
}


/** 
 * etchlogw_timestamp()
 */
void etchlogw_timestamp(wchar_t* buf, const wchar_t* component, etchlog_level lvl)   
{   /* 012345678901234567 */
    /* MMDD HHMMSS CCCC L */
    static const wchar_t* TIMESTAMPMASKW = L"%02d%02d %02d%02d%02d "; 
    wchar_t cbuf[ETCHLOG_CBUFLEN+1];
    struct tm* t;
    time_t l; 

    time(&l);                                
    t = localtime(&l);
    wmemset (cbuf, L' ', ETCHLOG_CBUFLEN+1);
    wmemcpy (cbuf, component, wcslen(component));

    swprintf(buf, ETCHLOG_STAMPLEN+1, TIMESTAMPMASKW, /* t->tm_year+1900, */
            t->tm_mon+1, t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec);

    wmemcpy(buf+12, cbuf, ETCHLOG_CBUFLEN);  /* note that numeric constants */
    *(buf+17) = *etchlog_level_str[lvl];    /* specify characters not bytes */
    *(buf+19) = '\0';  /* caller will overwrite term */
}


/**
 * etchlog_get_dirpath()
 * get the relative path to the log file directory.
 * @return a path string or null.
 */
char* etchlog_get_dirpath() 
{
    return config.calculated_is_client? ETCHCLIENTLOGDIR: ETCHSERVERLOGDIR;
}


/**
 * etchlog_get_logfile_count()
 * @return the current running count of log files.
 */
int etchlog_get_logfile_count() 
{                
    return etch_logfiles;
}


/**
 * etchlog_set_logfile_count()
 * set the initial count of log files.
 */
void etchlog_set_logfile_count(const int count) 
{                
    etch_logfiles = count;
}


/**
 * etchlog_get_bkuppath()
 * get a backup log file name/relative path.
 * @param outbuf buffer to receive the file name/path
 * @param outbufsize size of the out buffer, must be a/l 23 + characters plus 
 * the size of the relative path. if relative directory strings are "../logcli/" 
 * and "../logsrv/", the return string will be 32 characters including null term.
 * @param seqno the start value of the timestamp sequencer.
 * @return current sequencer on success, -1 failure. file name and relative path in outbuf.
 */
int etchlog_get_bkuppath (char* outbuf, const int outbufsize, int seqno)
{
    /* 0         1         2         3
    /* 012345678901234567890123456789012 */
    /* ../logcli/YYYYMMDDHHMMSSNNNN.log  */
    struct tm* t;
    char buf[48], *p = 0;
    const char *BKMASK = "%04d%02d%02d%02d%02d%02d%04d%s", *DOTLOG = ".log";
    time_t l; time(&l);                                
    t  = localtime(&l);
    strcpy (buf, config.calculated_is_client? ETCHCLIENTLOGDIR: ETCHSERVERLOGDIR);
    strcat (buf, "/");
    p = buf + (int) strlen(buf);

    while(1)
    {   sprintf (p, BKMASK, t->tm_year+1900, t->tm_mon+1, 
            t->tm_mday, t->tm_hour, t->tm_min, t->tm_sec, seqno, DOTLOG);
        if (!etch_existsfile(buf)) break;
        if (++seqno > 9999) return -1;
    }

    if (outbufsize < ((int) strlen(buf) + 1)) return -1;
    strcpy(outbuf, buf);
    return seqno + 1;
}


/**
 * etchlog_purge_logfiles()
 * purge files from current log directory.
 * @return count of files purged.
 */
int etchlog_purge_logfiles ()
{
    int purgedcount = 0;

    if (etch_logfiles >= config.max_log_files + ETCHCONFIGDEF_LOGCOUNT_GRACE)
    {   
        char* dirpath = etchlog_get_dirpath();
        const int purgecount = etch_logfiles - config.max_log_files;
        etchlog (ETCHLOGX, ETCHLOG_XDEBUG, "purging %d log files from %s ...\n", 
                 purgecount, dirpath);
        purgedcount = etchlog_purgefiles (dirpath, purgecount, 0);
        etchlog (ETCHLOGX, ETCHLOG_DEBUG, "purged %d log files\n", purgedcount);
        etch_logfiles -= purgedcount;
    }

    return purgedcount;
}


/**
 * etchlog_copy_logfile()
 * copy primary log file to a backup file.
 * @param is_purge_logfiles if true, and count of log files exceeds configured
 * maximum plus a grace overage, a number of log files is deleted such that the
 * log file count is brought back down to the configured maximum.
 * @remarks we may at some point want to move the log file purge to a work thread.
 * @return 0 success, -1 failure.
 */
int etchlog_copy_logfile (const int is_purge_logfiles)
{   
    char newpath[48];
    int result = -1, attempts = 0, seqno = 0;
    if (NULL == logpath) return -1;

    is_etchlog_suspended = TRUE;
    if (is_etch_logfile_open)
        fclose(flog);

    while (attempts++ < 10)  
    {   
        if (-1 == (seqno = etchlog_get_bkuppath (newpath, sizeof(newpath), seqno))) 
            break;

        if (0 != rename (logpath, newpath)) 
            continue;

        etch_logfiles++;
        etch_loglines = 0;

        if (is_etch_logfile_open)
            if (NULL == (flog = fopen(logpath, "w"))) 
                break;         

        is_etchlog_suspended = FALSE;

        if (is_purge_logfiles)  
            etchlog_purge_logfiles();

        result = 0;
        break;
    }  

    return result;
}

