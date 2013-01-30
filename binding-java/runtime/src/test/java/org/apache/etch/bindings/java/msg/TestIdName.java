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
import static org.junit.Assert.assertTrue;

import org.apache.etch.util.Hash;
import org.junit.Test;


/** */
public class TestIdName
{
	/** */
	@Test
	public void getId()
	{
		assertEquals( 1, new IdName( 1, "foo" ).getId() );
		assertEquals( 1, new IdName( 1, "bar" ).getId() );
		assertEquals( 2, new IdName( 2, "foo" ).getId() );
		assertEquals( 2, new IdName( 2, "bar" ).getId() );
	}

	/** */
	@Test
	public void getName()
	{
		assertEquals( "foo", new IdName( 1, "foo" ).getName() );
		assertEquals( "bar", new IdName( 1, "bar" ).getName() );
		assertEquals( "foo", new IdName( 2, "foo" ).getName() );
		assertEquals( "bar", new IdName( 2, "bar" ).getName() );
	}

	/** */
	@Test
	public void xtoString()
	{
		assertEquals( "foo(1)", new IdName( 1, "foo" ).toString() );
		assertEquals( "bar(1)", new IdName( 1, "bar" ).toString() );
		assertEquals( "foo(2)", new IdName( 2, "foo" ).toString() );
		assertEquals( "bar(2)", new IdName( 2, "bar" ).toString() );
	}

	/** */
	@Test
	public void equalsObject()
	{
		assertTrue( new IdName( 1, "foo" ).equals( new IdName( 1, "foo" ) ) );
		assertTrue( new IdName( 1, "bar" ).equals( new IdName( 1, "bar" ) ) );
		assertTrue( new IdName( 2, "foo" ).equals( new IdName( 2, "foo" ) ) );
		assertTrue( new IdName( 2, "bar" ).equals( new IdName( 2, "bar" ) ) );
		
		assertFalse( new IdName( 1, "foo" ).equals( new IdName( 2, "foo" ) ) ); // id changed
		assertFalse( new IdName( 1, "foo" ).equals( new IdName( 1, "bar" ) ) ); // name changed
		assertFalse( new IdName( 1, "foo" ).equals( new IdName( 2, "bar" ) ) ); // id and name changed
		
		assertFalse( new IdName( 1, "foo" ).equals( null ) );
		assertFalse( new IdName( 1, "foo" ).equals( 1 ) );
		assertFalse( new IdName( 1, "foo" ).equals( true ) );
		assertFalse( new IdName( 1, "foo" ) {}.equals( new IdName( 1, "foo" ) {} ) ); // different subclasses
	}

	/** */
	@Test
	public void hashCodeObject()
	{
		// you cannot really predict when hashCode will not be equal. the
		// rule is that it must be equal when the objects compare equal.
		assertEquals( new IdName( 1, "foo" ).hashCode(), new IdName( 1, "foo" ).hashCode() );
		assertEquals( new IdName( 2, "foo" ).hashCode(), new IdName( 2, "foo" ).hashCode() );
		assertEquals( new IdName( 1, "bar" ).hashCode(), new IdName( 1, "bar" ).hashCode() );
		assertEquals( new IdName( 2, "bar" ).hashCode(), new IdName( 2, "bar" ).hashCode() );
	}

	/** */
	@Test
	public void hash()
	{
		assertEquals( 5381, Hash.hash( "" ) );
		assertEquals( 352988316, Hash.hash( "a" ) );
		assertEquals( 1511848646, Hash.hash( "ab" ) );
		assertEquals( 669497117, Hash.hash( "abc" ) );
		assertEquals( -1994190713, Hash.hash( "abcd" ) );
		assertEquals( -802680418, Hash.hash( "abcde" ) );
		assertEquals( 1266308680, Hash.hash( "abcdef" ) );
		assertEquals( -379372513, Hash.hash( "abcdefg" ) );
		assertEquals( -1416967159, Hash.hash( "abcdefgh" ) );
		assertEquals( 53556896, Hash.hash( "abcdefghi" ) );
		assertEquals( -4427318, Hash.hash( "abcdefghij" ) );
	}
}
