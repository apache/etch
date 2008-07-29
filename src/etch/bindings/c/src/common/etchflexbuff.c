#include "etchflexbuff.h"
#include "objtypes.h"
#include "etchmem.h"
#include "etchlog.h"

#define INIT_BUFFER_SIZE     32
#define MAX_BUFFER_SIZE      (4*1024*1024)

#define LITTLE_ENDIAN        1

static void etch_flex_buffer_fix_length(etch_flex_buffer *efb_ptr);
static BOOLEAN etch_flex_buffer_ensure_size( etch_flex_buffer *efb_ptr, size_t size );

/**
 * Create a flex buffer with default initial size and set index to 0.
 *
 * @return the created and initialized the flex buffer.
 *
 */
etch_flex_buffer *etch_flex_buffer_create(void)
{
    return etch_flex_buffer_create_s(INIT_BUFFER_SIZE);
}

/**
 * Create a flex buffer with given size and set index to 0.
 *
 * @param size the buffer size.
 *
 * @return the created and initialized the flex buffer.
 *
 */
etch_flex_buffer *etch_flex_buffer_create_s(size_t size)
{
    void *buffer_ptr;
    /* dont do 0 size buffer and limit max size to be 4 meg */
    if (size <= 0 || size > MAX_BUFFER_SIZE)
    {
        etchlog_report("etch_flex_buffer", ETCHLOG_ERROR, "Invalid size(%d) to create etch_flex_buffer.", size);
        return NULL;
    }

    buffer_ptr = etch_malloc(size, 0);
    if (buffer_ptr == NULL)
    {
        etchlog_report("etch_flex_buffer", ETCHLOG_ERROR, "Failed to malloc memory to create etch_flex_buffer.");
        return NULL;
    }
    return etch_flex_buffer_create_bi(buffer_ptr, size, 0, 0);
}


/**
 * Create a flex buffer out of an existing buffer, ready to read with  
 * specified size and index of 0.
 *
 * @param buffer_ptr the existing buffer.
 * @param size the buffer size.
 * @param length the data length in the buffer
 *
 * @return the created and initialized the flex buffer.
 *
 */
etch_flex_buffer *etch_flex_buffer_create_b(void *buffer_ptr, size_t size, size_t length)
{
    return etch_flex_buffer_create_bi(buffer_ptr, size, length, 0);
}

/**
 * Create a flex buffer out of an existing buffer, ready to read with specified 
 * index and size.
 *
 * @param buffer_ptr the existing buffer.
 * @param size the buffer size.
 * @param length the data length in the buffer
 * @param index current position of the buffer.
 *
 * @return the created and initialized the flex buffer.
 *
 * Note: the semantics of size is different from java version. Here size is the 
 *       total length of the existing buffer, not just valid data length contained
 *       in the buffer.
 */
etch_flex_buffer *etch_flex_buffer_create_bi(void *buffer_ptr, size_t size, size_t length, int index)
{
    etch_flex_buffer *efb_ptr = etch_malloc(sizeof(etch_flex_buffer), 0);
    if (efb_ptr == NULL)
    {
        etchlog_report("etch_flex_buffer", ETCHLOG_ERROR, "Failed to malloc memory to create etch_flex_buffer.");
        return NULL;
    }

    efb_ptr->buffer_ptr = buffer_ptr;
    efb_ptr->size = size;
    efb_ptr->length = length;
    efb_ptr->index = index;

    return efb_ptr;    
}

/**
 * @return the current byte array. Might change if any operation
 * needs to extend length past the end of the array.
 */
byte* etch_flex_buffer_get_buffer(etch_flex_buffer *efb_ptr)
{
	return efb_ptr->buffer_ptr;
}


static BOOLEAN etch_flex_buffer_ensure_size( etch_flex_buffer *efb_ptr, size_t size )
{
    byte *buffer_ptr;
    size_t k;
	size_t n = efb_ptr->size;
	if (size <= n)
		return TRUE;
	
	/* the buffer is not big enough, we have to expand it. */
	
	k = n;
	if (k < INIT_BUFFER_SIZE)
		k = INIT_BUFFER_SIZE;
	

    /* double the buffer size until it is big enough */
	while (size > k && k < MAX_BUFFER_SIZE)
		k = k << 1;
	
	if (size > k)
    {
        etchlog_report("etch_flex_buffer", ETCHLOG_ERROR, "buffer overflow (size=%d)", size );
        return FALSE;
    }

	buffer_ptr = etch_realloc(efb_ptr->buffer_ptr, k, 0);
    if (buffer_ptr == NULL)
    {
        etchlog_report("etch_flex_buffer", ETCHLOG_ERROR, "Failed to expand flex buffer from %d to %d", n, k);
        return FALSE;
    }

    efb_ptr->buffer_ptr = buffer_ptr;
    efb_ptr->size = k;
    
    return TRUE;
}

