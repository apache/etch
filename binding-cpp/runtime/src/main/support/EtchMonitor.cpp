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

#include "capu/os/NumericLimits.h"
#include "support/EtchMonitor.h"

EtchMonitor::EtchMonitor(EtchString description, EtchString initialValue) {
  mDescription = description;
  mValue = initialValue;
}

EtchMonitor::~EtchMonitor() {}

EtchString EtchMonitor::getDescription() {
  return mDescription;
}

EtchString EtchMonitor::get() {
  return mValue;
}

status_t EtchMonitor::set(EtchString value, EtchString& oldValue) {
  mMutex.lock();
  oldValue = mValue;
  mValue = value;
  mCv.broadcast();
  mMutex.unlock();
  return ETCH_OK;
}

status_t EtchMonitor::waitUntilEqAndSet(const EtchString& desiredValue, EtchString& newValue, EtchString &old) {
  return waitUntilEqAndSet(desiredValue, 0, newValue, old);
}

status_t EtchMonitor::waitUntilEqAndSet(const EtchString& desiredValue, capu::int32_t maxDelay, EtchString& newValue, EtchString &old) {
  mMutex.lock();
  if (waitUntilEqIntern(desiredValue, maxDelay) != ETCH_OK) {
    mMutex.unlock();
    return ETCH_TIMEOUT;
  }
  set(newValue, old);
  mMutex.unlock();
  return ETCH_OK;
}

status_t EtchMonitor::waitUntilEqIntern(const EtchString& desiredValue, capu::int32_t maxDelay) {
  capu::uint64_t now = capu::Time::GetMilliseconds();
  capu::uint64_t end = (maxDelay > 0) ? now + maxDelay : capu::NumericLimits::Max<capu::int64_t>();

  capu::int64_t d = end - now;
  while (!eq(mValue, desiredValue) && d > 0) {
    mCv.wait(&mMutex, static_cast<capu::uint32_t>(d));
    now = capu::Time::GetMilliseconds();
    d = end - now;
  }

  if (!eq(mValue, desiredValue)) {
    return ETCH_TIMEOUT;
  }
  return ETCH_OK;
}

status_t EtchMonitor::waitUntilEq(const EtchString& desiredValue) {
  return waitUntilEq(desiredValue, 0);
}

status_t EtchMonitor::waitUntilEq(const EtchString& desiredValue, capu::int32_t maxDelay) {
  status_t status;
  mMutex.lock();
  status = waitUntilEqIntern(desiredValue, maxDelay);
  mMutex.unlock();
  return status;
}

status_t EtchMonitor::waitUntilNotEqAndSet(const EtchString& undesiredValue, EtchString& newValue, EtchString& old) {
  return waitUntilNotEqAndSet(undesiredValue, 0, newValue, old);
}

status_t EtchMonitor::waitUntilNotEqAndSet(const EtchString& undesiredValue, capu::int32_t maxDelay, EtchString& newValue, EtchString& old) {
  mMutex.lock();
  EtchString str;
  if (waitUntilNotEqIntern(undesiredValue, maxDelay, str) != ETCH_OK) {
    mMutex.unlock();
    return ETCH_TIMEOUT;
  }
  set(newValue, old);
  mMutex.unlock();
  return ETCH_OK;
}

status_t EtchMonitor::waitUntilNotEq(const EtchString& undesiredValue, EtchString& current) {
  return waitUntilNotEq(undesiredValue, 0, current);
}

status_t EtchMonitor::waitUntilNotEq(const EtchString& undesiredValue, capu::uint32_t maxDelay, EtchString& current) {
  status_t status;
  mMutex.lock();
  status = waitUntilNotEqIntern(undesiredValue, maxDelay, current);
  mMutex.unlock();
  return status;
}

status_t EtchMonitor::waitUntilNotEqIntern(const EtchString& undesiredValue, capu::uint32_t maxDelay, EtchString& current) {
  capu::uint64_t now = capu::Time::GetMilliseconds();
  capu::uint64_t end = (maxDelay > 0) ? now + maxDelay : capu::NumericLimits::Max<capu::int64_t>();

  capu::int64_t d = end - now;
  while (eq(mValue, undesiredValue) && d > 0) {
    mCv.wait(&mMutex, static_cast<capu::uint32_t>(d));

    now = capu::Time::GetMilliseconds();
    d = end - now;
  }

  if (eq(mValue, undesiredValue)) {
    return ETCH_TIMEOUT;
  }

  current = mValue;
  return ETCH_OK;
}

capu::bool_t EtchMonitor::eq(const EtchString& v1, const EtchString& v2) {
  return v1.equals(&v2);
}
