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

package org.apache.etch.bindings.java.msg;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNotSame;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;

import java.util.HashSet;
import java.util.Set;

import org.junit.Before;
import org.junit.Test;

/** */
public class TestIdNameMap
{
	/** */
	@Before
	public void init()
	{
		map = new IdNameMap<IdName>()
		{
			@Override
			public IdName makeNew( String name )
			{
				return new IdName( name );
			}
		};
		
		a = map.add( new IdName( 1, "a" ) );
		b = map.add( new IdName( 2, "b" ) );
	}
	
	private IdNameMap<IdName> map;
	
	private IdName a;
	
	private IdName b;

	/** @throws Exception */
	@Test
	public void check_a() throws Exception
	{
		assertEquals( 2, map.size() );
		check( "a", 1, a );
		assertEquals( 2, map.size() );
	}
	
	private void check( String name, int id, IdName idName )
	{
		IdName x = map.get( id );
		assertNotNull( x );
		assertSame( idName, x );
		assertEquals( id, x.getId() );
		assertEquals( name, x.getName() );
		
		x = map.get( name );
		assertNotNull( x );
		assertSame( idName, x );
		assertEquals( id, x.getId() );
		assertEquals( name, x.getName() );
	}
	
	/** @throws Exception */
	@Test
	public void check_b() throws Exception
	{
		assertEquals( 2, map.size() );
		check( "b", 2, b );
		assertEquals( 2, map.size() );
	}
	
	/** @throws Exception */
	@Test
	public void get_3() throws Exception
	{
		assertEquals( 2, map.size() );
		IdName x = map.get( 3 );
		assertNull( x );
		assertEquals( 2, map.size() );
	}
	
	/** @throws Exception */
	@Test
	public void get_3_locked() throws Exception
	{
		assertEquals( 2, map.size() );
		map.lock();
		IdName x = map.get( 3 );
		assertNull( x );
		assertEquals( 2, map.size() );
	}
	
	/** @throws Exception */
	@Test
	public void get_c() throws Exception
	{
		assertEquals( 2, map.size() );
		IdName c = map.get( "c" );
		check( "c", 352988318, c );
		assertEquals( 3, map.size() );
	}
	
	/** @throws Exception */
	@Test
	public void get_d_locked() throws Exception
	{
		assertEquals( 2, map.size() );
		map.lock();
		IdName d = map.get( "d" );
		assertNull( d );
		assertEquals( 2, map.size() );
	}

	/** @throws Exception */
	@Test( expected = IllegalStateException.class )
	public void add_3_c_locked() throws Exception
	{
		assertEquals( 2, map.size() );
		map.lock();
		map.add( new IdName( 3, "c" ) );
		assertEquals( 2, map.size() );
	}
	
	/** @throws Exception */
	@Test( expected=IllegalArgumentException.class )
	public void add_id_coll() throws Exception
	{
		assertEquals( 2, map.size() );
		map.add( new IdName( 1, "c" ) );
		assertEquals( 2, map.size() );
	}
	
	/** @throws Exception */
	@Test( expected=IllegalArgumentException.class )
	public void add_name_coll() throws Exception
	{
		assertEquals( 2, map.size() );
		map.add( new IdName( 3, "a" ) );
		assertEquals( 2, map.size() );
	}
	
	/** @throws Exception */
	@Test( expected=IllegalArgumentException.class )
	public void add_id_name_coll() throws Exception
	{
		assertEquals( 2, map.size() );
		map.add( new IdName( 1, "a" ) );
		assertEquals( 2, map.size() );
	}
	
	/** @throws Exception */
	@Test
	public void values1() throws Exception
	{
		Set<IdName> actual = map.values();
		assertEquals( 2, actual.size() );
		Set<IdName> expected = new HashSet<IdName>();
		expected.add( new IdName( 1, "a" ) );
		expected.add( new IdName( 2, "b" ) );
		assertEquals( expected, actual );
		
		map.get( "c" );
		
		actual = map.values();
		assertEquals( 3, actual.size() );
		expected = new HashSet<IdName>();
		expected.add( new IdName( 1, "a" ) );
		expected.add( new IdName( 2, "b" ) );
		expected.add( new IdName( "c" ) );
		assertEquals( expected, actual );
	}
	
	/** @throws Exception */
	@Test
	public void values2() throws Exception
	{
		// code in value factory depends upon the fact that the
		// set returned from values is a copy.
		assertNotSame( map.values(), map.values() );
	}
}
