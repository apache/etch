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

#ifndef __ETCHTRANSPORTPACKET_H__
#define __ETCHTRANSPORTPACKET_H__

#include "capu/util/SmartPointer.h"
#include "transport/EtchSessionPacket.h"
#include "transport/EtchTransport.h"
#include "transport/EtchFlexBuffer.h"
#include "transport/EtchWho.h"

class EtchTransportPacket : public virtual EtchTransport<EtchSessionPacket> {

public:

  /**
   * Returns the size of the packet header for the transport.
   * @return the size of the packet header for the transport.
   */
  virtual capu::int32_t getHeaderSize() = 0;

  /**
   * Delivers the packet to the transport after adding the packet header.
   * @param buf the FlexBuffer positioned on the packet, including space for
   * the header.
   */
  virtual status_t transportPacket(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchFlexBuffer> buf) = 0;

  /**
   * Destructor
   */
  virtual ~EtchTransportPacket() {

  }
};

#endif /* ETCHTRANSPORTPACKET_H */

