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
#include "serialization/EtchBinaryTaggedDataOutput.h"
#include "support/EtchRuntime.h"

const EtchString& EtchBinaryTaggedDataOutput::STRING_TYPE_AND_FIELD() {
  static const EtchString name("BinaryTaggedDataOutput.stringTypeAndField");
  return name;
}

EtchBinaryTaggedDataOutput::EtchBinaryTaggedDataOutput(EtchRuntime* runtime, EtchValueFactory *vf, EtchURL* uri)
: EtchBinaryTaggedData(vf), mRuntime(runtime), mLevel(vf->getLevel()), mBuffer(NULL) {
  EtchString tmp;
  if (uri->getTerms().get(STRING_TYPE_AND_FIELD(), &tmp) == ETCH_OK) {
    mStringTypeAndField = atoi(tmp.c_str()) > 0;
  } else {
    mStringTypeAndField = false;
  }
  EtchValidatorInt::Get(mRuntime, 0, mIntValidator);
  EtchValidatorString::Get(mRuntime, 0, mStringValidator);
  EtchValidatorNone::Get(mRuntime, mNoneValidator);
}

EtchBinaryTaggedDataOutput::~EtchBinaryTaggedDataOutput() {
}

status_t EtchBinaryTaggedDataOutput::writeMessage(capu::SmartPointer<EtchMessage> msg, capu::SmartPointer<EtchFlexBuffer> buf) {
  if ((buf.get() == NULL) || (msg.get() == NULL)) {
    return ETCH_EINVAL;
  }

  status_t ret;
  mBuffer = buf;

  ret = startMessage(msg);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Serialization of Message header has failed");
    return ETCH_ERROR;
  }

  ret = writeKeysAndValues(msg);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Serialization of Message body has failed");
    return ETCH_ERROR;
  }

  ret = endMessage(msg);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Serialization of Message end has failed");
    return ETCH_ERROR;
  }

  return ETCH_OK;
}

status_t EtchBinaryTaggedDataOutput::writeStruct(capu::SmartPointer<EtchStructValue> sv) {
  status_t ret;

  ret = startStruct(sv);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Serialization of struct header has failed");
    return ETCH_ERROR;
  }

  ret = writeKeysAndValues(sv);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Serialization of struct body has failed");
    return ETCH_ERROR;
  }

  ret = endStruct(sv);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Serialization of struct end has failed");
    return ETCH_ERROR;
  }

  return ETCH_OK;
}

status_t EtchBinaryTaggedDataOutput::writeArray(EtchArrayValue* av, EtchValidator* v) {
  status_t ret;

  ret = startArray(av);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Serialization of array header has failed");
    return ETCH_ERROR;
  }

  ret = writeValues(av, v);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Serialization of array body has failed");
    return ETCH_ERROR;
  }

  ret = endArray(av);
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Serialization of array end has failed");
    return ETCH_ERROR;
  }

  delete av;
  return ETCH_OK;
}

status_t EtchBinaryTaggedDataOutput::writeKeysAndValues(capu::SmartPointer<EtchStructValue> sv) {
  if (sv.get() == NULL) {
    return ETCH_EINVAL;
  }

  status_t ret;
  EtchType* t = sv->getType();
  EtchStructValue::Iterator it = sv->begin();
  EtchStructValue::HashTableEntry entry;
  capu::SmartPointer<EtchValidator> tmp;

  while (it.hasNext()) {
    if (it.next(&entry) == ETCH_OK) {

      EtchField f = entry.key;

      ret = writeField(&f);
      if (ret != ETCH_OK) {
        return ETCH_ERROR;
      }

      ret = t->getValidator(f, tmp);
      if (ret != ETCH_OK) {
        return ETCH_ERROR;
      }

      ret = writeValue(tmp, entry.value);
      if (ret != ETCH_OK) {
        return ETCH_ERROR;
      }
    } else {
      return ETCH_ERROR;
    }
   
  }
  return ETCH_OK;
}

