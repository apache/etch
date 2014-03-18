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

#include "serialization/EtchDefaultValueFactory.h"

const EtchString& EtchDefaultValueFactory::ETCH_RUNTIME_EXCEPTION_TYPE_NAME() {
  static const EtchString name("_Etch_RuntimeException");
  return name;
}

const EtchString& EtchDefaultValueFactory::ETCH_LIST_TYPE_NAME() {
  static const EtchString name("_Etch_List");
  return name;
}

const EtchString& EtchDefaultValueFactory::ETCH_MAP_TYPE_NAME() {
  static const EtchString name("_Etch_Map");
  return name;
}

const EtchString& EtchDefaultValueFactory::ETCH_SET_TYPE_NAME() {
  static const EtchString name("_Etch_Set");
  return name;
}

const EtchString& EtchDefaultValueFactory::ETCH_DATETIME_TYPE_NAME() {
  static const EtchString name("_Etch_Datetime");
  return name;
}

const EtchString& EtchDefaultValueFactory::ETCH_AUTH_EXCEPTION_TYPE_NAME() {
  static const EtchString name("_Etch_AuthException");
  return name;
}

const EtchString& EtchDefaultValueFactory::ETCH_EXCEPTION_MESSAGE_NAME() {
  static const EtchString name("_exception");
  return name;
}

const EtchString& EtchDefaultValueFactory::MSG_FIELD_NAME() {
  static const EtchString name("msg");
  return name;
}

const EtchString& EtchDefaultValueFactory::MESSAGE_ID_FIELD_NAME() {
  static const EtchString name("_messageId");
  return name;
}

const EtchString& EtchDefaultValueFactory::IN_REPLY_TO_FIELD_NAME() {
  static const EtchString name("_inReplyTo");
  return name;
}

const EtchString& EtchDefaultValueFactory::RESULT_FIELD_NAME() {
  static const EtchString name("result");
  return name;
}

const EtchField& EtchDefaultValueFactory::_mf_msg() {
  static const EtchField name(EtchDefaultValueFactory::MSG_FIELD_NAME());
  return name;
}

const EtchField& EtchDefaultValueFactory::_mf__messageId() {
  static const EtchField name(EtchDefaultValueFactory::MESSAGE_ID_FIELD_NAME());
  return name;
}

const EtchField& EtchDefaultValueFactory::_mf__inReplyTo() {
  static const EtchField name(EtchDefaultValueFactory::IN_REPLY_TO_FIELD_NAME());
  return name;
}

const EtchField& EtchDefaultValueFactory::_mf_result() {
  static const EtchField name(EtchDefaultValueFactory::RESULT_FIELD_NAME());
  return name;
}

capu::Mutex EtchDefaultValueFactory::Mutex;

EtchDefaultValueFactory::EtchDefaultValueFactory(EtchString uri, EtchTypeMap* types, EtchClass2TypeMap* class2type)
: mMixins(NULL), mTypes(types), mClass2type(class2type) {
  EtchString result, key("DefaultValueFactory.level");
  EtchURL u(uri);
  status_t error = u.getTerms().get(key, &result);
  if (error != ETCH_OK) {
    //default
    mLevel = LEVEL_FULL;
  } else {
    if (result.c_str() != NULL)
      mLevel = atoi(result.c_str());
    if ((mLevel > LEVEL_FULL) || (mLevel < LEVEL_NONE)) {
      mLevel = LEVEL_FULL;
    }
  }
  mMixins = new EtchList<EtchValueFactory*> ();
  mDynamicTypes = new EtchTypeMap();
  if (types != NULL) {
    types->get(EtchDefaultValueFactory::ETCH_AUTH_EXCEPTION_TYPE_NAME(), m_mt__Etch_AuthException);
    types->get(EtchDefaultValueFactory::ETCH_DATETIME_TYPE_NAME(), m_mt__Etch_Datetime);
    types->get(EtchDefaultValueFactory::ETCH_LIST_TYPE_NAME(), m_mt__Etch_List);
    types->get(EtchDefaultValueFactory::ETCH_MAP_TYPE_NAME(), m_mt__Etch_Map);
    types->get(EtchDefaultValueFactory::ETCH_RUNTIME_EXCEPTION_TYPE_NAME(), m_mt__Etch_RuntimeException);
    types->get(EtchDefaultValueFactory::ETCH_SET_TYPE_NAME(), m_mt__Etch_Set);
    types->get(EtchDefaultValueFactory::ETCH_EXCEPTION_MESSAGE_NAME(), m_mt__exception);
  }

}

EtchDefaultValueFactory::~EtchDefaultValueFactory() {

  mDynamicTypes->clear();
  delete mMixins;
  delete mDynamicTypes;
}

