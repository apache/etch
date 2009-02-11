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
using System.Collections.Generic;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace org.apache.etch.examples.chat
{
    public class ImplChatClient : BaseChatClient
    {
        private readonly String _whoami;
        private readonly bool _snub;
        private List<String> whoisOnline = new List<string>();
        private ChatServer server;


        /// <summary>
        /// Constructs the ChatClientImpl
        /// </summary>
        /// <param name="whoami"></param>
        /// <param name="snub"></param>
        public ImplChatClient( RemoteChatServer server, String whoami, bool snub )
        {
            this.server = server;
            _whoami = whoami;
            _snub = snub;
        }

        #region ChatClient Members

        public override void whoIsOnline( string[] names )
        {
            foreach ( String name in names )
            {
                Console.WriteLine( "{0}: {1} online (src = whoIsOnline)\n", _whoami, name );
                whoisOnline.Add( name );
            }
        }

        public override void  statusChange( string name, bool? online )
        {
            if ( (bool) online )
            {
                Console.WriteLine( "{0}: {1} online (src = statusChange)\n", _whoami, name );
                whoisOnline.Add( name );
            }
            else
            {
                Console.WriteLine( " {0}: {1} offline\n", _whoami, name );
                whoisOnline.Remove( name );
            }
        }

        #endregion

        #region Chat Members

        public override bool? isLoggedIn()
        {
            // server is always logged in to the client
            return true;
        }

        public override void send( string who, string msg )
        {
            Console.WriteLine( "{0}: msg from {1}: {2}\n", _whoami, who, msg );
            if ( _snub )
            {
                server.send( who, String.Format( "{0}, leave me alone!", who ) );
                Console.WriteLine( "snub done" );
            }
        }

        #endregion

        

        #region Session Members

        public void SessionControl( object control, object value )
        {
            throw new Exception( "The method or operation is not implemented." );
        }

        public void SessionNotify( object eventObj )
        {
            if ( eventObj.Equals( SessionConsts.DOWN ) )
                Console.WriteLine( "(server disconnected)" );
        }

        public object SessionQuery( object query )
        {
            throw new Exception( "The method or operation is not implemented." );
        }

        #endregion
    }
}
