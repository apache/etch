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

/**
 * etch_global.c -- logically global methods, "static" methods, etc.
 * methods which can logically be executed from any context.
 */

#include <stdio.h>
#ifdef WIN32
#include <windows.h>
#endif

#include "etch_global.h"
#include "etchlog.h"
#include "etchutl.h"
#include <conio.h>
wchar_t* etch_emptystru = L"";
char*    etch_emptystra = "";
char*    ETCHGLOB = "ETCH";

void etch_instantiate_global_constants();
void etch_free_global_constants();


/**
 * etch_runtime_init()
 * global etch runtime startup initialization.
 */
int etch_runtime_init(const int is_client)
{
    int result = 0;
    if (is_runtime_initialized) return 1;
    etchlog_open(is_client);               /* open log file */
    etch_read_configfile(is_client, TRUE); /* initialize config - read config file */
    etchlog(ETCHGLOB, ETCHLOG_INFO, "log priority level is '%c'\n", config.log_level);

    etch_instantiate_global_constants();   /* initialize quasi-static constants */

    #if IS_TRACKING_ETCHHEAP               /* warning re memory tracking overhead */
    etchlog(ETCHGLOB, ETCHLOG_WARNING, "IS_TRACKING_ETCHHEAP is ON (normally off)\n");
    /* etch_watch_id is the global allocator ordinal for memory leak analysis. 
     * if etch_watch_id is nonzero, and if IS_TRACKING_ETCHHEAP is compiled TRUE, 
     * a debug break will occur at the specified memory allocation ordinal. 
     */ 
    etch_watch_id = config.memory_watch_id;
    if (etch_watch_id)
        etchlog(ETCHGLOB, ETCHLOG_WARNING, 
           "memory watch ID found - break to debugger may occur\n");
    #endif
    #if(0)
    #if IS_USING_ALLOC_ID   
    etchlog(ETCHGLOB, ETCHLOG_INFO, "IS_USING_ALLOC_ID is compiled ON (normally off)\n");
    #endif
    #endif

    is_runtime_initialized = TRUE;

    result = cache_create()? 0: -1;  /* instantiate runtime cache */
    
    if (0 == result)                 /* initialize running count of log files */
        etchlog_set_logfile_count (etchlog_countfiles (etchlog_get_dirpath()));

    return result;
}


/**
 * etch_runtime_cleanup()
 * global etch runtime cleanup
 * clears and frees the memory tracking table, and finally the runtime cache
 */
int etch_runtime_cleanup(int n, int m)
{
    is_memtable_instance = TRUE;
    etch_free_global_constants();
    destroy_hashtable(memtable, TRUE, TRUE);
    memtable = NULL;
    is_memtable_instance = FALSE;
    cache_destroy();
    return 0;
}


/**
 * memtable_clear()
 * global memory tracking table checkpoint.
 * clears all entries from the memory tracking table but leaves table intact.
 * we would use this for example between unit tests which uncovered memory leaks,
 * but we did not want to carry forward the leaks to the next tests.
 */
int memtable_clear()
{
    etchheap_currbytes = 0;
    if (NULL == memtable) return 0;

    is_memtable_instance = TRUE;
    memtable->vtab->clear(memtable->realtable, TRUE, TRUE, memtable, 0); 
    is_memtable_instance = FALSE;
    return 0;
}


/**
 * etch_strbytes()
 * @param s a unicode string.
 * @return number of bytes in s
 */
size_t etch_strbytes(const wchar_t* s) 
{
    return s? (wcslen(s) + 1) * sizeof(wchar_t): 0;
}


/**
 * etch_instantiate_global_constants()
 * instantiate global constants. these are constants which can't be statically intialized.
 * some of these may need to be freed in etch_free_global_constants in order that tests
 * can show all memory freed. any etch objects instantiated here may need to have their  
 * is_static marker set in order that they remain instantiated throughout execution.
 */
void etch_instantiate_global_constants()
{
    etchgc.etch_charsetname_us_ascii = L"us-ascii";
    etchgc.etch_charsetname_utf8     = L"utf-8";
    etchgc.etch_charsetname_utf16    = L"utf-16";
    etchgc.pctd = "%d";
}


/**
 * etch_free_global_constants()
 * free memory for global constants. etch objects freed here may need to have their  
 * is_static marker reset to zero so that their destructors will free them.
 */
void etch_free_global_constants()
{

}


/**
 * get_dynamic_classid()
 * get a class ID for objects not known to the binding.  
 * assignment is currently non-atomic.
 */
unsigned short get_dynamic_classid() 
{
    if (g_etch_curr_classid == 0)
        g_etch_curr_classid = CLASSID_DYNAMIC_START;

    return g_etch_curr_classid++;
} 


/**
 * get_dynamic_classid_unique()
 * if specified ID already assigned, return it; otherwise generate and return. 
 * assignment is currently non-atomic.
 */
unsigned short get_dynamic_classid_unique(unsigned short* globalid) 
{
    if (*globalid == 0) 
        *globalid = get_dynamic_classid();
    return (*globalid);
} 


/**
 * is_bad_pointer()
 * not sure how portable this is, but it catches freed windows heap content
 * such as pointer values 0xcccccccc, 0xfeeefeee, etc. it judges the specified
 * pointer to be bad if the high nibble is non-zero.
 */
boolean is_bad_pointer(void* p)
{
    const static size_t mask = 0xf << (((sizeof(void*) - 1) * 8) + 4);
    return ((size_t)p & mask) != 0;
}


/**
 * waitkey()
 * method to wait for a keypress. called from client and server exe's in order  
 * to optionally not exit console window until a key is pressed.
 */
int waitkey(const int is_waitkey_enabled, const int result)
{
    if (is_waitkey_enabled)
    {   printf("any key ..."); 
        while(!_getch());
        printf("\n");
    }
    return result;
}


/**
 * etch_system_nanotime()
 * operating system specific implementation of java System.nanotime().
 */
int64 etch_system_nanotime()
{
    int64 result = 0;

    #ifdef WIN32  

    LARGE_INTEGER li;
    QueryPerformanceCounter(&li);
    result = li.QuadPart;

    #endif /* WIN32 */

    return result;
}

