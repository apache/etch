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

package org.apache.etch.util;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

/** Test of AlarmManager */
public class TestAlarmManager
{
	private final static int Q1 = 50;

	private final static int Q2 = 100;

	private final static int Q3 = 150;
	
	private final Object s1 = new Object();
	
	private final Object s2 = new Object();
	
	private final MyAlarmListener listener = new MyAlarmListener( 0 );

	/** @throws Exception */
	@Test
	public void start1() throws Exception
	{
		AlarmManager am = new AlarmManager();
		assertFalse( am.isStarted() );
		am.start();
		assertTrue( am.isStarted() );
	}

	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void start2() throws Exception
	{
		// already started
		AlarmManager am = new AlarmManager();
		assertFalse( am.isStarted() );
		am.start();
		assertTrue( am.isStarted() );
		am.start();
	}

	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void stop1() throws Exception
	{
		// is not started
		AlarmManager am = new AlarmManager();
		assertFalse( am.isStarted() );
		am.stop();
	}

	/** @throws Exception */
	@Test
	public void stop2() throws Exception
	{
		AlarmManager am = new AlarmManager();
		assertFalse( am.isStarted() );
		am.start();
		assertTrue( am.isStarted() );
		am.stop();
		assertFalse( am.isStarted() );
	}
	
	/** @throws Exception */
	@Test
	public void static1() throws Exception
	{
		AlarmManager.shutdown();
		
		assertNull( AlarmManager.getAlarmManager( false ) );
		assertNull( AlarmManager.getAlarmManager( false ) );
		
		AlarmManager am1 = AlarmManager.getAlarmManager( true );
		assertNotNull( am1 );
		assertTrue( am1.isStarted() );
		
		AlarmManager am2 = AlarmManager.getAlarmManager( true );
		assertSame( am2, am1 );
		assertTrue( am1.isStarted() );
		
		AlarmManager.shutdown();
		assertFalse( am1.isStarted() );
		assertNull( AlarmManager.getAlarmManager( false ) );
		
		AlarmManager.shutdown();
	}

	/** @throws Exception */
	@Test
	public void listener1() throws Exception
	{
		assertEquals( 0, listener.delay );
		assertFalse( listener.wake.get() );
		assertNull( listener.state );

		listener.wakeup( null, null, 12345 );
		assertEquals( 0, listener.delay );
		assertTrue( listener.wake.get() );
		assertNull( listener.state );
		
		listener.wakeup( null, s1, 23456 );
		assertEquals( 0, listener.delay );
		assertTrue( listener.wake.get() );
		assertSame( s1, listener.state );

		MyAlarmListener listener1 = new MyAlarmListener( 1 );
		assertEquals( 1, listener1.delay );
	}

	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void add1() throws Exception
	{
		// is not started
		AlarmManager am = new AlarmManager();
		
		am.add( listener, null, Q1 );
	}

