/* $Id$
 *
 * Created by sccomer on Jan 17, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport.fmt.binary.test;

import java.io.IOException;
import java.lang.reflect.Array;
import java.util.Date;

import metreos.core.Who;
import metreos.core.io.PacketHandler;
import metreos.core.io.PacketSource;
import metreos.util.FlexBuffer;
import metreos.util.Log;

import org.junit.Assert;
import org.junit.Test;

import etch.bindings.java.msg.ArrayValue;
import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator;
import etch.bindings.java.msg.Validator_boolean;
import etch.bindings.java.msg.Validator_byte;
import etch.bindings.java.msg.Validator_double;
import etch.bindings.java.msg.Validator_float;
import etch.bindings.java.msg.Validator_int;
import etch.bindings.java.msg.Validator_long;
import etch.bindings.java.msg.Validator_short;
import etch.bindings.java.msg.Validator_string;
import etch.bindings.java.support.DefaultValueFactory;
import etch.bindings.java.transport.Messagizer;
import etch.bindings.java.transport.fmt.binary.BinaryTaggedData;
import etch.bindings.java.transport.fmt.binary.BinaryTaggedDataInput;
import etch.bindings.java.transport.fmt.binary.BinaryTaggedDataOutput;

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
		
		BinaryTaggedDataOutput btdo = new BinaryTaggedDataOutput( (FlexBuffer) null, null );
		
		// positive small bytes:
		Assert.assertEquals( (byte) 0, btdo.checkValue( (byte) 0 ) );
		Assert.assertEquals( (byte) 1, btdo.checkValue( (byte) 1 ) );
		Assert.assertEquals( (byte) 31, btdo.checkValue( (byte) 31 ) );
		
		// positive small shorts:
		Assert.assertEquals( (byte) 32, btdo.checkValue( (short) 0 ) );
		Assert.assertEquals( (byte) 33, btdo.checkValue( (short) 1 ) );
		Assert.assertEquals( (byte) 63, btdo.checkValue( (short) 31 ) );
		
		// positive small ints:
		Assert.assertEquals( (byte) 64, btdo.checkValue( 0 ) );
		Assert.assertEquals( (byte) 65, btdo.checkValue( 1 ) );
		Assert.assertEquals( (byte) 95, btdo.checkValue( 31 ) );
		
		// positive small longs:
		Assert.assertEquals( (byte) 96, btdo.checkValue( 0L ) );
		Assert.assertEquals( (byte) 97, btdo.checkValue( 1L ) );
		Assert.assertEquals( (byte) 127, btdo.checkValue( 31L ) );
		
		// negative small bytes:
		Assert.assertEquals( (byte) -128, btdo.checkValue( (byte) -1 ) );
		Assert.assertEquals( (byte) -127, btdo.checkValue( (byte) -2 ) );
		Assert.assertEquals( (byte) -113, btdo.checkValue( (byte) -16 ) );
		
		// negative small shorts:
		Assert.assertEquals( (byte) -112, btdo.checkValue( (short) -1 ) );
		Assert.assertEquals( (byte) -111, btdo.checkValue( (short) -2 ) );
		Assert.assertEquals( (byte) -97, btdo.checkValue( (short) -16 ) );
		
		// negative small ints:
		Assert.assertEquals( (byte) -96, btdo.checkValue( -1 ) );
		Assert.assertEquals( (byte) -95, btdo.checkValue( -2 ) );
		Assert.assertEquals( (byte) -81, btdo.checkValue( -16 ) );
		
		// negative small longs:
		Assert.assertEquals( (byte) -80, btdo.checkValue( -1L ) );
		Assert.assertEquals( (byte) -79, btdo.checkValue( -2L ) );
		Assert.assertEquals( (byte) -65, btdo.checkValue( -16L ) );
		
		// null value
		Assert.assertEquals( (byte) -1, btdo.checkValue( null ) );
		
		// boolean values
		Assert.assertEquals( (byte) -2, btdo.checkValue( false ) );
		Assert.assertEquals( (byte) -3, btdo.checkValue( true ) );
		
		// normal bytes
		Assert.assertEquals( (byte) -4, btdo.checkValue( (byte) -45 ) );
		Assert.assertEquals( (byte) -4, btdo.checkValue( (byte) 45 ) );
		
		// normal shorts
		Assert.assertEquals( (byte) -5, btdo.checkValue( (short) -45 ) );
		Assert.assertEquals( (byte) -5, btdo.checkValue( (short) 45 ) );

		Assert.assertEquals( (byte) -6, btdo.checkValue( (short) -4545 ) );
		Assert.assertEquals( (byte) -6, btdo.checkValue( (short) 4545 ) );
		
		// normal ints
		Assert.assertEquals( (byte) -7, btdo.checkValue( -45 ) );
		Assert.assertEquals( (byte) -7, btdo.checkValue( 45 ) );

		Assert.assertEquals( (byte) -8, btdo.checkValue( -4545 ) );
		Assert.assertEquals( (byte) -8, btdo.checkValue( 4545 ) );
		
		Assert.assertEquals( (byte) -9, btdo.checkValue( -45454545 ) );
		Assert.assertEquals( (byte) -9, btdo.checkValue( 45454545 ) );

		// normal longs
		Assert.assertEquals( (byte) -10, btdo.checkValue( -45L ) );
		Assert.assertEquals( (byte) -10, btdo.checkValue( 45L ) );

		Assert.assertEquals( (byte) -11, btdo.checkValue( -4545L ) );
		Assert.assertEquals( (byte) -11, btdo.checkValue( 4545L ) );
		
		Assert.assertEquals( (byte) -12, btdo.checkValue( -45454545L ) );
		Assert.assertEquals( (byte) -12, btdo.checkValue( 45454545L ) );

		Assert.assertEquals( (byte) -13, btdo.checkValue( -4545454545454545L ) );
		Assert.assertEquals( (byte) -13, btdo.checkValue( 4545454545454545L ) );
		
		// float
		Assert.assertEquals( (byte) -14, btdo.checkValue( 3.14159f ) );
		
		// double
		Assert.assertEquals( (byte) -15, btdo.checkValue( 3.14159 ) );
		
		// byte array
		Assert.assertEquals( (byte) -16, btdo.checkValue( new byte[0] ) );
		Assert.assertEquals( (byte) -16, btdo.checkValue( new byte[1] ) );
		
		// string
		Assert.assertEquals( (byte) -17, btdo.checkValue( "" ) );
		Assert.assertEquals( (byte) -18, btdo.checkValue( "abc" ) );
		
		// struct
		Assert.assertEquals( (byte) -19, btdo.checkValue( new StructValue( new Type( "foo" ) ) ) );
		
		// array
		Assert.assertEquals( (byte) -20, btdo.checkValue( new ArrayValue() ) );
		
		// custom
		Assert.assertEquals( (byte) -21, btdo.checkValue( new Date() ) );
		
		// none
		Assert.assertEquals( (byte) -22, btdo.checkValue( BinaryTaggedData.NONE ) );
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
		
		Field x = vf.getField( "x" );
		add.putValidator( x, Validator_int.get( 0 ) );
		
		Field y = vf.getField( "y" );
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
		msg.put( x, 1 );
		msg.put( y, 2 );
		msg.put( _mf__messageId, msgid );
//		System.out.println( "msg = "+msg );
		/* byte[] buf = */ msg2bytes( msg );
