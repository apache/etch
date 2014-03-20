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

#ifndef __ETCHPACKETIZER_H__
#define __ETCHPACKETIZER_H__

#include "capu/util/SmartPointer.h"
#include "transport/EtchSessionData.h"
#include "transport/EtchTransportPacket.h"
#include "transport/EtchSessionPacket.h"
#include "transport/EtchFlexBuffer.h"
#include "transport/EtchTransportData.h"
#include "util/EtchURL.h"
#include "util/EtchResources.h"
#include "common/EtchString.h"

class EtchRuntime;

/**
 * Packetizes a stream data source. Reads a packet header:
 * a 32-bit flag and a 32-bit length, both big-endian, verifies
 * the flag, and then, using the length from the header,
 * reads the packet data and passes it to the packet handler.
 * As a packet source, accepts a packet and prepends a packet
 * header to it before delivering it to a data source.
 */

class EtchPacketizer : public virtual EtchTransportPacket, public virtual EtchSessionData {
public:

  /**
   * The default maximum packet size that will be accepted, 16376 bytes.
   */
  const static capu::int32_t& DEFAULT_MAX_PKT_SIZE();

  const static EtchString& MAX_PKT_SIZE_TERM();

  const static capu::uint32_t& SIG();
  
  const static capu::uint32_t& SIG_SIZE();
  
  const static capu::uint32_t& HEADER_SIZE();
  


  /**
   * Constructs the Packetizer with null packet handler and uri specified
   * or defaulted maximum packet size.
   * @param transport used to deliver data to the peer.
   * @param uri
   * @param resources
   */
  EtchPacketizer(EtchRuntime* runtime, EtchTransportData* transport, EtchString& uri);

  /**
   * Constructs the Packetizer with null packet handler and uri specified
   * or defaulted maximum packet size.
   * @param transport used to deliver data to the peer.
   * @param uri
   * @param resources
   */
  EtchPacketizer(EtchRuntime* runtime, EtchTransportData* transport, EtchURL* uri);


public:
  /**
   * Destructor
   */
  ~EtchPacketizer();

  /**
   * @return the transport.
   */
  EtchTransportData* getTransport();

  /**
   * Returns header size
   */
  capu::int32_t getHeaderSize();

  /**
   * @see EtchSessionData
   */
  status_t sessionData(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchFlexBuffer> buf);

  /**
   * @see EtchTransport
   */
  status_t transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result);

  /**
   * @see EtchTransport
   */
  EtchSessionPacket* getSession();

  /**
   * @see EtchTransport
   */
  void setSession(EtchSessionPacket* session);

  /**
   * @see EtchTransport
   */
  status_t transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);

  /**
   * @see EtchTransport
   */
  status_t transportNotify(capu::SmartPointer<EtchObject> event);

  /**
   * @see EtchTransportPacket
   */
  status_t transportPacket(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchFlexBuffer> buf);

  /**
   * @see EtchSession
   */
  status_t sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result);

  /**
   * @see EtchSession
   */
  status_t sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value);

  /**
   * @see EtchSession
   */
  status_t sessionNotify(capu::SmartPointer<EtchObject> event);

private:
  EtchRuntime* mRuntime;

  EtchTransportData* mTransport;

  EtchSessionPacket* mSession;

  capu::uint32_t mMaxPktSize;

  EtchFlexBufferPtr mReadBuf;
  EtchFlexBufferPtr mTempBuf;
    
    
  capu::bool_t      bufferContainsPacket();
  capu::bool_t      bufferContainsHeader();
  void              bufferClean();
  EtchFlexBufferPtr extractPacket();
  void              handleCorruptedBuffer();
  

};


#endif /* ETCHPACKETIZER_H */

