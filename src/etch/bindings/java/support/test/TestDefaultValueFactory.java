package etch.bindings.java.support.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNotSame;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Date;
import java.util.List;

import org.junit.Test;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator_long;
import etch.bindings.java.support.DefaultValueFactory;
import etch.bindings.java.support._Etch_RuntimeException;

public class TestDefaultValueFactory
{
	private DefaultValueFactory vf = new DefaultValueFactory( 3 );

	//////////
	// TYPE //
	//////////
	
	@Test
	public void addType() throws Exception
	{
		///////
		// a //
		///////
		
		Type a = vf.addType( new Type( "a" ) );
		assertNotNull( a );
		
		Type x = vf.getType( "a" );
		assertNotNull( x );
		assertSame( a, x );
		
		x = vf.addType( new Type( "a" ) );
		assertNotNull( x );
		assertSame( a, x );

		///////
		// b //
		///////
		
		Type b = vf.addType( new Type( "b" ) );
		assertNotNull( b );
		assertNotSame( a, b );
		
		Type y = vf.getType( "b" );
		assertNotNull( y );
		assertSame( b, y );
		
		y = vf.addType( new Type( "b" ) );
		assertNotNull( y );
		assertSame( b, y );

		///////
		// c //
		///////
		
		Type c = vf.addType( new Type( "c" ) );
		assertNotNull( c );
		assertNotSame( a, c );
		assertNotSame( b, c );
		
		Type z = vf.getType( "c" );
		assertNotNull( z );
		assertSame( c, z );
		
		z = vf.addType( new Type( "c" ) );
		assertNotNull( z );
		assertSame( c, z );
	}
	
	@Test
	public void getType_id_Etch_RuntimeException() throws Exception
	{
		Type x = vf.getType( -284614264 );
		assertNotNull( x );
		assertSame( DefaultValueFactory._mt__Etch_RuntimeException, x );
	}
	
	@Test
	public void getType_name_Etch_RuntimeException() throws Exception
	{
		Type x = vf.getType( "_Etch_RuntimeException" );
		assertNotNull( x );
		assertSame( DefaultValueFactory._mt__Etch_RuntimeException, x );
	}
	
	@Test
	public void getType_auto_just_enough() throws Exception
	{
		vf.getType( "a" );
		vf.getType( "b" );
		vf.getType( "c" );
	}
	
	@Test( expected=IllegalArgumentException.class )
	public void getType_auto_too_many() throws Exception
	{
		vf.getType( "a" );
		vf.getType( "b" );
		vf.getType( "c" );
		vf.getType( "d" );
	}
	
	@Test
	public void getTypes() throws Exception
	{
		List<Type> list = new ArrayList<Type>();
		list.add( vf.getType( "a" ) );
		list.add( vf.getType( "b" ) );
		list.add( vf.getType( "c" ) );
		list.add( DefaultValueFactory._mt__Etch_RuntimeException );
		list.add( DefaultValueFactory._mt__Etch_AuthException );
		list.add( DefaultValueFactory._mt__Etch_Date );
		list.add( DefaultValueFactory._mt__exception );
		
		Collection<Type> c = vf.getTypes();
		assertTrue( c.containsAll( list ) );
		assertTrue( list.containsAll( c ) );
	}
	
	///////////
	// FIELD //
	///////////
	
	@Test
	public void addField() throws Exception
	{
		///////
		// a //
		///////
		
		Field a = vf.addField( new Field( "a" ) );
		assertNotNull( a );
		
		Field x = vf.getField( "a" );
		assertNotNull( x );
		assertSame( a, x );
		
		x = vf.addField( new Field( "a" ) );
		assertNotNull( x );
		assertSame( a, x );

		///////
		// b //
		///////
		
		Field b = vf.addField( new Field( "b" ) );
		assertNotNull( b );
		assertNotSame( a, b );
		
		Field y = vf.getField( "b" );
		assertNotNull( y );
		assertSame( b, y );
		
		y = vf.addField( new Field( "b" ) );
		assertNotNull( y );
		assertSame( b, y );

		///////
		// c //
		///////
		
		Field c = vf.addField( new Field( "c" ) );
		assertNotNull( c );
		assertNotSame( a, c );
		assertNotSame( b, c );
		
		Field z = vf.getField( "c" );
		assertNotNull( z );
		assertSame( c, z );
		
		z = vf.addField( new Field( "c" ) );
		assertNotNull( z );
		assertSame( c, z );
	}
	
