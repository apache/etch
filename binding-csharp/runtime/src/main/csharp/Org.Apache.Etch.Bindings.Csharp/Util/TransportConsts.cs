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
    public class TransportConsts
    {
        #region Well-known queries, controls, and events.

        /// <summary>
        /// Name of value factory in resources. The value is "Transport.valueFactory".
        /// The result of looking up this name should be a {@link ValueFactory} for
        /// the service.
        /// </summary>
        public const String VALUE_FACTORY = "Transport.valueFactory";

        /// <summary>
        /// Transport control which starts the transport stack. The argument
        /// should be null
        /// </summary>
        public const String START = "START";

        /// <summary>
        /// Transport control which starts the transport stack and waits
        /// for it to come up. The argument should be the integer number of
        /// milliseconds to wait before giving up.
        /// </summary>
        public const String START_AND_WAIT_UP = "START_AND_WAIT_UP";

        /// <summary>
        /// Transport query which waits for the transport stack to come up.
        /// </summary>
        public class WaitUp
        {
            public readonly int _maxDelay;

            /// <summary>
            /// 
            /// </summary>
            /// <param name="maxDelay">delay in milliseconds</param>
            public WaitUp( int maxDelay )
            {
                _maxDelay = maxDelay;
            }
        }

        /// <summary>
        /// Transport control which stops the transport stack. The argument
        /// should be null, or a Boolean reset (true for instant close,
        /// false for a nicer, gentler close).
        /// </summary>
        public const String STOP = "STOP";

        /// <summary>
        /// Transport control which stops the transport stack and waits
        /// for it to go down. The argument should be the integer number of
        /// milliseconds to wait before giving up.
        /// </summary>
        public const String STOP_AND_WAIT_DOWN = "STOP_AND_WAIT_DOWN";

        /// <summary>
        /// Transport control which resets the transport stack (e.g., closes the
        /// socket) without stopping it. If it is set to reconnect, then it will do
        /// that. Only meaningful for connection oriented transports. Others types
        /// will ignore this.
        /// </summary>
        public const String RESET = "RESET";

        /// <summary>
        /// Transport query which asks is this a listener initiated connection or
        /// is this a client initiated connection.
        /// </summary>
        public const String IS_SERVER = "IS_SERVER";

        /// <summary>
        /// Transport query which waits for the transport stack to go down.
        /// </summary>
        public class WaitDown
        {
            public readonly int _maxDelay;

            /// <summary>
            /// 
            /// </summary>
            /// <param name="maxDelay">delay in milliseconds</param>
            public WaitDown( int maxDelay )
            {
                _maxDelay = maxDelay;
            }
        }

        #endregion
    }
}