status_t EtchDefaultValueFactory::Init(EtchRuntime* runtime, EtchTypeMap* types, EtchClass2TypeMap* class2type) {
  status_t result = ETCH_OK;

  EtchType *type;
  result |= types->get(ETCH_RUNTIME_EXCEPTION_TYPE_NAME(), type);

  result |= EtchRuntimeExceptionSerializer::Init(runtime, type, class2type);

  result |= types->get(ETCH_LIST_TYPE_NAME(), type);
  result |= EtchListSerializer::Init(runtime, type, class2type);

  result |= types->get(ETCH_MAP_TYPE_NAME(), type);
  result |= EtchHashTableSerializer::Init(runtime, type, class2type);

  result |= types->get(ETCH_SET_TYPE_NAME(), type);
  result |= EtchSetSerializer::Init(runtime, type, class2type);

  result |= types->get(ETCH_DATETIME_TYPE_NAME(), type);
  result |= EtchDateSerializer::Init(runtime, type, class2type);

  result |= types->get(ETCH_AUTH_EXCEPTION_TYPE_NAME(), type);
  result |= EtchAuthenticationExceptionSerializer::Init(runtime, type, class2type);

  {
    result |= types->get(ETCH_EXCEPTION_MESSAGE_NAME(), type);
    capu::SmartPointer<EtchValidator> val;
    EtchValidatorRuntimeException::Get(runtime, val);
    result |= type->putValidator(_mf_result(), val);
    EtchValidatorLong::Get(runtime, 0, val);
    result |= type->putValidator(_mf__messageId(), val);
    result |= type->putValidator(_mf__inReplyTo(), val);
  }

  return result;
}

const EtchType * EtchDefaultValueFactory::get_mt__Etch_RuntimeException() {
  return m_mt__Etch_RuntimeException;
}

const EtchType * EtchDefaultValueFactory::get_mt__Etch_List() {
  return m_mt__Etch_List;
}

const EtchType* EtchDefaultValueFactory::get_mt__Etch_Map() {
  return m_mt__Etch_Map;
}

const EtchType* EtchDefaultValueFactory::get_mt__Etch_Set() {
  return m_mt__Etch_Set;
}

const EtchType* EtchDefaultValueFactory::get_mt__Etch_Datetime() {
  return m_mt__Etch_Datetime;
}

const EtchType* EtchDefaultValueFactory::get_mt__Etch_AuthException() {
  return m_mt__Etch_AuthException;
}

EtchType* EtchDefaultValueFactory::get_mt__exception() {
  return m_mt__exception;
}

status_t EtchDefaultValueFactory::getType(capu::uint32_t id, EtchType *&result) {
  status_t res = mTypes->get(id, result);
  if (res == ETCH_OK)
    return ETCH_OK;

  if (mMixins == NULL)
    return ETCH_ERROR;

  EtchValueFactory *tmp;
  for (int i = 0; i < mMixins->size(); i++) {
    if (mMixins->get(i, &tmp) == ETCH_OK) {
      if (tmp->getType(id, result) == ETCH_OK)
        return ETCH_OK;
    }
  }

  Mutex.lock();
  res = mDynamicTypes->get(id, result);
  if (res == ETCH_OK) {
    Mutex.unlock();
    return ETCH_OK;
  }
  Mutex.unlock();
  return ETCH_ERROR;
}

status_t EtchDefaultValueFactory::getType(EtchString& name, EtchType *&result) {
  status_t res = mTypes->get(name, result);
  if (res == ETCH_OK)
    return ETCH_OK;

  if (mMixins == NULL)
    return ETCH_ERROR;

  EtchValueFactory *tmp;
  for (int i = 0; i < mMixins->size(); i++) {
    if (mMixins->get(i, &tmp) == ETCH_OK) {
      if (tmp->getType(name, result) == ETCH_OK)
        return ETCH_OK;
    }
  }

  Mutex.lock();
  res = mDynamicTypes->get(name, result);
  if (res == ETCH_OK) {
    Mutex.unlock();
    return ETCH_OK;
  }
  Mutex.unlock();
  return ETCH_ERROR;
}

status_t EtchDefaultValueFactory::addType(EtchType* type) {
  Mutex.lock();
  status_t result = mDynamicTypes->add(type);
  Mutex.unlock();
  return result;
}

