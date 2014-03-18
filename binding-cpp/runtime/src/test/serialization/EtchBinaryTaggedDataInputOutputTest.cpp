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

#include "gtest/gtest.h"
#include "gmock/gmock.h"
#include "serialization/EtchTypeCodes.h"
#include "serialization/EtchBinaryTaggedDataInput.h"
#include "serialization/EtchDefaultValueFactory.h"
#include "serialization/EtchTypeMap.h"
#include "serialization/EtchClass2TypeMap.h"
#include "serialization/EtchBinaryTaggedDataInput.h"
#include "serialization/EtchBinaryTaggedDataOutput.h"
#include "serialization/EtchValidatorBoolean.h"
#include "serialization/EtchValidatorShort.h"
#include "serialization/EtchValidatorByte.h"
#include "serialization/EtchValidatorFloat.h"
#include "serialization/EtchValidatorDouble.h"
#include "support/EtchRuntime.h"

class MockValueFactory9 : public virtual EtchValueFactory {
public:

  ~MockValueFactory9() {

  }

  MOCK_METHOD2(getType, status_t(capu::uint32_t id, EtchType*& result));

  MOCK_METHOD2(getType, status_t(EtchString& name, EtchType*& result));

  MOCK_METHOD1(addType, status_t(EtchType* type));

  MOCK_METHOD0(lockDynamicTypes, status_t());

  MOCK_METHOD0(unlockDynamicTypes, status_t());

  MOCK_METHOD2(getMessageId, status_t(EtchMessage* msg, capu::int64_t &result));

  MOCK_METHOD2(setMessageId, status_t(EtchMessage* msg, capu::int64_t msgid));

  MOCK_METHOD0(get_mf__messageId, EtchField());

  MOCK_METHOD2(getInReplyToMessageId, status_t(EtchMessage* msg, capu::int64_t &result));

  MOCK_METHOD2(setInReplyToMessageId, status_t(EtchMessage* msg, capu::int64_t msgid));

  MOCK_METHOD0(get_mf__inReplyTo, EtchField());

  MOCK_METHOD2(importCustomValue, status_t(EtchStructValue* _struct, capu::SmartPointer<EtchObject>& result));

  MOCK_METHOD2(exportCustomValue, status_t(capu::SmartPointer<EtchObject> value, EtchStructValue*& result));

  MOCK_METHOD0(get_mt__exception, EtchType*());

  MOCK_METHOD2(getCustomStructType, status_t(const EtchObjectType* c, EtchType *& type));

  MOCK_METHOD4(getNativeArrayForComponentType, status_t(const EtchObjectType *objectType,  capu::SmartPointer<EtchNativeArrayBase> &nativeArray, capu::int32_t length, capu::int32_t dim));

  EtchLevel getLevel() {
    return LEVEL_FULL;
  }

  MOCK_METHOD1(setLevel, EtchLevel(EtchLevel level));

};

class MyValueFactory {
public:
  EtchTypeMap types;
  EtchClass2TypeMap class2type;
  EtchDefaultValueFactory * factory;
  /**
   * Constructs the MyValueFactory.
   * @param uri
   */
public:

  MyValueFactory(EtchRuntime* runtime, EtchString uri) {
    EtchDefaultValueFactory::Init(runtime, &types, &class2type);
    factory = new EtchDefaultValueFactory(uri, &types, &class2type);
  }

  ~MyValueFactory() {
    delete factory;
    types.clear();
  }
};

class Utility {
public:

  static capu::int32_t* testInt() {
    capu::int32_t n = 65536 + 2 + 2;
    capu::int32_t k = 65536 + 2;
    capu::int32_t min = capu::NumericLimits::Min<capu::int16_t > () - 1;
    capu::int32_t *vals = new capu::int32_t[n];
    capu::int32_t i = 0;
    while (k > 0) {
      vals[i++] = min++;
      k--;
    }
    vals[i++] = capu::NumericLimits::Min<capu::int32_t > ();
    vals[i++] = capu::NumericLimits::Max<capu::int32_t > ();
    return vals;
  }

  static capu::int64_t* testLong() {
    capu::int32_t n = 65536 + 2 + 6;
    capu::int32_t k = 65536 + 2;
    capu::int32_t min = capu::NumericLimits::Min<capu::int16_t > () - 1;
    capu::int64_t* vals = new capu::int64_t[n];
    capu::int32_t i = 0;
    while (k > 0) {
      vals[i++] = min++;
      k--;
    }
    vals[i++] = capu::NumericLimits::Min<capu::int32_t > ();
    vals[i++] = capu::NumericLimits::Max<capu::int32_t > ();
    vals[i++] = (capu::int64_t) capu::NumericLimits::Min<capu::int32_t > () - (capu::int64_t)1L;
    vals[i++] = (capu::int64_t) capu::NumericLimits::Max<capu::int32_t > () + (capu::int64_t)1L;
    return vals;
  }