//		dump( buf );
//		buf = null;
		
		msg = new Message( add, vf );
		msg.put( x, 1000000000 );
		msg.put( y, 2000000000 );
		msg.put( _mf__messageId, msgid );
//		System.out.println( "msg = "+msg );
		/* buf = */ msg2bytes( msg );
//		dump( buf );
//		buf = null;
	}
	
	/** Tests reading a pre-canned add message. 
	 * @throws Exception */
	@Test public void test_add_in() throws Exception
	{
		Type add = vf.getType( "add" );
		
		Field x = vf.getField( "x" );
		add.putValidator( x, Validator_int.get( 0 ) );
		
		Field y = vf.getField( "y" );
		add.putValidator( y, Validator_int.get( 0 ) );
		
		Field _mf__messageId = DefaultValueFactory._mf__messageId;
		add.putValidator( _mf__messageId, Validator_long.get( 0 ) );
		
		long msgid = 0x0123456789abcdefL;

		byte[] buf =
		{
			1, // version
			-9, // INT4
			39, -23, -73, -100, // add
			-9, // INT4
			99, 6, -76, 104,
			-13, // LONG8
			1, 35, 69, 103, -119, -85, -51, -17,
			-9, // INT4
			21, 10, 44, -76, // y
			66, // INT4=2
			-9, // INT4
			21, 10, 44, -77, // x
			65, // INT4=1
			-22 // NONE
		};
		Message msg = bytes2msg( buf );
		msg.checkType( add );
		Assert.assertEquals( 3, msg.size() );
		Assert.assertEquals( 1, msg.get( x ) );
		Assert.assertEquals( 2, msg.get( y ) );
		Assert.assertEquals( msgid, msg.get( _mf__messageId ) );
		
		buf = new byte[] { 1, -9, 39, -23, -73, -100, -9, 99, 6, -76, 104, -13, 1, 35, 69, 103, -119, -85, -51, -17, -9, 21, 10, 44, -76, -9, 119, 53, -108, 0, -9, 21, 10, 44, -77, -9, 59, -102, -54, 0, -22 };
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
		
		Field x = vf.getField( "x" );
		add.putValidator( x, Validator_int.get( 0 ) );
		
		Field y = vf.getField( "y" );
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
		
		Messagizer m = new Messagizer( null, vf );
		MyPacketSource ps = new MyPacketSource();
		m.setSource( ps );
		m.messagex( null, msg );
		
		int n = 900973;
		for (int i = 0; i < 3; i++)
			testPerf( "test_add_perf", i, m, msg, n );
	}
	
	/** Tests sum performance.
	 * @throws Exception */
	@Test public void test_sum_perf() throws Exception
	{
		Type sum = vf.getType( "sum" );
		
		Field values = vf.getField( "values" );
		sum.putValidator( values, Validator_int.get( 1 ) );
		
		Field _mf__messageId = DefaultValueFactory._mf__messageId;
		sum.putValidator( _mf__messageId, Validator_long.get( 0 ) );
		
		long msgid = 0x0123456789abcdefL;
		
//		System.out.printf( "add.id() = %04x\n", add.getId() );
//		System.out.printf( "x.id() = %04x\n", x.getId() );
//		System.out.printf( "y.id() = %04x\n", y.getId() );
//		System.out.printf( "_mf__messageId.id() = %04x\n", _mf__messageId.getId() );
//		System.out.printf( "msgid = %08x\n", msgid );
		
		int[] array = new int[1000];
		for (int i = 0; i < 1000; i++)
			array[i] = 123456789;
		
		Message msg = new Message( sum, vf );
		msg.put( values, array );
		msg.put( _mf__messageId, msgid );
//		System.out.println( "msg = "+msg );
		
		Messagizer m = new Messagizer( null, vf );
		MyPacketSource ps = new MyPacketSource();
		m.setSource( ps );
		m.messagex( null, msg );
		
		int n = 2934;
		for (int i = 0; i < 3; i++)
			testPerf( "test_sum_perf", i, m, msg, n );
	}
	
	private final static void testPerf( String name, int iter, Messagizer m,
		Message msg, int n ) throws Exception
	{
		long t0 = System.nanoTime();
		
		for (int i = 0; i < n; i++)
			m.messagex( null, msg );
		
		long t1 = System.nanoTime();
		double t = (t1-t0) / 1000000000.0;
		double r = n / t;
		Log.report( name, "iteration", iter, "time", t, "count", n, "rate", r );
	}
	
	/**
	 * Test jig for perf tests.
	 */
	public static class MyPacketSource implements PacketSource
	{
		public int headerSize()
        {
	        return 0;
        }

		public void packet( Who recipient, FlexBuffer buf ) throws Exception
        {
	        // ignore.
//			System.out.println( buf.avail() );
        }

		public PacketHandler getHandler()
        {
	        // ignore.
	        return null;
        }

		public void setHandler( PacketHandler handler )
        {
	        // ignore.
        }

		public void transportControl( Object control, Object value ) throws Exception
		{
			// ignore.
		}

		public void transportNotify( Object event ) throws Exception
		{
			// ignore.
		}

		public Object transportQuery( Object query ) throws Exception
		{
			// ignore.
			return null;
		}
	}
	
	private byte[] msg2bytes( Message msg ) throws IOException
	{
		FlexBuffer buf = new FlexBuffer();
		BinaryTaggedDataOutput btdo = new BinaryTaggedDataOutput( buf, vf );
		msg.writeMessage( btdo );
		buf.setIndex( 0 );
		return buf.getAvailBytes();
	}
	
	private Message bytes2msg( byte[] buf ) throws Exception
	{
		return BinaryTaggedDataInput.readMessage( vf, new FlexBuffer( buf ), null );
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
		Object y = test( x, v );
		assertEquals( x, y );
	}
	
	/**
	 * value factory for tests. it is ok that it is shared between tests.
	 */
	final DefaultValueFactory vf = new DefaultValueFactory();
	
	private final Type mt_foo = vf.getType( "foo" );
	
	private final Field mf_x = vf.getField( "x" );

	private Object test( Object x, Validator v ) throws Exception
	{
//		System.out.println( "-----------------------------------------" );
		
		mt_foo.clearValidator( mf_x );
		mt_foo.putValidator( mf_x, v );
		
		Message msg = new Message( mt_foo, vf );
		msg.put( mf_x, x );
//		System.out.println( "msg = "+msg );
		
		byte[] bufx = msg2bytes( msg );
//		dump( bufx );
		Message msg2 = bytes2msg( bufx );
		
//		System.out.println( "msg2 = "+msg2 );
		msg2.checkType( mt_foo );
		Assert.assertEquals( 1, msg2.size() );
		msg.containsKey( mf_x );
		return msg2.get( mf_x );
	}
	
//	private void dump( byte[] buf )
//	{
//		System.out.print( "byte["+buf.length+"] buf = {" );
//		for (int i = 0; i < buf.length; i++)
//		{
//			if (i > 0)
//				System.out.print( ", " );
//			else
//				System.out.print( " " );
//			System.out.printf( "%d", buf[i] );
//		}
//		System.out.println( " }" );
//	}

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
}
