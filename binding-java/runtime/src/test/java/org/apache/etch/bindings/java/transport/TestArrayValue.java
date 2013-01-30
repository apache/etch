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

package org.apache.etch.bindings.java.transport;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;

import java.util.Date;

import org.apache.etch.bindings.java.msg.Type;
import org.junit.Test;


/**
 * Tests ArrayValue.
 */
public class TestArrayValue
{
	/** @throws Exception */
	@Test
	public void constructor1() throws Exception
	{
		ArrayValue av = new ArrayValue( new byte[0] );
		assertEquals( (byte) 0, av.typeCode() );
		assertNull( av.customStructType() );
		assertEquals( 0, av.dim() );
	}
	
	/** @throws Exception */
	@Test
	public void constructor2() throws Exception
	{
		Type t1 = new Type( "t1" );
		ArrayValue av = new ArrayValue( new byte[0], (byte) 1, t1, 2 );
		assertEquals( (byte) 1, av.typeCode() );
		assertEquals( t1, av.customStructType() );
		assertEquals( 2, av.dim() );
	}
	
	/** @throws Exception */
	@Test
	public void constructor3() throws Exception
	{
		Type t2 = new Type( "t2" );
		ArrayValue av = new ArrayValue( new byte[0], (byte) 3, t2, 4 );
		assertEquals( (byte) 3, av.typeCode() );
		assertEquals( t2, av.customStructType() );
		assertEquals( 4, av.dim() );
	}
	
	/** */
	@Test
	public void addMany1()
	{
		Date dt = new Date();
		ArrayValue av = new ArrayValue( new Object[] { false, (byte) 1, (short) 2, 3, (long) 4, (float) 1.5, 1.6, "abc", null, dt } );
		assertEquals( 10, av.size() );
		int index = 0;
		assertEquals( false, av.get( index++ ) );
		assertEquals( (byte) 1, av.get( index++ ) );
		assertEquals( (short) 2, av.get( index++ ) );
		assertEquals( 3, av.get( index++ ) );
		assertEquals( (long) 4, av.get( index++ ) );
		assertEquals( (float) 1.5, av.get( index++ ) );
		assertEquals( 1.6, av.get( index++ ) );
		assertEquals( "abc", av.get( index++ ) );
		assertEquals( null, av.get( index++ ) );
		assertEquals( dt, av.get( index++ ) );
	}
	
	/** */
	@Test
	public void addMany2()
	{
		Date dt = new Date();
		ArrayValue av = new ArrayValue( new Object[] { (float) 1.5, 1.6, "abc", null, dt, false, (byte) 1, (short) 2, 3, (long) 4 } );
		assertEquals( 10, av.size() );
		int index = 0;
		assertEquals( (float) 1.5, av.get( index++ ) );
		assertEquals( 1.6, av.get( index++ ) );
		assertEquals( "abc", av.get( index++ ) );
		assertEquals( null, av.get( index++ ) );
		assertEquals( dt, av.get( index++ ) );
		assertEquals( false, av.get( index++ ) );
		assertEquals( (byte) 1, av.get( index++ ) );
		assertEquals( (short) 2, av.get( index++ ) );
		assertEquals( 3, av.get( index++ ) );
		assertEquals( (long) 4, av.get( index++ ) );
	}
}
