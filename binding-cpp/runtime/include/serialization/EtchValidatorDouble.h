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

#ifndef __ETCHVALIDATORDOUBLE_H__
#define __ETCHVALIDATORDOUBLE_H__
#include "serialization/EtchTypeValidator.h"
#include "common/EtchFloat.h"
#include "common/EtchDouble.h"
#include "serialization/EtchTypeCodes.h"
#include "capu/os/NumericLimits.h"
#include "common/EtchNativeArray.h"

class EtchValidatorDouble : public EtchTypeValidator {
public:

  virtual ~EtchValidatorDouble();

  /**
   * @see EtchValidator
   */
  virtual status_t getElementValidator(capu::SmartPointer<EtchValidator> &val);

  /**
   * @see EtchValidator
   */
  virtual capu::bool_t validate(EtchObject* value);

  /**
   * @see EtchValidator
   */
  virtual status_t validateValue(EtchObject* value, EtchObject*& result);

  /**
   * @see EtchValidator
   */
  static status_t Get(capu::uint32_t ndim, capu::SmartPointer<EtchValidator> &val);

  static const EtchObjectType TYPE;

protected:
  EtchValidatorDouble(capu::uint32_t ndim);

private:
  static capu::SmartPointer<EtchValidator> mValidator[MAX_CACHED];

};


#endif /* __ETCHVALIDATORDOUBLE_H__ */

