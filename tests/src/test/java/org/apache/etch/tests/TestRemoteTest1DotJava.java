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

package org.apache.etch.tests;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

import java.lang.reflect.Array;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.support.DeliveryService;
import org.apache.etch.bindings.java.support.Mailbox;
import org.apache.etch.bindings.java.transport.SessionMessage;
import org.apache.etch.tests.Test1.E1;
import org.apache.etch.tests.Test1.Excp1;
import org.apache.etch.tests.Test1.Excp3;
import org.apache.etch.tests.Test1.Excp4;
import org.apache.etch.tests.Test1.S1;
import org.apache.etch.tests.Test1.S2;
import org.apache.etch.util.core.Who;


/** Test of RemoteTest1 */
public class TestRemoteTest1DotJava
{
	/** @throws Exception */
	@org.junit.Test
	public void method_nothing() throws Exception
	{
		test.nothing();
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_nothing,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_nothing,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_nothing.getTimeout() );
	}

	/** @throws Exception */
	@org.junit.Test
	public void method_incr1() throws Exception
	{
		svc.xresult = 2;
		assertEquals( 2, test.incr( 1 ) );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_incr,
			new Field[] { ValueFactoryTest1._mf_x },
			new Object[] { 1 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_incr,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_incr.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_incr2() throws Exception
	{
		svc.xresult = 3;
		assertEquals( 3, test.incr( 2 ) );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_incr,
			new Field[] { ValueFactoryTest1._mf_x },
			new Object[] { 2 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_incr,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_incr.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_incr3() throws Exception
	{
		svc.xresult = -1;
		assertEquals( -1, test.incr( -2 ) );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_incr,
			new Field[] { ValueFactoryTest1._mf_x },
			new Object[] { -2 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_incr,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_incr.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_sub1() throws Exception
	{
		svc.xresult = 5;
		assertEquals( 5, test.sub( 7, 2 ) );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sub,
			new Field[] { ValueFactoryTest1._mf_x, ValueFactoryTest1._mf_y },
			new Object[] { 7, 2 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sub,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sub.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_sub2() throws Exception
	{
		svc.xresult = 8;
		assertEquals( 8, test.sub( 23, 15 ) );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sub,
			new Field[] { ValueFactoryTest1._mf_x, ValueFactoryTest1._mf_y },
			new Object[] { 23, 15 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sub,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sub.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_sub3() throws Exception
	{
		svc.xresult = -5;
		assertEquals( -5, test.sub( 2, 7 ) );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sub,
			new Field[] { ValueFactoryTest1._mf_x, ValueFactoryTest1._mf_y },
			new Object[] { 2, 7 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sub,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sub.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_sum() throws Exception
	{
		svc.xresult = 24;
		int[] arg = { 1, 2, 3, 7, 11 };
		assertEquals( 24, test.sum( arg ) );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_sum,
			new Field[] { ValueFactoryTest1._mf_x },
			new Object[] { arg },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sum,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_sum.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_trans1() throws Exception
	{
		svc.xresult = 2;
		assertEquals( 2, test.trans( E1.A, 5 ) ); // divide by 2
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_trans,
			new Field[] { ValueFactoryTest1._mf_e, ValueFactoryTest1._mf_x },
			new Object[] { E1.A, 5 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_trans2() throws Exception
	{
		svc.xresult = 10;
		assertEquals( 10, test.trans( E1.B, 5 ) ); // mul by 2
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_trans,
			new Field[] { ValueFactoryTest1._mf_e, ValueFactoryTest1._mf_x },
			new Object[] { E1.B, 5 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_trans3() throws Exception
	{
		svc.xresult = 12;
		assertEquals( 12, test.trans( E1.C, 5 ) ); // add 7
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_trans,
			new Field[] { ValueFactoryTest1._mf_e, ValueFactoryTest1._mf_x },
			new Object[] { E1.C, 5 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_trans.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_dist1() throws Exception
	{
		svc.xresult = Math.sqrt( 3 );
		S1 arg1 =  new S1( 1, 1, 1 );
		S1 arg2 = new S1( 0, 0, 0 );
		assertEquals( Math.sqrt( 3 ), test.dist( arg1, arg2 ) );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_dist,
			new Field[] { ValueFactoryTest1._mf_a, ValueFactoryTest1._mf_b },
			new Object[] { arg1, arg2 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_dist2() throws Exception
	{
		svc.xresult = Math.sqrt( 35 );
		S1 arg1 = new S1( 1, 2, 3 );
		S1 arg2 = new S1( 6, 5, 4 );
		assertEquals( Math.sqrt( 35 ), test.dist( arg1, arg2 ) );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_dist,
			new Field[] { ValueFactoryTest1._mf_a, ValueFactoryTest1._mf_b },
			new Object[] { arg1, arg2 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_dist3() throws Exception
	{
		svc.xresult = Math.sqrt( 56 );
		S1 arg1 = new S1( 1, 2, 3 );
		S1 arg2 = new S1( -1, -2, -3 );
		assertEquals( Math.sqrt( 56 ), test.dist( arg1, arg2 ) );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_dist,
			new Field[] { ValueFactoryTest1._mf_a, ValueFactoryTest1._mf_b },
			new Object[] { arg1, arg2 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_dist.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_fill1() throws Exception
	{
		svc.xresult = new int[] {};
		int[] x = test.fill( 0, 1 );
		assertEquals( 0, x.length );
		for (int i: x)
			assertEquals( 1, i );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_fill,
			new Field[] { ValueFactoryTest1._mf_n, ValueFactoryTest1._mf_x },
			new Object[] { 0, 1 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fill,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fill.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_fill2() throws Exception
	{
		svc.xresult = new int[] { 2 };
		int[] x = test.fill( 1, 2 );
		assertEquals( 1, x.length );
		for (int i: x)
			assertEquals( 2, i );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_fill,
			new Field[] { ValueFactoryTest1._mf_n, ValueFactoryTest1._mf_x },
			new Object[] { 1, 2 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fill,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fill.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_fill3() throws Exception
	{
		svc.xresult = new int[] { 3, 3 };
		int[] x = test.fill( 2, 3 );
		assertEquals( 2, x.length );
		for (int i: x)
			assertEquals( 3, i );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_fill,
			new Field[] { ValueFactoryTest1._mf_n, ValueFactoryTest1._mf_x },
			new Object[] { 2, 3 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fill,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_fill.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_blow1() throws Exception
	{
		try
		{
			svc.xresult = new Excp1( "foo", 2 );
			test.blow( "foo", 2 );
			fail( "blow did not throw" );
		}
		catch( Excp1 e )
		{
			assertEquals( "foo", e.msg );
			assertEquals( 2, e.code );
		}
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_blow,
			new Field[] { ValueFactoryTest1._mf_msg, ValueFactoryTest1._mf_code },
			new Object[] { "foo", 2 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_blow,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_blow.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_blow2() throws Exception
	{
		try
		{
			svc.xresult = new Excp1( "bar", 3 );
			test.blow( "bar", 3 );
			fail( "blow did not throw" );
		}
		catch( Excp1 e )
		{
			assertEquals( "bar", e.msg );
			assertEquals( 3, e.code );
		}
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_blow,
			new Field[] { ValueFactoryTest1._mf_msg, ValueFactoryTest1._mf_code },
			new Object[] { "bar", 3 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_blow,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_blow.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_beets1() throws Exception
	{
		svc.xresult = 5;
		assertEquals( 5, test.beets( E1.A ) );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets,
			new Field[] { ValueFactoryTest1._mf_e },
			new Object[] { E1.A },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_beets2() throws Exception
	{
		try
		{
			svc.xresult = new Excp3();
			test.beets( E1.B );
			fail( "beets did not throw" );
		}
		catch ( Excp3 e )
		{
			assertTrue( true );
		}
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets,
			new Field[] { ValueFactoryTest1._mf_e },
			new Object[] { E1.B },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_beets3() throws Exception
	{
		try
		{
			svc.xresult = new Excp4();
			test.beets( E1.C );
			fail( "beets did not throw" );
		}
		catch ( Excp4 e )
		{
			assertTrue( true );
		}
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets,
			new Field[] { ValueFactoryTest1._mf_e },
			new Object[] { E1.C },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_beets4() throws Exception
	{
		svc.xresult = null;
		assertNull( test.beets( null ) );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_beets,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_beets.getTimeout() );
	}

	/////////////
	// BOOLEAN //
	/////////////
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_boolean_null() throws Exception
	{
		Boolean a = null;
		svc.xresult = a;
		Boolean r = test.p_boolean( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_boolean,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_boolean() throws Exception
	{
		Boolean a = true;
		svc.xresult = a;
		Boolean r = test.p_boolean( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_boolean,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_boolean_array_null() throws Exception
	{
		boolean[] a = null;
		svc.xresult = a;
		boolean[] r = test.p_boolean_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_boolean_array,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean_array.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_boolean_array() throws Exception
	{
		boolean x = false;
		boolean y = true;
		boolean[] a = { x, y };
		svc.xresult = a;
		boolean[] r = test.p_boolean_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_boolean_array,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_boolean_array.getTimeout() );
	}

	//////////
	// BYTE //
	//////////
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_byte_null() throws Exception
	{
		Byte a = null;
		svc.xresult = a;
		Byte r = test.p_byte( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_byte,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_byte() throws Exception
	{
		Byte a = 1;
		svc.xresult = a;
		Byte r = test.p_byte( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_byte,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_byte_array_null() throws Exception
	{
		byte[] a = null;
		svc.xresult = a;
		byte[] r = test.p_byte_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_byte_array,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte_array.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_byte_array() throws Exception
	{
		byte x = 12;
		byte y = 23;
		byte[] a = { x, y };
		svc.xresult = a;
		byte[] r = test.p_byte_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_byte_array,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_byte_array.getTimeout() );
	}

	///////////
	// SHORT //
	///////////
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_short_null() throws Exception
	{
		Short a = null;
		svc.xresult = a;
		Short r = test.p_short( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_short,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_short() throws Exception
	{
		Short a = 1234;
		svc.xresult = a;
		Short r = test.p_short( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_short,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_short_array_null() throws Exception
	{
		short[] a = null;
		svc.xresult = a;
		short[] r = test.p_short_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_short_array,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short_array.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_short_array() throws Exception
	{
		short x = 1234;
		short y = 2345;
		short[] a = { x, y };
		svc.xresult = a;
		short[] r = test.p_short_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_short_array,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_short_array.getTimeout() );
	}

	/////////
	// INT //
	/////////
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_int_null() throws Exception
	{
		Integer a = null;
		svc.xresult = a;
		Integer r = test.p_int( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_int,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_int() throws Exception
	{
		Integer a = 12345678;
		svc.xresult = a;
		Integer r = test.p_int( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_int,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_int_array_null() throws Exception
	{
		int[] a = null;
		svc.xresult = a;
		int[] r = test.p_int_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_int_array,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int_array.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_int_array() throws Exception
	{
		int x = 12345678;
		int y = 23456789;
		int[] a = { x, y };
		svc.xresult = a;
		int[] r = test.p_int_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_int_array,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_int_array.getTimeout() );
	}

	//////////
	// LONG //
	//////////
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_long_null() throws Exception
	{
		Long a = null;
		svc.xresult = a;
		Long r = test.p_long( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_long,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_long() throws Exception
	{
		Long a = 1234567812345678L;
		svc.xresult = a;
		Long r = test.p_long( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_long,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_long_array_null() throws Exception
	{
		long[] a = null;
		svc.xresult = a;
		long[] r = test.p_long_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_long_array,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long_array.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_long_array() throws Exception
	{
		long x = 1234567812345678L;
		long y = 2345678923456789L;
		long[] a = { x, y };
		svc.xresult = a;
		long[] r = test.p_long_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_long_array,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_long_array.getTimeout() );
	}

	///////////
	// FLOAT //
	///////////
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_float_null() throws Exception
	{
		Float a = null;
		svc.xresult = a;
		Float r = test.p_float( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_float,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_float,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_float.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_float() throws Exception
	{
		Float a = 123.456f;
		svc.xresult = a;
		Float r = test.p_float( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_float,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_float,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_float.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_float_array_null() throws Exception
	{
		float[] a = null;
		svc.xresult = a;
		float[] r = test.p_float_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_float_array,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_float_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_float_array.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_float_array() throws Exception
	{
		float x = 123.456f;
		float y = 234.567f;
		float[] a = { x, y };
		svc.xresult = a;
		float[] r = test.p_float_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_float_array,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_float_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_float_array.getTimeout() );
	}

	////////////
	// DOUBLE //
	////////////
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_double_null() throws Exception
	{
		Double a = null;
		svc.xresult = a;
		Double r = test.p_double( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_double,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_double() throws Exception
	{
		Double a = 12345678.12345678;
		svc.xresult = a;
		Double r = test.p_double( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_double,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_double_array_null() throws Exception
	{
		double[] a = null;
		svc.xresult = a;
		double[] r = test.p_double_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_double_array,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double_array.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_double_array() throws Exception
	{
		double x = 12345678.12345678;
		double y = 23456789.23456789;
		double[] a = { x, y };
		svc.xresult = a;
		double[] r = test.p_double_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_double_array,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_double_array.getTimeout() );
	}

	////////////
	// STRING //
	////////////
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_string_null() throws Exception
	{
		String a = null;
		svc.xresult = a;
		String r = test.p_string( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_string,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_string() throws Exception
	{
		String a = "abc";
		svc.xresult = a;
		String r = test.p_string( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_string,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_string_array_null() throws Exception
	{
		String[] a = null;
		svc.xresult = a;
		String[] r = test.p_string_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_string_array,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string_array.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_string_array() throws Exception
	{
		String x = "abc";
		String y = "bcd";
		String[] a = { x, y };
		svc.xresult = a;
		String[] r = test.p_string_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_string_array,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string_array.getTimeout() );
	}
	
	////////////
	// OBJECT //
	////////////
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_object_null() throws Exception
	{
		Object a = null;
		svc.xresult = a;
		Object r = test.p_object( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_object,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_object,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_object.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_object() throws Exception
	{
		Object a = "abc";
		svc.xresult = a;
		Object r = test.p_object( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_object,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_object,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_object.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_object1() throws Exception
	{
		Object a1 = 23;
		svc.xresult = a1;
		Object r1 = test.p_object( a1 );
		assertObjectEquals( a1, r1 );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_object,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a1 },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_object,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_object.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_object_array_null() throws Exception
	{
		String[] a = null;
		svc.xresult = a;
		String[] r = test.p_string_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_string_array,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_string_array.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_object_array() throws Exception
	{
		Object x = "abc";
		Object y = "bcd";
		Object[] a = { x, y };
		svc.xresult = a;
		Object[] r = test.p_object_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_object_array,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_object_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_object_array.getTimeout() );		
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_object_array1() throws Exception
	{
		Object x = 22;
		Object y = 33;
		Object[] a = { x, y };
		svc.xresult = a;
		Object[] r = test.p_object_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_object_array,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_object_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_object_array.getTimeout() );		
	}
	
	

	////////
	// E1 //
	////////
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_E1_null() throws Exception
	{
		E1 a = null;
		svc.xresult = a;
		E1 r = test.p_E1( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_E1,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_E1,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_E1.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_E1() throws Exception
	{
		E1 a = E1.A;
		svc.xresult = a;
		E1 r = test.p_E1( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_E1,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_E1,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_E1.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_E1_array_null() throws Exception
	{
		E1[] a = null;
		svc.xresult = a;
		E1[] r = test.p_E1_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_E1_array,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_E1_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_E1_array.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_E1_array() throws Exception
	{
		E1 x = E1.A;
		E1 y = E1.B;
		E1[] a = { x, y };
		svc.xresult = a;
		E1[] r = test.p_E1_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_E1_array,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_E1_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_E1_array.getTimeout() );
	}

	////////
	// S1 //
	////////
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_S1_null() throws Exception
	{
		S1 a = null;
		svc.xresult = a;
		S1 r = test.p_S1( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S1,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S1,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S1.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_S1() throws Exception
	{
		S1 a = new S1( 1, 2, 3 );
		svc.xresult = a;
		S1 r = test.p_S1( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S1,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S1,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S1.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_S1_array_null() throws Exception
	{
		S1[] a = null;
		svc.xresult = a;
		S1[] r = test.p_S1_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S1_array,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S1_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S1_array.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_S1_array() throws Exception
	{
		S1 x = new S1( 1, 2, 3 );
		S1 y = new S1( 2, 3, 4 );
		S1[] a = { x, y };
		svc.xresult = a;
		S1[] r = test.p_S1_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S1_array,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S1_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S1_array.getTimeout() );
	}

	////////
	// S2 //
	////////
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_S2_null() throws Exception
	{
		S2 a = null;
		svc.xresult = a;
		S2 r = test.p_S2( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S2,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S2,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S2.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_S2() throws Exception
	{
		S2 a = new S2( new S1( 1, 2, 3 ), new S1( 2, 3, 4 ), E1.A );
		svc.xresult = a;
		S2 r = test.p_S2( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S2,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S2,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S2.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_S2_array_null() throws Exception
	{
		S2[] a = null;
		svc.xresult = a;
		S2[] r = test.p_S2_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S2_array,
			new Field[] {},
			new Object[] {},
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S2_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S2_array.getTimeout() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_p_S2_array() throws Exception
	{
		S2 x = new S2( new S1( 1, 2, 3 ), new S1( 2, 3, 4 ), E1.A );
		S2 y = new S2( new S1( 2, 3, 4 ), new S1( 3, 4, 5 ), E1.B );
		S2[] a = { x, y };
		svc.xresult = a;
		S2[] r = test.p_S2_array( a );
		assertObjectEquals( a, r );
		check( What.CALL,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1_p_S2_array,
			new Field[] { ValueFactoryTest1._mf_a },
			new Object[] { a },
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S2_array,
			ValueFactoryTest1._mt_org_apache_etch_tests_Test1__result_p_S2_array.getTimeout() );
	}

	///////////////////
	// UTILITY STUFF //
	///////////////////

	/**
	 * @param a
	 * @param b
	 */
	private void assertObjectEquals( Object a, Object b )
	{
		if (a == b)
			return;
		
		if (a == null || b == null)
		{
			fail( "a == null || b == null" );
			return;
		}
		
		assertSame( a.getClass(), b.getClass() );
		
		if (a.getClass().isArray() && b.getClass().isArray())
		{
			assertArrayEquals( a, b );
			return;
		}
		
		assertEquals( a, b );
	}
	
	/**
	 * @param a
	 * @param b
	 */
	private void assertArrayEquals( Object a, Object b )
	{
		int n = Array.getLength( a );
		assertEquals( n, Array.getLength( b ) );
		for (int i = 0; i < n; i++)
			assertObjectEquals( Array.get( a, i ), Array.get( b, i ) );
	}

	private void check( What what, Type type, Field[] fields, Object[] objects,
		Type resultType, int timeout )
	{
		assertSame( "what", what, svc.what );
		assertSame( "type", type, svc.xmsg.type() );
		
		int n = fields.length;
		assertEquals( "objects.length", n, objects.length );
		assertEquals( "msg.size", n, svc.xmsg.size() );
//		System.out.println("size = " + svc.xmsg.size());
		
		for (int i = 0; i < n; i++)
		{
			Field f = fields[i];
			assertEquals( "object_"+i, objects[i], svc.xmsg.get( f ) );
		}
		
		assertSame( "resultType", resultType, svc.xresponseType );
		assertSame( "responseField", ValueFactoryTest1._mf_result, svc.xresponseField );
		assertEquals( "timeout", timeout, svc.xtimeout );
	}
	
	/** type of call */
	public enum What
	{
		/** */
		SEND,
		/** */
		CALL,
		/** */
		SHUTDOWNOUTPUT,
		/** */
		BEGINCALL
	}
	
	private final MyDeliveryService svc = new MyDeliveryService();
	
	private final Test1 test = new RemoteTest1( svc, new ValueFactoryTest1( "tcp:" ) );

	/** */
	public static class MyDeliveryService implements DeliveryService
	{
		/** */
		public What what;
		/** */
		public Message xmsg;
		/** */
		public Type xresponseType;
		/** */
		public Field xresponseField;
		/** */
		public Integer xtimeout;
		/** */
		public Object xresult;
		/** */
		public Mailbox xmb;

		/** */
		public void clear()
		{
			what = null;
			xmsg = null;
			xresponseType = null;
			xresponseField = null;
			xtimeout = null;
			xresult = null;
		}

		public void transportMessage( Who recipient, Message msg )
			throws Exception
		{
			assertNull( what );
			what = What.SEND;
			xmsg = msg;
		}

		public Mailbox begincall( Message msg )
		{
			assertNull( what );
			what = What.BEGINCALL;
			xmsg = msg;
			xmb = new MyMailbox();
			return xmb;
		}
		
		public Object endcall( Mailbox mb, Type responseType ) throws Exception
		{
			assertSame( What.BEGINCALL, what );
			assertSame( xmb, mb );
			what = What.CALL;
			xmb = null;
			xresponseType = responseType;
			xresponseField = responseType.getResponseField();
			xtimeout = responseType.getTimeout();
			if (xresult instanceof Exception)
				throw (Exception) xresult;
			return xresult;
		}

		public Object transportQuery( Object query ) throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public void transportControl( Object control, Object value ) throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public void transportNotify( Object event ) throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public boolean sessionMessage( Who sender, Message msg )
			throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public Object sessionQuery( Object query ) throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public void sessionControl( Object control, Object value )
			throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public void sessionNotify( Object event ) throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public SessionMessage getSession()
		{
			throw new UnsupportedOperationException();
		}

		public void setSession( SessionMessage session )
		{
			throw new UnsupportedOperationException();
		}
	}
	
	static class MyMailbox implements Mailbox
	{
		public boolean closeDelivery()
		{
			throw new UnsupportedOperationException();
		}

		public boolean closeRead() throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public Long getMessageId()
		{
			throw new UnsupportedOperationException();
		}

		public boolean isClosed()
		{
			throw new UnsupportedOperationException();
		}

		public boolean isEmpty()
		{
			throw new UnsupportedOperationException();
		}

		public boolean isFull()
		{
			throw new UnsupportedOperationException();
		}

		public boolean message( Who sender, Message msg ) throws Exception
		{
			throw new UnsupportedOperationException();
		}

		public Element read() throws InterruptedException
		{
			throw new UnsupportedOperationException();
		}

		public Element read( int maxDelay ) throws InterruptedException
		{
			throw new UnsupportedOperationException();
		}

		public void registerNotify( Notify notify, Object state, int maxDelay )
		{
			throw new UnsupportedOperationException();
		}

		public void unregisterNotify( Notify notify )
		{
			throw new UnsupportedOperationException();
		}
	}
}
