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
#include <gmock/gmock.h>
#include "transport/EtchMessage.h"
#include "serialization/EtchValidatorBoolean.h"
#include "support/EtchRuntime.h"

class MockValueFactory7 : public virtual EtchValueFactory {
public:

  ~MockValueFactory7() {

  }
  MOCK_METHOD2(getType, status_t(capu::uint32_t id, EtchType*& result));

  MOCK_METHOD2(getType, status_t(EtchString& name, EtchType*& result));

  MOCK_METHOD1(addType, status_t(EtchType* type));

  MOCK_METHOD0(lockDynamicTypes, status_t());

  MOCK_METHOD0(unlockDynamicTypes, status_t());

  status_t getMessageId(EtchMessage* msg, capu::int64_t &result) {
    result = 0;
    return ETCH_OK;
  }

  MOCK_METHOD0(get_mf__messageId, EtchField());

  status_t getInReplyToMessageId(EtchMessage* msg, capu::int64_t &result) {
    result = 0;
    return ETCH_OK;
  }
  MOCK_METHOD2(setMessageId, status_t(EtchMessage* msg, capu::int64_t msgid));

  MOCK_METHOD2(setInReplyToMessageId, status_t(EtchMessage* msg, capu::int64_t msgid));

  MOCK_METHOD0(get_mf__inReplyTo, EtchField());

  MOCK_METHOD2(importCustomValue, status_t(EtchStructValue* _struct, capu::SmartPointer<EtchObject> &result));

  MOCK_METHOD2(exportCustomValue, status_t(capu::SmartPointer<EtchObject> value, EtchStructValue*& result));

  MOCK_METHOD0(get_mt__exception, EtchType*());

  MOCK_METHOD2(getCustomStructType, status_t(const EtchObjectType *c, EtchType *& type));

  MOCK_METHOD4(getNativeArrayForComponentType, status_t(const EtchObjectType *objectType,  capu::SmartPointer<EtchNativeArrayBase> &nativeArray, capu::int32_t length, capu::int32_t dim));

  EtchLevel getLevel() {
    return LEVEL_FULL;
  }

  MOCK_METHOD1(setLevel, EtchLevel(EtchLevel level));
private:
  capu::int64_t tmp;
};

class EtchMessageTest
  : public ::testing::Test {
protected:
  virtual void SetUp() {
    mRuntime = new EtchRuntime();
    mRuntime->start();
  }

  virtual void TearDown() {
    mRuntime->shutdown();
    delete mRuntime;
    mRuntime = NULL;

  }
  EtchRuntime* mRuntime;
};


TEST_F(EtchMessageTest, createTest) {
  EtchType *type = new EtchType("test");
  EtchValueFactory *factory = new MockValueFactory7();
  EtchMessage *message = new EtchMessage(type, factory);
  EXPECT_TRUE(message != NULL);
  delete message;
  delete type;
  delete factory;
}

TEST_F(EtchMessageTest, getMessageIdTests) {
  EtchType *type = new EtchType("test");
  EtchValueFactory *factory = new MockValueFactory7();
  EtchMessage *message = new EtchMessage(type, factory);
  EXPECT_TRUE(message != NULL);
  capu::int64_t tmp;
  EXPECT_TRUE(message->getInReplyToMessageId(tmp) == ETCH_OK);
  EXPECT_TRUE(message->getMessageId(tmp) == ETCH_OK);
  delete message;
  delete type;
  delete factory;
}

TEST_F(EtchMessageTest, getTypeTest) {
  EtchField field1("type1");
  EtchField field2("type2");

  //CreateType
  EtchType* comp = new EtchType(90, "comp");

  //fill validators
  capu::SmartPointer<EtchValidator> val1;
  EtchValidatorBoolean::Get(mRuntime, 0, val1);
  capu::SmartPointer<EtchValidator> val2;
  EtchValidatorBoolean::Get(mRuntime, 0, val2);
  comp->putValidator(field1, val1);
  comp->putValidator(field2, val2);

  //Create Value Factory
  EtchValueFactory* factory = new MockValueFactory7();

  //create message
  EtchMessage *sv = new EtchMessage(comp, factory);
  EXPECT_TRUE(sv->getType()->equals(comp));
  delete sv;
  delete factory;
  delete comp;
}

TEST_F(EtchMessageTest, isType) {
  //Type 1
  EtchField field1("type1");
  EtchField field2("type2");

  //CreateType
  EtchType* comp = new EtchType(90, "comp");

  capu::SmartPointer<EtchValidator> val1;
  EtchValidatorBoolean::Get(mRuntime, 0, val1);
  capu::SmartPointer<EtchValidator> val2;
  EtchValidatorBoolean::Get(mRuntime, 0, val2);
  comp->putValidator(field1, val1);
  comp->putValidator(field2, val2);
  //Create Value Factory
  EtchValueFactory* factory = new MockValueFactory7();
  //type 2
  EtchString typeName2("comp2");
  EtchType* comp2 = new EtchType(91, typeName2);

  EtchMessage *sv = new EtchMessage(comp, factory);
  EXPECT_TRUE(sv->isType(comp));
  EXPECT_FALSE(sv->isType(comp2));
  delete sv;
  delete factory;
  delete comp;
  delete comp2;
}

