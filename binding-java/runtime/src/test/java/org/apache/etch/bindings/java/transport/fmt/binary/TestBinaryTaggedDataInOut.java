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

package org.apache.etch.bindings.java.transport.fmt.binary;

import java.io.IOException;
import java.lang.reflect.Array;
import java.util.Date;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.TypeMap;
import org.apache.etch.bindings.java.msg.Validator;
import org.apache.etch.bindings.java.msg.Validator.Level;
import org.apache.etch.bindings.java.support.Class2TypeMap;
import org.apache.etch.bindings.java.support.DefaultValueFactory;
import org.apache.etch.bindings.java.support.Validator_boolean;
import org.apache.etch.bindings.java.support.Validator_byte;
import org.apache.etch.bindings.java.support.Validator_double;
import org.apache.etch.bindings.java.support.Validator_float;
import org.apache.etch.bindings.java.support.Validator_int;
import org.apache.etch.bindings.java.support.Validator_long;
import org.apache.etch.bindings.java.support.Validator_object;
import org.apache.etch.bindings.java.support.Validator_short;
import org.apache.etch.bindings.java.support.Validator_string;
import org.apache.etch.bindings.java.transport.Messagizer;
import org.apache.etch.bindings.java.transport.fmt.TypeCode;
import org.apache.etch.util.FlexBuffer;
import org.apache.etch.util.Log;
import org.apache.etch.util.Resources;
import org.apache.etch.util.URL;
import org.apache.etch.util.core.Who;
import org.apache.etch.util.core.io.SessionPacket;
import org.apache.etch.util.core.io.Transport;
import org.apache.etch.util.core.io.TransportPacket;
import org.junit.Assert;
import org.junit.Test;


/**
 * Test binary input and output.
 */
public class TestBinaryTaggedDataInOut
{
	private static final double DOUBLE_MIN_NORMAL = 0x1.0p-1022; // Double.MIN_NORMAL;
	private static final float FLOAT_MIN_NORMAL = 0x1.0p-126f; // Float.MIN_NORMAL;
	
	static
	{
		Log.addSink( null );
	}
	
