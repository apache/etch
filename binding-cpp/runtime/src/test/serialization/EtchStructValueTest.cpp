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
#include "serialization/EtchField.h"
#include "serialization/EtchStructValue.h"
#include "serialization/EtchValueFactory.h"
#include "serialization/EtchValidatorBoolean.h"
#include "serialization/EtchValidatorByte.h"

class MockValueFactory_FULL : public virtual EtchValueFactory {
public:

  ~MockValueFactory_FULL() {

  }
  MOCK_METHOD2(getType, status_t(capu::uint32_t id, EtchType*& result));

  MOCK_METHOD2(getType, status_t(EtchString &name, EtchType *&result));

  MOCK_METHOD1(addType, status_t(EtchType *type));

  MOCK_METHOD0(lockDynamicTypes, status_t());

  MOCK_METHOD0(unlockDynamicTypes, status_t());

  MOCK_METHOD2(getMessageId, status_t(EtchMessage* msg, capu::int64_t &result));

  MOCK_METHOD2(setMessageId, status_t(EtchMessage* msg, capu::int64_t msgid));

  MOCK_METHOD0(get_mf__messageId, EtchField());

  MOCK_METHOD2(getInReplyToMessageId, status_t(EtchMessage* msg, capu::int64_t &result));

  MOCK_METHOD2(setInReplyToMessageId, status_t(EtchMessage* msg, capu::int64_t msgid));

  MOCK_METHOD0(get_mf__inReplyTo, EtchField());

  MOCK_METHOD2(importCustomValue, status_t(EtchStructValue* _struct, capu::SmartPointer<EtchObject> &result));

  MOCK_METHOD2(exportCustomValue, status_t(capu::SmartPointer<EtchObject> value, EtchStructValue *&result));

  MOCK_METHOD0(get_mt__exception, EtchType*());

  MOCK_METHOD2(getCustomStructType, status_t(const EtchObjectType *c, EtchType *& type));

  MOCK_METHOD4(getNativeArrayForComponentType, status_t(const EtchObjectType *objectType,  capu::SmartPointer<EtchNativeArrayBase> &nativeArray, capu::int32_t length, capu::int32_t dim));

  EtchLevel getLevel() {
    return LEVEL_FULL;
  }

  MOCK_METHOD1(setLevel, EtchLevel(EtchLevel level));

};

class MockValueFactory_MISSING_OK : public virtual EtchValueFactory {
public:

  ~MockValueFactory_MISSING_OK() {

  }
  MOCK_METHOD2(getType, status_t(capu::uint32_t id, EtchType*& result));

  MOCK_METHOD2(getType, status_t(EtchString &name, EtchType *&result));

  MOCK_METHOD1(addType, status_t(EtchType* type));

  MOCK_METHOD0(lockDynamicTypes, status_t());

  MOCK_METHOD0(unlockDynamicTypes, status_t());

  MOCK_METHOD2(getMessageId, status_t(EtchMessage* msg, capu::int64_t &result));

  MOCK_METHOD2(setMessageId, status_t(EtchMessage* msg, capu::int64_t msgid));

  MOCK_METHOD0(get_mf__messageId, EtchField());

  MOCK_METHOD2(getInReplyToMessageId, status_t(EtchMessage* msg, capu::int64_t &result));

  MOCK_METHOD2(setInReplyToMessageId, status_t(EtchMessage* msg, capu::int64_t msgid));

  MOCK_METHOD0(get_mf__inReplyTo, EtchField());

  MOCK_METHOD2(importCustomValue, status_t(EtchStructValue* _struct, capu::SmartPointer<EtchObject> &result));

  MOCK_METHOD2(exportCustomValue, status_t(capu::SmartPointer<EtchObject> value, EtchStructValue *&result));

  MOCK_METHOD0(get_mt__exception, EtchType*());

  MOCK_METHOD2(getCustomStructType, status_t(const EtchObjectType *c, EtchType *& type));

  MOCK_METHOD4(getNativeArrayForComponentType, status_t(const EtchObjectType *objectType,  capu::SmartPointer<EtchNativeArrayBase> &nativeArray, capu::int32_t length, capu::int32_t dim));

  EtchLevel getLevel() {
    return LEVEL_MISSING_OK;
  }

