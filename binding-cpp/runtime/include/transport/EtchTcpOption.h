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

#ifndef __ETCHTCPOPTION_H__
#define __ETCHTCPOPTION_H__

#include "util/EtchURL.h"

class EtchTcpOption {
public:

  EtchTcpOption(EtchURL *url);

  virtual ~EtchTcpOption();

  /**
   * Term on the uri which specifies the output buffer size in bytes. The term
   * string is "TcpTransport.bufferSize". The value is an integer between 0
   * and 65536. The default is 0, which means no output buffering.
   */
  const static EtchString& BUFFER_SIZE();

  /**
   * Term on the uri which specifies the keep alive flag. The term string is
   * "TcpTransport.keepAlive". The value is "true" or "false". The default is
   * "false". Here keep alive refers to TCP specified keep alive, which is not
   * the same as any application level keep alive.
   */
  const static EtchString& KEEP_ALIVE();

  /**
   * Term on the uri which specifies the linger on close time in seconds. The
   * term string is "TcpTransport.lingerTime". The value is an integer between
   * -1 and 65535. The default is 30. The value -1 means "no linger on close".
   * The value determines how long close will wait for buffered but
   * unacknowledged data to be delivered. When the time expires, the
   * connection will be forcefully closed. (The difference between a linger of
   * -1 and a linger of 0 is subtle, but comes down to a close with a linger
   * of -1 means perform forceful close while a close with a linger of 0 means
   * perform a graceful close if there is no buffered data and a forceful
   * close if there is buffered data. A forceful close is send RST, while a
   * graceful close means send FIN and wait for FINACK.)
   */
  const static EtchString& LINGER_TIME();

  /**
   * Term on the uri which specifies the no delay flag. The term string is
   * "TcpTransport.noDelay". The value is "true" or "false". The default is
   * "true". When true, the operating system will make a best effort to
   * transmit data ASAP. When false, data might be delayed somewhat in order
   * to allow for more efficient transmission by combining packets (see
   * Nagle's Algorithm).
   */
  const static EtchString& NO_DELAY();

  /**
   * Term on the uri which specifies the reconnect delay in milliseconds. The
   * term string is "TcpTransport.reconnectDelay". The value is an integer >=
   * 0. The default is 0. The value 0 means no automatic reconnection is
   * desired.
   */
  const static EtchString& RECONNECT_DELAY();

private:

  capu::bool_t isValidNumber(const char * str, capu::int32_t len);

  capu::bool_t checkBufferSize(capu::int32_t size);

  capu::bool_t checkLingerTime(capu::int32_t lingertime);

  capu::bool_t checkReconnectDelay(capu::int32_t delay);

  capu::int32_t mBufferSize;

  capu::int32_t mLingerTime;

  capu::int32_t mReconnectDelay;

  capu::int32_t mKeepAlive;

  capu::int32_t mNoDelay;

public:
  capu::int32_t getBufferSize();

  capu::int32_t getLingerTime();

  capu::int32_t getReconnectDelay();

  capu::int32_t getKeepAlive();

  capu::int32_t getNoDelay();



};

#endif /* ETCHTCPOPTION_H */

