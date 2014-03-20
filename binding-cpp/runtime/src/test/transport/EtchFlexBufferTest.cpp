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


#include <gtest/gtest.h>
#include "transport/EtchFlexBuffer.h"
#include "common/EtchError.h"

TEST(EtchFlexBufferTest, constructorTest) {
  EtchFlexBuffer *buffer = new EtchFlexBuffer();
  EXPECT_TRUE(buffer != NULL);
  EXPECT_TRUE(buffer->getLength() == 0);
  EXPECT_TRUE(buffer->getIndex() == 0);
  EXPECT_TRUE(buffer->getSize() == 32);
  delete buffer;
}

TEST(EtchFlexBufferTest, putTest) {
  EtchFlexBuffer *buffer = new EtchFlexBuffer();
  EtchFlexBuffer *buffer2 = new EtchFlexBuffer();
  capu::int8_t key [] = {'a', 'b', 'c'};

  EXPECT_TRUE(buffer->put(NULL, 32) == ETCH_EINVAL);

  EXPECT_TRUE(buffer->put(key, 0) == ETCH_ERANGE);

  EXPECT_TRUE(buffer->put(key, 3) == ETCH_OK);
  EXPECT_TRUE(memcmp(buffer->getBuffer(), "abc", 3) == 0);

  EXPECT_TRUE((buffer2->put(key, 2)) == ETCH_OK);
  EXPECT_TRUE(memcmp(buffer2->getBuffer(), "ab", 2) == 0);

  buffer->setIndex(0);
  EXPECT_TRUE((buffer2->put(*buffer)) == ETCH_OK);
  EXPECT_TRUE(memcmp(buffer2->getBuffer(), "ababc", 5) == 0);

  EXPECT_TRUE((buffer2->put(*buffer, 1)) == ETCH_OK);
  EXPECT_TRUE(memcmp(buffer2->getBuffer(), "ababca", 6) == 0);

  buffer2->setLength(32);
  buffer2->setIndex(31);
  EXPECT_TRUE((buffer2->put(*buffer, 2)) == ETCH_OK);
  EXPECT_TRUE(strncmp((const char *) &buffer2->getBuffer()[32], "b", 1) == 0);

  delete buffer;
  delete buffer2;
}

TEST(EtchFlexBufferTest, getTests) {
  EtchFlexBuffer *buffer = new EtchFlexBuffer();
  capu::int8_t key [] = {'a', 'b', 'c'};

  EXPECT_TRUE(buffer->put(NULL, 32) == ETCH_EINVAL);

  EXPECT_TRUE(buffer->put(key, 0) == ETCH_ERANGE);

  //check the length of valid buffer
  EXPECT_TRUE(buffer->getLength() == 0);
  //check the index position after the insert
  EXPECT_TRUE(buffer->getIndex() == 0);
  //check that there is no new valid buffer to read
  EXPECT_TRUE(buffer->getAvailableBytes() == 0);

  EXPECT_TRUE(buffer->put(key, 3) == ETCH_OK);
  EXPECT_TRUE(memcmp(buffer->getBuffer(), "abc", 3) == 0);

  //check the length of valid buffer
  EXPECT_TRUE(buffer->getLength() == 3);
  //check the current size of the buffer
  EXPECT_TRUE(buffer->getSize() == 32);
  //check the index position after the insert
  EXPECT_TRUE(buffer->getIndex() == 3);
  //check that there is no new valid buffer to read
  EXPECT_TRUE(buffer->getAvailableBytes() == 0);

  delete buffer;
}

TEST(EtchFlexBufferTest, setIndexTest) {
  EtchFlexBuffer *buffer = new EtchFlexBuffer();
  capu::int8_t key [] = {'a', 'b', 'c'};

  EXPECT_TRUE(buffer->getIndex() == 0);
  EXPECT_TRUE(buffer->setIndex(129) == ETCH_EINVAL);

  EXPECT_TRUE(buffer->setIndex(0) == ETCH_OK);
  EXPECT_TRUE(buffer->put(key, 3) == ETCH_OK);
  EXPECT_TRUE(memcmp(buffer->getBuffer(), "abc", 3) == 0);

  EXPECT_TRUE(buffer->setIndex(1) == ETCH_OK);
  EXPECT_TRUE(buffer->put(key, 3) == ETCH_OK);
  EXPECT_TRUE(memcmp(buffer->getBuffer(), "aabc", 3) == 0);

  delete buffer;
}

