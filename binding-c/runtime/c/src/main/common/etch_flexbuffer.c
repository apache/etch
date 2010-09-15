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
 * etchflexbuf.c
 * flex buffer
 */

#include "etch_flexbuffer.h"
#include "etch_objecttypes.h"
#include "etch_log.h"
#include "etch_mem.h"

#define ETCH_INIT_FLEXBUFSIZE 2048
#define ETCH_MAX_FLEXBUFSIZE (4*1024*1024)

static void    etch_flexbuf_fix_length  (etch_flexbuffer*);
static int etch_flexbuf_ensure_size (etch_flexbuffer*, size_t);

/* 
 * java binding syntax is confusing and ambiguous so we change it a bit. java 
 * calls the data region in the buffer "length". since "length" is non-specific,
 * and there are other "length"s, such as "buffer.length", and "object.length", 
 * we'll call it "datalen", to mean "meaningful bytes within the buffer". java
 * also uses "buffer.length", which is the size of the allocated buffer. c of
 * course does not associate properties with arrays, so we carry an additional
 * property "bufsize" to indicate the allocated length of the byte array.  
 */


/**
 * new_flexbuffer()
 * etch_flexbuffer constructor
 */
etch_flexbuffer *new_flexbuffer (size_t bufsize)
{
    void *buf;
    if (bufsize <= 0 || bufsize > ETCH_MAX_FLEXBUFSIZE) bufsize = ETCH_INIT_FLEXBUFSIZE;

    buf = etch_malloc(bufsize, ETCHTYPEB_BYTES);
    memset(buf, 0, bufsize);
   
    return etch_flexbuf_create_bi (buf, bufsize, 0, 0);
}


/**
 * new_flexwriter_from()
 * etch_flexbuffer constructor with index set to write
 */
etch_flexbuffer *new_flexwriter_from (void *buf, size_t datalen, size_t bufsize) 
{
    return new_flexbuffer_from (buf, datalen, bufsize, datalen);       
}


/**
 * new_flexbuffer_from()
 * etch_flexbuffer constructor.
 * @param buf data which is to become the internal buffer. caller relinquishes this memory.
 * @param datalen length of real data in bytes.
 * @param bufsize size of buffer in bytes.  
 * @param buffer index at which to start.
 */
etch_flexbuffer *new_flexbuffer_from (void* buf, size_t datalen, size_t bufsize, size_t index)
{
    if (bufsize <= 0 || bufsize > ETCH_MAX_FLEXBUFSIZE) bufsize = ETCH_INIT_FLEXBUFSIZE;
    if (datalen > bufsize || (!buf && datalen)) return NULL;
    if (NULL == buf) 
    {   buf = etch_malloc(bufsize, ETCHTYPEB_BYTES);
        memset(buf, 0, bufsize);
    }
   
    return etch_flexbuf_create_bi (buf, bufsize, datalen, index);
}


/**
 * etch_flexbuf_create_b()
 * create a flex buffer out of an existing buffer, ready to read to, 
 * using specified size and an index of zero.
 *
 * @param buf the existing buffer.
 * @param bufsize the buffer size.
 * @param datalen the data length in the buffer
 *
 * @return the created and initialized the flex buffer.
 *
 */
etch_flexbuffer *etch_flexbuf_create_b (void *buf, size_t bufsize, size_t datalen)
{
    return etch_flexbuf_create_bi(buf, bufsize, datalen, 0);
}


/**
 * etch_flexbuf_create_bi()
 * create a flex buffer out of an existing buffer, ready to read with specified 
 * index and size.
 * @param buf the existing buffer.
 * @param bufsize the buffer size.
 * @param datalen the data length in the buffer
 * @param index current position of the buffer.
 */
