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
using System.Net;
using System.Net.Sockets;
using System.Threading;
using Org.Apache.Etch.Bindings.Csharp.Util;
using NUnit.Framework;
using TcpListener=Org.Apache.Etch.Bindings.Csharp.Util.TcpListener;

namespace Org.Apache.Etch.Bindings.Csharp.Transport
{       
    [TestFixture]
    public class TestTcpConnection
    {
        public static MyListener lh = new MyListener();
        public static Connection<SessionListener<Socket>> l;
        public TcpConnection c;
        private const int TIMEOUT = 4000;
        private static int port;

        [TestFixtureSetUp]
        public void StartListener()
        {
            Console.WriteLine();
            Console.Write( "TestTCPConnection" );

            l = new TcpListener( "tcp:", null );
            l.SetSession(lh);
            l.Start();
            l.WaitUp( TIMEOUT );
            port = ( ( IPEndPoint ) l.LocalAddress() ).Port;
            //Console.WriteLine( " port = " + port );
        }

        [TestFixtureTearDown]
        public void StopListener()
        {
            l.Stop();
            l.WaitDown( TIMEOUT );
        }

        #region Constructor tests

        [Test]
        [ExpectedException(typeof(ArgumentNullException))]
        public void Constructor1()
        {
            new TcpConnection( null, "tcp:", null );
        }

        [TearDown]
        public void SleepAWhile()
        {
            Thread.Sleep( 1000 );
        }

        [Test]
        public void Constructor2()
        {
            IPEndPoint ipe = new IPEndPoint( IPAddress.Parse( "127.0.0.1" ), port );
            Socket s = new Socket( ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp );
            s.Connect( ipe );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, TIMEOUT, null );

            TcpConnection c = new TcpConnection( s, "tcp:",null );
            Assert.IsNull( c.GetSession() );

            s.Close();
            t.Close();
        }

        [Test]
        public void Constructor3()
        {
            IPEndPoint ipe = new IPEndPoint( IPAddress.Parse( "127.0.0.1" ), port );
            Socket s = new Socket( ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp );
            s.Connect( ipe );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, TIMEOUT, null );

            TcpConnection c = new TcpConnection(s, "tcp:", null);
            SessionData dh = new MyDataHandler(c);
            Assert.AreEqual( dh, c.GetSession() );

