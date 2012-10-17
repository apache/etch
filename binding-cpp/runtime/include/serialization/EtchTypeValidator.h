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

#ifndef __ETCHTYPEVALIDATOR_H__
#define __ETCHTYPEVALIDATOR_H__
#include "serialization/EtchValidator.h"
#include "common/EtchError.h"

class EtchTypeValidator : public EtchValidator {
public:

  /**
   * Constructs the TypeValidator.
   *
   * @param type the type of the derived class
   * @param scalarClass class to use if nDims == 0
   * @param arrayClass class to use if nDims > 0
   * @param nDims the number of dimensions. 0 for a scalar
   * @param subClass default is false
   */
  EtchTypeValidator(const EtchObjectType * type, const EtchObjectType* scalarClass, const EtchObjectType* arrayClass, capu::uint32_t nDims, capu::bool_t subclass = false);

  /**
   * Copy Constructor
   */
  EtchTypeValidator(const EtchTypeValidator& other);

  /**
   *  Default Destructor
   */
  virtual ~EtchTypeValidator();

  /**
   * Checks the number of dimensions for standard validators.
   * @param nDims
   * @returns ETCH_ERROR if nDims < 0 || mNDims > MAX_NDIMS
   *          ETCH_OK otherwise
   */
  status_t checkDimension(capu::uint32_t ndim);

  /**
   * @return the expected number of dimensions.
   */
  virtual capu::uint32_t getNDims();

  /**
   * Returns expected Type
   * @return expected type
   */
  virtual const EtchObjectType* getExpectedType();

protected:

  capu::uint32_t mNDims;
  const EtchObjectType* mExpectedType;
  const EtchObjectType* mArrayComponentType;
  capu::bool_t mSubclass;

};


#endif /* ETCHTYPEVALIDATOR_H */