status_t EtchBinaryTaggedDataOutput::writeValues(EtchArrayValue* av, EtchValidator* v) {
  if ((v == NULL) || (av == NULL))
    return ETCH_EINVAL;

  capu::SmartPointer<EtchValidator> validatorElement;
  if (v->getElementValidator(validatorElement) != ETCH_OK)
    return ETCH_ERROR;

  capu::int32_t size = av->getIndex();
  for (capu::int32_t i = 0; i < size; i++) {
    capu::SmartPointer<EtchObject> tmp;
    av->get(i, tmp);
    if (writeValue(validatorElement, tmp) != ETCH_OK)
      return ETCH_ERROR;
  }
  return ETCH_OK;
}

status_t EtchBinaryTaggedDataOutput::startMessage(capu::SmartPointer<EtchMessage> msg) {
  // since message is top-level, no type code is written to
  // indicate a message is starting. we do write a version
  // number to indicate this version of the binary tagged
  // data output format.
  mBuffer->putByte(VERSION());
  return startStruct(msg);
}

status_t EtchBinaryTaggedDataOutput::endMessage(capu::SmartPointer<EtchMessage> msg) {
  return endStruct(msg);
}

status_t EtchBinaryTaggedDataOutput::startStruct(capu::SmartPointer<EtchStructValue> _struct) {
  // the caller has already written a type code to indicate a
  // struct is starting. this code is shared by startMessage().
  if (writeType(_struct->getType()) != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Serialization of struct type information has failed");
    return ETCH_ERROR;
  } else if (writeIntValue(_struct->count()) != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Serialization of field number on struct has failed");
    return ETCH_ERROR;
  }
  return ETCH_OK;
}

status_t EtchBinaryTaggedDataOutput::endStruct(capu::SmartPointer<EtchStructValue> _struct) {
  // this code is shared by endMessage.
  return writeNoneValue();
}

status_t EtchBinaryTaggedDataOutput::startArray(EtchArrayValue* array) {
  // the caller has already written a type code to indicate an
  // array is starting.
  if (array == NULL) {
    return ETCH_ERROR;
  }

  status_t ret;

  capu::int8_t type = array->getTypeCode();
  mBuffer->putByte(type);

  if (type == EtchTypeCode::CUSTOM) {
    ret = writeType(array->getCustomStructType());
    if (ret != ETCH_OK) {
      ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Serialization of array type information has failed");
      return ETCH_ERROR;
    }
  }

  ret = writeIntValue(array->getDim());
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Serialization of array dimension information has failed");
    return ETCH_ERROR;
  }

  ret = writeIntValue(array->getSize());
  if (ret != ETCH_OK) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Serialization of array size information has failed");
    return ETCH_ERROR;
  }

  return ETCH_OK;

}

status_t EtchBinaryTaggedDataOutput::endArray(EtchArrayValue* array) {
  return writeNoneValue();
}

status_t EtchBinaryTaggedDataOutput::writeType(EtchType* type) {
  if (mStringTypeAndField) {
    capu::SmartPointer<EtchString> tmp = new EtchString(type->getName());
    return writeStringValue(tmp);
  } else
    return writeIntValue(type->getId());
}

status_t EtchBinaryTaggedDataOutput::writeIntValue(capu::int32_t value) {
  capu::SmartPointer<EtchObject> tmp = new EtchInt32(value);
  return writeValue(mIntValidator, tmp);
}

status_t EtchBinaryTaggedDataOutput::writeStringValue(capu::SmartPointer<EtchString> value) {
  return writeValue(mStringValidator, capu::smartpointer_cast<EtchObject > (value));
}

status_t EtchBinaryTaggedDataOutput::writeNoneValue() {
  return writeValue(mNoneValidator, NONE());
}

status_t EtchBinaryTaggedDataOutput::writeField(EtchField* field) {
  if (mStringTypeAndField) {
    capu::SmartPointer<EtchString> tmp = new EtchString(field->getName());
    return writeStringValue(tmp);
  } else {
    return writeIntValue(field->getId());
  }
}