  MOCK_METHOD1(setLevel, EtchLevel(EtchLevel level));

};

class MockValueFactory_NONE : public virtual EtchValueFactory {
public:

  ~MockValueFactory_NONE() {

  }
  MOCK_METHOD2(getType, status_t(capu::uint32_t id, EtchType*& result));

  MOCK_METHOD2(getType, status_t(EtchString &name, EtchType *&result));

  MOCK_METHOD1(addType, status_t(EtchType* type));

  MOCK_METHOD0(lockDynamicTypes, status_t());

  MOCK_METHOD0(unlockDynamicTypes, status_t());

  MOCK_METHOD2(getMessageId, status_t(EtchMessage* msg, capu::int64_t &result));

  MOCK_METHOD2(setMessageId, status_t(EtchMessage* msg, capu::int64_t msgid));

  MOCK_METHOD0(get_mf__messageId, EtchField());

  MOCK_METHOD2(getInReplyToMessageId, status_t(EtchMessage* msg, capu::int64_t &result));

  MOCK_METHOD2(setInReplyToMessageId, status_t(EtchMessage* msg, capu::int64_t msgid));

  MOCK_METHOD0(get_mf__inReplyTo, EtchField());

  MOCK_METHOD2(importCustomValue, status_t(EtchStructValue* _struct, capu::SmartPointer<EtchObject>  &result));

  MOCK_METHOD2(exportCustomValue, status_t(capu::SmartPointer<EtchObject> value, EtchStructValue *&result));

  MOCK_METHOD0(get_mt__exception, EtchType*());

  MOCK_METHOD2(getCustomStructType, status_t(const EtchObjectType *c, EtchType *& type));

  MOCK_METHOD4(getNativeArrayForComponentType, status_t(const EtchObjectType *objectType,  capu::SmartPointer<EtchNativeArrayBase> &nativeArray, capu::int32_t length, capu::int32_t dim));

  EtchLevel getLevel() {
    return LEVEL_NONE;
  }

  MOCK_METHOD1(setLevel, EtchLevel(EtchLevel level));

};

class EtchStructValueTest
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

TEST_F(EtchStructValueTest, createTest) {
  EtchString fieldtype1("type1");
  EtchString fieldtype2("type2");
  EtchField field1(fieldtype1);
  EtchField field2(fieldtype2);
  EtchString typeName("comp");
  //CreateType
  EtchType* comp = new EtchType(90, typeName);

  capu::SmartPointer<EtchValidator> v1;
  capu::SmartPointer<EtchValidator> v2;
  EtchValidatorBoolean::Get(mRuntime, 0, v1);
  EtchValidatorBoolean::Get(mRuntime, 0, v2);

  comp->putValidator(field1, v1);
  comp->putValidator(field2, v2);

  EtchStructValue *ptr = NULL;
  //Create Value Factory
  EtchValueFactory* factory = new MockValueFactory_FULL();
  ptr = new EtchStructValue(comp, factory);

  delete ptr;
  delete factory;
  delete comp;
}

TEST_F(EtchStructValueTest, getTypeTest) {
  EtchString fieldtype1("type1");
  EtchString fieldtype2("type2");
  EtchField field1(fieldtype1);
  EtchField field2(fieldtype2);
  EtchString typeName("comp");
  //CreateType
  EtchType* comp = new EtchType(90, typeName);

  capu::SmartPointer<EtchValidator> v1;
  capu::SmartPointer<EtchValidator> v2;
  EtchValidatorBoolean::Get(mRuntime, 0, v1);
  EtchValidatorBoolean::Get(mRuntime, 0, v2);

  comp->putValidator(field1, v1);
  comp->putValidator(field2, v2);
  //Create Value Factory
  EtchValueFactory* factory = new MockValueFactory_FULL();
  EtchStructValue *sv = new EtchStructValue(comp, factory);
  EXPECT_TRUE(sv->getType()->equals(comp));
  delete sv;
  delete factory;
  delete comp;
}

