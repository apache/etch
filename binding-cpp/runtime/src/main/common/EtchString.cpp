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

#include "capu/os/StringUtils.h"
#include "capu/os/Memory.h"
#include "common/EtchString.h"
#include "util/EtchUtil.h"

static capu::uint32_t ENCODING_ASCII = 0;
static capu::uint32_t ENCODING_UTF8  = 1;

const EtchObjectType* EtchString::TYPE() {
  const static EtchObjectType TYPE(EOTID_STRING, NULL);
  return &TYPE;
}

// TODO: String should support UTF-8 by default

EtchString::EtchString()
 : mData(NULL), mDataSize(0), mEncoding(ENCODING_ASCII) {
  addObjectType(TYPE());
}

EtchString::EtchString(const char* string)
 : mData(NULL), mDataSize(0), mEncoding(ENCODING_ASCII) {
  addObjectType(TYPE());
  if (string != NULL) {
    capu::uint32_t len = capu::StringUtils::Strlen(string);
    mData = new char[len + 1];
    // TODO: refactor this to use capu
    etch_strcpy_s(mData, len + 1, string);
  }
}

EtchString::EtchString(const capu::int8_t* buffer, const capu::int32_t bufferSize, EtchString encoding)
: mData(NULL), mDataSize(0), mEncoding(ENCODING_ASCII) {
  addObjectType(TYPE());
  EtchString enc("utf-8");
  if (encoding.equals(&enc)) {
    mEncoding = ENCODING_UTF8;
    mDataSize = bufferSize;
    if (buffer != NULL) {
      //utf8
      // TODO: refactor this an use a utf-8 strncpy function from capu
      mData = new char[bufferSize + 1];
      capu::Memory::Copy(mData, (const char*)buffer, bufferSize);
      mData [bufferSize] = 0x0;
    }
  } else {
    if (buffer != NULL) {
      //ascii
      capu::uint32_t len = capu::StringUtils::Strlen((const char*) buffer);
      mData = new char[len + 1];
      // TODO: refactor this to use capu
      etch_strcpy_s(mData, len + 1, (const char*) buffer);
    }
  }
}

EtchString::EtchString(const EtchString &copy)
: EtchObject(copy), mDataSize(copy.mDataSize), mEncoding(copy.mEncoding) {
  if (copy.mData == NULL)
    return;
  if (mEncoding != ENCODING_UTF8) {
    //ascii
    capu::int32_t len = capu::StringUtils::Strlen(copy.mData);
    mData = new char [len + 1];
    // TODO: refactor this to use capu
    etch_strcpy_s(mData, len + 1, copy.mData);
  } else {
    //utf8
    // TODO: refactor this to use capu utf-8 strncpy
    mData = new char [copy.mDataSize + 1];
    capu::StringUtils::Strncpy(mData, copy.mDataSize, copy.mData);
    mData[copy.mDataSize] = 0x0;
  }
}


EtchString& EtchString::operator=(const char *str) {
  // TODO: refactor this
  if (this->mData != str) {
    if (mEncoding == ENCODING_UTF8) {
      //utf8
      if (mData != NULL) {
        delete[] mData;
        mData = NULL;
      }
      if (str != NULL) {
        mDataSize = capu::StringUtils::Strlen(str);
        mData = new char[mDataSize + 1];
        capu::StringUtils::Strncpy(mData, mDataSize, str);
        mData[mDataSize] = 0x0;
      }
    } else {
      set(str);
    }
  }
  return *this;
}

void EtchString::set(const char* string, capu::uint32_t len) {
  // TODO: refactor this
  if (mData != NULL) {
    delete[] mData;
    mData = NULL;
  }
  if (string != NULL) {
    if (capu::StringUtils::Strlen(string) < len)
      return;
    capu::int32_t length = len;
    mData = new char[length + 1];
    etch_strcpy_s(mData, length + 1, string);
  }
}

EtchString& EtchString::operator=(const EtchString &str) {
  // TODO: refactor this
  if (this != &str) {
    if (mEncoding == ENCODING_UTF8) {
      //utf8
      if (mData != NULL) {
        delete[] mData;
        mData = NULL;
      }
      mData = new char [str.mDataSize];
      capu::StringUtils::Strncpy(mData, str.mDataSize, str.mData);
      mDataSize = str.mDataSize;
    } else {
      //ascii
      set(str.mData);
    }
  }
  return *this;
}

