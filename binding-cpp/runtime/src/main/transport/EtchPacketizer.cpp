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

#include "transport/EtchPacketizer.h"
#include "support/EtchRuntime.h"

const capu::uint32_t& EtchPacketizer::HEADER_SIZE() {
  static const capu::uint32_t headerSize(8);
  return headerSize;
}

const capu::uint32_t& EtchPacketizer::SIG() {
  static const capu::uint32_t sig(0xDEADBEEFu);
  return sig;
}

const capu::uint32_t& EtchPacketizer::SIG_SIZE() {
  static const capu::uint32_t sigSize(4);
  return sigSize;
}


const capu::int32_t& EtchPacketizer::DEFAULT_MAX_PKT_SIZE(){
  static const capu::int32_t pktSize(16384 - EtchPacketizer::HEADER_SIZE());
  return pktSize;
}

const EtchString& EtchPacketizer::MAX_PKT_SIZE_TERM() {
  static const EtchString pktSize("Packetizer.maxPktSize");
  return pktSize;
}

EtchPacketizer::EtchPacketizer(EtchRuntime* runtime, EtchTransportData* transport, EtchString& uri)
: mRuntime(runtime), mTransport(transport) {
  if (mTransport != NULL)
    mTransport->setSession(this);

  EtchURL url(uri.c_str());
  EtchString value;
  url.getTerms().get(EtchPacketizer::MAX_PKT_SIZE_TERM(), &value);
  if (value.length() == 0)
    mMaxPktSize = EtchPacketizer::DEFAULT_MAX_PKT_SIZE();
  else {
    mMaxPktSize = atoi(value.c_str());

    if (mMaxPktSize <= 0)
      mMaxPktSize = DEFAULT_MAX_PKT_SIZE();
  }
  mReadBuf = new EtchFlexBuffer();
  mTempBuf = new EtchFlexBuffer();
}

EtchPacketizer::EtchPacketizer(EtchRuntime* runtime, EtchTransportData* transport, EtchURL* uri)
: mRuntime(runtime), mTransport(transport) {
  EtchString value("");
  if (mTransport != NULL)
    transport->setSession(this);

  uri->getTerms().get(EtchPacketizer::MAX_PKT_SIZE_TERM(), &value);
  if (value.length() == 0)
    mMaxPktSize = EtchPacketizer::DEFAULT_MAX_PKT_SIZE();
  else {
    mMaxPktSize = atoi(value.c_str());

    if (mMaxPktSize <= 0)
      mMaxPktSize = DEFAULT_MAX_PKT_SIZE();
  }
  mReadBuf = new EtchFlexBuffer();
  mTempBuf = new EtchFlexBuffer();
}

EtchPacketizer::~EtchPacketizer() {

}

capu::int32_t EtchPacketizer::getHeaderSize() {
  return EtchPacketizer::HEADER_SIZE();
}

status_t EtchPacketizer::transportControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
  return mTransport->transportControl(control, value);
}

status_t EtchPacketizer::transportNotify(capu::SmartPointer<EtchObject> event) {
  return mTransport->transportNotify(event);
}

status_t EtchPacketizer::transportQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> *result) {
  return mTransport->transportQuery(query, result);
}

EtchSessionPacket* EtchPacketizer::getSession() {
  return mSession;
}

void EtchPacketizer::setSession(EtchSessionPacket* session) {
  mSession = session;
}

status_t EtchPacketizer::sessionControl(capu::SmartPointer<EtchObject> control, capu::SmartPointer<EtchObject> value) {
  return mSession->sessionControl(control, value);
}

status_t EtchPacketizer::sessionNotify(capu::SmartPointer<EtchObject> event) {
  return mSession->sessionNotify(event);
}

status_t EtchPacketizer::sessionQuery(capu::SmartPointer<EtchObject> query, capu::SmartPointer<EtchObject> &result) {
  return mSession->sessionQuery(query, result);
}

status_t EtchPacketizer::transportPacket(capu::SmartPointer<EtchWho> recipient, capu::SmartPointer<EtchFlexBuffer> buf) {
  // assert index is at the start of the header.
  capu::uint32_t dataSize = buf->getAvailableBytes();
  if (dataSize < EtchPacketizer::HEADER_SIZE())
    return ETCH_ERANGE;
  capu::uint32_t pktSize = dataSize - EtchPacketizer::HEADER_SIZE();
  if (pktSize > mMaxPktSize)
    return ETCH_ERANGE;

  capu::uint32_t index = buf->getIndex();
  buf->putInt(EtchPacketizer::SIG());
  buf->putInt(pktSize);
  buf->setIndex(index);
  ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getPacketizerContext(), "Header is constructed and raw data has been sent to Transport for transmission");

  return mTransport->transportData(recipient, buf);
}

