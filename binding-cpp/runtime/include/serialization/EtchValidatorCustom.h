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

#ifndef __ETCHVALIDATORCUSTOM_H__
#define __ETCHVALIDATORCUSTOM_H__

#include "common/EtchByte.h"
#include "common/EtchShort.h"
#include "common/EtchInt32.h"
#include "common/EtchLong.h"
#include "common/EtchNativeArray.h"
#include "common/EtchHashTable.h"

#include "serialization/EtchTypeValidator.h"
#include "serialization/EtchTypeCodes.h"

class EtchValidatorCustomKey : public EtchObject {
public:

  /**
   * EtchObjectType for EtchValidatorCustomKey.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructs the key
   */
  EtchValidatorCustomKey();

  /**
   * Copy Constructor
   */
  EtchValidatorCustomKey(const EtchValidatorCustomKey& other);

  /**
   * Destructor
   */
  virtual ~EtchValidatorCustomKey();

  /**
   * Constructs the key
   * @param type EtchObject type
   * @param dim dimensions
   * @param sub is subclass ok
   */
  EtchValidatorCustomKey(const EtchObjectType* type, capu::int32_t dim, capu::bool_t sub);

  /**
   * Returns hash code
   */
  capu::uint32_t getHashCode() const;

  /**
   * @return true if two object is equal
   *         false otherwise
   */
  capu::bool_t equals(const EtchObject * other) const;

  const EtchObjectType *mType;
  capu::int32_t mDims;
  capu::bool_t mSubclassOk;
};


class EtchValidatorCustom : public EtchTypeValidator {
public:

  /**
   * EtchObjectType for EtchValidatorCustom.
   */
  static const EtchObjectType* TYPE();

  EtchValidatorCustom(const EtchValidatorCustom& other);

  /**
   * Destructor
   */
  virtual ~EtchValidatorCustom();

  /**
   * @see EtchValidator
   */
  virtual status_t getElementValidator(capu::SmartPointer<EtchValidator> &val);

  /**
   * @see EtchValidator
   */
  virtual capu::bool_t validate(capu::SmartPointer<EtchObject> value);

  /**
   * @see EtchValidator
   */
  virtual status_t validateValue(capu::SmartPointer<EtchObject> value, capu::SmartPointer<EtchObject>& result);

  /**
   * @see EtchValidator
   */
  static status_t Get(EtchRuntime* runtime, capu::uint32_t ndim, const EtchObjectType *type, capu::bool_t sub, capu::SmartPointer<EtchValidator> &val);

protected:

  /**
   * Constructs the validator instance
   * @param type EtchObject type
   * @param dim dimensions
   * @param sub is subclass ok
   */
  EtchValidatorCustom(EtchRuntime* runtime, const EtchObjectType *type, capu::uint32_t ndim, capu::bool_t sub);

private:
  EtchRuntime* mRuntime;
  static EtchHashTable<EtchValidatorCustomKey, capu::SmartPointer<EtchValidator> >& Validators(EtchRuntime* runtime);
};

#endif /* ETCHVALIDATORCUSTOM_H */
