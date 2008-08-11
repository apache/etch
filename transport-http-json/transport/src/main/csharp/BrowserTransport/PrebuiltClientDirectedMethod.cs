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
using System.Collections.Generic;
using System.Text;

namespace BasicHttpListener
{
    public abstract class PrebuiltClientDirectedMethod : ClientDirectedMethod
    {
        private Dictionary<string, object> data;

        // transactionId doesn't matter, because this message is pre-built
        public PrebuiltClientDirectedMethod( string sessionId, Dictionary<string, object> data) : base ("0", sessionId) 
        {
            if( data == null )
            {
                data = new Dictionary<string, object>();
            }

            data["type"] = MessageType;

            data["sessionId"] = sessionId;

            this.data = data;
        }


        public abstract string MessageType { get; }

        public override Dictionary<String, Object> Serialize()
        {
            return this.data;
        }
    }

    public class PrebuiltNewMessage : PrebuiltClientDirectedMethod
    {
        public PrebuiltNewMessage(string sessionId, Dictionary<string, object> data)
            : base(sessionId, data)
        {
        }

        public override string MessageType { get { return "clientMessage"; } }
    }

    public class PrebuiltResponse : PrebuiltClientDirectedMethod
    {
        public PrebuiltResponse( string sessionId, Dictionary<string, object> data )
            : base(sessionId, data)
        {
        }

        public override string MessageType { get { return "messageResult"; } }

    }
}