	/**
	 * @throws Exception
	 */
	@Test public void test_check_value() throws Exception
	{
		// i'm testing with hard coded constants on the left hand
		// side here because this represents our external interface
		// for messages. if these values change, it could bring about
		// accidental missile launches, inadvertant bank balance transfers
		// to unfriendly foreign nations, or the dropping of a small
		// child's lollipop into the sand. like a pizza face down in
		// the parking lot, you don't wanna go there.
		
//		ValueFactory vf = new DummyValueFactory();
		BinaryTaggedDataOutput btdo = new BinaryTaggedDataOutput( vf, "none:" );
		
		// byte values
		for (byte i: getBytes( Byte.MIN_VALUE, 256 ))
			if (i >= TypeCode.MIN_TINY_INT && i <= TypeCode.MAX_TINY_INT)
				Assert.assertEquals( i, btdo.checkValue( i ) );
			else
				Assert.assertEquals( TypeCode.BYTE, btdo.checkValue( i ) );
		
		// short values
		for (short i: getShorts( Short.MIN_VALUE, 65536 ))
			if (i >= TypeCode.MIN_TINY_INT && i <= TypeCode.MAX_TINY_INT)
				Assert.assertEquals( (byte) i, btdo.checkValue( i ) );
			else if (i >= Byte.MIN_VALUE && i <= Byte.MAX_VALUE)
				Assert.assertEquals( TypeCode.BYTE, btdo.checkValue( i ) );
			else
				Assert.assertEquals( TypeCode.SHORT, btdo.checkValue( i ) );
		
		// int values
		for (int i: getTestInts())
			if (i >= TypeCode.MIN_TINY_INT && i <= TypeCode.MAX_TINY_INT)
				Assert.assertEquals( (byte) i, btdo.checkValue( i ) );
			else if (i >= Byte.MIN_VALUE && i <= Byte.MAX_VALUE)
				Assert.assertEquals( TypeCode.BYTE, btdo.checkValue( i ) );
			else if (i >= Short.MIN_VALUE && i <= Short.MAX_VALUE)
				Assert.assertEquals( TypeCode.SHORT, btdo.checkValue( i ) );
			else
				Assert.assertEquals( TypeCode.INT, btdo.checkValue( i ) );
		
		// long values
		for (long i: getTestLongs())
			if (i >= TypeCode.MIN_TINY_INT && i <= TypeCode.MAX_TINY_INT)
				Assert.assertEquals( (byte) i, btdo.checkValue( i ) );
			else if (i >= Byte.MIN_VALUE && i <= Byte.MAX_VALUE)
				Assert.assertEquals( TypeCode.BYTE, btdo.checkValue( i ) );
			else if (i >= Short.MIN_VALUE && i <= Short.MAX_VALUE)
				Assert.assertEquals( TypeCode.SHORT, btdo.checkValue( i ) );
			else if (i >= Integer.MIN_VALUE && i <= Integer.MAX_VALUE)
				Assert.assertEquals( TypeCode.INT, btdo.checkValue( i ) );
			else
				Assert.assertEquals( TypeCode.LONG, btdo.checkValue( i ) );
		
		// null value
		Assert.assertEquals( TypeCode.NULL, btdo.checkValue( null ) );
		
		// boolean values
		Assert.assertEquals( TypeCode.BOOLEAN_FALSE, btdo.checkValue( false ) );
		Assert.assertEquals( TypeCode.BOOLEAN_TRUE, btdo.checkValue( true ) );
		
		// float
		Assert.assertEquals( TypeCode.FLOAT, btdo.checkValue( 3.14159f ) );
		
		// double
		Assert.assertEquals( TypeCode.DOUBLE, btdo.checkValue( 3.14159 ) );
		
		// byte array
		Assert.assertEquals( TypeCode.BYTES, btdo.checkValue( new byte[0] ) );
		Assert.assertEquals( TypeCode.BYTES, btdo.checkValue( new byte[1] ) );
		
		// string
		Assert.assertEquals( TypeCode.EMPTY_STRING, btdo.checkValue( "" ) );
		Assert.assertEquals( TypeCode.STRING, btdo.checkValue( "abc" ) );
		
		// struct
		Assert.assertEquals( TypeCode.CUSTOM, btdo.checkValue( new StructValue( new Type( "foo" ), vf ) ) );
		
		// custom
		Assert.assertEquals( TypeCode.CUSTOM, btdo.checkValue( new Date() ) );
		
		// none
		Assert.assertEquals( TypeCode.NONE, btdo.checkValue( BinaryTaggedData.NONE ) );
	}
	
	private long[] getTestLongs()
	{
		int n = 65536+2+6;
		int k = 65536+2;
		int min = Short.MIN_VALUE-1;
		long[] vals = new long[n];
		int i = 0;
		while (k-- > 0)
			vals[i++] = min++;
		vals[i++] = Integer.MIN_VALUE;
		vals[i++] = Integer.MAX_VALUE;
		vals[i++] = Integer.MIN_VALUE-1L;
		vals[i++] = Integer.MAX_VALUE+1L;
		vals[i++] = Long.MIN_VALUE;
		vals[i++] = Long.MAX_VALUE;
		return vals;
	}

	private int[] getTestInts()
	{
		int n = 65536+2+2;
		int k = 65536+2;
		int min = Short.MIN_VALUE-1;
		int[] vals = new int[n];
		int i = 0;
		while (k-- > 0)
			vals[i++] = min++;
		vals[i++] = Integer.MIN_VALUE;
		vals[i++] = Integer.MAX_VALUE;
		return vals;
	}

	private short[] getShorts( short min, int n )
	{
		short[] vals = new short[n];
		int i = 0;
		while (n-- > 0)
			vals[i++] = min++;
		return vals;
	}

	private byte[] getBytes( byte min, int n )
	{
		byte[] vals = new byte[n];
		int i = 0;
		while (n-- > 0)
			vals[i++] = min++;
		return vals;
	}

	/**
	 * @throws Exception
	 */
	@Test public void test_boolean() throws Exception
	{
		testx( new boolean[][] { { false, true }, { true, false } },
			Validator_boolean.get( 2 ) );
	}
	
