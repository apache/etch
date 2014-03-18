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

#ifndef __ETCHSTRUCTVALUE_H__
#define __ETCHSTRUCTVALUE_H__

#include "capu/util/SmartPointer.h"
#include "common/EtchHashTable.h"
#include "common/EtchObject.h"
#include "common/EtchConfig.h"
#include "serialization/EtchType.h"
#include "serialization/EtchLevel.h"

class EtchValueFactory;

/**
 * A typed map of key/value pairs, where the type is a Type,
 * each key is a Field, and each value is of arbitrary type
 * chosen from the basic cpp types boolean, byte, short, int, long,
 * float, double, String, an array of those, the extended types
 * ArrayValue and StructValue, and specific types supported by
 * ValueFactory.
 *
 * StructValue is not protected against concurrent access.
 */

class EtchStructValue : public EtchObject {
public:

  /**
   * EtchObjectType for EtchStructValue.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructs the StructValue with default length.
   *
   * @param type the type of the struct.
   * @param vf the value factory.
   */
  EtchStructValue(EtchType* type, EtchValueFactory* vf);

  /**
   * Constructs the StructValue.
   *
   * @param type the type of the struct.
   * @param vf the value factory.
   * @param length the number of name / value pairs expected. 0 means use
   * default.
   */
  EtchStructValue(EtchType* type, EtchValueFactory* vf, capu::uint32_t length);

  /**
   * Copy Constructor
   */
  EtchStructValue(const EtchStructValue& other);

  /**
   * Default Destructor
   */
  virtual ~EtchStructValue();

  /**
   * @return EtchType
   */
  EtchType* getType();

  /**
   * Puts an object to EtchStructValue by validating it
   * @param field  field of custom type
   * @param object value of field that will be added. If object is set to NULL, the field is removed.
   *
   * @return ETCH_OK if successfully added
   *         ETCH_EINVAL if the level of type is not LEVEL_NONE and the object is NULL
   *         ETCH_ERROR  if the level of type is not LEVEL_NONE and the object is not validated and is not NULL
   */
  status_t put(const EtchField &field, capu::SmartPointer<EtchObject> object);

  /**
   * Get value associated with key in the EtchStructValue.
   * @param key       Key
   * @param value     buffer which will be used to return the found element
   *
   * @return ETCH_OK if get is successful performed
   *         ETCH_EINVAL if value is null
   *         ETCH_ENOT_EXIST if there is no existing pair with specified key
   *
   */
  status_t get(const EtchField &key, capu::SmartPointer<EtchObject> *value);

  /**
   * Remove value associated with key in the EtchStructValue.
   *
   * @param key               Key value
   * @param value_old         buffer which will be used to store value of removed element
   *
   * @return ETCH_OK if remove is successful
   *         ETCH_EINVAL if value_old is null
   *         ETCH_ERANGE if the pair with specified key does not exist in hash table
   *
   */
  status_t remove(const EtchField &key, capu::SmartPointer<EtchObject> *value_old);

  /**
   * Compares the type of this struct to another type.
   * @param otherType the type to compare this type to.
   * @return true if this struct is of the specified type.
   */
  capu::bool_t isType(EtchType *otherType);

  /**
   * Returns count of the EtchStructValue.
   * @return number of element in hash table
   */
  capu::uint32_t count();

  /**
   * Clear all key and values of the EtchStructValue.
   *
   * @return ETCH_OK if all elements in list have been deleted
   */
  status_t clear();

  /**
   * @return EtchLevel
   */
  EtchLevel getLevel();

  typedef EtchHashTable<EtchField, capu::SmartPointer<EtchObject> >::ConstIterator Iterator;
  typedef EtchHashTable<EtchField, capu::SmartPointer<EtchObject> >::HashTableEntry HashTableEntry;

  /**
   * @return an iterator which is pointing the beginning of collection
   */
  Iterator begin() const;

  /**
   * @return true if the struct value is empty, otherwise false
   */
  capu::bool_t isEmpty();

private:

  EtchType* mType;
  EtchLevel mLevel;
  EtchHashTable<EtchField, capu::SmartPointer<EtchObject> > mTable;
  /**
   * A bogus serial version uid.
   */
  static const capu::int64_t serialVersionUID = 1L;

  //Default size of mTable
  static const capu::int32_t DEFAULT_SIZE = ETCH_DEFAULT_STRUCTVALUE_HASH_BIT_SIZE;

};


#endif /* ETCHSTRUCTVALUE_H */
