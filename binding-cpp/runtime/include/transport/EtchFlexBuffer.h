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

#ifndef __ETCHFLEXBUFFER_H__
#define __ETCHFLEXBUFFER_H__

#include "capu/util/SmartPointer.h"
#include "common/EtchConfig.h"
#include "common/EtchError.h"
#include "transport/EtchByteRepresentation.h"

/**
 * A FlexBuffer wraps a byte array and manages the active region of
 * it (0..length). It also supports dynamically extending the buffer
 * as needed.
 *
 * A FlexBuffer also has an index (read or write cursor). The various
 * get and put operations always occur at the current index, with the
 * index adjusted appropriately afterward. Get will not move index past
 * length. If put needs to move index past length, length is also
 * adjusted. This may cause the byte array to be re-allocated to a
 * larger size.
 */

class EtchFlexBuffer {
public:

  /**
   * Constructs a FlexBuffer with internal buffer and index and length of 0.
   */
  EtchFlexBuffer();

  /**
   * Constructs a FlexBuffer with internal buffer and index and length of 0.
   */
  EtchFlexBuffer(capu::int8_t *buf, capu::uint32_t size);

  /**
   * Destructor a FlexBuffer with internal buffer and index and length of 0.
   */
  ~EtchFlexBuffer();

  /**
   * @return the current byte array. Might change if any operation
   * needs to extend length past the end of the array.
   */
  capu::int8_t * getBuffer();

  /**
   * @return the current value of length. This is the last
   * index of valid data in the buffer.
   */
  capu::uint32_t getLength();

  /**
   * @return the current size of the buffer
   *
   */
  capu::uint32_t getSize();

  /**
   * @return the current value of index.
   */
  capu::uint32_t getIndex();

  /**
   * @return length() - index(). This is the amount
   * of data that could be read using the various forms of get. It doesn't
   * really mean anything in relation to put.
   */
  capu::uint32_t getAvailableBytes();

  /**
   * gets an integer value from flexible buffer
   * @param an int composed from the next 4 bytes. Little-endian.
   * @return ETCH_OK if an integer successfully read
   *         ETCH_EINVAL if the byte representation is not BIG ENDIAN or LITTLE ENDIAN
   *         ETCH_ERANGE if there is not enough value to be read (less than 4 byte)
   */
  status_t getInteger(capu::uint32_t & value);

  /**
   * gets an byte value from flexible buffer
   * @param an char composed from the next 1 bytes.
   * @return ETCH_OK if an integer successfully read
   *         ETCH_EINVAL if the byte representation is not BIG ENDIAN or LITTLE ENDIAN
   *         ETCH_ERANGE if there is not enough value to be read (less than 4 byte)
   */
  status_t getByte(capu::int8_t & value);


  /**
   * gets an short value from flexible buffer
   * @param an short composed from the next 2 bytes. Little-endian.
   * @return ETCH_OK if an integer successfully read
   *         ETCH_EINVAL if the byte representation is not BIG ENDIAN or LITTLE ENDIAN
   *         ETCH_ERANGE if there is not enough value to be read (less than 4 byte)
   */
  status_t getShort(capu::int16_t & value);


  /**
   * gets an long value from flexible buffer
   * @param an long composed from the next 8 bytes. Little-endian.
   * @return ETCH_OK if an integer successfully read
   *         ETCH_EINVAL if the byte representation is not BIG ENDIAN or LITTLE ENDIAN
   *         ETCH_ERANGE if there is not enough value to be read (less than 4 byte)
   */
  status_t getLong(capu::int64_t & value);

  /**
   * gets an float value from flexible buffer
   * @param an float composed from the next 2 bytes. Little-endian.
   * @return ETCH_OK if an integer successfully read
   *         ETCH_EINVAL if the byte representation is not BIG ENDIAN or LITTLE ENDIAN
   *         ETCH_ERANGE if there is not enough value to be read (less than 4 byte)
   */
  status_t getFloat(capu::float_t & value);


  /**
   * gets an double value from flexible buffer
   * @param an double composed from the next 8 bytes. Little-endian.
   * @return ETCH_OK if an integer successfully read
   *         ETCH_EINVAL if the byte representation is not BIG ENDIAN or LITTLE ENDIAN
   *         ETCH_ERANGE if there is not enough value to be read (less than 4 byte)
   */
  status_t getDouble(capu::double_t & value);


  /**
   * Sets the index
   *
   * @param index the new value of index.
   * @return ETCH_OK if the index is successfully set
   *         ETCH_EINVAL if the given index is not valid
   *
   */
  status_t setIndex(capu::uint32_t index);

  /**
   * Sets the length of flexbuffer
   *
   * @param index the new value of index.
   * @return ETCH_OK if the length is successfully set
   *         ETCH_ENO_MEMORY if the buffer with given size can not be allocated
   *
   */
  status_t setLength(capu::uint32_t length);

  /**
   * Sets both index and length to 0. Trims the buffer length if it is past
   * the recommended limit.
   *
   */
  void reset();

  /**
   * Copies the available bytes from buffer into mBuffer as if by
   * repeated execution of put( buf.get() ).
   * @param buffer the source of the bytes to put. All available
   * bytes are copied.
   * @return ETCH_OK if the buffer is put into the this
   *         ETCH_EINVAL if the buffer has inconsistency within their internals
   *         ETCH_ERANGE if the buffer's size is less than its index
   *
   */
  status_t put(EtchFlexBuffer & buffer);