/**
 *  Destroy the etch flex buffer and free all the memory associated with the buffer.
 *
 * @param efb_ptr the pointer to the buffer to be destroyed.
 *
 */
void etch_flex_buffer_destroy(etch_flex_buffer *efb_ptr)
{
    if (efb_ptr == NULL)
        return;     /* nothing to do */

    if (efb_ptr->buffer_ptr != NULL)
        etch_free(efb_ptr->buffer_ptr);

    etch_free(efb_ptr);
}

/**
 * Sets the new size of the buffer. If the index is larger than new length, the index
 * is set to the new size as well. 
 *
 * @param efb_ptr the buffer to be changed.
 * @param len the new length for the buffer. It has be to greater than 0.
 *
 * @return true if the buffer is set to new size. otherwise false.
 *
 */
BOOLEAN etch_flex_buffer_set_length(etch_flex_buffer *efb_ptr, size_t len)
{
    BOOLEAN rc;

    if (len < 0)
        return FALSE;

    rc = etch_flex_buffer_ensure_size(efb_ptr, len);
    if (rc)
    {   
        efb_ptr->length = len;
        if (efb_ptr->index > len)
            efb_ptr->index = len;
    }

    return rc;
}

/**
 * Sets the new index of the buffer. The index must be greater than 0 and less
 * than the buffer size.
 *
 * @param efb_ptr the buffer pointer to be set.
 * @param index the new index for the buffer.
 *
 * @return true if the index is set. otherwise false.
 *
 */
BOOLEAN etch_flex_buffer_set_index(etch_flex_buffer *efb_ptr, size_t index)
{
    if (index < 0 || index > efb_ptr->length)
        return FALSE;

    efb_ptr->index = index;

    return TRUE;
}

/**
 * Get the number of bytes available in the buffer.
 *
 * @return the available space in the buffer.
 *
 */
size_t etch_flex_buffer_avail(etch_flex_buffer *efb_ptr)
{
    return (efb_ptr->length - efb_ptr->index);
}

/**
 * Compacts the buffer by moving remaining data (from index to length)
 * to the front of the buffer. Sets index to 0, and sets length to
 * avail (before index was changed).
 * 
 * @return this flex buffer object.
 */
etch_flex_buffer *etch_flex_buffer_compact(etch_flex_buffer *efb_ptr)
{
    size_t n;

	if (efb_ptr->index == 0)
		return efb_ptr;
	
	n = etch_flex_buffer_avail(efb_ptr);
	if (n == 0)
	{
		etch_flex_buffer_set_length(efb_ptr, 0);
		return efb_ptr;
	}
	
    memmove(efb_ptr->buffer_ptr, efb_ptr->buffer_ptr+efb_ptr->index, n);
	efb_ptr->index = 0;
	efb_ptr->length = n;
	
	return efb_ptr;
}

/**
 * Copies data from the internal buffer to buf.
 *
 * @param efb_ptr the flex buffer to get data from.
 * @param buf a buffer to receive the data. At most
 * min( len, avail() ) bytes are transferred, starting
 * at off.
 * @param off the index in buf to receive the data.
 * Off must be >= 0 && <= buf.length.
 * @param len the max amount of data to transfer. Len
 * must be >= 0 and <= buf.length - off.
 * @return the amount of data transferred.
 */
size_t etch_flex_buffer_get( etch_flex_buffer *efb_ptr, byte *buf, size_t off, size_t len )
{
    size_t n;

	if (len == 0)
		return 0;

	n = min( len, etch_flex_buffer_avail(efb_ptr) );
	
    memcpy(buf+off, efb_ptr->buffer_ptr+efb_ptr->index, n);
	efb_ptr->index += n;
	
	return n;
}

/**
 * Return the next byte in buffer. Caller should check and make sure
 * there is data in the buffer before calling this.
 *
 * @return the next available byte.
 */
byte etch_flex_buffer_get_byte(etch_flex_buffer *efb_ptr)
{
	return efb_ptr->buffer_ptr[efb_ptr->index++];
}

/**
 * @return a short composed from the next 2 bytes. Little-endian.
 * @throws IOException if avail() < 2.
 */
short etch_flex_buffer_get_short(etch_flex_buffer *efb_ptr)
{   
    int value;

	if (LITTLE_ENDIAN)
	{
		// little-endian
		value = efb_ptr->buffer_ptr[efb_ptr->index++] & 255;
		return (short) (value + ((efb_ptr->buffer_ptr[efb_ptr->index++] & 255) << 8));
	}
	
	// big-endian
	value = efb_ptr->buffer_ptr[efb_ptr->index++];
	return (short) ((value << 8) + (efb_ptr->buffer_ptr[efb_ptr->index++] & 255));
}