status_t EtchBinaryTaggedDataOutput::writeValue(capu::SmartPointer<EtchValidator> v, capu::SmartPointer<EtchObject> value) {
  capu::int8_t typeCode = checkValue(value);

  if (mLevel != LEVEL_NONE && value.get() != NULL) {
    if (mLevel == LEVEL_FULL && v.get() == NULL) {
      return ETCH_ENOT_EXIST; // missing validator
    }

    if (v.get() != NULL && !v->validate(value)) {
      return ETCH_ERROR; //validation error
    }
  }

  mBuffer->putByte(typeCode);

  switch (typeCode) {
    case EtchTypeCode::Null:
    case EtchTypeCode::NONE:
      return ETCH_OK;
    case EtchTypeCode::BOOLEAN_FALSE:
      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Boolean False has been serialized. (No Data send - encoded via TypeId)");
      return ETCH_OK;
    case EtchTypeCode::BOOLEAN_TRUE:
      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Boolean True has been serialized. (No Data send - encoded via TypeId)");
      return ETCH_OK;
    case EtchTypeCode::EMPTY_STRING:
      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Empty String has been serialized. (No Data send - encoded via TypeId)");
      return ETCH_OK;

    case EtchTypeCode::BYTE:
      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Byte has been serialized");
      return mBuffer->putByte(((EtchNumber*) value.get())->getByteValue());

    case EtchTypeCode::SHORT:
      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Short has been serialized");
      return mBuffer->putShort(((EtchNumber*) value.get())->getShortValue());

    case EtchTypeCode::INT:
      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Int has been serialized");
      return mBuffer->putInt(((EtchNumber*) value.get())->getInt32Value());

    case EtchTypeCode::LONG:
      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Long has been serialized");
      return mBuffer->putLong(((EtchNumber*) value.get())->getLongValue());

    case EtchTypeCode::FLOAT:
      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Float has been serialized");

      return mBuffer->putFloat(((EtchFloat*) value.get())->get());

    case EtchTypeCode::DOUBLE:
      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Double has been serialized");
      return mBuffer->putDouble(((EtchDouble*) value.get())->get());


    case EtchTypeCode::BYTES:
    {
      EtchNativeArray<capu::int8_t> *na = (EtchNativeArray<capu::int8_t> *)value.get();
      capu::int8_t *data = new capu::int8_t[na->getLength()];
      na->get(Pos(0), data, na->getLength(), 0, na->getLength());
      status_t ret = writeBytes(data, na->getLength());
      delete[] data;
      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Byte Array has been serialized");
      return ret;
    }


      //			reserved for future use:
      //			case EtchTypeCode::BOOLS:
      //			case EtchTypeCode::SHORTS:
      //			case EtchTypeCode::INTS:
      //			case EtchTypeCode::LONGS:
      //			case EtchTypeCode::FLOATS:
      //			case EtchTypeCode::DOUBLES:

    case EtchTypeCode::ARRAY:
    {
      EtchArrayValue *val = NULL;
      if (toArrayValue(value, v.get(), val) != ETCH_OK) {
        ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Array couldn't be serialized");
        return ETCH_ERROR;
      }
      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Array has been serialized");
      return writeArray(val, v.get());
    }
    case EtchTypeCode::STRING:
    {
      capu::SmartPointer<EtchString> str = capu::smartpointer_cast<EtchString > (value);
      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "String has been serialized");
      return writeBytes((capu::int8_t*)str->c_str(), str->getNumBytes());
    }
    case EtchTypeCode::CUSTOM:
    {
      EtchStructValue* _struct;
      if (mVf->exportCustomValue(value, _struct) != ETCH_OK) {
        ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Custom Data Type couldn't be serialized");
        return ETCH_ERROR;
      }
      ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Custom Data Type has been serialized");
      return writeStruct(_struct);
    }

    default:
      // type is either "tiny" integer or unused
      if (typeCode >= EtchTypeCode::MIN_TINY_INT && typeCode <= EtchTypeCode::MAX_TINY_INT) {
        ETCH_LOG_TRACE(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Tiny Int has been serialized");
        return ETCH_OK;
      }
      //invalid type
      ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getSerializerContext(), "Attempted serialization of an invalid Data Type");
      return ETCH_EINVAL;
  }
  return ETCH_ERROR;
}

status_t EtchBinaryTaggedDataOutput::writeBytes(capu::int8_t* value, capu::int32_t size) {
  if (writeIntValue(size) != ETCH_OK) {
    return ETCH_ERROR;
  }
  mBuffer->put(value, size);
  return ETCH_OK;
}