	/**
	 * @throws Exception
	 */
	@Test public void test_byte() throws Exception
	{
		testx( new byte[][] { { Byte.MIN_VALUE, -1, 0, 1, Byte.MAX_VALUE }, { 23 } },
			Validator_byte.get( 2 ) );
	}
	
	/**
	 * @throws Exception
	 */
	@Test public void test_short() throws Exception
	{
		testx( new short[][] { { Short.MIN_VALUE, Byte.MIN_VALUE, -1, 0, 1,
			Byte.MAX_VALUE, Short.MAX_VALUE }, { 23 } },
			Validator_short.get( 2 ) );
	}
	
	/**
	 * @throws Exception
	 */
	@Test public void test_int() throws Exception
	{
		testx( new int[][] { { Integer.MIN_VALUE, Short.MIN_VALUE, Byte.MIN_VALUE,
			-1, 0, 1, Byte.MAX_VALUE, Short.MAX_VALUE, Integer.MAX_VALUE }, { 23 } },
			Validator_int.get( 2 ) );
	}
	
	/**
	 * @throws Exception
	 */
	@Test public void test_long() throws Exception
	{
		testx( new long[][] { { Long.MIN_VALUE, Integer.MIN_VALUE, Short.MIN_VALUE,
			Byte.MIN_VALUE, -1, 0, 1, Byte.MAX_VALUE, Short.MAX_VALUE,
			Integer.MAX_VALUE, Long.MAX_VALUE }, { 23 } },
			Validator_long.get( 2 ) );
	}
	
	/**
	 * @throws Exception
	 */
	@Test public void test_float() throws Exception
	{
		testx( new float[][] { { -1, 0, 1, Float.MIN_VALUE, FLOAT_MIN_NORMAL,
			Float.MAX_VALUE, Float.NaN, Float.NEGATIVE_INFINITY,
			Float.POSITIVE_INFINITY, -0.0f, 1.1f,
			3.141592653589793238462643383279f }, { 23 } },
			Validator_float.get( 2 ) );
	}
	
	/**
	 * @throws Exception
	 */
	@Test public void test_double() throws Exception
	{
		testx( new double[][] { { -1, 0, 1, Double.MIN_VALUE, DOUBLE_MIN_NORMAL,
			Double.MAX_VALUE, Double.NaN, Double.NEGATIVE_INFINITY,
			Double.POSITIVE_INFINITY, -0.0f, 1.1f,
			3.141592653589793238462643383279 }, { 23 } },
			Validator_double.get( 2 ) );
	}
	
	/**
	 * @throws Exception
	 */
	@Test public void test_String() throws Exception
	{
		testx( new String[][] { { "", "a", "ab", "abc" }, { "23" } },
			Validator_string.get( 2 ) );
	}
	