  static void test(EtchRuntime* runtime, capu::SmartPointer<EtchObject> x, capu::SmartPointer<EtchValidator> v, capu::bool_t stringTypeAndField) {
    MyValueFactory vf(runtime, "tcp:");
    EtchType *mt_foo = NULL;
    EtchField mf_x("x");
    EtchString foo("foo");
    vf.factory->getType(foo, mt_foo);
    mt_foo->putValidator(mf_x, v);

    capu::SmartPointer<EtchMessage> msg = new EtchMessage(mt_foo, vf.factory);
    msg->put(mf_x, x);

    capu::uint32_t length;
    capu::int8_t * buffer;
    //SERIALIZE DATA INTO FLEX BUFFER
    msg2bytes(runtime, msg, stringTypeAndField, length, buffer, vf);
    capu::SmartPointer<EtchMessage> msg2;
    //DESERIALIZE DATA FROM FLEX BYTE ARRAY
    bytes2msg(runtime, buffer, length, msg2, vf);
    //TYPE CHECK
    EXPECT_TRUE(msg2->isType(mt_foo));
    EXPECT_TRUE(msg2->count() == 1);
    capu::SmartPointer<EtchObject> result;
    EXPECT_TRUE(msg2->get(mf_x, &result) == ETCH_OK);
    //check result of serialization
    check_equal(result.get(), x.get());
  }

  static void check_equal(EtchObject* first, EtchObject* second) {
    if (first->getObjectType()->isArray()) {
      //FOR ARRAYS
      EtchNativeArray<capu::SmartPointer<EtchObject> > * array = (EtchNativeArray<capu::SmartPointer<EtchObject> > *) first;
      EtchNativeArray<capu::SmartPointer<EtchObject> > * array2 = (EtchNativeArray<capu::SmartPointer<EtchObject> > *) second;
      for (capu::int32_t i = 0; i < array->getLength(); i++) {
        capu::SmartPointer<EtchObject> obj;
        capu::SmartPointer<EtchObject> obj1;

        if (array->getDim() == 1) {
          array->get(i, &obj);
          array2->get(i, &obj1);
          check_equal(obj.get(), obj1.get());
        } else if (array->getDim() > 1) {
          capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchObject> > > subarray;
          capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchObject> > > subarray2;
          array->get(i,subarray);
          array2->get(i, subarray2);
          check_equal(subarray.get(), subarray2.get());
        } else {
          //CHECK EACH OBJECT IN THE ARRAY CORRECTLY SERIALIZED OR NOT
          EXPECT_TRUE(obj1->equals(obj.get()));
        }
      }
    } else {
      //FOR PRIMITIVE TYPES
      EXPECT_TRUE(second->equals(first));
    }
  }

  static void msg2bytes(EtchRuntime* runtime, capu::SmartPointer<EtchMessage> msg, capu::bool_t stringTypeAndField, capu::uint32_t &size, capu::int8_t *&buffer, MyValueFactory &vf) {
    capu::SmartPointer<EtchFlexBuffer> buf = new EtchFlexBuffer();

    EtchURL u("none:");
    //    u.addTerm(EtchBinaryTaggedDataOutput::STRING_TYPE_AND_FIELD(), stringTypeAndField);

    EtchBinaryTaggedDataOutput btdo(runtime, vf.factory, &u);
    EXPECT_TRUE(btdo.writeMessage(msg, buf) == ETCH_OK);
    buf->setIndex(0);
    size = buf->getAvailableBytes();
    buffer = new capu::int8_t[size];
    capu::uint32_t len;
    EXPECT_TRUE(buf->get(buffer, 0, size, len) == ETCH_OK);
    return;
  }

  static void bytes2msg(EtchRuntime* runtime, capu::int8_t* buf, capu::uint32_t length, capu::SmartPointer<EtchMessage> &msg, MyValueFactory &vf) {
    bytes2msg(runtime, buf, length, msg, LEVEL_FULL, vf);
  }

  static void bytes2msg(EtchRuntime* runtime, capu::int8_t* buf, capu::uint32_t length, capu::SmartPointer<EtchMessage> &msg, EtchLevel level, MyValueFactory &vf) {
    EtchLevel oldLevel = vf.factory->setLevel(level);
    EtchBinaryTaggedDataInput btdi(runtime, vf.factory);
    capu::SmartPointer<EtchFlexBuffer> buffer = new EtchFlexBuffer(buf, length);
    buffer->setIndex(0);
    EXPECT_TRUE(btdi.readMessage(buffer, msg) == ETCH_OK);
    vf.factory->setLevel(oldLevel);
  }

  static void test_bto_write(EtchRuntime* runtime, capu::SmartPointer<EtchObject> x, capu::int8_t *compare_buffer, capu::SmartPointer<EtchValidator> v) {
    EtchString type("a");
    EtchString field("b");
    MyValueFactory vf(runtime, "tcp:");

    EtchType t(1, type);
    EtchField f(2, field);
    t.putValidator(f, v);

    capu::SmartPointer<EtchMessage> msg = new EtchMessage(&t, vf.factory);
    msg->put(f, x);
    EtchURL u("none:");
    EtchBinaryTaggedDataOutput btdo(runtime, vf.factory, &u);
    capu::SmartPointer<EtchFlexBuffer> buf = new EtchFlexBuffer();
    buf->setByteRepresentation(ETCH_BIG_ENDIAN);
    EXPECT_TRUE(btdo.writeMessage(msg, buf) == ETCH_OK);
    buf->setIndex(0);
    capu::int32_t size = buf->getAvailableBytes();
    capu::int8_t* buffer = new capu::int8_t[size];
    capu::uint32_t len;
    EXPECT_TRUE(buf->get(buffer, 0, size, len) == ETCH_OK);

    EXPECT_TRUE(memcmp(buffer, compare_buffer, len) == 0);
    delete [] buffer;
  }

};

class EtchBinaryTaggedDataInputOutputTest
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

