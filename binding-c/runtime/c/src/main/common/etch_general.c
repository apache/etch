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
 * etch_general.h -- general stuff.
 */

#include "etch_general.h"
#include "etch_objecttypes.h"
#include "etch_hash.h"
#include "etch_arraylist.h"
#include <wchar.h>


// current offset from CLASSID_DYNAMIC_START (etchobjtypes.h)
// TODO: make atomic
// TODO: check range
static unsigned short g_etch_curr_classid;

//TODO: make atomic
unsigned short get_dynamic_classid() 
{
    if (g_etch_curr_classid == 0)
        g_etch_curr_classid = CLASSID_DYNAMIC_START;

    return g_etch_curr_classid++;
} 

//TODO: make atomic
unsigned short get_dynamic_classid_unique(unsigned short* globalid) 
{
    if (*globalid == 0) 
        *globalid = get_dynamic_classid();
    return (*globalid);
} 

char* strtrim(char* str)
{
    char *startpos = str;
    {
        // left trim
        if(str != NULL) {
            size_t i = 0;
            size_t l = strlen(str);
            for(i = 0; i < l; i++) {
                if(str[i] == ' ' || str[i] == '\t') {
                    continue;
                }
                startpos = &str[i];
                break;
            }
        }
    }

    {
        // right trim
        if(startpos != NULL) {
            size_t i = 0;
            for(i = strlen(startpos); i > 0; i--) {
                if(startpos[i-1] == ' ' || startpos[i-1] == '\n') {
                    startpos[i-1] = '\0';
                    continue;
                }
                break;
            }
        }
    }
    return startpos;
}

void waitkey()
{
    printf("any key ...\n");
#ifndef _WIN32_WCE    
    while(!getc(stdin)) {}
#else    
    MessageBox(NULL, L"Press any key!", L"Waiting...", NULL);
#endif    
}

/**
 * hexchar_to_int()
 */
int hexchar_to_int (const unsigned char hexchar)
{
    switch(hexchar) 
    {   case '0': return 0;
        case '1': return 1;
        case '2': return 2;
        case '3': return 3;
        case '4': return 4;
        case '5': return 5;
        case '6': return 6;
        case '7': return 7;
        case '8': return 8;
        case '9': return 9;
        case 'a': case 'A': return 10;
        case 'b': case 'B': return 11;
        case 'c': case 'C': return 12;
        case 'd': case 'D': return 13;
        case 'e': case 'E': return 14;
        case 'f': case 'F': return 15;
    }
    return -1;
}


/**
 * hexwchar_to_int()
 */
int hexwchar_to_int (const wchar_t hexwchar)
{
    switch(hexwchar) 
    {   case L'0': return 0;
        case L'1': return 1;
        case L'2': return 2;
        case L'3': return 3;
        case L'4': return 4;
        case L'5': return 5;
        case L'6': return 6;
        case L'7': return 7;
        case L'8': return 8;
        case L'9': return 9;
        case L'a': case L'A': return 10;
        case L'b': case L'B': return 11;
        case L'c': case L'C': return 12;
        case L'd': case L'D': return 13;
        case L'e': case L'E': return 14;
        case L'f': case L'F': return 15;
    }
    return -1;
}


int etch_snwprintf(wchar_t* buffer, size_t count, const wchar_t *format, ...)
{
    va_list args;
    va_start(args, format);
#if defined(_WIN32)
    return _vsnwprintf(buffer, count, format, args);
#elif defined(__APPLE__) || defined(__QNX__) || defined(__LINUX__)
    return vswprintf(buffer, count, format, args);
#else
    return vswprintf(buffer, /*count,*/ format, args);
#endif
}
