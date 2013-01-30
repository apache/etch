/* $Id$
 *
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements. See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership. The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License. You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied. See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

package org.apache.etch.util.core.nio;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import java.io.IOException;
import java.net.InetSocketAddress;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;

import org.apache.etch.util.FlexBuffer;
import org.apache.etch.util.Log;
import org.apache.etch.util.Monitor;
import org.apache.etch.util.Resources;
import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.Session;
import org.apache.etch.util.core.io.SessionData;
import org.apache.etch.util.core.io.SessionListener;
import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;


/**
 * Tests Tcp2Connection.
 */
public class TestTcp2Connection
{
	private final static MyListener lh = new MyListener();

	private final static int TIMEOUT = 4000;
	
	private final static SuperSelector selector = new SuperSelector();
	static
	{
		selector.start();
	}
	
	private final static ByteBufferPool pool = new ByteBufferPool(1024, 10, 1000, 0, 0 );
	
	private final static Resources r = new Resources();
	static
	{
		r.put( "bufferPool", pool );
		r.put( "selector", selector );
	}

	private final static Tcp2Listener l = new Tcp2Listener( "tcp://0.0.0.0:0", r );
	
	private static int port;

	/** @throws Exception */
	@BeforeClass
	public static void startListener() throws Exception
	{
		l.setSession( lh );
		
		// start listener
		l.start();
		l.waitUp( TIMEOUT );
		
		port = ((InetSocketAddress) l.localAddress()).getPort();
		
		Log.addSink( null );
		Log.report( "TestTcp2Connection" );
	}
	