TEST_F(EtchBinaryTaggedDataInputOutputTest, createTest) {
  MockValueFactory9 *factory = new MockValueFactory9();
  EtchBinaryTaggedDataInput* dataIn = new EtchBinaryTaggedDataInput(mRuntime, factory);
  delete dataIn;
  delete factory;
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, checkTest) {
  MockValueFactory9 *factory = new MockValueFactory9();
  EtchBinaryTaggedDataInput* dataIn = new EtchBinaryTaggedDataInput(mRuntime, factory);

  // i'm testing with hard coded constants on the left hand
  // side here because this represents our external interface
  // for messages. if these values change, it could bring about
  // accidental missile launches, inadvertant bank balance transfers
  // to unfriendly foreign nations, or the dropping of a small
  // child's lollipop into the sand. like a pizza face down in
  // the parking lot, you don't wanna go there.

  // byte values
  for (capu::int8_t i = capu::NumericLimits::Min<capu::int8_t > (); i < capu::NumericLimits::Max<capu::int8_t > (); i++) {
    capu::SmartPointer<EtchByte> _byte = new EtchByte(i);
    if (i >= EtchTypeCode::MIN_TINY_INT && i <= EtchTypeCode::MAX_TINY_INT)
      EXPECT_TRUE(i == dataIn->checkValue(_byte));
    else {
      capu::int8_t var = EtchTypeCode::BYTE;
      EXPECT_TRUE(var == dataIn->checkValue(_byte));
    }
  }
  // short values
  for (capu::int16_t i = capu::NumericLimits::Min<capu::int16_t > (); i < capu::NumericLimits::Max<capu::int16_t > (); i++) {
    capu::SmartPointer<EtchShort> _short = new EtchShort(i);
    if (i >= EtchTypeCode::MIN_TINY_INT && i <= EtchTypeCode::MAX_TINY_INT)
      EXPECT_TRUE((capu::int8_t) i == dataIn->checkValue(_short));
    else if (i >= capu::NumericLimits::Min<capu::int8_t > () && i <= capu::NumericLimits::Max<capu::int8_t > ()) {
      capu::int8_t var = EtchTypeCode::BYTE;
      EXPECT_TRUE(var == dataIn->checkValue(_short));
    } else {
      capu::int8_t var = EtchTypeCode::SHORT;
      EXPECT_TRUE(var == dataIn->checkValue(_short));
    }
  }

  // int values
  capu::int32_t *array = Utility::testInt();
  for (capu::int32_t i = 0; i < 65540; i++) {
    capu::SmartPointer<EtchInt32> _int = new EtchInt32(array[i]);
    if (array[i] >= EtchTypeCode::MIN_TINY_INT && array[i] <= EtchTypeCode::MAX_TINY_INT)
      EXPECT_TRUE((capu::int8_t) array[i] == dataIn->checkValue(_int));
    else if ((array[i] >= capu::NumericLimits::Min<capu::int8_t > ()) && (array[i] <= capu::NumericLimits::Max<capu::int8_t > ())) {
      capu::int8_t var = EtchTypeCode::BYTE;
      EXPECT_TRUE(var == dataIn->checkValue(_int));
    } else if ((array[i] >= capu::NumericLimits::Min<capu::int16_t > ()) && (array[i] <= capu::NumericLimits::Max<capu::int16_t > ())) {
      capu::int8_t var = EtchTypeCode::SHORT;
      EXPECT_TRUE(var == dataIn->checkValue(_int));
    } else {
      capu::int8_t var = EtchTypeCode::INT;
      EXPECT_TRUE(var == dataIn->checkValue(_int));
    }
  }
  delete [] array;

  // long values
  capu::int64_t *array2 = Utility::testLong();
  for (capu::int64_t i = 0; i < 65542; i++) {
    capu::SmartPointer<EtchLong> _long = new EtchLong(array2[i]);
    if (array2[i] >= EtchTypeCode::MIN_TINY_INT && array2[i] <= EtchTypeCode::MAX_TINY_INT)
      EXPECT_TRUE((capu::int8_t) array2[i] == dataIn->checkValue(_long));
    else if (array2[i] >= capu::NumericLimits::Min<capu::int8_t > () && array2[i] <= capu::NumericLimits::Max<capu::int8_t > ()) {
      capu::int8_t var = EtchTypeCode::BYTE;
      EXPECT_TRUE(var == dataIn->checkValue(_long));
    } else if (array2[i] >= capu::NumericLimits::Min<capu::int16_t > () && array2[i] <= capu::NumericLimits::Max<capu::int16_t > ()) {
      capu::int8_t var = EtchTypeCode::SHORT;
      EXPECT_TRUE(var == dataIn->checkValue(_long));
    } else if (array2[i] >= capu::NumericLimits::Min<capu::int32_t > () && array2[i] <= capu::NumericLimits::Max<capu::int32_t > ()) {
      capu::int8_t var = EtchTypeCode::INT;
      EXPECT_TRUE(var == dataIn->checkValue(_long));
    } else {
      capu::int8_t var = EtchTypeCode::LONG;
      EXPECT_TRUE(var == dataIn->checkValue(_long));
    }
  }
  delete [] array2;

  // null value
  capu::int8_t var = EtchTypeCode::Null;
  EXPECT_TRUE(var == dataIn->checkValue(NULL));

  // boolean values
  capu::SmartPointer<EtchBool> _true = new EtchBool(true);
  capu::SmartPointer<EtchBool> _false = new EtchBool(false);
  var = EtchTypeCode::BOOLEAN_FALSE;
  EXPECT_TRUE(var == dataIn->checkValue(_false));

  var = EtchTypeCode::BOOLEAN_TRUE;
  EXPECT_TRUE(var == dataIn->checkValue(_true));

  capu::SmartPointer<EtchFloat> _float = new EtchFloat(3.14159f);
  // float
  var = EtchTypeCode::FLOAT;
  EXPECT_TRUE(var == dataIn->checkValue(_float));

  capu::SmartPointer<EtchDouble> _double = new EtchDouble(3.14159);
  // double
  var = EtchTypeCode::DOUBLE;
  EXPECT_TRUE(var == dataIn->checkValue(_double));

  capu::SmartPointer<EtchNativeArray<capu::int8_t> > narray = new EtchNativeArray<capu::int8_t > (2);
  // byte array
  var = EtchTypeCode::BYTES;
  EXPECT_TRUE(var == dataIn->checkValue(narray));

  capu::SmartPointer<EtchString> empty = new EtchString("");
  capu::SmartPointer<EtchString> randstr = new EtchString("abc");
  // string
  var = EtchTypeCode::EMPTY_STRING;
  EXPECT_TRUE(var == dataIn->checkValue(empty));

  var = EtchTypeCode::STRING;
  EXPECT_TRUE(var == dataIn->checkValue(randstr));

  EtchString tmp = "type";
  EtchType type(tmp);
  capu::SmartPointer<EtchStructValue> sv = new EtchStructValue(&type, factory);

  // struct
  var = EtchTypeCode::CUSTOM;
  EXPECT_TRUE(var == dataIn->checkValue(sv));

  // none
  var = EtchTypeCode::NONE;
  EXPECT_TRUE(var == dataIn->checkValue(EtchBinaryTaggedData::NONE()));

  delete dataIn;
  delete factory;
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, boolean_serialization) {
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchBool> > > carray1 = new EtchNativeArray<capu::SmartPointer<EtchBool> > (2, 2);
  carray1->createArray(0, 2);
  carray1->createArray(1, 2);
  capu::SmartPointer<EtchBool> content1 = new EtchBool(false);
  capu::SmartPointer<EtchBool> content2 = new EtchBool(true);
  capu::SmartPointer<EtchBool> content3 = new EtchBool(false);
  capu::SmartPointer<EtchBool> content4 = new EtchBool(true);
  carray1->set(Pos(0, 0), content1);
  carray1->set(Pos(0, 1), content2);
  carray1->set(Pos(1, 0), content3);
  carray1->set(Pos(1, 1), content4);

  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorBoolean::Get(mRuntime, 2, val);
  Utility::test(mRuntime, carray1, val, false);
  EtchValidatorBoolean::Get(mRuntime, 0, val);
  Utility::test(mRuntime, content1, val, false);
  Utility::test(mRuntime, content2, val, false);
  Utility::test(mRuntime, content3, val, false);
  Utility::test(mRuntime, content4, val, false);
}
//

