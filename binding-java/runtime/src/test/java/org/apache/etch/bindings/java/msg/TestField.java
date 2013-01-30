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

import org.apache.etch.util.Hash;
import org.junit.Test;


/** Test of Field */
public class TestField
{
	/** */
	@Test
	public void fieldIntegerString()
	{
		testMf( 1, "one" );
		testMf( 2, "two" );
		testMf( 3, "three" );
	}

	/** */
	@Test
	public void fieldString()
	{
		testMf( "one" );
		testMf( "two" );
		testMf( "three" );
	}

	private void testMf( int id, String name )
	{
		Field mf = new Field( id, name );
		assertEquals( id, mf.getId() );
		assertEquals( name, mf.getName() );
	}

	private void testMf( String name )
	{
		Field mf = new Field( name );
		assertEquals( Hash.hash( name ), mf.getId() );
		assertEquals( name, mf.getName() );
	}
}
