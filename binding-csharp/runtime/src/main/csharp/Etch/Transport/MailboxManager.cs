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
using Etch.Support;
using Etch.Util;

namespace Etch.Transport
{
    public interface MailboxManager : TransportMessage, SessionMessage
    {
        /// <summary>
        /// Removes the mailbox from the set of mailbox receiving responses
        /// to messages
        /// </summary>
        /// <param name="mb"></param>
        
        void Unregister(Mailbox mb);

        /// <summary>
        /// Redelivers defunct messages from a closed mailbox.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="msg"></param>

        void Redeliver(Who sender, Message msg);

        /// <summary>
        /// Begin a call sequence and return the mailbox which will receive the
        /// response.
        /// </summary>
        /// <param name="recipient"></param>
        /// <param name="msg"></param>
        /// <returns></returns>
        Mailbox TransportCall(Who recipient, Message msg);
    }
}
