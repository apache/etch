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
#include "serialization/EtchDefaultValueFactory.h"
#include "support/EtchRuntime.h"

class EtchDefaultValueFactoryTest
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

TEST_F(EtchDefaultValueFactoryTest, createTest) {
  EtchType *_mt_x;
  EtchType *_mt_y;
  EtchField *_mf_a = new EtchField("a");
  EtchField *_mf_b = new EtchField("b");
  EtchTypeMap *types = new EtchTypeMap();
  EtchClass2TypeMap *class2type = new EtchClass2TypeMap();

  //example types
  EtchString str("x");
  types->get(str, _mt_x);
  str.set("y");
  types->get(str, _mt_y);
  EtchDefaultValueFactory::Init(mRuntime, types, class2type);
  types->lock();
  class2type->lock();

  EtchDefaultValueFactory *test = NULL;
  test = new EtchDefaultValueFactory("tcp:", types, class2type);
  EXPECT_TRUE(test != NULL);

  //clear the added types
  types->clear();
  delete test;
  delete _mf_a;
  delete _mf_b;
  delete types;
  delete class2type;
}

TEST_F(EtchDefaultValueFactoryTest, typeTest) {
  EtchType *_mt_x;
  EtchType *_mt_y;
  EtchField *_mf_a = new EtchField("a");
  EtchField *_mf_b = new EtchField("b");
  EtchTypeMap *types = new EtchTypeMap();
  EtchClass2TypeMap *class2type = new EtchClass2TypeMap();

  //example types
  EtchString str("x");
  types->get(str, _mt_x);
  str.set("y");
  types->get(str, _mt_y);
  EtchDefaultValueFactory::Init(mRuntime, types, class2type);
  types->lock();
  class2type->lock();

  EtchDefaultValueFactory *test = NULL;
  test = new EtchDefaultValueFactory("tcp:", types, class2type);
  EXPECT_TRUE(test != NULL);


  str.set("_Etch_RuntimeException");
  EtchType *runtime_exception;
  test->getType(str, runtime_exception);
  EXPECT_TRUE(runtime_exception->getComponentType()->equals (EtchRuntimeException::TYPE()));
  EXPECT_TRUE(runtime_exception->getName().equals(&str));


  str.set("_Etch_List");
  EtchType *list;
  test->getType(str, list);
  EXPECT_TRUE(list->getComponentType()->equals (EtchList<EtchObject*>::TYPE()));
  EXPECT_TRUE(list->getName().equals(&str));

  str.set("_Etch_Map");
  EtchType *hashTable;
  test->getType(str, hashTable);
  EXPECT_TRUE(hashTable->getComponentType()->equals (EtchHashTable<EtchObject*, EtchObject*>::TYPE()));
  EXPECT_TRUE(hashTable->getName().equals(&str));

  str.set("_Etch_Set");
  EtchType *set;
  test->getType(str, set);
  EXPECT_TRUE(set->getComponentType()->equals (EtchHashSet<EtchObject*>::TYPE()));
  EXPECT_TRUE(set->getName().equals(&str));

  str.set("_Etch_Datetime");
  EtchType *date;
  test->getType(str, date);
  EXPECT_TRUE(date->getComponentType()->equals (EtchDate::TYPE()));
  EXPECT_TRUE(date->getName().equals(&str));

  str.set("_Etch_AuthException");
  EtchType *auth_exception;
  test->getType(str, auth_exception);
  EXPECT_TRUE(auth_exception->getComponentType()->equals (EtchAuthenticationException::TYPE()));
  EXPECT_TRUE(auth_exception->getName().equals(&str));

  str.set("_exception");
  EtchType *exception;
  test->getType(str, exception);
  EXPECT_TRUE(exception->getName().equals(&str));

  str.set("x");
  EtchType *x;
  test->getType(str, x);
  EXPECT_TRUE(x->equals(_mt_x));

  str.set("y");
  EtchType *y;
  test->getType(str, y);
  EXPECT_TRUE(y->equals(_mt_y));

  //clear the added types
  types->clear();
  delete test;
  delete _mf_a;
  delete _mf_b;
  delete types;
  delete class2type;
}

