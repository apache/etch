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

#ifndef __ETCHCOMBOVALIDATOR_H__
#define __ETCHCOMBOVALIDATOR_H__

#include "serialization/EtchValidator.h"

class EtchComboValidator : public EtchValidator {
public:

   /**
   * EtchObjectType for EtchBool.
   */
  static const EtchObjectType* TYPE();

  /**
   * Constructs the ComboValidator.
   *
   * @param a
   * @param b
   */
  EtchComboValidator(capu::SmartPointer<EtchValidator> a, capu::SmartPointer<EtchValidator> b);

  /**
   * Destructor
   */
  virtual ~EtchComboValidator();

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
  status_t getElementValidator(capu::SmartPointer<EtchValidator> &val);

  /**
   * return First child validator
   * @return first child validator
   */
  capu::SmartPointer<EtchValidator> getFirst();

  /**
   * return Second child validator
   * @return second child validator
   */
  capu::SmartPointer<EtchValidator> getSecond();

private:
  capu::SmartPointer<EtchValidator> mFirst;
  capu::SmartPointer<EtchValidator> mSecond;

};


#endif /* ETCHCOMBOVALIDATOR_H */

