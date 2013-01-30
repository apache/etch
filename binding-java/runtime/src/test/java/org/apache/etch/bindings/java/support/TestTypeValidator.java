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
import static org.junit.Assert.assertSame;

import org.apache.etch.bindings.java.msg.Validator;
import org.junit.Test;


/** */
public class TestTypeValidator
{
	/** */
	@Test( expected = IllegalArgumentException.class )
	public void testValidate()
	{
		new MyTypeValidator( int.class, int.class, -1, "bzzt" );
	}

	/** */
	@Test
	public void testTypeValidator1()
	{
		MyTypeValidator v = new MyTypeValidator( int.class, null, 0, "abc" );
		assertSame( int.class, v.getExpectedClass() );
		assertEquals( 0, v.getNDims() );
		assertEquals( "abc", v.toString() );
	}

	/** */
	@Test
	public void testTypeValidator2()
	{
		MyTypeValidator v = new MyTypeValidator( boolean.class, null, 0, "def" );
		assertSame( boolean.class, v.getExpectedClass() );
		assertEquals( 0, v.getNDims() );
		assertEquals( "def", v.toString() );
	}

	/** */
	@Test
	public void testTypeValidator3()
	{
		MyTypeValidator v = new MyTypeValidator( null, int.class, 1, "ghi" );
		assertSame( int[].class, v.getExpectedClass() );
		assertEquals( 1, v.getNDims() );
		assertEquals( "ghi", v.toString() );
	}

	/** */
	@Test
	public void testTypeValidator4()
	{
		MyTypeValidator v = new MyTypeValidator( null, boolean.class, 1, "jkl" );
		assertSame( boolean[].class, v.getExpectedClass() );
		assertEquals( 1, v.getNDims() );
		assertEquals( "jkl", v.toString() );
	}

	/** */
	@Test
	public void testToString()
	{
		MyTypeValidator v = new MyTypeValidator( int.class, int.class, 0, "abc" );
		assertEquals( "abc", v.toString() );
		
		v = new MyTypeValidator( int.class, int.class, 0, "def" );
		assertEquals( "def", v.toString() );
		
		v = new MyTypeValidator( int.class, int.class, 0, "ghi" );
		assertEquals( "ghi", v.toString() );
	}

	/** @throws Exception */
	@Test
	public void testSilly() throws Exception
	{
		new MyTypeValidator( int.class, int.class, 0, "ghi" ).elementValidator();
	}

	/** */
	public static class MyTypeValidator extends TypeValidator
	{
		/**
		 * @param scalarClass
		 * @param arrayClass
		 * @param nDims
		 * @param descr
		 */
		public MyTypeValidator( Class<?> scalarClass, Class<?> arrayClass,
			int nDims, String descr )
		{
			super( scalarClass, arrayClass, nDims, descr );
		}

		public Validator elementValidator()
		{
			return null;
		}
	}
}
