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

#include <apr_iconv.h>
#include "etch_encoding.h"
#include "etch_log.h"
#include "etch_mem.h"
#include <wchar.h>

static const char* LOG_CATEGORY = "etch_encoding";

extern apr_pool_t*         g_etch_main_pool;
extern apr_thread_mutex_t* g_etch_main_pool_mutex;

static const unsigned char CODEPAGE_TABLE_SIZE = 6;


static apr_iconv_t* codepage_table = NULL;

etch_status_t etch_encoding_initialize()
{
    size_t s       = CODEPAGE_TABLE_SIZE * CODEPAGE_TABLE_SIZE * sizeof(apr_iconv_t);
    codepage_table = etch_malloc(s,0);
    memset(codepage_table, 0, s);
    return ETCH_SUCCESS;
}

etch_status_t etch_encoding_shutdown()
{
    int i = 0;
    
	apr_thread_mutex_lock(g_etch_main_pool_mutex);
    for(i = 0; i < CODEPAGE_TABLE_SIZE * CODEPAGE_TABLE_SIZE; i++) {
        if(codepage_table[i] != NULL) {
	        apr_iconv_close(codepage_table[i], g_etch_main_pool);    
        }
    }
    apr_thread_mutex_unlock(g_etch_main_pool_mutex);
    etch_free(codepage_table);
    codepage_table = NULL;
    return ETCH_SUCCESS;
}


static char* etch_encoding_get_encoding(unsigned char encoding)
{
    switch (encoding) {
    case ETCH_ENCODING_ASCII:
      return "iso-8859-1";
    case ETCH_ENCODING_UTF8:
      return "utf-8";
    case ETCH_ENCODING_UCS2:
      return "ucs2-internal";
    case ETCH_ENCODING_UCS4:
      return "ucs4-internal";
    case ETCH_ENCODING_UTF16:
      return "utf-16";
    default:
      ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "unsupported src-encoding %d\n", encoding);
      ETCH_ASSERT(!"encoding not supported");
      return 0;
    }
}


etch_status_t etch_encoding_get_codepage(unsigned char src, unsigned char dst, apr_iconv_t* codepage)
{
    apr_iconv_t* temp   = NULL;
	apr_status_t status = APR_SUCCESS;
	char*        apr_codepage_src = NULL;
	char*        apr_codepage_dst = NULL;

	apr_codepage_src = etch_encoding_get_encoding(src);
	apr_codepage_dst = etch_encoding_get_encoding(dst);
	if (apr_codepage_src == NULL || apr_codepage_dst == NULL) {
		return ETCH_EINVAL;
    }

	apr_thread_mutex_lock(g_etch_main_pool_mutex);
	temp = &codepage_table[(CODEPAGE_TABLE_SIZE * src) + dst];
	if (*temp == NULL) {
        status = apr_iconv_open(apr_codepage_dst, apr_codepage_src, g_etch_main_pool, temp);
    }
	*codepage = *temp;
	apr_thread_mutex_unlock(g_etch_main_pool_mutex);
	
	if(status != APR_SUCCESS){
		return ETCH_ERROR;
    }
    return ETCH_SUCCESS;
}


int etch_encoding_transcode(char** out, unsigned char outEncoding, const char* in, unsigned char inEncoding, unsigned int inByteCount, int* resultingByteCount, etch_pool_t* pool)
{
    // in
    const char* apr_in_buf = in;
    apr_size_t apr_in_bytecount = inByteCount;
    
    // out
    char* apr_out_buf = NULL;
    apr_size_t apr_out_bytecount = 0;
    apr_size_t apr_out_bytecount_original;

    // translated
    apr_size_t apr_translated = 0;

    // converter
    apr_iconv_t apr_cd   = NULL;

    apr_status_t apr_status = 0;
    etch_status_t etch_status;

    // chech input params
    if (NULL == out) {
        return -1;
    }

    if(pool == NULL) {
        pool = g_etch_main_pool;
    }

    apr_out_bytecount = apr_in_bytecount*4; // worst case ascii->ucs4
    apr_out_bytecount_original = apr_out_bytecount;
    apr_out_buf = etch_malloc(apr_out_bytecount, ETCHTYPEB_BYTES);
    (*out) = apr_out_buf;
    memset(apr_out_buf, 0, apr_out_bytecount);

    if(inEncoding == outEncoding){
        memcpy(apr_out_buf,in,inByteCount);
        return 0;
    }

    // open iconv
    etch_status = etch_encoding_get_codepage(inEncoding, outEncoding, &apr_cd);

    if(etch_status != ETCH_SUCCESS)
    {
        char buffer[512];
        apr_strerror(apr_status, buffer, 512);
        printf("%s", buffer);

        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "encoding conversion error %d\n", apr_status);
        return -1;
    }

    // convert
    apr_status = apr_iconv(apr_cd, &apr_in_buf, &apr_in_bytecount, &apr_out_buf, &apr_out_bytecount, &apr_translated);
    if(apr_status != APR_SUCCESS)
    {
        char buffer[512];
        apr_strerror(apr_status, buffer, 512);
        ETCH_LOG(LOG_CATEGORY, ETCH_LOG_ERROR, "encoding conversion error-code: %d error-msg: %s\n", apr_status, buffer);

        //clean up
        etch_free((*out));
        (*out) = NULL;
        return -1;
    }
    *resultingByteCount = (int)(apr_out_bytecount_original - apr_out_bytecount);

    return 0;
}

unsigned char etch_encoding_for_wchar()
{
  switch (sizeof(wchar_t)) {
  case 2:
    return ETCH_ENCODING_UCS2;
  case 4:
    return ETCH_ENCODING_UCS4;
  default:
    ETCH_ASSERT(!"unknown wchar_t size");
    return 0;
  }
}

int etch_encoding_transcode_wchar(char** out, unsigned char outEncoding, const wchar_t* in, etch_pool_t* pool)
{
  int outByteCount;
  unsigned int charcount;
  unsigned char srcEncoding;
  charcount = (unsigned int)wcslen(in);
  srcEncoding = etch_encoding_for_wchar();
  return etch_encoding_transcode(out, outEncoding, (const char*)in, srcEncoding, charcount * sizeof(wchar_t), &outByteCount, pool);
}

int etch_encoding_transcode_to_wchar(wchar_t** out, const void* in, unsigned char inEncoding, unsigned int inByteCount, etch_pool_t* pool)
{
  int outByteCount;
  unsigned char dstEncoding;
  dstEncoding = etch_encoding_for_wchar();
  return etch_encoding_transcode((char**)out, dstEncoding, in, inEncoding, inByteCount, &outByteCount, pool);
}

unsigned int etch_encoding_get_sizeof_terminator(unsigned char encoding)
{
  switch (encoding) {
  case ETCH_ENCODING_ASCII:
  case ETCH_ENCODING_UTF8:
    return 1;
  case ETCH_ENCODING_UCS2:
    return 2;
  case ETCH_ENCODING_UCS4:
    return 4;
  case ETCH_ENCODING_UTF16:
  case ETCH_ENCODING_UTF32:
  default:
    ETCH_ASSERT(!"encoding not supported");
    return 0;
  }
}
