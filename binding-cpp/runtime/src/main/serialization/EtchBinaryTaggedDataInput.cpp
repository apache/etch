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

#include "serialization/EtchBinaryTaggedDataInput.h"
#include "support/EtchRuntime.h"

EtchBinaryTaggedDataInput::EtchBinaryTaggedDataInput(EtchRuntime* runtime, EtchValueFactory* vf)
: EtchBinaryTaggedData(vf), mRuntime(runtime), mBuffer(NULL), mLengthBudget(0) {
  //get int validator
  EtchValidatorInt::Get(runtime, 0, mIntValidator);

  //get string validator
  capu::SmartPointer<EtchValidator> stringValidator;
  EtchValidatorString::Get(runtime, 0, stringValidator);

  //create combo validator
  mIntOrStrValidator = new EtchComboValidator(mIntValidator, stringValidator);
}

EtchBinaryTaggedDataInput::~EtchBinaryTaggedDataInput() {
}

status_t EtchBinaryTaggedDataInput::readMessage(capu::SmartPointer<EtchFlexBuffer> buf, capu::SmartPointer<EtchMessage> &message) {
  mBuffer = buf;

  status_t ret;

  // lengthBudget is how many array elements total are reasonable to
  // allocate while parsing this message. the largest value comes as each
  // byte of the incoming message turned into an array element. the total
  // will always be lower than this as often it takes multiple bytes to
  // make a single array element. so, each time we make an array, we
  // deduct the specified length from lengthBudget. if the result is
  // negative, then either the incoming message is misformed or someone is
  // trying to spoof us.

  mLengthBudget = buf->getAvailableBytes();

  ret = startMessage(message);
  if (ret != ETCH_OK) {
    mBuffer = NULL;
    mLengthBudget = 0;
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Deserialization of Message header has failed");
    return ret;
  }

  ret = readKeysAndValues(message);
  if (ret != ETCH_OK) {
    mBuffer = NULL;
    mLengthBudget = 0;
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Deserialization of Message body has failed");
    return ret;
  }

  ret = endMessage(message);
  if (ret != ETCH_OK) {
    mBuffer = NULL;
    mLengthBudget = 0;
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(),  "Deserialization of Message end has failed");
    return ret;
  }

  return ETCH_OK;
}

status_t EtchBinaryTaggedDataInput::readStruct(capu::SmartPointer<EtchStructValue> &result) {
  status_t ret;

  ret = startStruct(result);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Reading header of struct has failed");
    return ret;
  }

  ret = readKeysAndValues(result);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Reading body of struct has failed");
    return ret;
  }

  ret = endStruct(result);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Reading end of struct has failed");
    return ret;
  }

  return ETCH_OK;
}

status_t EtchBinaryTaggedDataInput::readArray(capu::SmartPointer<EtchValidator> v, EtchArrayValue *&result) {
  status_t ret;

  ret = startArray(result);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Reading header of array has failed");
    return ret;
  }

  ret = readValues(result, v);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Reading body of array has failed");
    return ret;
  }

  ret = endArray(result);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Reading end of array has failed");
    return ret;
  }
  return ETCH_OK;
}

status_t EtchBinaryTaggedDataInput::readKeysAndValues(capu::SmartPointer<EtchStructValue> sv) {
  status_t ret;
  EtchType* t = sv->getType();
  while (true) {
    EtchField key;
    ret = readField(t, key);
    if (ret != ETCH_OK) {
      break;
    }

    capu::SmartPointer<EtchValidator> v;
    if (t->getValidator(key, v) == ETCH_OK) { // read the value but ignore it.
      capu::SmartPointer<EtchObject> obj;
      ret = readValue(v, obj);
      if (ret == ETCH_OK) {
        sv->put(key, obj);
      }
      else {
        return ret;
      }
    } else {
      capu::SmartPointer<EtchObject> obj;
      capu::SmartPointer<EtchValidator> val;
      EtchValidatorObject::Get(mRuntime, 0, val);
      readValue(val, obj);
    }
  }

  return ETCH_OK;
}

