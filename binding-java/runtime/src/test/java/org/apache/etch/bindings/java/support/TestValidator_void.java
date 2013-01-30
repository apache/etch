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

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import org.apache.etch.bindings.java.msg.Validator;
import org.junit.Test;


/**
 * Test Validator_void.
 */
public class TestValidator_void
{
    /** */
	@Test
	public void test()
	{
		test( 0, "void[0]", null, 1 );
		test( 0, "void[0]", null, (float) 2 );
		test( 0, "void[0]", null, (double) 3 );
		test( 0, "void[0]", null, (byte) 4 );
		test( 0, "void[0]", null, (long) 5 );
		test( 0, "void[0]", null, (short) 6 );
		
	}
	
	private void test( int n, String s, Object good, Object bad )
	{
		Validator_void v = Validator_void.get( n );
		assertTrue( v.validate( good ) );
		assertFalse( v.validate( bad ) );
	}

    /** */
	@Test( expected = IllegalArgumentException.class )
	public void testGetNeg1()
	{
		Validator_void.get( -1 );
	}

    /** */
	@Test
	public void testGetNeg2()
	{
		assertNull( Validator_void.get( 0 ).elementValidator() );
	}

    /** */
	@Test( expected = IllegalArgumentException.class )
	public void testGetMaxPlusOne()
	{
		Validator_void.get( Validator.MAX_NDIMS+1 );
	}
}
