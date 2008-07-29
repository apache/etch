/* $Id$
 *
 * Created by wert on Jun 12, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.compiler.test;

import etch.bindings.java.util.StrIntHashMap;

/**
 * Silly implementation of Test used for testing.
 */
public class FakeTest implements Test
{
	public Integer beets( E1 e ) throws Excp3, Excp4
	{
		if (e == null)
			return null;
		
		switch (e)
		{
			case A: return 5;
			case B: throw new Excp3();
			case C: throw new Excp4();
			default: return null;
		}
	}

	public void blow( String msg, Integer code ) throws Excp1
	{
		throw new Excp1( msg, code );
	}

	public Double dist( S1 a, S1 b )
	{
		S1 c = new S1( a.x-b.x, a.y-b.y, a.z-b.z );
		return Math.sqrt( c.x*c.x + c.y*c.y + c.z*c.z );
	}

	public Integer incr( Integer x )
	{
		return x+1;
	}

	public void nothing()
	{
		// nothing.
	}

	public Integer sub( Integer x, Integer y )
	{
		return x-y;
	}

	public Integer sum( int[] x )
	{
		int sum = 0;
		for (int i: x)
			sum += i;
		return sum;
	}

	public Integer trans( E1 e, Integer x )
	{
		switch (e)
		{
			case A: return x / 2;
			case B: return x * 2;
			case C: return x + 7;
			default: return null;
		}
	}

	public int[] fill( Integer n, Integer x )
	{
		int[] y = new int[n];
		for (int i = 0; i < n; i++)
			y[i] = x;
		return y;
	}

	// Fill numElements of an array with value o
	public Object[] fillObject( Integer numElements, Object o )
	{
		Object [] y = new Object[numElements];
		for (int i = 0; i < numElements; i++)
			y[i] = o;
		return y;
	}
	
	public E1 p_E1( E1 a )
	{
		return a;
	}

	public E1[] p_E1_array( E1[] a )
	{
		return a;
	}

	public S1 p_S1( S1 a )
	{
		return a;
	}

	public S1[] p_S1_array( S1[] a )
	{
		return a;
	}

	public S2 p_S2( S2 a )
	{
		return a;
	}

	public S2[] p_S2_array( S2[] a )
	{
		return a;
	}

	public Boolean p_boolean( Boolean a )
	{
		return a;
	}

	public boolean[] p_boolean_array( boolean[] a )
	{
		return a;
	}

	public Byte p_byte( Byte a )
	{
		return a;
	}

	public byte[] p_byte_array( byte[] a )
	{
		return a;
	}

	public Double p_double( Double a )
	{
		return a;
	}

	public double[] p_double_array( double[] a )
	{
		return a;
	}

	public Float p_float( Float a )
	{
		return a;
	}

	public float[] p_float_array( float[] a )
	{
		return a;
	}

	public Integer p_int( Integer a )
	{
		return a;
	}

	public int[] p_int_array( int[] a )
	{
		return a;
	}

	public Long p_long( Long a )
	{
		return a;
	}

	public long[] p_long_array( long[] a )
	{
		return a;
	}

	public Short p_short( Short a )
	{
		return a;
	}

	public short[] p_short_array( short[] a )
	{
		return a;
	}

	public String p_string( String a )
	{
		return a;
	}

	public String[] p_string_array( String[] a )
	{
		return a;
	}

	public StrIntHashMap p_Blob( StrIntHashMap a )
	{
		return a;
	}

	public StrIntHashMap[] p_Blob_array( StrIntHashMap[] a )
	{
		return a;
	}

	public void alwaysFails()
	{
		// i'm never called.
		throw new UnsupportedOperationException( "alwaysFails" );
	}

	public Integer alwaysWorks()
	{
		// i'm always called.
		return 23;
	}

	public Boolean isFalse()
	{
		return false;
	}

	public Boolean isTrue()
	{
		return true;
	}

	public Integer add( Integer a, Integer b )
	{
		return a + b;
	}

