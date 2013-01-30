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
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;
import static org.junit.Assert.fail;

/** Test of Test1 */
public class TestTest1DotJava
{
	/** @throws Exception */
	@org.junit.Test
	public void const_booleans() throws Exception
	{
		assertFalse( Test1.BOOL1 );
		assertTrue( Test1.BOOL2 );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void const_bytes() throws Exception
	{
		assertEquals( Byte.MIN_VALUE, Test1.BYTE1 );
		assertEquals( (byte) -1, Test1.BYTE2 );
		assertEquals( (byte) 0, Test1.BYTE3 );
		assertEquals( (byte) 1, Test1.BYTE4 );
		assertEquals( Byte.MAX_VALUE, Test1.BYTE5 );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void const_shorts() throws Exception
	{
		assertEquals( Short.MIN_VALUE, Test1.SHORT1 );
		assertEquals( (short) -1, Test1.SHORT2 );
		assertEquals( (short) 0, Test1.SHORT3 );
		assertEquals( (short) 1, Test1.SHORT4 );
		assertEquals( Short.MAX_VALUE, Test1.SHORT5 );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void const_ints() throws Exception
	{
		assertEquals( Integer.MIN_VALUE, Test1.INT1 );
		assertEquals( -1, Test1.INT2 );
		assertEquals( 0, Test1.INT3 );
		assertEquals( 1, Test1.INT4 );
		assertEquals( Integer.MAX_VALUE, Test1.INT5 );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void const_longs() throws Exception
	{
		assertEquals( Long.MIN_VALUE, Test1.LONG1 );
		assertEquals( (long) -1, Test1.LONG2 );
		assertEquals( (long) 0, Test1.LONG3 );
		assertEquals( (long) 1, Test1.LONG4 );
		assertEquals( Long.MAX_VALUE, Test1.LONG5 );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void const_floats() throws Exception
	{
		assertEquals( Float.MIN_VALUE, Test1.FLOAT1 );
		assertEquals( (float) -1, Test1.FLOAT2 );
		assertEquals( (float) 0, Test1.FLOAT3 );
		assertEquals( (float) 1, Test1.FLOAT4 );
		assertEquals( Float.MAX_VALUE, Test1.FLOAT5 );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void const_doubles() throws Exception
	{
		assertEquals( Double.MIN_VALUE, Test1.DOUBLE1 );
		assertEquals( (double) -1, Test1.DOUBLE2 );
		assertEquals( (double) 0, Test1.DOUBLE3 );
		assertEquals( (double) 1, Test1.DOUBLE4 );
		assertEquals( Double.MAX_VALUE, Test1.DOUBLE5 );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void const_strings() throws Exception
	{
		assertEquals( "", Test1.STRING1 );
		assertEquals( "a", Test1.STRING2 );
		assertEquals( "abc", Test1.STRING3 );
		assertEquals( "a\tb\tc\r\n", Test1.STRING4 );
		assertEquals( "x x", Test1.STRING5 );
		assertEquals( "x\u00bfx", Test1.STRING6 );
		assertEquals( "x\u03a9x", Test1.STRING7 );
		assertEquals( "x\\x", Test1.STRING8 );
		assertEquals( "x'x", Test1.STRING9 );
		assertEquals( "x\"x", Test1.STRING10 );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void enum_E1() throws Exception
	{
		assertEquals( 3, Test1.E1.values().length );
		assertNotNull( Test1.E1.A );
		assertNotNull( Test1.E1.B );
		assertNotNull( Test1.E1.C );
		assertSame( Test1.E1.A, Test1.E1.valueOf( "A" ) );
		assertSame( Test1.E1.B, Test1.E1.valueOf( "B" ) );
		assertSame( Test1.E1.C, Test1.E1.valueOf( "C" ) );
		assertEquals( 0, Test1.E1.A.ordinal() );
		assertEquals( 1, Test1.E1.B.ordinal() );
		assertEquals( 2, Test1.E1.C.ordinal() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void struct_S1() throws Exception
	{
		Test1.S1 s = new Test1.S1( 1, 2, 3 );
		assertEquals( 1, s.x );
		assertEquals( 2, s.y );
		assertEquals( 3, s.z );
		
		s = new Test1.S1( null, null, null );
		assertNull( s.x );
		assertNull( s.y );
		assertNull( s.z );
		
		s = new Test1.S1();
		assertNull( s.x );
		assertNull( s.y );
		assertNull( s.z );
		
		s.x = 4;
		s.y = 5;
		s.z = 6;
		
		assertEquals( 4, s.x );
		assertEquals( 5, s.y );
		assertEquals( 6, s.z );
		
		s.x = null;
		assertNull( s.x );
		
		s.x = 7;
		assertEquals( 7, s.x );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void except_Excp1() throws Exception
	{
		Test1.Excp1 e = new Test1.Excp1( "foo", 23 );
		assertEquals( "foo", e.msg );
		assertEquals( 23, e.code );
		
		e = new Test1.Excp1( null, null );
		assertNull( e.msg );
		assertNull( e.code );
		
		e = new Test1.Excp1();
		assertNull( e.msg );
		assertNull( e.code );
		
		e.msg = "bar";
		e.code = 24;
		
		assertEquals( "bar", e.msg );
		assertEquals( 24, e.code );
		
		e.msg = null;
		e.code = null;
		
		assertNull( e.msg );
		assertNull( e.code );

		Throwable t = e;
		assertTrue( t instanceof Exception );
		assertFalse( t instanceof RuntimeException );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void except_Excp2() throws Exception
	{
		Test1.Excp2 e = new Test1.Excp2();

		Throwable t = e;
		assertTrue( t instanceof Exception );
		assertTrue( t instanceof RuntimeException );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void except_Excp3() throws Exception
	{
		Test1.Excp3 e = new Test1.Excp3();

		Throwable t = e;
		assertTrue( t instanceof Exception );
		assertTrue( t instanceof RuntimeException );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void except_Excp4() throws Exception
	{
		Test1.Excp4 e = new Test1.Excp4();

		Throwable t = e;
		assertTrue( t instanceof Exception );
		assertFalse( t instanceof RuntimeException );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_nothing() throws Exception
	{
		test.nothing();
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_incr1() throws Exception
	{
		assertEquals( 2, test.incr( 1 ) );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_incr2() throws Exception
	{
		assertEquals( 3, test.incr( 2 ) );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_incr3() throws Exception
	{
		assertEquals( -1, test.incr( -2 ) );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_sub1() throws Exception
	{
		assertEquals( 5, test.sub( 7, 2 ) );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_sub2() throws Exception
	{
		assertEquals( 8, test.sub( 23, 15 ) );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_sub3() throws Exception
	{
		assertEquals( -5, test.sub( 2, 7 ) );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_sum() throws Exception
	{
		assertEquals( 24, test.sum( new int[] { 1, 2, 3, 7, 11 } ) );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_trans1() throws Exception
	{
		assertEquals( 2, test.trans( Test1.E1.A, 5 ) ); // divide by 2
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_trans2() throws Exception
	{
		assertEquals( 10, test.trans( Test1.E1.B, 5 ) ); // mul by 2
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_trans3() throws Exception
	{
		assertEquals( 12, test.trans( Test1.E1.C, 5 ) ); // add 7
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_dist1() throws Exception
	{
		assertEquals( Math.sqrt( 3 ), test.dist( new Test1.S1( 1, 1, 1 ), new Test1.S1( 0, 0, 0 ) ) );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_dist2() throws Exception
	{
		assertEquals( Math.sqrt( 35 ), test.dist( new Test1.S1( 1, 2, 3 ), new Test1.S1( 6, 5, 4 ) ) );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_dist3() throws Exception
	{
		assertEquals( Math.sqrt( 56 ), test.dist( new Test1.S1( 1, 2, 3 ), new Test1.S1( -1, -2, -3 ) ) );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_fill1() throws Exception
	{
		int[] x = test.fill( 0, 1 );
		assertEquals( 0, x.length );
		for (int i: x)
			assertEquals( 1, i );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_fill2() throws Exception
	{
		int[] x = test.fill( 1, 2 );
		assertEquals( 1, x.length );
		for (int i: x)
			assertEquals( 2, i );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_fill3() throws Exception
	{
		int[] x = test.fill( 2, 3 );
		assertEquals( 2, x.length );
		for (int i: x)
			assertEquals( 3, i );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_fillObject() throws Exception
	{
		int sz = 10;
		
		// All const types
		method_fillObjectHelper( sz, Test1.BOOL1 );	
		method_fillObjectHelper( sz, Test1.BYTE5 );
		method_fillObjectHelper( sz, Test1.SHORT5 );	
		method_fillObjectHelper( sz, Test1.INT5 );
		method_fillObjectHelper( sz, Test1.LONG5 );	
		method_fillObjectHelper( sz, Test1.FLOAT5 );
		method_fillObjectHelper( sz, Test1.DOUBLE5 );	
		method_fillObjectHelper( sz, Test1.STRING3 );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_blow1() throws Exception
	{
		try
		{
			test.blow( "foo", 2 );
			fail( "blow did not throw" );
		}
		catch( Test1.Excp1 e )
		{
			assertEquals( "foo", e.msg );
			assertEquals( 2, e.code );
		}
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_blow2() throws Exception
	{
		try
		{
			test.blow( "bar", 3 );
			fail( "blow did not throw" );
		}
		catch( Test1.Excp1 e )
		{
			assertEquals( "bar", e.msg );
			assertEquals( 3, e.code );
		}
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_beets1() throws Exception
	{
		assertEquals( 5, test.beets( Test1.E1.A ) );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_beets2() throws Exception
	{
		try
		{
			test.beets( Test1.E1.B );
			fail( "beets did not throw" );
		}
		catch ( Test1.Excp3 e )
		{
			assertTrue( true );
		}
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_beets3() throws Exception
	{
		try
		{
			test.beets( Test1.E1.C );
			fail( "beets did not throw" );
		}
		catch ( Test1.Excp4 e )
		{
			assertTrue( true );
		}
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_beets4() throws Exception
	{
		assertNull( test.beets( null ) );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void struct_S3() throws Exception
	{
		testS3Helper( "boolean", Test1.BOOL2 );
		testS3Helper( "byte", Test1.BYTE5 );
		testS3Helper( "short", Test1.SHORT5 );
		testS3Helper( "int", Test1.INT5 );
		testS3Helper( "long", Test1.LONG5 );
		testS3Helper( "float", Test1.FLOAT5 );
		testS3Helper( "double", Test1.DOUBLE5 );
		testS3Helper( "string", Test1.STRING3 );		
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void struct_S4() throws Exception
	{		
		int sz = 10;
		
		testS4Helper(sz, "boolean", Test1.BOOL2 );
		testS4Helper(sz, "byte", Test1.BYTE5 );
		testS4Helper(sz, "short", Test1.SHORT5 );
		testS4Helper(sz, "int", Test1.INT5 );
		testS4Helper(sz, "long", Test1.LONG5 );
		testS4Helper(sz, "float", Test1.FLOAT5 );
		testS4Helper(sz, "double", Test1.DOUBLE5 );
		testS4Helper(sz, "string", Test1.STRING3 );		

	}
	
	/** @throws Exception */
	@org.junit.Test
	public void except_Excp5() throws Exception
	{
		int code = 500;
		String msg = "Excp5";
		
		testExcp5Helper (msg, code, Test1.BOOL1);
		testExcp5Helper (msg, code, Test1.BYTE5);
		testExcp5Helper (msg, code, Test1.SHORT5);
		testExcp5Helper (msg, code, Test1.INT5);
		testExcp5Helper (msg, code, Test1.LONG5);
		testExcp5Helper (msg, code, Test1.FLOAT5);
		testExcp5Helper (msg, code, Test1.DOUBLE5);
		testExcp5Helper (msg, code, Test1.STRING3);		
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void except_Excp6() throws Exception
	{
		int sz = 10;
		int code = 500;
		String msg = "Excp6";
		
		testExcp6Helper( sz, msg, code, Test1.BOOL1 );
		testExcp6Helper( sz, msg, code, Test1.BYTE5 );
		testExcp6Helper( sz, msg, code, Test1.SHORT5 );
		testExcp6Helper( sz, msg, code, Test1.INT5 );
		testExcp6Helper( sz, msg, code, Test1.LONG5 );
		testExcp6Helper( sz, msg, code, Test1.FLOAT5 );
		testExcp6Helper( sz, msg, code, Test1.DOUBLE5 );
		testExcp6Helper( sz, msg, code, Test1.STRING3 );		
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_throwExcp5() throws Exception
	{
		String msg = "throwExcp5";
		int code = 500;

		method_throwExcp5Helper( msg, code, Test1.BOOL1 );
		method_throwExcp5Helper( msg, code, Test1.BYTE5 );
		method_throwExcp5Helper( msg, code, Test1.SHORT5 );
		method_throwExcp5Helper( msg, code, Test1.INT5 );
		method_throwExcp5Helper( msg, code, Test1.LONG5 );
		method_throwExcp5Helper( msg, code, Test1.FLOAT5 );
		method_throwExcp5Helper( msg, code, Test1.DOUBLE5 );
		method_throwExcp5Helper( msg, code, Test1.STRING3 );		
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void method_throwExcp6() throws Exception
	{
		String msg = "throwExcp6";
		int code = 500;
		int sz = 10;

		method_throwExcp6Helper( sz, msg, code, Test1.BOOL1 );
		method_throwExcp6Helper( sz, msg, code, Test1.BYTE5 );
		method_throwExcp6Helper( sz, msg, code, Test1.SHORT5 );
		method_throwExcp6Helper( sz, msg, code, Test1.INT5 );
		method_throwExcp6Helper( sz, msg, code, Test1.LONG5 );
		method_throwExcp6Helper( sz, msg, code, Test1.FLOAT5 );
		method_throwExcp6Helper( sz, msg, code, Test1.DOUBLE5 );
		method_throwExcp6Helper( sz, msg, code, Test1.STRING3 );		

	}
	
	//
	// Utility methods
	//
	
	private void testS3Helper( String type, Object value )
	{
		Test1.S3 s = new Test1.S3( type, value );
		assertEquals( type, s.tipe );
		assertEquals( value, s.x );
		
		s = new Test1.S3( null, null );
		assertNull( s.tipe );
		assertNull( s.x );
		
		s = new Test1.S3();
		assertNull( s.tipe );
		assertNull( s.x );
		
		s.tipe = type;
		s.x = value;	
		assertEquals( type, s.tipe );
		assertEquals( value, s.x );
		
		s.x = null;
		assertNull( s.x );
		
		s.x = value;
		assertEquals( value, s.x );
	}

	/*
	 * jacapps 10-OCT-2007
	 * 
	 * testS4Helper - Fill an S4 struct with a values for the type 
	 * and Object array and verify the values.
	 * 
	 * @param sz - The number of elements in the Object array
	 * @param type - The type of the data to be put in Object array
	 * @param value - The value of type to fill the array with
	 * 
	 */
	private void testS4Helper( int sz, String type, Object value )
	{
		Object[] myObject = test.fillObject( sz, value );
		
		Test1.S4 s = new Test1.S4( type, myObject );
		assertEquals( type, s.tipe );
		assertEquals( sz, myObject.length );
		
		for (Object element : myObject)
		{
			assertEquals( value, element );
		}
	}

	private void testExcp5Helper (String msg, int code, Object value)
	{
		Test1.Excp5 e = new Test1.Excp5( msg, code, value );
		assertEquals( msg, e.msg );
		assertEquals( code, e.code );
		assertEquals( value, e.x);
		
		e = new Test1.Excp5( null, null, null );
		assertNull( e.msg );
		assertNull( e.code );
		assertNull( e.x);

		e = new Test1.Excp5();
		assertNull( e.msg );
		assertNull( e.code );
		assertNull( e.x );
		
		e.msg = msg;
		e.code = code;
		e.x = value;
		
		assertEquals( msg, e.msg );
		assertEquals( code, e.code );
		assertEquals( value, e.x );
		
		e.msg = null;
		e.code = null;
		e.x = null;
		
		assertNull( e.msg );
		assertNull( e.code );
		assertNull( e.x );		

		Throwable t = e;
		assertTrue( t instanceof Exception );
		assertFalse( t instanceof RuntimeException );
	}
	
	private void testExcp6Helper (int sz, String msg, int code, Object value )
	{
		Object[] myObject = test.fillObject( sz, value );
		
		Test1.Excp6 e = new Test1.Excp6( msg, code, myObject );
		assertEquals( msg, e.msg );
		assertEquals( code, e.code );

		for (Object element : myObject)
		{
			assertEquals( value, element );
		}
		
		e = new Test1.Excp6( null, null, null );
		assertNull( e.msg );
		assertNull( e.code );
		assertNull( e.x );

		e = new Test1.Excp6();
		assertNull( e.msg );
		assertNull( e.code );
		assertNull( e.x );
		
		e.msg = msg;
		e.code = code;
		e.x = myObject;
		
		assertEquals( msg, e.msg );
		assertEquals( code, e.code );
		
		for (Object element : myObject)
		{
			assertEquals( value, element );
		}
		
		e.msg = null;
		e.code = null;
		e.x = null;
		
		assertNull( e.msg );
		assertNull( e.code );
		assertNull( e.x );		

		Throwable t = e;
		assertTrue( t instanceof Exception );
		assertFalse( t instanceof RuntimeException );
	}
	
	private void method_throwExcp5Helper (String msg, int code, Object value)
	{
		try
		{
			test.throwExcp5( msg, code, value );
			fail( "throwExcp5 did not throw" );
		}
		catch( Test1.Excp5 e )
		{
			assertEquals( msg, e.msg );
			assertEquals( code, e.code );
			assertEquals( value, e.x );
		}
	}
	
	private void method_throwExcp6Helper (int sz, String msg, int code, Object value)
	{
		Object[] myObject = test.fillObject( sz, value );
		
		try
		{
			test.throwExcp6( msg, code, myObject );
			fail( "throwExcp5 did not throw" );
		}
		catch( Test1.Excp6 e )
		{
			assertEquals( msg, e.msg );
			assertEquals( code, e.code );
			
			for ( int i = 0; i < myObject.length; i++ ) 
			{
				assertEquals( value, e.x[i] );
			}
		}
	}
	
	private void method_fillObjectHelper (int sz, Object value)
	{
		Object[] myObject = test.fillObject( sz, value );
		
		for (Object element : myObject)
		{
			assertEquals( value, element );
		}	
	}

	private final Test1 test = new FakeTest1();
}
