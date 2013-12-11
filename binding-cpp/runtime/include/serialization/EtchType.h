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

#ifndef __ETCHTYPE_H__
#define __ETCHTYPE_H__

#include "common/EtchString.h"
#include "common/EtchHashTable.h"
#include "serialization/EtchDirection.h"
#include "serialization/EtchAsyncMode.h"
#include "serialization/EtchField.h"
#include "serialization/EtchValidator.h"
#include "serialization/EtchImportExportHelper.h"
#include "serialization/EtchFieldMap.h"
#include "serialization/EtchComboValidator.h"

class EtchType : public EtchObject {
public:

  /**
   * EtchObjectType for EtchNativeArray.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructor
   */
  EtchType();

  /**
   * Constructor
   * @param name name of the type
   */
  EtchType(EtchString name);

  /**
   * Constructor
   * @param id identifier of the type
   * @param name name of the type
   */
  EtchType(capu::uint32_t id, EtchString name);

  /**
   * Copy Constructor
   */
  EtchType(const EtchType& other);

  /**
   * Default Destructor
   */
  virtual ~EtchType();

  /**
   * @return the result message type of this message type.
   */
  EtchType* getResult();

  /**
   * Sets the result message type of this message type.
   * @param rType
   */
  void setResult(EtchType *rType);

  /**
   * Adds a field to the set of fields.
   *
   * @param field a field to add.
   * @return ETCH_EINVAL if it is locked
   *         ETCH_ERROR  if there is an error
   *         ETCH_OK otherwise
   */
  status_t addField(const EtchField &field);

  /**
   * Translates a field id into the appropriate Field object.
   * @param id a field id.
   * @param field a buffer which will store the returned element.
   * @return    ETCH_OK if get is successful performed
   *            ETCH_EINVAL if value is null
   *            ETCH_ERANGE if there is no existing pair with specified key
   */
  status_t getField(capu::uint32_t id, EtchField *field);

  /**
   * Translates a field name into the appropriate Field
   * object.
   * @param name a field name.
   * @param field a buffer which will store the returned element.
   * @return ETCH_OK if get is successful performed
   *         ETCH_EINVAL if value is null
   *         ETCH_ERANGE if there is no existing pair with specified key
   */
  status_t getField(const EtchString &name, EtchField *field);

  /**
   * @return the time in milliseconds to wait for this response message.
   */
  capu::uint32_t getTimeout();

  /**
   * @param timeout the time in milliseconds to wait for this response message.
   */
  void setTimeout(capu::uint32_t timeout);

  /**
   * @return the super type of this type. If struct A extends B, then
   * B is the super type of A.
   */
  EtchType* getSuperType();

  /**
   * Sets the super type of this type. If struct A extends B, then
   * B is the super type of A.
   * @param superType
   */
  void setSuperType(EtchType* superType);

  /**
   * @return the AsyncMode for this type.
   */
  EtchAsyncMode getAsyncMode();

  /**
   * Sets the AsyncMode for this type.
   * @param mode
   */
  void setAsyncMode(EtchAsyncMode mode);

  /**
   * @return the message direction.
   */
  EtchDirection getDirection();

  /**
   * Sets the message direction.
   * @param direction the direction to set.
   */
  void setDirection(EtchDirection direction);

  /**
   *
   * @param key as field
   * @param result the validator for this key of this type.
   * @return ETCH_OK if get is successful performed
   *         ETCH_EINVAL if value is null
   *         ETCH_ENOT_EXIST if there is no existing pair with specified key
   */
  status_t getValidator(const EtchField &key, capu::SmartPointer<EtchValidator> &result);

  /**
   * Adds the validator to the chain for this key.
   * @param key as field
   * @param validator for the given field
   * @return ETCH_OK if remove is successful
   *         ETCH_EINVAL if value is NULL or locked
   */
  status_t putValidator(const EtchField key, capu::SmartPointer<EtchValidator> validator);

  /**
   * Removes the validator chain for this key.
   * @param key as field
   * @return ETCH_OK if remove is successful
   *         ETCH_EINVAL if value_old is null
   *         ETCH_ERANGE if the pair with specified key does not exist in hash table
   */
  status_t clearValidator(EtchField key);

  /**
   * Locks the fields for this type
   */
  void lock();

  //overridden
  capu::bool_t equals(const EtchObject *other) const;

  /**
   * @return the associated import / export helper.
   */
  EtchImportExportHelper* getImportExportHelper();

  /**
   * Sets the associated import / export helper.
   * @param helper
   */
  void setImportExportHelper(EtchImportExportHelper* helper);


  /**
   * @return the associated StubHelper.
   */
  EtchObject* getStubHelper();

  /**
   * Sets the associated StubHelper
   * @param StubHelper as EtchObject
   */
  void setStubHelper(EtchObject* stubHelper);

  /**
   * @return the field with the value of this response.
   */
  EtchField getResponseField();

  /**
   * @param responseField the field with the value of this response.
   */
  void setResponseField(EtchField responseField);

  /**
   * returns the id of type
   * @return id of type
   */
  capu::uint32_t getId();

  /**
   * returns the name of the type
   * @return name of type
   */
  EtchString getName();

  /**
   * (overridden)
   * hash code
   * @return hash
   */
  capu::uint32_t getHashCode() const;

  /**
   * returns component type
   * @return component type
   */
  const EtchObjectType* getComponentType();

  /**
   * Sets component type
   */
  void setComponentType(const EtchObjectType* type);

private:
  EtchFieldMap mFieldMap;
  EtchHashTable<EtchField, capu::SmartPointer<EtchValidator> > mValidatorMap;
  EtchInt32 mId;
  capu::uint32_t mTimeout;
  EtchString mName;
  EtchType* mSuperType;
  EtchType* mResultType;
  EtchDirection mDirection;
  EtchAsyncMode mAsyncMode;
  capu::bool_t mLocked;
  EtchField mResponseField;
  const EtchObjectType* mComponentType;
  EtchImportExportHelper* mHelper;
  EtchObject* mStubHelper;
};

#endif /* ETCHTYPE_H */

