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

#ifndef __ETCHSESSIONPACKET_H__
#define __ETCHSESSIONPACKET_H__

#include "transport/EtchSession.h"
#include "transport/EtchFlexBuffer.h"
#include "transport/EtchWho.h"

class EtchSessionPacket : public EtchSession {

public:

  /**
   * Delivers a packet to the session from the transport.
   * @param sender the sender of the packet, for transports which allow
   * multiple senders. This is who to return any response to.
   * @param buf a FlexBuffer positioned at the packet.
   */
  virtual status_t sessionPacket(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchFlexBuffer> buf) = 0;

  /**
   * Destructor
   */
  virtual ~EtchSessionPacket() {

  }

};

#endif /* ETCHSESSIONPACKET_H */

