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

const capu::uint32_t EtchPacketizer::HEADER_SIZE = 8;

const capu::int32_t EtchPacketizer::SIG = 0xdeadbeef;

const capu::int32_t EtchPacketizer::DEFAULT_MAX_PKT_SIZE = 16384 - EtchPacketizer::HEADER_SIZE;

const EtchString EtchPacketizer::MAX_PKT_SIZE_TERM("Packetizer.maxPktSize");

EtchPacketizer::EtchPacketizer(EtchTransportData* transport, EtchString& uri)
: mTransport(transport), mBodyLen(0), mWantHeader(true) {

  if (mTransport != NULL)
    mTransport->setSession(this);
  
  EtchURL url(uri.c_str());
  EtchString value;
  url.getTerms().get((EtchString&) EtchPacketizer::MAX_PKT_SIZE_TERM, &value);
  if (value.length() == 0)
    mMaxPktSize = EtchPacketizer::DEFAULT_MAX_PKT_SIZE;
  else {
    mMaxPktSize = atoi(value.c_str());

    if (mMaxPktSize <= 0)
      mMaxPktSize = DEFAULT_MAX_PKT_SIZE;
  }
  mSavedBuf = new EtchFlexBuffer();
}

EtchPacketizer::EtchPacketizer(EtchTransportData* transport, EtchURL* uri)
: mTransport(transport), mBodyLen(0), mWantHeader(true) {
  EtchString value("");
  if (mTransport != NULL)
    transport->setSession(this);

  uri->getTerms().get((EtchString&) EtchPacketizer::MAX_PKT_SIZE_TERM, &value);
  if (value.length() == 0)
    mMaxPktSize = EtchPacketizer::DEFAULT_MAX_PKT_SIZE;
  else {
    mMaxPktSize = atoi(value.c_str());

    if (mMaxPktSize <= 0)
      mMaxPktSize = DEFAULT_MAX_PKT_SIZE;
  }
  mSavedBuf = new EtchFlexBuffer();
}

EtchPacketizer::~EtchPacketizer() {
  mTransport->setSession(NULL);
}

capu::int32_t EtchPacketizer::getHeaderSize() {
  return EtchPacketizer::HEADER_SIZE;
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
  if (dataSize < EtchPacketizer::HEADER_SIZE)
    return ETCH_ERANGE;
  capu::uint32_t pktSize = dataSize - EtchPacketizer::HEADER_SIZE;
  if (pktSize > mMaxPktSize)
    return ETCH_ERANGE;

  capu::uint32_t index = buf->getIndex();
  buf->put((capu::int8_t*) & EtchPacketizer::SIG, sizeof (EtchPacketizer::SIG));
  buf->put((capu::int8_t*) & pktSize, sizeof (pktSize));
  buf->setIndex(index);
  return mTransport->transportData(recipient, buf);
}

status_t EtchPacketizer::sessionData(capu::SmartPointer<EtchWho> sender, capu::SmartPointer<EtchFlexBuffer> buf) {

  // there are two options here. one is that we have no buffered data
  // and the entire packet is contained within the buf. in that case
  // we could optimize the daylights out of the process and directly
  // drop the packet on the handler.

  status_t result;
  while (buf->getAvailableBytes() > 0) {
    if (mWantHeader) {
      // do we have enough to make a header?

      if (mSavedBuf->getLength() + buf->getAvailableBytes() >= HEADER_SIZE) {
        capu::uint32_t pktSize;

        if (mSavedBuf->getLength() == 0) {
          // savedBuf is empty, entire header in buf.

          result = processHeader(buf.get(), false, pktSize);
          if (result != ETCH_OK)
            return result;
        } else // header split across savedBuf and buf
        {
          // move just enough data from buf to savedBuf to have a header.

          capu::uint32_t needFromBuf = HEADER_SIZE - mSavedBuf->getLength();
          mSavedBuf->put(*buf, needFromBuf);
          mSavedBuf->setIndex(0);

          result = processHeader(mSavedBuf.get(), true, pktSize);
          if (result != ETCH_OK)
            return result;
        }

        if (pktSize == 0)
          continue;

        mBodyLen = pktSize;
        mWantHeader = false;
      } else // want header, but there's not enough to make it.
      {
        // save buf in savedBuf.

        mSavedBuf->setIndex(mSavedBuf->getLength());
        mSavedBuf->put(*buf);
      }
    } else if (mSavedBuf->getLength() + buf->getAvailableBytes() >= mBodyLen) {
      // want body, and there's enough to make it.

      // three possible cases: the body is entirely in savedBuf,
      // the body is split, or the body is entirely in buf. assert
      // that the body cannot entirely be in savedBuf, or else
      // we'd have processed it last time.

      if (mSavedBuf->getLength() == 0) {
        // savedBuf is empty, entire body in buf.

        capu::uint32_t length = buf->getLength();
        capu::uint32_t index = buf->getIndex();
        buf->setLength(index + mBodyLen);

        mSession->sessionPacket(sender, buf);

        buf->setLength(length);
        buf->setIndex(index + mBodyLen);

        mWantHeader = true;
      } else // body split across savedBuf and buf
      {
        // move just enough data from buf to savedBuf to have a body.

        capu::uint32_t needFromBuf = mBodyLen - mSavedBuf->getLength();
        mSavedBuf->put(*buf, needFromBuf);
        mSavedBuf->setIndex(0);

        mSession->sessionPacket(sender, mSavedBuf);

        mSavedBuf->reset();
        mWantHeader = true;
      }
    } else // want body, but there's not enough to make it.
    {
      // save buf in savedBuf.
      mSavedBuf->put(*buf);
    }
  }
  // buf is now empty, and there's nothing else to do.
  if (buf->getAvailableBytes() != 0)
    return ETCH_ERROR;
  return ETCH_OK;
}

status_t EtchPacketizer::processHeader(EtchFlexBuffer* buf, capu::bool_t reset, capu::uint32_t &pktSize) {
  capu::int32_t sig = 0;
  buf->getInteger(sig);

  if (sig != SIG) {
    return ETCH_ERROR;
  }

  buf->getInteger((capu::int32_t&)pktSize);

  if (reset)
    buf->reset();

  if (pktSize > mMaxPktSize) {
    return ETCH_ERROR;
  }
  
  return ETCH_OK;
}

EtchTransportData* EtchPacketizer::getTransport() {
  return mTransport;
}
