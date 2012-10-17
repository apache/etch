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

#ifndef __THREADPOOL_H__
#define __THREADPOOL_H__

#include "capu/Config.h"
#include "capu/container/Array.h"
#include "capu/container/Queue.h"
#include "capu/os/CondVar.h"
#include "capu/os/Mutex.h"
#include "capu/os/Thread.h"
#include "capu/util/Runnable.h"
#include "capu/util/SmartPointer.h"

namespace capu
{

  class ThreadPool{
    public:
      /**
      * creats a new Threadpool instance
      * @param threads amount of initial threads
      */
      ThreadPool();

      /**
      * creats a new Threadpool instance
      * @param size amounts of threads
      */
      ThreadPool(uint32_t size);

      /**
      * destructor
      */
      virtual ~ThreadPool();

      /**
      * Adds a runnable to the threadpool
      * @param runnable The runnable which should be executed by the threadpool
      */
      status_t add(SmartPointer<Runnable> runnable);

      /**
      * Waits until every thread has been terminated
      */
      status_t join();

      /**
      * Returns the number of threads used by the threadPool
      * @param runnable The runnable which should be executed by the threadpool
      */
      int32_t getSize();

    private:
      class PoolRunnable : public Runnable {
      public:
        PoolRunnable() {
          mPool = NULL;
        }

        PoolRunnable(ThreadPool* pool) {
          mPool = pool;
        }

        void run() {
          if (mPool == NULL) {
            return;
          }
          while (1) {
            mPool->mMutex.lock();
            while(mPool->mRunnableQueue->isEmpty() && !mPool->isClosed()) {
              if (mPool->mCloseRequested) {
                //if Queue is empty and close was requested
                mPool->mClosed = true;
                break;
              }

              mPool->mCV.wait(&mPool->mMutex);
            }
            if (mPool->isClosed()) {
              mPool->mMutex.unlock();
              break;
            }
            capu::SmartPointer<capu::Runnable> r = NULL;
            status_t result = mPool->mRunnableQueue->next(&r);

            mPool->mMutex.unlock();
            if (result == CAPU_OK) {
              if (r.get() != NULL) {
                r->run();
              }
            }
          }
        }
      private:
        ThreadPool *mPool;
      };

      class PoolWorker {
      public:
        PoolWorker()
        : mPool(NULL), mPoolRunnable(NULL), mThread(NULL) {
        }

        PoolWorker(ThreadPool *pool, int32_t id)
          : mPool(pool)
        {
          mPoolRunnable = new PoolRunnable(mPool);
          mThread = new Thread(mPoolRunnable);

        }

        virtual ~PoolWorker() {
          delete mPoolRunnable;
          delete mThread;
        }

        status_t startThread() {
          return mThread->start();
        }

        status_t joinThread() {
          return mThread->join();
        }

      private:
        int32_t mId;
        ThreadPool *mPool;
        PoolRunnable *mPoolRunnable;
        Thread *mThread;

      };

      status_t init();

      uint32_t mSize;
      Queue<SmartPointer<Runnable> > *mRunnableQueue;
      PoolWorker **mThreadArray;
      CondVar mCV;
      Mutex mMutex;
      bool_t mClosed;
      bool_t mCloseRequested;

      bool_t isClosed();


  };


}

#endif /* Thread_H */