	/**
	 * @throws Exception
	 */
	@Test public void test_add() throws Exception
	{
		Type add = vf.getType( "add" );
		
		Field x = new Field( "x" );
		add.putValidator( x, Validator_int.get( 0 ) );
		
		Field y = new Field( "y" );
		add.putValidator( y, Validator_int.get( 0 ) );
		
		Field _mf__messageId = DefaultValueFactory._mf__messageId;
		add.putValidator( _mf__messageId, Validator_long.get( 0 ) );
		
		long msgid = 0x0102030405060708L;
		
//		System.out.printf( "add.id() = %04x\n", add.getId() );
//		System.out.printf( "x.id() = %04x\n", x.getId() );
//		System.out.printf( "y.id() = %04x\n", y.getId() );
//		System.out.printf( "_mf__messageId.id() = %04x\n", _mf__messageId.getId() );
//		System.out.printf( "msgid = %08x\n", msgid );
		
		Message msg = new Message( add, vf );
		msg.put( x, 1 );
		msg.put( y, 2 );
		msg.put( _mf__messageId, msgid );
		testmsg2bytes( msg, null,  new byte[] { 3, -122, 39, -23, -73, -100, 3, -122, 21, 10, 44, -77, 1, -122, 99, 6, -76, 104, -121, 1, 2, 3, 4, 5, 6, 7, 8, -122, 21, 10, 44, -76, 2, -127 } );
		testmsg2bytes( msg, false, new byte[] { 3, -122, 39, -23, -73, -100, 3, -122, 21, 10, 44, -77, 1, -122, 99, 6, -76, 104, -121, 1, 2, 3, 4, 5, 6, 7, 8, -122, 21, 10, 44, -76, 2, -127 } );
		testmsg2bytes( msg, true,  new byte[] { 3, -109, 3, 97, 100, 100, 3, -109, 1, 120, 1, -109, 10, 95, 109, 101, 115, 115, 97, 103, 101, 73, 100, -121, 1, 2, 3, 4, 5, 6, 7, 8, -109, 1, 121, 2, -127 } );
		
		msg = new Message( add, vf );
		msg.put( x, 1000000000 );
		msg.put( y, 2000000000 );
		msg.put( _mf__messageId, msgid );
		testmsg2bytes( msg, null,  new byte[] { 3, -122, 39, -23, -73, -100, 3, -122, 21, 10, 44, -77, -122, 59, -102, -54, 0, -122, 99, 6, -76, 104, -121, 1, 2, 3, 4, 5, 6, 7, 8, -122, 21, 10, 44, -76, -122, 119, 53, -108, 0, -127 } );
		testmsg2bytes( msg, false, new byte[] { 3, -122, 39, -23, -73, -100, 3, -122, 21, 10, 44, -77, -122, 59, -102, -54, 0, -122, 99, 6, -76, 104, -121, 1, 2, 3, 4, 5, 6, 7, 8, -122, 21, 10, 44, -76, -122, 119, 53, -108, 0, -127 } );
		testmsg2bytes( msg, true,  new byte[] { 3, -109, 3, 97, 100, 100, 3, -109, 1, 120, -122, 59, -102, -54, 0, -109, 10, 95, 109, 101, 115, 115, 97, 103, 101, 73, 100, -121, 1, 2, 3, 4, 5, 6, 7, 8, -109, 1, 121, -122, 119, 53, -108, 0, -127 } );
	}
	
	private void testmsg2bytes( Message msg, Boolean stringTypeAndField,
		byte[] expected ) throws Exception
	{
		byte[] actual = msg2bytes( msg, stringTypeAndField );
		try
		{
			assertArrayEquals( expected, actual );
		}
		catch ( AssertionError e )
		{
			dump( expected );
			dump( actual );
			throw e;
		}
	}

	/** Tests reading a pre-canned add message.
	 * @throws Exception */
	@Test public void test_add_in() throws Exception
	{
		Type add = vf.getType( "add" );
		
		Field x = new Field( "x" );
		add.putValidator( x, Validator_int.get( 0 ) );
		
		Field y = new Field( "y" );
		add.putValidator( y, Validator_int.get( 0 ) );
		
		Field _mf__messageId = DefaultValueFactory._mf__messageId;
		add.putValidator( _mf__messageId, Validator_long.get( 0 ) );
		
		long msgid = 0x0123456789abcdefL;

		byte[] buf =
		{
			3, // version
			-122, // INT (type)
			39, -23, -73, -100, // add
			3, // length
			-122, // INT (key)
			99, 6, -76, 104,
			-121, // LONG (value)
			1, 35, 69, 103, -119, -85, -51, -17,
			-122, // INT (key)
			21, 10, 44, -76, // y
			2, // tiny int = 2 (value)
			-122, // INT (key)
			21, 10, 44, -77, // x
			1, // tiny int = 1 (value)
			-127 // NONE
		};
		Message msg = bytes2msg( buf );
		msg.checkType( add );
		Assert.assertEquals( 3, msg.size() );
		Assert.assertEquals( 1, msg.get( x ) );
		Assert.assertEquals( 2, msg.get( y ) );
		Assert.assertEquals( msgid, msg.get( _mf__messageId ) );
		
		buf = new byte[]
		{
			3, // version
			-122, // INT (type)
			39, -23, -73, -100, // add
			3, // length
			-122, // INT (key)
			99, 6, -76, 104,
			-121, // LONG (value)
			1, 35, 69, 103, -119, -85, -51, -17,
			-122, // INT (key)
			21, 10, 44, -76, // y
			-122, // INT (value)
			119, 53, -108, 0,
			-122, // INT (key)
			21, 10, 44, -77, // x
			-122, // INT (value)
			59, -102, -54, 0,
			-127 // NONE
		};
		msg = bytes2msg( buf );
		msg.checkType( add );
		Assert.assertEquals( 3, msg.size() );
		Assert.assertEquals( 1000000000, msg.get( x ) );
		Assert.assertEquals( 2000000000, msg.get( y ) );
		Assert.assertEquals( msgid, msg.get( _mf__messageId ) );
	}

