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
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import java.util.Date;

import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.Validator;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.junit.Test;


/**
 * Test Validator_object.
 */
public class TestValidator_object
{
	private final ValueFactory vf = new DummyValueFactory();
	
	/** @throws Exception */
	@Test
	public void constructor1() throws Exception
	{
		testconstructor( 0, "object[0]", Object.class );
		testconstructor( 1, "object[1]", Object[].class );
		testconstructor( 2, "object[2]", Object[][].class );
		testconstructor( 3, "object[3]", Object[][][].class );
		testconstructor( 4, "object[4]", Object[][][][].class );
		testconstructor( 5, "object[5]", Object[][][][][].class );
		testconstructor( 6, "object[6]", Object[][][][][][].class );
		testconstructor( 7, "object[7]", Object[][][][][][][].class );
		testconstructor( 8, "object[8]", Object[][][][][][][][].class );
		testconstructor( 9, "object[9]", Object[][][][][][][][][].class );
		assertEquals( 9, Validator.MAX_NDIMS );
	}
	
	private void testconstructor( int n, String descr, Class<?> expectedClass )
	{
		TypeValidator v = Validator_object.get( n );
		assertEquals( n, v.getNDims() );
		assertSame( expectedClass, v.getExpectedClass() );
		assertEquals( descr, v.toString() );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor2() throws Exception
	{
		Validator_object.get( -1 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor3() throws Exception
	{
		Validator_object.get( Validator.MAX_NDIMS+1 );
	}
	
	/** @throws Exception */
	@Test
	public void elementvalidator1() throws Exception
	{
		testelementvalidator( 0, 0, "object[0]", Object.class );
		testelementvalidator( 1, 0, "object[0]", Object.class );
		testelementvalidator( 2, 1, "object[1]", Object[].class );
		testelementvalidator( 3, 2, "object[2]", Object[][].class );
		testelementvalidator( 4, 3, "object[3]", Object[][][].class );
		testelementvalidator( 5, 4, "object[4]", Object[][][][].class );
		testelementvalidator( 6, 5, "object[5]", Object[][][][][].class );
		testelementvalidator( 7, 6, "object[6]", Object[][][][][][].class );
		testelementvalidator( 8, 7, "object[7]", Object[][][][][][][].class );
		testelementvalidator( 9, 8, "object[8]", Object[][][][][][][][].class );
		assertEquals( 9, Validator.MAX_NDIMS );
	}
	
	private void testelementvalidator( int n, int k, String descr, Class<?> expectedClass )
	{
		TypeValidator v = (TypeValidator) Validator_object.get( n ).elementValidator();
		assertEquals( k, v.getNDims() );
		assertSame( expectedClass, v.getExpectedClass() );
		assertEquals( descr, v.toString() );
	}
	
	/** @throws Exception */
	@Test
	public void good_scalar() throws Exception
	{
		testgoodvalue( 0, false );
		testgoodvalue( 0, true );
		testgoodvalue( 0, (byte) 1 );
		testgoodvalue( 0, (short) 2222 );
		testgoodvalue( 0, 33333333 );
		testgoodvalue( 0, 4444444444444444L );
		testgoodvalue( 0, 5.5f );
		testgoodvalue( 0, 6.6 );
		testgoodvalue( 0, "" );
		testgoodvalue( 0, "abc" );
		testgoodvalue( 0, new Object() );
		testgoodvalue( 0, new StructValue( new Type( "abc" ), vf ) );
		testgoodvalue( 0, new Date() );
	}
	
	/** @throws Exception */
	@Test
	public void good_array() throws Exception
	{
		// primitive array types are compatible with object.
		
		testgoodvalue( 0, new boolean[] {} );
		testgoodvalue( 0, new byte[] {} );
		testgoodvalue( 0, new short[] {} );
		testgoodvalue( 0, new int[] {} );
		testgoodvalue( 0, new long[] {} );
		testgoodvalue( 0, new float[] {} );
		testgoodvalue( 0, new double[] {} );
		
		// ref array types are compatible with object.

		testgoodvalue( 0, new Boolean[] {} );
		testgoodvalue( 0, new Byte[] {} );
		testgoodvalue( 0, new Short[] {} );
		testgoodvalue( 0, new Integer[] {} );
		testgoodvalue( 0, new Long[] {} );
		testgoodvalue( 0, new Float[] {} );
		testgoodvalue( 0, new Double[] {} );
		testgoodvalue( 0, new String[] {} );
		testgoodvalue( 0, new Object[] {} );
		testgoodvalue( 0, new StructValue[] {} );
		testgoodvalue( 0, new Date[] {} );
		
		// ref array types are compatible with object array (same size).
		
		testgoodvalue( 1, new Boolean[] {} );
		testgoodvalue( 1, new Byte[] {} );
		testgoodvalue( 1, new Short[] {} );
		testgoodvalue( 1, new Integer[] {} );
		testgoodvalue( 1, new Long[] {} );
		testgoodvalue( 1, new Float[] {} );
		testgoodvalue( 1, new Double[] {} );
		testgoodvalue( 1, new String[] {} );
		testgoodvalue( 1, new Object[] {} );
		testgoodvalue( 1, new StructValue[] {} );
		testgoodvalue( 1, new Date[] {} );
		
		// ref array types are compatible with object array (larger size).
		
		testgoodvalue( 1, new Boolean[][] {} );
		testgoodvalue( 1, new Byte[][] {} );
		testgoodvalue( 1, new Short[][] {} );
		testgoodvalue( 1, new Integer[][] {} );
		testgoodvalue( 1, new Long[][] {} );
		testgoodvalue( 1, new Float[][] {} );
		testgoodvalue( 1, new Double[][] {} );
		testgoodvalue( 1, new String[][] {} );
		testgoodvalue( 1, new Object[][] {} );
		testgoodvalue( 1, new StructValue[][] {} );
		testgoodvalue( 1, new Date[][] {} );
	}
	
	private void testgoodvalue( int n, Object value )
	{
		TypeValidator v = Validator_object.get( n );
		assertTrue( v.validate( value ) );
		assertTrue( validateValueOk( v, value ) );
	}
	
	/** @throws Exception */
	@Test
	public void assignable1() throws Exception
	{
		Object x;
		
		x = new Object();
		assertTrue( x( Object.class, Object.class ) );

		x = new Object[] {};
		assertTrue( x( Object.class, Object[].class ) );
		
		x = new Object[][] {};
		assertTrue( x( Object.class, Object[][].class ) );

		x = new Integer( 0 );
		assertTrue( x( Object.class, Integer.class ) );
		
		x = new Integer[] {};
		assertTrue( x( Object.class, Integer[].class ) );

		x = new int[] {};
		assertTrue( x( Object.class, int[].class ) );
		
		assertNotNull( x );
	}
	
	/** @throws Exception */
	@Test
	public void assignable2() throws Exception
	{
		Object[] y;
		
//		y = new Object();
		assertFalse( x( Object[].class, Object.class ) );

		y = new Object[] {};
		assertTrue( x( Object[].class, Object[].class ) );
		
		y = new Object[][] {};
		assertTrue( x( Object[].class, Object[][].class ) );

//		y = new Integer( 0 );
		assertFalse( x( Object[].class, Integer.class ) );
		
		y = new Integer[] { 1, 2, 3 };
		y[0] = 1;
		assertTrue( x( Object[].class, Integer[].class ) );

//		y = new int[] {};
		assertFalse( x( Object[].class, int[].class ) );
	}
	
	/** @throws Exception */
	@Test( expected = ArrayStoreException.class )
	public void assignable3() throws Exception
	{
		Object[] x = new Integer[] { 1, 2, 3 };
		x[0] = true; // wrong type!
	}
	
	private boolean x( Class<?> a, Class<?> b )
	{
		return a.isAssignableFrom( b );
	}

	/** @throws Exception */
	@Test
	public void bad_scalar() throws Exception
	{
		testbadvalue( 0, null );
//		testbadvalue( 0, false );
//		testbadvalue( 0, true );
//		testbadvalue( 0, (byte) 1 );
//		testbadvalue( 0, (short) 2222 );
//		testbadvalue( 0, 33333333 );
//		testbadvalue( 0, 4444444444444444L );
//		testbadvalue( 0, 5.5f );
//		testbadvalue( 0, 6.6 );
//		testbadvalue( 0, "" );
//		testbadvalue( 0, "abc" );
//		testbadvalue( 0, new Object() );
//		testbadvalue( 0, new StructValue( new Type( "abc" ) ) );
//		testbadvalue( 0, new Date() );

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
		// primitive arrays cannot map into object arrays...
		
		testbadvalue( 1, new boolean[] {} );
		testbadvalue( 1, new byte[] {} );
		testbadvalue( 1, new short[] {} );
		testbadvalue( 1, new int[] {} );
		testbadvalue( 1, new long[] {} );
		testbadvalue( 1, new float[] {} );
		testbadvalue( 1, new double[] {} );

		// too short...
		
		testbadvalue( 2, new Object[] {} );
		testbadvalue( 3, new Object[][] {} );
		testbadvalue( 4, new Object[][][] {} );
		testbadvalue( 5, new Object[][][][] {} );
		testbadvalue( 6, new Object[][][][][] {} );
		testbadvalue( 7, new Object[][][][][][] {} );
		testbadvalue( 8, new Object[][][][][][][] {} );
		testbadvalue( 9, new Object[][][][][][][][] {} );
		assertEquals( 9, Validator.MAX_NDIMS );
	}

	private void testbadvalue( int n, Object value )
	{
		TypeValidator v = Validator_object.get( n );
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
