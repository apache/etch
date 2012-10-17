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
#include "common/EtchString.h"

class EtchObjectMy1Object : public EtchObject {
public:

   /**
   * EtchObjectType for EtchObjectMyObject.
   */
  static const EtchObjectType* TYPE() {
    const static EtchObjectType TYPE(11, NULL);
    return &TYPE;
  }

  /**
   * Constructor
   */
  EtchObjectMy1Object()
    : EtchObject() {
      EtchObject::addObjectType(TYPE());
  }

};

class EtchObjectMy2Object : public EtchObject {
public:

   /**
   * EtchObjectType for EtchObjectMyObject.
   */
  static const EtchObjectType* TYPE() {
    const static EtchObjectType TYPE(12, NULL);
    return &TYPE;
  }

  /**
   * Constructor
   */
  EtchObjectMy2Object()
    : EtchObject() {
      EtchObject::addObjectType(TYPE());
  }

};


TEST(EtchObjectTest, getTypeTrait) {
  EXPECT_EQ(EtchObjectType::VALUE, EtchObjectType::getTypeTrait<EtchString>());
  EXPECT_EQ(EtchObjectType::POINTER, EtchObjectType::getTypeTrait<EtchString*>());
  EXPECT_EQ(EtchObjectType::SMART_POINTER, EtchObjectType::getTypeTrait<capu::SmartPointer<EtchString> >());

}

TEST(EtchObjectTest, isInstanceOf) {
  EtchObjectMy1Object* o1 = new EtchObjectMy1Object();
  EXPECT_TRUE(o1->isInstanceOf(EtchObjectMy1Object::TYPE()));
  EXPECT_TRUE(o1->isInstanceOf(EtchObject::TYPE()));
  EXPECT_FALSE(o1->isInstanceOf(EtchObjectMy2Object::TYPE()));

  delete o1;
}