TEST_F(EtchMessageTest, putTest) {
  EtchString fieldtype1("type1");
  EtchString fieldtype2("type2");
  EtchString typeName("comp");

  EtchField field1(fieldtype1);
  EtchField field2(fieldtype2);
  //Create Value Factory
  EtchValueFactory* factory = new MockValueFactory7();
  //CreateType
  EtchType* comp = new EtchType(90, typeName);

  capu::SmartPointer<EtchValidator> val1;
  EtchValidatorBoolean::Get(mRuntime, 0, val1);
  capu::SmartPointer<EtchValidator> val2;
  EtchValidatorBoolean::Get(mRuntime, 0, val2);
  comp->putValidator(field1, val1);
  comp->putValidator(field2, val2);

  EtchMessage *sv = new EtchMessage(comp, factory);

  capu::SmartPointer<EtchObject> object = new EtchBool(true);
  capu::SmartPointer<EtchObject> object2 = new EtchBool(false);
  capu::SmartPointer<EtchObject> object3 = NULL;

  //check the empty struct value
  EXPECT_TRUE(sv->count() == 0);
  //add new element
  EXPECT_TRUE(sv->put(field1, object) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 1);
  //add another element
  EXPECT_TRUE(sv->put(field2, object2) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 2);
  //add a null element and expect a remove operation
  EXPECT_TRUE(sv->put(field1, object3) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 1);

  delete sv;
  delete factory;
  delete comp;
}

TEST_F(EtchMessageTest, getTest) {
  EtchString fieldtype1("type1");
  EtchString fieldtype2("type2");
  EtchField field1(fieldtype1);
  EtchField field2(fieldtype2);
  EtchString typeName("comp");
  EtchType* comp = new EtchType(90, typeName);

  capu::SmartPointer<EtchValidator> val1;
  EtchValidatorBoolean::Get(mRuntime, 0, val1);
  capu::SmartPointer<EtchValidator> val2;
  EtchValidatorBoolean::Get(mRuntime, 0, val2);
  comp->putValidator(field1, val1);
  comp->putValidator(field2, val2);

  //Create Value Factory
  EtchValueFactory* factory = new MockValueFactory7();
  //create struct value
  EtchMessage *sv = new EtchMessage(comp, factory);
  EXPECT_TRUE(0 == sv->count());

  //add element
  capu::SmartPointer<EtchObject> object = new EtchBool(true);
  EXPECT_TRUE(sv->put(field1, object) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 1);

  //add another element
  capu::SmartPointer<EtchObject> object2 = new EtchBool(false);
  EXPECT_TRUE(sv->put(field1, object2) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 1);
  //get element
  EXPECT_TRUE(sv->get(field1, &object) == ETCH_OK);
  EXPECT_TRUE(((EtchBool*) object.get())->get() == false);

  delete sv;
  delete factory;
  delete comp;
}

TEST_F(EtchMessageTest, removeTest) {
  EtchString fieldtype1("type1");
  EtchString typeName("comp");
  EtchField field1(fieldtype1);
  EtchValueFactory* factory = new MockValueFactory7();

  EtchType* comp = new EtchType(90, typeName);

  capu::SmartPointer<EtchValidator> val1;
  EtchValidatorBoolean::Get(mRuntime, 0, val1);
  comp->putValidator(field1, val1);

  EtchMessage *sv = new EtchMessage(comp, factory);
  capu::SmartPointer<EtchObject> object;
  capu::SmartPointer<EtchObject> object2;
  //check the empty struct value
  EXPECT_TRUE(sv->count() == 0);
  EXPECT_TRUE(sv->get(field1, &object) == ETCH_ENOT_EXIST);

  //try to delete non existing element
  EXPECT_TRUE(sv->remove(field1, &object) == ETCH_ERANGE);
  EXPECT_TRUE(sv->count() == 0);
  EXPECT_TRUE(sv->get(field1, &object) == ETCH_ENOT_EXIST);

  //add element
  EtchObject* boolean = new EtchBool(true);
  sv->put(field1, boolean);
  EXPECT_TRUE(sv->count() == 1);
  EXPECT_TRUE(sv->get(field1, &object) == ETCH_OK);
  capu::SmartPointer<EtchBool> ptr = capu::smartpointer_cast<EtchBool> (object);
  EXPECT_TRUE((ptr->get()) == true);

  //delete element
  EXPECT_TRUE(sv->remove(field1, &object2) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 0);
  EXPECT_TRUE(sv->get(field1, &object2) == ETCH_ENOT_EXIST);
  //try to delete non existing element
  EXPECT_TRUE(sv->remove(field1, &object2) == ETCH_ERANGE);
  EXPECT_TRUE(sv->count() == 0);
  EXPECT_TRUE(sv->get(field1, &object2) == ETCH_ENOT_EXIST);
  delete sv;
  delete comp;
  delete factory;
}