TEST_F(EtchBinaryTaggedDataInputOutputTest, int_serialization) {
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchInt32> > > carray1 = new EtchNativeArray<capu::SmartPointer<EtchInt32> > (2, 2);
  carray1->createArray(0, 2);
  carray1->createArray(1, 2);
  capu::SmartPointer<EtchInt32> content1 = new EtchInt32(12);
  capu::SmartPointer<EtchInt32> content2 = new EtchInt32(23);
  capu::SmartPointer<EtchInt32> content3 = new EtchInt32(43);
  capu::SmartPointer<EtchInt32> content4 = new EtchInt32(54);
  carray1->set(Pos(0, 0), content1);
  carray1->set(Pos(0, 1), content2);
  carray1->set(Pos(1, 0), content3);
  carray1->set(Pos(1, 1), content4);

  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorInt::Get(mRuntime, 2, val);
  Utility::test(mRuntime, carray1, val, false);
  EtchValidatorInt::Get(mRuntime, 0, val);
  Utility::test(mRuntime, content1, val, false);
  Utility::test(mRuntime, content2, val, false);
  Utility::test(mRuntime, content3, val, false);
  Utility::test(mRuntime, content4, val, false);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, string_serialization) {
  const capu::uint8_t utf8_1 [] = {0xF0, 0xA4, 0xAD, 0xA2, 0xE2, 0x82, 0xAC, 0xC2, 0xA2, 0x24, 0x0};
  const capu::uint8_t utf8_2 [] = {0xF0, 0xA4, 0xAD, 0xA2, 0xE2, 0x82, 0xAC, 0xC2, 0xA2, 0x25, 0x0};
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchString> > > carray1 = new EtchNativeArray<capu::SmartPointer<EtchString> > (2, 2);
  carray1->createArray(0, 2);
  carray1->createArray(1, 2);
  capu::SmartPointer<EtchString> content1 = new EtchString((capu::int8_t*)utf8_1, 11, "utf-8");
  capu::SmartPointer<EtchString> content2 = new EtchString((capu::int8_t*)utf8_2, 11, "utf-8");
  capu::SmartPointer<EtchString> content3 = new EtchString((capu::int8_t*)utf8_1, 11, "utf-8");
  capu::SmartPointer<EtchString> content4 = new EtchString((capu::int8_t*)utf8_2, 11, "utf-8");
  carray1->set(Pos(0, 0), content1);
  carray1->set(Pos(0, 1), content2);
  carray1->set(Pos(1, 0), content3);
  carray1->set(Pos(1, 1), content4);

  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorString::Get(mRuntime, 2, val);
  Utility::test(mRuntime, carray1, val, false);
  EtchValidatorString::Get(mRuntime, 0, val);
  Utility::test(mRuntime, content1, val, false);
  Utility::test(mRuntime, content2, val, false);
  Utility::test(mRuntime, content3, val, false);
  Utility::test(mRuntime, content4, val, false);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, long_serialization) {
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchLong> > > carray1 = new EtchNativeArray<capu::SmartPointer<EtchLong> > (2, 2);
  carray1->createArray(0, 2);
  carray1->createArray(1, 2);
  capu::SmartPointer<EtchLong> content1 = new EtchLong(90);
  capu::SmartPointer<EtchLong> content2 = new EtchLong(1123123213);
  capu::SmartPointer<EtchLong> content3 = new EtchLong(90);
  capu::SmartPointer<EtchLong> content4 = new EtchLong(1123123213);
  carray1->set(Pos(0, 0), content1);
  carray1->set(Pos(0, 1), content2);
  carray1->set(Pos(1, 0), content3);
  carray1->set(Pos(1, 1), content4);

  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorLong::Get(mRuntime, 2, val);
  Utility::test(mRuntime, carray1, val, false);
  EtchValidatorLong::Get(mRuntime, 0, val);
  Utility::test(mRuntime, content1, val, false);
  Utility::test(mRuntime, content2, val, false);
  Utility::test(mRuntime, content3, val, false);
  Utility::test(mRuntime, content4, val, false);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, byte_serialization) {
  //  EtchNativeArray<EtchByte*> carray(2, &EtchByte::TYPE);
  capu::SmartPointer<EtchByte> content1 = new EtchByte(90);
  //array can not be tested because it is optimized as native
  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorByte::Get(mRuntime, 0, val);
  Utility::test(mRuntime, content1, val, false);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, short_serialization) {
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchShort> > > carray1 = new EtchNativeArray<capu::SmartPointer<EtchShort> > (2, 2);
  carray1->createArray(0, 2);
  carray1->createArray(1, 2);
  capu::SmartPointer<EtchShort> content1 = new EtchShort(90);
  capu::SmartPointer<EtchShort> content2 = new EtchShort(1113);
  capu::SmartPointer<EtchShort> content3 = new EtchShort(90);
  capu::SmartPointer<EtchShort> content4 = new EtchShort(32423);
  carray1->set(Pos(0, 0), content1);
  carray1->set(Pos(0, 1), content2);
  carray1->set(Pos(1, 0), content3);
  carray1->set(Pos(1, 1), content4);

  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorShort::Get(mRuntime, 2, val);
  Utility::test(mRuntime, carray1, val, false);
  EtchValidatorShort::Get(mRuntime, 0, val);
  Utility::test(mRuntime, content1, val, false);
  Utility::test(mRuntime, content2, val, false);
  Utility::test(mRuntime, content3, val, false);
  Utility::test(mRuntime, content4, val, false);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, float_serialization) {
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchFloat> > > carray1 = new EtchNativeArray<capu::SmartPointer<EtchFloat> > (2, 2);
  carray1->createArray(0, 2);
  carray1->createArray(1, 2);
  capu::SmartPointer<EtchFloat> content1 = new EtchFloat(90.2f);
  capu::SmartPointer<EtchFloat> content2 = new EtchFloat(1113.2f);
  capu::SmartPointer<EtchFloat> content3 = new EtchFloat(90.2f);
  capu::SmartPointer<EtchFloat> content4 = new EtchFloat(32423.2f);
  carray1->set(Pos(0, 0), content1);
  carray1->set(Pos(0, 1), content2);
  carray1->set(Pos(1, 0), content3);
  carray1->set(Pos(1, 1), content4);

  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorFloat::Get(mRuntime, 2, val);
  Utility::test(mRuntime, carray1, val, false);
  EtchValidatorFloat::Get(mRuntime, 0, val);
  Utility::test(mRuntime, content1, val, false);
  Utility::test(mRuntime, content2, val, false);
  Utility::test(mRuntime, content3, val, false);
  Utility::test(mRuntime, content4, val, false);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, double_serialization) {
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchDouble> > > carray1 = new EtchNativeArray<capu::SmartPointer<EtchDouble> > (2, 2);
  carray1->createArray(0, 2);
  carray1->createArray(1, 2);
  capu::SmartPointer<EtchDouble> content1 = new EtchDouble(90.2);
  capu::SmartPointer<EtchDouble> content2 = new EtchDouble(1113.2);
  capu::SmartPointer<EtchDouble> content3 = new EtchDouble(90.2);
  capu::SmartPointer<EtchDouble> content4 = new EtchDouble(32423.2);
  carray1->set(Pos(0, 0), content1);
  carray1->set(Pos(0, 1), content2);
  carray1->set(Pos(1, 0), content3);
  carray1->set(Pos(1, 1), content4);

  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorDouble::Get(mRuntime, 2, val);
  Utility::test(mRuntime, carray1, val, false);
  EtchValidatorDouble::Get(mRuntime, 0, val);
  Utility::test(mRuntime, content1, val, false);
  Utility::test(mRuntime, content2, val, false);
  Utility::test(mRuntime, content3, val, false);
  Utility::test(mRuntime, content4, val, false);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, empty_string_serialization) {
  capu::SmartPointer<EtchString> str = new EtchString(NULL, 0, "utf-8");
  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorString::Get(mRuntime, 0, val);
  Utility::test(mRuntime, str, val, false);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, date_serialization) {
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchDate> > > carray1 = new EtchNativeArray<capu::SmartPointer<EtchDate> > (2, 2);
  carray1->createArray(0, 2);
  carray1->createArray(1, 2);
  capu::SmartPointer<EtchDate> content1 = new EtchDate();
  capu::SmartPointer<EtchDate> content2 = new EtchDate();
  capu::SmartPointer<EtchDate> content3 = new EtchDate();
  capu::SmartPointer<EtchDate> content4 = new EtchDate();
  content1->set(90);
  content2->set(1113);
  content3->set(90);
  content4->set(32423);
  carray1->set(Pos(0, 0), content1);
  carray1->set(Pos(0, 1), content2);
  carray1->set(Pos(1, 0), content3);
  carray1->set(Pos(1, 1), content4);

  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorObject::Get(mRuntime, 2, val);
  Utility::test(mRuntime, carray1, val, false);
  EtchValidatorObject::Get(mRuntime, 0, val);
  Utility::test(mRuntime, content1, val, false);
  Utility::test(mRuntime, content2, val, false);
  Utility::test(mRuntime, content3, val, false);
  Utility::test(mRuntime, content4, val, false);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, btdo_object_serialization) {
  capu::int8_t byte_array[] = {3, 1, 1, 2, -111, -106, 1, 2, 90, -126, -127, -127};
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchObject> > > carray1 = new EtchNativeArray<capu::SmartPointer<EtchObject> > (2);

  capu::SmartPointer<EtchObject> content1 = new EtchLong(90);
  capu::SmartPointer<EtchObject> content2 = new EtchBool(false);
  carray1->set(0, content1);
  carray1->set(1, content2);

  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorObject::Get(mRuntime, 1, val);
  Utility::test_bto_write(mRuntime, carray1, byte_array, val);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, btdo_boolean_write) {

  capu::int8_t byte_array[] = {3, 1, 1, 2, -111, -125, 1, 2, -125, -126, -127, -127};
  capu::int8_t byte_false[] = {3, 1, 1, 2, -126, -127};
  capu::int8_t byte_true[] = {3, 1, 1, 2, -125, -127};
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchBool> > > carray = new EtchNativeArray<capu::SmartPointer<EtchBool> > (2);
  capu::SmartPointer<EtchBool> content1 = new EtchBool(true);
  capu::SmartPointer<EtchBool> content2 = new EtchBool(false);
  carray->set(0, content1);
  carray->set(1, content2);
  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorBoolean::Get(mRuntime, 0, val);
  Utility::test_bto_write(mRuntime, content2, byte_false, val);
  Utility::test_bto_write(mRuntime, content1, byte_true, val);
  EtchValidatorBoolean::Get(mRuntime, 1, val);
  Utility::test_bto_write(mRuntime, carray, byte_array, val);

}

