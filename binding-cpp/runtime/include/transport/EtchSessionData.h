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

#ifndef __ETCHSESSIONDATA_H__
#define __ETCHSESSIONDATA_H__

#include "common/EtchError.h"
#include "transport/EtchFlexBuffer.h"
#include "transport/EtchSession.h"
#include "transport/EtchWho.h"

/**
 * Interface used to deliver packets to the session from the transport.
 */

class EtchSessionData : public virtual EtchSession {

public:

  /**
   * Delivers data to the session from the transport.
   * @param sender the sender of the data, for transports which allow
   * multiple senders. This is who to return any response to.
   * @param buf a FlexBuffer positioned at the data.
   * @return ETCH_ERROR if there is a parsing error
   *         ETCH_OK if received data from transport is successfully parsed and passed to the session
   */
  virtual status_t sessionData(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchFlexBuffer> buf) = 0;

  /**
   * Destructor
   */
  virtual ~EtchSessionData() {

  }
};

#endif /* ETCHSESSIONDATA_H */

