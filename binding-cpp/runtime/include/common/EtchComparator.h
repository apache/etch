/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

#ifndef __ETCHCOMPARATOR_H__
#define __ETCHCOMPARATOR_H__
#include "common/EtchObject.h"

template <class T>
class EtchComparator {
public:

  inline capu::bool_t operator() (const EtchObject &first, const EtchObject &second) const {
    return first.equals(&second);
  }
};

template <class T>
class EtchComparator <T*> {
public:

  inline capu::bool_t operator() (const EtchObject* first, const EtchObject* second) const {
    return first->equals(second);
  }
};

template <class T>
class EtchComparator <capu::SmartPointer<T> > {
public:

  inline capu::bool_t operator() (const capu::SmartPointer<EtchObject>& first, const capu::SmartPointer<EtchObject>& second) const {
    return first->equals(second.get());
  }
};

#endif
