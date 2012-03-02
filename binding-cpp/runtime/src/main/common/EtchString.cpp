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

#include "common/EtchString.h"
const EtchObjectType EtchString::TYPE(EOTID_STRING, NULL);

EtchString::EtchString()
: EtchObject(&EtchString::TYPE)
, mData(NULL) {
  //ETCH_LOG("EtchString: ", EtchLogLevel::Error, "dies ist ein Test");
}

EtchString::EtchString(const char* string)
: EtchObject(&EtchString::TYPE)
, mData(NULL) {
  if (string != NULL) {
    capu::int32_t len = strlen(string);
    mData = new char[len + 1];
    etch_strcpy_s(mData, len + 1, string);
  }
}

EtchString::EtchString(const EtchString &copy)
: EtchObject(&EtchString::TYPE) {
  if (copy.mData == NULL)
    return;
  capu::int32_t len = strlen(copy.mData);
  mData = new char [len + 1];
  etch_strcpy_s(mData, len + 1, copy.mData);
}

EtchString& EtchString::operator=(const char *str) {
  if (this->mData != str) {
    set(str);
  }
  return *this;
}

void EtchString::set(const char* string, capu::uint32_t len) {
  if (mData != NULL) {
    delete[] mData;
    mData = NULL;
  }
  if (string != NULL) {
    if (strlen(string) < len)
      return;
    capu::int32_t length = len;
    mData = new char[length + 1];
    etch_strcpy_s(mData, length + 1, string);
  }

}

EtchString& EtchString::operator=(const EtchString &str) {
  if (this != &str) {
    set(str.mData);
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
    capu::int32_t len = strlen(string);
    mData = new char[len + 1];
    etch_strcpy_s(mData, len + 1, string);
  }
}

capu::int32_t EtchString::length() const {
  return strlen(mData);
}

const char* EtchString::c_str() const{
  capu::int32_t len = strlen(mData);
  char* tmp = new char[len + 1];
  etch_strcpy_s(tmp, len + 1, mData);
  return tmp;
}

const char* EtchString::c_str() {
  return mData;
}

capu::bool_t EtchString::equals(const EtchObject* other) const{
  if (other == NULL)
    return false;
  else if (!other->getObjectType()->equals(&EtchString::TYPE))
    return false;
  if (strcmp(((EtchString*) other)->mData, this->mData) == 0)
    return true;
  return false;
}

capu::int32_t EtchString::rightFind(const char c) {
  const char * str = this->c_str();
  capu::int32_t index = -1;
  char* ch = strchr((char *) str, c);
  while (ch != NULL) {
    index = ch - str;
    ch = strchr(ch + 1, c);
  }
  return index;
}

capu::int32_t EtchString::leftFind(const char c) {
  const char * str = this->c_str();
  capu::int32_t index = -1;
  char* ch = strchr((char*) str, c);
  //NOT FOUND
  if (ch == NULL)
    return -1;
  index = ch - str;
  return index;
}

status_t EtchString::substring(capu::uint32_t start, capu::uint32_t length, EtchString *dest) {
  capu::uint32_t len = strlen(this->mData);
  if (start >= len || len < (start + length) || dest == NULL) {
    return ETCH_EINVAL;
  } else {
    dest->set(&this->mData[start], length);
    return ETCH_OK;
  }
}

capu::uint64_t EtchString::getHashCode() const{
  capu::uint64_t result = 0;
  capu::uint32_t len = strlen(mData);
  for (capu::uint32_t i = 0; i < len; i++) {
    result = (result + static_cast<capu::uint64_t> (mData[i]) * 13);
  }
  return result;
}
