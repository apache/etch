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
 * Test Validator_long.
 */
public class TestValidator_long
{
	private final ValueFactory vf = new DummyValueFactory();
	
	/** @throws Exception */
	@Test
	public void constructor1() throws Exception
	{
		testconstructor( 0, "long[0]", Long.class );
		testconstructor( 1, "long[1]", long[].class );
		testconstructor( 2, "long[2]", long[][].class );
		testconstructor( 3, "long[3]", long[][][].class );
		testconstructor( 4, "long[4]", long[][][][].class );
		testconstructor( 5, "long[5]", long[][][][][].class );
		testconstructor( 6, "long[6]", long[][][][][][].class );
		testconstructor( 7, "long[7]", long[][][][][][][].class );
		testconstructor( 8, "long[8]", long[][][][][][][][].class );
		testconstructor( 9, "long[9]", long[][][][][][][][][].class );
		assertEquals( 9, Validator.MAX_NDIMS );
	}
	
	private void testconstructor( int n, String descr, Class<?> expectedClass )
	{
		TypeValidator v = Validator_long.get( n );
		assertEquals( n, v.getNDims() );
		assertSame( expectedClass, v.getExpectedClass() );
		assertEquals( descr, v.toString() );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor2() throws Exception
	{
		Validator_long.get( -1 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor3() throws Exception
	{
		Validator_long.get( Validator.MAX_NDIMS+1 );
	}
	
	/** @throws Exception */
	@Test
	public void elementvalidator1() throws Exception
	{
		testelementvalidator( 1, "long[0]", Long.class );
		testelementvalidator( 2, "long[1]", long[].class );
		testelementvalidator( 3, "long[2]", long[][].class );
		testelementvalidator( 4, "long[3]", long[][][].class );
		testelementvalidator( 5, "long[4]", long[][][][].class );
		testelementvalidator( 6, "long[5]", long[][][][][].class );
		testelementvalidator( 7, "long[6]", long[][][][][][].class );
		testelementvalidator( 8, "long[7]", long[][][][][][][].class );
		testelementvalidator( 9, "long[8]", long[][][][][][][][].class );
		assertEquals( 9, Validator.MAX_NDIMS );
	}
	
	private void testelementvalidator( int n, String descr, Class<?> expectedClass )
	{
		TypeValidator v = (TypeValidator) Validator_long.get( n ).elementValidator();
		assertEquals( n-1, v.getNDims() );
		assertSame( expectedClass, v.getExpectedClass() );
		assertEquals( descr, v.toString() );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void elementvalidator2() throws Exception
	{
		Validator_long.get( 0 ).elementValidator();
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
		testgoodvalue( 0, (short) 2222 );
		testgoodvalue( 0, Short.MAX_VALUE );
		
		testgoodvalue( 0, (short) -1 );
		testgoodvalue( 0, (short) -64 );
		testgoodvalue( 0, (short) Byte.MIN_VALUE );
		testgoodvalue( 0, (short) -2222 );
		testgoodvalue( 0, Short.MIN_VALUE );

		// INTS
		
		testgoodvalue( 0, 0 );
		
		testgoodvalue( 0, 1 );
		testgoodvalue( 0, 64 );
		testgoodvalue( 0, (int) Byte.MAX_VALUE );
		testgoodvalue( 0, 2222 );
		testgoodvalue( 0, (int) Short.MAX_VALUE );
		testgoodvalue( 0, 33333333 );
		testgoodvalue( 0, Integer.MAX_VALUE );
		
		testgoodvalue( 0, -1 );
		testgoodvalue( 0, -64 );
		testgoodvalue( 0, (int) Byte.MIN_VALUE );
		testgoodvalue( 0, -2222 );
		testgoodvalue( 0, (int) Short.MIN_VALUE );
		testgoodvalue( 0, -33333333 );
		testgoodvalue( 0, Integer.MIN_VALUE );

		// LONGS
		
		testgoodvalue( 0, (long) 0 );
		
		testgoodvalue( 0, (long) 1 );
		testgoodvalue( 0, (long) 64 );
		testgoodvalue( 0, (long) Byte.MAX_VALUE );
		testgoodvalue( 0, (long) 2222 );
		testgoodvalue( 0, (long) Short.MAX_VALUE );
		testgoodvalue( 0, (long) 33333333 );
		testgoodvalue( 0, (long) Integer.MAX_VALUE );
		testgoodvalue( 0, 4444444444444444L );
		testgoodvalue( 0, Long.MAX_VALUE );
		
		testgoodvalue( 0, (long) -1 );
		testgoodvalue( 0, (long) -64 );
		testgoodvalue( 0, (long) Byte.MIN_VALUE );
		testgoodvalue( 0, (long) -2222 );
		testgoodvalue( 0, (long) Short.MIN_VALUE );
		testgoodvalue( 0, (long) -33333333 );
		testgoodvalue( 0, (long) Integer.MIN_VALUE );
		testgoodvalue( 0, -4444444444444444L );
		testgoodvalue( 0, Long.MIN_VALUE );
	}
	
	/** @throws Exception */
	@Test
	public void good_array() throws Exception
	{
		testgoodvalue( 1, new long[] {} );
		testgoodvalue( 2, new long[][] {} );
		testgoodvalue( 3, new long[][][] {} );
		testgoodvalue( 4, new long[][][][] {} );
		testgoodvalue( 5, new long[][][][][] {} );
		testgoodvalue( 6, new long[][][][][][] {} );
		testgoodvalue( 7, new long[][][][][][][] {} );
		testgoodvalue( 8, new long[][][][][][][][] {} );
		testgoodvalue( 9, new long[][][][][][][][][] {} );
		assertEquals( 9, Validator.MAX_NDIMS );
	}
	
	private void testgoodvalue( int n, Object value )
	{
		TypeValidator v = Validator_long.get( n );
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
//		testbadvalue( 0, (short) 2222 ); good!
//		testbadvalue( 0, 33333333 ); good!
//		testbadvalue( 0, 4444444444444444L ); good!
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
		testbadvalue( 0, new long[] {} );
		testbadvalue( 1, new long[][] {} );
		testbadvalue( 2, new long[][][] {} );
		testbadvalue( 3, new long[][][][] {} );
		testbadvalue( 4, new long[][][][][] {} );
		testbadvalue( 5, new long[][][][][][] {} );
		testbadvalue( 6, new long[][][][][][][] {} );
		testbadvalue( 7, new long[][][][][][][][] {} );
		testbadvalue( 8, new long[][][][][][][][][] {} );
		testbadvalue( 9, new long[][][][][][][][][][] {} );
		assertEquals( 9, Validator.MAX_NDIMS );

		testbadvalue( 2, new long[] {} );
		testbadvalue( 3, new long[][] {} );
		testbadvalue( 4, new long[][][] {} );
		testbadvalue( 5, new long[][][][] {} );
		testbadvalue( 6, new long[][][][][] {} );
		testbadvalue( 7, new long[][][][][][] {} );
		testbadvalue( 8, new long[][][][][][][] {} );
		testbadvalue( 9, new long[][][][][][][][] {} );
		assertEquals( 9, Validator.MAX_NDIMS );
	}

	private void testbadvalue( int n, Object value )
	{
		TypeValidator v = Validator_long.get( n );
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