EtchString::~EtchString() {
  if (mData != NULL) {
    delete[] mData;
    mData = NULL;
  }
}

void EtchString::set(const char* string) {
  if (mData != NULL) {
    delete[] mData;
    mData = NULL;
  }
  if (string != NULL) {
    capu::int32_t len = capu::StringUtils::Strlen(string);
    mData = new char[len + 1];
    etch_strcpy_s(mData, len + 1, string);
  }
}

capu::int32_t EtchString::length() const {
  // TODO: refactor this
  if (mEncoding != ENCODING_UTF8) {
    return capu::StringUtils::Strlen(mData);
  } else {
    capu::uint32_t result = 0;
    etch_strlen_utf8(mData, result);
    return result;
  }
}

const char* EtchString::c_str() const {
  return mData;
}

capu::bool_t EtchString::equals(const EtchObject * other) const {
  // TODO: refactor this
  if (other == NULL)
    return false;
  else if (!other->getObjectType()->equals(EtchString::TYPE()))
    return false;
  if (mEncoding != ENCODING_UTF8 && (((EtchString*)other)->mEncoding != ENCODING_UTF8)) {
    //ascii
    if (capu::StringUtils::Strcmp(((EtchString*) other)->mData, this->mData) == 0)
      return true;
  } else if (mEncoding == ENCODING_UTF8 && (((EtchString*)other)->mEncoding == ENCODING_UTF8)) {
    //utf8
    if (strncmp(((EtchString*) other)->mData, this->mData, mDataSize) == 0)
      return true;
  }
  return false;
}

capu::int32_t EtchString::rightFind(const char c) const {
  if (mEncoding == ENCODING_UTF8) {
    //utf8
    //TODO: Implementation needed
    return -1;
  }
  const char * str = this->c_str();
  capu::int32_t index = -1;
  char* ch = strchr((char *) str, c);
  while (ch != NULL) {
    index = ch - str;
    ch = strchr(ch + 1, c);
  }
  return index;
}

capu::int32_t EtchString::leftFind(const char c) const {
  if (mEncoding == ENCODING_UTF8) {
    //utf8
    //TODO: Implementation needed
    return -1;
  }
  const char * str = c_str();
  capu::int32_t index = -1;
  char* ch = strchr((char*) str, c);
  //NOT FOUND
  if (ch == NULL)
    return -1;
  index = ch - str;
  return index;
}

status_t EtchString::substring(capu::uint32_t start, capu::uint32_t length, EtchString * dest) const {
  if (mEncoding == ENCODING_UTF8) {
    //utf8
    //TODO: Implementation needed
    return ETCH_EUNIMPL;
  }
  capu::uint32_t len = capu::StringUtils::Strlen(this->mData);
  if (start >= len || len < (start + length) || dest == NULL) {
    return ETCH_EINVAL;
  } else {
    dest->set(&this->mData[start], length);
    return ETCH_OK;
  }
}

capu::uint32_t EtchString::getHashCode() const {
  capu::uint32_t result = 0;
  capu::uint32_t len = 0;
  if (mEncoding != ENCODING_UTF8) {
    //ascii
    len = capu::StringUtils::Strlen(mData);
  } else {
    //utf8
    len = mDataSize;
  }

  for (capu::uint32_t i = 0; i < len; i++) {
    result = (result + static_cast<capu::uint32_t> (mData[i]) * 13);
  }
  return result;
}

status_t EtchString::getBytes(capu::int8_t** buffer, capu::int32_t *bufferSize, EtchString encoding) {
  //TODO: use encoding
  if (buffer == NULL || bufferSize == NULL) {
    return ETCH_EINVAL;
  }

  *buffer = new capu::int8_t[length() + 1];
  memcpy(*buffer, mData, length() + 1);
  *bufferSize = length() + 1;

  return ETCH_OK;
}

capu::uint32_t EtchString::getNumBytes() const {
  if (mEncoding == ENCODING_UTF8) {
    return mDataSize;
  } else {
    return capu::StringUtils::Strlen(mData);
  }
}
