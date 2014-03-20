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

#include "transport/EtchFlexBuffer.h"

EtchFlexBuffer::EtchFlexBuffer() :
INIT_BUFFER_LEN(32), MAX_BUFFER_LEN(4 * 1024 * 1024),
TRIM_BUFFER_LEN(16 * 1024), mBuffer(NULL),
mSize(INIT_BUFFER_LEN), mIndex(0),
mLength(0), mByteRepresentation(ETCH_BIG_ENDIAN) {

  mBuffer = new capu::int8_t[INIT_BUFFER_LEN];
}

EtchFlexBuffer::EtchFlexBuffer(capu::int8_t *buf, capu::uint32_t size) :
INIT_BUFFER_LEN(32),
MAX_BUFFER_LEN(4 * 1024 * 1024),
TRIM_BUFFER_LEN(16 * 1024),
mBuffer(buf), mSize(size), mIndex(0),
mLength(size), mByteRepresentation(ETCH_BIG_ENDIAN) {

}

EtchFlexBuffer::~EtchFlexBuffer() {
  delete [] mBuffer;
}

capu::int8_t * EtchFlexBuffer::getBuffer() {
  return mBuffer;
}

capu::uint32_t EtchFlexBuffer::getLength() {
  return mLength;
}

capu::uint32_t EtchFlexBuffer::getSize() {
  return mSize;
}

capu::uint32_t EtchFlexBuffer::getIndex() {
  return mIndex;
}

status_t EtchFlexBuffer::getInteger(capu::uint32_t & value) {

  if (sizeof (capu::uint32_t) > getAvailableBytes())
    return ETCH_ERANGE;

  if (mByteRepresentation == ETCH_LITTLE_ENDIAN) {
    // little-endian
    capu::uint32_t result = (mBuffer[mIndex++] & 255);
    result += (mBuffer[mIndex++] & 255) << 8;
    result += (mBuffer[mIndex++] & 255) << 16;
    value = result + ((mBuffer[mIndex++] & 255) << 24);
  } else if (mByteRepresentation == ETCH_BIG_ENDIAN) {
    // big-endian
    capu::uint32_t result = mBuffer[mIndex++] & 255;
    result = (result << 8) + (mBuffer[mIndex++] & 255);
    result = (result << 8) + (mBuffer[mIndex++] & 255);
    value = (result << 8) + (mBuffer[mIndex++] & 255);
  } else {
    return ETCH_EINVAL;
  }
  return ETCH_OK;
}

status_t EtchFlexBuffer::getByte(capu::int8_t & value) {

  if (1 > getAvailableBytes())
    return ETCH_ERANGE;

  value = mBuffer[mIndex++];
  return ETCH_OK;
}

status_t EtchFlexBuffer::getShort(capu::int16_t & value) {
  if (2 > getAvailableBytes())
    return ETCH_ERANGE;
  if (mByteRepresentation == ETCH_LITTLE_ENDIAN) {
    // little-endian
    capu::int32_t _value = mBuffer[mIndex++] & 255;
    value = (capu::int16_t) (_value + ((mBuffer[mIndex++] & 255) << 8));
  } else if (mByteRepresentation == ETCH_BIG_ENDIAN) {
    // big-endian
    capu::int32_t _value = mBuffer[mIndex++];
    value = (capu::int16_t) ((_value << 8) + (mBuffer[mIndex++] & 255));
  } else {
    return ETCH_EINVAL;
  }
  return ETCH_OK;
}

status_t EtchFlexBuffer::getLong(capu::int64_t & value) {
  if (8 > getAvailableBytes())
    return ETCH_ERANGE;

  if (mByteRepresentation == ETCH_LITTLE_ENDIAN) {
    // little-endian
    capu::int64_t _value = mBuffer[mIndex++] & 255;
    _value += (capu::int64_t) (mBuffer[mIndex++] & 255) << 8;
    _value += (capu::int64_t) (mBuffer[mIndex++] & 255) << 16;
    _value += (capu::int64_t) (mBuffer[mIndex++] & 255) << 24;
    _value += (capu::int64_t) (mBuffer[mIndex++] & 255) << 32;
    _value += (capu::int64_t) (mBuffer[mIndex++] & 255) << 40;
    _value += (capu::int64_t) (mBuffer[mIndex++] & 255) << 48;
    value = _value + ((capu::int64_t) (mBuffer[mIndex++] & 255) << 56);
  } else if (mByteRepresentation == ETCH_BIG_ENDIAN) {
    // big-endian
    capu::int64_t _value = mBuffer[mIndex++];
    _value = (_value << 8) + (mBuffer[mIndex++] & 255);
    _value = (_value << 8) + (mBuffer[mIndex++] & 255);
    _value = (_value << 8) + (mBuffer[mIndex++] & 255);
    _value = (_value << 8) + (mBuffer[mIndex++] & 255);
    _value = (_value << 8) + (mBuffer[mIndex++] & 255);
    _value = (_value << 8) + (mBuffer[mIndex++] & 255);
    value = (_value << 8) + (mBuffer[mIndex++] & 255);
  } else {
    return ETCH_EINVAL;
  }
  return ETCH_OK;
}