	/** Tests add performance.
	 * @throws Exception */
	@Test public void test_add_perf() throws Exception
	{
		Type add = vf.getType( "add" );
		
		Field x = new Field( "x" );
		add.putValidator( x, Validator_int.get( 0 ) );
		
		Field y = new Field( "y" );
		add.putValidator( y, Validator_int.get( 0 ) );
		
		Field _mf__messageId = DefaultValueFactory._mf__messageId;
		add.putValidator( _mf__messageId, Validator_long.get( 0 ) );
		
		long msgid = 0x0123456789abcdefL;
		
//		System.out.printf( "add.id() = %04x\n", add.getId() );
//		System.out.printf( "x.id() = %04x\n", x.getId() );
//		System.out.printf( "y.id() = %04x\n", y.getId() );
//		System.out.printf( "_mf__messageId.id() = %04x\n", _mf__messageId.getId() );
//		System.out.printf( "msgid = %08x\n", msgid );
		
		Message msg = new Message( add, vf );
		msg.put( x, 123456789 );
		msg.put( y, 876543210 );
		msg.put( _mf__messageId, msgid );
//		System.out.println( "msg = "+msg );
		
		Resources res = new Resources();
		res.put( Transport.VALUE_FACTORY, vf );
		MyPacketSource ps = new MyPacketSource();
		Messagizer m = new Messagizer( ps, "foo:?Messagizer.format=binary", res );
		m.transportMessage( null, msg );
		
		int n = 900973;
		for (int i = 0; i < 3; i++)
			testPerf( "test_add_perf", i, m, msg, n );
	}
	
	/** Tests sum performance.
	 * @throws Exception */
	@Test public void test_sum_perf() throws Exception
	{
		Type sum = vf.getType( "sum" );
		
		Field values = new Field( "values" );
		sum.putValidator( values, Validator_int.get( 1 ) );
		
		Field _mf__messageId = DefaultValueFactory._mf__messageId;
		sum.putValidator( _mf__messageId, Validator_long.get( 0 ) );
		
		long msgid = 0x0123456789abcdefL;
		
//		System.out.printf( "add.id() = %04x\n", add.getId() );
//		System.out.printf( "x.id() = %04x\n", x.getId() );
//		System.out.printf( "y.id() = %04x\n", y.getId() );
//		System.out.printf( "_mf__messageId.id() = %04x\n", _mf__messageId.getId() );
//		System.out.printf( "msgid = %08x\n", msgid );
		
		int[] array = new int[2];
		for (int i = 0; i < array.length; i++)
			array[i] = 123456789;
		
		Message msg = new Message( sum, vf );
		msg.put( values, array );
		msg.put( _mf__messageId, msgid );
//		System.out.println( "msg = "+msg );
		
		Resources res = new Resources();
		res.put( Transport.VALUE_FACTORY, vf );
		MyPacketSource ps = new MyPacketSource();
		Messagizer m = new Messagizer( ps, "foo:?Messagizer.format=binary", res );
		m.transportMessage( null, msg );
		
		int n = 509520;
		for (int i = 0; i < 3; i++)
			testPerf( "test_sum_perf", i, m, msg, n );
	}
	
