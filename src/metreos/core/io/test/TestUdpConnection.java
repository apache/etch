/* $Id$
 *
 * Created by wert on Jun 19, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package metreos.core.io.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNotSame;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import java.net.DatagramSocket;
import java.net.InetAddress;

import metreos.core.Who;
import metreos.core.io.PacketHandler;
import metreos.core.io.PacketSource;
import metreos.core.io.UdpConnection;
import metreos.util.FlexBuffer;

import org.junit.After;
import org.junit.Before;
import org.junit.Test;

import etch.bindings.java.transport.Session;

public class TestUdpConnection
{
	@Before
	public void init() throws Exception
	{
		a = new DatagramSocket( 4001 );
		a.connect( InetAddress.getByName( "localhost" ), 4002 );
		aph = new MyPacketHandler();
		ac = new UdpConnection( aph, a );
		ac.start();
		ac.waitUp( 4000 );
		System.out.println( "ac up" );
		
		b = new DatagramSocket( 4002 );
		b.connect( InetAddress.getByName( "localhost" ), 4001 );
		bph = new MyPacketHandler();
		bc = new UdpConnection( bph, b );
		bc.start();
		bc.waitUp( 4000 );
		System.out.println( "bc up" );
	}
	
	@After
	public void fini() throws Exception
	{
		ac.close( false );
		bc.close( false );
	}
	
	private DatagramSocket a;
	
	private MyPacketHandler aph;
	
	private UdpConnection ac;
	
	private DatagramSocket b;
	
	private MyPacketHandler bph;
	
	private UdpConnection bc;
	
	@Test
	public void nothing() throws Exception
	{
		// nothing to do, keep silly ant junit test runner happy.
	}
	
	// @Test
	public void blah() throws Exception
	{
		assertEquals( What.UP, aph.what );
		assertEquals( What.UP, bph.what );
		FlexBuffer buf = new FlexBuffer();
		buf.put( 1 );
		buf.put( 2 );
		buf.put( 3 );
		buf.put( 4 );
		buf.put( 5 );
		buf.setIndex( 0 );
		ac.packet( null, buf );
		Thread.sleep( 500 );
		assertEquals( What.PACKET, bph.what );
		assertNotNull( bph.xsender );
		assertNotSame( buf, bph.xbuf );
		assertEquals( 0, bph.xbuf.index() );
		assertEquals( 5, bph.xbuf.length() );
		assertEquals( 1, bph.xbuf.get() );
		assertEquals( 2, bph.xbuf.get() );
		assertEquals( 3, bph.xbuf.get() );
		assertEquals( 4, bph.xbuf.get() );
		assertEquals( 5, bph.xbuf.get() );
	}
	
	public enum What { UP, PACKET, DOWN }
	
	public static class MyPacketHandler implements PacketHandler
	{
		public What what;
		public Who xsender;
		public FlexBuffer xbuf;

		public void packet( Who sender, FlexBuffer buf ) throws Exception
		{
			assertEquals( What.UP, what );
			what = What.PACKET;
			xsender = sender;
			xbuf = buf;
		}

		public void setSource( PacketSource src )
		{
			// ignore.
			
		}

		public PacketSource getSource()
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
			if (event.equals( Session.UP ))
			{
				assertNull( what );
				what = What.UP;
				return;
			}
			
			if (event.equals( Session.DOWN ))
			{
				assertTrue( what == What.UP || what == What.PACKET );
				what = What.DOWN;
				return;
			}
		}

		public Object sessionQuery( Object query )
		{
			// ignore.
			return null;
		}
	}
}
