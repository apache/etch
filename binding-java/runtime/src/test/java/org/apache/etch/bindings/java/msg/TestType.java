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
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;

import org.apache.etch.bindings.java.support.Validator_int;
import org.apache.etch.util.Hash;
import org.junit.Test;


/** */
public class TestType
{
	/** */
	@Test
	public void typeIntegerString()
	{
		testMt( 1, "one" );
		testMt( 2, "two" );
		testMt( 3, "three" );
	}

	/** */
	@Test
	public void typeString()
	{
		testMt( "one" );
		testMt( "two" );
		testMt( "three" );
	}

	/** @throws Exception */
	@Test
	public void testGetValidator() throws Exception
	{
		Type t = new Type( "t" );
		Field f = new Field( "f" );
		Field g = new Field( "g" );
		Validator v = Validator_int.get( 0 );
		
		t.putValidator( f, v );
		
		assertSame( v, t.getValidator( f ) );
		assertNull( t.getValidator( g ) );
	}

	/** @throws Exception */
	@Test
	public void testPutValidatorNull() throws Exception
	{
		Type t = new Type( "t" );
		Field f = new Field( "f" );
		Validator v = Validator_int.get( 0 );

		assertNull( t.getValidator( f ) );
		
		t.putValidator( f, null );
		assertNull( t.getValidator( f ) );
		
		t.putValidator( f, v );
		assertSame( v, t.getValidator( f ) );
		
		t.putValidator( f, v );
		assertSame( ComboValidator.class, t.getValidator( f ).getClass() );
		
		t.clearValidator( f );
		assertNull( t.getValidator( f ) );
	}

	private void testMt( int id, String name )
	{
		Type mt = new Type( id, name );
		assertEquals( id, mt.getId() );
		assertEquals( name, mt.getName() );
	}

	private void testMt( String name )
	{
		Type mt = new Type( name );
		assertEquals( Hash.hash( name ), mt.getId() );
		assertEquals( name, mt.getName() );
	}
}
