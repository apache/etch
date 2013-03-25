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

#ifndef __ETCHQUEUEDPOOL_H__
#define __ETCHQUEUEDPOOL_H__

#include "support/EtchPool.h"
#include "support/EtchPoolRunnable.h"

namespace capu {
  class ThreadPool;
};

/**
 * A implementation of the free pool.
 */
class EtchQueuedPool : public EtchPool
{
public:

  /**
   * EtchObjectType for EtchQueuedPool.
   */
  static const EtchObjectType* TYPE();

  /**
   * Creats a new instance of the EtchQueuedPool-Class.
   * @param size of the pool
   */
  EtchQueuedPool(capu::int32_t size = 50);

  /**
   * Destructure.
   */
  virtual ~EtchQueuedPool();

  /**
   * Closes the pool. This just marks the pool as being closed, it doesn't
   * actually do anything to the currently running thread. But no more
   * threads are allowed to start.
   * @return error if somthings goes wrong
   */
  status_t close();

  /**
   * Joins each of the threads in this pool until there
   * are none left. The pool will be closed first.
   * @return error if somthings goes wrong
   */
  status_t join();

  /**
   * @see EtchPool
   */
  status_t add(capu::SmartPointer<EtchPoolRunnable> runnable);

private:
  capu::uint32_t mSizeMax;
  capu::bool_t mIsOpen;
  capu::ThreadPool* mPool;
};

#endif /*__ETCHQUEUEDPOOL_H__ */