	/** @throws Exception */
	@Test( expected = NullPointerException.class )
	public void add2() throws Exception
	{
		// listener == null
		AlarmManager am = new AlarmManager();
		am.start();
		
		am.add( null, null, Q1 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void add3() throws Exception
	{
		// delay <= 0
		AlarmManager am = new AlarmManager();
		am.start();
		
		am.add( listener, null, -1 );
	}

	/** @throws Exception */
	@Test
	public void add4() throws Exception
	{
		AlarmManager am = new AlarmManager();
		am.start();

		am.add( listener, null, Q1 );
		assertFalse( listener.wake.get() );
		assertNull( listener.state );

		Thread.sleep( Q2 );
		assertTrue( listener.wake.get() );
		assertNull( listener.state );
	}

	/** @throws Exception */
	@Test
	public void add5() throws Exception
	{
		AlarmManager am = new AlarmManager();
		am.start();

		am.add( listener, s1, Q1 );
		assertFalse( listener.wake.get() );
		assertNull( listener.state );

		Thread.sleep( Q2 );
		assertTrue( listener.wake.get() );
		assertSame( s1, listener.state );
	}

	/** @throws Exception */
	@Test
	public void add6() throws Exception
	{
		AlarmManager am = new AlarmManager();
		am.start();

		am.add( listener, s1, Q3 );
		assertFalse( listener.wake.get() );
		assertNull( listener.state );

		am.add( listener, s2, Q1 );
		assertFalse( listener.wake.get() );
		assertNull( listener.state );

		Thread.sleep( Q2 );
		assertTrue( listener.wake.get() );
		assertSame( s2, listener.state );
	}

	/** @throws Exception */
	@Test
	public void add7() throws Exception
	{
		AlarmManager am = new AlarmManager();
		am.start();

		listener.excp = true;
		am.add( listener, s1, Q1 );
		assertFalse( listener.wake.get() );
		assertNull( listener.state );

		Thread.sleep( Q2 );
		assertTrue( listener.wake.get() );
		assertSame( s1, listener.state );
	}

	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void remove1() throws Exception
	{
		// is not started
		AlarmManager am = new AlarmManager();
		
		am.remove( listener );
	}

	/** @throws Exception */
	@Test
	public void remove2() throws Exception
	{
		AlarmManager am = new AlarmManager();
		am.start();
		
		am.remove( listener );
	}

	/** @throws Exception */
	@Test
	public void remove3() throws Exception
	{
		AlarmManager am = new AlarmManager();
		am.start();

		am.add( listener, null, Q1 );
		assertFalse( listener.wake.get() );
		assertNull( listener.state );
		
		am.remove( listener );
		assertFalse( listener.wake.get() );
		assertNull( listener.state );
		
		Thread.sleep( Q2 );
		assertFalse( listener.wake.get() );
		assertNull( listener.state );
	}
	
	/** @throws Exception */
	@Test
	public void repeat1() throws Exception
	{
		AlarmManager am = new AlarmManager();
		am.start();
		
		listener.delay = Q2;
		am.add( listener, null, Q2 );
		
		listener.wake.waitUntilEqAndSet( true, Q3, false );
		System.out.printf( "due0 = %d\n", listener.due );
		long t0 = listener.due;
		
		listener.wake.waitUntilEqAndSet( true, Q3, false );
		System.out.printf( "due1 = %d\n", listener.due );
		long d1 = listener.due - t0;
		
		listener.wake.waitUntilEqAndSet( true, Q3, false );
		System.out.printf( "due2 = %d\n", listener.due );
		long d2 = listener.due - t0;
		
		listener.wake.waitUntilEqAndSet( true, Q3, false );
		System.out.printf( "due3 = %d\n", listener.due );
		long d3 = listener.due - t0;
		
		Thread.sleep( Q1 );
		listener.delay = 0;
		
		listener.wake.waitUntilEqAndSet( true, Q3, false );
		System.out.printf( "due4 = %d\n", listener.due );
		long d4 = listener.due - t0;
		
		long m1 = Math.round( d1/(double) d1 );
		long m2 = Math.round( d2/(double) d1 );
		long m3 = Math.round( d3/(double) d1 );
		long m4 = Math.round( d4/(double) d1 );
		
		System.out.printf( "times %d %d %d %d %d\n", t0, d1, d2, d3, d4 );
		
		assertEquals( 1, m1 );
		assertEquals( 2, m2 );
		assertEquals( 3, m3 );
		assertEquals( 4, m4 );
	}
	
	/** @throws Exception */
	@Test
	public void repeat2() throws Exception
	{
		AlarmManager am = new AlarmManager();
		am.start();
		
		listener.delay = -Q2;
		am.add( listener, null, Q2 );
		
		listener.wake.waitUntilEqAndSet( true, Q3, false );
		System.out.printf( "due0 = %d\n", listener.due );
		long t0 = listener.due;
		
		listener.wake.waitUntilEqAndSet( true, Q3, false );
		System.out.printf( "due1 = %d\n", listener.due );
		long d1 = listener.due - t0;
		
		listener.wake.waitUntilEqAndSet( true, Q3, false );
		System.out.printf( "due2 = %d\n", listener.due );
		long d2 = listener.due - t0;
		
		listener.wake.waitUntilEqAndSet( true, Q3, false );
		System.out.printf( "due3 = %d\n", listener.due );
		long d3 = listener.due - t0;
		
		Thread.sleep( Q1 );
		listener.delay = 0;
		
		listener.wake.waitUntilEqAndSet( true, Q3, false );
		System.out.printf( "due4 = %d\n", listener.due );
		long d4 = listener.due - t0;
		
		long m1 = Math.round( d1/(double) d1 );
		long m2 = Math.round( d2/(double) d1 );
		long m3 = Math.round( d3/(double) d1 );
		long m4 = Math.round( d4/(double) d1 );
		
		System.out.printf( "times %d %d %d %d %d\n", t0, d1, d2, d3, d4 );
		
		assertEquals( 1, m1 );
		assertEquals( 2, m2 );
		assertEquals( 3, m3 );
		assertEquals( 4, m4 );
	}
	
	/** @throws Exception */
	@Test( expected = TimeoutException.class )
	public void repeat3() throws Exception
	{
		// timeout
		AlarmManager am = new AlarmManager();
		am.start();
		
		listener.delay = Q2;
		am.add( listener, null, Q2 );
		
		listener.wake.waitUntilEqAndSet( true, Q3, false );
		
		listener.wake.waitUntilEqAndSet( true, Q3, false );
		
		listener.wake.waitUntilEqAndSet( true, Q3, false );
		
		Thread.sleep( Q1 );
		listener.delay = 0;
		
		listener.wake.waitUntilEqAndSet( true, Q3, false );
		// alarm is now canceled
		
		// this should timeout...
		listener.wake.waitUntilEqAndSet( true, Q3, false );
	}

	private static class MyAlarmListener implements AlarmListener
	{
		public MyAlarmListener( int delay )
		{
			this.delay = delay;
		}

		private int delay;
		
		private boolean excp;

		private Monitor<Boolean> wake = new Monitor<Boolean>( "wake", false );

		private Object state;
		
		private long due;

		public int wakeup( AlarmManager manager, Object state, long due )
		{
			this.state = state;
			this.due = due;
			wake.set( true );
			
			if (excp)
				throw new RuntimeException();
			
			return delay;
		}
	}
}
