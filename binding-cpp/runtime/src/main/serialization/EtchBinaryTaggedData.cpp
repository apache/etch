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

#include "capu/os/NumericLimits.h"
#include "serialization/EtchBinaryTaggedData.h"

capu::SmartPointer<EtchObject>& EtchBinaryTaggedData::NONE()
{
  static capu::SmartPointer<EtchObject> ret = new EtchString("NONE");
  return ret;
}

EtchBinaryTaggedData::EtchBinaryTaggedData(EtchValueFactory* vf)
: EtchTaggedData(vf) {

}

EtchBinaryTaggedData::~EtchBinaryTaggedData() {

}

capu::int8_t EtchBinaryTaggedData::checkValue(capu::SmartPointer<EtchObject> value) {
  if (value.get() == NULL) {
    return EtchTypeCode::Null;
  }

  if (value.get() == NONE().get()) {
    return EtchTypeCode::NONE;
  }

  if (value->getObjectType()->equals(EtchBool::TYPE())) {
    capu::SmartPointer<EtchBool> _bool = capu::smartpointer_cast<EtchBool> (value);
    if (_bool->get()) {
      return EtchTypeCode::BOOLEAN_TRUE;
    }
    return EtchTypeCode::BOOLEAN_FALSE;
  }

  if (value->getObjectType()->equals(EtchByte::TYPE())) {
    capu::SmartPointer<EtchByte> _byte = capu::smartpointer_cast<EtchByte> (value);
    return checkByte(_byte->get());
  }

  if (value->getObjectType()->equals(EtchShort::TYPE())) {
    capu::SmartPointer<EtchShort> _short = capu::smartpointer_cast<EtchShort> (value);
    return checkShort(_short->get());
  }

  if (value->getObjectType()->equals(EtchInt32::TYPE())) {
    capu::SmartPointer<EtchInt32> _int = capu::smartpointer_cast<EtchInt32> (value);
    return checkInteger(_int->get());
  }

  if (value->getObjectType()->equals(EtchLong::TYPE())) {
    capu::SmartPointer<EtchLong> _long = capu::smartpointer_cast<EtchLong> (value);
    return checkLong(_long->get());
  }

  if (value->getObjectType()->equals(EtchFloat::TYPE())) {
    return EtchTypeCode::FLOAT;
  }

  if (value->getObjectType()->equals(EtchDouble::TYPE())) {
    return EtchTypeCode::DOUBLE;
  }

  if (value->getObjectType()->equals(EtchString::TYPE())) {
    capu::SmartPointer<EtchString> _string = capu::smartpointer_cast<EtchString> (value);
    if (_string->length() == 0)
      return EtchTypeCode::EMPTY_STRING;
    return EtchTypeCode::STRING;
  }

  if (value->getObjectType()->isArray()) {
    capu::SmartPointer<EtchNativeArrayBase> array = capu::smartpointer_cast<EtchNativeArrayBase> (value);
    if (array->getObjectType()->getObjectComponentType()->equals(&EtchObjectType::NATIVE_INT8()) && array->getDim() == 1) {
      return EtchTypeCode::BYTES;
    }
    return EtchTypeCode::ARRAY;
  }

  return EtchTypeCode::CUSTOM;
}

capu::int8_t EtchBinaryTaggedData::checkByte(capu::int8_t v) {
  if ((v >= EtchTypeCode::MIN_TINY_INT) && (v <= EtchTypeCode::MAX_TINY_INT)) {
    return v;
  }

  return EtchTypeCode::BYTE;
}

capu::int8_t EtchBinaryTaggedData::checkShort(capu::int16_t v) {
  if ((v >= capu::NumericLimits::Min<capu::int8_t>()) && (v <= capu::NumericLimits::Max<capu::int8_t>()))
    return checkByte((capu::int8_t) v);

  return EtchTypeCode::SHORT;
}

capu::int8_t EtchBinaryTaggedData::checkInteger(capu::int32_t v) {
  if ((v >= capu::NumericLimits::Min<capu::int16_t>()) && (v <= capu::NumericLimits::Max<capu::int16_t>()))
    return checkShort((capu::int16_t) v);

  return EtchTypeCode::INT;
}

capu::int8_t EtchBinaryTaggedData::checkLong(capu::int64_t v) {
  if (v >= capu::NumericLimits::Min<capu::int32_t>() && v <= capu::NumericLimits::Max<capu::int32_t>())
    return checkInteger((capu::int32_t) v);

  return EtchTypeCode::LONG;
}

capu::int8_t EtchBinaryTaggedData::getNativeTypeCode(const EtchObjectType *c) {
  if ((c->equals(&EtchObjectType::NATIVE_BOOL()))|| (c->equals(EtchBool::TYPE())))
    return EtchTypeCode::BOOLEAN_TRUE;

  if ((c->equals(&EtchObjectType::NATIVE_INT8())) || (c->equals(EtchByte::TYPE())))
    return EtchTypeCode::BYTE;

  if ((c->equals(&EtchObjectType::NATIVE_INT16())) || (c->equals(EtchShort::TYPE())))
    return EtchTypeCode::SHORT;

  if ((c->equals(&EtchObjectType::NATIVE_INT32())) || (c->equals(EtchInt32::TYPE())))
    return EtchTypeCode::INT;

  if ((c->equals(&EtchObjectType::NATIVE_INT64())) || (c->equals(EtchLong::TYPE())))
    return EtchTypeCode::LONG;

  if ((c->equals(&EtchObjectType::NATIVE_FLOAT())) || (c->equals(EtchFloat::TYPE())))
    return EtchTypeCode::FLOAT;

  if ((c->equals(&EtchObjectType::NATIVE_DOUBLE())) || (c->equals(EtchDouble::TYPE())))
    return EtchTypeCode::DOUBLE;

  if (c->equals(EtchString::TYPE()))
    return EtchTypeCode::STRING;

  if (c->equals(EtchObject::TYPE()))
    return EtchTypeCode::ANY;

  return EtchTypeCode::CUSTOM;
}

status_t EtchBinaryTaggedData::getCustomStructType(const EtchObjectType* c, EtchType*& result) {
  return mVf->getCustomStructType(c, result);
}

const EtchObjectType* EtchBinaryTaggedData::getNativeType(capu::int8_t type) {
  switch (type) {
    case EtchTypeCode::BOOLEAN_TRUE:
    case EtchTypeCode::BOOLEAN_FALSE:
      return EtchBool::TYPE();

    case EtchTypeCode::BYTE:
      return EtchByte::TYPE();

    case EtchTypeCode::SHORT:
      return EtchShort::TYPE();

    case EtchTypeCode::INT:
      return EtchInt32::TYPE();

    case EtchTypeCode::LONG:
      return EtchLong::TYPE();

    case EtchTypeCode::FLOAT:
      return EtchFloat::TYPE();

    case EtchTypeCode::DOUBLE:
      return EtchDouble::TYPE();

    case EtchTypeCode::EMPTY_STRING:
    case EtchTypeCode::STRING:
      return EtchString::TYPE();

    case EtchTypeCode::ANY:
      return EtchObject::TYPE();

    default:
      return NULL;
  }
}
