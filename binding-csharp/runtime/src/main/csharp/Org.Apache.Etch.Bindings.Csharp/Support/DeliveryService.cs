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
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Transport;

namespace Org.Apache.Etch.Bindings.Csharp.Support{

    ///<summary>Adapter between remote and message source.</summary>
    public interface DeliveryService : TransportMessage,SessionMessage
    {
	
        

        /// <summary>
        /// Sends the message which begins a call sequence.
        /// </summary>
        /// <param name="msg">the message to send.</param>
        /// <returns>a mailbox which can be used to read response</returns>
        Mailbox BeginCall( Message msg );

        /// <summary>Finishes a call sequence by waiting for the response message.</summary>
        /// <param name="mb"></param>
        /// <param name="responseType">responseType the type of the expected response.</param>
        /// which would contain any result value or thrown exception.</param>
	    /// <returns>the value of the response field if it isn't an exception.</returns>
        /// Exception:
        ///             throws Exception if there is a problem sending or a timeout waiting or if the result 
        ///             value was an exception.
        Object EndCall(Mailbox mb, XType responseType );

       
    }
}
