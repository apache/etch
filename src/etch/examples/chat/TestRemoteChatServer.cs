using System;
using System.Collections.Generic;
using System.Text;

using Etch.Msg;
using Etch.Support;
using Etch.Transport;
using System.Globalization;
using NUnit.Framework;
using Etch.Util;

namespace etch.examples.chat
{
    [TestFixture]
    public class TestRemoteChatServer
    {
        private readonly static String _fred = "Fred";
        private readonly static String _mary = "Mary";
        private readonly static String _jack = "Jack";
        Listener l;
        ChatServer fred_server, fred_server2;
        ChatServer mary_server;
        ChatServer jack_server;

        //[TestFixtureSetUp]
        //public  void SetupListener()
        //{
        //    // First start the Chat Listener

        //    TodoManager tm = new TodoManager( 99, 20, 5, 20, 60, 1 );
        //    tm.Start();

        //    Pool queuedPool = new QueuedPool( tm );
        //    Pool freePool = new FreePool();

        //    ListenerHandler lh = ChatHelper.NewChatServer( queuedPool, freePool );
        //    l = new Listener( lh, 5, "127.0.0.1", 4001, 0 );
        //    l.Start();
        //    l.WaitUp( 4000 );
        //}

        [SetUp]
        public void AssignNullToConnections()
        {
            fred_server = null;
            fred_server2 = null;
            mary_server = null;
            jack_server = null;
        }

        #region Basic <Username, Password> boundary tests
        
        [Test]
        [ExpectedException(typeof(etch.examples.chat.ConstsChat.Failure))]
        public void TestNullLoginAndPassword()
        {
            fred_server = (ChatServer) SetupUserAndConnection( null, "127.0.0.1", 4001 )[ 0 ];
            Assert.IsFalse( (bool) fred_server.isLoggedIn() );

            fred_server.login( null, null );

        }


        [Test]
        [ExpectedException( typeof( etch.examples.chat.ConstsChat.Failure ) )]
        public void TestNullPassword()
        {
            fred_server = ( ChatServer ) SetupUserAndConnection( null, "127.0.0.1", 4001 )[ 0 ];
            Assert.IsFalse( ( bool ) fred_server.isLoggedIn() );

            fred_server.login( _fred, null );
            
        }

        [Test]
        [ExpectedException( typeof( etch.examples.chat.ConstsChat.Failure ) )]
        public void TestNullLogin()
        {
            fred_server = ( ChatServer ) SetupUserAndConnection( null, "127.0.0.1", 4001 )[ 0 ];
            Assert.IsFalse( ( bool ) fred_server.isLoggedIn() );

            fred_server.login( null, "Fred" );
        }

        [Test]
        [ExpectedException( typeof( etch.examples.chat.ConstsChat.Failure ) )]
        public void TestUsernameEmpty()
        {
            fred_server = ( ChatServer ) SetupUserAndConnection( null, "127.0.0.1", 4001 )[ 0 ];
            Assert.IsFalse( ( bool ) fred_server.isLoggedIn() );

            fred_server.login( "", "Fred" );

        }

        #endregion

        #region Functional Tests

        [ExpectedException( typeof( etch.examples.chat.ConstsChat.Failure ) )]
        [Test]
        public void TestLoginTwice()
        {
            fred_server = ( ChatServer ) SetupUserAndConnection( _fred, "127.0.0.1", 4001 )[ 0 ];
            Assert.IsFalse( ( bool ) fred_server.isLoggedIn() );

            fred_server.login( _fred, "Fred" );
            Assert.IsTrue( ( bool ) fred_server.isLoggedIn() );

            fred_server.login( _fred, "Fred" );
        }