	@Test
	public void getField_id_messageId() throws Exception
	{
		Field x = vf.getField( 1661383784 );
		assertNotNull( x );
		assertSame( DefaultValueFactory._mf__messageId, x );
	}
	
	@Test
	public void getField_id_inReplyTo() throws Exception
	{
		Field x = vf.getField( -307705434 );
		assertNotNull( x );
		assertSame( DefaultValueFactory._mf__inReplyTo, x );
	}
	
	@Test
	public void getField_id_msg() throws Exception
	{
		Field x = vf.getField( 769750364 );
		assertNotNull( x );
		assertSame( DefaultValueFactory._mf_msg, x );
	}
	
	@Test
	public void getField_name_messageId() throws Exception
	{
		Field x = vf.getField( "_messageId" );
		assertNotNull( x );
		assertSame( DefaultValueFactory._mf__messageId, x );
	}
	
	@Test
	public void getField_name_inReplyTo() throws Exception
	{
		Field x = vf.getField( "_inReplyTo" );
		assertNotNull( x );
		assertSame( DefaultValueFactory._mf__inReplyTo, x );
	}
	
	@Test
	public void getField_name_msg() throws Exception
	{
		Field x = vf.getField( "msg" );
		assertNotNull( x );
		assertSame( DefaultValueFactory._mf_msg, x );
	}
	
	@Test
	public void getField_auto_just_enough() throws Exception
	{
		vf.getField( "a" );
		vf.getField( "b" );
		vf.getField( "c" );
	}
	
	@Test( expected=IllegalArgumentException.class )
	public void getField_auto_too_many() throws Exception
	{
		vf.getField( "a" );
		vf.getField( "b" );
		vf.getField( "c" );
		vf.getField( "d" );
	}
	
	@Test
	public void getFields() throws Exception
	{
		List<Field> list = new ArrayList<Field>();
		list.add( vf.getField( "a" ) );
		list.add( vf.getField( "b" ) );
		list.add( vf.getField( "c" ) );
		list.add( DefaultValueFactory._mf_msg );
		list.add( DefaultValueFactory._mf_ticks );
		list.add( DefaultValueFactory._mf__messageId );
		list.add( DefaultValueFactory._mf__inReplyTo );
		list.add( DefaultValueFactory._mf_result );
		
		Collection<Field> c = vf.getFields();
		assertTrue( c.containsAll( list ) );
		assertTrue( list.containsAll( c ) );
	}

	////////////////////
	// TYPE AND FIELD //
	////////////////////
	
	@Test
	public void getTypeAndFieldAreDistinct() throws Exception
	{
		// verify that the type and field maps are distinct.

		Type mta = vf.getType( "a" );
		assertNotNull( mta );
		
		Field mfa = vf.getField( "a" );
		assertNotNull( mfa );
		
		assertNotSame( mta, mfa );
	}

	/////////////////////
	// STRING ENCODING //
	/////////////////////
	
	@Test
	public void getStringEncoding() throws Exception
	{
		String enc = vf.getStringEncoding();
		String s = new String( new byte[] { 'a', 'b', 'c' }, enc );
		assertEquals( "abc", s );
	}

	////////////////
	// MESSAGE ID //
	////////////////
	
	@Test
	public void messageId() throws Exception
	{
		Type mt_foo = new Type( "foo" );
		mt_foo.putValidator( vf._mf__messageId, Validator_long.get( 0 ) );
		
		Message msg = new Message( mt_foo, vf );
		
		assertNull( vf.getMessageId( msg ) );
		
		vf.setMessageId( msg, 234L );
		
		Long msgid = vf.getMessageId( msg );
		assertNotNull( msgid );
		assertEquals( 234L, msgid );
		
		vf.setMessageId( msg, null );
		
		assertNull( vf.getMessageId( msg ) );
	}

	/////////////////
	// IN REPLY TO //
	/////////////////
	
	@Test
	public void inReplyTo() throws Exception
	{
		Type mt_foo = new Type( "foo" );
		mt_foo.putValidator( vf._mf__inReplyTo, Validator_long.get( 0 ) );
		
		Message msg = new Message( mt_foo, vf );
		
		assertNull( vf.getInReplyTo( msg ) );
		
		vf.setInReplyTo( msg, 234L );
		
		Long msgid = vf.getInReplyTo( msg );
		assertNotNull( msgid );
		assertEquals( 234L, msgid );
		
		vf.setInReplyTo( msg, null );
		
		assertNull( vf.getInReplyTo( msg ) );
	}