TEST_F(EtchStructValueTest, isType) {
  //Type 1
  EtchString fieldtype1("type1");
  EtchString fieldtype2("type2");
  EtchField field1(fieldtype1);
  EtchField field2(fieldtype2);
  EtchString typeName("comp");
  //CreateType
  EtchType* comp = new EtchType(90, typeName);

  capu::SmartPointer<EtchValidator> v1;
  capu::SmartPointer<EtchValidator> v2;
  EtchValidatorBoolean::Get(mRuntime, 0, v1);
  EtchValidatorBoolean::Get(mRuntime, 0, v2);

  comp->putValidator(field1, v1);
  comp->putValidator(field2, v2);
  //Create Value Factory
  EtchValueFactory* factory = new MockValueFactory_FULL();
  //type 2
  EtchString typeName2("comp2");
  EtchType* comp2 = new EtchType(91, typeName2);

  EtchStructValue *sv = new EtchStructValue(comp, factory);
  EXPECT_TRUE(sv->isType(comp));
  EXPECT_FALSE(sv->isType(comp2));
  delete sv;
  delete factory;
  delete comp;
  delete comp2;
}

TEST_F(EtchStructValueTest, putTest_FULL) {
  EtchString fieldtype1("type1");
  EtchString fieldtype2("type2");
  EtchString fieldtype3("type3");
  EtchString typeName("comp");

  EtchField field1(fieldtype1);
  EtchField field2(fieldtype2);
  EtchField field3(fieldtype3);
  //Create Value Factory
  EtchValueFactory* factory = new MockValueFactory_FULL();
  //CreateType
  EtchType* comp = new EtchType(90, typeName);

  capu::SmartPointer<EtchValidator> v1;
  capu::SmartPointer<EtchValidator> v2;
  EtchValidatorBoolean::Get(mRuntime, 0, v1);
  EtchValidatorBoolean::Get(mRuntime, 0, v2);

  comp->putValidator(field1, v1);
  comp->putValidator(field2, v2);

  EtchStructValue *sv = new EtchStructValue(comp, factory);

  capu::SmartPointer<EtchObject> object = new EtchBool(true);
  capu::SmartPointer<EtchObject> object2 = new EtchBool(false);
  capu::SmartPointer<EtchObject> object3 = NULL;
  capu::SmartPointer<EtchObject> object5 = new EtchInt32(32);

  //check the empty struct value
  EXPECT_TRUE(sv->count() == 0);
  //add new element
  EXPECT_TRUE(sv->put(field1, object) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 1);
  //add another element
  EXPECT_TRUE(sv->put(field2, object2) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 2);
  //add another element with missing validator
  EXPECT_TRUE(sv->put(field3, object5) == ETCH_ENOT_EXIST);
  EXPECT_TRUE(sv->count() == 2);
  //add a null element and expect a remove operation
  EXPECT_TRUE(sv->put(field1, object3) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 1);

  delete sv;
  delete factory;
  delete comp;
}

TEST_F(EtchStructValueTest, putTest_MISSING_OK) {
  EtchString fieldtype1("type1");
  EtchString fieldtype2("type2");
  EtchString fieldtype3("type3");
  EtchString typeName("comp");

  EtchField field1(fieldtype1);
  EtchField field2(fieldtype2);
  EtchField field3(fieldtype3);
  //Create Value Factory
  EtchValueFactory* factory = new MockValueFactory_MISSING_OK();
  //CreateType
  EtchType* comp = new EtchType(90, typeName);

  capu::SmartPointer<EtchValidator> v1;
  capu::SmartPointer<EtchValidator> v2;
  EtchValidatorBoolean::Get(mRuntime, 0, v1);
  EtchValidatorBoolean::Get(mRuntime, 0, v2);

  comp->putValidator(field1, v1);
  comp->putValidator(field2, v2);

  EtchStructValue *sv = new EtchStructValue(comp, factory);

  capu::SmartPointer<EtchObject> object = new EtchBool(true);
  capu::SmartPointer<EtchObject> object2 = new EtchBool(false);
  capu::SmartPointer<EtchObject> object3 = NULL;
  capu::SmartPointer<EtchObject> object5 = new EtchInt32(32);

  //check the empty struct value
  EXPECT_TRUE(sv->count() == 0);
  //add new element
  EXPECT_TRUE(sv->put(field1, object) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 1);
  //add another element
  EXPECT_TRUE(sv->put(field2, object2) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 2);
  //add another element with missing validator
  EXPECT_TRUE(sv->put(field3, object5) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 3);
  //add a null element and expect a remove operation
  EXPECT_TRUE(sv->put(field1, object3) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 2);

  delete sv;
  delete factory;
  delete comp;
}