TEST_F(EtchBinaryTaggedDataInputOutputTest, btdo_byte_write) {

  capu::int8_t byte_array[] = {3, 1, 1, 2, -117, 3, 1, 2, 3, -127};
  capu::int8_t byte_pos[] = {3, 1, 1, 2, 1, -127};
  capu::int8_t byte_neg[] = {3, 1, 1, 2, -1, -127};
  capu::int8_t byte_zero[] = {3, 1, 1, 2, 0, -127};
  capu::int8_t bytes[] = {3, 1, 1, 2, -124, -100, -127};
  capu::SmartPointer<EtchNativeArray<capu::int8_t> > carray = new EtchNativeArray<capu::int8_t > (3);
  capu::SmartPointer<EtchByte> content1 = new EtchByte(1);
  capu::SmartPointer<EtchByte> content2 = new EtchByte(-1);
  capu::SmartPointer<EtchByte> content0 = new EtchByte(0);
  capu::SmartPointer<EtchByte> content7 = new EtchByte(-100);

  capu::int8_t content3 = 1;
  capu::int8_t content4 = 2;
  capu::int8_t content5 = 3;
  carray->set(0, content3);
  carray->set(1, content4);
  carray->set(2, content5);
  capu::SmartPointer<EtchValidator> val = NULL;

  EtchValidatorByte::Get(mRuntime, 0, val);
  Utility::test_bto_write(mRuntime, content1, byte_pos, val);
  Utility::test_bto_write(mRuntime, content2, byte_neg, val);
  Utility::test_bto_write(mRuntime, content0, byte_zero, val);
  Utility::test_bto_write(mRuntime, content7, bytes, val);

  EtchValidatorByte::Get(mRuntime, 1, val);
  Utility::test_bto_write(mRuntime, carray, byte_array, val);

}

