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
 * etch_encoding.c -- character encoding
 */

/* we include an APR header only to get required platform specific headers (windows.h or whatever */
#include <apr_thread_cond.h> 
#include "etch_encoding.h"
#include "etch_global.h"
#include "etchlog.h"

#if IS_WINDOWS_ETCH
#include "winnls.h" /* for WideCharToMultiByte etc */
#endif
char* ETCH_ENCODING_LOGID = "ENCO";
char* ETCH_ENCODING_EMASK = "encoding conversion error %d\n";
void etch_encoding_errcheck(const int);


/*
 * etch_unicode_to_8bit()
 * currently windows-specific. todo need portable version.
 * @param unicode_string_in a character string, caller retains ownership.
 * @param which which codepage to use, utf-8 (1) or ansi (0)
 * @return an 8-bit character string, via the out parameter,
 * caller assumes ownership of this memory.
 * @return 0 or -1
 */
int etch_unicode_to_8bit(char** bit8_string_out, wchar_t* unicode_string_in, const which)
{
    const int charcount_in = (int) wcslen(unicode_string_in);
    int   bit8_charcount = 0, bytecount_out = 0, codepage = 0, result = -1;
    char* bit8_buf = NULL;
    if (NULL == bit8_string_out) return -1;

    #if IS_WINDOWS_ETCH  /* todo: portable implementation */

    codepage = which == 1? CP_UTF8: CP_ACP;

    /* call system first to calculate utf-8 buffer size required for 
     * given unicode input and output code page. out buffer length zero 
     * instructs system to do so. */
    bit8_charcount = WideCharToMultiByte(codepage, 0, 
        unicode_string_in, charcount_in, bit8_buf, bit8_charcount, NULL, NULL); 

    bit8_buf = etch_malloc(++bit8_charcount, ETCHTYPEB_BYTES); /* add null term */
    memset(bit8_buf, 0, bit8_charcount);

    bytecount_out = WideCharToMultiByte(codepage, 0, /* do conversion */ 
        unicode_string_in, charcount_in, bit8_buf, bit8_charcount, NULL, NULL); 

    result = bytecount_out > 0? 0: -1;  

    if (result == 0)    /* transfer ownership of out buffer to caller */  
       *bit8_string_out = bit8_buf; 
    else
    {  *bit8_string_out = NULL; 
        etch_free(bit8_buf); bit8_buf = NULL;
    }   
    
    etch_encoding_errcheck(result);

    #endif /* IS_WINDOWS_ETCH */ 

    return result;
}


/*
 * etch_unicode_to_utf8()
 * @param unicode_string_in a character string owned by caller.
 * @return a UTF-8 encoded string, via the out parameter, 
 * caller assumes ownership of this memory.
 * @return 0 or -1 
 */
int etch_unicode_to_utf8(char** utf8_string_out, wchar_t* unicode_string_in)
{
    return etch_unicode_to_8bit(utf8_string_out, unicode_string_in, 1);
}


/*
 * etch_unicode_to_ansi()
 * @param unicode_string_in a character string owned by caller.
 * @return an ascii encoded string, via the out parameter, 
 * caller assumes ownership of this memory, which must be etch_free()'d
 * @return 0 or -1 
 */
int etch_unicode_to_ansi(char** ansi_string_out, wchar_t* unicode_string_in)
{
    return etch_unicode_to_8bit(ansi_string_out, unicode_string_in, 0);
}


/*
 * etch_unicode_to_8bit()
 * currently windows-specific. todo need portable version.
 * @param bit8_string_in a character string owned by caller.
 * @return a unicode string, via the out parameter, 
 * caller assumes ownership of this memory, which must be etch_free()'d
 * @return 0 or -1 
 */
int etch_8bit_to_unicode(wchar_t** unicode_string_out, char* bit8_string_in, const which)
{
    const int charcount_in = (int) strlen(bit8_string_in);
    int unicode_bufsize = 0, charcount_out = 0, codepage = 0, result = -1;
    wchar_t* ubuf = NULL;
    if (NULL == unicode_string_out) return -1;

    #if IS_WINDOWS_ETCH  /* todo: portable implementation */

    codepage = which == 1? CP_UTF8: CP_ACP;

    unicode_bufsize = (charcount_in + 1) * 2; 
    ubuf = etch_malloc(unicode_bufsize, ETCHTYPEB_BYTES); 
    memset(ubuf, 0, unicode_bufsize);

    charcount_out = MultiByteToWideChar(codepage, 0, /* do conversion */ 
        bit8_string_in, charcount_in + 1, ubuf, unicode_bufsize); 

    result = charcount_out > 0? 0: -1;  

    if (result == 0)    /* transfer ownership of out buffer to caller */  
       *unicode_string_out = ubuf; 
    else
    {  *unicode_string_out = NULL; 
        etch_free(ubuf); ubuf = NULL;
    }   

    etch_encoding_errcheck(result);

    #endif /* IS_WINDOWS_ETCH */ 

    return result;
}


/*
 * etch_utf8_to_unicode()
 * @param utf8_string_in a character string owned by caller.
 * @return a UTF-16 encoded string, via the out parameter, 
 * caller assumes ownership of this memory, which must be etch_free()'d
 * @return 0 or -1 
 */
int etch_utf8_to_unicode(wchar_t** unicode_string_out, char* utf8_string_in)
{
    return etch_8bit_to_unicode(unicode_string_out, utf8_string_in, 1);
}


/*
 * etch_ansi_to_unicode()
 * @param ascii_string_in a character string owned by caller.
 * @return a UTF-16 encoded string, via the out parameter, 
 * caller assumes ownership of this memory, which must be etch_free()'d
 */
int etch_ansi_to_unicode(wchar_t** unicode_string_out, char* ascii_string_in)
{
    return etch_8bit_to_unicode(unicode_string_out, ascii_string_in, 0);
}


/*
 * etch_encoding_errcheck()
 * private method to check result of an encoding conversion and log error.
 */
void etch_encoding_errcheck(const int result)
{
    int ecode, whicherr;
    if (result == 0) return;

    #if IS_WINDOWS_ETCH

    ecode = GetLastError();
    switch(whicherr = ecode)
    {   case ERROR_INSUFFICIENT_BUFFER: whicherr = 1; break;
        case ERROR_INVALID_FLAGS:       whicherr = 2; break; 
        case ERROR_INVALID_PARAMETER:   whicherr = 3; break;
    }
    etchlog(ETCH_ENCODING_LOGID,ETCHLOG_ERROR,ETCH_ENCODING_EMASK, whicherr); 

   #endif  
}


/*
 * etch_get_unicode_bytecount()
 * @param widestring a unicode string.
 * @return number of bytes in the string, including the null terminator.
 */
size_t etch_get_unicode_bytecount (wchar_t* widestring)
{
    wchar_t* q = NULL;
    size_t bytecount = 0;
    const int charcount = widestring? (int) wcslen(widestring): 0;
    if (0 == charcount) return 0;
    q = &widestring[charcount];
    q++;  
    bytecount = ((size_t)(char*) q) - ((size_t)(char*) widestring);
    return bytecount;
}



