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
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import org.junit.Test;



/** Test of History */
public class TestHistory
{
	/** @throws Exception */
	@Test
	public void construct1() throws Exception
	{
		History h = new History( 1, 2, 3 );
		assertEquals( 1, h.min() );
		assertEquals( 2, h.limit() );
		assertEquals( 3, h.length() );
	}

	/** @throws Exception */
	@Test
	public void construct2() throws Exception
	{
		History h = new History( 102, 119, 7 );
		assertEquals( 102, h.min() );
		assertEquals( 119, h.limit() );
		assertEquals( 7, h.length() );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void construct3() throws Exception
	{
		// min < 0
		new History( -1, 2, 3 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void construct4() throws Exception
	{
		// limit < min
		new History( 0, -1, 3 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void construct5() throws Exception
	{
		// len < 0
		new History( 0, 0, -1 );
	}
	
	/** @throws Exception */
	@Test
	public void used1() throws Exception
	{
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		assertTrue( h.used( 1 ) );
		checkUsedAlloc( h, 1, 0 );
		assertTrue( h.used( -1 ) );
		checkUsedAlloc( h, 0, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void used2() throws Exception
	{
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		assertTrue( h.used( 2 ) );
		checkUsedAlloc( h, 2, 0 );
		assertTrue( h.used( -2 ) );
		checkUsedAlloc( h, 0, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void used3() throws Exception
	{
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		assertTrue( h.used( 1 ) );
		checkUsedAlloc( h, 1, 0 );
		assertTrue( h.used( 1 ) );
		checkUsedAlloc( h, 2, 0 );
		assertTrue( h.used( -1 ) );
		checkUsedAlloc( h, 1, 0 );
		assertTrue( h.used( -1 ) );
		checkUsedAlloc( h, 0, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void used4() throws Exception
	{
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		// try to overflow
		assertFalse( h.used( 11 ) );
		checkUsedAlloc( h, 0, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void used5() throws Exception
	{
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		assertTrue( h.used( 1 ) );
		checkUsedAlloc( h, 1, 0 );
		assertTrue( h.used( 9 ) );
		checkUsedAlloc( h, 10, 0 );
		// try to overflow
		assertFalse( h.used( 1 ) );
		checkUsedAlloc( h, 10, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void used6() throws Exception
	{
		// newUsed < 0
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		h.used( -1 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void used7() throws Exception
	{
		// newUsed < 0
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		h.used( 1 );
		checkUsedAlloc( h, 1, 0 );
		h.used( -2 );
	}
	
	/** @throws Exception */
	@Test
	public void used8() throws Exception
	{
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		h.used( 1 );
		checkUsedAlloc( h, 1, 0 );
		try
		{
			h.used( -2 );
			fail( "never reached" );
		}
		catch ( IllegalStateException e )
		{
			// make sure used was not adjusted...
			checkUsedAlloc( h, 1, 0 );
		}
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void used9() throws Exception
	{
		// k == 0
		History h = new History( 0, 10, 5 );
		h.used( 0 );
	}
	
	/** @throws Exception */
	@Test
	public void alloc1() throws Exception
	{
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		h.alloc( 1 );
		checkUsedAlloc( h, 0, 1 );
		h.alloc( -1 );
		checkUsedAlloc( h, 0, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void alloc2() throws Exception
	{
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		h.alloc( 2 );
		checkUsedAlloc( h, 0, 2 );
		h.alloc( -2 );
		checkUsedAlloc( h, 0, 0 );
	}
	
	/** @throws Exception */
	@Test
	public void alloc3() throws Exception
	{
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		h.alloc( 1 );
		checkUsedAlloc( h, 0, 1 );
		h.alloc( 1 );
		checkUsedAlloc( h, 0, 2 );
		h.alloc( -1 );
		checkUsedAlloc( h, 0, 1 );
		h.alloc( -1 );
		checkUsedAlloc( h, 0, 0 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void alloc4() throws Exception
	{
		// newAlloc > limit
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		// try to overflow
		h.alloc( 11 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void alloc5() throws Exception
	{
		// newAlloc > limit
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		h.alloc( 1 );
		checkUsedAlloc( h, 0, 1 );
		h.alloc( 9 );
		checkUsedAlloc( h, 0, 10 );
		// try to overflow
		h.alloc( 1 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void alloc6() throws Exception
	{
		// newAlloc < 0
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		// try to underflow
		h.alloc( -1 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void alloc7() throws Exception
	{
		// newAlloc < 0
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		h.alloc( 1 );
		checkUsedAlloc( h, 0, 1 );
		// try to underflow
		h.alloc( -2 );
	}
	
	/** @throws Exception */
	@Test
	public void alloc8() throws Exception
	{
		History h = new History( 0, 10, 5 );
		checkUsedAlloc( h, 0, 0 );
		h.alloc( 1 );
		checkUsedAlloc( h, 0, 1 );
		try
		{
			// try to underflow
			h.alloc( -2 );
			fail( "never reached" );
		}
		catch ( IllegalStateException e )
		{
			// make sure alloc was not adjusted...
			checkUsedAlloc( h, 0, 1 );
		}
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void alloc9() throws Exception
	{
		// k == 0
		History h = new History( 0, 10, 5 );
		h.alloc( 0 );
	}
	
	/** @throws Exception */
	@Test
	public void suggested1() throws Exception
	{
		History h = new History( 0, 10, 5 );
		assertEquals( 0, h.suggested() );
		h.tickle();
		assertEquals( 0, h.suggested() );
		h.init();
		assertEquals( 0, h.suggested() );
	}
	
	/** @throws Exception */
	@Test
	public void suggested2() throws Exception
	{
		History h = new History( 2, 10, 5 );
		assertEquals( 2, h.suggested() );
		h.tickle();
		assertEquals( 2, h.suggested() );
		h.init();
		assertEquals( 2, h.suggested() );
	}
	
	/** @throws Exception */
	@Test
	public void suggested3() throws Exception
	{
		History h = new History( 0, 10, 5 );
		assertEquals( 0, h.suggested() );
		
		assertTrue( h.used( 10 ) );
		assertEquals( 10, h.getUsed() );
		
		assertEquals( 0, h.suggested() ); // avg(0, 0, 0, 0, 0) == 0
		h.tickle();
		assertEquals( 2, h.suggested() ); // avg(10, 0, 0, 0, 0) == 2
		h.tickle();
		assertEquals( 4, h.suggested() ); // avg(10, 10, 0, 0, 0) == 4
		h.tickle();
		assertEquals( 6, h.suggested() ); // avg(10, 10, 10, 0, 0) == 6
		h.tickle();
		assertEquals( 8, h.suggested() ); // avg(10, 10, 10, 10, 0) == 8
		h.tickle();
		assertEquals( 10, h.suggested() ); // avg(10, 10, 10, 10, 10) == 10
		h.tickle();
		assertEquals( 10, h.suggested() ); // avg(10, 10, 10, 10, 10) == 10
		
		assertTrue( h.used( -10 ) );
		assertEquals( 0, h.getUsed() );
		
		h.tickle();
		assertEquals( 10, h.suggested() ); // avg(10, 10, 10, 10, 10) == 10
		h.tickle();
		assertEquals( 8, h.suggested() ); // avg(0, 10, 10, 10, 10) == 8
		h.tickle();
		assertEquals( 6, h.suggested() ); // avg(0, 0, 10, 10, 10) == 6
		h.tickle();
		assertEquals( 4, h.suggested() ); // avg(0, 0, 0, 10, 10) == 4
		h.tickle();
		assertEquals( 2, h.suggested() ); // avg(0, 0, 0, 0, 10) == 2
		h.tickle();
		assertEquals( 0, h.suggested() ); // avg(0, 0, 0, 0, 0) == 0
	}
	
	/** @throws Exception */
	@Test
	public void suggested4() throws Exception
	{
		History h = new History( 2, 10, 5 );
		assertEquals( 2, h.suggested() );
		
		assertTrue( h.used( 10 ) );
		assertEquals( 10, h.getUsed() );
		
		assertEquals( 2, h.suggested() ); // avg(2, 2, 2, 2, 2) == 2
		h.tickle();
		assertEquals( 3, h.suggested() ); // avg(10, 2, 2, 2, 2) == 3
		h.tickle();
		assertEquals( 5, h.suggested() ); // avg(10, 10, 2, 2, 2) == 5
		h.tickle();
		assertEquals( 6, h.suggested() ); // avg(10, 10, 10, 2, 2) == 6
		h.tickle();
		assertEquals( 8, h.suggested() ); // avg(10, 10, 10, 10, 2) == 8
		h.tickle();
		assertEquals( 10, h.suggested() ); // avg(10, 10, 10, 10, 10) == 10
		h.tickle();
		assertEquals( 10, h.suggested() ); // avg(10, 10, 10, 10, 10) == 10
		
		assertTrue( h.used( -10 ) );
		assertEquals( 0, h.getUsed() );
		
		h.tickle();
		assertEquals( 10, h.suggested() ); // avg(10, 10, 10, 10, 10) == 10
		h.tickle();
		assertEquals( 8, h.suggested() ); // avg(2, 10, 10, 10, 10) == 8
		h.tickle();
		assertEquals( 6, h.suggested() ); // avg(2, 2, 10, 10, 10) == 6
		h.tickle();
		assertEquals( 5, h.suggested() ); // avg(2, 2, 2, 10, 10) == 5
		h.tickle();
		assertEquals( 3, h.suggested() ); // avg(2, 2, 2, 2, 10) == 3
		h.tickle();
		assertEquals( 2, h.suggested() ); // avg(2, 2, 2, 2, 2) == 2
	}
	
	/** @throws Exception */
	@Test
	public void suggested5() throws Exception
	{
		History h = new History( 0, 10, 0 );
		assertEquals( 0, h.suggested() );
		assertTrue( h.used( 5 ) );
		assertEquals( 0, h.suggested() );
		h.tickle();
		assertEquals( 0, h.suggested() );
	}
	
	private void checkUsedAlloc( History h, int u, int a )
	{
		assertEquals( u, h.getUsed() );
		assertEquals( a, h.getAlloc() );
	}
}
