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

package org.apache.etch.util.core.nio;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import java.io.IOException;
import java.nio.ByteBuffer;

import org.apache.etch.util.core.nio.ByteBufferPool.Notify;
import org.junit.Test;




/** Test of ByteBufferPool */
public class TestByteBufferPool
{
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void construct1() throws Exception
	{
		// bufferSize <= 0
		new ByteBufferPool( -1, -1, -1, -1, -1 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void construct2() throws Exception
	{
		// bufferSize <= 0
		new ByteBufferPool( 0, -1, -1, -1, -1 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void construct3() throws Exception
	{
		// historyInterval < 0
		new ByteBufferPool( 4096, -1, -1, -1, -1 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void construct4() throws Exception
	{
		// min < 0
		new ByteBufferPool( 4096, -1, -1, 0, -1 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void construct5() throws Exception
	{
		// limit < min
		new ByteBufferPool( 4096, 0, -1, 0, -1 );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void construct6() throws Exception
	{
		// len < 0
		new ByteBufferPool( 4096, 0, 10, 1, -1 );
	}

	/** @throws Exception */
	@Test
	public void construct7() throws Exception
	{
		ByteBufferPool p = new ByteBufferPool( 1, 2, 3, 4, 5 );
		assertEquals( 1, p.bufferSize() );
		assertEquals( 2, p.min() );
		assertEquals( 3, p.limit() );
		assertEquals( 4, p.interval() );
		assertEquals( 5, p.length() );
		p.shutdown();
	}

	/** @throws Exception */
	@Test
	public void construct8() throws Exception
	{
		ByteBufferPool p = new ByteBufferPool( 101, 103, 105, 107, 109 );
		assertEquals( 101, p.bufferSize() );
		assertEquals( 103, p.min() );
		assertEquals( 105, p.limit() );
		assertEquals( 107, p.interval() );
		assertEquals( 109, p.length() );
		p.shutdown();
	}

	/** @throws Exception */
	@Test
	public void construct9() throws Exception
	{
		ByteBufferPool p = new ByteBufferPool( 101, 103, 105, 0, 109 );
		assertEquals( 101, p.bufferSize() );
		assertEquals( 103, p.min() );
		assertEquals( 105, p.limit() );
		assertEquals( 0, p.interval() );
		assertEquals( 0, p.length() );
		p.shutdown();
	}

	/** @throws Exception */
	@Test
	public void construct10() throws Exception
	{
		ByteBufferPool p = new ByteBufferPool( 101, 103, 105, 107, 0 );
		assertEquals( 101, p.bufferSize() );
		assertEquals( 103, p.min() );
		assertEquals( 105, p.limit() );
		assertEquals( 0, p.interval() );
		assertEquals( 0, p.length() );
		p.shutdown();
	}

	/** @throws Exception */
	@Test
	public void construct11() throws Exception
	{
		ByteBufferPool p = new ByteBufferPool( 101, 103, 105, 0, 0 );
		assertEquals( 101, p.bufferSize() );
		assertEquals( 103, p.min() );
		assertEquals( 105, p.limit() );
		assertEquals( 0, p.interval() );
		assertEquals( 0, p.length() );
		p.shutdown();
	}
	
	/** @throws Exception */
	@Test
	public void alloc1() throws Exception
	{
		ByteBufferPool p = new ByteBufferPool( 4096, 0, 10, 0, 0 );
		ByteBuffer b = p.alloc( null );
		assertNotNull( b );
		assertEquals( 4096, b.limit() );
		assertEquals( 1, p.used() );
		p.release( b );
		assertEquals( 0, p.used() );
	}
	
	/** @throws Exception */
	@Test
	public void alloc2() throws Exception
	{
		ByteBufferPool p = new ByteBufferPool( 1024, 0, 10, 0, 0 );
		ByteBuffer b = p.alloc( null );
		assertNotNull( b );
		assertEquals( 1024, b.limit() );
		assertEquals( 1, p.used() );
		p.release( b );
		assertEquals( 0, p.used() );
	}
	
	/** @throws Exception */
	@Test
	public void alloc3() throws Exception
	{
		ByteBufferPool p = new ByteBufferPool( 4096, 0, 10, 0, 0 );
		ByteBuffer[] b = p.alloc( null, 2 );
		assertNotNull( b );
		assertEquals( 2, b.length );
		assertNotNull( b[0] );
		assertEquals( 4096, b[0].limit() );
		assertNotNull( b[1] );
		assertEquals( 4096, b[1].limit() );
		assertEquals( 2, p.used() );
		p.release( b );
		assertEquals( 0, p.used() );
	}
	
	/** @throws Exception */
	@Test( expected = IOException.class )
	public void alloc4() throws Exception
	{
		// out of buffers
		ByteBufferPool p = new ByteBufferPool( 1024, 0, 1, 0, 0 );
		p.alloc( null );
		p.alloc( null );
	}
	
	/** @throws Exception */
	@Test
	public void alloc5() throws Exception
	{
		// slower because of no pooling.
		ByteBufferPool p = new ByteBufferPool( 1024, 0, 1, 0, 0 );
		for (int i = 0; i < 100000; i++)
			p.release( p.alloc( null ) );
		assertEquals( 0, p.used() );
	}
	
	/** @throws Exception */
	@Test
	public void alloc6() throws Exception
	{
		// faster because of pooling.
		ByteBufferPool p = new ByteBufferPool( 1024, 1, 1, 0, 0 );
		for (int i = 0; i < 100000; i++)
			p.release( p.alloc( null ) );
		assertEquals( 0, p.used() );
	}
	
	/** @throws Exception */
	@Test
	public void notify1() throws Exception
	{
		ByteBufferPool p = new ByteBufferPool( 1024, 1, 1, 0, 0 );
		MyNotify n1 = new MyNotify();
		MyNotify n2 = new MyNotify();
		
		ByteBuffer b1 = p.alloc( n1 );
		assertNotNull( b1 );
		assertEquals( 1, p.used() );
		Thread.sleep( 15 );
		assertFalse( n1.available );
		assertFalse( n2.available );
		
		ByteBuffer b2 = p.alloc( n2 );
		assertNull( b2 );
		assertEquals( 1, p.used() );
		Thread.sleep( 15 );
		assertFalse( n1.available );
		assertFalse( n2.available );
		
		// the alloc of b2 failed, but n2 should be registered to be notified
		// when a block is freed. release of b1 should notify n2 of availability
		
		p.release( b1 );
		b1 = null;
		assertEquals( 0, p.used() );
		Thread.sleep( 15 );
		assertFalse( n1.available );
		assertTrue( n2.available );
		
		n2.available = false;
		b2 = p.alloc( n2 );
		assertNotNull( b2 );
		assertEquals( 1, p.used() );
		Thread.sleep( 15 );
		assertFalse( n1.available );
		assertFalse( n2.available );
		
		p.release( b2 );
		b2 = null;
		assertEquals( 0, p.used() );
		Thread.sleep( 15 );
		assertFalse( n1.available );
		assertFalse( n2.available );
	}
	
	/** */
	public static class MyNotify implements Notify
	{
		/** */
		public boolean available;
		public void bufferAvailable( ByteBufferPool pool )
		{
			available = true;
		}
	}
}
