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

#ifndef __ETCHFREEPOOL_H__
#define __ETCHFREEPOOL_H__

#include "capu/container/List.h"
#include "support/EtchPool.h"
#include "support/EtchPoolRunnable.h"

// forward declarations
namespace capu {
  class Thread;
}

/**
 * A implementation of the free pool.
 */
class EtchFreePool : public EtchPool
{
public:

  /**
   * EtchObjectType for EtchFreePool.
   */
  static const EtchObjectType* TYPE();

  /**
   * Creats a new instance of the EtchFreePool-Class.
   * @param size of the pool
   */
  EtchFreePool(capu::int32_t size = 50);

  /**
   * Destructure.
   */
  virtual ~EtchFreePool();

  /**
   * Closes the pool. This just marks the pool as being closed, it doesn't
   * actually do anything to the currently running thread. But no more
   * threads are allowed to start.
   * @return error if somthings goes wrong
   */
  capu::status_t close();

  /**
   * Joins each of the threads in this pool until there
   * are none left. The pool will be closed first.
   * @return error if somthings goes wrong
   */
  capu::status_t join();

  /**
   * @see EtchPool
   */
  virtual capu::status_t add(capu::SmartPointer<EtchPoolRunnable> runnable);

private:
  capu::int32_t mMaxSize;
  capu::bool_t mIsOpen;
  capu::List<capu::Thread*> mThreads;
};

#endif /* __ETCHFREEPOOL_H__ */
