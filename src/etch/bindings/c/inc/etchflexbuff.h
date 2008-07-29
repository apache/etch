/**
 * A flex_buffer wraps a byte array and manages the active region of
 * it (0..length). It also supports dynamically extending the buffer
 * as needed.
 * 
 * A flex_buffer also has an index (read or write cursor). The various
 * get and put operations always occur at the current index, with the
 * index adjusted appropriately afterward. Get will not move index past
 * length. If put needs to move index past length, length is also
 * adjusted. This may cause the byte array to be re-allocated to a
 * larger size.
 */

#ifndef ETCH_FLEX_BUFFER_H
#define ETCH_FLEX_BUFFER_H

#include "etchdefs.h"

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

typedef struct etch_flex_buffer
{
    unsigned char   *buffer_ptr;
    size_t          size;   /* buffer size */
    size_t          length; /* valid data length */
    size_t          index;  /* current position */
} etch_flex_buffer;

etch_flex_buffer *etch_flex_buffer_create(void);
etch_flex_buffer *etch_flex_buffer_create_s(size_t size);
etch_flex_buffer *etch_flex_buffer_create_b(void *buffer_ptr, size_t size, size_t length);
etch_flex_buffer *etch_flex_buffer_create_bi(void *buffer_ptr, size_t size, size_t length, int index);

etch_flex_buffer *etch_flex_buffer_skip( etch_flex_buffer *efb_ptr, size_t len, BOOLEAN put );

byte* etch_flex_buffer_get_buffer(etch_flex_buffer *efb_ptr);
void etch_flex_buffer_destroy(etch_flex_buffer *efb_ptr);
BOOLEAN etch_flex_buffer_set_length(etch_flex_buffer *efb_ptr, size_t len);
BOOLEAN etch_flex_buffer_set_index(etch_flex_buffer *efb_ptr, size_t index);
size_t etch_flex_buffer_avail(etch_flex_buffer *efb_ptr);
etch_flex_buffer *etch_flex_buffer_compact(etch_flex_buffer *efb_ptr);

size_t etch_flex_buffer_get( etch_flex_buffer *efb_ptr, byte *buf, size_t off, size_t len );
byte etch_flex_buffer_get_byte(etch_flex_buffer *efb_ptr);
short etch_flex_buffer_get_short(etch_flex_buffer *efb_ptr);
int etch_flex_buffer_get_int(etch_flex_buffer *efb_ptr);
int64 etch_flex_buffer_get_long(etch_flex_buffer *efb_ptr);
float etch_flex_buffer_get_float(etch_flex_buffer *efb_ptr);
double etch_flex_buffer_get_double(etch_flex_buffer *efb_ptr);
size_t etch_flex_buffer_get_fully( etch_flex_buffer *efb_ptr, byte *b, size_t size );

size_t etch_flex_buffer_put( etch_flex_buffer *efb_ptr, byte *buf, size_t off, size_t len );
size_t etch_flex_buffer_put_from_flex_buffer( etch_flex_buffer *dst_efb_ptr, etch_flex_buffer *src_efb_ptr, size_t len );
size_t etch_flex_buffer_put_byte( etch_flex_buffer *efb_ptr, byte value );
size_t etch_flex_buffer_put_short( etch_flex_buffer *efb_ptr, short value );
size_t etch_flex_buffer_put_int( etch_flex_buffer *efb_ptr, int value );
size_t etch_flex_buffer_put_long( etch_flex_buffer *efb_ptr, int64 value );
size_t etch_flex_buffer_put_float( etch_flex_buffer *efb_ptr, float value );
size_t etch_flex_buffer_put_double( etch_flex_buffer *efb_ptr, double value );

#ifdef __cplusplus
}
#endif /* __cplusplus */


#endif