etch_flexbuffer *etch_flexbuf_create_bi(void *buf, size_t bufsize, size_t datalen, size_t index)
{
    etch_flexbuffer *fbuf = (etch_flexbuffer*) new_object(sizeof(etch_flexbuffer), ETCHTYPEB_FLEXBUF, CLASSID_FLEXBUF);

    ((etch_object*)fbuf)->destroy = destroy_etch_flexbuffer;

    fbuf->buf     = buf;
    fbuf->bufsize = bufsize;
    fbuf->datalen = datalen;
    fbuf->index   = index;
    fbuf->is_littleendian = IS_ETCH_TRANSPORT_LITTLEENDIAN;
    return fbuf;
}


/**
 * etch_flexbuf_get_buffer()
 * @return the current byte array. might change if any operation
 * needs to extend length past the end of the array.
 */
byte* etch_flexbuf_get_buffer (etch_flexbuffer *fbuf)
{
	return fbuf->buf;
}


/**
 * etch_flexbuf_clear()
 * zero fill the internal buffer
 */
void etch_flexbuf_clear (etch_flexbuffer *fbuf)
{
	memset (fbuf->buf, 0, fbuf->bufsize);
}


/**
 * etch_flexbuf_ensure_size().
 * verify sufficient buffer capacity, reallocating if necessary.
 * @return boolean value indicating if buffer is as requested.
 */
static int etch_flexbuf_ensure_size (etch_flexbuffer *fbuf, size_t reqsize)
{
    byte *newbuf = NULL;
    size_t newsize = fbuf->bufsize;
	if (reqsize  < ETCH_INIT_FLEXBUFSIZE)
		reqsize  = ETCH_INIT_FLEXBUFSIZE;
	if (reqsize <= newsize) return TRUE;	
    if (reqsize  > ETCH_MAX_FLEXBUFSIZE) return FALSE;
	
	while(reqsize > newsize) newsize += ETCH_INIT_FLEXBUFSIZE;
	newbuf = etch_realloc (fbuf->buf, newsize, 0);
	
    fbuf->buf = newbuf;
    fbuf->bufsize = newsize; 
    return TRUE;
}


/**
 * destroy_etch_flexbuffer()
 * etch_flexbuffer destructor
 */
int destroy_etch_flexbuffer(void* obj)
{
    etch_flexbuffer* fbuf = (etch_flexbuffer*)obj;
    if (fbuf == NULL) return 0; 
    
    if (!is_etchobj_static_content(fbuf))
        etch_free(fbuf->buf);  /* OK if null */

    destroy_objectex((etch_object*) fbuf);
    return 0;
}


/**
 * sets a new data length. If the index is larger than new length, 
 * the index is set to the new length as well. 
 * the method name was retained from java, however it is more properly 
 * set_data_length, since as noted previously, "length" is ambiguous,
 * referring to data length, not to allocated bytes. 
 */
int etch_flexbuf_set_length (etch_flexbuffer *fbuf, size_t new_datalen)
{
    int result = 0;

    if (new_datalen >= 0 && etch_flexbuf_ensure_size(fbuf, new_datalen))
    {   
        fbuf->datalen = new_datalen;  

        if (fbuf->index > new_datalen)
            fbuf->index = new_datalen;
    }
    else result = -1;

   return result;
}


/**
 * sets a new buffer index. the index must be greater than 0 and less
 * than the buffer size
 * @param fbuf the buffer pointer to be set.
 * @param index the new index for the buffer.
 */
int etch_flexbuf_set_index (etch_flexbuffer *fbuf, size_t index)
{
    if (index < 0 || index > fbuf->datalen) return -1;
    fbuf->index = index;
    return 0;
}


/**
 * get the number of bytes available in the buffer.
 * @return the available space in the buffer.
 */
size_t etch_flexbuf_avail (etch_flexbuffer *fbuf)
{
    return (fbuf->datalen - fbuf->index);
}


/**
 * etch_flexbuffer_reset_to()
 * set index to zero and length to specified length.
 */
int etch_flexbuffer_reset_to (etch_flexbuffer *fbuf, size_t new_datalen)
{
    const int result = etch_flexbuf_set_length (fbuf, new_datalen);
    fbuf->index = 0;
    return result;
}