status_t EtchBinaryTaggedDataInput::readValues(EtchArrayValue *av, capu::SmartPointer<EtchValidator> v) {
  status_t ret = ETCH_OK;
  capu::SmartPointer<EtchValidator> validator;
  ret = v->getElementValidator(validator);

  while (true && ret == ETCH_OK) {
    capu::SmartPointer<EtchObject> value;
    ret = readValue(validator, true, value);
    if (value == NONE()) {
      break;
    }
    ret = av->add(value);
  }
  return ret;
}

status_t EtchBinaryTaggedDataInput::startMessage(capu::SmartPointer<EtchMessage> &result) {
  capu::int8_t version = 0;
  mBuffer->getByte(version);

  if (version != VERSION()) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Incorrect serializer version");
    return ETCH_EIO;
  }

  EtchType *type = NULL;
  if (readType(type) != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Type information is not read correctly");
    return ETCH_EIO;
  }

  capu::int32_t length = 0;
  if (readLength(length) != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Length of message is not read correctly");
    return ETCH_EIO;
  }

  result = new EtchMessage(type, mVf, length);
  return ETCH_OK;
}

status_t EtchBinaryTaggedDataInput::endMessage(capu::SmartPointer<EtchMessage> msg) {
  return ETCH_OK;
}

status_t EtchBinaryTaggedDataInput::startStruct(capu::SmartPointer<EtchStructValue> &result) {
  EtchType* t = NULL;
  if (readType(t) != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "type of struct is not read correctly");
    return ETCH_ERROR;
  }

  capu::int32_t length;
  if (readLength(length) != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Length of struct is not read correctly");
    return ETCH_ERROR;
  }

  result = new EtchStructValue(t, mVf, length);
  return ETCH_OK;
}

status_t EtchBinaryTaggedDataInput::endStruct(capu::SmartPointer<EtchStructValue> result) {
  return ETCH_OK;
}

status_t EtchBinaryTaggedDataInput::startArray(EtchArrayValue *& result) {
  capu::int8_t type;
  mBuffer->getByte(type);

  EtchType* customStructType = NULL;
  if (type == EtchTypeCode::CUSTOM || type == EtchTypeCode::STRUCT) {
    if (readType(customStructType) != ETCH_OK) {
      ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "type of array is not read correctly");
      return ETCH_ERROR;
    }
  } else {
    customStructType = NULL;
  }

  capu::int32_t dim;
  if (readIntegerValue(dim) != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "dimension of array is not read correctly");
    return ETCH_EINVAL;
  }

  if (dim <= 0 || dim > (capu::int32_t)EtchValidator::MAX_NDIMS) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "dimension of array is not within predefined limits");
    return ETCH_EINVAL;
  }

  capu::int32_t length;
  if (readLength(length) != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "length of array is not read correctly");
    return ETCH_ERROR;
  }

  capu::SmartPointer<EtchNativeArrayBase> array;
  if (allocNativeArray(type, customStructType, dim, length, array) != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Allocation of array with specified type has failed");
    return ETCH_ERROR;
  }

  result = new EtchArrayValue(array, length, type, customStructType, dim);
  result->setIndex(0);
  return ETCH_OK;
}

status_t EtchBinaryTaggedDataInput::endArray(EtchArrayValue *array) {
  return ETCH_OK;
}

status_t EtchBinaryTaggedDataInput::readType(EtchType *&type) {
  capu::SmartPointer<EtchObject> obj;
  if (readValue(mIntOrStrValidator, false, obj) != ETCH_OK) {
    return ETCH_ERROR;
  }

  if (obj->getObjectType()->equals(EtchInt32::TYPE())) {
    EtchInt32* id = (EtchInt32*) obj.get();
    if (mVf->getType(id->get(), type) != ETCH_OK) {
      char num[100];
      capu::StringUtils::Sprintf(num, 100, "%d", id->get());
      EtchString str(num);
      type = new EtchType(id->get(), str);
    }
    return ETCH_OK;
  }

  if (obj->getObjectType()->equals(EtchString::TYPE())) {
    EtchString* name = (EtchString*) obj.get();
    if (mVf->getType(*name, type) != ETCH_OK) {
      type = new EtchType(*name);
    }
    return ETCH_OK;
  }

  return ETCH_ERROR;
}

