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

#include "util/EtchIdGenerator.h"
#include "capu/os/Mutex.h"

//TODO: Replace this via AtomicOperations

EtchIdGenerator::~EtchIdGenerator() {
}

EtchIdGenerator::EtchIdGenerator()
: mNextId(1), mStride(1) {
}

EtchIdGenerator::EtchIdGenerator(capu::uint64_t nextId)
: mNextId(nextId), mStride(1) {
}

EtchIdGenerator::EtchIdGenerator(capu::uint64_t nextId, capu::uint32_t stride)
: mNextId(nextId), mStride(stride) {
}

capu::uint64_t EtchIdGenerator::next() {
  mMutex.lock();

  capu::uint64_t id = mNextId;
  mNextId += mStride;

  mMutex.unlock();
  return id;
}