	//////////////////////
	// VALUE CONVERSION //
	//////////////////////
	
	@Test
	public void exportCustomValue_RuntimeException() throws Exception
	{
		Exception value = new NullPointerException();
		StructValue sv = vf.exportCustomValue( value );
		sv.checkType( DefaultValueFactory._mt__Etch_RuntimeException );
		assertEquals( 1, sv.size() );
		assertEquals( "java.lang.NullPointerException",
			sv.get( DefaultValueFactory._mf_msg ) );
	}
	
	@Test
	public void exportCustomValue_RuntimeException_msg() throws Exception
	{
		Exception value = new NullPointerException( "foo != null" );
		StructValue sv = vf.exportCustomValue( value );
		sv.checkType( DefaultValueFactory._mt__Etch_RuntimeException );
		assertEquals( 1, sv.size() );
		assertEquals( "java.lang.NullPointerException: foo != null",
			sv.get( DefaultValueFactory._mf_msg ) );
	}
	
	@Test
	public void exportCustomValue_Etch_Date() throws Exception
	{
		Date value = new Date( 12345678901234L );
		StructValue sv = vf.exportCustomValue( value );
		sv.checkType( DefaultValueFactory._mt__Etch_Date );
		assertEquals( 1, sv.size() );
		assertEquals( 12345678901234L, sv.get( DefaultValueFactory._mf_ticks ) );
	}
	
	@Test
	public void exportCustomValue_Object() throws Exception
	{
		Object value = new Object();
		assertNull( vf.exportCustomValue( value ) );
		// cannot export unsupported type
	}
	
	@Test
	public void importCustomValue_Etch_RuntimeException() throws Exception
	{
		StructValue sv = new StructValue( DefaultValueFactory._mt__Etch_RuntimeException );
		_Etch_RuntimeException e = (_Etch_RuntimeException) vf.importCustomValue( sv );
		assertNotNull( e );
		assertNull( e.msg );
	}
	
	@Test
	public void importCustomValue_Etch_RuntimeException_msg()
	{
		StructValue sv = new StructValue( DefaultValueFactory._mt__Etch_RuntimeException );
		sv.put( DefaultValueFactory._mf_msg, "foo" );
		_Etch_RuntimeException e = (_Etch_RuntimeException) vf.importCustomValue( sv );
		assertNotNull( e );
		assertEquals( "foo", e.msg );
	}
	
	@Test
	public void importCustomValue_Etch_Date() throws Exception
	{
		StructValue sv = new StructValue( DefaultValueFactory._mt__Etch_Date );
		sv.put( DefaultValueFactory._mf_ticks, 123456789L );
		Date d = (Date) vf.importCustomValue( sv );
		assertNotNull( d );
		assertEquals( 123456789L, d.getTime() );
	}
	
