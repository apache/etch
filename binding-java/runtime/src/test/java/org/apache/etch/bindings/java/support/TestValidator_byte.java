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
 * Test Validator_byte.
 */
public class TestValidator_byte
{
	private final ValueFactory vf = new DummyValueFactory();
	
	/** @throws Exception */
	@Test
	public void constructor1() throws Exception
	{
		testconstructor( 0, "byte[0]", Byte.class );
		testconstructor( 1, "byte[1]", byte[].class );
		testconstructor( 2, "byte[2]", byte[][].class );
		testconstructor( 3, "byte[3]", byte[][][].class );
		testconstructor( 4, "byte[4]", byte[][][][].class );
		testconstructor( 5, "byte[5]", byte[][][][][].class );
		testconstructor( 6, "byte[6]", byte[][][][][][].class );
		testconstructor( 7, "byte[7]", byte[][][][][][][].class );
		testconstructor( 8, "byte[8]", byte[][][][][][][][].class );
		testconstructor( 9, "byte[9]", byte[][][][][][][][][].class );
		assertEquals( 9, Validator.MAX_NDIMS );
	}
	
	private void testconstructor( int n, String descr, Class<?> expectedClass )
	{
		TypeValidator v = Validator_byte.get( n );
		assertEquals( n, v.getNDims() );
		assertSame( expectedClass, v.getExpectedClass() );
		assertEquals( descr, v.toString() );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor2() throws Exception
	{
		Validator_byte.get( -1 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor3() throws Exception
	{
		Validator_byte.get( Validator.MAX_NDIMS+1 );
	}
	
	/** @throws Exception */
	@Test
	public void elementvalidator1() throws Exception
	{
		testelementvalidator( 1, "byte[0]", Byte.class );
		testelementvalidator( 2, "byte[1]", byte[].class );
		testelementvalidator( 3, "byte[2]", byte[][].class );
		testelementvalidator( 4, "byte[3]", byte[][][].class );
		testelementvalidator( 5, "byte[4]", byte[][][][].class );
		testelementvalidator( 6, "byte[5]", byte[][][][][].class );
		testelementvalidator( 7, "byte[6]", byte[][][][][][].class );
		testelementvalidator( 8, "byte[7]", byte[][][][][][][].class );
		testelementvalidator( 9, "byte[8]", byte[][][][][][][][].class );
		assertEquals( 9, Validator.MAX_NDIMS );
	}
	
	private void testelementvalidator( int n, String descr, Class<?> expectedClass )
	{
		TypeValidator v = (TypeValidator) Validator_byte.get( n ).elementValidator();
		assertEquals( n-1, v.getNDims() );
		assertSame( expectedClass, v.getExpectedClass() );
		assertEquals( descr, v.toString() );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void elementvalidator2() throws Exception
	{
		Validator_byte.get( 0 ).elementValidator();
	}
	
	/** @throws Exception */
	@Test
	public void good_scalar() throws Exception
	{
		// BYTES
		
		testgoodvalue( 0, (byte) 0 );
		
		testgoodvalue( 0, (byte) 1 );
		testgoodvalue( 0, (byte) 64 );
		testgoodvalue( 0, Byte.MAX_VALUE );
		
		testgoodvalue( 0, (byte) -1 );
		testgoodvalue( 0, (byte) -64 );
		testgoodvalue( 0, Byte.MIN_VALUE );

		// SHORTS
		
		testgoodvalue( 0, (short) 0 );
		
		testgoodvalue( 0, (short) 1 );
		testgoodvalue( 0, (short) 64 );
		testgoodvalue( 0, (short) Byte.MAX_VALUE );
		
		testgoodvalue( 0, (short) -1 );
		testgoodvalue( 0, (short) -64 );
		testgoodvalue( 0, (short) Byte.MIN_VALUE );

		// INTS
		
		testgoodvalue( 0, 0 );
		
		testgoodvalue( 0, 1 );
		testgoodvalue( 0, 64 );
		testgoodvalue( 0, (int) Byte.MAX_VALUE );
		
		testgoodvalue( 0, -1 );
		testgoodvalue( 0, -64 );
		testgoodvalue( 0, (int) Byte.MIN_VALUE );

		// LONGS
		
		testgoodvalue( 0, (long) 0 );
		
		testgoodvalue( 0, (long) 1 );
		testgoodvalue( 0, (long) 64 );
		testgoodvalue( 0, (long) Byte.MAX_VALUE );
		
		testgoodvalue( 0, (long) -1 );
		testgoodvalue( 0, (long) -64 );
		testgoodvalue( 0, (long) Byte.MIN_VALUE );
	}
	
	/** @throws Exception */
	@Test
	public void good_array() throws Exception
	{
		testgoodvalue( 1, new byte[] {} );
		testgoodvalue( 2, new byte[][] {} );
		testgoodvalue( 3, new byte[][][] {} );
		testgoodvalue( 4, new byte[][][][] {} );
		testgoodvalue( 5, new byte[][][][][] {} );
		testgoodvalue( 6, new byte[][][][][][] {} );
		testgoodvalue( 7, new byte[][][][][][][] {} );
		testgoodvalue( 8, new byte[][][][][][][][] {} );
		testgoodvalue( 9, new byte[][][][][][][][][] {} );
		assertEquals( 9, Validator.MAX_NDIMS );
	}
	
	private void testgoodvalue( int n, Object value )
	{
		TypeValidator v = Validator_byte.get( n );
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
//		testbadvalue( 0, (byte) 1 ); good!
		testbadvalue( 0, (short) 2222 );
		testbadvalue( 0, 33333333 );
		testbadvalue( 0, 4444444444444444L );
		testbadvalue( 0, 5.5f );
		testbadvalue( 0, 6.6 );
		testbadvalue( 0, "" );
		testbadvalue( 0, "abc" );
		testbadvalue( 0, new Object() );
		testbadvalue( 0, new StructValue( new Type( "abc" ), vf ) );
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
		testbadvalue( 1, new StructValue( new Type( "abc" ), vf ) );
		testbadvalue( 1, new Date() );
	}
	
	/** @throws Exception */
	@Test
	public void bad_array() throws Exception
	{
		testbadvalue( 0, new byte[] {} );
		testbadvalue( 1, new byte[][] {} );
		testbadvalue( 2, new byte[][][] {} );
		testbadvalue( 3, new byte[][][][] {} );
		testbadvalue( 4, new byte[][][][][] {} );
		testbadvalue( 5, new byte[][][][][][] {} );
		testbadvalue( 6, new byte[][][][][][][] {} );
		testbadvalue( 7, new byte[][][][][][][][] {} );
		testbadvalue( 8, new byte[][][][][][][][][] {} );
		testbadvalue( 9, new byte[][][][][][][][][][] {} );
		assertEquals( 9, Validator.MAX_NDIMS );

		testbadvalue( 2, new byte[] {} );
		testbadvalue( 3, new byte[][] {} );
		testbadvalue( 4, new byte[][][] {} );
		testbadvalue( 5, new byte[][][][] {} );
		testbadvalue( 6, new byte[][][][][] {} );
		testbadvalue( 7, new byte[][][][][][] {} );
		testbadvalue( 8, new byte[][][][][][][] {} );
		testbadvalue( 9, new byte[][][][][][][][] {} );
		assertEquals( 9, Validator.MAX_NDIMS );
	}

	private void testbadvalue( int n, Object value )
	{
		TypeValidator v = Validator_byte.get( n );
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
