// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
using System;

namespace Org.Apache.Etch.Bindings.Csharp.Util
{
    /// <summary>
    /// TCP connection options.
    /// </summary>
    public class TcpOptions
    {
        /// <summary>
        /// Constructs TcpOptions from uri and resources.
        /// </summary>
        /// <param name="uri"></param>
        /// <param name="resources"></param>
        public TcpOptions( URL uri, Resources resources )
        {
            autoFlush = uri.GetBooleanTerm(AUTO_FLUSH, false);
            bufferSize = CheckBufferSize(uri.GetIntegerTerm(BUFFER_SIZE, 0));
            keepAlive = uri.GetBooleanTerm(KEEP_ALIVE, false);
            lingerTime = CheckLingerTime(uri.GetIntegerTerm(LINGER_TIME, 30));
            noDelay = uri.GetBooleanTerm(NO_DELAY, true);
            reconnectDelay = CheckReconnectDelay(uri.GetIntegerTerm(RECONNECT_DELAY, 0));
            trafficClass = CheckTrafficClass(uri.GetIntegerTerm(TRAFFIC_CLASS, 0));
        }

        private static int CheckBufferSize(int bufferSize)
        {
            if (bufferSize < 0 || bufferSize > 65536)
                throw new ArgumentException(
                    "bufferSize < 0 || bufferSize > 65536");
            return bufferSize;
        }

        private static int CheckLingerTime(int lingerTime)
        {
            if (lingerTime < -1 || lingerTime > 240)
                throw new ArgumentException(
                    "lingerTime < -1 || lingerTime > 240");
            return lingerTime;
        }

        private static int CheckReconnectDelay(int reconnectDelay)
        {
            if (reconnectDelay < 0)
                throw new ArgumentException(
                    "reconnectDelay < 0");
            return reconnectDelay;
        }

        private static int CheckTrafficClass(int trafficClass)
        {
            if (trafficClass < 0 || trafficClass > 255)
                throw new ArgumentException(
                    "trafficClass < 0 || trafficClass > 255");
            return trafficClass;
        }

        /// <summary>
        /// The auto flush setting for this connection. If true, each call to send
        /// must automatically call flush.
        /// </summary>
        public bool autoFlush;

        /// <summary>
        /// The output buffer size to use for this connection. Bytes, 0 means
        /// unbuffered output. If using buffered output, you'll want to disable
        /// auto flush and call flush manually only when needed.
        /// </summary>
        public int bufferSize;

        /// <summary>
        /// The tcp keep alive setting for this connection.
        /// </summary>
        public bool keepAlive;

        /// <summary>
        /// The tcp linger time setting for this connection. Time in seconds, -1
        /// means disable.
        /// </summary>
        public int lingerTime;

        /// <summary>
        /// The tcp no delay setting for this connection. True disables nagle's
        /// algorithm and causes all sends to be made asap.
        /// </summary>
        public bool noDelay;

        /// <summary>
        /// The reconnect delay for this connection. Time in milliseconds, 0 means
        /// do not reconnect.
        /// </summary>
        public int reconnectDelay;

        /// <summary>
        /// The traffic class for this connection. 0-255, 0 means normal handling.
        /// Also called type of service or dscp.
        /// </summary>
        public int trafficClass;

        /// <summary>
        /// Term on the uri which specifies the auto flush flag. The term string is
        /// "TcpTransport.autoFlush". The value is "true" or "false". The default
        /// is "false".
        /// </summary>
        public const String AUTO_FLUSH = "TcpTransport.autoFlush";

        /// <summary>
        /// Term on the uri which specifies the buffer size in bytes. The term
        /// string is "TcpTransport.bufferSize". The value is an integer between
        /// 0 and 65536. The default is 0.
        /// </summary>
        public const String BUFFER_SIZE = "TcpTransport.bufferSize";

        /// <summary>
        /// Term on the uri which specifies the keep alive flag. The term string is
        /// "TcpTransport.keepAlive". The value is "true" or "false". The default is
        /// "false".
        /// </summary>
        public const String KEEP_ALIVE = "TcpTransport.keepAlive";

        /// <summary>
        /// Term on the uri which specifies the linger time in seconds. The term
        /// string is "TcpTransport.lingerTime". The value is an integer between -1
        /// and 240. The default is 30. -1 means disable.
        /// </summary>
        public const String LINGER_TIME = "TcpTransport.lingerTime";

        /// <summary>
        /// Term on the uri which specifies the no delay flag. The term string is
        /// "TcpTransport.noDelay". The value is "true" or "false". The default is
        /// "true".
        /// </summary>
        public const String NO_DELAY = "TcpTransport.noDelay";

        /// <summary>
        /// Term on the uri which specifies the reconnect delay in milliseconds. The
        /// term string is "TcpTransport.reconnectDelay". The value is an integer >=
        /// 0. The default is 0.
        /// </summary>
        public const String RECONNECT_DELAY = "TcpTransport.reconnectDelay";

        /// <summary>
        /// Term on the uri which specifies the traffic class. The term string is
        /// "TcpTransport.trafficClass". The value is an integer between 0 and 255.
        /// The default is 0.
        /// </summary>
        public const String TRAFFIC_CLASS = "TcpTransport.trafficClass";
    }
}
