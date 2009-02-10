// $Id$
//
// Copyright 2007-2008 Cisco Systems Inc.
//
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

using System;
using System.Collections.Generic;
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace org.apache.etch.examples.chat
{
    ///
    ///<summary> Main implementation for ChatListener. </summary>
    ///
    public class MainChatListener : ChatHelper.ChatServerFactory
    {
        ///
        /// <summary>Main for ChatListener.</summary>
        /// <param name="args"></param>
        ///
        public static void Main(String[] args)
        {
            string uri = "tcp://127.0.0.1:4005";
            ServerFactory listener = ChatHelper.NewListener(uri, null, new MainChatListener());
            listener.TransportControl(TransportConsts.START_AND_WAIT_UP, 4000);
        }

        ///
        /// <summary>Constructs the MainChatListener.</summary>
        ///
        public MainChatListener()
        {
            whoIsOnline = new Dictionary<string, ImplChatServer>();
        }

        public readonly Dictionary<string, ImplChatServer> whoIsOnline;

        ///
        /// <summary>Return a new instance of ChatServer.</summary>
        /// <param>Remote Chat Client</param>
        ///
        public ChatServer NewChatServer(RemoteChatClient client)
        {
            return new ImplChatServer(client, whoIsOnline);
        }
    }
}