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

#ifndef __ETCHMONITOR_H__
#define __ETCHMONITOR_H__

#include "capu/os/Mutex.h"
#include "capu/os/CondVar.h"
#include "capu/os/Time.h"
#include "common/EtchString.h"

/**
 * A class which we can use to monitor conditions.
 */
class EtchMonitor {
public:

  /**
   * Constructs the Monitor.
   *
   * @param description a description of this monitor.
   * @param initialValue the initial value of this monitor.
   */
  EtchMonitor(EtchString description, EtchString initialValue);

  /**
   * Destructor
   */
  virtual ~EtchMonitor();

  /**
   * @return the description of this monitor.
   */
  EtchString getDescription();

  /**
   * @return the current value of the monitor.
   */
  EtchString get();

  /**
   * Sets the current value.
   *
   * @param newValue the value to be set.
   * @param the old value.
   */
  status_t set(EtchString value, EtchString& oldValue);

  /**
   * Waits until value equals the desired value and
   * then sets the value. Will wait forever.
   *
   * @param desiredValue the value we want.
   * @param newValue the value to be set.
   * @param the old value
   * @return ETCH_OK if it was successfully
   */
  status_t waitUntilEqAndSet(const EtchString& desiredValue, EtchString& newValue, EtchString &old);

  /**
   * Waits until value equals the desired value and
   * then sets the value.
   *
   * @param desiredValue the value we want.
   * @param maxDelay the max amount of time in ms to wait.
   * If 0 is specified, we will wait forever.
   * @param newValue the value to be set.
   * @param the old value
   * @return ETCH_OK if it was successfully
   */
  status_t waitUntilEqAndSet(const EtchString& desiredValue, capu::int32_t maxDelay, EtchString& newValue, EtchString &old);

  /**
   * Waits until value equals the desired value. Will wait forever.
   *
   * @param desiredValue the value we want.
   * @return ETCH_OK if it was successfully
   */
  status_t waitUntilEq(const EtchString& desiredValue);

  /**
   * Waits until value equals the desired value.
   *
   * @param desiredValue the value we want.
   *
   * @param maxDelay the max amount of time in ms to wait.
   * If 0 is specified, we will wait forever.
   * @return ETCH_OK if it was successfully
   */

  status_t waitUntilEq(const EtchString& desiredValue, capu::int32_t maxDelay);

  /**
   * Waits until value does not equal the undesired value and then
   * sets the value. Will wait forever.
   *
   * @param undesiredValue the value we do not want.
   * @param newValue the value to be set.
   * @param the old value
   * @return ETCH_OK if it was successfully
   */

  status_t waitUntilNotEqAndSet(const EtchString& undesiredValue, EtchString& newValue, EtchString& old);
  /**
   * Waits until value does not equal the undesired value and then
   * sets the value.
   *
   * @param undesiredValue the value we do not want.
   * @param maxDelay the max amount of time in ms to wait.
   * If 0 is specified, we will wait forever.
   * @param newValue the value to be set.
   * @param the old value
   * @return ETCH_OK if it was successfully, ETCH_TIMEOUT if a timeout occurs
   */

  status_t waitUntilNotEqAndSet(const EtchString& undesiredValue, capu::int32_t maxDelay, EtchString& newValue, EtchString& old);

  /**
   * Waits until value does not equal the undesired value. Will
   * wait forever.
   *
   * @param undesiredValue the value we do not want.
   * @param the current value.
   * @return ETCH_OK if it was successfully
   */

  status_t waitUntilNotEq(const EtchString& undesiredValue, EtchString& current);

  /**
   * Waits until value does not equal the undesired value.
   *
   * @param undesiredValue the value we do not want.
   * @param maxDelay the max amount of time in ms to wait.
   * If 0 is specified, we will wait forever.
   * @param the current value.
   * @return ETCH_OK if it was successfully, ETCH_TIMEOUT if a timeout occurs
   */
  status_t waitUntilNotEq(const EtchString& undesiredValue, capu::uint32_t maxDelay, EtchString& current);

private:
  /////////////////////
  // PRIVATE METHODS //
  /////////////////////

  /**
   * waitUntilEq internal
   */
  status_t waitUntilEqIntern(const EtchString& desiredValue, capu::int32_t maxDelay);

  /**
   * waitUntilNotEq internal
   */
  status_t waitUntilNotEqIntern(const EtchString& undesiredValue, capu::uint32_t maxDelay, EtchString& current);

  /**
   * Compares the specified values.
   *
   * @param v1 a value to compare, which may be null.
   * @param v2 another value to compare, which may be null.
   * @return true if the values are equal, false otherwise.
   */
  capu::bool_t eq(const EtchString& v1, const EtchString& v2);

  EtchString mDescription;
  EtchString mValue;
  capu::Mutex mMutex;
  capu::CondVar mCv;
};

#endif
