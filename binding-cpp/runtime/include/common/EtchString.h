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

#ifndef __ETCHSTRING_H__
#define __ETCHSTRING_H__

#include "common/EtchObject.h"

/**
 * String type.
 */
class EtchString :
public EtchObject {

public:

  /**
   * EtchObjectType for EtchString.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructs the String.
   */
  EtchString();

  /**
   * Copy constructor
   * @param copy
   */
  EtchString(const EtchString & copy);

  /**
   * Constructs the String.
   * @param string as c string
   */
  EtchString(const char* string);

  /**
   * Constructs the String from given buffer
   * @param buffer as bytes
   * @param bufferSize size of the buffer
   * @param encoding of the string
   */
  EtchString(const capu::int8_t* buffer, const capu::int32_t bufferSize, EtchString encoding);

  /**
   * Destructure.
   */
  virtual ~EtchString();

  /**
   * Set a new string.
   * @param string as c string
   */
  void set(const char* string);

  /**
   * Set a new string.
   * @param string as c string
   * @param len as unsigned integer to represent size of string
   */
  void set(const char* string, capu::uint32_t len);

  /**
   * Returns the amount of characters in the string.
   * @return amount of characters
   */
  capu::int32_t length() const;

  /**
   * the substring is generated.
   * note: User must explicitly deallocate the returned object
   * @param start starting index of substring
   * @param length length of given string
   * @param dest where the substring will be stored
   * @return ETCH_OK if the substring has been successfully stored into the dest
   *         ETCH_EINVAL if there are errors in the indexing or buffer is NULL
   */
  status_t substring(capu::uint32_t start, capu::uint32_t length, EtchString *dest) const;

  /**
   * Returns the index of the first occurrence of a character within given string
   * @param c
   * @return index index of first occurrence of character
   *         -1 if the character is not found
   */
  capu::int32_t leftFind(const char c) const;

  /**
   * Returns the index of last occurrence of a character within given string
   * @param c
   * @return index index of last occurrence of character
   *         -1 if the character is not found
   */
  capu::int32_t rightFind(const char c) const;

  /**
   * Check two string is equal or not
   * @param other
   * @return true if this equals b, false otherwise. takes into account nulls.
   */
  capu::bool_t equals(const EtchObject * other) const;

  /**
   * Returns c styled string.
   */
  const char* c_str() const;

  /**
   * Returns the bytes of the string.
   * @param buffer out parameter for bytes
   * @param bufferSize size of buffer
   * @param encoding of the string
   */
  status_t getBytes(capu::int8_t** buffer, capu::int32_t *bufferSize, EtchString encoding = "utf-8");

  /**
   * Assignment operator overloading
   */
  EtchString& operator=(const EtchString &str);

  /**
   * Assignment operator overloading
   */
  EtchString& operator=(const EtchString *str);

  /**
   * Assignment operator overloading
   */
  EtchString& operator=(const char *str);

  /**
   * Returns hash code
   */
  capu::uint32_t getHashCode() const;

  /**
   * Returns number of bytes
   */
  capu::uint32_t getNumBytes() const;

private:
  char* mData;
  capu::uint32_t mDataSize;
  capu::uint32_t mEncoding;
};

typedef capu::SmartPointer<EtchString> EtchStringPtr;

#endif
