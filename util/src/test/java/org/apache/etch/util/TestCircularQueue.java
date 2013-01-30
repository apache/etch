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
import static org.junit.Assert.assertTrue;

import org.junit.Test;

/** Test of CircularQueue */
public class TestCircularQueue
{
	// change this to a much smaller number (e.g., 0.1) to actually test the
	// circular queue timeout accuracy.
	private final static double REL_ERROR = 100000;
	
	/** @throws Exception */
	@Test
	public void testgettimes() throws Exception
	{
		@SuppressWarnings("unused")
		double[] times;
		
		times = gettimes();
//		System.out.printf( "times: hp %f / tod %f\n", times[0], times[1] );
		
		times = gettimes();
//		System.out.printf( "times: hp %f / tod %f\n", times[0], times[1] );
		
		times = gettimes();
//		System.out.printf( "times: hp %f / tod %f\n", times[0], times[1] );
	}
	
	private double[] gettimes() throws InterruptedException
	{
		long t0 = Timer.getNanos();
		long s0 = System.currentTimeMillis();
		
		Thread.sleep( 1000 );
		
		double t = Timer.getSecondsSince( t0 );
		double s = (System.currentTimeMillis() - s0) / 1000.0;
		
		return new double[] { t, s };
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void construct1() throws Exception
	{
		new CircularQueue<Integer>( 0 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void construct2() throws Exception
	{
		new CircularQueue<Integer>( -1 );
	}
	
	/** @throws Exception */
	@Test
	public void construct3() throws Exception
	{
		CircularQueue<Integer> queue = new CircularQueue<Integer>( 1 );
		assertEquals( 1, queue.size() );
	}
	
	/** @throws Exception */
	@Test
	public void construct4() throws Exception
	{
		CircularQueue<Integer> queue = new CircularQueue<Integer>( 2 );
		assertEquals( 2, queue.size() );
	}
	
	/** @throws Exception */
	@Test
	public void construct5() throws Exception
	{
		CircularQueue<Integer> queue = new CircularQueue<Integer>();
		assertEquals( 10, queue.size() );
	}
	
	/** @throws Exception */
	@Test
	public void putget1() throws Exception
	{
		CircularQueue<Integer> queue = new CircularQueue<Integer>();
		
		assertEquals( 0, queue.count() );
		assertTrue( queue.isEmpty() );
		assertFalse( queue.isFull() );
		
		for (int i = 0; i < 10000; i++)
		{
			queue.put( i );
			assertEquals( 1, queue.count() );
			assertFalse( queue.isEmpty() );
			
			assertEquals( i, queue.get() );
			assertEquals( 0, queue.count() );
			assertTrue( queue.isEmpty() );
		}
	}
	
	/** @throws Exception */
	@Test
	public void putget2() throws Exception
	{
		CircularQueue<Integer> queue = new CircularQueue<Integer>();
		
		assertNull( queue.get( 1 ) );
		assertNull( queue.get( -1 ) );
		assertNull( queue.get( 10 ) );
		
		queue.put( 9 );
		assertEquals( 9, queue.get( 1 ) );
		
		queue.put( 9 );
		assertEquals( 9, queue.get( -1 ) );
		
		queue.put( 9 );
		assertEquals( 9, queue.get( 10 ) );
		
		assertNull( queue.get( 1 ) );
		assertNull( queue.get( -1 ) );
		assertNull( queue.get( 10 ) );
		
		long t0;
		double t;
		
		t0 = System.nanoTime();
		assertNull( queue.get( 10 ) );
		t = (System.nanoTime() - t0) / 1000000.0;
		// TODO is there a better way for choosing relative error?
		TestHelp.assertRelError( 10, t, REL_ERROR );
		
		t0 = System.nanoTime();
		assertNull( queue.get( 20 ) );
		t = (System.nanoTime() - t0) / 1000000.0;
		// TODO is there a better way for choosing relative error?
		TestHelp.assertRelError( 20, t, REL_ERROR );
		
		t0 = System.nanoTime();
		assertNull( queue.get( 30 ) );
		t = (System.nanoTime() - t0) / 1000000.0;
		// TODO is there a better way for choosing relative error?
		TestHelp.assertRelError( 30, t, REL_ERROR );
	}
	
	/** @throws Exception */
	@Test
	public void putget3() throws Exception
	{
		CircularQueue<Integer> queue = new CircularQueue<Integer>();
		
		assertTrue( queue.put( 1 ) );
		assertTrue( queue.put( 2 ) );
		assertTrue( queue.put( 3 ) );
		assertEquals( 1, queue.get() );
		assertEquals( 2, queue.get() );
		assertEquals( 3, queue.get() );
		
		assertTrue( queue.put( 1, 0 ) );
		assertTrue( queue.put( 2, 0 ) );
		assertTrue( queue.put( 3, 0 ) );
		assertEquals( 1, queue.get( 0 ) );
		assertEquals( 2, queue.get( 0 ) );
		assertEquals( 3, queue.get( 0 ) );
		
		assertTrue( queue.put( 1, -1 ) );
		assertTrue( queue.put( 2, -1 ) );
		assertTrue( queue.put( 3, -1 ) );
		assertEquals( 1, queue.get( -1 ) );
		assertEquals( 2, queue.get( -1 ) );
		assertEquals( 3, queue.get( -1 ) );
		
		assertTrue( queue.put( 1, 1 ) );
		assertTrue( queue.put( 2, 1 ) );
		assertTrue( queue.put( 3, 1 ) );
		assertEquals( 1, queue.get( 1 ) );
		assertEquals( 2, queue.get( 1 ) );
		assertEquals( 3, queue.get( 1 ) );
	}
	
	/** @throws Exception */
	@Test
	public void get() throws Exception
	{
		CircularQueue<Integer> queue = new CircularQueue<Integer>( 1 );
		
		System.nanoTime();
		assertNull( queue.get( 10 ) );
		TestHelp.assertRelError( 1, 1, 1 );

		for (int i: new int[] { 10, 20, 30, 50, 80, 130, 210, 340, 550, 890, 1440 } )
		{
//			System.out.printf( "get delay = %d\n", i );
			long t0 = System.nanoTime();
			assertNull( queue.get( i ) );
			double t = (System.nanoTime() - t0) / 1000000.0;
			// TODO is there a better way for choosing relative error?
			TestHelp.assertRelError( i, t, REL_ERROR );
		}
	}
	
	/** @throws Exception */
	@Test( expected = NullPointerException.class )
	public void put() throws Exception
	{
		CircularQueue<Integer> queue = new CircularQueue<Integer>();
		queue.put( null );
	}
	
	/** @throws Exception */
	@Test
	public void full() throws Exception
	{
		CircularQueue<Integer> queue = new CircularQueue<Integer>( 1 );
		
		assertFalse( queue.isFull() );
		assertTrue( queue.put( 0 ) );
		assertTrue( queue.isFull() );
		assertEquals( 0, queue.get() );
		assertFalse( queue.isFull() );
		
		assertTrue( queue.put( 0 ) );
		assertFalse( queue.put( 0, -1 ) );
		assertFalse( queue.put( 0, 1 ) );
		assertFalse( queue.put( 0, 10 ) );
	}
	
	/** @throws Exception */
	@Test
	public void close1() throws Exception
	{
		CircularQueue<Integer> queue = new CircularQueue<Integer>();
		
		assertFalse( queue.isClosed() );
		queue.close();
		assertTrue( queue.isClosed() );
		
		assertNull( queue.get() );
		assertNull( queue.get( -1 ) );
		assertNull( queue.get( 0 ) );
		assertNull( queue.get( 1 ) );
		
		queue.close();
		assertTrue( queue.isClosed() );
	}
	
	/** @throws Exception */
	@Test
	public void close2() throws Exception
	{
		CircularQueue<Integer> queue = new CircularQueue<Integer>();

		assertFalse( queue.isClosed() );
		queue.close();
		assertTrue( queue.isClosed() );
		
		assertFalse( queue.put( 0 ) );
		assertFalse( queue.put( 0, -1 ) );
		assertFalse( queue.put( 0, 0 ) );
		assertFalse( queue.put( 0, 1 ) );
		
		queue.close();
		assertTrue( queue.isClosed() );
	}
	
	/** @throws Exception */
	@Test
	public void close3() throws Exception
	{
		CircularQueue<Integer> queue = new CircularQueue<Integer>();
		queue.put( 1 );
		queue.put( 2 );
		queue.put( 3 );
		queue.close();
		assertEquals( 1, queue.get() );
		assertEquals( 2, queue.get() );
		assertEquals( 3, queue.get() );
		assertNull( queue.get() );
	}
	
	/** @throws Exception */
	@Test
	public void delay1() throws Exception
	{
		final CircularQueue<Integer> queue = new CircularQueue<Integer>();
		
		TestHelp.delayAndStart( 10, new TestHelp.DelayRunnable() { public void run() throws Exception { queue.put( 99 ); } } );
		assertEquals( 99, queue.get() );
		
		TestHelp.delayAndStart( 10, new TestHelp.DelayRunnable() { public void run() throws Exception { queue.close(); } } );
		assertNull( queue.get() );
	}
	
	/** @throws Exception */
	@Test
	public void delay2() throws Exception
	{
		final CircularQueue<Integer> queue = new CircularQueue<Integer>( 1 );
		
		assertTrue( queue.put( 1 ) );
		TestHelp.delayAndStart( 10, new TestHelp.DelayRunnable() { public void run() throws Exception { assertEquals( 1, queue.get() ); } } );
		assertTrue( queue.put( 2 ) );
		assertEquals( 2, queue.get() );
		
		assertTrue( queue.put( 1 ) );
		TestHelp.delayAndStart( 10, new TestHelp.DelayRunnable() { public void run() throws Exception { queue.close(); } } );
		assertFalse( queue.put( 2 ) );
		assertEquals( 1, queue.get() );
		assertNull( queue.get() );
	}
	
	/** @throws Exception */
	@Test
	public void stress1() throws Exception
	{
		final CircularQueue<Integer> queue = new CircularQueue<Integer>( 1 );
		final int n = 10000;
		
		new Thread( new Runnable()
		{
			public void run()
			{
				for (int i = 0; i < n; i++)
				{
					try
					{
						queue.put( i );
					}
					catch ( InterruptedException e )
					{
						///CLOVER:OFF
						e.printStackTrace();
						///CLOVER:ON
					}
				}
			}
		} ).start();
		
		for (int i = 0; i < n; i++)
			assertEquals( i, queue.get() );
	}
	
	/** @throws Exception */
	@Test
	public void stress2() throws Exception
	{
		final CircularQueue<Integer> queue = new CircularQueue<Integer>( 1 );
		final int n = 1000;
		
		new Thread( new Runnable()
		{
			public void run()
			{
				for (int i = 0; i < n; i++)
				{
					try
					{
						Thread.sleep( 5 );
						queue.put( i );
					}
					catch ( InterruptedException e )
					{
						///CLOVER:OFF
						e.printStackTrace();
						///CLOVER:ON
					}
				}
			}
		} ).start();
		
		for (int i = 0; i < n; i++)
			assertEquals( i, queue.get() );
	}
	
	/** @throws Exception */
	@Test
	public void stress3() throws Exception
	{
		final CircularQueue<Integer> queue = new CircularQueue<Integer>( 1 );
		final int n = 1000;
		
		new Thread( new Runnable()
		{
			public void run()
			{
				for (int i = 0; i < n; i++)
				{
					try
					{
						queue.put( i );
					}
					catch ( InterruptedException e )
					{
						///CLOVER:OFF
						e.printStackTrace();
						///CLOVER:ON
					}
				}
			}
		} ).start();
		
		for (int i = 0; i < n; i++)
		{
			Thread.sleep( 5 );
			assertEquals( i, queue.get() );
		}
	}
	
	/** @throws Exception */
	@Test
	public void stress4() throws Exception
	{
		final CircularQueue<Integer> queue = new CircularQueue<Integer>();
		
		// we will setup two threads waiting to get and
		// then in a single synchronized step put two
		// items in the queue. the first thread will be
		// woken to get, and once done the second thread
		// should be woken by the first.
		
		Thread t1 = new Thread( new Runnable()
		{
			public void run()
			{
				try
				{
					assertNotNull( queue.get() );
				}
				catch ( InterruptedException e )
				{
					// e.printStackTrace();
				}
			}
		}, "t1" );
		
		Thread t2 = new Thread( new Runnable()
		{
			public void run()
			{
				try
				{
					assertNotNull( queue.get() );
				}
				catch ( InterruptedException e )
				{
					// e.printStackTrace();
				}
			}
		}, "t2" );

		t1.start();
		t2.start();
		
		// wait until both threads are waiting on queue...
		
		Thread.sleep( 100 );
		
		synchronized (queue)
		{
			queue.put( 1 );
			queue.put( 2 );
		}

		harvest( t1 );
		harvest( t2 );
	}
	
	/** @throws Exception */
	@Test
	public void stress5() throws Exception
	{
		final CircularQueue<Integer> queue = new CircularQueue<Integer>( 3 );
		
		// we will setup two threads waiting to put to the queue,
		// then in a single synchronized step, read two items from
		// the queue. the first thread will be woken to put, and
		// once done the second thread should be woken by the first.
		
		queue.put( 0 );
		queue.put( 1 );
		queue.put( 2 );
		assertTrue( queue.isFull() );
		
		Thread t1 = new Thread( new Runnable()
		{
			public void run()
			{
				try
				{
					assertTrue( queue.put( 3 ) );
				}
				catch ( InterruptedException e )
				{
					// e.printStackTrace();
				}
			}
		}, "t1" );
		
		Thread t2 = new Thread( new Runnable()
		{
			public void run()
			{
				try
				{
					assertTrue( queue.put( 4 ) );
				}
				catch ( InterruptedException e )
				{
					// e.printStackTrace();
				}
			}
		}, "t2" );
		
		t1.start();
		t2.start();
		
		// wait until both threads are waiting on queue...
		
		Thread.sleep( 100 );
		
		synchronized (queue)
		{
			assertNotNull( queue.get() );
			assertNotNull( queue.get() );
		}

		harvest( t1 );
		harvest( t2 );
	}
	
	/** @throws Exception */
	@Test
	public void stress6() throws Exception
	{
		final CircularQueue<Integer> queue = new CircularQueue<Integer>( 5 );
		
		// start two getters and two putters and let 'em duke it out...
		
		Thread t1 = new Thread( new Runnable()
		{
			public void run()
			{
				try
				{
					for (int i = 0; i < 100; i++)
						assertTrue( queue.put( i ) );
				}
				catch ( InterruptedException e )
				{
					// e.printStackTrace();
				}
			}
		}, "t1" );
		
		Thread t2 = new Thread( new Runnable()
		{
			public void run()
			{
				try
				{
					for (int i = 0; i < 100; i++)
						assertTrue( queue.put( i ) );
				}
				catch ( InterruptedException e )
				{
					// e.printStackTrace();
				}
			}
		}, "t2" );
		
		Thread t3 = new Thread( new Runnable()
		{
			public void run()
			{
				try
				{
					for (int i = 0; i < 100; i++)
						assertNotNull( queue.get() );
				}
				catch ( InterruptedException e )
				{
					// e.printStackTrace();
				}
			}
		}, "t3" );
		
		Thread t4 = new Thread( new Runnable()
		{
			public void run()
			{
				try
				{
					for (int i = 0; i < 100; i++)
						assertNotNull( queue.get() );
				}
				catch ( InterruptedException e )
				{
					// e.printStackTrace();
				}
			}
		}, "t4" );
		
		t1.start();
		t2.start();
		t3.start();
		t4.start();

		harvest( t1 );
		harvest( t2 );
		harvest( t3 );
		harvest( t4 );
	}
	
	/** @throws Exception */
	@Test
	public void harvest1() throws Exception
	{
		Thread t = new Thread( new Runnable()
		{
			public void run()
			{
				// nothing to do.
			}
		}, "t" );
		t.start();
		harvest( t );
	}
	
	/** @throws Exception */
	@Test( expected = TimeoutException.class )
	public void harvest2() throws Exception
	{
		Thread t = new Thread( new Runnable()
		{
			public void run()
			{
				try
				{
					Thread.sleep( 10000 );
				}
				catch ( InterruptedException e )
				{
					// ignore.
				}
			}
		}, "t" );
		t.start();
		harvest( t );
	}
	
	private void harvest( Thread t ) throws Exception
	{
		t.join( 1000 );
		if (t.isAlive())
		{
			t.interrupt();
			throw new TimeoutException( t.getName()+" is stuck" );
		}
	}
}