            s.Close();
            t.Close();
        }

        [Test]
        [ExpectedException(typeof(ArgumentNullException))]
        public void Constructor4()
        {
            new TcpConnection( null, (string) null, null );
        }

        [Test]
        [ExpectedException(typeof(ArgumentNullException))]
        public void Constructor5()
        {
            new TcpConnection( null, (string) null,null );
        }

        [Test]
        public void Constructor8()
        {
            TcpConnection c = new TcpConnection( null,"tcp://localhost:"+port, null );
            Assert.IsNull( c.GetSession() );
        }

        [Test]
        public void Constructor9()
        {

            TcpConnection c = new TcpConnection(null, "tcp://localhost:" + port, null);
            SessionData dh = new MyDataHandler(c);
            Assert.AreEqual( dh, c.GetSession() );
        }

        #endregion

        #region Start variations

        [Test]
        public void Start1()
        {
            IPEndPoint ipe = new IPEndPoint( IPAddress.Parse( "127.0.0.1" ), port );
            Socket s = new Socket( ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp );
            s.Connect( ipe );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, TIMEOUT, null );

           
            TcpConnection c = new TcpConnection( s,"tcp:",null );
            MyDataHandler dh = new MyDataHandler(c);

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

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, TIMEOUT, null );

            
            TcpConnection c = new TcpConnection(s, "tcp:", null );
            MyDataHandler dh = new MyDataHandler(c);

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

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, TIMEOUT, null );

           
            TcpConnection c = new TcpConnection( s, "tcp:", null );
            MyDataHandler dh = new MyDataHandler(c);

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
            
            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, TIMEOUT, null );

          
            TcpConnection c = new TcpConnection( s, "tcp:", null );
            MyDataHandler dh = new MyDataHandler(c);

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
            IPEndPoint ipe = new IPEndPoint(IPAddress.Parse("127.0.0.1"), port);
            Socket s = new Socket(ipe.AddressFamily, SocketType.Stream, ProtocolType.Tcp);
            s.Connect(ipe);

            Socket t = lh.accepted.WaitUntilNotEqAndSet(null, TIMEOUT, null);

            TcpConnection c = new TcpConnection(s, "tcp:", null);
            MyDataHandler dh = new MyDataHandler(c);

            Assert.IsFalse(c.IsStarted());
            c.Start();
            Assert.IsTrue(c.IsStarted());

            c.WaitUp(TIMEOUT);

            AssertWhat(What.UP, dh.what);
            Assert.IsNull(dh.xsender);
            Assert.IsNull(dh.xbuf);

            Assert.IsTrue(c.IsStarted());
            c.Close(true);

            c.WaitDown(TIMEOUT);

            AssertWhat(What.DOWN, dh.what);
            Assert.IsNull(dh.xsender);
            Assert.IsNull(dh.xbuf);

            t.Close();
        }

        [Test]
        public void Start6()
        {

            TcpConnection c = new TcpConnection(null, "tcp://127.0.0.1:" + port, null);
            MyDataHandler dh = new MyDataHandler(c);

            Assert.IsFalse( c.IsStarted() );
            c.Start();
            Assert.IsTrue( c.IsStarted() );
            c.WaitUp( TIMEOUT );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, TIMEOUT, null );

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
        
        // Cause BuildServer to hang, hence ignored
        [Ignore]
	    [Test]
	    public void restart1() 
	    {



            TcpConnection c = new TcpConnection(null, "tcp://127.0.0.1:" + port + "?TcpTransport.reconnectDelay=200", null);
            MyDataHandler dh = new MyDataHandler(c);
		
		    c.Start();
		    c.WaitUp( TIMEOUT );
		
		    int n = 10;
		    int i = 0;
		    while (++i <= n)
		    {
			    Socket t = lh.accepted.WaitUntilNotEqAndSet( null, TIMEOUT, null );

			    Console.WriteLine(String.Format( "accepted i {0} t.local {1} t.remote {2}",  i,
				     t.LocalEndPoint, t.RemoteEndPoint ));
			
			AssertWhat( What.UP, dh.what );
			
			Assert.AreEqual( t.LocalEndPoint, c.RemoteAddress() );
			Assert.AreEqual( t.RemoteEndPoint, c.LocalAddress() );
            byte[] data = new byte[] {Convert.ToByte(i)};
		        
		       t.Send(data);
	//		t.getOutputStream().write( i );
			AssertWhat( What.DATA, dh.what );
			Assert.IsNull( dh.xsender );
			Assert.IsNotNull( dh.xbuf );
			Assert.AreEqual( 1, dh.xbuf.Length );
			Assert.AreEqual( (byte) i, dh.xbuf[0] );

		//	t.shutdownOutput();
                t.Shutdown(SocketShutdown.Send);
			t.Close();
			t = null;
			
			AssertWhat( What.DOWN, dh.what );
		}

        Thread t1 = new Thread(new ThreadStart(RunMonitorSocketThread));

        Thread t2 = new Thread(new ThreadStart(delegate()
                                                   {

                                                       try
                                                       {
                                                           Object t;
                                                           Monitor<What> mon = dh.what;
                                                           while ((t = mon.WaitUntilNotEqAndSet(default(What), TIMEOUT, default(What))) != null) { }
                                                           //	Log.report( "harvested", "mon", mon, "t", t );
                                                       }
                                                       catch (Exception)
                                                       {
                                                           // ignore
                                                       }    
            


                                                   }));
		
	//	Thread t1 = new MonitorHarvester<Socket>( lh.accepted );
		t1.Start();
	//	Thread t2 = new MonitorHarvester<What>( dh.what );
		t2.Start();

		Console.WriteLine( "stopping" );
		c.Stop();
		c.WaitDown( TIMEOUT );
		Console.WriteLine( "stoppped" );
		
		t1.Join();
		t2.Join();
	}

        private void RunMonitorSocketThread()
        {
            try
            {
                Object t;
                Monitor<Socket> mon = lh.accepted;
                while ((t = mon.WaitUntilNotEqAndSet(null, TIMEOUT, null)) != null)
                {
                    //	Log.report( "harvested", "mon", mon, "t", t );   
                }

            }
            catch (Exception)
            {
                // ignore
            }    
        }

       
	/**
	 * @param <T>
	 */
	    public class MonitorHarvester<T> 
	{
		/**
		 * @param mon
		 */
		public MonitorHarvester( Monitor<T> mon )
		{
			this.mon = mon;
           
		}
		
		private readonly Monitor<T> mon;

	
	}
    // Cause BuildServer to hang, hence ignored
    [Ignore]
	[Test]
	public void restart2() 
	{



        TcpConnection c = new TcpConnection(null, "tcp://127.0.0.1:" + port + "?TcpTransport.reconnectDelay=200", null);
        MyDataHandler dh = new MyDataHandler(c);
		
		c.Start();
		c.WaitUp( TIMEOUT );
		
		int n = 10;
		int i = 0;
		while (++i <= n)
		{
//			Log.report( "--- waitAccepted" );
			Socket t = lh.accepted.WaitUntilNotEqAndSet( null, TIMEOUT, null );

            Console.WriteLine(String.Format("accepted i {0} t.local {1} t.remote {2}", i,
                        t.LocalEndPoint, t.RemoteEndPoint));
			
			AssertWhat( What.UP, dh.what );
//			Log.report( "up", "c.local", c.localAddress(), "c.remote", c.remoteAddress() );
			
			Assert.AreEqual( t.LocalEndPoint, c.RemoteAddress() );
			Assert.AreEqual( t.RemoteEndPoint, c.LocalAddress() );

            byte[] data = new byte[] { Convert.ToByte(i) };

            t.Send(data);
			AssertWhat( What.DATA, dh.what );
			Assert.IsNull( dh.xsender );
			Assert.IsNotNull( dh.xbuf );
			Assert.AreEqual( 1, dh.xbuf.Length );
			Assert.AreEqual( (byte) i, dh.xbuf[0] );

//			Log.report( "c.shutingDownOutput" );
			c.ShutdownOutput();
//			Log.report( "c.shutdownOutput" );
			
//			Log.report( "t.reading" );
			Assert.AreEqual( 0, t.Receive(data) );
//			Log.report( "t.eof" );
			
			t.Shutdown(SocketShutdown.Send);
			t.Close();
			t = null;
			
			AssertWhat( What.DOWN, dh.what );
//			Log.report( "down" );
		}

        Thread t1 = new Thread(new ThreadStart(RunMonitorSocketThread));
		t1.Start();
        Thread t2 = new Thread(new ThreadStart(delegate()
                                                  {

                                                      try
                                                      {
                                                          Object t;
                                                          Monitor<What> mon = dh.what;
                                                          while ((t = mon.WaitUntilNotEqAndSet(default(What), TIMEOUT, default(What))) != null) { }
                                                          //	Log.report( "harvested", "mon", mon, "t", t );
                                                      }
                                                      catch (Exception)
                                                      {
                                                          // ignore
                                                      }



                                                  }));
		t2.Start();
		
		Console.WriteLine( "stopping" );
		c.Stop();
		c.WaitDown( TIMEOUT );
		Console.WriteLine( "stopped" );
		
		t1.Join();
		t2.Join();
	} 

        #region Data tests

        [Test]
        public void Data1()
        {

            TcpConnection c = new TcpConnection(null, "tcp://127.0.0.1:" + port, null);
            MyDataHandler dh = new MyDataHandler(c);

            Assert.IsFalse( c.IsStarted() );
            c.Start();
            Assert.IsTrue( c.IsStarted() );

            c.WaitUp( TIMEOUT );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, TIMEOUT, null );

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

            TcpConnection c = new TcpConnection(null, "tcp://127.0.0.1:" + port, null);
            MyDataHandler dh = new MyDataHandler(c);

            Assert.IsFalse( c.IsStarted() );
            c.Start();
            Assert.IsTrue( c.IsStarted() );

            c.WaitUp( TIMEOUT );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null,TIMEOUT, null );

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

            TcpConnection c = new TcpConnection(null, "tcp://127.0.0.1:" + port, null);
            MyDataHandler dh = new MyDataHandler(c);

            Assert.IsFalse( c.IsStarted() );
            c.Start();
            Assert.IsTrue( c.IsStarted() );

            c.WaitUp( TIMEOUT );

            Socket t = lh.accepted.WaitUntilNotEqAndSet( null, TIMEOUT, null );

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

            TcpConnection c = new TcpConnection(null, "tcp://127.0.0.1:" + port, null);
            MyDataHandler dh = new MyDataHandler(c);

		    c.Start();

            c.WaitUp( TIMEOUT );

		    Socket t = lh.accepted.WaitUntilNotEqAndSet( null,TIMEOUT, null );
    		
		    AssertWhat( What.UP, dh.what );
		    Assert.IsNull( dh.xsender );
		    Assert.IsNull( dh.xbuf );
    		
            byte[] sendBuf = new byte[] { 21, 22, 23 };

	//	    c.Data( null, new FlexBuffer( sendBuf ) );
            c.TransportData(null, new FlexBuffer(sendBuf));
		    // c.flush();
    		
            byte[] receiveBuf = new byte[ 3 ];
            t.Receive( receiveBuf );

            Assert.AreEqual( sendBuf, receiveBuf );

            c.ShutdownOutput();
        		
		    // Assert.AreEqual( -1, is.read() );
    		
		    c.Close();
    		
		    AssertWhat( What.DOWN, dh.what );
		    Assert.IsNull( dh.xsender );
		    Assert.IsNull( dh.xbuf );
    		
		    t.Close();
        }

        #endregion


        private void AssertWhat( What what, Monitor<What> mon )
        {
            mon.WaitUntilEqAndSet( what, TIMEOUT, default( What) );
        }

        public enum What { 
            // connection is up
            UP, 
            // connection is down
            DOWN, 
            // connection read some data
            DATA 
        };

        #region Filler classes

        public class MyListener : SessionListener<Socket>
        {
            public Monitor<Socket> accepted = new Monitor<Socket>( "accepted" );

            public void Accepted( Socket s )
            {
              //  accepted.Set( s );
                accepted.WaitUntilEqAndSet(null, TIMEOUT, s);
            }

           
            #endregion

            #region Session Members

            public object SessionQuery( object query )
            {
                // ignore
               throw new NotSupportedException("unknown query: " + query);
            }

            public void SessionControl( object control, object value )
            {
                // ignore
                throw new NotSupportedException("unknown control:  " + control);
            }

            public void SessionNotify( object eventObj )
            {
                // ignore
                if (eventObj is Exception)
                    Console.WriteLine("MyListener.SessionNotify: " + eventObj);
            }

            #endregion

            #region SessionListener Members

            public void SessionAccepted(Socket socket)
            {
                  accepted.WaitUntilEqAndSet(null, TIMEOUT, socket);
            }

            #endregion
        }

        public class MyDataHandler : SessionData
        {
            // holds event type
            public Monitor<What> what = new Monitor<What>( "what" );

		    // sender of the event if DATA 
		    public Who xsender;

		    // data of the event if DATA 
		    public byte[] xbuf;
    		
            //public void Data( Who sender, FlexBuffer buf ) 
            //{
            //    xsender = sender;
            //    xbuf = buf.GetAvailBytes();
           
            //    what.WaitUntilEqAndSet(default(What), TIMEOUT, What.DATA);
            //}
            public MyDataHandler(TcpConnection conn)
            {
                conn.SetSession(this);
            }
           

            #region Session Members

            public object SessionQuery( object query )
            {
                throw new NotSupportedException( " Unsupported query " + query );
            }

            public void SessionControl( object control, object value )
            {
                throw new NotSupportedException( " Unsupported control " + control );
            }

            public void SessionNotify( object eventObj )
            {
                if ( eventObj.Equals( SessionConsts.UP ) )
                {
                    xbuf = null;
                //    what.Set( What.UP );
                    what.WaitUntilEqAndSet(default(What), TIMEOUT, What.UP);
                    return;
                }

                if ( eventObj.Equals( SessionConsts.DOWN ) )
                {
                    xbuf = null;
                    what.WaitUntilEqAndSet(default(What), TIMEOUT, What.DOWN);
                    return;
                }
            }

            #endregion

            #region SessionData Members

            public void SessionData(Who sender, FlexBuffer buf)
            {
                xsender = sender;
                xbuf = buf.GetAvailBytes();

                what.WaitUntilEqAndSet(default(What), TIMEOUT, What.DATA);
            }

            #endregion
        }
       
    }
}