status_t EtchDefaultValueFactory::exportCustomValue(capu::SmartPointer<EtchObject> value, EtchStructValue*& result) {
  if (value.get() == NULL)
    return ETCH_EINVAL;
  const EtchObjectType* typeID = value->getObjectType();
  EtchType* type = NULL;
  getCustomStructType(typeID, type);

  if (type == NULL && typeID == EtchStructValue::TYPE()) {
    capu::SmartPointer<EtchStructValue> _struct = capu::smartpointer_cast<EtchStructValue > (value);
    type = _struct->getType();
  }

  if (type == NULL) {
    if ((typeID == EtchAuthenticationException::TYPE()) || (typeID == EtchRuntimeException::TYPE()) || (typeID == EtchException::TYPE()))
      type = m_mt__Etch_RuntimeException;
    else if (typeID->getTypeId() == EOTID_LIST)
      type = m_mt__Etch_List;
    else if (typeID->getTypeId() == EOTID_HASHTABLE)
      type = m_mt__Etch_Map;
    else if (typeID->getTypeId() == EOTID_SET)
      type = m_mt__Etch_Set;
    else {
      return ETCH_EINVAL;
    }
  }

  EtchImportExportHelper* helper = type->getImportExportHelper();

  if (helper == NULL)
    return ETCH_EINVAL;

  return helper->exportValue(this, value, result);
}

status_t EtchDefaultValueFactory::importCustomValue(EtchStructValue* value, capu::SmartPointer<EtchObject> &result) {
  if (value == NULL)
    return ETCH_EINVAL;
  EtchType * type = value->getType();
  EtchImportExportHelper* helper;
  if (type == NULL)
    return ETCH_EINVAL;

  helper = type->getImportExportHelper();
  if (helper == NULL)
    return ETCH_EINVAL;

  return helper->importValue(value, result);
}

status_t EtchDefaultValueFactory::getCustomStructType(const EtchObjectType *c, EtchType *& type) {
  status_t result = mClass2type->get(c, &type);
  if (result == ETCH_OK)
    return ETCH_OK;

  EtchValueFactory *tmp;
  for (int i = 0; i < mMixins->size(); i++) {
    mMixins->get(i, &tmp);
    if (tmp->getCustomStructType(c, type) == ETCH_OK)
      return ETCH_OK;
  }
  return ETCH_EINVAL;
}

status_t EtchDefaultValueFactory::getNativeArrayForComponentType(const EtchObjectType *objectType,  capu::SmartPointer<EtchNativeArrayBase> &nativeArray, capu::int32_t length, capu::int32_t dim) {
  return ETCH_EUNIMPL;
}

EtchString EtchDefaultValueFactory::getStringEncoding() {
  return EtchString("utf-8");
}

status_t EtchDefaultValueFactory::getMessageId(EtchMessage* msg, capu::int64_t &result) {
  if (msg == NULL)
    return ETCH_EINVAL;

  capu::SmartPointer<EtchObject> l;
  if (msg->get(_mf__messageId(), &l) != ETCH_OK)
    return ETCH_ERROR;

  result = ((EtchLong *) l.get())->get();
  return ETCH_OK;
}

status_t EtchDefaultValueFactory::setMessageId(EtchMessage* msg, capu::int64_t msgid) {
  if (msg == NULL)
    return ETCH_EINVAL;
  capu::SmartPointer<EtchObject> value = new EtchLong(msgid);
  status_t res = msg->put(_mf__messageId(), value);
  if (res != ETCH_OK) {
    return res;
  }
  return ETCH_OK;
}

EtchField EtchDefaultValueFactory::get_mf__messageId() {
  return _mf__messageId();
}

status_t EtchDefaultValueFactory::getInReplyToMessageId(EtchMessage* msg, capu::int64_t &result) {
  if (msg == NULL)
    return ETCH_EINVAL;
  capu::SmartPointer<EtchObject> l;
  if (msg->get(_mf__inReplyTo(), &l) != ETCH_OK)
    return ETCH_ERROR;
  result = ((EtchLong *) l.get())->get();
  return ETCH_OK;
}

status_t EtchDefaultValueFactory::setInReplyToMessageId(EtchMessage* msg, capu::int64_t msgid) {
  if (msg == NULL)
    return ETCH_EINVAL;
  capu::SmartPointer<EtchObject> value = new EtchLong(msgid);
  status_t res = msg->put(_mf__inReplyTo(), value);
  if (res != ETCH_OK) {
    return res;
  }
  return ETCH_OK;
}

EtchField EtchDefaultValueFactory::get_mf__inReplyTo() {
  return _mf__inReplyTo();
}

status_t EtchDefaultValueFactory::lockDynamicTypes() {
  mDynamicTypes->lock();
  return ETCH_OK;
}

status_t EtchDefaultValueFactory::unlockDynamicTypes() {
  return ETCH_ENOT_SUPPORTED;
}

EtchLevel EtchDefaultValueFactory::getLevel() {
  return mLevel;
}

EtchLevel EtchDefaultValueFactory::setLevel(EtchLevel level) {
  EtchLevel oldLevel = this->mLevel;
  this->mLevel = level;
  return oldLevel;
}

void EtchDefaultValueFactory::addMixin(EtchValueFactory* vf) {
  mMixins->add(vf);
}