	/** @throws Exception */
	@Test
	public void testValueToBytes() throws Exception
	{
		// nulls
		
		assertValueToBytes( null,
			new byte[] { 3, 1, 0, -127 } );
		
		assertValueToBytes( new Object[] { null },
			new byte[] { 3, 1, 1, 2, -111, -106, 1, 1, -128, -127, -127 } );
		
		// booleans:
		
		assertValueToBytes( false,
			new byte[] { 3, 1, 1, 2, -126, -127 } );
		
		assertValueToBytes( true,
			new byte[] { 3, 1, 1, 2, -125, -127 } );
		
		// tiny ints:
		
		assertValueToBytes( 0,
			new byte[] { 3, 1, 1, 2, 0, -127 } );
		
		assertValueToBytes( 1,
			new byte[] { 3, 1, 1, 2, 1, -127 } );
		
		assertValueToBytes( -1,
			new byte[] { 3, 1, 1, 2, -1, -127 } );
		
		// bytes:
		
		assertValueToBytes( -100,
			new byte[] { 3, 1, 1, 2, -124, -100, -127 } );
		
		// shorts:
		
		assertValueToBytes( 10000,
			new byte[] { 3, 1, 1, 2, -123, 39, 16, -127 } );
		
		assertValueToBytes( -10000,
			new byte[] { 3, 1, 1, 2, -123, -40, -16, -127 } );
		
		// ints:
		
		assertValueToBytes( 1000000000,
			new byte[] { 3, 1, 1, 2, -122, 59, -102, -54, 0, -127 } );
		
		assertValueToBytes( -1000000000,
			new byte[] { 3, 1, 1, 2, -122, -60, 101, 54, 0, -127 } );
		
		// longs:
		
		assertValueToBytes( 1000000000000000000L,
			new byte[] { 3, 1, 1, 2, -121, 13, -32, -74, -77, -89, 100, 0, 0, -127 } );
		
		assertValueToBytes( -1000000000000000000L,
			new byte[] { 3, 1, 1, 2, -121, -14, 31, 73, 76, 88, -100, 0, 0, -127 } );
		
		// float:
		
		assertValueToBytes( 12345f,
			new byte[] { 3, 1, 1, 2, -120, 70, 64, -28, 0, -127 } );
		
		// double:
		
		assertValueToBytes( 23456d,
			new byte[] { 3, 1, 1, 2, -119, 64, -42, -24, 0, 0, 0, 0, 0, -127 } );
		
		// boolean[]:
		
		assertValueToBytes( new boolean[] { true, false },
			new byte[] { 3, 1, 1, 2, -111, -125, 1, 2, -125, -126, -127, -127 } );
		
		// byte[]:
		
		assertValueToBytes( new byte[] { 1, 2, 3 },
			new byte[] { 3, 1, 1, 2, -117, 3, 1, 2, 3, -127 } );

		// byte[][]:
		
		assertValueToBytes( new byte[][] { new byte[] { 1, 2, 3 }, new byte[] { 2, 3, 4 } },
			new byte[] { 3, 1, 1, 2, -111, -124, 2, 2, -117, 3, 1, 2, 3, -117, 3, 2, 3, 4, -127, -127 } );

		// short[]:
		
		assertValueToBytes( new short[] { 1, 2, 3 },
			new byte[] { 3, 1, 1, 2, -111, -123, 1, 3, 1, 2, 3, -127, -127 } );

		// int[]:
		
		assertValueToBytes( new int[] { 1, 2, 3 },
			new byte[] { 3, 1, 1, 2, -111, -122, 1, 3, 1, 2, 3, -127, -127 } );

		// long[]:
		
		assertValueToBytes( new long[] { 1, 2, 3 },
			new byte[] { 3, 1, 1, 2, -111, -121, 1, 3, 1, 2, 3, -127, -127 } );

		// float[]:
		
		assertValueToBytes( new float[] { 1, 2, 3 },
			new byte[] { 3, 1, 1, 2, -111, -120, 1, 3, -120, 63, -128, 0, 0, -120, 64, 0, 0, 0, -120, 64, 64, 0, 0, -127, -127 } );

		// double[]:
		
		assertValueToBytes( new double[] { 1, 2, 3 },
			new byte[] { 3, 1, 1, 2, -111, -119, 1, 3, -119, 63, -16, 0, 0, 0, 0, 0, 0, -119, 64, 0, 0, 0, 0, 0, 0, 0, -119, 64, 8, 0, 0, 0, 0, 0, 0, -127, -127 } );

		// empty string:
		
		assertValueToBytes( "",
			new byte[] { 3, 1, 1, 2, -110, -127 } );
		
		// string:
		
		assertValueToBytes( "abc",
			new byte[] { 3, 1, 1, 2, -109, 3, 97, 98, 99, -127 } );
		
		// string[]:
		
		assertValueToBytes( new String[] { "a", "", "c" },
			new byte[] { 3, 1, 1, 2, -111, -109, 1, 3, -109, 1, 97, -110, -109, 1, 99, -127, -127 } );

		// Date:
		
		assertValueToBytes( new Date( 1234567890L ),
			new byte[] { 3, 1, 1, 2, -107, -122, 43, 57, 107, -52, 1, -122, 102, 0, 26, 64, -122, 73, -106, 2, -46, -127, -127 } );

		// Date[]:
		
		assertValueToBytes( new Date[] { new Date( 1234567890L ), new Date( 2345678901L ) },
			new byte[] { 3, 1, 1, 2, -111, -107, -122, 43, 57, 107, -52, 1, 2, -107, -122, 43, 57, 107, -52, 1, -122, 102, 0, 26, 64, -122, 73, -106, 2, -46, -127, -107, -122, 43, 57, 107, -52, 1, -122, 102, 0, 26, 64, -121, 0, 0, 0, 0, -117, -48, 56, 53, -127, -127, -127 } );
	}
	
