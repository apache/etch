package etch.bindings.java.support.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertSame;

import org.junit.Test;

import etch.bindings.java.msg.Validator;
import etch.bindings.java.support.TypeValidator;

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

		@Override
		public Validator elementValidator()
		{
			return null;
		}
	}
}
