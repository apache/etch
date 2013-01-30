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
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.apache.etch.bindings.java.support.DummyValueFactory;
import org.apache.etch.bindings.java.support.Validator_boolean;
import org.apache.etch.bindings.java.support.Validator_int;
import org.apache.etch.bindings.java.support.Validator_string;
import org.junit.Test;


/** Tests StructValue */
public class TestStructValue
{
	private final ValueFactory vf = new DummyValueFactory();
	
	private Type mt1 = new Type( "one" );
	private Type mt2 = new Type( "two" );
	private Field mf1 = new Field( "f1" );
	private Field mf2 = new Field( "f2" );
	private Field mf3 = new Field( "f3" );
	private Field mf4 = new Field( "f4" );
	private Field mf5 = new Field( "f5" );
	private Field mf6 = new Field( "f6" );

	{
		mt1.putValidator( mf1, Validator_boolean.get( 0 ) );
		mt1.putValidator( mf2, Validator_boolean.get( 1 ) );
		mt1.putValidator( mf3, Validator_int.get( 0 ) );
		mt1.putValidator( mf4, Validator_int.get( 1 ) );
		mt1.putValidator( mf5, Validator_string.get( 0 ) );
		mt1.putValidator( mf6, Validator_string.get( 1 ) );
	}

	/** @throws Exception */
	@Test
	public void test2str() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		assertEquals( "one(785945377): {}", sv.toString() );

		sv = new StructValue( mt2, vf );
		assertEquals( "two(827843303): {}", sv.toString() );

		sv = new StructValue( mt1, vf );
		sv.put( mf1, true );
		assertEquals( "one(785945377): {f1(1512176592)=true}", sv.toString() );

		sv = new StructValue( mt1, vf );
		sv.put( mf3, 23 );
		assertEquals( "one(785945377): {f3(1512176594)=23}", sv.toString() );