/**
 * etch_flexbuf_reset()
 * set index and length to zero, same as etch_flexbuf_set_index(buf, 0)
 */
etch_flexbuffer *etch_flexbuf_reset (etch_flexbuffer *fbuf)
{
    fbuf->index = fbuf->datalen = 0;
	return fbuf;
}


/**
 * Compacts the buffer by moving remaining data (from index to length)
 * to the front of the buffer. Sets index to 0, and sets length to
 * avail (before index was changed).
 * @return this flex buffer object.
 */
etch_flexbuffer *etch_flexbuf_compact(etch_flexbuffer *fbuf)
{
    size_t curlen;
	if (fbuf->index == 0) return fbuf;
	
	if (0 == (curlen = etch_flexbuf_avail(fbuf)))
	{
		etch_flexbuf_set_length(fbuf, 0);
		return fbuf;
	}
	
    memmove(fbuf->buf, fbuf->buf+fbuf->index, curlen);
	fbuf->index = 0;
	fbuf->datalen = curlen;
	
	return fbuf;
}


/**
 * Copies data from the internal buffer to buf.
 *
 * @param fbuf the flex buffer to get data from.
 * @param buf a buffer to receive the data. At most
 * min( len, avail() ) bytes are transferred, starting
 * at off.
 * @param off the index in buf to receive the data.
 * Off must be >= 0 && <= buf.datalen.
 * @param len the max amount of data to transfer. Len
 * must be >= 0 and <= buf.datalen - off.
 * @return the amount of data transferred.
 */
size_t etch_flexbuf_get(etch_flexbuffer *fbuf, byte *buf, size_t off, size_t len)
{
    size_t bytecount = 0, avail = etch_flexbuf_avail(fbuf);
	if (len <= 0 || NULL == buf) return 0;

	bytecount = len < avail ? len : avail ;	
                     /* changed 6/16 to add index */
    memcpy(buf + off, fbuf->buf + fbuf->index + off, bytecount);
	fbuf->index += bytecount;
	
	return bytecount;
}


/**
 * etch_flexbuf_get_allfrom()
 * return buffer contents starting at index, in a byte vector, caller owns returned memory
 */
byte* etch_flexbuf_get_allfrom(etch_flexbuffer* fbuf, size_t index, size_t* out_count)
{
    byte* newbuf = 0;
    size_t bytecount = 0;
    if (index < 0) index = fbuf->index;
    if (index > fbuf->datalen) index = fbuf->datalen;
    fbuf->index = index;  /* 6/16 so etch_flexbuf_get can skip index */

    bytecount = fbuf->datalen - index;  
    newbuf    = etch_malloc(bytecount, ETCHTYPEB_BYTES);

    bytecount = etch_flexbuf_get(fbuf, newbuf, 0, bytecount);

    if (out_count) *out_count = bytecount; 
    return newbuf;
}


/**
 * etch_flexbuf_get_all()
 * return buffer contents in a byte array, caller owns memory
 */
byte* etch_flexbuf_get_all(etch_flexbuffer* fbuf, size_t* out_count)
{
    return etch_flexbuf_get_allfrom(fbuf, fbuf->index, out_count); 
}


/**
 * return next byte in buffer
 */
int etch_flexbuf_get_byte(etch_flexbuffer *fbuf, byte* out)
{
   if (etch_flexbuf_avail(fbuf) < sizeof(byte)) return -1;  
   *out = fbuf->buf[fbuf->index++];
   return 0;
}

/**
 * etch_flexbuf_get_short()
 * @return a short composed from the next 2 bytes.
 */
int etch_flexbuf_get_short(etch_flexbuffer *fbuf, short* out)
{   
    int value, svalue;
    if (etch_flexbuf_avail(fbuf) < sizeof(short)) return -1;

	if (fbuf->is_littleendian)
	{
		value = fbuf->buf[fbuf->index++] & 255;
		svalue = (short) (value + ((fbuf->buf[fbuf->index++] & 255) << 8));
	}
	else
    {   value  = fbuf->buf[fbuf->index++]; /* big endian */
	    svalue = (short) ((value << 8) + (fbuf->buf[fbuf->index++] & 255));
    }

    *out = svalue;
    return 0;
}