	@Test
	public void importCustomValue_Unknown() throws Exception
	{
		StructValue sv = new StructValue( vf.getType( "foo" ) );
		assertNull( vf.importCustomValue( sv ) );
		// cannot import unsupported type
	}
	
//	@Test
//	public void to_boolean_array()
//	{
//		// try null array
//		
//		assertNull( DefaultValueFactory.to_boolean( boolean.class, null ) );
//		assertNull( DefaultValueFactory.to_boolean( boolean[].class, null ) );
//		
//		// our data values
//		
//		final boolean x = true;
//		final boolean y = false;
//		final boolean z = true;
//		
//		// try a 0-dimensional array
//		
//		assertEquals( x, DefaultValueFactory.to_boolean( boolean.class, x ) );
//		assertEquals( y, DefaultValueFactory.to_boolean( boolean.class, y ) );
//		assertEquals( z, DefaultValueFactory.to_boolean( boolean.class, z ) );
//		
//		// try a 1-dimensional array
//		
//		ArrayValue av = new ArrayValue();
//		av.add( x );
//		av.add( y );
//		av.add( z );
//		
//		boolean[] b1x3 = (boolean[]) DefaultValueFactory.to_boolean( boolean[].class, av );
//		assertNotNull( b1x3 );
//		assertTrue( arrayEquals( new boolean[] { x, y, z }, b1x3 ) );
//		b1x3 = null;
//		
//		// try a 2-dimensional array
//		
//		av.clear();
//		ArrayValue av0 = av.addArrayValue();
//		av0.add( x );
//		av0.add( y );
//		av0.add( z );
//		ArrayValue av1 = av.addArrayValue();
//		av1.add( z );
//		av1.add( x );
//		av1.add( y );
//		
//		boolean[][] b2x3 = (boolean[][]) DefaultValueFactory.to_boolean( boolean[][].class, av );
//		assertNotNull( b2x3 );
//		assertTrue( arrayEquals( new boolean[][] { {x, y, z}, {z, x, y} }, b2x3 ) );
//	}
	
//	private boolean arrayEquals( Object a, Object b )
//	{
//		if (a.getClass().isArray() != b.getClass().isArray())
//			return false;
//		
//		if (!a.getClass().isArray())
//			return objectEquals( a, b );
//		
//		int n = Array.getLength( a );
//		
//		if (n != Array.getLength( b ))
//			return false;
//		
//		for (int i = 0; i < n; i++)
//			if (!arrayEquals( Array.get( a, i ), Array.get( b, i ) ))
//				return false;
//		
//		return true;
//	}

//	private boolean objectEquals( Object a, Object b )
//	{
//		if (a == null && b == null)
//			return true;
//		
//		if (a == null || b == null)
//			return false;
//		
//		return a.equals( b );
//	}

//	@Test
//	public void to_byte_array()
//	{
//		// try null array
//		
//		assertNull( DefaultValueFactory.to_byte( byte.class, null ) );
//		assertNull( DefaultValueFactory.to_byte( byte[].class, null ) );
//		
//		// our data values
//		
//		final byte x = 0;
//		final byte y = 1;
//		final byte z = -1;
//		
//		// try a 0-dimensional array
//		
//		assertEquals( x, DefaultValueFactory.to_byte( byte.class, x ) );
//		assertEquals( y, DefaultValueFactory.to_byte( byte.class, y ) );
//		assertEquals( z, DefaultValueFactory.to_byte( byte.class, z ) );
//		
//		// try a 1-dimensional array
//		
//		ArrayValue av = new ArrayValue();
//		av.add( x );
//		av.add( y );
//		av.add( z );
//		
//		byte[] b1x3 = (byte[]) DefaultValueFactory.to_byte( byte[].class, av );
//		assertNotNull( b1x3 );
//		assertTrue( arrayEquals( new byte[] { x, y, z }, b1x3 ) );
//		b1x3 = null;
//		
//		// try a 2-dimensional array
//		
//		av.clear();
//		ArrayValue av0 = av.addArrayValue();
//		av0.add( x );
//		av0.add( y );
//		av0.add( z );
//		ArrayValue av1 = av.addArrayValue();
//		av1.add( z );
//		av1.add( x );
//		av1.add( y );
//		
//		byte[][] b2x3 = (byte[][]) DefaultValueFactory.to_byte( byte[][].class, av );
//		assertNotNull( b2x3 );
//		assertTrue( arrayEquals( new byte[][] { {x, y, z}, {z, x, y} }, b2x3 ) );
//	}
	
//	@Test
//	public void to_short_array()
//	{
//		// try null array
//		
//		assertNull( DefaultValueFactory.to_short( short.class, null ) );
//		assertNull( DefaultValueFactory.to_short( short[].class, null ) );
//		
//		// our data values
//		
//		final short x = 1234;
//		final short y = 2345;
//		final short z = -3456;
//		
//		// try a 0-dimensional array
//		
//		assertEquals( x, DefaultValueFactory.to_short( short.class, x ) );
//		assertEquals( y, DefaultValueFactory.to_short( short.class, y ) );
//		assertEquals( z, DefaultValueFactory.to_short( short.class, z ) );
//		
//		// try a 1-dimensional array
//		
//		ArrayValue av = new ArrayValue();
//		av.add( x );
//		av.add( y );
//		av.add( z );
//		
//		short[] b1x3 = (short[]) DefaultValueFactory.to_short( short[].class, av );
//		assertNotNull( b1x3 );
//		assertTrue( arrayEquals( new short[] { x, y, z }, b1x3 ) );
//		b1x3 = null;
//		
//		// try a 2-dimensional array
//		
//		av.clear();
//		ArrayValue av0 = av.addArrayValue();
//		av0.add( x );
//		av0.add( y );
//		av0.add( z );
//		ArrayValue av1 = av.addArrayValue();
//		av1.add( z );
//		av1.add( x );
//		av1.add( y );
//		
//		short[][] b2x3 = (short[][]) DefaultValueFactory.to_short( short[][].class, av );
//		assertNotNull( b2x3 );
//		assertTrue( arrayEquals( new short[][] { {x, y, z}, {z, x, y} }, b2x3 ) );
//	}
	
//	@Test
//	public void to_int_array()
//	{
//		// try null array
//		
//		assertNull( DefaultValueFactory.to_int( int.class, null ) );
//		assertNull( DefaultValueFactory.to_int( int[].class, null ) );
//		
//		// our data values
//		
//		final int x = 123456;
//		final int y = 234567;
//		final int z = -345678;
//		
//		// try a 0-dimensional array
//		
//		assertEquals( x, DefaultValueFactory.to_int( int.class, x ) );
//		assertEquals( y, DefaultValueFactory.to_int( int.class, y ) );
//		assertEquals( z, DefaultValueFactory.to_int( int.class, z ) );
//		
//		// try a 1-dimensional array
//		
//		ArrayValue av = new ArrayValue();
//		av.add( x );
//		av.add( y );
//		av.add( z );
//		
//		int[] b1x3 = (int[]) DefaultValueFactory.to_int( int[].class, av );
//		assertNotNull( b1x3 );
//		assertTrue( arrayEquals( new int[] { x, y, z }, b1x3 ) );
//		b1x3 = null;
//		
//		// try a 2-dimensional array
//		
//		av.clear();
//		ArrayValue av0 = av.addArrayValue();
//		av0.add( x );
//		av0.add( y );
//		av0.add( z );
//		ArrayValue av1 = av.addArrayValue();
//		av1.add( z );
//		av1.add( x );
//		av1.add( y );
//		
//		int[][] b2x3 = (int[][]) DefaultValueFactory.to_int( int[][].class, av );
//		assertNotNull( b2x3 );
//		assertTrue( arrayEquals( new int[][] { {x, y, z}, {z, x, y} }, b2x3 ) );
//	}
	
//	@Test
//	public void to_long_array()
//	{
//		// try null array
//		
//		assertNull( DefaultValueFactory.to_long( long.class, null ) );
//		assertNull( DefaultValueFactory.to_long( long[].class, null ) );
//		
//		// our data values
//		
//		final long x = 123456789123L;
//		final long y = 234567891234L;
//		final long z = -345678912345L;
//		
//		// try a 0-dimensional array
//		
//		assertEquals( x, DefaultValueFactory.to_long( long.class, x ) );
//		assertEquals( y, DefaultValueFactory.to_long( long.class, y ) );
//		assertEquals( z, DefaultValueFactory.to_long( long.class, z ) );
//		
//		// try a 1-dimensional array
//		
//		ArrayValue av = new ArrayValue();
//		av.add( x );
//		av.add( y );
//		av.add( z );
//		
//		long[] b1x3 = (long[]) DefaultValueFactory.to_long( long[].class, av );
//		assertNotNull( b1x3 );
//		assertTrue( arrayEquals( new long[] { x, y, z }, b1x3 ) );
//		b1x3 = null;
//		
//		// try a 2-dimensional array
//		
//		av.clear();
//		ArrayValue av0 = av.addArrayValue();
//		av0.add( x );
//		av0.add( y );
//		av0.add( z );
//		ArrayValue av1 = av.addArrayValue();
//		av1.add( z );
//		av1.add( x );
//		av1.add( y );
//		
//		long[][] b2x3 = (long[][]) DefaultValueFactory.to_long( long[][].class, av );
//		assertNotNull( b2x3 );
//		assertTrue( arrayEquals( new long[][] { {x, y, z}, {z, x, y} }, b2x3 ) );
//	}
	
//	@Test
//	public void to_float_array()
//	{
//		// try null array
//		
//		assertNull( DefaultValueFactory.to_float( float.class, null ) );
//		assertNull( DefaultValueFactory.to_float( float[].class, null ) );
//		
//		// our data values
//		
//		final float x = 1.2f;
//		final float y = 2.3f;
//		final float z = -3.4f;
//		
//		// try a 0-dimensional array
//		
//		assertEquals( x, DefaultValueFactory.to_float( float.class, x ) );
//		assertEquals( y, DefaultValueFactory.to_float( float.class, y ) );
//		assertEquals( z, DefaultValueFactory.to_float( float.class, z ) );
//		
//		// try a 1-dimensional array
//		
//		ArrayValue av = new ArrayValue();
//		av.add( x );
//		av.add( y );
//		av.add( z );
//		
//		float[] b1x3 = (float[]) DefaultValueFactory.to_float( float[].class, av );
//		assertNotNull( b1x3 );
//		assertTrue( arrayEquals( new float[] { x, y, z }, b1x3 ) );
//		b1x3 = null;
//		
//		// try a 2-dimensional array
//		
//		av.clear();
//		ArrayValue av0 = av.addArrayValue();
//		av0.add( x );
//		av0.add( y );
//		av0.add( z );
//		ArrayValue av1 = av.addArrayValue();
//		av1.add( z );
//		av1.add( x );
//		av1.add( y );
//		
//		float[][] b2x3 = (float[][]) DefaultValueFactory.to_float( float[][].class, av );
//		assertNotNull( b2x3 );
//		assertTrue( arrayEquals( new float[][] { {x, y, z}, {z, x, y} }, b2x3 ) );
//	}
	
//	@Test
//	public void to_double_array()
//	{
//		// try null array
//		
//		assertNull( DefaultValueFactory.to_double( double.class, null ) );
//		assertNull( DefaultValueFactory.to_double( double[].class, null ) );
//		
//		// our data values
//		
//		final double x = 1.2e99;
//		final double y = 2.3e99;
//		final double z = -3.4e99;
//		
//		// try a 0-dimensional array
//		
//		assertEquals( x, DefaultValueFactory.to_double( double.class, x ) );
//		assertEquals( y, DefaultValueFactory.to_double( double.class, y ) );
//		assertEquals( z, DefaultValueFactory.to_double( double.class, z ) );
//		
//		// try a 1-dimensional array
//		
//		ArrayValue av = new ArrayValue();
//		av.add( x );
//		av.add( y );
//		av.add( z );
//		
//		double[] b1x3 = (double[]) DefaultValueFactory.to_double( double[].class, av );
//		assertNotNull( b1x3 );
//		assertTrue( arrayEquals( new double[] { x, y, z }, b1x3 ) );
//		b1x3 = null;
//		
//		// try a 2-dimensional array
//		
//		av.clear();
//		ArrayValue av0 = av.addArrayValue();
//		av0.add( x );
//		av0.add( y );
//		av0.add( z );
//		ArrayValue av1 = av.addArrayValue();
//		av1.add( z );
//		av1.add( x );
//		av1.add( y );
//		
//		double[][] b2x3 = (double[][]) DefaultValueFactory.to_double( double[][].class, av );
//		assertNotNull( b2x3 );
//		assertTrue( arrayEquals( new double[][] { {x, y, z}, {z, x, y} }, b2x3 ) );
//	}
	
//	@Test
//	public void to_string_array()
//	{
//		// try null array
//		
//		assertNull( DefaultValueFactory.to_string( String.class, null ) );
//		assertNull( DefaultValueFactory.to_string( String[].class, null ) );
//		
//		// our data values
//		
//		final String x = "abc";
//		final String y = "def";
//		final String z = "ghi";
//		
//		// try a 0-dimensional array
//		
//		assertEquals( x, DefaultValueFactory.to_string( String.class, x ) );
//		assertEquals( y, DefaultValueFactory.to_string( String.class, y ) );
//		assertEquals( z, DefaultValueFactory.to_string( String.class, z ) );
//		
//		// try a 1-dimensional array
//		
//		ArrayValue av = new ArrayValue();
//		av.add( x );
//		av.add( y );
//		av.add( z );
//		
//		String[] b1x3 = (String[]) DefaultValueFactory.to_string( String[].class, av );
//		assertNotNull( b1x3 );
//		assertTrue( arrayEquals( new String[] { x, y, z }, b1x3 ) );
//		b1x3 = null;
//		
//		// try a 2-dimensional array
//		
//		av.clear();
//		ArrayValue av0 = av.addArrayValue();
//		av0.add( x );
//		av0.add( y );
//		av0.add( z );
//		ArrayValue av1 = av.addArrayValue();
//		av1.add( z );
//		av1.add( x );
//		av1.add( y );
//		
//		String[][] b2x3 = (String[][]) DefaultValueFactory.to_string( String[][].class, av );
//		assertNotNull( b2x3 );
//		assertTrue( arrayEquals( new String[][] { {x, y, z}, {z, x, y} }, b2x3 ) );
//	}
}
