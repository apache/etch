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

import java.util.Date;

import org.apache.etch.bindings.java.support.Validator_boolean;
import org.apache.etch.bindings.java.support.Validator_int;
import org.junit.Test;


/** Test ComboValidator */
public class TestComboValidator
{
	/** @throws Exception */
	@Test
	public void test() throws Exception
	{
		test( 0, "(int[0] OR boolean[0])", true, 123456789012L );
		test( 0, "(int[0] OR boolean[0])", false, -123456789012L );
// TODO		test( 0, "(int[0] OR boolean[0])", TypeCode.BYTE1, 0, (long) 3 );
		test( 0, "(int[0] OR boolean[0])", 1, (float) 4 );
		test( 0, "(int[0] OR boolean[0])", -1, (double) 5 );
		test( 0, "(int[0] OR boolean[0])", 127, "abc" );
		test( 0, "(int[0] OR boolean[0])", -128, new Object() );
		test( 0, "(int[0] OR boolean[0])", 32767, new int[] {} );
		test( 0, "(int[0] OR boolean[0])", -32768, new boolean[] {} );
		test( 0, "(int[0] OR boolean[0])", 1000000000, new Date() );
		test( 0, "(int[0] OR boolean[0])", -1000000000, int.class );
		test( 1, "(int[1] OR boolean[1])", new int[] {}, 0 );
		test( 1, "(int[1] OR boolean[1])", new boolean[] {}, true );
	}
	
	private void test( int n, String s, Object good, Object bad )
	{
		Validator_int v1 = Validator_int.get( n );
		Validator_boolean v2 = Validator_boolean.get( n );
		ComboValidator v = new ComboValidator( v1, v2 );
		
		assertEquals( s, v.toString() );
		assertTrue( v.validate( good ) );
		assertFalse( v.validate( bad ) );
		if (n > 0)
		{
			v.elementValidator();
		}
	}
	
	/** @throws Exception */
	@Test
	public void test2() throws Exception
	{
		Validator v1 = Validator_int.get( 1 );
		Validator v2 = Validator_boolean.get( 0 );
		Validator v = new ComboValidator( v1, v2 );

		assertTrue( v.validate( new int[] {} ) );
		assertTrue( v.validate( true ) );
		assertTrue( v.validate( false ) );
		assertFalse( v.validate( 0 ) );
		
		v = v.elementValidator();

		assertFalse( v.validate( new int[] {} ) );
		assertFalse( v.validate( true ) );
		assertFalse( v.validate( false ) );
		assertTrue( v.validate( 0 ) );
	}
	
	/** @throws Exception */
	@Test
	public void test3() throws Exception
	{
		Validator v1 = Validator_boolean.get( 0 );
		Validator v2 = Validator_int.get( 1 );
		Validator v = new ComboValidator( v1, v2 );

		assertTrue( v.validate( new int[] {} ) );
		assertTrue( v.validate( true ) );
		assertTrue( v.validate( false ) );
		assertFalse( v.validate( 0 ) );
		
		v = v.elementValidator();

		assertFalse( v.validate( new int[] {} ) );
		assertFalse( v.validate( true ) );
		assertFalse( v.validate( false ) );
		assertTrue( v.validate( 0 ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void test4() throws Exception
	{
		Validator v1 = Validator_boolean.get( 0 );
		Validator v2 = Validator_int.get( 1 );
		Validator v = new ComboValidator( v1, v2 );

		assertTrue( v.validate( new int[] {} ) );
		assertTrue( v.validate( true ) );
		assertTrue( v.validate( false ) );
		assertFalse( v.validate( 0 ) );
		
		v = v.elementValidator();

		assertFalse( v.validate( new int[] {} ) );
		assertFalse( v.validate( true ) );
		assertFalse( v.validate( false ) );
		assertTrue( v.validate( 0 ) );
		
		v = v.elementValidator();
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void test5() throws Exception
	{
		Validator v1 = Validator_boolean.get( 1 );
		Validator v2 = Validator_int.get( 1 );
		Validator v = new ComboValidator( v1, v2 );

		assertTrue( v.validate( new boolean[] {} ) );
		assertTrue( v.validate( new int[] {} ) );
		
		v = v.elementValidator();

		assertTrue( v.validate( true ) );
		assertTrue( v.validate( false ) );
		assertTrue( v.validate( 0 ) );
		
		v = v.elementValidator();
	}
}