TEST_F(EtchDefaultValueFactoryTest, exportCustomValueTest) {
  EtchType *_mt_x;
  EtchType *_mt_y;
  EtchField *_mf_a = new EtchField("a");
  EtchField *_mf_b = new EtchField("b");
  EtchTypeMap *types = new EtchTypeMap();
  EtchClass2TypeMap *class2type = new EtchClass2TypeMap();

  //example types
  EtchString str("x");
  types->get(str, _mt_x);
  str.set("y");
  types->get(str, _mt_y);
  EtchDefaultValueFactory::Init(mRuntime, types, class2type);
  types->lock();
  class2type->lock();

  EtchDefaultValueFactory *test = NULL;
  test = new EtchDefaultValueFactory("tcp:", types, class2type);
  EXPECT_TRUE(test != NULL);

  str.set("Error Message");
  EtchType *type;
  capu::SmartPointer<EtchException> value = new EtchRuntimeException(str, ETCH_ERROR);
  capu::SmartPointer<EtchObject> serialized;
  EtchStructValue* sv;

  //ETCH RUNTIME EXCEPTION EXPORT
  EXPECT_TRUE(test->exportCustomValue(value, sv) == ETCH_OK);
  EtchString strRuntime = "_Etch_RuntimeException";
  test->getType(strRuntime, type);
  EXPECT_TRUE(sv->isType(type));
  EXPECT_EQ(1u, sv->count());
  EXPECT_EQ(ETCH_OK, sv->get(EtchDefaultValueFactory::_mf_msg(), &serialized));
  EXPECT_TRUE(serialized->equals(&str));

  EtchStructValue* sv_unknown;

  //TRY UNKNOWN VALUE
  capu::SmartPointer<EtchObject> unknown = new EtchObject();
  EXPECT_TRUE(test->exportCustomValue(unknown, sv_unknown) == ETCH_EINVAL);

  //clear the added types
  types->clear();
  delete sv;
  delete test;
  delete _mf_a;
  delete _mf_b;
  delete types;
  delete class2type;
}

TEST_F(EtchDefaultValueFactoryTest, importCustomValueTest) {
  EtchType *_mt_x;
  EtchType *_mt_y;
  EtchField *_mf_a = new EtchField("a");
  EtchField *_mf_b = new EtchField("b");
  EtchTypeMap *types = new EtchTypeMap();
  EtchClass2TypeMap *class2type = new EtchClass2TypeMap();

  //example types
  EtchString str("x");
  types->get(str, _mt_x);
  str.set("y");
  types->get(str, _mt_y);
  EtchDefaultValueFactory::Init(mRuntime, types, class2type);
  types->lock();
  class2type->lock();

  EtchDefaultValueFactory *test = NULL;
  test = new EtchDefaultValueFactory("tcp:", types, class2type);
  EXPECT_TRUE(test != NULL);

  str.set("Error Message");
  EtchType *type;
  capu::SmartPointer<EtchObject> deserialized;

  //create serialized struct value
  EtchString strRuntime = "_Etch_RuntimeException";
  test->getType(strRuntime, type);
  EtchStructValue *sv = new EtchStructValue(type, test);
  sv->put(EtchDefaultValueFactory::_mf_msg(), new EtchString("Error Message"));

  //construct the object
  test->importCustomValue(sv, deserialized);
  //check object values
  EXPECT_TRUE(deserialized.get() != NULL);
  EXPECT_TRUE(((EtchException*) deserialized.get())->getErrorMessage().equals(&str));

  EtchString tmp = "x";
  test->getType(tmp, type);
  EtchStructValue* sv_unknown = new EtchStructValue(type, test);
  //TRY IMPORT UNKNOWN VALUE
  capu::SmartPointer<EtchObject> unknown;
  EXPECT_TRUE(test->importCustomValue(sv_unknown, unknown) == ETCH_EINVAL);

  //clear the added types
  types->clear();
  delete sv;
  delete sv_unknown;
  delete test;
  delete _mf_a;
  delete _mf_b;
  delete types;
  delete class2type;
}