        [Test]
        public void TestLoginTwiceDifferentServers()
        {
            fred_server = ( ChatServer ) SetupUserAndConnection( _fred, "127.0.0.1", 4001 )[ 0 ];
            Assert.IsFalse( ( bool ) fred_server.isLoggedIn() );
            
            fred_server.login( _fred, "Fred" );
            Assert.IsTrue( ( bool ) fred_server.isLoggedIn() );

            //
            // Fred tries to login from another machine.
            //
            fred_server2 = ( ChatServer ) SetupUserAndConnection( _fred, "127.0.0.1", 4001 )[ 0 ];
            Assert.IsFalse( ( bool ) fred_server2.isLoggedIn() );

            fred_server2.login( _fred, "Fred" );
            Assert.IsTrue( ( bool ) fred_server2.isLoggedIn() );

            //
            // The earlier Fred is logged out.
            //
            // Assert.IsFalse( ( bool ) fred_server.isLoggedIn() );
            Assert.IsTrue( ( bool ) fred_server2.isLoggedIn() );
        }
        

        [Test]
        public void TestSimpleMessageSend()
        {
            // Connect Fred
            Object[] fred_array = SetupUserAndConnection( _fred, "127.0.0.1", 4001 );
            fred_server = ( ChatServer ) fred_array[ 0 ];
            MyChatClientImpl fred_client = ( MyChatClientImpl ) fred_array[ 1 ];
            
            Assert.IsFalse( (bool) fred_server.isLoggedIn() );


            // Connect Mary
            Object[] mary_array = SetupUserAndConnection( _mary, "127.0.0.1", 4001 );
            mary_server = ( ChatServer ) mary_array[ 0 ];
            MyChatClientImpl mary_client = ( MyChatClientImpl ) mary_array[ 1 ];

            Assert.IsFalse( ( bool ) mary_server.isLoggedIn() );

            // login fred
            fred_server.login( _fred, "Fred" );
            Assert.IsTrue( (bool) fred_server.isLoggedIn() );
            Assert.IsEmpty( fred_client._whoisOnline );

            // login mary
            mary_server.login( _mary, "Mary" );
            Assert.IsTrue( (bool) mary_server.isLoggedIn() );
            
            //
            // check whether their online lists are correct
            //
            System.Threading.Thread.Sleep( 2000 );
            Assert.AreEqual( 1, mary_client._whoisOnline.Count );
            Assert.AreEqual( _fred, mary_client._whoisOnline[ 0 ] );
            Assert.AreEqual( 1, fred_client._whoisOnline.Count );
            Assert.AreEqual( _mary, fred_client._whoisOnline[ 0 ] );
            
            // send message
            String messageToSend = " mary, what is the meaning of life? ";
            fred_server.send( _mary, messageToSend );

            // Sleep for a while, then test 
            System.Threading.Thread.Sleep( 2000 );

            // verify message received
            Assert.AreEqual( messageToSend, mary_client._msg );

        }

        [Test]
        [ExpectedException( typeof( etch.examples.chat.ConstsChat.Failure ) )]
        public void TestSendToUnknownUser()
        {
            // Connect Fred
            Object[] fred_array = SetupUserAndConnection( _fred, "127.0.0.1", 4001 );
            fred_server = ( ChatServer ) fred_array[ 0 ];

            // login fred
            fred_server.login( _fred, "Fred" );

            // send message to unknown user
            String messageToSend = " should throw failure ";
            fred_server.send( "unknown", messageToSend );

        }