		sv = new StructValue( mt1, vf );
		sv.put( mf1, false );
		sv.put( mf3, 74 );
		assertTrue("one(785945377): {f1(1512176592)=false, f3(1512176594)=74}".equals(sv.toString()) ||
			"one(785945377): {f3(1512176594)=74, f1(1512176592)=false}".equals(sv.toString()));
	}

	/** */
	@Test
	public void getType()
	{
		StructValue sv = new StructValue( mt1, vf );
		assertSame( mt1, sv.type() );

		sv = new StructValue( mt2, vf );
		assertSame( mt2, sv.type() );
	}

	/** */
	@Test
	public void isType()
	{
		StructValue sv = new StructValue( mt1, vf );
		assertTrue( sv.isType( mt1 ) );
		assertFalse( sv.isType( mt2 ) );

		sv = new StructValue( mt2, vf );
		assertTrue( sv.isType( mt2 ) );
		assertFalse( sv.isType( mt1 ) );
	}

	/** */
	@Test
	public void checkType1()
	{
		new StructValue( mt1, vf ).checkType( mt1 );
		new StructValue( mt2, vf ).checkType( mt2 );
	}

	/** */
	@Test( expected = IllegalArgumentException.class )
	public void checkType2()
	{
		new StructValue( mt1, vf ).checkType( mt2 );
	}

	/** */
	@Test( expected = IllegalArgumentException.class )
	public void checkType()
	{
		new StructValue( mt2, vf ).checkType( mt1 );
	}

	/** @throws Exception */
	@Test
	public void get() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		assertTrue( sv.isEmpty() );
		assertEquals( 0, sv.size() );
		assertNull( sv.get( mf1 ) );

		sv.put( mf1, true );
		assertFalse( sv.isEmpty() );
		assertEquals( 1, sv.size() );
		assertEquals( true, sv.get( mf1 ) );

		sv.put( mf1, false );
		assertFalse( sv.isEmpty() );
		assertEquals( 1, sv.size() );
		assertEquals( false, sv.get( mf1 ) );

		sv.put( mf1, null );
		assertTrue( sv.isEmpty() );
		assertEquals( 0, sv.size() );
		assertNull( sv.get( mf1 ) );
	}

	/** @throws Exception */
	@Test
	public void remove() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		assertTrue( sv.isEmpty() );
		assertEquals( 0, sv.size() );
		assertNull( sv.get( mf1 ) );

		sv.remove( mf1 );
		assertTrue( sv.isEmpty() );
		assertEquals( 0, sv.size() );
		assertNull( sv.get( mf1 ) );

		sv.put( mf1, true );
		assertFalse( sv.isEmpty() );
		assertEquals( 1, sv.size() );
		assertEquals( true, sv.get( mf1 ) );

		sv.remove( mf1 );
		assertTrue( sv.isEmpty() );
		assertEquals( 0, sv.size() );
		assertNull( sv.get( mf1 ) );

		sv.remove( mf1 );
		assertTrue( sv.isEmpty() );
		assertEquals( 0, sv.size() );
		assertNull( sv.get( mf1 ) );
	}

	/** @throws Exception */
	@Test
	public void put1() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		sv.put( mf1, false );
		sv.put( mf2, new boolean[] { true, false } );
		sv.put( mf3, 1 );
		sv.put( mf4, new int[] { 1, 2 } );
		sv.put( mf5, "a" );
		sv.put( mf6, new String[] { "a", "b" } );
		assertEquals( 6, sv.size() );
	}

	/** @throws Exception */
	@Test
	public void put2() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		sv.put( mf1, null );
		sv.put( mf2, null );
		sv.put( mf3, null );
		sv.put( mf4, null );
		sv.put( mf5, null );
		sv.put( mf6, null );
		assertEquals( 0, sv.size() );
	}

	/** @throws Exception */
	@Test
	public void put3() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		sv.put( mf1, false );
		sv.put( mf2, new boolean[] { true, false } );
		sv.put( mf3, 1 );
		sv.put( mf4, new int[] { 1, 2 } );
		sv.put( mf5, "a" );
		sv.put( mf6, new String[] { "a", "b" } );
		assertEquals( 6, sv.size() );

		// now "remove" them all

		sv.put( mf1, null );
		sv.put( mf2, null );
		sv.put( mf3, null );
		sv.put( mf4, null );
		sv.put( mf5, null );
		sv.put( mf6, null );
		assertEquals( 0, sv.size() );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put5() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		sv.put( mf1, (byte) 1 ); // wrong type
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put6() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		sv.put( mf1, (short) 1 ); // wrong type
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put7() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		sv.put( mf1, 1 ); // wrong type
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put8() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		sv.put( mf1, (long) 1 ); // wrong type
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put9() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		sv.put( mf1, (float) 1.2 ); // wrong type
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put10() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		sv.put( mf1, 1.2 ); // wrong type
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put11() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		sv.put( mf1, "a" ); // wrong type
	}

	/** @throws Exception */
	@Test
	public void put12() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		sv.put( mf2, null );
	}

	/** @throws Exception */
	@Test
	public void put13() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		sv.put( mf2, new boolean[] {} );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put14() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		sv.put( mf2, true );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void put15() throws Exception
	{
		StructValue sv = new StructValue( mt1, vf );
		sv.put( mf2, new boolean[][] {} );
	}

	/** */
	@Test
	public void iterator()
	{
		StructValue sv = new StructValue( mt1, vf );
		assertFalse( sv.iterator().hasNext() );

		sv.put( mf1, true );

		Iterator<Map.Entry<Field, Object>> i = sv.iterator();
		assertTrue( i.hasNext() );
		Map.Entry<Field, Object> me = i.next();
		assertSame( mf1, me.getKey() );
		assertEquals( true, me.getValue() );
		assertFalse( i.hasNext() );

		Map<Field, Object> vals = new HashMap<Field, Object>();
		vals.put( mf1, true );
		vals.put( mf3, 2 );
		vals.put( mf5, "a" );

		sv.put( mf3, 2 );
		sv.put( mf5, "a" );

		i = sv.iterator();
		while (i.hasNext())
		{
			me = i.next();
			Field key = me.getKey();
			assertTrue( vals.containsKey( key ) );
			assertEquals( vals.remove( key ), me.getValue() );
		}
		assertTrue( vals.isEmpty() );
	}
}