  /**
   * Copies the available bytes from buf into buffer as if by
   * repeated execution of put( buf.get() ).
   * @param buffer the source of the bytes to put. All available
   * bytes are copied.
   * @param numBytes number of byte that will be copied
   *
   * @return ETCH_OK if the buf is put into the this flexbuffer
   *         ETCH_EINVAL if the buf has inconsistency within their internals
   *         ETCH_ERANGE if the buf's size is less than its index
   *
   */
  status_t put(EtchFlexBuffer & buffer, capu::uint32_t numBytes);

  /**
   * Copies the available bytes from buf into buffer as if by
   * repeated execution of put( buf.get() ).
   * @param buf the source of the bytes to put. All available
   * bytes are copied.
   * @param  size number of bytes that will be copied
   *
   * @return ETCH_OK if the buf is put into the this flexbuffer
   *         ETCH_EINVAL if the buf has inconsistency within their internals
   *         ETCH_ERANGE if the buf's size is less than its index
   *
   */
  status_t put(capu::int8_t *buffer, capu::uint32_t numBytes);

  /**
   * Copies a byte to flex buffer
   * @param value that will be copied
   * @return ETCH_OK if the buffer is put into the this
   *         ETCH_ERANGE if the buffer's size is less than its index
   *
   */
  status_t putByte(capu::int8_t value);

  /**
   * Copies a short to flex buffer
   * @param value that will be copied
   * @return ETCH_OK if the buffer is put into the this
   *         ETCH_EINVAL if the byte representation is not BIG ENDIAN or LITTLE ENDIAN
   *         ETCH_ERANGE if the buffer's size is less than its index
   *
   */
  status_t putShort(capu::int16_t value);

  /**
   * Copies a long to flex buffer
   * @param value that will be copied
   * @return ETCH_OK if the buffer is put into the this
   *         ETCH_EINVAL if the byte representation is not BIG ENDIAN or LITTLE ENDIAN
   *         ETCH_ERANGE if the buffer's size is less than its index
   *
   */
  status_t putLong(capu::int64_t value);

  /**
   * Copies a int to flex buffer
   * @param value that will be copied
   * @return ETCH_OK if the buffer is put into the this
   *         ETCH_EINVAL if the byte representation is not BIG ENDIAN or LITTLE ENDIAN
   *         ETCH_ERANGE if the buffer's size is less than its index
   *
   */
  status_t putInt(capu::int32_t value);
  
  /**
   * Copies a float to flex buffer
   * @param value that will be copied
   * @return ETCH_OK if the buffer is put into the this
   *         ETCH_EINVAL if the byte representation is not BIG ENDIAN or LITTLE ENDIAN
   *         ETCH_ERANGE if the buffer's size is less than its index
   *
   */
  status_t putFloat(capu::float_t value);
  
  /**
   * Copies a double to flex buffer
   * @param value that will be copied
   * @return ETCH_OK if the buffer is put into the this
   *         ETCH_EINVAL if the byte representation is not BIG ENDIAN or LITTLE ENDIAN
   *         ETCH_ERANGE if the buffer's size is less than its index
   *
   */
  status_t putDouble(capu::double_t value);

  /**
   * @param value is a boolean value to specify LittleEndian or BigEndian representation
   *        ETCH_LITTLE_ENDIAN for LittleEndian
   *        ETCH_BIG_ENDIAN for BigEndian
   */
  void setByteRepresentation(EtchByteRepresentation value);

  /**
   * @return ETCH_LITTLE_ENDIAN for LittleEndian
   *         ETCH_BIG_ENDIAN for BigEndian
   */
  EtchByteRepresentation getByteRepresentation();

  /**
   * Copies data from the byte array to buf as if by repeated
   * calls to getByte().
   * @param buf a buffer to receive the data. At most
   * min( mLength, getAvailableBytes() ) bytes are transferred, starting
   * at off.
   * @param off the index in buf to receive the data.
   * off must be <= buf.length.
   *
   * @param len, length of buffer
   * @param numBytes contains the amount of data which has been copied to buf. 0 if there occured an error.
   *
   * @return ETCH_OK  data is successfully transferred.
   *         ETCH_EINVAL if buffer is null or len is 0
   */
  status_t get(capu::int8_t *buf, capu::uint32_t off, capu::uint32_t len, capu::uint32_t& numBytes);

private:

  const capu::uint32_t INIT_BUFFER_LEN;

  const capu::uint32_t MAX_BUFFER_LEN;

  const capu::uint32_t TRIM_BUFFER_LEN;

  capu::int8_t *mBuffer;

  capu::uint32_t mSize;

  capu::uint32_t mIndex;

  capu::uint32_t mLength;

  EtchByteRepresentation mByteRepresentation;

  status_t expandBuffer(capu::uint32_t numBytes);

  status_t checkBuffer(EtchFlexBuffer & buffer, capu::uint32_t numBytes);

  void copy(capu::int8_t *buffer, capu::uint32_t numBytes);
};

typedef capu::SmartPointer<EtchFlexBuffer> EtchFlexBufferPtr;

#endif /* ETCHFLEXBUFFER_H */


