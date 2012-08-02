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
#include "capu/os/File.h"

TEST(File, ConstructorTest) {
  capu::File* f1 = new capu::File("foobar.txt", "r");
  EXPECT_TRUE(f1 != NULL);
  EXPECT_FALSE(f1->isOpen());
  delete f1;

  capu::File* f2 = new capu::File("test.txt", "w");
  EXPECT_TRUE(f1 != NULL);
  EXPECT_TRUE(f1->isOpen());
  delete f2;
}

TEST(File, IsOpenTest) {
  capu::File* f1 = new capu::File("foobar.txt", "r");
  EXPECT_TRUE(f1 != NULL);
  EXPECT_FALSE(f1->isOpen());
  delete f1;

  capu::File* f2 = new capu::File("test.txt", "w");
  EXPECT_TRUE(f2 != NULL);
  EXPECT_TRUE(f2->isOpen());
  delete f2;
}

TEST(File, WriteTest) {
  char buf1[15] = "This is a test";
  capu::status_t status;

  capu::File* f1 = new capu::File("test.txt", "w");
  EXPECT_TRUE(f1 != NULL);
  EXPECT_TRUE(f1->isOpen());

  // invalid params
  status = f1->write(NULL, 0);
  EXPECT_TRUE(status == capu::CAPU_EINVAL);

  // write data
  status = f1->write(buf1, sizeof(buf1)-1);
  EXPECT_TRUE(status == capu::CAPU_OK);

  delete f1;
}

TEST(File, ReadTest) {
  char buf1[20] = "This is a test";
  char buf2[20];
  
  capu::status_t status;
  capu::int32_t read = 0;

  // read data
  capu::File* f1 = new capu::File("test1.txt", "r");
  EXPECT_TRUE(f1 != NULL);
  EXPECT_FALSE(f1->isOpen());

  // file not open
  status = f1->write(buf2, strlen(buf1));
  EXPECT_TRUE(status == capu::CAPU_ERROR);

  delete f1;

  // write data
  capu::File* f2 = new capu::File("test.txt", "w");
  EXPECT_TRUE(f2 != NULL);
  EXPECT_TRUE(f2->isOpen());

  status = f2->write(buf1, strlen(buf1));
  EXPECT_TRUE(status == capu::CAPU_OK);
  delete f2;

  // read data
  capu::File* f3 = new capu::File("test.txt", "r");
  EXPECT_TRUE(f3 != NULL);
  EXPECT_TRUE(f3->isOpen());

  // invalid params
  status = f3->read(NULL, 0, NULL);
  EXPECT_TRUE(status == capu::CAPU_EINVAL);

  read = 0;
  memset(buf2, 0, sizeof(buf2));
  status = f3->read(buf2, strlen(buf1), &read);
  EXPECT_TRUE(status == capu::CAPU_OK);
  EXPECT_TRUE(read == strlen(buf1));
  delete f3;

  // read data
  capu::File* f4 = new capu::File("test.txt", "r");
  EXPECT_TRUE(f4 != NULL);
  EXPECT_TRUE(f4->isOpen());

  memset(buf2, 0, sizeof(buf2));
  status = f4->read(buf2, strlen(buf1), NULL);
  EXPECT_TRUE(status == capu::CAPU_OK);
  EXPECT_TRUE(read == strlen(buf1));
  delete f4;

  // read data Eof
  capu::File* f5 = new capu::File("test.txt", "r");
  EXPECT_TRUE(f5 != NULL);
  EXPECT_TRUE(f5->isOpen());

  read = 0;
  memset(buf2, 0, sizeof(buf2));
  status = f5->read(buf2, sizeof(buf2), &read);
  EXPECT_TRUE(status == capu::CAPU_EOF);
  EXPECT_TRUE(read == strlen(buf1));
  EXPECT_TRUE(strcmp(buf1, buf2) == 0);
  delete f5;
}