/**
 * @return an int composed from the next 4 bytes. Little-endian.
 * @throws IOException if avail() < 4.
 */
int etch_flex_buffer_get_int(etch_flex_buffer *efb_ptr)
{
    int value;
	if (LITTLE_ENDIAN)
	{
		// little-endian
		value = efb_ptr->buffer_ptr[efb_ptr->index++] & 255;
		value += ((efb_ptr->buffer_ptr[efb_ptr->index++] & 255) << 8);
		value += ((efb_ptr->buffer_ptr[efb_ptr->index++] & 255) << 16);
		return value + ((efb_ptr->buffer_ptr[efb_ptr->index++] & 255) << 24);
	}
	
	// big-endian
	value = efb_ptr->buffer_ptr[efb_ptr->index++];
	value = (value << 8) + (efb_ptr->buffer_ptr[efb_ptr->index++] & 255);
	value = (value << 8) + (efb_ptr->buffer_ptr[efb_ptr->index++] & 255);
	return (value << 8) + (efb_ptr->buffer_ptr[efb_ptr->index++] & 255);
}

/**
 * @return a long composed from the next 8 bytes. Little-endian.
 * @throws IOException if avail() < 8.
 */
int64 etch_flex_buffer_get_long(etch_flex_buffer *efb_ptr)
{
    int64 value;

	if (LITTLE_ENDIAN)
	{
		// little-endian
		value = efb_ptr->buffer_ptr[efb_ptr->index++] & 255;
		value += (((int64)(efb_ptr->buffer_ptr[efb_ptr->index++] & 255)) << 8);
		value += (((int64)(efb_ptr->buffer_ptr[efb_ptr->index++] & 255)) << 16);
		value += (((int64)(efb_ptr->buffer_ptr[efb_ptr->index++] & 255)) << 24);
		value += (((int64)(efb_ptr->buffer_ptr[efb_ptr->index++] & 255)) << 32);
		value += (((int64)(efb_ptr->buffer_ptr[efb_ptr->index++] & 255)) << 40);
		value += (((int64)(efb_ptr->buffer_ptr[efb_ptr->index++] & 255)) << 48);
		return value + (((int64)(efb_ptr->buffer_ptr[efb_ptr->index++] & 255)) << 56);
	}
	
	// big-endian
	value = efb_ptr->buffer_ptr[efb_ptr->index++];
	value = (value << 8) + (efb_ptr->buffer_ptr[efb_ptr->index++] & 255);
	value = (value << 8) + (efb_ptr->buffer_ptr[efb_ptr->index++] & 255);
	value = (value << 8) + (efb_ptr->buffer_ptr[efb_ptr->index++] & 255);
	value = (value << 8) + (efb_ptr->buffer_ptr[efb_ptr->index++] & 255);
	value = (value << 8) + (efb_ptr->buffer_ptr[efb_ptr->index++] & 255);
	value = (value << 8) + (efb_ptr->buffer_ptr[efb_ptr->index++] & 255);
	return (value << 8) + (efb_ptr->buffer_ptr[efb_ptr->index++] & 255);
}

/**
 * @return a float from the next available bytes.
 */
float etch_flex_buffer_get_float(etch_flex_buffer *efb_ptr)
{
    int i = etch_flex_buffer_get_int(efb_ptr);
    return *( (float *) &i );
}

/**
 * @return a double from the next available bytes.
 * @throws IOException if avail() < 8.
 */
double etch_flex_buffer_get_double(etch_flex_buffer *efb_ptr)
{
    int64 l = etch_flex_buffer_get_long(efb_ptr);
	return *( (double *) &l );
}

/**
 * Fills a buffer fully from the next available bytes.
 * @param b
 * @throws IOException if avail() < b.length.
 */
size_t etch_flex_buffer_get_fully( etch_flex_buffer *efb_ptr, byte *b, size_t size )
{
    return etch_flex_buffer_get( efb_ptr, b, 0, size );
}

/**
 * If index has moved past length during a put, then adjust length
 * to track index.
 */
static void etch_flex_buffer_fix_length(etch_flex_buffer *efb_ptr)
{
	if (efb_ptr->index > efb_ptr->length)
		efb_ptr->length = efb_ptr->index;
}

/**
 * Puts some bytes into the buffer as if by repeated
 * calls to put().
 * @param buf the source of the bytes to put.
 * @param off the index to the first byte to put.
 * @param len the number of bytes to put.
 * @return number of bytes it puts into the buffer.
 */
size_t etch_flex_buffer_put( etch_flex_buffer *efb_ptr, byte *buf, size_t off, size_t len )
{
	if (len == 0)
		return 0;
	
	if (!etch_flex_buffer_ensure_size( efb_ptr, efb_ptr->index+len ))
        return 0;

    memcpy(efb_ptr->buffer_ptr+efb_ptr->index, buf+off, len);
	efb_ptr->index += len;
	etch_flex_buffer_fix_length(efb_ptr);
	
	return len;
}


