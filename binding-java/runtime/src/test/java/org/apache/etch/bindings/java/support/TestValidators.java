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

import java.lang.reflect.Array;
import java.util.Date;

import org.apache.etch.bindings.java.msg.Validator;
import org.junit.Assert;
import org.junit.Test;


/** Tests various validators */
public class TestValidators
{
	/** @throws Exception */
	@Test
	public void test_boolean() throws Exception
	{
		test( Validator_boolean.get( 2 ), 2,
			new boolean[][] { { false, false }, { false, false } } );
		
		test( Validator_boolean.get( 2 ), 2,
			new boolean[][] { { true, true }, { true, true } } );
	}

	/** @throws Exception */
	@Test
	public void test_byte() throws Exception
	{
		test( Validator_byte.get( 2 ), 2,
			new byte[][] { { 0, 0 }, { 0, 0 } } );

		test( Validator_byte.get( 2 ), 2,
			new byte[][] { { 1, 1 }, { 1, 1 } } );

		test( Validator_byte.get( 2 ), 2,
			new byte[][] { { -1, -1 }, { -1, -1 } } );
		
		test( Validator_byte.get( 2 ), 2,
			new byte[][] { { Byte.MAX_VALUE, Byte.MAX_VALUE }, { Byte.MAX_VALUE, Byte.MAX_VALUE } } );
		
		test( Validator_byte.get( 2 ), 2,
			new byte[][] { { Byte.MIN_VALUE, Byte.MIN_VALUE }, { Byte.MIN_VALUE, Byte.MIN_VALUE } } );
	}

	/** @throws Exception */
	@Test
	public void test_short() throws Exception
	{
		test( Validator_short.get( 2 ), 2,
			new short[][] { { 0, 0 }, { 0, 0 } } );
		
		test( Validator_short.get( 2 ), 2,
			new short[][] { { 1, 1 }, { 1, 1 } } );
		
		test( Validator_short.get( 2 ), 2,
			new short[][] { { -1, -1 }, { -1, -1 } } );
		
		test( Validator_short.get( 2 ), 2,
			new short[][] { { Byte.MAX_VALUE, Byte.MAX_VALUE }, { Byte.MAX_VALUE, Byte.MAX_VALUE } } );
		
		test( Validator_short.get( 2 ), 2,
			new short[][] { { Byte.MIN_VALUE, Byte.MIN_VALUE }, { Byte.MIN_VALUE, Byte.MIN_VALUE } } );
		
		test( Validator_short.get( 2 ), 2,
			new short[][] { { Short.MAX_VALUE, Short.MAX_VALUE }, { Short.MAX_VALUE, Short.MAX_VALUE } } );
		
		test( Validator_short.get( 2 ), 2,
			new short[][] { { Short.MIN_VALUE, Short.MIN_VALUE }, { Short.MIN_VALUE, Short.MIN_VALUE } } );
	}

	/** @throws Exception */
	@Test
	public void test_int() throws Exception
	{
		test( Validator_int.get( 2 ), 2,
			new int[][] { { 0, 0 }, { 0, 0 } } );
		
		test( Validator_int.get( 2 ), 2,
			new int[][] { { 1, 1 }, { 1, 1 } } );
		
		test( Validator_int.get( 2 ), 2,
			new int[][] { { -1, -1 }, { -1, -1 } } );
		
		test( Validator_int.get( 2 ), 2,
			new int[][] { { Byte.MAX_VALUE, Byte.MAX_VALUE }, { Byte.MAX_VALUE, Byte.MAX_VALUE } } );
		
		test( Validator_int.get( 2 ), 2,
			new int[][] { { Byte.MIN_VALUE, Byte.MIN_VALUE }, { Byte.MIN_VALUE, Byte.MIN_VALUE } } );
		
		test( Validator_int.get( 2 ), 2,
			new int[][] { { Short.MAX_VALUE, Short.MAX_VALUE }, { Short.MAX_VALUE, Short.MAX_VALUE } } );
		
		test( Validator_int.get( 2 ), 2,
			new int[][] { { Short.MIN_VALUE, Short.MIN_VALUE }, { Short.MIN_VALUE, Short.MIN_VALUE } } );
		
		test( Validator_int.get( 2 ), 2,
			new int[][] { { Integer.MAX_VALUE, Integer.MAX_VALUE }, { Integer.MAX_VALUE, Integer.MAX_VALUE } } );
		
		test( Validator_int.get( 2 ), 2,
			new int[][] { { Integer.MIN_VALUE, Integer.MIN_VALUE }, { Integer.MIN_VALUE, Integer.MIN_VALUE } } );
	}

