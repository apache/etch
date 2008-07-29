using System;
using System.Collections.Generic;
using System.Text;
using NUnit.Framework;
using System.Runtime.CompilerServices;
using System.Globalization;
using System.Net.Sockets;
using System.Threading;
using System.Net;
using Etch.Support;

namespace Etch.Transport.Test
{       
    [TestFixture]
    public class TestTcpConnection
    {

        public static MyListener lh = new MyListener();
        public static Connection<ListenerHandler> l;
        public TcpConnection c;
        private static int TIMEOUT = 4000;
        private static int port;
        
        [TestFixtureSetUp]
        public void StartListener()
        {
            l =  new Listener( lh, 5, "127.0.0.1", 4000, 0 );
            l.Start();
            l.WaitUp( 4000 );
            port = ( ( IPEndPoint ) l.LocalAddress() ).Port;
            Console.WriteLine( " port = " + port );
        }

        [TestFixtureTearDown]
        public void StopListener()
        {
            l.Stop();
            l.WaitDown( 4000 );
        }

        #region Constructor tests

        [Test]
        [ExpectedException( typeof( ArgumentException ) )]
        public void Constructor1()
        {
            new TcpConnection( ( DataHandler) null, (Socket) null );
        }

        [Test]
        public void Constructor2()
        {
            IPEndPoint ipe = new IPEndPoint( IPAddress.Parse( "127.0.0.1" ), port );
            Socket s = new Socket( ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp );
            s.Connect( ipe );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, null );

            TcpConnection c = new TcpConnection( null, s );
            Assert.IsNull( c.GetHandler() );

            s.Close();
            t.Close();

        }

        [Test]
        public void Constructor3()
        {
            DataHandler dh = new MyDataHandler();
            IPEndPoint ipe = new IPEndPoint( IPAddress.Parse( "127.0.0.1" ), port );
            Socket s = new Socket( ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp );
            s.Connect( ipe );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, null );

            TcpConnection c = new TcpConnection( dh, s );
            Assert.AreEqual( dh, c.GetHandler() );