TEST_F(EtchDefaultValueFactoryTest, lockTest) {
  EtchType *_mt_x;
  EtchType *_mt_y;
  EtchField *_mf_a = new EtchField("a");
  EtchField *_mf_b = new EtchField("b");
  EtchTypeMap *types = new EtchTypeMap();
  EtchClass2TypeMap *class2type = new EtchClass2TypeMap();

  //example types
  EtchString str("x");
  types->get(str, _mt_x);
  str.set("y");
  types->get(str, _mt_y);
  EtchDefaultValueFactory::Init(mRuntime, types, class2type);
  types->lock();
  class2type->lock();

  //create value factory
  EtchDefaultValueFactory *test = NULL;
  test = new EtchDefaultValueFactory("tcp:", types, class2type);
  EXPECT_TRUE(test != NULL);

  EtchType *t;
  EtchString x = "x";
  EtchString y = "y";
  EtchString a = "a";
  EtchString b = "b";

  EXPECT_TRUE(test->getType(x, t) == ETCH_OK);
  EXPECT_TRUE(test->getType(y, t) == ETCH_OK);
  EXPECT_TRUE(test->getType(a, t) == ETCH_OK);
  test->lockDynamicTypes();
  EXPECT_TRUE(test->getType(b, t) == ETCH_ERROR);

  //clear the added types
  types->clear();
  delete test;
  delete _mf_a;
  delete _mf_b;
  delete types;
  delete class2type;
}

TEST_F(EtchDefaultValueFactoryTest, messageIdTest) {
  EtchType *_mt_x;
  EtchType *_mt_y;
  EtchField *_mf_a = new EtchField("a");
  EtchField *_mf_b = new EtchField("b");
  EtchTypeMap *types = new EtchTypeMap();
  EtchClass2TypeMap *class2type = new EtchClass2TypeMap();

  //example types
  EtchString str("x");
  types->get(str, _mt_x);
  str.set("y");
  types->get(str, _mt_y);
  EtchDefaultValueFactory::Init(mRuntime, types, class2type);
  types->lock();
  class2type->lock();

  EtchDefaultValueFactory *test = NULL;
  test = new EtchDefaultValueFactory("tcp:", types, class2type);
  EXPECT_TRUE(test != NULL);

  str.set("foo");
  EtchType *mt_foo = new EtchType(str);

  capu::SmartPointer<EtchValidator> val;
  EtchValidatorLong::Get(mRuntime, 0, val);
  mt_foo->putValidator(EtchDefaultValueFactory::_mf__messageId(), val);

  EtchMessage *msg = new EtchMessage(mt_foo, test);
  capu::int64_t id;
  EXPECT_TRUE(test->getMessageId(msg, id) == ETCH_ERROR);
  EtchMessage *msg2 = NULL;
  EXPECT_TRUE(test->getMessageId(msg2, id) == ETCH_EINVAL);

  test->setMessageId(msg, 234L);

  test->getMessageId(msg, id);
  EXPECT_TRUE(234L == id);

  //clear the added types
  types->clear();
  delete test;
  delete msg;
  delete _mf_a;
  delete _mf_b;
  delete types;
  delete class2type;
  delete mt_foo;
}

TEST_F(EtchDefaultValueFactoryTest, inReplyToTest) {
  EtchType *_mt_x;
  EtchType *_mt_y;
  EtchField *_mf_a = new EtchField("a");
  EtchField *_mf_b = new EtchField("b");
  EtchTypeMap *types = new EtchTypeMap();
  EtchClass2TypeMap *class2type = new EtchClass2TypeMap();

  //example types
  EtchString str("x");
  types->get(str, _mt_x);
  str.set("y");
  types->get(str, _mt_y);
  EtchDefaultValueFactory::Init(mRuntime, types, class2type);
  types->lock();
  class2type->lock();

  EtchDefaultValueFactory *test = NULL;
  test = new EtchDefaultValueFactory("tcp:", types, class2type);
  EXPECT_TRUE(test != NULL);

  str.set("foo");
  EtchType *mt_foo = new EtchType(str);

  capu::SmartPointer<EtchValidator> val;
  EtchValidatorLong::Get(mRuntime, 0, val);
  mt_foo->putValidator(EtchDefaultValueFactory::_mf__inReplyTo(), val);

  EtchMessage *msg = new EtchMessage(mt_foo, test);
  capu::int64_t id;
  EXPECT_TRUE(test->getInReplyToMessageId(msg, id) == ETCH_ERROR);
  EtchMessage *msg2 = NULL;
  EXPECT_TRUE(test->getInReplyToMessageId(msg2, id) == ETCH_EINVAL);

  test->setInReplyToMessageId(msg, 234L);

  test->getInReplyToMessageId(msg, id);
  EXPECT_TRUE(234L == id);

  //clear the added types
  types->clear();
  delete test;
  delete msg;
  delete _mf_a;
  delete _mf_b;
  delete types;
  delete class2type;
  delete mt_foo;
}
