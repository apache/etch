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

#ifndef __ETCHASYNCRESULTNONE_H__
#define __ETCHASYNCRESULTNONE_H__

#include "capu/os/Mutex.h"
#include "capu/os/CondVar.h"
#include "support/EtchMailbox.h"
#include "common/EtchException.h"

class EtchRuntime;

/**
 * Base class of AsyncResultBase.
 */
class EtchAsyncResultNone : public EtchMailbox::EtchNotify {
public:

  /**
   * Constructor
   * @param runtime
   */
  explicit EtchAsyncResultNone(EtchRuntime* runtime = NULL, capu::SmartPointer<EtchMailbox> mailbox = NULL);

  /**
   * Destructor
   */
  virtual ~EtchAsyncResultNone();

  /**
   * Check if an exception occured
   */
  virtual capu::bool_t hasException();

  /**
   * Returns the exception
   */
  virtual capu::SmartPointer<EtchException> getException();

  /**
   * Sets the exception
   * @param exception
   */
  void setException(capu::SmartPointer<EtchException> exception);

  /**
   * @see EtchNotify
   */
  virtual status_t mailboxStatus(EtchMailbox* mb, EtchObject* state, capu::bool_t closed) {return ETCH_OK;};

protected:
  /**
   * Sets the mailbox status to true and signals waiting threads
   */
  void setMailboxStatus();

  EtchRuntime* mRuntime;
  capu::SmartPointer<EtchMailbox> mMailbox;
  capu::Mutex mMutex;
  capu::CondVar mCond;
  capu::bool_t mHasMailboxStatus;
  capu::bool_t mHasException;
  capu::SmartPointer<EtchException> mException;
};

typedef capu::SmartPointer<EtchAsyncResultNone> EtchAsyncResultNonePtr;

#endif
