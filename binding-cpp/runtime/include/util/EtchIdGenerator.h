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

#ifndef __ETCHIDGENERATOR_H__
#define __ETCHIDGENERATOR_H__

#include "capu/os/Mutex.h"
#include "common/EtchConfig.h"

/**
 * Synchronously generates long id numbers.
 */
class EtchIdGenerator {
public:

  /**
   * Constructs the IdGenerator with the default starting point of 1
   * and the default stride of 1.
   */
  EtchIdGenerator();

  /**
   * Constructs the IdGenerator with the specified starting point
   * and the default stride of 1.
   *
   * @param nextId
   */
  EtchIdGenerator(capu::uint64_t nextId);

  /**
   * Constructs the IdGenerator with the specified starting point
   * and the specified stride;
   * @param nextId
   * @param stride
   */
  EtchIdGenerator(capu::uint64_t nextId, capu::uint32_t stride);
  /**
   * Destructor
   */
  virtual ~EtchIdGenerator();

  /**
   * @return the next id in sequence.
   */
  capu::uint64_t next();

private:

  capu::uint64_t mNextId;
  const capu::uint32_t mStride;
  capu::Mutex mMutex;
};

#endif	/* ETCHIDGENERATOR_H */
