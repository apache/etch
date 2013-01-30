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

package org.apache.etch.util.core.io;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import java.net.InetSocketAddress;
import java.net.Socket;

import org.apache.etch.util.Monitor;
import org.apache.etch.util.TimeoutException;
import org.junit.Test;


/**
 * Tests Listener.
 */
public class TestTcpListener
{
	private final static int TIMEOUT = 4000;

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor1() throws Exception
	{
		// negative port
		new TcpListener( "tcp://0.0.0.0:-1", null );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor2() throws Exception
	{
		// too positive port
		new TcpListener( "tcp://0.0.0.0:65536", null );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor3() throws Exception
	{
		// negative backlog
		new TcpListener( "tcp://0.0.0.0:0?TcpListener.backlog=-1", null );
	}

	/** @throws Exception */
	@Test
	public void constructor6() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();

		TcpListener l = new TcpListener( "tcp://0.0.0.0:0", null );
		l.setSession( lh );
		
		assertSame( lh, l.getSession() );
	}

	/** @throws Exception */
	@Test
	public void setHandler() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();

		TcpListener l = new TcpListener( "tcp://0.0.0.0:0", null );
		assertNull( l.getSession() );

		l.setSession( lh );
		assertSame( lh, l.getSession() );
	}

	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void stop1() throws Exception
	{
		TcpListener l = new TcpListener( "tcp://0.0.0.0:0", null );
		l.stop(); // illegal state: not started.
	}

	/** @throws Exception */
	@Test
	public void start1() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();
		
		TcpListener l = new TcpListener( "tcp://0.0.0.0:0", null );
		l.setSession( lh );
		
		assertWhat( null, lh.what );
		assertNull( lh.xsocket );

		l.start();
		l.waitUp( TIMEOUT );

		assertTrue( l.isStarted() );

		assertWhat( What.UP, lh.what );
		assertNull( lh.xsocket );

		InetSocketAddress a = (InetSocketAddress) l.localAddress();
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
		TcpListener l = new TcpListener( "tcp://127.0.0.1:0", null );
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
		TcpListener l = new TcpListener( "tcp://0.0.0.0:4009", null );
		l.setSession( lh );

		assertWhat( null, lh.what );
		assertNull( lh.xsocket );

		l.start();
		l.waitUp( TIMEOUT );
		assertTrue( l.isStarted() );

		assertWhat( What.UP, lh.what );
		assertNull( lh.xsocket );

		InetSocketAddress a = (InetSocketAddress) l.localAddress();
		assertEquals( "0.0.0.0", a.getAddress().getHostAddress().toString() );
		assertEquals( 4009, a.getPort() );

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
		TcpListener l = new TcpListener( "tcp://127.0.0.1:4006", null );
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
		assertEquals( 4006, a.getPort() );

		l.stop();
		l.waitDown( TIMEOUT );

		assertFalse( l.isStarted() );

		assertWhat( What.DOWN, lh.what );
		assertNull( lh.xsocket );
	}

	/** @throws Exception */
	@Test( expected = TimeoutException.class )
	public void start5() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();
		TcpListener l = new TcpListener( "tcp://1.2.3.4.5:4007", null );
		l.setSession( lh );

		assertWhat( null, lh.what );
		assertNull( lh.xsocket );

		l.start();
		l.waitUp( TIMEOUT );

		assertTrue( l.isStarted() );

		// this will timeout because of bad listening address.
	}

	/** @throws Exception */
	@Test
	public void accept1() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();
		TcpListener l = new TcpListener( "tcp://0.0.0.0:4008", null );
		l.setSession( lh );

		l.start();
		l.waitUp( TIMEOUT );

		Socket s = new Socket( "127.0.0.1", 4008 );
		assertWhat( What.ACCEPTED, lh.what );
		assertNotNull( lh.xsocket );

		// verify they are connected to each other

		assertEquals( s.getLocalSocketAddress(), lh.xsocket.getRemoteSocketAddress() );
		assertEquals( s.getRemoteSocketAddress(), lh.xsocket.getLocalSocketAddress() );

		// verify they can talk to each other

		s.getOutputStream().write( 23 );
		s.getOutputStream().flush();
		assertEquals( 23, lh.xsocket.getInputStream().read() );

		lh.xsocket.getOutputStream().write( 23 );
		lh.xsocket.getOutputStream().flush();
		assertEquals( 23, s.getInputStream().read() );

		// shut 'em down.

		s.close();
		lh.xsocket.close();

		l.stop();
		l.waitDown( TIMEOUT );
	}

	/** @throws Exception */
	@Test
	public void accept2() throws Exception
	{
		MyOtherListenerHandler lh = new MyOtherListenerHandler();
		TcpListener l = new TcpListener( "tcp://0.0.0.0:0?TcpListener.backlog=100", null );
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
	public static class MyListenerHandler implements SessionListener<Socket>
	{
		/** the event seen */
		public Monitor<What> what = new Monitor<What>( null );

		/** the socket accepted (if accept) */
		public Socket xsocket;

		public void sessionAccepted( Socket socket ) throws Exception
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
			
			System.out.println("MyListenerHandler " + event);
		}
	}

	/**
	 * ListenerHandler for testing.
	 */
	public static class MyOtherListenerHandler implements SessionListener<Socket>
	{
		/** count of accepted connections. */
		public int accepted;

		public void sessionAccepted( Socket socket ) throws Exception
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
