package metreos.core.io.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import java.io.InputStream;
import java.net.InetSocketAddress;
import java.net.Socket;

import metreos.core.Who;
import metreos.core.io.Connection;
import metreos.core.io.DataHandler;
import metreos.core.io.DataSource;
import metreos.core.io.Listener;
import metreos.core.io.ListenerHandler;
import metreos.core.io.TcpConnection;
import metreos.util.FlexBuffer;
import metreos.util.Monitor;

import org.junit.AfterClass;
import org.junit.BeforeClass;
import org.junit.Test;

import etch.bindings.java.transport.Session;

/**
 * Tests TcpConnection.
 */
public class TestTcpConnection
{
	private final static MyListener lh = new MyListener();

	private final static Connection<ListenerHandler> l = new Listener( lh, 0, null, 0, 0 );

	private static final int TIMEOUT = 4000;
	
	private static int port;

	/** @throws Exception */
	@BeforeClass
	public static void startListener() throws Exception
	{
		// start listener
		l.start();
		l.waitUp( 4000 );
		port = ((InetSocketAddress) l.localAddress()).getPort();
	}
	
	/** @throws Exception */
	@AfterClass
	public static void stopListener() throws Exception
	{
		l.stop();
		l.waitDown( 4000 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor1() throws Exception
	{
		new TcpConnection( (DataHandler) null, (Socket) null );
	}

	/** @throws Exception */
	@Test
	public void constructor2() throws Exception
	{
		Socket s = new Socket( "127.0.0.1", port );
		Socket t = lh.accepted.waitUntilNotEqAndSet( null, null );
		
		TcpConnection c = new TcpConnection( null, s );
		assertNull( c.getHandler() );
		
		s.close();
		t.close();
	}

	/** @throws Exception */
	@Test
	public void constructor3() throws Exception
	{
		DataHandler dh = new MyDataHandler();
		Socket s = new Socket( "127.0.0.1", port );
		Socket t = lh.accepted.waitUntilNotEqAndSet( null, null );
		
		TcpConnection c = new TcpConnection( dh, s );
		assertSame( dh, c.getHandler() );
		
		s.close();
		t.close();
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor4() throws Exception
	{
		new TcpConnection( null, null, 0, 0 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor5() throws Exception
	{
		new TcpConnection( null, "localhost", 0, 0 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor6() throws Exception
	{
		new TcpConnection( null, "localhost", 65536, 0 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor7() throws Exception
	{
		new TcpConnection( null, "localhost", 1, -1 );
	}

	/** @throws Exception */
	@Test
	public void constructor8() throws Exception
	{
		TcpConnection c = new TcpConnection( null, "localhost", port, 0 );
		assertNull( c.getHandler() );
	}

	/** @throws Exception */
	@Test
	public void constructor9() throws Exception
	{
		DataHandler dh = new MyDataHandler();
		TcpConnection c = new TcpConnection( dh, "localhost", port, 0 );
		assertSame( dh, c.getHandler() );
	}

	/** @throws Exception */
	@Test
	public void start1() throws Exception
	{
		Socket s = new Socket( "127.0.0.1", port );
		Socket t = lh.accepted.waitUntilNotEqAndSet( null, null );
		
		MyDataHandler dh = new MyDataHandler();
		TcpConnection c = new TcpConnection( dh, s );

		assertFalse( c.isStarted() );
		c.start();
		assertTrue( c.isStarted() );
		
		c.waitUp( TIMEOUT );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		c.flush();
		assertNotNull( c.remoteAddress() );
		assertNotNull( c.localAddress() );
		assertEquals( c.remoteAddress(), t.getLocalSocketAddress() );
		assertEquals( c.localAddress(), t.getRemoteSocketAddress() );
		
		t.close();
		
		assertWhat( What.DOWN, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
	}

	/** @throws Exception */
	@Test
	public void start2() throws Exception
	{
		Socket s = new Socket( "127.0.0.1", port );
		Socket t = lh.accepted.waitUntilNotEqAndSet( null, null );
		
		MyDataHandler dh = new MyDataHandler();
		TcpConnection c = new TcpConnection( dh, s );
		
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
		Socket s = new Socket( "127.0.0.1", port );
		Socket t = lh.accepted.waitUntilNotEqAndSet( null, null );
		
		MyDataHandler dh = new MyDataHandler();
		TcpConnection c = new TcpConnection( dh, s );
		
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
		Socket s = new Socket( "127.0.0.1", port );
		Socket t = lh.accepted.waitUntilNotEqAndSet( null, null );
		
		MyDataHandler dh = new MyDataHandler();
		TcpConnection c = new TcpConnection( dh, s );
		
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
		Socket s = new Socket( "127.0.0.1", port );
		Socket t = lh.accepted.waitUntilNotEqAndSet( null, null );
		
		MyDataHandler dh = new MyDataHandler();
		TcpConnection c = new TcpConnection( dh, s );
		
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
		TcpConnection c = new TcpConnection( dh, "127.0.0.1", port, 0 );
		
		assertFalse( c.isStarted() );
		c.start();
		assertTrue( c.isStarted() );
		
		Socket t = lh.accepted.waitUntilNotEqAndSet( null, null );
		
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
	public void data1() throws Exception
	{
		MyDataHandler dh = new MyDataHandler();
		TcpConnection c = new TcpConnection( dh, "127.0.0.1", port, 0 );
		
		assertFalse( c.isStarted() );
		c.start();
		assertTrue( c.isStarted() );
		
		Socket t = lh.accepted.waitUntilNotEqAndSet( null, null );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		t.getOutputStream().write( 23 );
		t.getOutputStream().flush();
		
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
		TcpConnection c = new TcpConnection( dh, "127.0.0.1", port, 0 );
		
		assertFalse( c.isStarted() );
		c.start();
		assertTrue( c.isStarted() );
		
		Socket t = lh.accepted.waitUntilNotEqAndSet( null, null );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		byte[] buf = { 21, 22, 23 };
		
		t.getOutputStream().write( buf );
		t.getOutputStream().flush();
		
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

	/** @throws Exception */
	@Test
	public void data3() throws Exception
	{
		MyDataHandler dh = new MyDataHandler();
		TcpConnection c = new TcpConnection( dh, "127.0.0.1", port, 0 );
		
		assertFalse( c.isStarted() );
		c.start();
		assertTrue( c.isStarted() );
		
		Socket t = lh.accepted.waitUntilNotEqAndSet( null, null );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		t.shutdownOutput();
		
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
		TcpConnection c = new TcpConnection( dh, "127.0.0.1", port, 0 );

		c.start();
		
		Socket t = lh.accepted.waitUntilNotEqAndSet( null, null );
		
		assertWhat( What.UP, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		c.data( null, new FlexBuffer( new byte[] { 21, 22, 23 } ) );
		c.flush();
		
		InputStream is = t.getInputStream();
		assertEquals( 21, is.read() );
		assertEquals( 22, is.read() );
		assertEquals( 23, is.read() );
		
		c.shutdownOutput();
		
		assertEquals( -1, is.read() );
		
		c.close();
		
		assertWhat( What.DOWN, dh.what );
		assertNull( dh.xsender );
		assertNull( dh.xbuf );
		
		t.close();
	}
	
	private void assertWhat( What what, Monitor<What> mon )
		throws InterruptedException
	{
		mon.waitUntilEqAndSet( what, 4000, null );
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
	static class MyListener implements ListenerHandler
	{
		/** monitor to report accepted socket */
		public Monitor<Socket> accepted = new Monitor<Socket>( "accepted" );
		
		public void accepted( Socket s ) throws Exception
		{
			accepted.set( s );
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
	
	/**
	 * DataHandler for testing.
	 */
	static class MyDataHandler implements DataHandler
	{
		/** holds event type */
		public Monitor<What> what = new Monitor<What>( "what" );

		/** sender of the event if DATA */
		public Who xsender;

		/** data of the event if DATA */
		public byte[] xbuf;
		
		public void data( Who sender, FlexBuffer buf ) throws Exception
		{
			xsender = sender;
			xbuf = buf.getAvailBytes();
			what.set( What.DATA );
		}

		public void setSource( DataSource src )
		{
			// ignore.
		}

		public DataSource getSource()
		{
			// ignore.
			return null;
		}

		public void sessionControl( Object control, Object value )
		{
			throw new UnsupportedOperationException( "unknown control "+control );
		}

		public void sessionNotify( Object event )
		{
			if (event.equals( Session.UP ))
			{
				xbuf = null;
				what.set( What.UP );
				return;
			}
			
			if (event.equals( Session.DOWN ))
			{
				xbuf = null;
				what.set( What.DOWN );
				return;
			}
		}

		public Object sessionQuery( Object query )
		{
			throw new UnsupportedOperationException( "unknown query "+query );
		}
	}
}