status_t EtchPacketizer::sessionData(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchFlexBuffer> buf) {
  status_t result = ETCH_OK;
  
  if(mReadBuf->getAvailableBytes() > 0)
  {
    //If there is already data in the buffer, amend the new buffer by setting the index to the end of the buffer
    mReadBuf->setIndex(mReadBuf->getLength());
  }

  mReadBuf->put(*buf);
  mReadBuf->setIndex(0);

  while(bufferContainsPacket())
  {
    EtchFlexBufferPtr packetData = extractPacket();
    result = mSession->sessionPacket(sender, packetData);
  }
  return result;
}

capu::bool_t EtchPacketizer::bufferContainsPacket()
{
  if(!bufferContainsHeader())
  {
    return false;
  }
  capu::uint32_t packetSize = 0;
  if(mReadBuf->getInteger(packetSize) != ETCH_OK || mReadBuf->getAvailableBytes() < packetSize)
  {
    mReadBuf->setIndex(0);
    return false;
  }
  if (packetSize > mMaxPktSize) {
    ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getPacketizerContext(), "EtchPacketizer: PacketSize exceeded Maximum Packetsize. MaximumSize: " << mMaxPktSize << " PacketSize: " << packetSize);
    mReadBuf->setIndex(mReadBuf->getIndex() + packetSize);
    bufferClean();
  }

  mReadBuf->setIndex(0);
  return true;
}

capu::bool_t EtchPacketizer::bufferContainsHeader()
{
  if(mReadBuf->getAvailableBytes() < HEADER_SIZE())
    return false;

  capu::uint32_t sig = 0;
  mReadBuf->getInteger(sig);
  
  if(sig == SIG()) {
    return true;
  }

  ETCH_LOG_ERROR(mRuntime->getLogger(), mRuntime->getLogger().getPacketizerContext(), "EtchPacketizer: Packet SIG is incorrect - discarding package and searching for next Valid SIG.");
  
  capu::uint32_t length = mReadBuf->getLength() - sizeof(capu::int32_t);
  capu::uint32_t index = mReadBuf->getIndex();
    
  for(capu::uint32_t i = index; i < length; ++i) {
    mReadBuf->getInteger(sig);
    if(sig == SIG()) {
      ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getPacketizerContext(), "EtchPacketizer: Found correct SIG in Buffer after having a damaged Buffer.");
      mReadBuf->setIndex(i); //we found a correct Header
      bufferClean();
      mReadBuf->setIndex(SIG_SIZE());
      return true;
    }
    mReadBuf->setIndex(i+1);
  }

  mReadBuf->setIndex(mReadBuf->getLength() - HEADER_SIZE()); //Save the last 8 Bytes
  bufferClean();
  return false;
}

void EtchPacketizer::bufferClean()
{
  mTempBuf->put(*mReadBuf);
  mTempBuf->setIndex(0);
  
  mReadBuf->reset();
  mReadBuf->put(*mTempBuf);
  mReadBuf->setIndex(0);
  
  mTempBuf->reset();
}

EtchFlexBufferPtr EtchPacketizer::extractPacket()
{
  mReadBuf->setIndex(mReadBuf->getIndex() + 4);
  capu::uint32_t packetSize = 0;
  mReadBuf->getInteger(packetSize);
  
  EtchFlexBufferPtr packetData = new EtchFlexBuffer();
  packetData->put(*mReadBuf, packetSize);
  
  mReadBuf->setIndex(mReadBuf->getIndex() + packetSize);
  if(mReadBuf->getAvailableBytes() > 0)
  {
    mTempBuf->put(*mReadBuf);
    mTempBuf->setIndex(0);
  }
  mReadBuf->reset();
  mReadBuf->put(*mTempBuf);
  mReadBuf->setIndex(0);
  mTempBuf->reset();
  packetData->setIndex(0);
  ETCH_LOG_DEBUG(mRuntime->getLogger(), mRuntime->getLogger().getPacketizerContext(), "EtchPacketizer: extracted packet - passing " << packetSize << " Bytes to Messagizer.")
  return packetData;
}


EtchTransportData* EtchPacketizer::getTransport() {
  return mTransport;
}