        /// <summary>
        /// What is being tested? 
        /// 
        /// 1) Login three people: Fred, Mary & Jack
        /// 2) Send message in all permutations.
        /// </summary>
        [Test]
        public void TestThreeLogins()
        {
            // Connect Fred
            Object[] fred_array = SetupUserAndConnection( _fred, "127.0.0.1", 4001 );
            fred_server = ( ChatServer ) fred_array[ 0 ];
            MyChatClientImpl fred_client = ( MyChatClientImpl ) fred_array[ 1 ];

            Assert.IsFalse( ( bool ) fred_server.isLoggedIn() );


            // Connect Mary
            Object[] mary_array = SetupUserAndConnection( _mary, "127.0.0.1", 4001 );
            mary_server = ( ChatServer ) mary_array[ 0 ];
            MyChatClientImpl mary_client = ( MyChatClientImpl ) mary_array[ 1 ];

            Assert.IsFalse( ( bool ) mary_server.isLoggedIn() );

            // Connect Jack
            Object[] jack_array = SetupUserAndConnection( _jack, "127.0.0.1", 4001 );
            jack_server = ( ChatServer ) jack_array[ 0 ];
            MyChatClientImpl jack_client = ( MyChatClientImpl ) jack_array[ 1 ];

            Assert.IsFalse( ( bool ) jack_server.isLoggedIn() );

            // login fred
            fred_server.login( _fred, "Fred" );
            Assert.IsTrue( ( bool ) fred_server.isLoggedIn() );
            Assert.IsEmpty( fred_client._whoisOnline );

            // login mary
            mary_server.login( _mary, "Mary" );
            Assert.IsTrue( ( bool ) mary_server.isLoggedIn() );

            // login jack
            jack_server.login( _jack, "Jack" );
            Assert.IsTrue( ( bool ) jack_server.isLoggedIn() );


            System.Threading.Thread.Sleep( 2000 );
            //
            // check whether their online lists are correct
            //
            Assert.AreEqual( 2, mary_client._whoisOnline.Count );
            Assert.AreEqual( 2, fred_client._whoisOnline.Count );
            Assert.AreEqual( 2, jack_client._whoisOnline.Count );


            // verify messages
            
            // fred -> mary
            String fred_mary = " mary, what is the meaning of life? ";
            fred_server.send( _mary, fred_mary );
            System.Threading.Thread.Sleep( 1000 );
            Assert.AreEqual( fred_mary, mary_client._msg );

            // mary -> jack
            String mary_jack = " jack, what is the meaning of fred's life? ";
            mary_server.send( _jack, mary_jack );
            System.Threading.Thread.Sleep( 1000 );
            Assert.AreEqual( mary_jack, jack_client._msg );

            // jack -> fred
            String jack_fred = " fred, why is the meaning of your life 'what'? ";
            jack_server.send( _fred, jack_fred );
            System.Threading.Thread.Sleep( 1000 );
            Assert.AreEqual( jack_fred, fred_client._msg );

            // fred -> jack
            String fred_jack = " jack, 'what'? ";
            fred_server.send( _jack, fred_jack );
            System.Threading.Thread.Sleep( 1000 );
            Assert.AreEqual( fred_jack, jack_client._msg );
        }

        #endregion

        #region logout tests

        [Test]
        public void TestLogout()
        {
            // Connect Fred
            Object[] fred_array = SetupUserAndConnection( _fred, "127.0.0.1", 4001 );
            fred_server = ( ChatServer ) fred_array[ 0 ];
            MyChatClientImpl fred_client = ( MyChatClientImpl ) fred_array[ 1 ];

            // Connect Mary
            Object[] mary_array = SetupUserAndConnection( _mary, "127.0.0.1", 4001 );
            mary_server = ( ChatServer ) mary_array[ 0 ];
            MyChatClientImpl mary_client = ( MyChatClientImpl ) mary_array[ 1 ];

            // login fred
            fred_server.login( _fred, "Fred" );

            // login mary
            mary_server.login( _mary, "Mary" );

            // logout fred
            fred_server.logout();
            
            System.Threading.Thread.Sleep(1000);

            Assert.IsFalse( ( bool ) fred_server.isLoggedIn() );
            Assert.IsEmpty( mary_client._whoisOnline );
        }

        #endregion

