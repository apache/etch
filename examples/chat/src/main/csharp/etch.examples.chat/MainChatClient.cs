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
using System.Globalization;
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace org.apache.etch.examples.chat
{
    public class MainChatClient : ChatClient, ObjSession
    {
        private readonly List<String> _whoIsOnline = new List<string>();
        private readonly DateTimeFormatInfo myDTFI = new CultureInfo("en-US", false).DateTimeFormat;


        public static void Main( String[] args )
        {
            string uri;
            if ( args.Length == 1 )
            {
                uri = args[0];
            }
            else if ( args.Length == 0 )
            {
                uri = "tcp://127.0.0.1:4005?TcpTransport.reconnectDelay=4000";
            }
            else
            {
                Console.WriteLine("usage: MainChatClient [<uri>]");
                Environment.Exit( 1 );
                return;
            }

            Console.WriteLine("Connecting to chat server at " + uri);

            MainChatClient client = new MainChatClient();
            RemoteChatServer server = ChatHelper.NewServer( uri, null, new MyChatClientFactory( client ));
            server._StartAndWaitUp( 4000 );

            Console.WriteLine( "( connected, enter commands or help)" );

            while ( true )
            {
                Console.Write( "cmd : " );
                Console.Out.Flush();

                String s = Console.ReadLine();
                if (s == null)
                    break;

                if ( s.Length == 0 )
                    continue;

                char[] seps = { ' ', '\t', '\r', '\n' };
                String[] cmds = s.Split( seps );
                
                if ( cmds.Length == 0 )
                    continue;

                if ( cmds[ 0 ].Equals( "help" ) )
                {
                    Do_help();
                    continue;
                }

                if ( cmds[0].Equals( "login") )
                {
                    Do_login( server, cmds );
                    continue;
                }

                if ( cmds[0].Equals( "logout") )
                {
                    server.logout();
                    continue;
                }

                if ( cmds[0].Equals( "quit") )
                {
                    break;
                }

                if ( cmds[ 0 ].Equals( "send" ) )
                {
                    // extract message 
                    // note: cmds[2] = message to be extracted

                    if ( cmds.Length < 3 )
                    {
                        Console.WriteLine("usage : send <user> <msg>");
                        continue;
                    }

                    char[] seps2 = {'\n', '\r'};
                    String msg = "";

                    for ( int i = 2; i < cmds.Length; i++ )
                        msg = msg + " " + cmds[ i ];

                    String[] cmds2 = msg.Split( seps2 );
                    Do_send( server, cmds[ 1 ], cmds2 );
                    continue;
                }

                if ( cmds[ 0 ].Equals( "who" ) )
                {
                    Do_who( client );
                    continue;
                }

                Console.WriteLine( "unknown command" );
                Do_help();
            }

            //server.logout();
            //( ( RemoteChatServer ) server )._StopAndWaitDown( 4000 );
            server._Stop();
        }

        private static void Do_help()
        {
            Console.WriteLine( "commands: " );
	        Console.WriteLine( "  help" );
	        Console.WriteLine( "  login <user> <pwd>" );
	        Console.WriteLine( "  logout" );
	        Console.WriteLine( "  quit" );
 	        Console.WriteLine( "  send <user> <msg>" );
 	        Console.WriteLine( "  who" );
        }

        private static void Do_login( ChatServer server, String[] st )
        {
            try
            {
                String name = st[ 1 ];
                String pwd = st[ 2 ];
                server.login( name, pwd );
                Console.WriteLine( "login successful" );
            }
            catch ( org.apache.etch.examples.chat.types.Chat.Failure e )
            {
                Console.WriteLine( "failed : " + e.GetMessage() );
            }
            catch ( IndexOutOfRangeException )
            {
                Console.WriteLine( "usage : login <username> <pwd>" );
            }
            catch ( Exception e )
            {
                Console.WriteLine( e.Message );
            }
        }

        /// <summary>
        /// Send message 
        /// </summary>
        /// <param name="server"></param>
        /// <param name="user">username</param>
        /// <param name="st">message to be sent</param>
        /// 
        private static void Do_send( ChatServer server, String user, String[] st )
        {
            try
            {
                String who = user;
                String msg = st[ 0 ];
                server.send( who, msg );
            }
            catch ( org.apache.etch.examples.chat.types.Chat.Failure e )
            {
                Console.WriteLine( "failed : " + e.GetMessage() );
            }
            catch ( _Etch_AuthException )
            {
                Console.WriteLine( "you must login first" );
            }
            catch ( IndexOutOfRangeException )
            {
                Console.WriteLine( "usage: send <user> <msg> " );
            }
            catch ( Exception e )
            {
                
                Console.WriteLine( e.Message );
            }
        }

        private static void Do_who(MainChatClient client)
        {
            String[] names = client.GetWhoIsOnline();
            foreach ( String name in names )
            {
                Console.WriteLine( " " + name );
            }
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns>a list of who is currently online</returns>
        public String[] GetWhoIsOnline()
        {
            return _whoIsOnline.ToArray();
        }


        #region ChatClient Members

        public void whoIsOnline( string[] names )
        {
            foreach ( String name in names )
            {
                Console.WriteLine( name + " is online" );
                _whoIsOnline.Add( name );
            }
        }

        public void statusChange( string name, bool? online )
        {
            if ( ( bool ) online )
            {
                Console.WriteLine( name + " is online" );
                _whoIsOnline.Add( name );
            }
            else
            {
                Console.WriteLine( name + " is offline" );
                _whoIsOnline.Remove( name );
            }
        }

        #endregion

        #region Chat Members

        public bool? isLoggedIn()
        {
            // server is always logged in to the client
            return true;
        }

        public void send( string who, string msg )
        {
            lock (myDTFI)
            {
                Console.WriteLine( "[ {0}@{1} ] {2}\n", who, DateTime.Now.ToString("HH:mm:ss",myDTFI ), msg );
            }
        }


        #endregion

        #region Session Members

        public void _SessionControl( object control, object value )
        {
            throw new Exception( "The method or operation is not implemented." );
        }

        public void _SessionNotify( object eventObj )
        {
            if ( eventObj.Equals( SessionConsts.UP ) )
            {
                Console.WriteLine( " (server opened connection)" );
                return;
            }
            if ( eventObj.Equals( SessionConsts.DOWN ) )
            {
                Console.WriteLine( " (server closed connection)" );
                return;
            }
            if ( eventObj is Exception )
            {
                Console.WriteLine( eventObj );
                return;
            }

        }

        public object _SessionQuery( object query )
        {
            throw new Exception( "The method or operation is not implemented." );
        }

        #endregion

        private class MyChatClientFactory : ChatHelper.ChatClientFactory
        {
            public MyChatClientFactory(MainChatClient client)
            {
                this.client = client;
            }

            private readonly MainChatClient client;

            public ChatClient NewChatClient(RemoteChatServer server)
            {
                return client;
            }
        }
    }
}
