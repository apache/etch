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
import java.net.Inet6Address;
import java.net.InetSocketAddress;
import java.net.Socket;
import java.net.SocketException;
import java.nio.ByteBuffer;
import java.nio.channels.SocketChannel;

import org.apache.etch.util.Monitor;
import org.apache.etch.util.Resources;
import org.apache.etch.util.core.io.Session;
import org.apache.etch.util.core.io.SessionListener;
import org.junit.Test;


/**
 * Tests Listener.
 */
public class TestTcp2Listener
{
	private final static int TIMEOUT = 4000;
	
	private final SuperSelector selector = new SuperSelector();
	{
		selector.start();
	}
	
	private final Resources r = new Resources();
	{
		r.put( "selector", selector );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor1() throws Exception
	{
		// negative port
		new Tcp2Listener( "tcp://0.0.0.0:-1", r );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor2() throws Exception
	{
		// too positive port
		new Tcp2Listener( "tcp://0.0.0.0:65536", r );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor3() throws Exception
	{
		// missing port
		new Tcp2Listener( "tcp://0.0.0.0", r );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor4() throws Exception
	{
		// negative backlog
		new Tcp2Listener( "tcp://0.0.0.0:0?backlog=-1", r );
	}

	/** @throws Exception */
	@Test
	public void constructor5() throws Exception
	{
		Tcp2Listener l = new Tcp2Listener( "tcp://0.0.0.0:45", r );
		assertEquals( "0.0.0.0", l.getHost() );
		assertEquals( 45, l.getPort() );
		assertEquals( 0, l.getBacklog() );
	}

	/** @throws Exception */
	@Test
	public void constructor6() throws Exception
	{
		Tcp2Listener l = new Tcp2Listener( "tcp://foo:2001?backlog=19", r );
		assertEquals( "foo", l.getHost() );
		assertEquals( 2001, l.getPort() );
		assertEquals( 19, l.getBacklog() );
	}

	/** @throws Exception */
	@Test
	public void session1() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();
		
		Tcp2Listener l = new Tcp2Listener( "tcp://0.0.0.0:0", r );
		l.setSession( lh );
		
		assertSame( lh, l.getSession() );
	}

	/** @throws Exception */
	@Test
	public void stop1() throws Exception
	{
		// illegal state: not started.
		Tcp2Listener l = new Tcp2Listener( "tcp://0.0.0.0:0", r );
		l.stop();
	}

	/** @throws Exception */
	@Test
	public void start1() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();
		
		Tcp2Listener l = new Tcp2Listener( "tcp://0.0.0.0:0", r );
		l.setSession( lh );
		
		assertWhat( null, lh.what );
		assertNull( lh.xsocket );

		l.start();
		l.waitUp( TIMEOUT );

		assertTrue( l.isStarted() );

		assertWhat( What.UP, lh.what );
		assertNull( lh.xsocket );

		InetSocketAddress a = (InetSocketAddress) l.localAddress();
		if ( a.getAddress() instanceof Inet6Address )
			assertEquals( "0:0:0:0:0:0:0:0", a.getAddress().getHostAddress().toString() );
		else
			assertEquals( "0.0.0.0", a.getAddress().getHostAddress().toString() );
		assertTrue( a.getPort() > 0 && a.getPort() < 65536 );

		l.stop();
		l.waitDown( TIMEOUT );

		assertFalse( l.isStarted() );

		assertWhat( What.DOWN, lh.what );
		assertNull( lh.xsocket );
	}

	/** @throws Exception */
	@Test
	public void start2() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();
		Tcp2Listener l = new Tcp2Listener( "tcp://127.0.0.1:0", r );
		l.setSession( lh );
		
		assertWhat( null, lh.what );
		assertNull( lh.xsocket );

		l.start();
		l.waitUp( TIMEOUT );

		assertTrue( l.isStarted() );

		assertWhat( What.UP, lh.what );
		assertNull( lh.xsocket );

		InetSocketAddress a = (InetSocketAddress) l.localAddress();
		assertEquals( "127.0.0.1", a.getAddress().getHostAddress().toString() );
		assertTrue( a.getPort() > 0 && a.getPort() < 65536 );

		l.stop();
		l.waitDown( TIMEOUT );
		assertFalse( l.isStarted() );
		assertWhat( What.DOWN, lh.what );
		assertNull( lh.xsocket );
	}

	/** @throws Exception */
	@Test
	public void start3() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();
		Tcp2Listener l = new Tcp2Listener( "tcp://0.0.0.0:4998", r );
		l.setSession( lh );

		assertWhat( null, lh.what );
		assertNull( lh.xsocket );

		l.start();
		l.waitUp( TIMEOUT );
		assertTrue( l.isStarted() );

		assertWhat( What.UP, lh.what );
		assertNull( lh.xsocket );

		InetSocketAddress a = (InetSocketAddress) l.localAddress();
		if ( a.getAddress() instanceof Inet6Address )
			assertEquals( "0:0:0:0:0:0:0:0", a.getAddress().getHostAddress().toString() );
		else
			assertEquals( "0.0.0.0", a.getAddress().getHostAddress().toString() );
		assertEquals( 4998, a.getPort() );

		l.stop();
		l.waitDown( TIMEOUT );

		assertFalse( l.isStarted() );

		assertWhat( What.DOWN, lh.what );
		assertNull( lh.xsocket );
	}

