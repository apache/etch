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
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

/** Test of Monitor */
public class TestMonitor
{
	// change this to a much smaller number (e.g., 0.1) to actually test the
	// circular queue timeout accuracy.
	private final static double REL_ERROR = 100000;
	
	/** @throws Exception */
	@Test
	public void construct1() throws Exception
	{
		testconstruct1( null );
		testconstruct1( "a" );
		testconstruct1( "b" );
	}
	
	private void testconstruct1( String descr )
	{
		Monitor<Object> m = new Monitor<Object>( descr );
		assertEquals( descr, m.getDescription() );
		assertNull( m.get() );
	}

	/** @throws Exception */
	@Test
	public void construct2() throws Exception
	{
		testconstruct2( null, null );
		testconstruct2( null, true );
		testconstruct2( null, 1 );
		testconstruct2( null, "abc" );
		testconstruct2( null, 3.14159 );
		
		testconstruct2( "a", null );
		testconstruct2( "b", true );
		testconstruct2( "c", 1 );
		testconstruct2( "d", "abc" );
		testconstruct2( "e", 3.14159 );
	}
	
	private void testconstruct2( String descr, Object value )
	{
		Monitor<Object> m = new Monitor<Object>( descr, value );
		assertEquals( descr, m.getDescription() );
		assertEquals( value, m.get() );
	}
	
	/** @throws Exception */
	@Test
	public void mToString() throws Exception
	{
		assertEquals( "Monitor null: null", new Monitor<Object>( null, null ).toString() );
		assertEquals( "Monitor null: true", new Monitor<Object>( null, true ).toString() );
		assertEquals( "Monitor null: 1", new Monitor<Object>( null, 1 ).toString() );
		assertEquals( "Monitor null: abc", new Monitor<Object>( null, "abc" ).toString() );
		assertEquals( "Monitor null: 3.14159", new Monitor<Object>( null, 3.14159 ).toString() );

		assertEquals( "Monitor a: null", new Monitor<Object>( "a", null ).toString() );
		assertEquals( "Monitor b: true", new Monitor<Object>( "b", true ).toString() );
		assertEquals( "Monitor c: 1", new Monitor<Object>( "c", 1 ).toString() );
		assertEquals( "Monitor d: abc", new Monitor<Object>( "d", "abc" ).toString() );
		assertEquals( "Monitor e: 3.14159", new Monitor<Object>( "e", 3.14159 ).toString() );
	}
	
	/** @throws Exception */
	@Test
	public void set1() throws Exception
	{
		Monitor<Object> m = new Monitor<Object>( "a", null );
		assertEquals( null, m.set( 1 ) );
		assertEquals( 1, m.set( 2 ) );
		assertEquals( 2, m.set( null ) );
		assertEquals( null, m.set( null ) );
	}
	
	/** @throws Exception */
	@Test
	public void set2() throws Exception
	{
		Monitor<Object> m = new Monitor<Object>( "a", 1 );
		assertEquals( 1, m.set( 2 ) );
		assertEquals( 2, m.set( null ) );
		assertEquals( null, m.set( null ) );
	}
	
	/** @throws Exception */
	@Test
	public void setget() throws Exception
	{
		Monitor<Object> m = new Monitor<Object>( "a", 1 );
		
		assertEquals( 1, m.set( 2 ) );
		assertEquals( 2, m.get() );
		
		assertEquals( 2, m.set( null ) );
		assertEquals( null, m.get() );
		
		assertEquals( null, m.set( null ) );
		assertEquals( null, m.get() );
	}
	
	/** @throws Exception */
	@Test
	public void setsync() throws Exception
	{
		final Monitor<Integer> m = new Monitor<Integer>( "a", null );
		final boolean[] flag = new boolean[2000];
		
		Thread t1 = TestHelp.delay( 0, new TestHelp.DelayRunnable()
		{
			public void run()
			{
				for (int i = 0; i < 1000; i++)
				{
					Integer x = m.set( i );
					if (x != null)
						flag[x] = true;
				}
			}
		} );
		
		Thread t2 = TestHelp.delay( 0, new TestHelp.DelayRunnable()
		{
			public void run()
			{
				for (int i = 1000; i < 2000; i++)
				{
					Integer x = m.set( i );
					if (x != null)
						flag[x] = true;
				}
			}
		} );

		t1.start();
		t2.start();
		
		t1.join();
		t2.join();
		
		Integer x = m.set( null );
		if (x != null)
			flag[x] = true;
		
		for (boolean b: flag)
			assertTrue( b );
	}
	
