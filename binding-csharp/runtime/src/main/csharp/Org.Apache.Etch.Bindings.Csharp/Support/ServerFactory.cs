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
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Transport;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    /// <summary>
    /// Interface to use for constructing new server instances by 
    /// TransportHelper
    /// </summary>
    public interface ServerFactory : Session, Transport<Session>
    {
        /// <summary>
        /// Constructs a new server session.
        /// </summary>
        /// <param name="transport">the TransportMessage to use with the new server instance.</param>
        /// <param name="uri">the uri to use to configure the new server instance.</param>
        /// <param name="resources">the resources to use for the new server instance.</param>
        void NewServer(TransportMessage transport, string uri, Resources resources);

        /// <summary>
        /// Constructs a new instance of value factory for this session.
        /// </summary>
        /// <param name="uri">the uri to use to configure the new value factory.</param>
        /// <returns>a new instance of value factory for this session.</returns>
        ValueFactory NewValueFactory(string uri);
    }
}