        [TearDown]
        public void ShutDownOpenConnections()
        {

            if ( fred_server != null )
            {
                System.Threading.Thread.Sleep( 1000 );
                ( ( RemoteChatServer ) fred_server )._StopAndWaitDown( 4000 );
            }

            if ( fred_server2 != null )
            {
                System.Threading.Thread.Sleep( 1000 );
                ( ( RemoteChatServer ) fred_server2 )._StopAndWaitDown( 4000 );
            }

            if ( mary_server != null )
            {
                System.Threading.Thread.Sleep( 1000 );
                ( ( RemoteChatServer ) mary_server )._StopAndWaitDown( 4000 );
            }

            if ( jack_server != null )
            {
                System.Threading.Thread.Sleep( 1000 );
                ( ( RemoteChatServer ) jack_server )._StopAndWaitDown( 4000 );
            }
        }

        [TestFixtureTearDown]
        public void TearDown()
        {
            l.Stop();
            l.WaitDown( 4000 );
        }


        /// <summary>
        /// Setup the connection and server specific information for this user. 
        /// </summary>
        /// <param name="username"></param>
        /// <param name="host"></param>
        /// <param name="port"></param>
        /// <returns>The ChatServer instance to play with</returns>
        /// 
        public static Object[] SetupUserAndConnection( String username, String host, int port )
        {
            //Pool queuedPool = new QueuedPool();
            //Pool freePool = new FreePool();

            //TcpConnection conn = new TcpConnection( null, host, port, 0 );
            //MyChatClientImpl client = new MyChatClientImpl( username, false );
            //ChatServer server = ChatHelper.NewChatServer( conn, client, queuedPool, freePool );
            //client.setServer( server );

            //conn.Start();
            //conn.WaitUp( 4000 );

            URL uri = new URL("tcp://127.0.0.1:4001");
            Resources resources = ChatHelper.InitResources(null);

            MyChatClientImpl client = new MyChatClientImpl( username, false );
            RemoteChatServer server = ChatHelper.NewChatServer( uri, resources, client );
            client.setServer( server );
            server._StartAndWaitUp( 4000 );

            //return server;
            Object[] serverClientPair = new Object[ 2 ];
            serverClientPair[ 0 ] = server;
            serverClientPair[ 1 ] = client;
            return serverClientPair;
        }

        /// <summary>
        /// Create your own ChatClientImpl so as to verify messages. 
        /// </summary>
        public class MyChatClientImpl : ChatClient, Close
        {
            private readonly String _whoami;
            private readonly bool _snub;
            public List<String> _whoisOnline = new List<string>();
            private ChatServer server;
            public String _msg;

            /// <summary>
            /// Constructs the ChatClientImpl
            /// </summary>
            /// <param name="whoami"></param>
            /// <param name="snub"></param>
            public MyChatClientImpl( String whoami, bool snub )
            {
                _whoami = whoami;
                _snub = snub;
            }

            #region ChatClient Members

            public void whoIsOnline( string[] names )
            {
                foreach ( String name in names )
                {
                    //Console.WriteLine( "{0}: {1} online (src = whoIsOnline)\n", _whoami, name );
                    _whoisOnline.Add( name );
                }
            }

            public void statusChange( string name, bool? online )
            {
                if ( ( bool ) online )
                {
                    Console.WriteLine( "{0}: {1} online (src = statusChange)\n", _whoami, name );
                    _whoisOnline.Add( name );
                }
                else
                {
                    Console.WriteLine( " {0}: {1} offline\n", _whoami, name );
                    _whoisOnline.Remove( name );
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
                _msg = msg;
                if ( _snub )
                {
                    server.send( who, String.Format( "{0}, leave me alone!", who ) );
                    Console.WriteLine( "snub done" );
                }
            }

            #endregion

            /// <summary>
            /// Sets the server so that we can send messages to it. 
            /// </summary>
            /// <param name="server"></param>
            public void setServer( ChatServer server )
            {
                this.server = server;
            }

            #region Close Members

            public void _Close( MessageSource src )
            {
                // our connection to the server has been closed
                Console.WriteLine( " Server disconnected ! " );
            }

            #endregion
        }

    }
}
