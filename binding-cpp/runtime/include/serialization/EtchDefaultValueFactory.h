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

#ifndef __ETCHDEFAULTVALUEFACTORY_H__
#define __ETCHDEFAULTVALUEFACTORY_H__

#include "capu/os/Mutex.h"
#include "common/EtchString.h"
#include "common/EtchRuntimeException.h"
#include "common/EtchHashSet.h"
#include "serialization/EtchTypeMap.h"
#include "serialization/EtchClass2TypeMap.h"
#include "serialization/EtchValueFactory.h"
#include "serialization/EtchRuntimeExceptionSerializer.h"
#include "serialization/EtchAuthenticationExceptionSerializer.h"
#include "serialization/EtchHashTableSerializer.h"
#include "serialization/EtchListSerializer.h"
#include "serialization/EtchSetSerializer.h"
#include "serialization/EtchDateSerializer.h"
#include "serialization/EtchValidatorRuntimeException.h"
#include "transport/EtchMessage.h"
#include "util/EtchURL.h"

class EtchDefaultValueFactory : public EtchValueFactory {
private:

  static const EtchString& ETCH_RUNTIME_EXCEPTION_TYPE_NAME();

  static const EtchString& ETCH_LIST_TYPE_NAME();

  static const EtchString& ETCH_MAP_TYPE_NAME();

  static const EtchString& ETCH_SET_TYPE_NAME();

  static const EtchString& ETCH_DATETIME_TYPE_NAME();

  static const EtchString& ETCH_AUTH_EXCEPTION_TYPE_NAME();

  static const EtchString& ETCH_EXCEPTION_MESSAGE_NAME();

  static const EtchString& MSG_FIELD_NAME();

  static const EtchString& MESSAGE_ID_FIELD_NAME();

  static const EtchString& IN_REPLY_TO_FIELD_NAME();

  static const EtchString& RESULT_FIELD_NAME();

  static capu::Mutex Mutex;

  EtchList<EtchValueFactory*> *mMixins;

  EtchTypeMap* mTypes;

  EtchTypeMap* mDynamicTypes;

  EtchClass2TypeMap* mClass2type;

  EtchType *m_mt__Etch_RuntimeException;

  EtchType *m_mt__Etch_List;

  EtchType *m_mt__Etch_Map;

  EtchType *m_mt__Etch_Set;

  EtchType *m_mt__Etch_Datetime;

  EtchType *m_mt__Etch_AuthException;

  EtchType *m_mt__exception;

  EtchLevel mLevel;


protected:

  /**
   * Adds a value factory as a mixin to this one.
   * @param vf
   */
  void addMixin(EtchValueFactory* vf);

public:

  static status_t Init(EtchRuntime* runtime, EtchTypeMap* types, EtchClass2TypeMap* class2type);

  /**
   * The msg field of the standard unchecked exception.
   */
  static const EtchField& _mf_msg();

  /**
   *  The well-known _messageId field.
   */
  static const EtchField& _mf__messageId();

  /**
   * The well-known _inReplyTo field.
   */
  static const EtchField& _mf__inReplyTo();

  /**
   * The well-known result field.
   */
  static const EtchField& _mf_result();

  ///////////////////////
  // ValueFactory impl //
  ///////////////////////

  /**
   * Constructs the DefaultValueFactory.
   * @param uri uri of this transport stack.
   * @param types the type map for this instance of value factory.
   * @param class2type the class to type map for this instance of value factory.
   */
  EtchDefaultValueFactory(EtchString uri, EtchTypeMap *types, EtchClass2TypeMap *class2type);

  //Destructor
  virtual ~EtchDefaultValueFactory();

  /**
   * @return the standard _mt__Etch_RuntimeException.
   */
  const EtchType* get_mt__Etch_RuntimeException();

  /**
   * @return the standard _mt__Etch_List.
   */
  const EtchType* get_mt__Etch_List();

  /**
   * @return the standard _mt__Etch_Map.
   */
  const EtchType* get_mt__Etch_Map();

  /**
   * @return the standard _mt__Etch_Set.
   */
  const EtchType* get_mt__Etch_Set();

  /**
   * @return the standard _mt__Etch_Datetime.
   */
  const EtchType* get_mt__Etch_Datetime();

  /**
   * @return the standard _mt__Etch_AuthException.
   */
  const EtchType* get_mt__Etch_AuthException();

  /**
   * @return the standard m_mt__exception.
   */
  EtchType* get_mt__exception();

  status_t getType(capu::uint32_t id, EtchType *&result);

  status_t getType(EtchString& name, EtchType *&result);

  status_t addType(EtchType* type);

  status_t exportCustomValue(capu::SmartPointer<EtchObject> value, EtchStructValue*& result);

  status_t importCustomValue(EtchStructValue* value, capu::SmartPointer<EtchObject> &result);

  status_t getCustomStructType(const EtchObjectType *c, EtchType *& type);

  virtual status_t getNativeArrayForComponentType(const EtchObjectType *objectType,  capu::SmartPointer<EtchNativeArrayBase> &nativeArray, capu::int32_t length, capu::int32_t dim);

  /////////////////////
  // STRING ENCODING //
  /////////////////////

  EtchString getStringEncoding();

  ////////////////
  // MESSAGE ID //
  ////////////////

  status_t getMessageId(EtchMessage* msg, capu::int64_t& result);

  status_t setMessageId(EtchMessage* msg, capu::int64_t msgid);

  EtchField get_mf__messageId();

  /////////////////
  // IN REPLY TO //
  /////////////////

  status_t getInReplyToMessageId(EtchMessage* msg, capu::int64_t& result);

  status_t setInReplyToMessageId(EtchMessage* msg, capu::int64_t msgid);

  EtchField get_mf__inReplyTo();

  ///// LOCKING /////
  status_t lockDynamicTypes();

  status_t unlockDynamicTypes();

  // LEVEL //
  EtchLevel getLevel();

  EtchLevel setLevel(EtchLevel level);

};

#endif /* ETCHDEFAULTVALUEFACTORY_H */

