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

#ifndef __ETCHVALIDATORRUNTIMEEXCEPTION_H__
#define __ETCHVALIDATORRUNTIMEEXCEPTION_H__
#include "serialization/EtchValidator.h"
#include "common/EtchRuntimeException.h"

class EtchRuntime;

class EtchValidatorRuntimeException : public EtchValidator {
public:

  /**
   * Copy Constructor
   */
  EtchValidatorRuntimeException(const EtchValidatorRuntimeException& other);

  /**
   * Destructor
   */
  virtual ~EtchValidatorRuntimeException();

  /**
   * @see EtchValidator
   */
  status_t getElementValidator(capu::SmartPointer<EtchValidator> &val);

  /**
   * @see EtchValidator
   */
  capu::bool_t validate(capu::SmartPointer<EtchObject> value);

  /**
   * @see EtchValidator
   */
  status_t validateValue(capu::SmartPointer<EtchObject> value, capu::SmartPointer<EtchObject>& result);

  /**
   * @see EtchValidator
   */
  static status_t Get(EtchRuntime* runtime, capu::SmartPointer<EtchValidator> &val);

  /**
   * EtchObjectType for EtchValidatorRuntimeException.
   */
  static const EtchObjectType* TYPE();

protected:
  EtchValidatorRuntimeException(EtchRuntime* runtime);

private:
  EtchRuntime* mRuntime;
  static capu::SmartPointer<EtchValidator>* Validators(EtchRuntime* runtime);
};

#endif