status_t EtchBinaryTaggedDataInput::readField(EtchType *type, EtchField& field) {
  capu::SmartPointer<EtchObject> obj;
  if (readValue(mIntOrStrValidator, true, obj) != ETCH_OK) {
    return ETCH_ERROR;
  }

  if (obj == NONE()) {
    return ETCH_ENOT_EXIST;
  }

  if (obj->getObjectType()->equals(EtchInt32::TYPE())) {
    EtchInt32* id = (EtchInt32*) obj.get();

    if (type->getField(id->get(), &field) != ETCH_OK) {
      char num[100];
      capu::StringUtils::Sprintf(num, 100, "%d", id->get());
      EtchString str(num);
      EtchField f(id->get(), str);
      //TODO: Create field on heap and use smartpointers
      field = f;
    }
    return ETCH_OK;
  }

  if (obj->getObjectType()->equals(EtchString::TYPE())) {
    EtchString* name = (EtchString*) obj.get();
    if (type->getField(*name, &field) != ETCH_OK) {
      EtchField f(*name);
      //TODO: Create field on heap and use smartpointers
      field = f;
    }
    return ETCH_OK;
  }
  return ETCH_ERROR;
}

status_t EtchBinaryTaggedDataInput::readLength(capu::int32_t& result) {
  capu::int32_t length = 0;
  if (readIntegerValue(length) != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "length information is not read correctly");
    return ETCH_ERROR;
  }

  if ((length < 0) || (length > mLengthBudget)) {
    return ETCH_EIO;
  }

  mLengthBudget -= length;

  result = length;
  return ETCH_OK;
}

status_t EtchBinaryTaggedDataInput::readIntegerValue(capu::int32_t& result) {

  capu::SmartPointer<EtchObject> tmp;
  if (readValue(mIntValidator, tmp) != ETCH_OK) {
    return ETCH_ERROR;
  }
  result = capu::smartpointer_cast<EtchInt32>(tmp)->get();
  return ETCH_OK;
}

status_t EtchBinaryTaggedDataInput::validateValue(capu::SmartPointer<EtchValidator> v, capu::SmartPointer<EtchObject> value, capu::SmartPointer<EtchObject>& result) {
  // v == null more or less implies that a field is not known
  // for a type. thus we don't care about the field value as
  // we are going to ignore it. therefore, return null.
  if (v.get() == NULL)
    return ETCH_OK;

  if (value.get() == NULL)
    return ETCH_OK;

  if (v->validateValue(value, result) == ETCH_OK) {
    return ETCH_OK;
  } else {
    return ETCH_ERROR;
  }
}

status_t EtchBinaryTaggedDataInput::validateValue(capu::SmartPointer<EtchValidator> v, capu::bool_t noneOk, capu::SmartPointer<EtchObject> value, capu::SmartPointer<EtchObject>& result) {
  if (noneOk && (value == NONE())) {
    result = value;
    return ETCH_OK;
  }
  return validateValue(v, value, result);
}

status_t EtchBinaryTaggedDataInput::validate(capu::SmartPointer<EtchValidator> v, capu::SmartPointer<EtchObject> value) {
  // v == null more or less implies that a field is not known
  // for a type. thus we don't care about the field value as
  // we are going to ignore it. therefore, return null.
  if (v.get() == NULL)
    return ETCH_ERROR;

  if (value.get() == NULL)
    return ETCH_ERROR;


  if (v->validate(value)) {
    return ETCH_OK;
  } else {
    return ETCH_ERROR;
  }
}

status_t EtchBinaryTaggedDataInput::readValue(capu::SmartPointer<EtchValidator> v, capu::SmartPointer<EtchObject>& result) {
  return readValue(v, false, result);
}