/**
 * etch_flexbuf_get_int()
 * @return an int composed from the next 4 bytes.
 */
int etch_flexbuf_get_int(etch_flexbuffer *fbuf, int32* out)
{
    int32  value        = 0;
    size_t bytes_avail  = 0;

    bytes_avail = etch_flexbuf_avail(fbuf);
    if(bytes_avail < sizeof(int32))
    {
        return -1;
    }

    if(fbuf->is_littleendian)
    {
        value = value | fbuf->buf[fbuf->index+0] << 0;
        value = value | fbuf->buf[fbuf->index+1] << 8;
        value = value | fbuf->buf[fbuf->index+2] << 16;
        value = value | fbuf->buf[fbuf->index+3] << 24;
        fbuf->index += sizeof(int32);
    }
    else
    {
        value = value | fbuf->buf[fbuf->index+3] << 0;
        value = value | fbuf->buf[fbuf->index+2] << 8;
        value = value | fbuf->buf[fbuf->index+1] << 16;
        value = value | fbuf->buf[fbuf->index+0] << 24;
        fbuf->index += sizeof(int32);
    }
    *out = value;
    return 0;
}

/**
 * etch_flexbuf_get_long()
 * @return a long composed from the next 8 bytes.
 * note jim are we taking sign into consideration on reversal?
 */
int etch_flexbuf_get_long(etch_flexbuffer *fbuf, int64* out)
{
    int64 value         = 0;
    size_t bytes_avail  = 0;

    bytes_avail = etch_flexbuf_avail(fbuf);
    if(bytes_avail < sizeof(int64))
    {
        return -1;
    }

    if (fbuf->is_littleendian)
    {
        // little endian
        value = value | (uint64)fbuf->buf[fbuf->index+0] << 0;
        value = value | (uint64)fbuf->buf[fbuf->index+1] << 8;
        value = value | (uint64)fbuf->buf[fbuf->index+2] << 16;
        value = value | (uint64)fbuf->buf[fbuf->index+3] << 24;
        value = value | (int64)fbuf->buf[fbuf->index+4] << 32;
        value = value | (int64)fbuf->buf[fbuf->index+5] << 40;
        value = value | (int64)fbuf->buf[fbuf->index+6] << 48;
        value = value | (int64)fbuf->buf[fbuf->index+7] << 56;
        fbuf->index += sizeof(int64);
    }
    else
    {
        // big endian
        value = value | (uint64)fbuf->buf[fbuf->index+7] << 0;
        value = value | (uint64)fbuf->buf[fbuf->index+6] << 8;
        value = value | (uint64)fbuf->buf[fbuf->index+5] << 16;
        value = value | (uint64)fbuf->buf[fbuf->index+4] << 24;
        value = value | (uint64)fbuf->buf[fbuf->index+3] << 32;
        value = value | (uint64)fbuf->buf[fbuf->index+2] << 40;
        value = value | (uint64)fbuf->buf[fbuf->index+1] << 48;
        value = value | (uint64)fbuf->buf[fbuf->index+0] << 56;
        fbuf->index += sizeof(int64);
    }
    *out = value;
    return 0;
}


/**
 * etch_flexbuf_get_float()
 * @return a float from the next available bytes.
 */
int etch_flexbuf_get_float(etch_flexbuffer *fbuf, float* out)
{
    float value;
    if (-1 == etch_flexbuf_get_int(fbuf, (int*) &value)) return -1;
    *out = value;
    return 0;
}


/**
 * etch_flexbuf_get_double()
 * @return a double from the next available bytes.
 */
int etch_flexbuf_get_double(etch_flexbuffer *fbuf, double* out)
{
    double value;
    if (-1 == etch_flexbuf_get_long(fbuf, (int64*) &value)) return -1;
    *out = value;
    return 0;
}



