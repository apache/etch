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
 * etch_global.h -- logically global methods, "static" methods, etc.
 * methods which can logically be executed from any context.
 */

#ifndef ETCHGLOBAL_H
#define ETCHGLOBAL_H

#include "etch_config.h"
#include "etchhash.h"
#include "etchmem.h"
#include "etchrun.h"


/**
 * etch_runtime_init()
 * global etch runtime startup initialization
 * instantiates the global cache
 */
int etch_runtime_init(const int is_client);


/**
 * etch_runtime_cleanup()
 * global etch runtime cleanup
 * clears and frees the memory tracking table, and finally the global cache
 */
int etch_runtime_cleanup(int n, int m);


/**
 * global memory tracking table checkpoint
 * clears all entries from the memory tracking table but leaves table intact.
 * we would use this for example between unit tests which had memory leaks,
 * but we did not want to carry forward the leaks to the next tests.
 */
int memtable_clear();

wchar_t* new_wchar    (const wchar_t*);
size_t   etch_strbytes(const wchar_t*);

/**
 * cached_etchobject_vtable
 * static cache vtable for etchobject in order to avoid global cache lookup
 */
void* cached_etchobject_vtable;


/**
 * object type sequence for generated objects
 */
int g_etch_curr_objtype;   /* current offset from ETCHTYPEB_USER */


/**
 * operating system specific implementation of java System.nanotime().
 */
int64 etch_system_nanotime();

unsigned char is_runtime_initialized;

unsigned char is_bad_pointer(void*);

int waitkey(const int is_waitkey_enabled, const int result);

/* empty string contants */
wchar_t* etch_emptystru;
char*    etch_emptystra;

/**
 * class ID sequence for generated classes
 */
/* current offset from CLASSID_DYNAMIC_START (etchobjtypes.h) */
unsigned short g_etch_curr_classid;  /* current offset from CLASSID_DYNAMIC_START (etchobjtypes.h) */
unsigned short get_dynamic_classid(); 
unsigned short get_dynamic_classid_unique(unsigned short* globalid); 


/**
 * malloc'ed global constants 
 */
typedef struct etch_global_constants
{
    #if(0)
    etch_string* emptystring_unicode;
    etch_boolean boolean_true;
    etch_boolean boolean_false;
    #endif

    wchar_t* etch_charsetname_us_ascii;
    wchar_t* etch_charsetname_utf8;
    wchar_t* etch_charsetname_utf16;

    char* pctd; /* for "%d" sprintf mask */

} etch_global_constants;

etch_global_constants etchgc;


#endif /* #ifndef ETCHGLOBAL_H */ 