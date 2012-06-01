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

#ifndef __ETCHVALIDATOR_H__
#define __ETCHVALIDATOR_H__
#include "capu/util/SmartPointer.h"
#include "common/EtchObject.h"
#include "common/EtchError.h"

class EtchValidator : public EtchObject {
public:

  EtchValidator(const EtchObjectType *type)
  : EtchObject(type) {

  }

  /**
   * Destructor
   */
  virtual ~EtchValidator() {

  }

  /**
   * The maximum number of dimensions for arrays (9).
   */
  const static capu::uint32_t MAX_NDIMS = 9;

  /**
   * The maximum number of validators to cache per type.
   */
  const static capu::uint32_t MAX_CACHED = 4;

  /**
   * @param val for an array type, return a validator for an element of
   * the array (which might be a scalar or another array element).
   * @return ETCH_OK if elementValidator is succesfully get
   *         ETCH_EINVAL otherwise
   */
  virtual status_t getElementValidator(capu::SmartPointer<EtchValidator> &val) = 0;

  /**
   * Checks the value for being valid. If the value is valid
   * by this validator, return true.
   * @param value the value to be validated.
   * @return true if the value is valid by any validator in the
   * chain, or false if all reject it.
   */
  virtual capu::bool_t validate(EtchObject* value) = 0;

  /**
   * @param value
   * @param result the appropriate value given the input value and this
   * validator's sensibility about what it should be really (e.g.,
   * input Byte but output Long.
   * @return TRUE if the value is validated
   *         FALSE otherwise
   */
  virtual status_t validateValue(EtchObject* value, EtchObject*& result) = 0;

};

#endif /* ETCHVALIDATOR_H */