TEST_F(EtchStructValueTest, putTest_NONE) {
  EtchString fieldtype1("type1");
  EtchString fieldtype2("type2");
  EtchString fieldtype3("type3");
  EtchString typeName("comp");

  EtchField field1(fieldtype1);
  EtchField field2(fieldtype2);
  EtchField field3(fieldtype3);
  //Create Value Factory
  EtchValueFactory* factory = new MockValueFactory_NONE();
  //CreateType
  EtchType* comp = new EtchType(90, typeName);

  capu::SmartPointer<EtchValidator> v1;
  capu::SmartPointer<EtchValidator> v2;
  EtchValidatorBoolean::Get(mRuntime, 0, v1);
  EtchValidatorBoolean::Get(mRuntime, 0, v2);

  comp->putValidator(field1, v1);
  comp->putValidator(field2, v2);

  EtchStructValue *sv = new EtchStructValue(comp, factory);

  capu::SmartPointer<EtchObject> object = new EtchBool(true);
  capu::SmartPointer<EtchObject> object2 = new EtchBool(false);
  capu::SmartPointer<EtchObject> object3 = NULL;
  capu::SmartPointer<EtchObject> object5 = new EtchInt32(32);

  //check the empty struct value
  EXPECT_TRUE(sv->count() == 0);
  //add new element
  EXPECT_TRUE(sv->put(field1, object) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 1);
  //add another element
  EXPECT_TRUE(sv->put(field2, object2) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 2);
  //add another element with missing validator
  EXPECT_TRUE(sv->put(field3, object5) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 3);
  //add a null element and expect a remove operation
  EXPECT_TRUE(sv->put(field1, object3) == ETCH_OK);
  EXPECT_TRUE(sv->count() == 2);

  delete sv;
  delete factory;
  delete comp;
}

TEST_F(EtchStructValueTest, getTest) {
  EtchString fieldtype1("type1");
  EtchString fieldtype2("type2");
  EtchField field1(fieldtype1);
  EtchField field2(fieldtype2);
  EtchString typeName("comp");
  EtchType* comp = new EtchType(90, typeName);

  capu::SmartPointer<EtchValidator> v1;
  capu::SmartPointer<EtchValidator> v2;
  EtchValidatorBoolean::Get(mRuntime, 0, v1);
  EtchValidatorBoolean::Get(mRuntime, 0, v2);

  comp->putValidator(field1, v1);
  comp->putValidator(field2, v2);

  //Create Value Factory
  EtchValueFactory* factory = new MockValueFactory_FULL();
  //create struct value
  EtchStructValue *sv = new EtchStructValue(comp, factory);
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

TEST_F(EtchStructValueTest, removeTest) {
  EtchString fieldtype1("type1");
  EtchString typeName("comp");
  EtchField field1(fieldtype1);
  EtchValueFactory* factory = new MockValueFactory_FULL();

  EtchType* comp = new EtchType(90, typeName);

  capu::SmartPointer<EtchValidator> v1;
  EtchValidatorBoolean::Get(mRuntime, 0, v1);
  comp->putValidator(field1, v1);

  EtchStructValue *sv = new EtchStructValue(comp, factory);
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
  capu::SmartPointer<EtchObject> boolean = new EtchBool(true);
  sv->put(field1, boolean);
  EXPECT_TRUE(sv->count() == 1);
  EXPECT_TRUE(sv->get(field1, &object) == ETCH_OK);
  capu::SmartPointer<EtchBool> smBoolean = capu::smartpointer_cast<EtchBool>(object);
  EXPECT_TRUE(smBoolean->get() == true);

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

TEST_F(EtchStructValueTest, isInstanceOf) {
  EtchString typeName("comp");
  EtchType* comp = new EtchType(90, typeName);
  EtchValueFactory* factory = new MockValueFactory_FULL();
  EtchObject* o1 = new EtchStructValue(comp, factory);
  EXPECT_TRUE(o1->isInstanceOf(EtchObject::TYPE()));
  EXPECT_TRUE(o1->isInstanceOf(EtchStructValue::TYPE()));
  EXPECT_FALSE(o1->isInstanceOf(EtchInt32::TYPE()));
  delete o1;
  delete comp;
  delete factory;
}