	public Boolean canAdd( Boolean flag, Integer x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	public Boolean can_m1()
	{
		// TODO Auto-generated method stub
		return null;
	}

	public Boolean can_m10( Double x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	public Boolean can_m2( Boolean x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	public Boolean can_m3( Boolean x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	public Boolean can_m4( Boolean x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	public Boolean can_m7( Integer x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	public void m1()
	{
		// TODO Auto-generated method stub
		
	}

	public void m10()
	{
		// TODO Auto-generated method stub
		
	}

	public void m2()
	{
		// TODO Auto-generated method stub
		
	}

	public void m3()
	{
		// TODO Auto-generated method stub
		
	}

	public void m4()
	{
		// TODO Auto-generated method stub
		
	}

	public void m7()
	{
		// TODO Auto-generated method stub
		
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#canM1()
	 */
	public Boolean canM1()
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#canM10(java.lang.Double)
	 */
	public Boolean canM10( Double x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#canM11(java.lang.String)
	 */
	public Boolean canM11( String x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#canM2(java.lang.Boolean)
	 */
	public Boolean canM2( Boolean x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#canM3(java.lang.Boolean)
	 */
	public Boolean canM3( Boolean x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#canM4(java.lang.Boolean)
	 */
	public Boolean canM4( Boolean x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#canM5(java.lang.Byte)
	 */
	public Boolean canM5( Byte x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#canM6(java.lang.Short)
	 */
	public Boolean canM6( Short x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#canM7(java.lang.Integer)
	 */
	public Boolean canM7( Integer x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#canM8(java.lang.Long)
	 */
	public Boolean canM8( Long x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#canM9(java.lang.Float)
	 */
	public Boolean canM9( Float x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#m11()
	 */
	public void m11()
	{
		// TODO Auto-generated method stub
		
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#m5()
	 */
	public void m5()
	{
		// TODO Auto-generated method stub
		
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#m6()
	 */
	public void m6()
	{
		// TODO Auto-generated method stub
		
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#m8()
	 */
	public void m8()
	{
		// TODO Auto-generated method stub
		
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#m9()
	 */
	public void m9()
	{
		// TODO Auto-generated method stub
		
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#can_m11(java.lang.String)
	 */
	public Boolean can_m11( String x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#can_m5(java.lang.Byte)
	 */
	public Boolean can_m5( Byte x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#can_m6(java.lang.Short)
	 */
	public Boolean can_m6( Short x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#can_m8(java.lang.Long)
	 */
	public Boolean can_m8( Long x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#can_m9(java.lang.Float)
	 */
	public Boolean can_m9( Float x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#can_m12(java.lang.Integer)
	 */
	public Boolean can_m12( Integer x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#can_m13(java.lang.Integer)
	 */
	public Boolean can_m13( Integer x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#can_m14(java.lang.Integer)
	 */
	public Boolean can_m14( Integer x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#can_m15(etch.bindings.java.compiler.test.Test.E1)
	 */
	public Boolean can_m15( E1 x )
	{
		// TODO Auto-generated method stub
		return null;
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#m12()
	 */
	public void m12()
	{
		// TODO Auto-generated method stub
		
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#m13(java.lang.Integer)
	 */
	public void m13( Integer k )
	{
		// TODO Auto-generated method stub
		
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#m14(etch.bindings.java.compiler.test.Test.S1)
	 */
	public void m14( S1 k )
	{
		// TODO Auto-generated method stub
		
	}

	/* (non-Javadoc)
	 * @see etch.bindings.java.compiler.test.Test#m15()
	 */
	public void m15()
	{
		// TODO Auto-generated method stub
		
	}

	public Object p_object(Object a) {
		// TODO Auto-generated method stub
		return a;
	}

	public Object[] p_object_array(Object[] a) {
		// TODO Auto-generated method stub
		return a;
	}

	public S3 p_object_struct(S3 a) {
		// TODO Auto-generated method stub
		return a;
	}

	public S4 p_object_struct_array(S4 a) {
		// TODO Auto-generated method stub
		return a;
	}

	public void throwExcp5(String msg, Integer code, Object value) throws Excp5 {
		// TODO Auto-generated method stub
		throw new Excp5( msg, code, value );
	}

	public void throwExcp6(String msg, Integer code, Object[] value) throws Excp6 {
		// TODO Auto-generated method stub
		throw new Excp6( msg, code, value );		
	}

}