TEST(EtchFlexBufferTest, resetTest) {
  EtchFlexBuffer *buffer = new EtchFlexBuffer();
  capu::int8_t key [] = {'a', 'b', 'c'};

  EXPECT_TRUE(buffer->getIndex() == 0);
  EXPECT_TRUE(buffer->setIndex(129) == ETCH_EINVAL);

  EXPECT_TRUE(buffer->setIndex(0) == ETCH_OK);
  EXPECT_TRUE(buffer->put(key, 3) == ETCH_OK);
  EXPECT_TRUE(buffer->getIndex() == 3);
  buffer->reset();
  EXPECT_TRUE(buffer->getIndex() == 0);
  EXPECT_TRUE(buffer->getLength() == 0);

  delete buffer;
}

TEST(EtchFlexBufferTest, getTest) {
  EtchFlexBuffer *buffer = new EtchFlexBuffer();
  capu::int8_t key [] = {'a', 'b', 'c'};
  capu::int8_t key2 [3];
  buffer->put(key, 3);
  buffer->setIndex(0);
  capu::uint32_t amount = 0;
  //null
  EXPECT_TRUE(buffer->get(NULL, 4, 5, amount) == ETCH_EINVAL);
  EXPECT_TRUE(amount == 0);
  //if the given length is too long just return the available buffer
  EXPECT_TRUE(buffer->get(key2, 0, 5, amount) == ETCH_OK);
  EXPECT_TRUE(amount == 3);

  //check result
  EXPECT_TRUE(memcmp(key, key2, 3) == 0);
  buffer->setIndex(0);
  //just partial read
  EXPECT_TRUE(buffer->get(key2, 0, 1, amount) == ETCH_OK);
  EXPECT_TRUE(amount == 1);
  //check result
  EXPECT_TRUE(key2[0] == key[0]);
  delete buffer;
}

TEST(EtchFlexBufferTest, expansionTest) {
  EtchFlexBuffer *buffer = new EtchFlexBuffer();

  EXPECT_TRUE(buffer->put(NULL, 32) == ETCH_EINVAL);

  //Default size of buffer
  EXPECT_TRUE(buffer->getSize() == 32);
  //Try to expand buffer exceeding the maximum limit
  EXPECT_TRUE(buffer->setLength((4 * 1024 * 1024) + 1) == ETCH_ENO_MEMORY);
  //Still in default size because it is not changed yet
  EXPECT_TRUE(buffer->getSize() == 32);
  //Expand buffer
  EXPECT_TRUE(buffer->setLength(1024 * 1024) == ETCH_OK);
  //check the size of buffer
  EXPECT_TRUE(buffer->getSize() == (1024 * 1024));

  //reset and trim
  buffer->reset();
  //After Trim
  EXPECT_TRUE(buffer->getSize() == (16 * 1024));

  delete buffer;
}

TEST(EtchFlexBufferTest, putAndgetIntegerTest) {
  EtchFlexBuffer *buffer = new EtchFlexBuffer();
  capu::int8_t key [] = {'a', 'b', 'c'};

  capu::uint32_t value = 5;
  EXPECT_TRUE(buffer->put(NULL, 32) == ETCH_EINVAL);
  //try to read an integer
  EXPECT_TRUE(buffer->getInteger(value) == ETCH_ERANGE);
  EXPECT_TRUE(buffer->putInt(value) == ETCH_OK);
  value = 0;
  //normal read
  buffer->setIndex(0);
  EXPECT_TRUE(buffer->getInteger(value) == ETCH_OK);
  EXPECT_TRUE(value == 5);
  buffer->reset();
  //let assume that there is only 3 byte on flex buffer
  EXPECT_TRUE(buffer->put(key, 3) == ETCH_OK);
  buffer->setIndex(0);
  EXPECT_TRUE(buffer->getInteger(value) == ETCH_ERANGE);

  delete buffer;
}