	/** @throws Exception */
	@Test
	public void start4() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();
		Tcp2Listener l = new Tcp2Listener( "tcp://127.0.0.1:4996", r );
		l.setSession( lh );

		assertWhat( null, lh.what );
		assertNull( lh.xsocket );

		l.start();
		l.waitUp( TIMEOUT );

		assertTrue( l.isStarted() );

		assertWhat( What.UP, lh.what );
		assertNull( lh.xsocket );

		InetSocketAddress a = (InetSocketAddress) l.localAddress();
		assertEquals( "127.0.0.1", a.getAddress().getHostAddress().toString() );
		assertEquals( 4996, a.getPort() );

		l.stop();
		l.waitDown( TIMEOUT );

		assertFalse( l.isStarted() );

		assertWhat( What.DOWN, lh.what );
		assertNull( lh.xsocket );
	}

	/** @throws Exception */
	@Test( expected = SocketException.class )
	public void start5() throws Exception
	{
		// bad address
		MyListenerHandler lh = new MyListenerHandler();
		Tcp2Listener l = new Tcp2Listener( "tcp://1.2.3.4.5:4997", r );
		l.setSession( lh );

		assertWhat( null, lh.what );
		assertNull( lh.xsocket );

		l.start();
	}

	/** @throws Exception */
	@Test
	public void accept1() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();
		Tcp2Listener l = new Tcp2Listener( "tcp://0.0.0.0:4995", r );
		l.setSession( lh );

		l.start();
		l.waitUp( TIMEOUT );

		Socket s = new Socket( "127.0.0.1", 4995 );
		assertWhat( What.ACCEPTED, lh.what );
		assertNotNull( lh.xsocket );

		// verify they are connected to each other

		assertEquals( s.getLocalSocketAddress(), lh.xsocket.socket().getRemoteSocketAddress() );
		assertEquals( s.getRemoteSocketAddress(), lh.xsocket.socket().getLocalSocketAddress() );

		// verify they can talk to each other

		s.getOutputStream().write( 23 );
		s.getOutputStream().flush();
		assertEquals( 23, read( lh.xsocket ) );

		s.getOutputStream().write( 67 );
		s.getOutputStream().flush();
		assertEquals( 67, read( lh.xsocket ) );

		write( lh.xsocket, 24 );
		assertEquals( 24, s.getInputStream().read() );

		write( lh.xsocket, 73 );
		assertEquals( 73, s.getInputStream().read() );

		// shut 'em down.

		s.close();
		lh.xsocket.close();

		l.stop();
		l.waitDown( TIMEOUT );
	}

	private void write( SocketChannel c, int b ) throws IOException
	{
		ByteBuffer buf = ByteBuffer.allocate( 1 );
		buf.put( (byte) b );
		buf.flip();
		int n = c.write( buf );
		if (n != 1)
			throw new IOException( "m != 1" );
	}

	private int read( SocketChannel c ) throws IOException
	{
		ByteBuffer buf = ByteBuffer.allocate( 1 );
		int n = c.read( buf );
		if (n <= 0)
			return -1;
		buf.flip();
		return buf.get() & 255;
	}

	/** @throws Exception */
	@Test
	public void accept2() throws Exception
	{
		MyOtherListenerHandler lh = new MyOtherListenerHandler();
		Tcp2Listener l = new Tcp2Listener( "tcp://0.0.0.0:0?TcpListener.backlog=100", r );
		l.setSession( lh );

		l.start();
		l.waitUp( TIMEOUT );

		int port = ((InetSocketAddress) l.localAddress()).getPort();

		int n = 1000;

		for (int i = 0; i < n; i++)
			new Socket( "127.0.0.1", port ).close();

		// wait for things to settle down...
		Thread.sleep( 100 );

		assertEquals( n, lh.accepted );

		l.stop();
		l.waitDown( TIMEOUT );
	}

	private void assertWhat( What what, Monitor<What> mon )
		throws Exception
	{
		mon.waitUntilEqAndSet( what, 4000, null );
	}

	/**
	 * Event types.
	 */
	enum What
	{
		/** the event seen was ACCEPTED */
		ACCEPTED,
		/** the event seen was UP */
		UP,
		/** the event seen was DOWN */
		DOWN
	}

	/**
	 * ListenerHandler for testing.
	 */
	public static class MyListenerHandler implements SessionListener<SocketChannel>
	{
		/** the event seen */
		public Monitor<What> what = new Monitor<What>( null );

		/** the socket accepted (if accept) */
		public SocketChannel xsocket;

		public void sessionAccepted( SocketChannel socket ) throws Exception
		{
			xsocket = socket;
			what.set( What.ACCEPTED );
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
			if (event.equals( Session.UP ))
			{
				what.set( What.UP );
				return;
			}
			
			if (event.equals( Session.DOWN ))
			{
				what.set( What.DOWN );
				return;
			}
		}
	}

	/**
	 * ListenerHandler for testing.
	 */
	public static class MyOtherListenerHandler implements SessionListener<SocketChannel>
	{
		/** count of accepted connections. */
		public int accepted;

		public void sessionAccepted( SocketChannel socket ) throws Exception
		{
			accepted++;
			socket.close();
		}

		public void sessionControl( Object control, Object value )
			throws Exception
		{
			// ignore
		}

		public void sessionNotify( Object event ) throws Exception
		{
			// ignore
		}

		public Object sessionQuery( Object query ) throws Exception
		{
			// ignore
			return null;
		}
	}
}