TEST_F(EtchBinaryTaggedDataInputOutputTest, btdo_short_write) {

  capu::int8_t byte_array[] = {3, 1, 1, 2, -111, -123, 1, 3, 1, 2, 3, -127, -127};
  capu::int8_t byte_pos[] = {3, 1, 1, 2, -123, 39, 16, -127};
  capu::int8_t byte_neg[] = {3, 1, 1, 2, -123, -40, -16, -127};
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchShort> > > carray = new EtchNativeArray<capu::SmartPointer<EtchShort> > (3);
  capu::SmartPointer<EtchShort> content0 = new EtchShort(10000);
  capu::SmartPointer<EtchShort> content7 = new EtchShort(-10000);
  capu::SmartPointer<EtchShort> content1 = new EtchShort(1);
  capu::SmartPointer<EtchShort> content2 = new EtchShort(2);
  capu::SmartPointer<EtchShort> content3 = new EtchShort(3);
  carray->set(0, content1);
  carray->set(1, content2);
  carray->set(2, content3);
  capu::SmartPointer<EtchValidator> val = NULL;

  EtchValidatorShort::Get(mRuntime, 0, val);
  Utility::test_bto_write(mRuntime, content0, byte_pos, val);
  Utility::test_bto_write(mRuntime, content7, byte_neg, val);

  EtchValidatorShort::Get(mRuntime, 1, val);
  Utility::test_bto_write(mRuntime, carray, byte_array, val);

}

