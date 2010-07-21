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
 * a flex_buffer wraps a byte array and manages the active region of
 * it (0..length). it supports dynamically extending the buffer.
 * 
 * a flexbuffer has an index (read or write cursor). the various get
 * and put operations always occur at the current index, with the index
 * adjusted appropriately afterward. get() will not move the index past
 * length. if put needs to move index past length, length is also
 * adjusted. this may cause the byte array to be re-allocated.
 */

#ifndef ETCH_FLEX_BUFFER_H
#define ETCH_FLEX_BUFFER_H

#include "etch_object.h"

#ifdef __cplusplus
extern "C" {
#endif

#define ETCH_FLEXBUF_PUT_ALL (-1)


typedef struct etch_flexbuffer
{
    etch_object object;

    unsigned char *buf;
    size_t bufsize; /* buffer size */
    size_t datalen; /* data length */
    size_t index;   /* current position */
    unsigned char   is_littleendian; 

} etch_flexbuffer;

etch_flexbuffer *new_flexbuffer(size_t);
etch_flexbuffer *new_flexwriter_from   (void *buf, size_t, size_t);
etch_flexbuffer *new_flexbuffer_from   (void *buf, size_t, size_t, size_t);
etch_flexbuffer *etch_flexbuf_create_b (void *buf, size_t, size_t);
etch_flexbuffer *etch_flexbuf_create_bi(void *buf, size_t, size_t, size_t);
etch_flexbuffer *etch_flexbuf_skip     (etch_flexbuffer*,  size_t, int);

byte*   etch_flexbuf_get_buffer(etch_flexbuffer*);
int     destroy_etch_flexbuffer(void*);
int     etch_flexbuf_set_length(etch_flexbuffer*, size_t);
int     etch_flexbuf_set_index (etch_flexbuffer*, size_t);
void    etch_flexbuf_clear     (etch_flexbuffer*);
size_t  etch_flexbuf_avail     (etch_flexbuffer*);
int     etch_flexbuffer_reset_to     (etch_flexbuffer*, size_t);
etch_flexbuffer *etch_flexbuf_compact(etch_flexbuffer*);
etch_flexbuffer *etch_flexbuf_reset  (etch_flexbuffer*);

size_t etch_flexbuf_get (etch_flexbuffer*, byte*, size_t, size_t);

int    etch_flexbuf_get_byte  (etch_flexbuffer*, byte* out);
int    etch_flexbuf_get_short (etch_flexbuffer*, short* out);
int    etch_flexbuf_get_int   (etch_flexbuffer*, int* out);
int    etch_flexbuf_get_long  (etch_flexbuffer*, int64* out);
int    etch_flexbuf_get_float (etch_flexbuffer*, float* out);
int    etch_flexbuf_get_double(etch_flexbuffer*, double* out);

size_t etch_flexbuf_get_fully  (etch_flexbuffer*, byte*, size_t);
byte*  etch_flexbuf_get_all    (etch_flexbuffer*, size_t* out_count);
byte*  etch_flexbuf_get_allfrom(etch_flexbuffer*, size_t, size_t* out_count);

size_t etch_flexbuf_put        (etch_flexbuffer*, byte*, size_t, size_t);
size_t etch_flexbuf_put_from   (etch_flexbuffer*, etch_flexbuffer*, size_t);
size_t etch_flexbuf_put_byte   (etch_flexbuffer*, byte   value);
size_t etch_flexbuf_put_short  (etch_flexbuffer*, short  value);
size_t etch_flexbuf_put_int    (etch_flexbuffer*, int    value);
size_t etch_flexbuf_put_long   (etch_flexbuffer*, int64  value);
size_t etch_flexbuf_put_float  (etch_flexbuffer*, float  value);
size_t etch_flexbuf_put_double (etch_flexbuffer*, double value);

#ifdef __cplusplus
}
#endif

#endif /* ETCH_FLEX_BUFFER_H */
