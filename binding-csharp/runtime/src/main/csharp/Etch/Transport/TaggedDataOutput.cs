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

using Etch.Msg;
using Etch.Util;

namespace Etch.Transport
{
    /// <summary>
    /// A TaggedDataOutputStream writes type tagged data values to an output stream.
    /// </summary>
    public interface TaggedDataOutput
    {
       
        /// <summary>
        /// Writes the message to the buf.
        /// </summary>
        /// <param name="msg">the message to be written</param>
        void WriteMessage(Message msg, FlexBuffer msgBuf);

        
    }
}