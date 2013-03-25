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

#include "capu/os/Mutex.h"
#include "capu/container/List.h"
#include "support/EtchPool.h"
#include "support/EtchPoolRunnable.h"

// forward declarations
namespace capu {
  class Thread;
}

class EtchFreePoolRunnable;
/**
 * A implementation of the free pool.
 */
class EtchFreePool : public EtchPool
{
public:

  /**
   * Internal friend class
   */
  friend class EtchFreePoolRunnable;

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
  status_t close();

  /**
   * Return the current pool size
   */
  capu::int32_t getSize();

  /**
   * Joins each of the threads in this pool until there
   * are none left. The pool will be closed first.
   * @return error if somthings goes wrong
   */
  status_t join();

  /**
   * @see EtchPool
   */
  virtual status_t add(capu::SmartPointer<EtchPoolRunnable> runnable);

private:
  capu::int32_t mSize;
  capu::int32_t mSizeMax;
  capu::bool_t mIsOpen;
  capu::Mutex mMutex;
  capu::Thread** mThreads;
  EtchFreePoolRunnable** mRunnables;

  /**
   * Checks thread list and clean up

   */
  status_t check();
};

#endif /* __ETCHFREEPOOL_H__ */

