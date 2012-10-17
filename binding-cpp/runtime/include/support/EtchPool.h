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
#ifndef __ETCHPOOL_H__
#define __ETCHPOOL_H__

#include "common/EtchObject.h"
#include "support/EtchPoolRunnable.h"

/**
  * Interface to a pool of runnables.
  */
class EtchPool
  : public EtchObject {
public:

  /**
   * EtchObjectType for EtchPool.
   */
  static const EtchObjectType* TYPE();

  /**
   * Creates a new instance of the EtchPool-Class.
   */
  EtchPool() {
    addObjectType(TYPE());
  }

  /**
   * Creates a copy of the EtchPool-Class.
   */
  EtchPool(const EtchObject& other)
   : EtchObject(other) {
  }
  /**
   * Destructure.
   */
  virtual ~EtchPool() {}

  /**
   * scheduling the runnable to run
   * @param runnable the thing to run.
   * @return error if there is a problem scheduling the
   *         runnable to run
   */
  virtual status_t add(capu::SmartPointer<EtchPoolRunnable> runnable) = 0;

};
#endif /* __ETCHPOOL_H__ */