	/** @throws Exception */
	@Test
	public void waitUntilEq1() throws Exception
	{
		Monitor<Integer> m = new Monitor<Integer>( "a", null );
		m.waitUntilEq( null );
		m.waitUntilEq( null, 0 );
		
		m.set( 1 );
		m.waitUntilEq( 1 );
		m.waitUntilEq( 1, 0 );
		
		m.set( 2 );
		m.waitUntilEq( 2 );
		m.waitUntilEq( 2, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = TimeoutException.class )
	public void waitUntilEq2() throws Exception
	{
		Monitor<Integer> m = new Monitor<Integer>( "a", 1 );
		m.waitUntilEq( 2, 1 );
	}
	
	/** @throws Exception */
	@Test( expected = TimeoutException.class )
	public void waitUntilEq3() throws Exception
	{
		Monitor<Integer> m = new Monitor<Integer>( "a", null );
		m.waitUntilEq( 2, 1 );
	}
	
	/** @throws Exception */
	@Test( expected = TimeoutException.class )
	public void waitUntilEq4() throws Exception
	{
		Monitor<Integer> m = new Monitor<Integer>( "a", 1 );
		m.waitUntilEq( null, 1 );
	}
	
	/** @throws Exception */
	@Test
	public void waitUntilNotEq1() throws Exception
	{
		Monitor<Integer> m = new Monitor<Integer>( "a", null );
		m.waitUntilNotEq( 1 );
		m.waitUntilNotEq( 1, 0 );
		
		m.set( 1 );
		m.waitUntilNotEq( null );
		m.waitUntilNotEq( null, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = TimeoutException.class )
	public void waitUntilNotEq2() throws Exception
	{
		Monitor<Integer> m = new Monitor<Integer>( "a", null );
		m.waitUntilNotEq( null, 1 );
	}
	
	/** @throws Exception */
	@Test( expected = TimeoutException.class )
	public void waitUntilNotEq3() throws Exception
	{
		Monitor<Integer> m = new Monitor<Integer>( "a", 1 );
		m.waitUntilNotEq( 1, 1 );
	}
	
	/** @throws Exception */
	@Test
	public void waitUntilEqAndSet1() throws Exception
	{
		Monitor<Integer> m = new Monitor<Integer>( "a", null );
		assertEquals( null, m.waitUntilEqAndSet( null, 1 ) );
		assertEquals( 1, m.waitUntilEqAndSet( 1, 2 ) );
		assertEquals( 2, m.waitUntilEqAndSet( 2, null ) );
		assertEquals( null, m.waitUntilEqAndSet( null, null ) );

		assertEquals( null, m.waitUntilEqAndSet( null, 0, 1 ) );
		assertEquals( 1, m.waitUntilEqAndSet( 1, 0, 2 ) );
		assertEquals( 2, m.waitUntilEqAndSet( 2, 0, null ) );
		assertEquals( null, m.waitUntilEqAndSet( null, 0, null ) );
	}
	
	/** @throws Exception */
	@Test( expected = TimeoutException.class )
	public void waitUntilEqAndSet2() throws Exception
	{
		Monitor<Integer> m = new Monitor<Integer>( "a", null );
		try
		{
			m.waitUntilEqAndSet( 1, 1, 2 );
		}
		finally
		{
			assertEquals( null, m.get() );
		}
	}
	
	/** @throws Exception */
	@Test( expected = TimeoutException.class )
	public void waitUntilEqAndSet3() throws Exception
	{
		Monitor<Integer> m = new Monitor<Integer>( "a", 1 );
		try
		{
			m.waitUntilEqAndSet( null, 1, 2 );
		}
		finally
		{
			assertEquals( 1, m.get() );
		}
	}
	
	/** @throws Exception */
	@Test
	public void waitUntilNotEqAndSet1() throws Exception
	{
		Monitor<Integer> m = new Monitor<Integer>( "a", null );
		assertEquals( null, m.waitUntilNotEqAndSet( 2, 1 ) );
		assertEquals( 1, m.waitUntilNotEqAndSet( 3, 2 ) );
		assertEquals( 2, m.waitUntilNotEqAndSet( 4, null ) );
		assertEquals( null, m.waitUntilNotEqAndSet( 5, null ) );

		assertEquals( null, m.waitUntilNotEqAndSet( 2, 0, 1 ) );
		assertEquals( 1, m.waitUntilNotEqAndSet( 3, 0, 2 ) );
		assertEquals( 2, m.waitUntilNotEqAndSet( 4, 0, null ) );
		assertEquals( null, m.waitUntilNotEqAndSet( 5, 0, null ) );
	}
	
	/** @throws Exception */
	@Test( expected = TimeoutException.class )
	public void waitUntilNotEqAndSet2() throws Exception
	{
		Monitor<Integer> m = new Monitor<Integer>( "a", null );
		try
		{
			m.waitUntilNotEqAndSet( null, 1, 1 );
		}
		finally
		{
			assertEquals( null, m.get() );
		}
	}
	
	/** @throws Exception */
	@Test( expected = TimeoutException.class )
	public void waitUntilNotEqAndSet3() throws Exception
	{
		Monitor<Integer> m = new Monitor<Integer>( "a", 1 );
		try
		{
			m.waitUntilNotEqAndSet( 1, 1, null );
		}
		finally
		{
			assertEquals( 1, m.get() );
		}
	}
	
	/** @throws Exception */
	@Test
	public void waitUntilEq5() throws Exception
	{
		final Monitor<Integer> m = new Monitor<Integer>( "a", null );
		
		Thread t = TestHelp.delayAndStart( 200, new TestHelp.DelayRunnable()
		{
			public void run()
			{
				m.set( 1 );
			}
		} );

		long t0 = System.nanoTime();
		m.waitUntilEq( 1 );
		double t1 = (System.nanoTime() - t0) / 1000000.0;
		TestHelp.assertRelError( 200, t1, REL_ERROR );
		
		t.join();
	}
	
	/** @throws Exception */
	@Test
	public void waitUntilNotEq4() throws Exception
	{
		final Monitor<Integer> m = new Monitor<Integer>( "a", null );
		
		Thread t = TestHelp.delayAndStart( 200, new TestHelp.DelayRunnable()
		{
			public void run()
			{
				m.set( 1 );
			}
		} );

		long t0 = System.nanoTime();
		m.waitUntilNotEq( null );
		double t1 = (System.nanoTime() - t0) / 1000000.0;
		TestHelp.assertRelError( 200, t1, REL_ERROR );
		
		t.join();
	}
	
	/** @throws Exception */
	@Test
	public void waitUntilEqAndSet4() throws Exception
	{
		final Monitor<Integer> m = new Monitor<Integer>( "a", null );
		
		Thread t = TestHelp.delayAndStart( 200, new TestHelp.DelayRunnable()
		{
			public void run()
			{
				m.set( 1 );
			}
		} );
		
		long t0 = System.nanoTime();
		m.waitUntilEqAndSet( 1, 2 );
		double t1 = (System.nanoTime() - t0) / 1000000.0;
		// TODO is there a better way for choosing relative error?
		TestHelp.assertRelError( 200, t1, REL_ERROR );
		
		t.join();
	}
	
	/** @throws Exception */
	@Test
	public void waitUntilNotEqAndSet4() throws Exception
	{
		final Monitor<Integer> m = new Monitor<Integer>( "a", null );
		
		Thread t = TestHelp.delayAndStart( 200, new TestHelp.DelayRunnable()
		{
			public void run()
			{
				m.set( 1 );
			}
		} );

		long t0 = System.nanoTime();
		m.waitUntilNotEqAndSet( null, 2 );
		double t1 = (System.nanoTime() - t0) / 1000000.0;
		TestHelp.assertRelError( 200, t1, REL_ERROR );
		
		t.join();
	}
}