TEST_F(EtchBinaryTaggedDataInputOutputTest, btdo_int_write) {

  capu::int8_t byte_array[] = {3, 1, 1, 2, -111, -122, 1, 3, 1, 2, 3, -127, -127};
  capu::int8_t byte_pos[] = {3, 1, 1, 2, -122, 59, -102, -54, 0, -127};
  capu::int8_t byte_neg[] = {3, 1, 1, 2, -122, -60, 101, 54, 0, -127};
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchInt32> > > carray = new EtchNativeArray<capu::SmartPointer<EtchInt32> > (3);
  capu::SmartPointer<EtchInt32> content0 = new EtchInt32(1000000000);
  capu::SmartPointer<EtchInt32> content7 = new EtchInt32(-1000000000);
  capu::SmartPointer<EtchInt32> content1 = new EtchInt32(1);
  capu::SmartPointer<EtchInt32> content2 = new EtchInt32(2);
  capu::SmartPointer<EtchInt32> content3 = new EtchInt32(3);
  carray->set(0, content1);
  carray->set(1, content2);
  carray->set(2, content3);
  capu::SmartPointer<EtchValidator> val = NULL;

  EtchValidatorInt::Get(mRuntime, 0, val);
  Utility::test_bto_write(mRuntime, content0, byte_pos, val);
  Utility::test_bto_write(mRuntime, content7, byte_neg, val);

  EtchValidatorInt::Get(mRuntime, 1, val);
  Utility::test_bto_write(mRuntime, carray, byte_array, val);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, btdo_long_write) {

  capu::int8_t byte_array[] = {3, 1, 1, 2, -111, -121, 1, 3, 1, 2, 3, -127, -127};
  capu::int8_t byte_pos[] = {3, 1, 1, 2, -121, 13, -32, -74, -77, -89, 100, 0, 0, -127};
  capu::int8_t byte_neg[] = {3, 1, 1, 2, -121, -14, 31, 73, 76, 88, -100, 0, 0, -127};
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchLong> > > carray = new EtchNativeArray<capu::SmartPointer<EtchLong> > (3);
  capu::SmartPointer<EtchLong> content0 = new EtchLong(1000000000000000000L);
  capu::SmartPointer<EtchLong> content7 = new EtchLong(-1000000000000000000L);
  capu::SmartPointer<EtchLong> content1 = new EtchLong(1);
  capu::SmartPointer<EtchLong> content2 = new EtchLong(2);
  capu::SmartPointer<EtchLong> content3 = new EtchLong(3);
  carray->set(0, content1);
  carray->set(1, content2);
  carray->set(2, content3);
  capu::SmartPointer<EtchValidator> val = NULL;

  EtchValidatorLong::Get(mRuntime, 0, val);
  Utility::test_bto_write(mRuntime, content0, byte_pos, val);
  Utility::test_bto_write(mRuntime, content7, byte_neg, val);

  EtchValidatorLong::Get(mRuntime, 1, val);
  Utility::test_bto_write(mRuntime, carray, byte_array, val);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, btdo_float_write) {
  capu::int8_t byte_array[] = {3, 1, 1, 2, -111, -120, 1, 3, -120, 63, -128, 0, 0, -120, 64, 0, 0, 0, -120, 64, 64, 0, 0, -127, -127};
  capu::int8_t byte_pos[] = {3, 1, 1, 2, -120, 70, 64, -28, 0, -127};
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchFloat> > > carray = new EtchNativeArray<capu::SmartPointer<EtchFloat> > (3);
  capu::SmartPointer<EtchFloat> content0 = new EtchFloat(12345.0f);
  capu::SmartPointer<EtchFloat> content1 = new EtchFloat(1);
  capu::SmartPointer<EtchFloat> content2 = new EtchFloat(2);
  capu::SmartPointer<EtchFloat> content3 = new EtchFloat(3);
  carray->set(0, content1);
  carray->set(1, content2);
  carray->set(2, content3);
  capu::SmartPointer<EtchValidator> val = NULL;

  EtchValidatorFloat::Get(mRuntime, 0, val);
  Utility::test_bto_write(mRuntime, content0, byte_pos, val);

  EtchValidatorFloat::Get(mRuntime, 1, val);
  Utility::test_bto_write(mRuntime, carray, byte_array, val);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, btdo_double_write) {
  capu::int8_t byte_array[] = {3, 1, 1, 2, -111, -119, 1, 3, -119, 63, -16, 0, 0, 0, 0, 0, 0, -119, 64, 0, 0, 0, 0, 0, 0, 0, -119, 64, 8, 0, 0, 0, 0, 0, 0, -127, -127};
  capu::int8_t byte_pos[] = {3, 1, 1, 2, -119, 64, -42, -24, 0, 0, 0, 0, 0, -127};
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchDouble> > > carray = new EtchNativeArray<capu::SmartPointer<EtchDouble> > (3);
  capu::SmartPointer<EtchDouble> content0 = new EtchDouble(23456);
  capu::SmartPointer<EtchDouble> content1 = new EtchDouble(1);
  capu::SmartPointer<EtchDouble> content2 = new EtchDouble(2);
  capu::SmartPointer<EtchDouble> content3 = new EtchDouble(3);
  carray->set(0, content1);
  carray->set(1, content2);
  carray->set(2, content3);
  capu::SmartPointer<EtchValidator> val = NULL;

  EtchValidatorDouble::Get(mRuntime, 0, val);
  Utility::test_bto_write(mRuntime, content0, byte_pos, val);

  EtchValidatorDouble::Get(mRuntime, 1, val);
  Utility::test_bto_write(mRuntime, carray, byte_array, val);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, btdo_empty_string_write) {
  capu::int8_t byte_pos[] = {3, 1, 1, 2, -110, -127};
  capu::SmartPointer<EtchString> content0 = new EtchString("");
  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorString::Get(mRuntime, 0, val);
  Utility::test_bto_write(mRuntime, content0, byte_pos, val);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, btdo_date_write) {
  capu::int8_t byte_array[] = {3, 1, 1, 2, -111, -107, -122, 43, 57, 107, -52, 1, 2, -107, -122, 43, 57, 107, -52, 1, -122, 102, 0, 26, 64, -122, 73, -106, 2, -46, -127, -107, -122, 43, 57, 107, -52, 1, -122, 102, 0, 26, 64, -121, 0, 0, 0, 0, -117, -48, 56, 53, -127, -127, -127};
  capu::int8_t byte_pos[] = {3, 1, 1, 2, -107, -122, 43, 57, 107, -52, 1, -122, 102, 0, 26, 64, -122, 73, -106, 2, -46, -127, -127};
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchDate> > > carray = new EtchNativeArray<capu::SmartPointer<EtchDate> >(2);
  capu::SmartPointer<EtchDate> date = new EtchDate();
  date->set(1234567890L);
  capu::SmartPointer<EtchDate> content1 = new EtchDate();
  content1->set(1234567890UL);
  capu::SmartPointer<EtchDate> content2 = new EtchDate();
  content2->set(2345678901UL);
  carray->set(0, content1);
  carray->set(1, content2);

  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorObject::Get(mRuntime, 1, val);
  Utility::test_bto_write(mRuntime, carray, byte_array, val);

  EtchValidatorObject::Get(mRuntime, 0, val);
  Utility::test_bto_write(mRuntime, date, byte_pos, val);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, null_write) {
  capu::int8_t byte_pos[] = {3, 1, 0, -127};
  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorObject::Get(mRuntime, 0, val);
  Utility::test_bto_write(mRuntime, NULL, byte_pos, val);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, multi_dimension_test) {
  capu::int8_t byte_pos[] = {3, 1, 1, 2, -111, -125, 2, 2, -111, -125, 1, 2, -126, -125, -127, -111, -125, 1, 2, -126, -125, -127, -127, -127};
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchBool> > > carray1 = new EtchNativeArray<capu::SmartPointer<EtchBool> >(2, 2);
  carray1->createArray(0, 2);
  carray1->createArray(1, 2);
  capu::SmartPointer<EtchBool> content1 = new EtchBool(false);
  capu::SmartPointer<EtchBool> content2 = new EtchBool(true);
  carray1->set(Pos(0, 0), content1);
  carray1->set(Pos(0, 1), content2);
  capu::SmartPointer<EtchBool> content3 = new EtchBool(false);
  capu::SmartPointer<EtchBool> content4 = new EtchBool(true);
  carray1->set(Pos(1, 0), content3);
  carray1->set(Pos(1, 1), content4);
  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorBoolean::Get(mRuntime, 2, val);
  Utility::test_bto_write(mRuntime, carray1, byte_pos, val);
}