	/** @throws Exception */
	@Test
	public void test_long() throws Exception
	{
		test( Validator_long.get( 2 ), 2,
			new long[][] { { 0, 0 }, { 0, 0 } } );
		
		test( Validator_long.get( 2 ), 2,
			new long[][] { { 1, 1 }, { 1, 1 } } );
		
		test( Validator_long.get( 2 ), 2,
			new long[][] { { -1, -1 }, { -1, -1 } } );
		
		test( Validator_long.get( 2 ), 2,
			new long[][] { { Byte.MAX_VALUE, Byte.MAX_VALUE }, { Byte.MAX_VALUE, Byte.MAX_VALUE } } );
		
		test( Validator_long.get( 2 ), 2,
			new long[][] { { Byte.MIN_VALUE, Byte.MIN_VALUE }, { Byte.MIN_VALUE, Byte.MIN_VALUE } } );
		
		test( Validator_long.get( 2 ), 2,
			new long[][] { { Short.MAX_VALUE, Short.MAX_VALUE }, { Short.MAX_VALUE, Short.MAX_VALUE } } );
		
		test( Validator_long.get( 2 ), 2,
			new long[][] { { Short.MIN_VALUE, Short.MIN_VALUE }, { Short.MIN_VALUE, Short.MIN_VALUE } } );
		
		test( Validator_long.get( 2 ), 2,
			new long[][] { { Integer.MAX_VALUE, Integer.MAX_VALUE }, { Integer.MAX_VALUE, Integer.MAX_VALUE } } );
		
		test( Validator_long.get( 2 ), 2,
			new long[][] { { Integer.MIN_VALUE, Integer.MIN_VALUE }, { Integer.MIN_VALUE, Integer.MIN_VALUE } } );
		
		test( Validator_long.get( 2 ), 2,
			new long[][] { { Long.MAX_VALUE, Long.MAX_VALUE }, { Long.MAX_VALUE, Long.MAX_VALUE } } );
		
		test( Validator_long.get( 2 ), 2,
			new long[][] { { Long.MIN_VALUE, Long.MIN_VALUE }, { Long.MIN_VALUE, Long.MIN_VALUE } } );
	}

	/** @throws Exception */
	@Test
	public void test_float() throws Exception
	{
		test( Validator_float.get( 2 ), 2,
			new float[][] { { 0, 0 }, { 0, 0 } } );
		
		test( Validator_float.get( 2 ), 2,
			new float[][] { { 1, 1 }, { 1, 1 } } );
		
		test( Validator_float.get( 2 ), 2,
			new float[][] { { -1, -1 }, { -1, -1 } } );
		
		test( Validator_float.get( 2 ), 2,
			new float[][] { { Float.MAX_VALUE, Float.MAX_VALUE }, { Float.MAX_VALUE, Float.MAX_VALUE } } );
		
		test( Validator_float.get( 2 ), 2,
			new float[][] { { Float.MIN_VALUE, Float.MIN_VALUE }, { Float.MIN_VALUE, Float.MIN_VALUE } } );
	}

	/** @throws Exception */
	@Test
	public void test_double() throws Exception
	{
		test( Validator_double.get( 2 ), 2,
			new double[][] { { 0, 0 }, { 0, 0 } } );
		
		test( Validator_double.get( 2 ), 2,
			new double[][] { { 1, 1 }, { 1, 1 } } );
		
		test( Validator_double.get( 2 ), 2,
			new double[][] { { -1, -1 }, { -1, -1 } } );
		
		test( Validator_double.get( 2 ), 2,
			new double[][] { { Double.MAX_VALUE, Double.MAX_VALUE }, { Double.MAX_VALUE, Double.MAX_VALUE } } );
		
		test( Validator_double.get( 2 ), 2,
			new double[][] { { Double.MIN_VALUE, Double.MIN_VALUE }, { Double.MIN_VALUE, Double.MIN_VALUE } } );
	}

	/** @throws Exception */
	@Test
	public void test_string() throws Exception
	{
		test( Validator_string.get( 2 ), 2,
			new String[][] { { "", "" }, { "", "" } } );
		
		test( Validator_string.get( 2 ), 2,
			new String[][] { { "abc", "abc" }, { "abc", "abc" } } );
	}

	/** @throws Exception */
	@Test
	public void test_custom() throws Exception
	{
		test( Validator_custom.get( Date.class, 2, false ), 2,
			new Date[][] { { new Date(), new Date() }, { new Date(), new Date() } } );
	}
	
	private void test( Validator v, int n, Object o )
	{
//		System.out.printf( "testing with %s\n", v );
//		Assert.assertEquals( n, v.getDim() );
		
		Assert.assertTrue( v.validate( o ) );
		
		if (n > 0)
		{
			
			
			int k = Array.getLength( o );
			for (int i = 0; i < k; i++)
				test( v.elementValidator(), n-1, Array.get( o, i ) );
		}
	}
}
