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

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import java.util.Date;

import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.Validator;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.junit.Test;


/**
 * Test Validator_StructValue.
 */
public class TestValidator_StructValue
{
	private final ValueFactory vf = new DummyValueFactory();
	private final Type abc = new Type( "abc" );
	
	/** @throws Exception */
	@Test
	public void constructor1() throws Exception
	{
		testconstructor( 0, "StructValue[abc(669497117), 0]", StructValue.class );
		testconstructor( 1, "StructValue[abc(669497117), 1]", StructValue[].class );
		testconstructor( 2, "StructValue[abc(669497117), 2]", StructValue[][].class );
		testconstructor( 3, "StructValue[abc(669497117), 3]", StructValue[][][].class );
		testconstructor( 4, "StructValue[abc(669497117), 4]", StructValue[][][][].class );
		testconstructor( 5, "StructValue[abc(669497117), 5]", StructValue[][][][][].class );
		testconstructor( 6, "StructValue[abc(669497117), 6]", StructValue[][][][][][].class );
		testconstructor( 7, "StructValue[abc(669497117), 7]", StructValue[][][][][][][].class );
		testconstructor( 8, "StructValue[abc(669497117), 8]", StructValue[][][][][][][][].class );
		testconstructor( 9, "StructValue[abc(669497117), 9]", StructValue[][][][][][][][][].class );
		assertEquals( 9, Validator.MAX_NDIMS );
	}
	