TEST_F(EtchBinaryTaggedDataInputOutputTest, btdo_multi_array_write) {
  capu::int8_t byte_array[] = {3, 1, 1, 2, -111, -106, 2, 2, -111, -106, 1, 2, -125, -126, -127, -111, -106, 1, 2, -126, -125, -127, -127, -127};
  capu::SmartPointer<EtchNativeArray<capu::SmartPointer<EtchObject> > > carray1 = new EtchNativeArray<capu::SmartPointer<EtchObject> >(2, 2);
  carray1->createArray(0, 2);
  carray1->createArray(1, 2);
  capu::SmartPointer<EtchBool> content1 = new EtchBool(true);
  capu::SmartPointer<EtchBool> content2 = new EtchBool(false);
  carray1->set(Pos(0, 0), content1);
  carray1->set(Pos(0, 1), content2);
  capu::SmartPointer<EtchBool> content3 = new EtchBool(false);
  capu::SmartPointer<EtchBool> content4 = new EtchBool(true);
  carray1->set(Pos(1, 0), content3);
  carray1->set(Pos(1, 1), content4);
  capu::SmartPointer<EtchValidator> val = NULL;
  EtchValidatorObject::Get(mRuntime, 2, val);
  Utility::test_bto_write(mRuntime, carray1, byte_array ,val);
}