	/** @throws Exception */
	@Test
	public void badtype() throws Exception
	{
		@SuppressWarnings("unused")
		Message msg = bytes2msg( new byte[] { 3, 1, 0, -127 } );
//		System.out.println( "msg = "+msg );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void badmsglen1() throws Exception
	{
		@SuppressWarnings("unused")
		Message msg = bytes2msg( new byte[] { 3, 1, -1, -127 } );
//		System.out.println( "msg = "+msg );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void badmsglen2() throws Exception
	{
		@SuppressWarnings("unused")
		Message msg = bytes2msg( new byte[] { 3, 1, 99, -127 } );
//		System.out.println( "msg = "+msg );
	}
	
	/** @throws Exception */
	@Test
	public void badfield() throws Exception
	{
		@SuppressWarnings("unused")
		Message msg = bytes2msg( new byte[] { 3, 1, 1, 2, 2, -127 }, Level.MISSING_OK );
//		System.out.println( "msg = "+msg );
	}
	
	private void assertValueToBytes( Object value, byte[] expectedBytes ) throws Exception
	{
		Type t = new Type( 1, "a" );
		Field f = new Field( 2, "b" );
		t.putValidator( f, Validator_object.get( 0 ) );
		
		Message msg = new Message( t, vf );
		msg.put( f, value );
//		System.out.println( "msg = "+msg );
		
		BinaryTaggedDataOutput btdo = new BinaryTaggedDataOutput( vf, "none:" );
		FlexBuffer buf = new FlexBuffer();
		
		btdo.writeMessage( msg, buf );
		
		buf.setIndex( 0 );
		byte[] b = buf.getAvailBytes();
//		dump( b );
		assertArrayEquals( expectedBytes, b );
	}
	
	private final static void testPerf( String name, int iter, Messagizer m,
		Message msg, int n ) throws Exception
	{
		long t0 = System.nanoTime();
		
		for (int i = 0; i < n; i++)
			m.transportMessage( null, msg );
		
		long t1 = System.nanoTime();
		double t = (t1-t0) / 1000000000.0;
		double r = n / t;
		Log.report( name, "iteration", iter, "time", t, "count", n, "rate", r );
	}
	
	/**
	 * Test jig for perf tests.
	 */
	public static class MyPacketSource implements TransportPacket
	{
		public int headerSize()
        {
	        return 0;
        }

		public void transportPacket( Who recipient, FlexBuffer buf ) throws Exception
        {
	        // ignore.
//			System.out.println( buf.avail() );
        }

		public SessionPacket getSession()
		{
			// ignore
			return null;
		}

		public void setSession( SessionPacket session )
		{
			// ignore
		}

		public void transportControl( Object control, Object value )
			throws Exception
		{
			// ignore
		}

		public void transportNotify( Object event ) throws Exception
		{
			// ignore
		}

		public Object transportQuery( Object query ) throws Exception
		{
			// ignore
			return null;
		}
	}
	
	private byte[] msg2bytes( Message msg, Boolean stringTypeAndField ) throws IOException
	{
		FlexBuffer buf = new FlexBuffer();
		
		URL u = new URL( "none:" );
		if (stringTypeAndField != null)
			u.addTerm( BinaryTaggedDataOutput.STRING_TYPE_AND_FIELD, stringTypeAndField );
		
		BinaryTaggedDataOutput btdo = new BinaryTaggedDataOutput( vf, u.toString() );
		btdo.writeMessage( msg, buf );
		buf.setIndex( 0 );
		return buf.getAvailBytes();
	}
	
	private Message bytes2msg( byte[] buf ) throws Exception
	{
		return bytes2msg( buf, Level.FULL );
	}
	
	private Message bytes2msg( byte[] buf, Level level ) throws Exception
	{
		Level oldLevel = vf.setLevel( level );
		try
		{
			BinaryTaggedDataInput btdi = new BinaryTaggedDataInput( vf, "none:" );
			return btdi.readMessage( new FlexBuffer( buf ) );
		}
		finally
		{
			vf.setLevel( oldLevel );
		}
	}
	
	private void testx( Object x, Validator v )
		throws Exception
	{
		Class<?> c = x.getClass();
		if (c.isArray())
		{
			int n = Array.getLength( x );
			for (int i = 0; i < n; i++)
			{
				testx( Array.get( x, i ), v.elementValidator() );
			}
		}
		test( x, v, null );
		test( x, v, false );
		test( x, v, true );
	}
	
	/**
	 * value factory for tests. it is ok that it is shared between tests.
	 */
	
	final DefaultValueFactory vf = new MyValueFactory( "tcp:" );
	
	private final Type mt_foo = vf.getType( "foo" );
	
	private final Field mf_x = new Field( "x" );
	
	private void test( Object x, Validator v, Boolean stringTypeAndField ) throws Exception
	{
//		System.out.println( "-----------------------------------------" );
		
		mt_foo.clearValidator( mf_x );
		mt_foo.putValidator( mf_x, v );
		
		Message msg = new Message( mt_foo, vf );
		msg.put( mf_x, x );
//		System.out.println( "msg = "+msg );
		
		byte[] bufx = msg2bytes( msg, stringTypeAndField );
//		dump( bufx );
		Message msg2 = bytes2msg( bufx );
		
//		System.out.println( "msg2 = "+msg2 );
		msg2.checkType( mt_foo );
		Assert.assertEquals( 1, msg2.size() );
		Assert.assertTrue( msg.containsKey( mf_x ) );
		Object y = msg2.get( mf_x );
		
		assertEquals( x, y );
	}
	
	@SuppressWarnings("unused")
	private void dump( byte[] buf )
	{
		System.out.print( "byte[/*"+buf.length+"*/] buf = {" );
		for (int i = 0; i < buf.length; i++)
		{
			if (i > 0)
				System.out.print( ", " );
			else
				System.out.print( " " );
			System.out.printf( "%d", buf[i] );
		}
		System.out.println( " }" );
	}

	private void assertEquals( Object a, Object b )
	{
		if (a == null || b == null)
		{
			Assert.assertEquals( a, b );
			return;
		}
		
		Class<?> aClass = a.getClass();
		Class<?> bClass = b.getClass();
		
		if (aClass == bClass)
		{
			if (aClass.isArray() && bClass.isArray())
				assertArrayEquals( a, b );
			else
				Assert.assertEquals( a, b );
		}
		//else if (a instanceof Number && b instanceof Number)
		else
		{
//			System.out.printf( "a %s, b %s\n", aClass, bClass );
			Assert.assertEquals( a, b );
		}
	}

	private void assertArrayEquals( Object a, Object b )
	{
		int an = Array.getLength( a );
		int bn = Array.getLength( b );
		
		assertEquals( an, bn );
		
		for (int i = 0; i < an; i++)
			assertEquals( Array.get( a, i ), Array.get( b, i ) );
	}
	
	/** */
	static class MyValueFactory extends DefaultValueFactory
	{
		private final static TypeMap types = new TypeMap();
		
		private final static Class2TypeMap class2type = new Class2TypeMap();

		static
		{
			DefaultValueFactory.init( types, class2type );
		}

		/**
		 * Constructs the MyValueFactory.
		 * @param uri
		 */
		public MyValueFactory( String uri )
		{
			super( uri, types, class2type );
		}
	}

}