/**
 * Copies the specified number of bytes from buf into buffer
 * as if by repeated execution of put( buf.get() ).
 * @param buf the source of the bytes to put.
 * @param len the number of bytes to put. Len must be <=
 * buf.avail().
 * @return number of bytes it puts into the buffer
 */
size_t etch_flex_buffer_put_from_flex_buffer( etch_flex_buffer *dst_efb_ptr, etch_flex_buffer *src_efb_ptr, size_t len )
{
	size_t n = etch_flex_buffer_put( dst_efb_ptr, src_efb_ptr->buffer_ptr, src_efb_ptr->index, len );
	etch_flex_buffer_skip(src_efb_ptr, n, FALSE );
	return n;
}

/**
 * Puts the byte.
 * @param value
 */
size_t etch_flex_buffer_put_byte( etch_flex_buffer *efb_ptr, byte value )
{
    size_t n = 0;

	if (etch_flex_buffer_ensure_size( efb_ptr, efb_ptr->index+1 ))
    {
        efb_ptr->buffer_ptr[efb_ptr->index++] = value;
    	etch_flex_buffer_fix_length(efb_ptr);
        n = sizeof(value);
    }

    return n;
}

/**
 * Puts the short as the next 2 bytes. Little-endian.
 * @param value
 */
size_t etch_flex_buffer_put_short( etch_flex_buffer *efb_ptr, short value )
{
    if (!etch_flex_buffer_ensure_size( efb_ptr, efb_ptr->index + sizeof(value) ))
        return 0;

	if (LITTLE_ENDIAN)
	{
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) value;
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (value >> 8);
	}
	else
	{
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (((unsigned short)value) >> 8);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) value;
	}
	
	etch_flex_buffer_fix_length(efb_ptr);

    return sizeof(value);
}

/**
 * Puts the int as the next 4 bytes. Little-endian.
 * @param value
 */
size_t etch_flex_buffer_put_int( etch_flex_buffer *efb_ptr, int value )
{
    if (!etch_flex_buffer_ensure_size( efb_ptr, efb_ptr->index + sizeof(value) ))
        return 0;

    if (LITTLE_ENDIAN)
	{
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) value;
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (value >> 8);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (value >> 16);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (value >> 24);
	}
	else
	{
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) ( ((unsigned int) value) >> 24);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) ( ((unsigned int) value) >> 16);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) ( ((unsigned int) value) >> 8);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) value;
	}
	
	etch_flex_buffer_fix_length(efb_ptr);

    return sizeof(value);
}

/**
 * Puts the long as the next 8 bytes. Little-endian.
 * @param value
 */
size_t etch_flex_buffer_put_long( etch_flex_buffer *efb_ptr, int64 value )
{
    if (!etch_flex_buffer_ensure_size( efb_ptr, efb_ptr->index + sizeof(value) ))
        return 0;

	if (LITTLE_ENDIAN)
	{
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) value;
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (value >> 8);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (value >> 16);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (value >> 24);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (value >> 32);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (value >> 40);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (value >> 48);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (value >> 56);
	}
	else
	{
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) ( ( (uint64) value ) >> 56);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (( (uint64) value ) >> 48);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (( (uint64) value ) >> 40);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (( (uint64) value ) >> 32);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (( (uint64) value ) >> 24);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (( (uint64) value ) >> 16);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) (( (uint64) value ) >> 8);
		efb_ptr->buffer_ptr[efb_ptr->index++] = (byte) value;
	}
	
	etch_flex_buffer_fix_length(efb_ptr);

    return sizeof(value);
}

/**
 * Puts the float as the next 4 bytes. Little-endian.
 * @param value
 */
size_t etch_flex_buffer_put_float( etch_flex_buffer *efb_ptr, float value )
{
	return etch_flex_buffer_put_int( efb_ptr, * ((int *)( &value )) );
}

/**
 * Puts the double as the next 8 bytes. Little-endian.
 * @param value
 * @throws IOException
 */
size_t etch_flex_buffer_put_double( etch_flex_buffer *efb_ptr, double value )
{
	return etch_flex_buffer_put_long( efb_ptr, *( (int64*) &value ) );
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
etch_flex_buffer *etch_flex_buffer_skip( etch_flex_buffer *efb_ptr, size_t len, BOOLEAN put )
{
	if (len < 0)
		return NULL;
	
	if (len == 0)
		return efb_ptr;
	
	if (put)
	{
		etch_flex_buffer_ensure_size( efb_ptr, efb_ptr->index+len );
		efb_ptr->index += len;
		etch_flex_buffer_fix_length(efb_ptr);
		return efb_ptr;
	}
	
	efb_ptr->index += len;
	
	return efb_ptr;
}
