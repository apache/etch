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
using System.Runtime.CompilerServices;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace org.apache.etch.examples.chat
{
    public class ImplChatServer : BaseChatServer
    {

        private readonly RemoteChatClient _client;
        private Dictionary<String, ImplChatServer> _whoIsOnline;

        /// <summary>
        /// Constructs the ImplChatServer.
        /// </summary>
        /// <param name="client">the client to use for callbacks.</param>
        /// <param name="whoIsOnline">the database of who is online.</param>
        public ImplChatServer( RemoteChatClient client, Dictionary<String, ImplChatServer> whoIsOnline )
        {
            _client = client;
            _whoIsOnline = whoIsOnline;
        }

        #region ChatServer Members

        private string _user;
 
        [ MethodImpl ( MethodImplOptions.Synchronized ) ]
        public override void login( string name, string pwd )
        {
            if ( (bool) isLoggedIn() )
                throw new org.apache.etch.examples.chat.types.Chat.Failure( "Already logged in" );

            if ( name == null )
                throw new types.Chat.Failure("Name is null");
            
            if ( name.StartsWith("bad"))
                throw new types.Chat.Failure("Username is not valid");

            if ( name.Length == 0 )
                throw new types.Chat.Failure("Username is empty");

            if ( pwd == null )
                throw new types.Chat.Failure("Password is not valid");

            if ( pwd.StartsWith("bad"))
                throw new types.Chat.Failure("Password is not valid");

            // TODO check user authentication.

            // add this user to the who is online map.

            lock ( _whoIsOnline ) 
            {
                ImplChatServer other ;

                // user is logged in already
                if ( _whoIsOnline.ContainsKey( name ) )
                {
                    other = _whoIsOnline[ name ];
                    try
                    {
                        other.Deliver( "SYSTEM", "You have been logged out because you logged in somewhere else." );
                    }
                    catch (Exception)
                    {
                        // ignore failure to send this message
                    }
                    other.logout();
                    other = null;
                }
                // key is not present, i.e., user isn't logged in already
                else
                    _whoIsOnline.Add( name, this );
            }

            // mark as logged in
            _user = name;

            // say we're online
            SayStatusChanged( _user, true );

            // tell the client who's online
            List<String> who;


            lock ( _whoIsOnline )
            {
                who = new List<string>( _whoIsOnline.Keys );
            }

            who.Remove( name );
            _client.whoIsOnline( who.ToArray() );

            Console.WriteLine( "login : " + name );
        }

        [MethodImpl( MethodImplOptions.Synchronized )]
        public override void logout()
        {
            if ( _user != null )
            {
                bool tookUsOffline = false;

                lock ( _whoIsOnline )
                {
                    if ( _whoIsOnline.ContainsKey( _user ) )
                    {
                        if ( _whoIsOnline[ _user ] == this )
                        {
                            // remove us from the online map

                            _whoIsOnline.Remove( _user );
                            tookUsOffline = true;
                        }
                    }
                }

                if ( tookUsOffline )
                {
                    // say we're offline
                    SayStatusChanged( _user, false );
                    Console.WriteLine( "logout : " + _user );
                }
            }

            _user = null;
        }

        #endregion

        #region Chat Members

        public override bool? isLoggedIn()
        {
            return ( _user != null );
        }

        private void SayStatusChanged( String u, bool isOnline )
        {
            Dictionary<String, ImplChatServer>.ValueCollection who = _whoIsOnline.Values;
            ImplChatServer[] cArray = new ImplChatServer[ _whoIsOnline.Count ];
            who.CopyTo( cArray, 0 );

            foreach ( ImplChatServer other in cArray )
            {
                if ( other != this )
                    other._client.statusChange( u, isOnline );
            }
        }

        // send
        public override void send( string who, string msg )
        {
            String me = _user;
            if ( me == null )
                return;

            ImplChatServer other;
            lock ( _whoIsOnline )
            {
                if ( _whoIsOnline.ContainsKey( who ) )    
                    other = _whoIsOnline[ who ];
                else
                    other = null;
            }

            if ( other == null )
                throw new types.Chat.Failure( "user is not online : " + who );

            other.Deliver( me, msg );
        }

        #endregion

        private void Deliver( String from, String msg )
        {
            try 
            {
                _client.send(from, msg );
            }
            catch( types.Chat.Failure e ) 
            {
                Console.WriteLine( e );
                throw e;
            }
            catch ( Exception e ) 
            {
                Console.WriteLine( e );
                throw e;
            }
        }
    
        #region Session Members

       

        public override void _SessionNotify(object eventObj)
        {
            if (eventObj.Equals(SessionConsts.UP))
                return;
             if (eventObj.Equals(SessionConsts.DOWN))
             {
                 logout();
                 return;
             }
             if (eventObj is Exception)
                 Console.WriteLine(((Exception) eventObj).StackTrace);
                
        }

   

        #endregion
    }
}
