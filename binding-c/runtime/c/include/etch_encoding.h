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
 * etch_encoding.h -- character encoding
 */

#ifndef _ETCH_ENCODING_H_
#define _ETCH_ENCODING_H_

#include "etch.h"
#include "etch_mem.h"
#include "etch_errno.h"

#ifdef __cplusplus
extern "C" {
#endif

etch_status_t etch_encoding_initialize();    

etch_status_t etch_encoding_shutdown();

/**
 * transcode a bytestream from encoding inEncoding to outEncoding
 * @param out must be freed by the caller via etch_free
 */
int etch_encoding_transcode(char** out, unsigned char outEncoding, const char* in, unsigned char inEncoding, unsigned int inByteCount, int* outByteCount, etch_pool_t* pool);

int etch_encoding_transcode_wchar(char** out, unsigned char outEncoding, const wchar_t* in, etch_pool_t* pool);

int etch_encoding_transcode_to_wchar(wchar_t** out, const void* in, unsigned char inEncoding, unsigned int inByteCount, etch_pool_t* pool);

unsigned char etch_encoding_for_wchar();

unsigned int etch_encoding_get_sizeof_terminator(unsigned char encoding);

#ifdef __cplusplus
}
#endif

#endif /* #ifndef ETCH_ENCODING_H */