TEST(EtchFlexBufferTest, putAndgetByte) {
  EtchFlexBuffer *buffer = new EtchFlexBuffer();

  capu::int8_t value = 5;
  //try to read an integer
  EXPECT_TRUE(buffer->getByte(value) == ETCH_ERANGE);
  EXPECT_TRUE(buffer->putByte(value) == ETCH_OK);
  value = 0;
  //normal read
  buffer->setIndex(0);
  EXPECT_TRUE(buffer->getByte(value) == ETCH_OK);
  EXPECT_TRUE(value == 5);
  buffer->reset();
  //let assume that there is only 0 byte on flex buffer
  buffer->setIndex(0);
  EXPECT_TRUE(buffer->getByte(value) == ETCH_ERANGE);

  delete buffer;
}

TEST(EtchFlexBufferTest, putAndgetShort) {
  EtchFlexBuffer *buffer = new EtchFlexBuffer();
  capu::int8_t key [] = {'a'};

  capu::int16_t value = 5;
  //try to read an integer
  EXPECT_TRUE(buffer->getShort(value) == ETCH_ERANGE);
  EXPECT_TRUE(buffer->putShort(value) == ETCH_OK);
  value = 0;
  //normal read
  buffer->setIndex(0);
  EXPECT_TRUE(buffer->getShort(value) == ETCH_OK);
  EXPECT_TRUE(value == 5);
  buffer->reset();
  //let assume that there is only 1 byte on flex buffer
  EXPECT_TRUE(buffer->put(key, 1) == ETCH_OK);
  buffer->setIndex(0);
  EXPECT_TRUE(buffer->getShort(value) == ETCH_ERANGE);

  delete buffer;
}

TEST(EtchFlexBufferTest, putAndgetLong) {
  EtchFlexBuffer *buffer = new EtchFlexBuffer();
  capu::int8_t key [] = {'a', 'b', 'c'};

  capu::int64_t value = 5;
  //try to read an integer
  EXPECT_TRUE(buffer->getLong(value) == ETCH_ERANGE);
  EXPECT_TRUE(buffer->putLong(value) == ETCH_OK);
  value = 0;
  //normal read
  buffer->setIndex(0);
  EXPECT_TRUE(buffer->getLong(value) == ETCH_OK);
  EXPECT_TRUE(value == 5);
  buffer->reset();
  //let assume that there is only 3 byte on flex buffer
  EXPECT_TRUE(buffer->put(key, 3) == ETCH_OK);
  buffer->setIndex(0);
  EXPECT_TRUE(buffer->getLong(value) == ETCH_ERANGE);

  delete buffer;
}

TEST(EtchFlexBufferTest, putAndgetFloat) {
  EtchFlexBuffer *buffer = new EtchFlexBuffer();
  capu::int8_t key [] = {'a', 'b', 'c'};

  capu::float_t value = 5.3f;
  //try to read an integer
  EXPECT_TRUE(buffer->getFloat(value) == ETCH_ERANGE);
  EXPECT_TRUE(buffer->putFloat(value) == ETCH_OK);
  value = 0;
  //normal read
  buffer->setIndex(0);
  EXPECT_TRUE(buffer->getFloat(value) == ETCH_OK);
  EXPECT_TRUE(value == 5.3f);
  buffer->reset();
  //let assume that there is only 3 byte on flex buffer
  EXPECT_TRUE(buffer->put(key, 3) == ETCH_OK);
  buffer->setIndex(0);
  EXPECT_TRUE(buffer->getFloat(value) == ETCH_ERANGE);

  delete buffer;
}

TEST(EtchFlexBufferTest, putAndgetDouble) {
  EtchFlexBuffer *buffer = new EtchFlexBuffer();
  capu::int8_t key [] = {'a', 'b', 'c'};

  capu::double_t value = 5.2;
  //try to read an integer
  EXPECT_TRUE(buffer->getDouble(value) == ETCH_ERANGE);
  EXPECT_TRUE(buffer->putDouble(value) == ETCH_OK);
  value = 0;
  //normal read
  buffer->setIndex(0);
  EXPECT_TRUE(buffer->getDouble(value) == ETCH_OK);
  EXPECT_TRUE(value == 5.2);
  buffer->reset();
  //let assume that there is only 3 byte on flex buffer
  EXPECT_TRUE(buffer->put(key, 3) == ETCH_OK);
  buffer->setIndex(0);
  EXPECT_TRUE(buffer->getDouble(value) == ETCH_ERANGE);

  delete buffer;
}