	/** @throws Exception */
	@AfterClass
	public static void stopListener() throws Exception
	{
		l.stop();
		l.waitDown( TIMEOUT );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor1() throws Exception
	{
		// no host or port
		new Tcp2Connection( "tcp:", r );
	}

	/** @throws Exception */
	@Test
	public void constructor2() throws Exception
	{
		SocketChannel s = newSocketChannel( "127.0.0.1", port );
		SocketChannel t = lh.accepted.waitUntilNotEqAndSet( null, TIMEOUT, null );
		
		r.put( "connection", s );
		Tcp2Connection c = new Tcp2Connection( "tcp:", r );
		assertNull( c.getSession() );
		
		s.close();
		t.close();
	}

	private static SocketChannel newSocketChannel( String host, int port )
		throws IOException
	{
		return SocketChannel.open( new InetSocketAddress( host, port )  );
	}

	/** @throws Exception */
	@Test
	public void constructor3() throws Exception
	{
		SessionData dh = new MyDataHandler();
		SocketChannel s = newSocketChannel( "127.0.0.1", port );
		SocketChannel t = lh.accepted.waitUntilNotEqAndSet( null, TIMEOUT, null );

		r.put( "connection", s );
		Tcp2Connection c = new Tcp2Connection( "tcp:", r );
		c.setSession( dh );
		assertSame( dh, c.getSession() );
		
		s.close();
		t.close();
	}

//	/** @throws Exception */
//	@Test( expected = IllegalArgumentException.class )
//	public void constructor4() throws Exception
//	{
//		// null host
//		new Tcp2Connection( "tcp://0.0.0.0:1", null );
//	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor5a() throws Exception
	{
		// no port
		new Tcp2Connection( "tcp://localhost", r );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor5b() throws Exception
	{
		// zero port
		new Tcp2Connection( "tcp://localhost:0", r );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor6() throws Exception
	{
		// too big port
		new Tcp2Connection( "tcp://localhost:65536", r );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor7() throws Exception
	{
		// negative reconnect delay
		new Tcp2Connection( "tcp://localhost:1?TcpTransport.reconnectDelay=-1", r );
	}

	/** @throws Exception */
	@Test
	public void constructor8() throws Exception
	{
		Tcp2Connection c = new Tcp2Connection( "tcp://localhost:"+port, r );
		assertNull( c.getSession() );
	}

	/** @throws Exception */
	@Test
	public void constructor9() throws Exception
	{
		SessionData dh = new MyDataHandler();
		Tcp2Connection c = new Tcp2Connection( "tcp://localhost:"+port, r );
		c.setSession( dh );
		assertSame( dh, c.getSession() );
	}

	/** @throws Exception */
	@Test
	public void start1() throws Exception
	{
		SocketChannel s = newSocketChannel( "127.0.0.1", port );
		SocketChannel t = lh.accepted.waitUntilNotEqAndSet( null, TIMEOUT, null );
		
		MyDataHandler dh = new MyDataHandler();
		
		r.put( "connection", s );
		Tcp2Connection c = new Tcp2Connection( "tcp:", r );
		c.setSession( dh );

		assertFalse( c.isStarted() );
		c.start();
		assertTrue( c.isStarted() );
		
		c.waitUp( TIMEOUT );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
//		c.flush();
		assertNotNull( c.remoteAddress() );
		assertNotNull( c.localAddress() );
		assertEquals( c.remoteAddress(), t.socket().getLocalSocketAddress() );
		assertEquals( c.localAddress(), t.socket().getRemoteSocketAddress() );
		
		t.close();
		
		assertWhat( What.DOWN, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
	}

	/** @throws Exception */
	@Test
	public void start2() throws Exception
	{
		SocketChannel s = newSocketChannel( "127.0.0.1", port );
		SocketChannel t = lh.accepted.waitUntilNotEqAndSet( null, TIMEOUT, null );
		
		MyDataHandler dh = new MyDataHandler();

		r.put( "connection", s );
		Tcp2Connection c = new Tcp2Connection( "tcp:", r );
		c.setSession( dh );
		
		assertFalse( c.isStarted() );
		c.start();
		assertTrue( c.isStarted() );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		assertTrue( c.isStarted() );
		c.stop();
		assertFalse( c.isStarted() );
		
		c.waitDown( TIMEOUT );
		
		assertWhat( What.DOWN, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		t.close();
	}

	/** @throws Exception */
	@Test
	public void start3() throws Exception
	{
		SocketChannel s = newSocketChannel( "127.0.0.1", port );
		SocketChannel t = lh.accepted.waitUntilNotEqAndSet( null, TIMEOUT, null );
		
		MyDataHandler dh = new MyDataHandler();

		r.put( "connection", s );
		Tcp2Connection c = new Tcp2Connection( "tcp:", r );
		c.setSession( dh );
		
		assertFalse( c.isStarted() );
		c.start();
		assertTrue( c.isStarted() );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		assertTrue( c.isStarted() );
		c.close();
		
		c.waitDown( TIMEOUT );
		
		assertWhat( What.DOWN, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		t.close();
	}

	/** @throws Exception */
	@Test
	public void start4() throws Exception
	{
		SocketChannel s = newSocketChannel( "127.0.0.1", port );
		SocketChannel t = lh.accepted.waitUntilNotEqAndSet( null, TIMEOUT, null );
		
		MyDataHandler dh = new MyDataHandler();

		r.put( "connection", s );
		Tcp2Connection c = new Tcp2Connection( "tcp:", r );
		c.setSession( dh );
		
		assertFalse( c.isStarted() );
		c.start();
		assertTrue( c.isStarted() );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		assertTrue( c.isStarted() );
		c.close( false );
		
		c.waitDown( TIMEOUT );
		
		assertWhat( What.DOWN, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		t.close();
	}

	/** @throws Exception */
	@Test
	public void start5() throws Exception
	{
		SocketChannel s = newSocketChannel( "127.0.0.1", port );
		SocketChannel t = lh.accepted.waitUntilNotEqAndSet( null, TIMEOUT, null );
		
		MyDataHandler dh = new MyDataHandler();

		r.put( "connection", s );
		Tcp2Connection c = new Tcp2Connection( "tcp:", r );
		c.setSession( dh );
		
		assertFalse( c.isStarted() );
		c.start();
		assertTrue( c.isStarted() );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		assertTrue( c.isStarted() );
		c.close( true );
		
		c.waitDown( TIMEOUT );
		
		assertWhat( What.DOWN, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		t.close();
	}

	/** @throws Exception */
	@Test
	public void start6() throws Exception
	{
		MyDataHandler dh = new MyDataHandler();
		
		Tcp2Connection c = new Tcp2Connection( "tcp://127.0.0.1:"+port, r );
		c.setSession( dh );
		
		assertFalse( c.isStarted() );
		c.start();
		assertTrue( c.isStarted() );
		
		SocketChannel t = lh.accepted.waitUntilNotEqAndSet( null, TIMEOUT, null );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		assertTrue( c.isStarted() );
		c.stop();
		assertFalse( c.isStarted() );
		
		assertWhat( What.DOWN, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		t.close();
	}
	
	/** @throws Exception */
	@Test
	public void restart1() throws Exception
	{
		Log.report( "--- restart1 ------------------------------" );
		
		MyDataHandler dh = new MyDataHandler();
		
		Tcp2Connection c = new Tcp2Connection( "tcp://127.0.0.1:"+port+"?TcpTransport.reconnectDelay=200", r );
		c.setSession( dh );
		
		c.start();
		c.waitUp( TIMEOUT );
		
		int n = 10;
		int i = 0;
		while (++i <= n)
		{
			SocketChannel t = lh.accepted.waitUntilNotEqAndSet( null, TIMEOUT, null );

			Log.report( "accepted", "i", i,
				"t.local", t.socket().getLocalSocketAddress(),
				"t.remote", t.socket().getRemoteSocketAddress() );
			
			assertWhat( What.UP, dh.what );
			
			assertEquals( t.socket().getLocalSocketAddress(), c.remoteAddress() );
			assertEquals( t.socket().getRemoteSocketAddress(), c.localAddress() );
			
			write( t, i );
			assertWhat( What.DATA, dh.what );
			assertNull( dh.xsender );
			assertNotNull( dh.xbuf );
			assertEquals( 1, dh.xbuf.length );
			assertEquals( (byte) i, dh.xbuf[0] );

			t.socket().shutdownOutput();
			t.close();
			t = null;
			
			assertWhat( What.DOWN, dh.what );
		}
		
		Thread t1 = new MonitorHarvester<SocketChannel>( lh.accepted );
		t1.start();
		Thread t2 = new MonitorHarvester<What>( dh.what );
		t2.start();

		Log.report( "stopping" );
		c.stop();
		c.waitDown( TIMEOUT );
		Log.report( "stoppped" );
		
		t1.join();
		t2.join();
	}
	
	private void write( SocketChannel t, int b ) throws IOException
	{
		ByteBuffer buf = ByteBuffer.allocate( 1 );
		buf.put( (byte) b );
		buf.flip();
		int n = t.write( buf );
		if (n != 1)
			throw new IOException( "n != 1" );
	}

	/**
	 * @param <T>
	 */
	public static class MonitorHarvester<T> extends Thread
	{
		/**
		 * @param mon
		 */
		public MonitorHarvester( Monitor<T> mon )
		{
			this.mon = mon;
		}
		
		private final Monitor<T> mon;

		@Override
		public void run()
		{
			try
			{
				Object t;
				while ((t = mon.waitUntilNotEqAndSet( null, TIMEOUT, null )) != null)
					Log.report( "harvested", "mon", mon, "t", t );
			}
			catch ( Exception e )
			{
				// ignore
			}
		}
	}
	
	/** @throws Exception */
	@Test
	public void restart2() throws Exception
	{
		Log.report( "--- restart2 ------------------------------" );
		
		MyDataHandler dh = new MyDataHandler();
		
		Tcp2Connection c = new Tcp2Connection( "tcp://127.0.0.1:"+port+"?TcpTransport.reconnectDelay=200", r );
		c.setSession( dh );
		
		c.start();
		c.waitUp( TIMEOUT );
		
		int n = 10;
		int i = 0;
		while (++i <= n)
		{
//			Log.report( "--- waitAccepted" );
			SocketChannel t = lh.accepted.waitUntilNotEqAndSet( null, TIMEOUT, null );

			Log.report( "accepted", "i", i,
				"t.local", t.socket().getLocalSocketAddress(),
				"t.remote", t.socket().getRemoteSocketAddress() );
			
			assertWhat( What.UP, dh.what );
//			Log.report( "up", "c.local", c.localAddress(), "c.remote", c.remoteAddress() );
			
			assertEquals( t.socket().getLocalSocketAddress(), c.remoteAddress() );
			assertEquals( t.socket().getRemoteSocketAddress(), c.localAddress() );
			
			write( t, i );
			assertWhat( What.DATA, dh.what );
			assertNull( dh.xsender );
			assertNotNull( dh.xbuf );
			assertEquals( 1, dh.xbuf.length );
			assertEquals( (byte) i, dh.xbuf[0] );

//			Log.report( "c.shutingDownOutput" );
			c.shutdownOutput();
//			Log.report( "c.shutdownOutput" );
			
//			Log.report( "t.reading" );
			assertEquals( -1, read( t ) );
//			Log.report( "t.eof" );
			
			t.socket().shutdownOutput();
			t.close();
			t = null;
			
			assertWhat( What.DOWN, dh.what );
//			Log.report( "down" );
		}
		
		Thread t1 = new MonitorHarvester<SocketChannel>( lh.accepted );
		t1.start();
		Thread t2 = new MonitorHarvester<What>( dh.what );
		t2.start();
		
		Log.report( "stopping" );
		c.stop();
		c.waitDown( TIMEOUT );
		Log.report( "stopped" );
		
		t1.join();
		t2.join();
	}

	private int read( SocketChannel t ) throws IOException
	{
		ByteBuffer buf = ByteBuffer.allocate( 1 );
		int n = t.read( buf );
		if (n <= 0)
			return -1;
		buf.flip();
		return buf.get() & 255;
	}

	/** @throws Exception */
	@Test
	public void data1() throws Exception
	{
		MyDataHandler dh = new MyDataHandler();
		
		Tcp2Connection c = new Tcp2Connection( "tcp://127.0.0.1:"+port, r );
		c.setSession( dh );
		
		assertFalse( c.isStarted() );
		c.start();
		assertTrue( c.isStarted() );
		
		SocketChannel t = lh.accepted.waitUntilNotEqAndSet( null, TIMEOUT, null );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		write( t, 23 );
		
		assertWhat( What.DATA, dh.what );
		assertNull( dh.xsender );
		assertNotNull( dh.xbuf );
		assertEquals( 1, dh.xbuf.length );
		assertEquals( (byte) 23, dh.xbuf[0] );
		
		assertTrue( c.isStarted() );
		c.stop();
		assertFalse( c.isStarted() );
		
		assertWhat( What.DOWN, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		t.close();
	}

	/** @throws Exception */
	@Test
	public void data2() throws Exception
	{
		MyDataHandler dh = new MyDataHandler();
		
		Tcp2Connection c = new Tcp2Connection( "tcp://127.0.0.1:"+port, r );
		c.setSession( dh );
		
		assertFalse( c.isStarted() );
		c.start();
		assertTrue( c.isStarted() );
		
		SocketChannel t = lh.accepted.waitUntilNotEqAndSet( null, TIMEOUT, null );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		byte[] buf = { 21, 22, 23 };
		
		write( t, buf );
		
		assertWhat( What.DATA, dh.what );
		assertNull( dh.xsender );
		assertNotNull( dh.xbuf );
		assertEquals( 3, dh.xbuf.length );
		assertEquals( (byte) 21, dh.xbuf[0] );
		assertEquals( (byte) 22, dh.xbuf[1] );
		assertEquals( (byte) 23, dh.xbuf[2] );
		
		assertTrue( c.isStarted() );
		c.stop();
		assertFalse( c.isStarted() );
		
		assertWhat( What.DOWN, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		t.close();
	}

	private void write( SocketChannel t, byte[] b ) throws IOException
	{
		ByteBuffer buf = ByteBuffer.allocate( b.length );
		buf.put( b );
		buf.flip();
		int n = t.write( buf );
		if (n != b.length)
			throw new IOException( "n != b.length" );
	}

	/** @throws Exception */
	@Test
	public void data3() throws Exception
	{
		MyDataHandler dh = new MyDataHandler();
		
		Tcp2Connection c = new Tcp2Connection( "tcp://127.0.0.1:"+port, r );
		c.setSession( dh );
		
		assertFalse( c.isStarted() );
		c.start();
		assertTrue( c.isStarted() );
		
		SocketChannel t = lh.accepted.waitUntilNotEqAndSet( null, TIMEOUT, null );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		t.socket().shutdownOutput();
		
		assertWhat( What.DOWN, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		t.close();
		c.close();
	}

	/** @throws Exception */
	@Test
	public void data4() throws Exception
	{
		MyDataHandler dh = new MyDataHandler();
		
		Tcp2Connection c = new Tcp2Connection( "tcp://127.0.0.1:"+port, r );
		c.setSession( dh );

		c.start();
		
		SocketChannel t = lh.accepted.waitUntilNotEqAndSet( null, TIMEOUT, null );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		c.transportData( null, new FlexBuffer( new byte[] { 21, 22, 23 } ) );
//		c.flush();
		
		assertEquals( 21, read( t ) );
		assertEquals( 22, read( t ) );
		assertEquals( 23, read( t ) );
		
		c.shutdownOutput();
		
		assertEquals( -1, read( t ) );
		
		c.close();
		
		assertWhat( What.DOWN, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		t.close();
	}
	
	private void assertWhat( What what, Monitor<What> mon )
		throws Exception
	{
		mon.waitUntilEqAndSet( what, TIMEOUT, null );
	}
	
	/**
	 * Test events.
	 */
	enum What
	{
		/** connection is up */
		UP,
		/** connection is down */
		DOWN,
		/** connection read some data */
		DATA
	}

	/**
	 * ListenerHandler for testing.
	 */
	static class MyListener implements SessionListener<SocketChannel>
	{
		/** monitor to report accepted socket */
		public Monitor<SocketChannel> accepted = new Monitor<SocketChannel>( "accepted" );
		
		public void sessionAccepted( SocketChannel s ) throws Exception
		{
//			Log.report( "listener.accepted",
//				"s.local", s.getLocalSocketAddress(),
//				"s.remote", s.getRemoteSocketAddress() );
			
			accepted.waitUntilEqAndSet( null, TIMEOUT, s );
		}

		public Object sessionQuery( Object query )
		{
			throw new UnsupportedOperationException( "unknown query "+query );
		}

		public void sessionControl( Object control, Object value )
		{
			throw new UnsupportedOperationException( "unknown control "+control );
		}

		public void sessionNotify( Object event )
		{
			if (event instanceof Exception)
				Log.report( "listener.notify", "event", event );
			
			System.out.println( "TestTcp2Connection.MyListener: event = " + event );
		}
	}
	
	/**
	 * DataHandler for testing.
	 */
	static class MyDataHandler implements SessionData
	{
		/** holds event type */
		public Monitor<What> what = new Monitor<What>( "what" );

		/** sender of the event if DATA */
		public Who xsender;

		/** data of the event if DATA */
		public byte[] xbuf;
		
		public void sessionData( Who sender, FlexBuffer buf ) throws Exception
		{
			xsender = sender;
			xbuf = buf.getAvailBytes();
			what.waitUntilEqAndSet( null, TIMEOUT, What.DATA );
		}

		public Object sessionQuery( Object query ) throws Exception
		{
			throw new UnsupportedOperationException( "unknown query "+query );
		}

		public void sessionControl( Object control, Object value ) throws Exception
		{
			throw new UnsupportedOperationException( "unknown control "+control );
		}

		public void sessionNotify( Object event ) throws Exception
		{
//			Log.report( "data.notify", "event", event );
			
			if (event.equals( Session.UP ))
			{
				xbuf = null;
				what.waitUntilEqAndSet( null, TIMEOUT, What.UP );
				return;
			}
			
			if (event.equals( Session.DOWN ))
			{
				xbuf = null;
				what.waitUntilEqAndSet( null, TIMEOUT, What.DOWN );
				return;
			}
			
			Log.report( "data.notify", "event", event );
		}
	}
}
