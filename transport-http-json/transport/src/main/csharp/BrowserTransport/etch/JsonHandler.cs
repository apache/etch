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

using System;
using System.Text;
using System.Collections.Generic;
using Etch.Support;

using Etch.Util;

namespace Etch.Transport
{
    /// <summary>
    /// Interface used to deliver JSON objects from a browser client
    /// </summary>
    public interface JsonHandler
    {
        /// <summary>
        /// Delivers a JSON request object from a browser client to server
        /// <param name="transactionId">Must be long-parseable</param>
        /// </summary>
        void MessageFromClient( Who sender, int version, string methodName, string transactionId, Dictionary<string, object> arguments );

        /// <summary>
        /// Delivers a JSON response object from a browser client to server
        /// <param name="transactionId">Must be long-parseable</param>
        /// </summary>
        void ResponseFromClient( Who sender, int version, string transactionId, string originalMethodName, object returnData );
    }
}