            s.Close();
            t.Close();
        }

        [Test]
        [ExpectedException ( typeof ( ArgumentException ) ) ]
        public void Constructor4()
        {
            new TcpConnection( null, null, 0, 0 );
        }

        [Test]
        [ExpectedException( typeof( ArgumentException ) )]
        public void Constructor5()
        {
            new TcpConnection( null, "localhost", 0, 0 );
        }

        [Test]
        [ExpectedException( typeof( ArgumentException ) )]
        public void Constructor6()
        {
            new TcpConnection( null, "localhost", 65536, 0 );
        }

        [Test]
        [ExpectedException( typeof( ArgumentException ) )]
        public void Constructor7()
        {
            new TcpConnection( null, "localhost", 1, -1 );
        }

        [Test]
        public void Constructor8()
        {
            TcpConnection c = new TcpConnection( null, "localhost", port, 0 );
            Assert.IsNull( c.GetHandler() );
        }

        [Test]
        public void Constructor9()
        {
            DataHandler dh = new MyDataHandler();
            TcpConnection c = new TcpConnection( dh, "localhost", port, 0 );
            Assert.AreEqual( dh, c.GetHandler() );
        }

        #endregion

        #region Start variations

        [Test]
        public void Start1()
        {
            IPEndPoint ipe = new IPEndPoint( IPAddress.Parse( "127.0.0.1" ), port );
            Socket s = new Socket( ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp );
            s.Connect( ipe );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, null );

            MyDataHandler dh = new MyDataHandler();
            TcpConnection c = new TcpConnection( dh, s );

            Assert.IsFalse( c.IsStarted() );
            c.Start();
            Assert.IsTrue( c.IsStarted() );

            c.WaitUp( TIMEOUT );

            AssertWhat( What.UP, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            c.Flush();
            Assert.IsNotNull( c.RemoteAddress() );
            Assert.IsNotNull( c.LocalAddress() );
            Assert.AreEqual( c.RemoteAddress(), t.LocalEndPoint );
            Assert.AreEqual( c.LocalAddress(), t.RemoteEndPoint );
            
            t.Close();

            AssertWhat( What.DOWN, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );
        }

        [Test]
        public void Start2()
        {
            IPEndPoint ipe = new IPEndPoint( IPAddress.Parse( "127.0.0.1" ), port );
            Socket s = new Socket( ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp );
            s.Connect( ipe );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, null );

            MyDataHandler dh = new MyDataHandler();
            TcpConnection c = new TcpConnection( dh, s );

            Assert.IsFalse( c.IsStarted() );
            c.Start();
            Assert.IsTrue( c.IsStarted() );
            c.WaitUp( TIMEOUT );

            AssertWhat( What.UP, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            Assert.IsTrue( c.IsStarted() );
            c.Stop();

            Assert.IsFalse( c.IsStarted() );

            c.WaitDown( TIMEOUT );

            
            AssertWhat( What.DOWN, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            t.Close();
        }

        [Test]
        public void Start3()
        {

            IPEndPoint ipe = new IPEndPoint( IPAddress.Parse( "127.0.0.1" ), port );
            Socket s = new Socket( ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp );
            s.Connect( ipe );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, null );

            MyDataHandler dh = new MyDataHandler();
            TcpConnection c = new TcpConnection( dh, s );

            Assert.IsFalse( c.IsStarted() );
            c.Start();
            Assert.IsTrue( c.IsStarted() );

            c.WaitUp( TIMEOUT );

            AssertWhat( What.UP, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            Assert.IsTrue( c.IsStarted() );
            c.Close();

            c.WaitDown( TIMEOUT );

            AssertWhat( What.DOWN, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            t.Close();
        }

        [Test]
        public void Start4()
        {

            IPEndPoint ipe = new IPEndPoint( IPAddress.Parse( "127.0.0.1" ), port );
            Socket s = new Socket( ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp );

            s.Connect( ipe );
            
            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, null );

            MyDataHandler dh = new MyDataHandler();
            TcpConnection c = new TcpConnection( dh, s );

            Assert.IsFalse( c.IsStarted() );
            c.Start();
            Assert.IsTrue( c.IsStarted() );

            c.WaitUp( TIMEOUT );

            AssertWhat( What.UP, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            Assert.IsTrue( c.IsStarted() );
            c.Close( false );

            c.WaitDown( TIMEOUT );

            AssertWhat( What.DOWN, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            t.Close();
        }

        [Test]
        public void Start5()
        {

            IPEndPoint ipe = new IPEndPoint( IPAddress.Parse( "127.0.0.1" ), port );
            Socket s = new Socket( ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp );
            s.Connect( ipe );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, null );

            MyDataHandler dh = new MyDataHandler();
            TcpConnection c = new TcpConnection( dh, s );

            Assert.IsFalse( c.IsStarted() );
            c.Start();
            Assert.IsTrue( c.IsStarted() );

            c.WaitUp( TIMEOUT );

            AssertWhat( What.UP, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            Assert.IsTrue( c.IsStarted() );
            c.Close( true );

            c.WaitDown( TIMEOUT );

            AssertWhat( What.DOWN, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            t.Close();
        }

        [Test]
        public void Start6()
        {
            MyDataHandler dh = new MyDataHandler();
            TcpConnection c = new TcpConnection( dh, "127.0.0.1", port, 0 );

            Assert.IsFalse( c.IsStarted() );
            c.Start();
            Assert.IsTrue( c.IsStarted() );
            c.WaitUp( TIMEOUT );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, null );

            AssertWhat( What.UP, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            Assert.IsTrue( c.IsStarted() );
            c.Stop();
            Assert.IsFalse( c.IsStarted() );

            AssertWhat( What.DOWN, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            t.Close();
        
        }
        
        #endregion

        #region Data tests

        [Test]
        public void Data1()
        {
            MyDataHandler dh = new MyDataHandler();
            TcpConnection c = new TcpConnection( dh, "127.0.0.1", port, 0 );

            Assert.IsFalse( c.IsStarted() );
            c.Start();
            Assert.IsTrue( c.IsStarted() );

            c.WaitUp( TIMEOUT );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, null );

            AssertWhat( What.UP, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );
        
            byte[] buf = new byte[] { 23 };
            t.Send( buf );

            AssertWhat( What.DATA, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNotNull( dh.xbuf );
            Assert.AreEqual( 1, dh.xbuf.Length );
            Assert.AreEqual( ( byte ) 23, dh.xbuf[ 0 ] );

            Assert.IsTrue( c.IsStarted() );
            c.Stop();
            Assert.IsFalse( c.IsStarted() );

            AssertWhat( What.DOWN, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            t.Close();

        }

        [Test]
        public void Data2()
        {
            MyDataHandler dh = new MyDataHandler();
            TcpConnection c = new TcpConnection( dh, "127.0.0.1", port, 0 );

            Assert.IsFalse( c.IsStarted() );
            c.Start();
            Assert.IsTrue( c.IsStarted() );

            c.WaitUp( TIMEOUT );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, null );

            AssertWhat( What.UP, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            byte[] buf = { 21, 22, 23 };

            t.Send( buf );
            // t.getOutputStream().flush();

            AssertWhat( What.DATA, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNotNull( dh.xbuf );
            Assert.AreEqual( 3, dh.xbuf.Length );
            Assert.AreEqual( ( byte ) 21, dh.xbuf[ 0 ] );
            Assert.AreEqual( ( byte ) 22, dh.xbuf[ 1 ] );
            Assert.AreEqual( ( byte ) 23, dh.xbuf[ 2 ] );

            Assert.IsTrue( c.IsStarted() );
            c.Stop();
            Assert.IsFalse( c.IsStarted() );

            AssertWhat( What.DOWN, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            t.Close();
        }

        [Test]
        public void Data3()
        {
            MyDataHandler dh = new MyDataHandler();
            TcpConnection c = new TcpConnection( dh, "127.0.0.1", port, 0 );

            Assert.IsFalse( c.IsStarted() );
            c.Start();
            Assert.IsTrue( c.IsStarted() );

            c.WaitUp( TIMEOUT );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, null );

            AssertWhat( What.UP, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            t.Shutdown( SocketShutdown.Send );

            AssertWhat( What.DOWN, dh.what );
            Assert.IsNull( dh.xsender );
            Assert.IsNull( dh.xbuf );

            t.Close();
            c.Close();
        }

        [Test]
        public void Data4()
        {
            MyDataHandler dh = new MyDataHandler();
		    TcpConnection c = new TcpConnection( dh, "127.0.0.1", port, 0 );

		    c.Start();

            c.WaitUp( TIMEOUT );

		    Socket t = lh.accepted.WaitUntilNotEqAndSet( null, null );
    		
		    AssertWhat( What.UP, dh.what );
		    Assert.IsNull( dh.xsender );
		    Assert.IsNull( dh.xbuf );
    		
            byte[] sendBuf = new byte[] { 21, 22, 23 };

		    c.Data( null, new FlexBuffer( sendBuf ) );
		    // c.flush();
    		
            byte[] receiveBuf = new byte[ 3 ];
            t.Receive( receiveBuf );

            Assert.AreEqual( sendBuf, receiveBuf );

            c.ShutdownOutput();
        		
		    // assertEquals( -1, is.read() );
    		
		    c.Close();
    		
		    AssertWhat( What.DOWN, dh.what );
		    Assert.IsNull( dh.xsender );
		    Assert.IsNull( dh.xbuf );
    		
		    t.Close();
        }

        #endregion


        private void AssertWhat( What what, Monitor<What> mon )
        {
            mon.WaitUntilEqAndSet( what, 4000, default( What) );
        }

        /**
        [Test]
        public void TestBasicConnection()
        {
            StartListener();
            StartConnection();

            Console.WriteLine("lh.what = " + lh.what);
            Assert.AreEqual(What.LISTENERHANDLERUP, lh.what );
            // Assert.AreEqual(What.SENDERCONNECTIONUP, stc.what);

            //l.WaitDown( 4000 );
            c.WaitDown( 4000 );

        }
        */
        public enum What { 
            // connection is up
            UP, 
            // connection is down
            DOWN, 
            // connection read some data
            DATA 
        };

        #region Filler classes

        public class MyListener : ListenerHandler
        {
            public Monitor<Socket> accepted = new Monitor<Socket>( "accepted" );

            public void Accepted( Socket s )
            {
                accepted.Set( s );
            }

            #region SourceHandler<Source> Members

            public Source GetSource()
            {
                // ignore
                return null;
            }

            public void SetSource( Source src )
            {
                // ignore
            }

            #endregion

            #region Session Members

            public object SessionQuery( object query )
            {
                // ignore
                return null;
            }

            public void SessionControl( object control, object value )
            {
                // ignore
            }

            public void SessionNotify( object eventObj )
            {
                // ignore
            }

            #endregion
        }

        public class MyDataHandler : DataHandler
        {
            // holds event type
            public Monitor<What> what = new Monitor<What>( "what" );

		    // sender of the event if DATA 
		    public Who xsender;

		    // data of the event if DATA 
		    public byte[] xbuf;
    		
            public void Data( Who sender, FlexBuffer buf ) 
		    {
			    xsender = sender;
			    xbuf = buf.GetAvailBytes();
                what.Set( What.DATA );
		    }

            #region SourceHandler<Source> Members

            public Source GetSource()
            {
                // ignore
                return null;
            }

            public void SetSource( Source src )
            {
                // ignore
            }

            #endregion

            #region Session Members

            public object SessionQuery( object query )
            {
                throw new Exception( " Unsupported query " + query );
            }

            public void SessionControl( object control, object value )
            {
                throw new Exception( " Unsupported control " + control );
            }

            public void SessionNotify( object eventObj )
            {
                if ( eventObj.Equals( SessionConsts.UP ) )
                {
                    xbuf = null;
                    what.Set( What.UP );
                    return;
                }

                if ( eventObj.Equals( SessionConsts.DOWN ) )
                {
                    xbuf = null;
                    what.Set(What.DOWN);
                    return;
                }
            }

            #endregion
        }
        #endregion
    }
}
