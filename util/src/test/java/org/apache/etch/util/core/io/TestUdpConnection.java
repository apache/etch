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
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNotSame;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import org.apache.etch.util.FlexBuffer;
import org.apache.etch.util.core.Who;
import org.junit.After;
import org.junit.Before;
import org.junit.Ignore;
import org.junit.Test;


/** Test UdpConnection. */
public class TestUdpConnection
{
	/** @throws Exception */
	@Before @Ignore
	public void init() throws Exception
	{
		aph = new MyPacketHandler();
		ac = new UdpConnection( "udp://localhost:4011" );
		ac.setSession( aph );
		ac.start();
		ac.waitUp( 4000 );
		System.out.println( "ac up" );
		
		bph = new MyPacketHandler();
		bc = new UdpConnection( "udp://localhost:4010" );
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
