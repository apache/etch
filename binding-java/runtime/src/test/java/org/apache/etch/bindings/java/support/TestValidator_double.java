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
 * Test Validator_double.
 */
public class TestValidator_double
{
	private final ValueFactory vf = new DummyValueFactory();
	
	/** @throws Exception */
	@Test
	public void constructor1() throws Exception
	{
		testconstructor( 0, "double[0]", Double.class );
		testconstructor( 1, "double[1]", double[].class );
		testconstructor( 2, "double[2]", double[][].class );
		testconstructor( 3, "double[3]", double[][][].class );
		testconstructor( 4, "double[4]", double[][][][].class );
		testconstructor( 5, "double[5]", double[][][][][].class );
		testconstructor( 6, "double[6]", double[][][][][][].class );
		testconstructor( 7, "double[7]", double[][][][][][][].class );
		testconstructor( 8, "double[8]", double[][][][][][][][].class );
		testconstructor( 9, "double[9]", double[][][][][][][][][].class );
		assertEquals( 9, Validator.MAX_NDIMS );
	}
	
	private void testconstructor( int n, String descr, Class<?> expectedClass )
	{
		TypeValidator v = Validator_double.get( n );
		assertEquals( n, v.getNDims() );
		assertSame( expectedClass, v.getExpectedClass() );
		assertEquals( descr, v.toString() );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor2() throws Exception
	{
		Validator_double.get( -1 );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void constructor3() throws Exception
	{
		Validator_double.get( Validator.MAX_NDIMS+1 );
	}
	
	/** @throws Exception */
	@Test
	public void elementvalidator1() throws Exception
	{
		testelementvalidator( 1, "double[0]", Double.class );
		testelementvalidator( 2, "double[1]", double[].class );
		testelementvalidator( 3, "double[2]", double[][].class );
		testelementvalidator( 4, "double[3]", double[][][].class );
		testelementvalidator( 5, "double[4]", double[][][][].class );
		testelementvalidator( 6, "double[5]", double[][][][][].class );
		testelementvalidator( 7, "double[6]", double[][][][][][].class );
		testelementvalidator( 8, "double[7]", double[][][][][][][].class );
		testelementvalidator( 9, "double[8]", double[][][][][][][][].class );
		assertEquals( 9, Validator.MAX_NDIMS );
	}
	
	private void testelementvalidator( int n, String descr, Class<?> expectedClass )
	{
		TypeValidator v = (TypeValidator) Validator_double.get( n ).elementValidator();
		assertEquals( n-1, v.getNDims() );
		assertSame( expectedClass, v.getExpectedClass() );
		assertEquals( descr, v.toString() );
	}

	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void elementvalidator2() throws Exception
	{
		Validator_double.get( 0 ).elementValidator();
	}
	
	/** @throws Exception */
	@Test
	public void good_scalar() throws Exception
	{
		testgoodvalue( 0, 0.0 );
		testgoodvalue( 0, 1.1 );
		testgoodvalue( 0, -1.1 );
	}
	
	/** @throws Exception */
	@Test
	public void good_array() throws Exception
	{
		testgoodvalue( 1, new double[] {} );
		testgoodvalue( 2, new double[][] {} );
		testgoodvalue( 3, new double[][][] {} );
		testgoodvalue( 4, new double[][][][] {} );
		testgoodvalue( 5, new double[][][][][] {} );
		testgoodvalue( 6, new double[][][][][][] {} );
		testgoodvalue( 7, new double[][][][][][][] {} );
		testgoodvalue( 8, new double[][][][][][][][] {} );
		testgoodvalue( 9, new double[][][][][][][][][] {} );
		assertEquals( 9, Validator.MAX_NDIMS );
	}
	
	private void testgoodvalue( int n, Object value )
	{
		TypeValidator v = Validator_double.get( n );
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
//		testbadvalue( 0, 6.6 ); good!
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
		testbadvalue( 0, new double[] {} );
		testbadvalue( 1, new double[][] {} );
		testbadvalue( 2, new double[][][] {} );
		testbadvalue( 3, new double[][][][] {} );
		testbadvalue( 4, new double[][][][][] {} );
		testbadvalue( 5, new double[][][][][][] {} );
		testbadvalue( 6, new double[][][][][][][] {} );
		testbadvalue( 7, new double[][][][][][][][] {} );
		testbadvalue( 8, new double[][][][][][][][][] {} );
		testbadvalue( 9, new double[][][][][][][][][][] {} );
		assertEquals( 9, Validator.MAX_NDIMS );

		testbadvalue( 2, new double[] {} );
		testbadvalue( 3, new double[][] {} );
		testbadvalue( 4, new double[][][] {} );
		testbadvalue( 5, new double[][][][] {} );
		testbadvalue( 6, new double[][][][][] {} );
		testbadvalue( 7, new double[][][][][][] {} );
		testbadvalue( 8, new double[][][][][][][] {} );
		testbadvalue( 9, new double[][][][][][][][] {} );
		assertEquals( 9, Validator.MAX_NDIMS );
	}

	private void testbadvalue( int n, Object value )
	{
		TypeValidator v = Validator_double.get( n );
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