	private void testconstructor( int n, String descr, Class<?> expectedClass )
	{
		Validator_StructValue v = Validator_StructValue.get( abc, n );
		assertEquals( n, v.getNDims() );
		assertSame( expectedClass, v.getExpectedClass() );
		assertEquals( descr, v.toString() );
		assertEquals( abc, v.getType() );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor2() throws Exception
	{
		Validator_StructValue.get( abc, -1 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor3() throws Exception
	{
		Validator_StructValue.get( abc, Validator.MAX_NDIMS+1 );
	}
	
	/** @throws Exception */
	@Test
	public void elementvalidator1() throws Exception
	{
		testelementvalidator( 1, "StructValue[abc(669497117), 0]", StructValue.class );
		testelementvalidator( 2, "StructValue[abc(669497117), 1]", StructValue[].class );
		testelementvalidator( 3, "StructValue[abc(669497117), 2]", StructValue[][].class );
		testelementvalidator( 4, "StructValue[abc(669497117), 3]", StructValue[][][].class );
		testelementvalidator( 5, "StructValue[abc(669497117), 4]", StructValue[][][][].class );
		testelementvalidator( 6, "StructValue[abc(669497117), 5]", StructValue[][][][][].class );
		testelementvalidator( 7, "StructValue[abc(669497117), 6]", StructValue[][][][][][].class );
		testelementvalidator( 8, "StructValue[abc(669497117), 7]", StructValue[][][][][][][].class );
		testelementvalidator( 9, "StructValue[abc(669497117), 8]", StructValue[][][][][][][][].class );
		assertEquals( 9, Validator.MAX_NDIMS );
	}
	
	private void testelementvalidator( int n, String descr, Class<?> expectedClass )
	{
		Validator_StructValue v = (Validator_StructValue) Validator_StructValue.get( abc, n ).elementValidator();
		assertEquals( n-1, v.getNDims() );
		assertSame( expectedClass, v.getExpectedClass() );
		assertEquals( descr, v.toString() );
		assertEquals( abc, v.getType() );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void elementvalidator2() throws Exception
	{
		Validator_StructValue.get( abc, 0 ).elementValidator();
	}
	
	/** @throws Exception */
	@Test
	public void good_scalar() throws Exception
	{
		testgoodvalue( 0, new StructValue( abc, vf ) );
	}
	
	/** @throws Exception */
	@Test
	public void good_array() throws Exception
	{
		testgoodvalue( 1, new StructValue[] {} );
		testgoodvalue( 2, new StructValue[][] {} );
		testgoodvalue( 3, new StructValue[][][] {} );
		testgoodvalue( 4, new StructValue[][][][] {} );
		testgoodvalue( 5, new StructValue[][][][][] {} );
		testgoodvalue( 6, new StructValue[][][][][][] {} );
		testgoodvalue( 7, new StructValue[][][][][][][] {} );
		testgoodvalue( 8, new StructValue[][][][][][][][] {} );
		testgoodvalue( 9, new StructValue[][][][][][][][][] {} );
		assertEquals( 9, Validator.MAX_NDIMS );
	}
	
	private void testgoodvalue( int n, Object value )
	{
		TypeValidator v = Validator_StructValue.get( abc, n );
		assertTrue( v.validate( value ) );
		assertTrue( validateValueOk( v, value ) );
	}
	
	/** @throws Exception */
	@Test
	public void bad_scalar() throws Exception
	{
		testbadvalue( 0, null );
		testbadvalue( 0, false );
		testbadvalue( 0, true );
		testbadvalue( 0, (byte) 1 );
		testbadvalue( 0, (short) 2222 );
		testbadvalue( 0, 33333333 );
		testbadvalue( 0, 4444444444444444L );
		testbadvalue( 0, 5.5f );
		testbadvalue( 0, 6.6 );
		testbadvalue( 0, "" );
		testbadvalue( 0, "abc" );
		testbadvalue( 0, new Object() );
		testbadvalue( 0, new StructValue( new Type( "def" ), vf ) );
		testbadvalue( 0, new Date() );

		testbadvalue( 1, null );
		testbadvalue( 1, false );
		testbadvalue( 1, true );
		testbadvalue( 1, (byte) 1 );
		testbadvalue( 1, (short) 2222 );
		testbadvalue( 1, 333333 );
		testbadvalue( 1, 4444444444444444L );
		testbadvalue( 1, 5.5f );
		testbadvalue( 1, 6.6 );
		testbadvalue( 1, "" );
		testbadvalue( 1, "abc" );
		testbadvalue( 1, new Object() );
		testbadvalue( 1, new StructValue( abc, vf ) );
		testbadvalue( 1, new Date() );
	}
	
	/** @throws Exception */
	@Test
	public void bad_array() throws Exception
	{
		testbadvalue( 0, new StructValue[] {} );
		testbadvalue( 1, new StructValue[][] {} );
		testbadvalue( 2, new StructValue[][][] {} );
		testbadvalue( 3, new StructValue[][][][] {} );
		testbadvalue( 4, new StructValue[][][][][] {} );
		testbadvalue( 5, new StructValue[][][][][][] {} );
		testbadvalue( 6, new StructValue[][][][][][][] {} );
		testbadvalue( 7, new StructValue[][][][][][][][] {} );
		testbadvalue( 8, new StructValue[][][][][][][][][] {} );
		testbadvalue( 9, new StructValue[][][][][][][][][][] {} );
		assertEquals( 9, Validator.MAX_NDIMS );

		testbadvalue( 2, new StructValue[] {} );
		testbadvalue( 3, new StructValue[][] {} );
		testbadvalue( 4, new StructValue[][][] {} );
		testbadvalue( 5, new StructValue[][][][] {} );
		testbadvalue( 6, new StructValue[][][][][] {} );
		testbadvalue( 7, new StructValue[][][][][][] {} );
		testbadvalue( 8, new StructValue[][][][][][][] {} );
		testbadvalue( 9, new StructValue[][][][][][][][] {} );
		assertEquals( 9, Validator.MAX_NDIMS );
	}

	private void testbadvalue( int n, Object value )
	{
		TypeValidator v = Validator_StructValue.get( abc, n );
		assertFalse( v.validate( value ) );
		assertFalse( validateValueOk( v, value ) );
	}

	private boolean validateValueOk( Validator v, Object value )
	{
		try
		{
			Object x = v.validateValue( value );
			assertEquals( value, x );
			return true;
		}
		catch ( Exception e )
		{
			return false;
		}
	}
}
