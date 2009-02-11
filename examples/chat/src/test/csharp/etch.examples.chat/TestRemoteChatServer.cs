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
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;
using NUnit.Framework;
using org.apache.etch.examples.chat.types.Chat;

namespace org.apache.etch.examples.chat
{
    [TestFixture]
    public class TestRemoteChatServer
    {
        private const String _fred = "Fred";
       
        private static RemoteChatServer fred_server;

        private static ServerFactory listener;

        [TestFixtureSetUp]
        public  void SetupListener()
        {
            String uri = "tcp://localhost:4010";

            MainChatListener implFactory = new MainChatListener();
            listener = ChatHelper.NewListener(uri, null, implFactory);

            listener.TransportControl(TransportConsts.START_AND_WAIT_UP, 4000);

            Console.WriteLine("Listener Started");
        }
      
        [SetUp]
        public void AssignNullToConnections()
        {
            fred_server = null;
           
        }

               
        [Test]
        public void TestLogin1()
        {
            fred_server = SetupUserAndConnection(_fred);
            fred_server.login( _fred, _fred );
        }


        [Test]
        [ExpectedException( typeof( Failure ) )]
        public void TestLogin2()
        {
            fred_server =  SetupUserAndConnection(_fred );           
            fred_server.login( _fred, _fred );
            fred_server.login(_fred, _fred);            
        }

        [Test]
        [ExpectedException(typeof(Failure))]
        public void TestLogin3()
        {
            fred_server = SetupUserAndConnection(null);
            fred_server.login(null, null);
        }

        [Test]
        [ExpectedException(typeof(Failure))]
        public void TestLogin4()
        {
            fred_server = SetupUserAndConnection("fred");
            fred_server.login("fred", "badPass");
        }

        [Test]
        [ExpectedException(typeof(Failure))]
        public void TestLogin()
        {
            fred_server = SetupUserAndConnection("badUser");
            fred_server.login("badUser", "badPass");

        }

        [TearDown]
        public void ShutDownOpenConnections()
        {
            if (fred_server != null)
            {
                fred_server.logout();
                fred_server._Stop();
            }
        }

        [TestFixtureTearDown]
        public void StopListener()
        {
            if (listener != null)
            {
                listener.TransportControl(TransportConsts.STOP_AND_WAIT_DOWN, 4000);
                listener = null;
            }
        }

        /// <summary>
        /// Setup the connection and server specific information for this user. 
        /// </summary>
        /// <param name="username"></param>
        /// <returns>The ChatServer instance to play with</returns>
        /// 
        public static RemoteChatServer SetupUserAndConnection( String username)
        {
            
            String uri = "tcp://localhost:4010";
            
            MyChatClientImpl client = new MyChatClientImpl( username);
            RemoteChatServer server = ChatHelper.NewServer( uri,null, client );
         
            server._StartAndWaitUp( 4000 );

            return server;           
        }

        /// <summary>
        /// Create your own ChatClientImpl so as to verify messages. 
        /// </summary>
        public class MyChatClientImpl : ChatHelper.ChatClientFactory
        {
                     
            public String user;

            /// <summary>
            /// Constructs the ChatClientImpl
            /// </summary>
          
            public MyChatClientImpl(string user )
            {
             
                this.user = user;
            }


            #region ChatClientFactory Members

            public ChatClient NewChatClient(RemoteChatServer server)
            {
                return new ImplChatClient(server, user, false);
            }

            #endregion
        }

    }
}
