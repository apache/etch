// $Id$
// 
// Copyright 2007-2008 Cisco Systems Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Transport;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Support
{
    /// <summary>
    /// Interface to use for constructing new server instances by 
    /// TransportHelper
    /// </summary>
    public interface ServerFactory : Session
    {
        /// <summary>
        /// Constructs a new server session.
        /// </summary>
        /// <param name="m"> the TransportMessage to be used with the new session</param>
        /// <param name="vf">the value factory to be used with the new session</param>
        /// <returns>the constructed DeliveryService</returns>
        DeliveryService NewServer(TransportMessage m, ValueFactory vf);

        /// <summary>
        /// Constructs a new instance of value factory for this session.
        /// </summary>
        /// <returns>a new instance of value factory for this session.</returns>
        ValueFactory NewValueFactory();
    }
}