status_t EtchFlexBuffer::getFloat(capu::float_t & value) {
  capu::uint32_t tmp;
  status_t err = getInteger(tmp);
  if (err != ETCH_OK)
    return err;
  memcpy(&value, &tmp, sizeof (capu::float_t));
  return ETCH_OK;
}

status_t EtchFlexBuffer::getDouble(capu::double_t & value) {
  capu::int64_t tmp;
  status_t err = getLong(tmp);
  if (err != ETCH_OK)
    return err;
  memcpy(&value, &tmp, sizeof (capu::double_t));
  return ETCH_OK;
}

status_t EtchFlexBuffer::get(capu::int8_t *buf, capu::uint32_t off, capu::uint32_t len, capu::uint32_t& numBytes) {
  if (buf == NULL) {
    numBytes = 0;
    return ETCH_EINVAL;
  }

  if (len == 0) {
    numBytes = 0;
    return ETCH_EINVAL;
  }

  capu::int32_t n = 0;

  if ((signed)(len - off) < static_cast<capu::int32_t>(getAvailableBytes())) {
    if ((signed)(len - off) < 0) {
      return ETCH_EINVAL;
    }
    n = len - off;
  } else {
    n = getAvailableBytes();
  }

  numBytes = n;
  memcpy(buf, &mBuffer[mIndex], n);
  mIndex += n;

  return ETCH_OK;
}

capu::uint32_t EtchFlexBuffer::getAvailableBytes() {
  return mLength - mIndex;
}

status_t EtchFlexBuffer::setIndex(capu::uint32_t index) {
  if (index > mLength)
    return ETCH_EINVAL;
  mIndex = index;
  return ETCH_OK;
}

status_t EtchFlexBuffer::setLength(capu::uint32_t length) {
  if (length > mSize) {
    if (expandBuffer(length) == ETCH_ENO_MEMORY)
      return ETCH_ENO_MEMORY;
  }
  mLength = length;
  if (mIndex > mLength)
    mIndex = mLength;
  return ETCH_OK;
}

void EtchFlexBuffer::reset() {
  mLength = 0;
  mIndex = 0;
  if (mSize > TRIM_BUFFER_LEN) {
    //if buffer is too big trim it
    delete [] mBuffer;
    mBuffer = new capu::int8_t[TRIM_BUFFER_LEN];
    mSize = TRIM_BUFFER_LEN;
  }
}

status_t EtchFlexBuffer::put(EtchFlexBuffer & buffer) {
  status_t result = checkBuffer(buffer, buffer.getAvailableBytes());
  if (result != ETCH_OK)
    return result;

  copy(&buffer.mBuffer[buffer.mIndex], buffer.getAvailableBytes());

  return ETCH_OK;
}

status_t EtchFlexBuffer::put(EtchFlexBuffer & buffer, capu::uint32_t numBytes) {
  status_t result = checkBuffer(buffer, numBytes);
  if (result != ETCH_OK)
    return result;

  copy(&buffer.mBuffer[buffer.mIndex], numBytes);
  return ETCH_OK;
}

status_t EtchFlexBuffer::put(capu::int8_t * buffer, capu::uint32_t numBytes) {

  if (buffer == NULL)
    return ETCH_EINVAL;

  if (numBytes == 0)
    return ETCH_ERANGE;

  if (numBytes > (mSize - mIndex)) {
    if (expandBuffer(mIndex + numBytes) == ETCH_ENO_MEMORY)
      return ETCH_ENO_MEMORY;
  }
  copy(buffer, numBytes);
  return ETCH_OK;
}

void EtchFlexBuffer::setByteRepresentation(EtchByteRepresentation value) {
  mByteRepresentation = value;
}

status_t EtchFlexBuffer::putByte(capu::int8_t value) {
  if (1 > (mSize - mIndex)) {
    if (expandBuffer(mIndex + 1) == ETCH_ENO_MEMORY)
      return ETCH_ENO_MEMORY;
  }
  mBuffer[mIndex++] = (capu::int8_t) value;

  if (mIndex > mLength)
    mLength = mIndex;
  return ETCH_OK;
}