/**
 * fills a buffer fully from the next available bytes.
 * @param b
 * @throws IOException if avail() < b.datalen.
 */
size_t etch_flexbuf_get_fully( etch_flexbuffer *fbuf, byte *b, size_t bufsize )
{
    return etch_flexbuf_get( fbuf, b, 0, bufsize );
}


/**
 * If index has moved past length during a put, then adjust length
 * to track index.
 */
static void etch_flexbuf_fix_length(etch_flexbuffer *fbuf)
{
	if (fbuf->index > fbuf->datalen)
		fbuf->datalen = fbuf->index;
}


/**
 * Puts some bytes into the buffer as if by repeated calls to put().
 * @param buf the source of the bytes to put.
 * @param off the index to the first byte to put.
 * @param bytecount the number of bytes to put.
 * @return count of bytes put
 */
size_t etch_flexbuf_put (etch_flexbuffer *fbuf, byte *buf, size_t off, size_t bytecount)
{
	if (bytecount <= 0) return 0;	
	if (!etch_flexbuf_ensure_size (fbuf, fbuf->index + bytecount)) return 0;

    memcpy(fbuf->buf + fbuf->index, buf + off, bytecount);
	fbuf->index += bytecount;
	etch_flexbuf_fix_length (fbuf);	
	return bytecount;
}


/**
 * Copies the specified number of bytes from buf[index] into buffer
 * as if by repeated execution of put( buf.get() ).
 * @param buf the source of the bytes to put.
 * @param len the number of bytes to put. if -1, copy everything.  
 * @return number of bytes put
 */
size_t etch_flexbuf_put_from(etch_flexbuffer *dstfb, etch_flexbuffer *srcfb, size_t bytecount)
{
    size_t bytes_put = 0;
    if (bytecount == -1) 
        bytecount = srcfb->datalen - srcfb->index;

	bytes_put = etch_flexbuf_put (dstfb, srcfb->buf, srcfb->index, bytecount);

	etch_flexbuf_skip (srcfb, bytes_put, FALSE);

	return bytes_put;
}


/**
 * etch_flexbuf_put_byte()
 * @return number of bytes put
 */
size_t etch_flexbuf_put_byte (etch_flexbuffer *fbuf, byte value)
{
    size_t bytes_put = 0;

	if (etch_flexbuf_ensure_size(fbuf, fbuf->index + 1))
    {
        fbuf->buf[fbuf->index++] = value;
    	etch_flexbuf_fix_length(fbuf);
        bytes_put = sizeof(value);
    }

    return bytes_put;
}


/**
 * etch_flexbuf_put_short()
 */
size_t etch_flexbuf_put_short(etch_flexbuffer *fbuf, short value)
{
    if (!etch_flexbuf_ensure_size( fbuf, fbuf->index + sizeof(value) ))
        return 0;

	if (fbuf->is_littleendian)
	{
		fbuf->buf[fbuf->index++] = (byte) value;
		fbuf->buf[fbuf->index++] = (byte) (value >> 8);
	}
	else
	{
		fbuf->buf[fbuf->index++] = (byte) (((unsigned short)value) >> 8);
		fbuf->buf[fbuf->index++] = (byte) value;
	}
	
	etch_flexbuf_fix_length(fbuf);

    return sizeof(value);
}


/**
 * etch_flexbuf_put_int()
 */
size_t etch_flexbuf_put_int( etch_flexbuffer *fbuf, int value )
{
    if (!etch_flexbuf_ensure_size( fbuf, fbuf->index + sizeof(int) ))
        return 0;

    if (fbuf->is_littleendian)
	{
		fbuf->buf[fbuf->index++] = (byte) value;
		fbuf->buf[fbuf->index++] = (byte) (value >> 8);
		fbuf->buf[fbuf->index++] = (byte) (value >> 16);
		fbuf->buf[fbuf->index++] = (byte) (value >> 24);
	}
	else
	{
		fbuf->buf[fbuf->index++] = (byte) ( ((unsigned int) value) >> 24);
		fbuf->buf[fbuf->index++] = (byte) ( ((unsigned int) value) >> 16);
		fbuf->buf[fbuf->index++] = (byte) ( ((unsigned int) value) >> 8);
		fbuf->buf[fbuf->index++] = (byte) value;
	}
	
	etch_flexbuf_fix_length(fbuf);

    return sizeof(value);
}


