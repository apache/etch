/* $Id$
 *
 * Created by wert on Jun 19, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.util.core.io.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNotSame;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;

import etch.util.FlexBuffer;
import etch.util.core.Who;
import etch.util.core.io.Session;
import etch.util.core.io.SessionPacket;
import etch.util.core.io.UdpConnection;

/** Test UdpConnection. */
public class TestUdpConnection
{
	/** @throws Exception */
	@Before @Ignore
	public void init() throws Exception
	{
		aph = new MyPacketHandler();
		ac = new UdpConnection( "udp://localhost:4011", null );
		ac.setSession( aph );
		ac.start();
		ac.waitUp( 4000 );
		System.out.println( "ac up" );
		
		bph = new MyPacketHandler();
		bc = new UdpConnection( "udp://localhost:4010", null );
		bc.setSession( bph );
		bc.start();
		bc.waitUp( 4000 );
		System.out.println( "bc up" );
	}
	
	/** @throws Exception */
	@After @Ignore
	public void fini() throws Exception
	{
		ac.close( false );
		bc.close( false );
	}
	
	private MyPacketHandler aph;
	
	private UdpConnection ac;
	
	private MyPacketHandler bph;
	
	private UdpConnection bc;
	
	/** @throws Exception */
	@Test @Ignore
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
		ac.transportPacket( null, buf );
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
	
	/** */
	public enum What
	{
		/** */ UP,
		/** */ PACKET,
		/** */ DOWN
	}
	
	/**
	 * receive packets from the udp connection
	 */
	public static class MyPacketHandler implements SessionPacket
	{
		/** */
		public What what;
		/** */
		public Who xsender;
		/** */
		public FlexBuffer xbuf;

		public void sessionPacket( Who sender, FlexBuffer buf ) throws Exception
		{
			assertEquals( What.UP, what );
			what = What.PACKET;
			xsender = sender;
			xbuf = buf;
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