status_t EtchFlexBuffer::putShort(capu::int16_t value) {
  if (2 > (mSize - mIndex)) {
    if (expandBuffer(mIndex + 1) == ETCH_ENO_MEMORY)
      return ETCH_ENO_MEMORY;
  }

  if (mByteRepresentation == ETCH_LITTLE_ENDIAN) {
    mBuffer[mIndex++] = (capu::int8_t) value;
    mBuffer[mIndex++] = (capu::int8_t) (value >> 8);
  } else if (mByteRepresentation == ETCH_BIG_ENDIAN) {
    mBuffer[mIndex++] = (capu::int8_t) (value >> 8);
    mBuffer[mIndex++] = (capu::int8_t) value;
  } else {
    return ETCH_EINVAL;
  }
  if (mIndex > mLength)
    mLength = mIndex;
  return ETCH_OK;
}

status_t EtchFlexBuffer::putInt(capu::int32_t value) {
  if (4 > (mSize - mIndex)) {
    if (expandBuffer(mIndex + 1) == ETCH_ENO_MEMORY)
      return ETCH_ENO_MEMORY;
  }

  if (mByteRepresentation == ETCH_LITTLE_ENDIAN) {
    mBuffer[mIndex++] = (capu::int8_t) value;
    mBuffer[mIndex++] = (capu::int8_t) (value >> 8);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 16);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 24);
  } else if (mByteRepresentation == ETCH_BIG_ENDIAN) {
    mBuffer[mIndex++] = (capu::int8_t) (value >> 24);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 16);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 8);
    mBuffer[mIndex++] = (capu::int8_t) value;
  } else {
    return ETCH_EINVAL;
  }

  if (mIndex > mLength)
    mLength = mIndex;
  return ETCH_OK;
}

status_t EtchFlexBuffer::putLong(capu::int64_t value) {
  if (8 > (mSize - mIndex)) {
    if (expandBuffer(mIndex + 1) == ETCH_ENO_MEMORY)
      return ETCH_ENO_MEMORY;
  }

  if (mByteRepresentation == ETCH_LITTLE_ENDIAN) {
    mBuffer[mIndex++] = (capu::int8_t) value;
    mBuffer[mIndex++] = (capu::int8_t) (value >> 8);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 16);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 24);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 32);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 40);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 48);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 56);
  } else if (mByteRepresentation == ETCH_BIG_ENDIAN) {
    mBuffer[mIndex++] = (capu::int8_t) (value >> 56);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 48);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 40);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 32);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 24);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 16);
    mBuffer[mIndex++] = (capu::int8_t) (value >> 8);
    mBuffer[mIndex++] = (capu::int8_t) value;
  } else {
    return ETCH_EINVAL;
  }

  if (mIndex > mLength)
    mLength = mIndex;
  return ETCH_OK;
}

status_t EtchFlexBuffer::putFloat(capu::float_t value) {
  capu::int32_t tmp;
  memcpy(&tmp, &value, sizeof (capu::float_t));
  return putInt(tmp);
}

status_t EtchFlexBuffer::putDouble(capu::double_t value) {
  capu::int64_t tmp;
  memcpy(&tmp, &value, sizeof (capu::double_t));
  return putLong(tmp);
}

EtchByteRepresentation EtchFlexBuffer::getByteRepresentation() {
  return mByteRepresentation;
}

status_t EtchFlexBuffer::expandBuffer(capu::uint32_t numBytes) {
  capu::uint32_t _size = mSize;
  if (_size < INIT_BUFFER_LEN)
    _size = INIT_BUFFER_LEN;

  while ((mIndex + numBytes > _size) && (_size < MAX_BUFFER_LEN))
    _size = _size << 1;

  //BUFFER OVERFLOW
  if (mIndex + numBytes > _size)
    return ETCH_ENO_MEMORY;

  capu::int8_t * b = new capu::int8_t[_size];
  memcpy(b, mBuffer, mSize);
  delete [] mBuffer;
  mBuffer = b;
  mSize = _size;
  return ETCH_OK;
}

status_t EtchFlexBuffer::checkBuffer(EtchFlexBuffer & buffer, capu::uint32_t numBytes) {

  if ((buffer.mBuffer == NULL)
          || (buffer.mIndex + numBytes > buffer.mSize)
          || (buffer.mIndex > buffer.mSize)
          || (numBytes <= 0)
          || (numBytes > buffer.getAvailableBytes())) {
    return ETCH_EINVAL;
  }

  if (numBytes > (mSize - mIndex)) {
    if (expandBuffer(mIndex + numBytes) == ETCH_ENO_MEMORY)

      return ETCH_ENO_MEMORY;
  }

  return ETCH_OK;
}

void EtchFlexBuffer::copy(capu::int8_t *buffer, capu::uint32_t numBytes) {
  memcpy(&mBuffer[mIndex], (const void *) buffer, numBytes);
  mIndex += numBytes;

  if (mIndex > mLength)
    mLength = mIndex;
}