/**
 * etch_flexbuf_put_long()
 */
size_t etch_flexbuf_put_long(etch_flexbuffer *fbuf, int64 value )
{
    if (!etch_flexbuf_ensure_size( fbuf, fbuf->index + sizeof(value) ))
        return 0;

	if (fbuf->is_littleendian)
	{
		fbuf->buf[fbuf->index++] = (byte) value;
		fbuf->buf[fbuf->index++] = (byte) (value >> 8);
		fbuf->buf[fbuf->index++] = (byte) (value >> 16);
		fbuf->buf[fbuf->index++] = (byte) (value >> 24);
		fbuf->buf[fbuf->index++] = (byte) (value >> 32);
		fbuf->buf[fbuf->index++] = (byte) (value >> 40);
		fbuf->buf[fbuf->index++] = (byte) (value >> 48);
		fbuf->buf[fbuf->index++] = (byte) (value >> 56);
	}
	else
	{
		fbuf->buf[fbuf->index++] = (byte) ( ((uint64) value ) >> 56);
		fbuf->buf[fbuf->index++] = (byte) (( (uint64) value ) >> 48);
		fbuf->buf[fbuf->index++] = (byte) (( (uint64) value ) >> 40);
		fbuf->buf[fbuf->index++] = (byte) (( (uint64) value ) >> 32);
		fbuf->buf[fbuf->index++] = (byte) (( (uint64) value ) >> 24);
		fbuf->buf[fbuf->index++] = (byte) (( (uint64) value ) >> 16);
		fbuf->buf[fbuf->index++] = (byte) (( (uint64) value ) >> 8);
		fbuf->buf[fbuf->index++] = (byte) value;
	}
	
	etch_flexbuf_fix_length(fbuf);

    return sizeof(value);
}


/**
 * etch_flexbuf_put_float()
 */
size_t etch_flexbuf_put_float( etch_flexbuffer *fbuf, float value )
{
	/* return etch_flexbuf_put_int( fbuf, * ((int *)( &value )) ); */
    const unsigned int u = * ( (int*)&value );
    return etch_flexbuf_put_int(fbuf, u); 
}


/**
 * etch_flexbuf_put_double()
 */
size_t etch_flexbuf_put_double(etch_flexbuffer *fbuf, double value )
{
	/* return etch_flexbuf_put_long( fbuf, *( (int64*) &value ) ); */
    const int64 u = * ( (int64*)&value );
    return etch_flexbuf_put_long(fbuf, u);
}


/**
 * Adjusts index as if by a get or put but without transferring
 * any data. This could be used to skip over a data item in an
 * input or output buffer.
 * 
 * @param len the number of bytes to skip over. Len must be
 * >= 0. If put is false, it is an error if len > avail().
 * If put is true, the buffer may be extended (and the buffer
 * length adjusted).
 * 
 * @param put if true it is ok to extend the length of the
 * buffer.
 * 
 * @return this flex buffer object.
 * 
 * @throws EOFException if put is false and len > avail().
 * 
 * @throws IOException if the max buffer size is exceeded.
 */
etch_flexbuffer *etch_flexbuf_skip(etch_flexbuffer *fbuf, size_t len, int put )
{
	if (len  < 0) return NULL;	
	if (len == 0) return fbuf;
	
	if (put)
	{
		etch_flexbuf_ensure_size( fbuf, fbuf->index+len );
		fbuf->index += len;
		etch_flexbuf_fix_length(fbuf);
		return fbuf;
	}
	
	fbuf->index += len;	
	return fbuf;
}