status_t EtchBinaryTaggedDataInput::readValue(capu::SmartPointer<EtchValidator> v, capu::bool_t noneOk, capu::SmartPointer<EtchObject>& result) {
  capu::int8_t type;
  if (mBuffer->getByte(type) != ETCH_OK) {
    return ETCH_ERROR;
  }

  switch (type) {
    case EtchTypeCode::Null:
      return validateValue(v, NULL, result);

    case EtchTypeCode::NONE:
      return validateValue(v, noneOk, NONE(), result);

    case EtchTypeCode::BOOLEAN_FALSE:
    {

      capu::SmartPointer<EtchBool> boolean = new EtchBool(false);
      if (validateValue(v, boolean, result) != ETCH_OK) {
        return ETCH_ERROR;
      } else {
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Boolean false value has been received");
        return ETCH_OK;
      }
    }

    case EtchTypeCode::BOOLEAN_TRUE:
    {
      capu::SmartPointer<EtchBool> boolean = new EtchBool(true);
      if (validateValue(v, boolean, result) != ETCH_OK) {
        return ETCH_ERROR;
      } else {
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Boolean true value has been received");

        return ETCH_OK;
      }
    }

    case EtchTypeCode::BYTE:
    {
      capu::int8_t tmp;
      if (mBuffer->getByte(tmp) != ETCH_OK)
        return ETCH_ERROR;
      capu::SmartPointer<EtchByte> byte = new EtchByte(tmp);
      if (validateValue(v, byte, result) != ETCH_OK) {
        return ETCH_ERROR;
      } else {
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Byte value has been received");
        return ETCH_OK;
      }
    }

    case EtchTypeCode::SHORT:
    {
      capu::int16_t tmp;
      if (mBuffer->getShort(tmp) != ETCH_OK)
        return ETCH_ERROR;
      capu::SmartPointer<EtchShort> shortNum = new EtchShort(tmp);
      if (validateValue(v, shortNum, result) != ETCH_OK) {
        return ETCH_ERROR;
      } else {
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Short value has been received");
        return ETCH_OK;
      }
    }

    case EtchTypeCode::INT:
    {
      capu::uint32_t tmp;
      if (mBuffer->getInteger(tmp) != ETCH_OK)
        return ETCH_ERROR;
      capu::SmartPointer<EtchInt32> intNum = new EtchInt32(tmp);
      if (validateValue(v, intNum, result) != ETCH_OK) {
        return ETCH_ERROR;
      } else {
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Int value has been received");
        return ETCH_OK;
      }
    }

    case EtchTypeCode::LONG:
    {
      capu::int64_t tmp;
      if (mBuffer->getLong(tmp) != ETCH_OK)
        return ETCH_ERROR;
      capu::SmartPointer<EtchLong> longNum = new EtchLong(tmp);
      if (validateValue(v, longNum, result) != ETCH_OK) {
        return ETCH_ERROR;
      } else {
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Long value has been received");
        return ETCH_OK;
      }
    }

    case EtchTypeCode::FLOAT:
    {
      capu::float_t tmp;
      if (mBuffer->getFloat(tmp) != ETCH_OK)
        return ETCH_ERROR;
      capu::SmartPointer<EtchFloat> floatNum = new EtchFloat(tmp);
      if (validateValue(v, floatNum, result) != ETCH_OK) {
        return ETCH_ERROR;
      } else {
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Float value has been received");
        return ETCH_OK;
      }
    }

    case EtchTypeCode::DOUBLE:
    {
      capu::double_t tmp;
      if (mBuffer->getDouble(tmp) != ETCH_OK)
        return ETCH_ERROR;
      capu::SmartPointer<EtchDouble> doubleNum = new EtchDouble(tmp);
      if (validateValue(v, doubleNum, result) != ETCH_OK) {
        return ETCH_ERROR;
      } else {
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Double value has been received");
        return ETCH_OK;
      }
    }

    case EtchTypeCode::BYTES:
    {
      status_t ret;

      capu::int8_t *buffer = NULL;
      capu::uint32_t bufferSize = 0;

      ret = readBytes(buffer, bufferSize);
      if (ret != ETCH_OK) {
        return ETCH_ERROR;
      }

      capu::SmartPointer<EtchNativeArray<capu::int8_t> > narray = new EtchNativeArray<capu::int8_t > (bufferSize);
      narray->set(0, buffer, bufferSize, 0, bufferSize);
      delete[] buffer;

      ret = validateValue(v, narray, result);

      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Byte Array has been received");

      return ret;
    }

      //			reserved for future use:
      //			case TypeCode.BOOLS:
      //			case TypeCode.SHORTS:
      //			case TypeCode.INTS:
      //			case TypeCode.LONGS:
      //			case TypeCode.FLOATS:
      //			case TypeCode.DOUBLES:

    case EtchTypeCode::ARRAY:
    {
      status_t ret;
      EtchArrayValue *arrayval = NULL;
      ret = readArray(v, arrayval);
      if (ret != ETCH_OK) {
        if (arrayval != NULL)
          delete arrayval;
        return ETCH_ERROR;
      }

      capu::SmartPointer<EtchObject> array;
      ret = fromArrayValue(arrayval, array);
      if (ret != ETCH_OK) {
        delete arrayval;
        return ETCH_ERROR;
      }
      delete arrayval;

      if (validate(v, array) != ETCH_OK) {
        return ETCH_ERROR;
      } else {
        result = array;
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Array has been received");
        return ETCH_OK;
      }
    }
    case EtchTypeCode::EMPTY_STRING:
    {
      EtchString *str = new EtchString(NULL, 0, mVf->getStringEncoding());
      if (validateValue(v, str, result) != ETCH_OK) {
        delete str;
        return ETCH_ERROR;
      } else {
        result = str;
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Empty String value has been received");
        return ETCH_OK;
      }
    }

    case EtchTypeCode::STRING:
    {
      status_t ret;
      capu::int8_t *buffer = NULL;
      capu::uint32_t bufferSize;

      ret = readBytes(buffer, bufferSize);
      if (ret != ETCH_OK)
        return ETCH_ERROR;

      capu::SmartPointer<EtchString> str = new EtchString(buffer, bufferSize, mVf->getStringEncoding());
      delete [] buffer;
      ret = validateValue(v, str, result);
      if (ret != ETCH_OK) {
        return ETCH_ERROR;
      } else {
        result = str;
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "String value has been received");
        return ETCH_OK;
      }
    }
    case EtchTypeCode::STRUCT:
    case EtchTypeCode::CUSTOM:
    {
      capu::SmartPointer<EtchStructValue> sv;
      if (readStruct(sv) != ETCH_OK) {
        return ETCH_ERROR;
      }

      capu::SmartPointer<EtchObject> obj;
      if (mVf->importCustomValue(sv.get(), obj) != ETCH_OK) {
        return ETCH_ERROR;
      }

      if (validateValue(v, obj, result) != ETCH_OK) {
        return ETCH_ERROR;
      } else {
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Custom value has been received");
        return ETCH_OK;
      }
    }

    default:
    {
      if (type >= EtchTypeCode::MIN_TINY_INT && type <= EtchTypeCode::MAX_TINY_INT) {
        capu::SmartPointer<EtchByte> tmp = new EtchByte(type);
        if (validateValue(v, tmp, result) != ETCH_OK) {
          return ETCH_ERROR;
        } else {
          ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Tiny Int value has been received");
          return ETCH_OK;
        }
      }
      return ETCH_ERANGE;
    }
  }
}

status_t EtchBinaryTaggedDataInput::readBytes(capu::int8_t*& array, capu::uint32_t &length) {
  capu::int32_t _length;
  if (readLength(_length) != ETCH_OK)
    return ETCH_ERROR;
  array = new capu::int8_t[_length];
  mBuffer->get(array, (capu::uint32_t) 0, (capu::uint32_t) _length, length);
  if (_length != (capu::int32_t) length) {
    delete[] array;
    return ETCH_ERROR;
  }
  return ETCH_OK;
}
