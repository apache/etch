package metreos.core.io.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import java.net.InetSocketAddress;
import java.net.Socket;

import metreos.core.io.Connection;
import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;
import metreos.util.Monitor;

import org.junit.Test;

import etch.bindings.java.transport.Session;

/**
 * Tests Listener.
 */
public class TestListener
{
	private final static int TIMEOUT = 4000;

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor1() throws Exception
	{
		new Listener( null, 0, null, -1, 0 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor2() throws Exception
	{
		new Listener( null, 0, null, 65536, 0 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor3() throws Exception
	{
		new Listener( null, -1, null, 0, 0 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor4() throws Exception
	{
		new Listener( null, 0, null, 0, -1 );
	}

	/** @throws Exception */
	@Test
	public void constructor5() throws Exception
	{
		Connection<ListenerHandler> l = new Listener( null, 0, null, 0, 0 );
		assertNull( l.getHandler() );
	}

	/** @throws Exception */
	@Test
	public void constructor6() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();

		Connection<ListenerHandler> l = new Listener( lh, 0, null, 0, 0 );
		assertSame( lh, l.getHandler() );
	}

	/** @throws Exception */
	@Test
	public void setHandler() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();

		Connection<ListenerHandler> l = new Listener( null, 0, null, 0, 0 );
		assertNull( l.getHandler() );

		l.setHandler( lh );
		assertSame( lh, l.getHandler() );
	}

	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void stop1() throws Exception
	{
		Connection<ListenerHandler> l = new Listener( null, 0, null, 0, 0 );
		l.stop(); // illegal state: not started.
	}

	/** @throws Exception */
	@Test
	public void start1() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();
		Connection<ListenerHandler> l = new Listener( lh, 0, null, 0, 0 );

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
		Connection<ListenerHandler> l = new Listener( lh, 0, "127.0.0.1", 0, 0 );

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
		Connection<ListenerHandler> l = new Listener( lh, 0, null, 4001, 0 );

		assertWhat( null, lh.what );
		assertNull( lh.xsocket );

		l.start();
		l.waitUp( TIMEOUT );
		assertTrue( l.isStarted() );

		assertWhat( What.UP, lh.what );
		assertNull( lh.xsocket );

		InetSocketAddress a = (InetSocketAddress) l.localAddress();
		assertEquals( "0.0.0.0", a.getAddress().getHostAddress().toString() );
		assertEquals( 4001, a.getPort() );

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
		Connection<ListenerHandler> l = new Listener( lh, 0, "127.0.0.1", 4002, 0 );

		assertWhat( null, lh.what );
		assertNull( lh.xsocket );

		l.start();
		l.waitUp( TIMEOUT );

		assertTrue( l.isStarted() );

		assertWhat( What.UP, lh.what );
		assertNull( lh.xsocket );

		InetSocketAddress a = (InetSocketAddress) l.localAddress();
		assertEquals( "127.0.0.1", a.getAddress().getHostAddress().toString() );
		assertEquals( 4002, a.getPort() );

		l.stop();
		l.waitDown( TIMEOUT );

		assertFalse( l.isStarted() );

		assertWhat( What.DOWN, lh.what );
		assertNull( lh.xsocket );
	}

	/** @throws Exception */
	@Test( expected = InterruptedException.class )
	public void start5() throws Exception
	{
		MyListenerHandler lh = new MyListenerHandler();
		Connection<ListenerHandler> l = new Listener( lh, 0, "1.2.3.4.5", 4003, 0 );

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
		Connection<ListenerHandler> l = new Listener( lh, 0, null, 4004, 0 );

		l.start();
		l.waitUp( TIMEOUT );

		Socket s = new Socket( "127.0.0.1", 4004 );
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
		Connection<ListenerHandler> l = new Listener( lh, 100, null, 0, 0 );

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
		throws InterruptedException
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
	public static class MyListenerHandler implements ListenerHandler
	{
		/** the event seen */
		public Monitor<What> what = new Monitor<What>( null );

		/** the socket accepted (if accept) */
		public Socket xsocket;

		public void accepted( Socket socket ) throws Exception
		{
			xsocket = socket;
			what.set( What.ACCEPTED );
		}

		public Listener getSource()
		{
			return src;
		}

		public void setSource( Listener src )
		{
			this.src = src;
		}
		
		private Listener src;
		
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
			if (event.equals( Session.UP))
			{
				what.set( What.UP );
				return;
			}
			
			if (event.equals( Session.DOWN))
			{
				what.set( What.DOWN );
				return;
			}
		}
	}

	/**
	 * ListenerHandler for testing.
	 */
	public static class MyOtherListenerHandler implements ListenerHandler
	{
		/** count of accepted connections. */
		public int accepted;

		public void accepted( Socket socket ) throws Exception
		{
			accepted++;
			socket.close();
		}

		public void setSource( Listener src )
		{
			// ignore.
		}

		public Listener getSource()
		{
			// ignore.
			return null;
		}

		public void sessionControl( Object control, Object value )
		{
			// ignore.
		}

		public void sessionNotify( Object event )
		{
			// ignore.
		}

		public Object sessionQuery( Object query )
		{
			// ignore.
			return null;
		}
	}
}
