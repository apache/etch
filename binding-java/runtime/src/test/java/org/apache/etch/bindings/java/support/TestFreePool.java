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

package org.apache.etch.bindings.java.support;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import org.apache.etch.bindings.java.support.Pool.PoolRunnable;
import org.junit.Test;


/**
 * Test of FreePool
 */
public class TestFreePool
{
	private final static int Q1 = 30; // 1 quanta of dependable clock tick
	private final static int Q2 = 60; // 2 quanta of dependable clock tick
	private final static int Q3 = 90; // 3 quanta of dependable clock tick
	
	/** @throws Exception */
	@Test
	public void close1() throws Exception
	{
		FreePool p = new FreePool( 2 );
		p.close();
	}

	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void close2() throws Exception
	{
		// free pool thread count exceeded or pool closed
		FreePool p = new FreePool( 2 );
		p.close();
		
		MyPoolRunnable r = new MyPoolRunnable( 0, false );
		p.run( r );
	}
	
	/** @throws Exception */
	@Test
	public void close3() throws Exception
	{
		FreePool p = new FreePool( 2 );
		p.close();
		p.close();
	}

	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void join1() throws Exception
	{
		// free pool thread count exceeded or pool closed
		FreePool p = new FreePool( 2 );
		p.join();
		
		MyPoolRunnable r = new MyPoolRunnable( 0, false );
		p.run( r );
	}

	/** @throws Exception */
	@Test
	public void join2() throws Exception
	{
		FreePool p = new FreePool( 2 );
		
		MyPoolRunnable r = new MyPoolRunnable( 0, false );
		assertFalse( r.done );
		assertNull( r.ex );
		
		p.run( r );
		
		Thread.sleep( Q1 );
		assertTrue( r.done );
		assertNull( r.ex );
		
		p.join();
	}

	/** @throws Exception */
	@Test
	public void join3() throws Exception
	{
		FreePool p = new FreePool( 2 );
		
		MyPoolRunnable r = new MyPoolRunnable( Q1, false );
		assertFalse( r.done );
		assertNull( r.ex );
		
		p.run( r );
		assertFalse( r.done );
		assertNull( r.ex );
		
		p.join();
		assertTrue( r.done );
		assertNull( r.ex );
	}
	
	/** @throws Exception */
	@Test
	public void run1() throws Exception
	{
		FreePool p = new FreePool( 2 );
		
		MyPoolRunnable r = new MyPoolRunnable( 0, false );
		assertFalse( r.done );
		assertNull( r.ex );
		
		p.run( r );
		
		Thread.sleep( Q1 );
		assertTrue( r.done );
		assertNull( r.ex );
	}
	
	/** @throws Exception */
	@Test
	public void run2() throws Exception
	{
		FreePool p = new FreePool( 2 );
		for (int i = 0; i < 100; i++)
		{
			MyPoolRunnable r = new MyPoolRunnable( 0, false );
			assertFalse( r.done );
			assertNull( r.ex );
			
			p.run( r );
			
			Thread.sleep( Q1 );
			assertTrue( r.done );
			assertNull( r.ex );
		}
	}
	
	/** @throws Exception */
	@Test
	public void run3() throws Exception
	{
		FreePool p = new FreePool( 2 );
		
		MyPoolRunnable r = new MyPoolRunnable( 0, true );
		assertFalse( r.done );
		assertNull( r.ex );
		
		p.run( r );
		
		Thread.sleep( Q1 );
		assertFalse( r.done );
		assertNotNull( r.ex );
	}
	
	/** @throws Exception */
	@Test
	public void run4() throws Exception
	{
		FreePool p = new FreePool( 2 );
		
		MyPoolRunnable r = new MyPoolRunnable( Q2, false );
		assertFalse( r.done );
		assertNull( r.ex );
		
		p.run( r );
		
		Thread.sleep( Q1 );
		assertFalse( r.done );
		assertNull( r.ex );
		
		Thread.sleep( Q3 );
		assertTrue( r.done );
		assertNull( r.ex );
	}
	
	/** @throws Exception */
	@Test
	public void run5() throws Exception
	{
		FreePool p = new FreePool( 2 );
		
		MyPoolRunnable r1 = new MyPoolRunnable( Q2, false );
		assertFalse( r1.done );
		assertNull( r1.ex );
		
		MyPoolRunnable r2 = new MyPoolRunnable( Q2, false );
		assertFalse( r2.done );
		assertNull( r2.ex );

		p.run( r1 );
		p.run( r2 );
		
		Thread.sleep( Q1 );
		assertFalse( r1.done );
		assertNull( r1.ex );
		
		assertFalse( r2.done );
		assertNull( r2.ex );
		
		Thread.sleep( Q3 );
		assertTrue( r1.done );
		assertNull( r1.ex );
		
		assertTrue( r2.done );
		assertNull( r2.ex );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void run6() throws Exception
	{
		// free pool thread count exceeded
		FreePool p = new FreePool( 2 );
		
		MyPoolRunnable r1 = new MyPoolRunnable( Q2, false );
		assertFalse( r1.done );
		assertNull( r1.ex );
		
		MyPoolRunnable r2 = new MyPoolRunnable( Q2, false );
		assertFalse( r2.done );
		assertNull( r2.ex );
		
		MyPoolRunnable r3 = new MyPoolRunnable( Q2, false );
		assertFalse( r3.done );
		assertNull( r3.ex );

		p.run( r1 );
		p.run( r2 );
		p.run( r3 );
	}
	
	/** @throws Exception */
	@Test
	public void run7() throws Exception
	{
		FreePool p = new FreePool( 2 );
		
		MyPoolRunnable r1 = new MyPoolRunnable( Q2, false );
		assertFalse( r1.done );
		assertNull( r1.ex );
		
		MyPoolRunnable r2 = new MyPoolRunnable( Q2, false );
		assertFalse( r2.done );
		assertNull( r2.ex );
		
		MyPoolRunnable r3 = new MyPoolRunnable( Q2, false );
		assertFalse( r3.done );
		assertNull( r3.ex );

		p.run( r1 );
		p.run( r2 );
		try { p.run( r3 ); } catch ( IllegalStateException e ) {}
		
		Thread.sleep( Q1 );
		assertFalse( r1.done );
		assertNull( r1.ex );
		
		assertFalse( r2.done );
		assertNull( r2.ex );
		
		assertFalse( r3.done );
		assertNull( r3.ex );
		
		Thread.sleep( Q3 );
		assertTrue( r1.done );
		assertNull( r1.ex );
		
		assertTrue( r2.done );
		assertNull( r2.ex );
		
		assertFalse( r3.done );
		assertNull( r3.ex );
	}
	
	private final class MyPoolRunnable implements PoolRunnable
	{
		public MyPoolRunnable( int delay, boolean excp )
		{
			this.delay = delay;
			this.excp = excp;
		}
		
		private final int delay;
		
		private final boolean excp;
		
		public boolean done;
		
		public Exception ex;

		public void run() throws Exception
		{
			if (delay > 0)
				Thread.sleep( delay );
			if (excp)
				throw new RuntimeException();
			done = true;
		}
		
		public void exception( Exception e )
		{
			ex = e;
		}
	}
}
