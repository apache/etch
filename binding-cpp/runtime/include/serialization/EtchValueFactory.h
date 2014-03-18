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


#ifndef __ETCHVALUEFACTORY_H__
#define __ETCHVALUEFACTORY_H__

#include "common/EtchString.h"
#include "serialization/EtchLevel.h"
#include "serialization/EtchField.h"

class EtchStructValue;
class EtchMessage;
class EtchType;
class EtchNativeArrayBase;

/**
 * Interface which defines the value factory which helps
 * the idl compiler serialize and deserialize messages,
 * convert values, etc.
 */


class EtchValueFactory : public EtchObject {
public:

  /**
   * EtchObjectType for EtchValueFactory.
   */
  static const EtchObjectType* TYPE();

  /**
   * Default Constructor
   */
  EtchValueFactory();

  /**
   * Copy Constructor
   */
  EtchValueFactory(const EtchValueFactory& other);

  /**
   * Destructor
   */
  virtual ~EtchValueFactory() {

  }

  /**
   * Translates a type id into the appropriate Type object. If the type does
   * not exist, and if dynamic typing is enabled, adds it to the dynamic types.
   * @param id a type id.
   * @param result a output buffer.
   */
  virtual status_t getType(capu::uint32_t id, EtchType *&result) = 0;

  /**
   * Translates a type name into the appropriate Type object. If the type does
   * not exist, and if dynamic typing is enabled, adds it to the dynamic types.
   * @param name a type name.
   * @param result a output buffer.
   */
  virtual status_t getType(EtchString &name, EtchType *&result) = 0;

  /**
   * Adds the type if it doesn't already exist. Use this to dynamically add
   * types to a ValueFactory. The type is per instance of the ValueFactory,
   * not global. Not available if dynamic typing is locked.
   * @param type
   */
  virtual status_t addType(EtchType *type) = 0;

  /**
   * Locks the dynamic typing so that no new types may be created by addType
   * or getType.
   */
  virtual status_t lockDynamicTypes() = 0;

  /**
   * Unlocks the dynamic typing so that new types may be created by addType
   * or getType.
   */
  virtual status_t unlockDynamicTypes() = 0;


  ////////////////
  // MESSAGE ID //
  ////////////////

  /**
   * @param msg the message whose well-known message-id field is to be
   * returned.
   * @param the value of the well-known message-id field. This is a
   * unique identifier for this message on a particular transport
   * during a particular session. If there is no well-known message-id
   * field defined, or if the message-id field has not been set, then
   * return null.
   * @return ETCH_OK if successfully getted
   *         ETCH_EINVAL if msg is null
   *         ETCH_ERROR if it is not found
   */
  virtual status_t getMessageId(EtchMessage *msg, capu::int64_t &result) = 0;

  /**
   * Sets the value of the well-known message-id field. This is a
   * unique identifier for this message on a particular transport
   * during a particular session. If there is no well-known message-id
   * field defined then nothing is done. If msgid is null, then the
   * field is cleared.
   * @param msg the message whose well-known message-id field is to
   * be set.
   * @param msgid the value of the well-known message-id field.
   * @return ETCH_EINVAL if msg is null
   *         ETCH_OK otherwise
   */
  virtual status_t setMessageId(EtchMessage *msg, capu::int64_t msgid) = 0;

  /**
   * @return well-known message field for message id.
   */
  virtual EtchField get_mf__messageId() = 0;

  /////////////////
  // IN REPLY TO //
  /////////////////

  /**
   * @param msg the message whose well-known in-reply-to field is to
   * be returned.
   * @param result output buffer
   * @return ETCH_OK if successfully getted
   *         ETCH_EINVAL if msg is null
   *         ETCH_ERROR if it is not found
   */
  virtual status_t getInReplyToMessageId(EtchMessage *msg, capu::int64_t &result) = 0;

  /**
   * @param msg the message whose well-known in-reply-to field is to
   * be set.
   * @param msgid the value of the well-known in-reply-to field. If
   * there is no well-known in-reply-to field defined then nothing
   * is done. If msgid is null, then the field is cleared.
   * @return ETCH_EINVAL if msg is null
   *         ETCH_OK otherwise
   */
  virtual status_t setInReplyToMessageId(EtchMessage *msg, capu::int64_t msgid) = 0;

  /**
   * @return well-known message field for in reply to.
   */
  virtual EtchField get_mf__inReplyTo() = 0;

  //////////////////////
  // VALUE CONVERSION //
  //////////////////////

  /**
   * Converts a value to a struct value representation to be exported
   * to a tagged data output.
   * @param value a custom type defined by a service, or a well-known
   * @param result buffer to store result which contains a struct value representing the value.
   * standard type (e.g., date).
   * @return ETCH_ERROR if the type cannot be exported.
   *         ETCH_OK otherwise
   */
  virtual status_t exportCustomValue(capu::SmartPointer<EtchObject> value, EtchStructValue *&result) = 0;

  /**
   * Converts a struct value imported from a tagged data input to
   * a normal type.
   * @param value a struct value representation of a custom type, or a well known standard type.
   * @param result buffer to store result which contains a custom type, or a well known standard type.
   * @return ETCH_ERROR if the type cannot be imported.
   *         ETCH_OK otherwise
   */
  virtual status_t importCustomValue(EtchStructValue* value, capu::SmartPointer<EtchObject> &result) = 0;

  /**
   * @param c the class of a custom value.
   * @param type the class of a custom value.
   * @return ETCH_OK if the type is found correctly.
   *         ETCH_EINVAL otherwise
   */
  virtual status_t getCustomStructType(const EtchObjectType *c, EtchType *&type) = 0;

  /**
   * Returns the current string encoding.
   * @return string encoding
   */
  virtual EtchString getStringEncoding();

  /**
   * @return well-known message type for exception thrown by one-way
   * message.
   */
  virtual EtchType* get_mt__exception() = 0;

  /**
   * @return the validation level of field StructValue.put and TaggedDataOutput
   */
  virtual EtchLevel getLevel() = 0;

  /**
   * Sets the validation level of field StructValue.put and TaggedDataOutput
   * @param level
   * @return the old value
   */
  virtual EtchLevel setLevel(EtchLevel level) = 0;

  /**
   * @param objectType the component's objectType of the EtchNativeArray
   * @param nativeArray the created native array for the given component type
   * @param length the length of the array
   * @param dim the dimension of the array
   * @return ETCH_OK if the type is found correctly.
   *         ETCH_EUNIMPL if no type information for arrays has been generated
   *         ETCH_ERROR if the array for the given object type cannot be generated
   */
  virtual status_t getNativeArrayForComponentType(const EtchObjectType *objectType,  capu::SmartPointer<EtchNativeArrayBase> &nativeArray, capu::int32_t length, capu::int32_t dim) = 0;

};

#endif /* ETCHVALUEFACTORY_H */

