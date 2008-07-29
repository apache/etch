package etch.bindings.java.support.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;

import java.util.HashSet;
import java.util.Set;

import org.junit.Test;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.support.Class2TypeMap;
import etch.bindings.java.support.DefaultValueFactory;
import etch.bindings.java.support.TypeMap;
import etch.bindings.java.support.Validator_long;
import etch.bindings.java.support._Etch_RuntimeException;

/** Test DefaultValueFactory */
public class TestDefaultValueFactory
{
	private MyValueFactory vf = new MyValueFactory();
	
	/** @throws Exception */
	@Test
	public void testTypes() throws Exception
	{
		Set<Type> eTypes = mkTypeSet(
			vf.getType( "_Etch_RuntimeException" ),
			vf.getType( "_Etch_List" ),
			vf.getType( "_Etch_Map" ),
			vf.getType( "_Etch_Set" ),
			vf.getType( "_Etch_Datetime" ),
			vf.getType( "_Etch_AuthException" ),
			vf.getType( "_exception" ),
			MyValueFactory._mt_x,
			MyValueFactory._mt_y
		);
		
		assertEquals( eTypes, vf.getTypes() );
		
		for (Type t: eTypes)
			testType( t );
	}
	
	private Set<Type> mkTypeSet( Type ... types )
	{
		Set<Type> set = new HashSet<Type>();
		for (Type t: types)
			set.add( t );
		return set;
	}
	
	private void testType( Type type )
	{
		assertSame( type, vf.getType( type.getId() ) );
		assertSame( type, vf.getType( type.getName() ) );
	}
	
	private Set<Field> mkFieldSet( Field ... fields )
	{
		Set<Field> set = new HashSet<Field>();
		for (Field f: fields )
			set.add( f );
		return set;
	}
	
	/** @throws Exception */
	@Test
	public void test_Etch_RuntimeException_fields() throws Exception
	{
		testTypeFields(
			vf.getType( "_Etch_RuntimeException" ),
			DefaultValueFactory._mf_msg
		);
		
		testTypeField(
			vf.getType( "_Etch_RuntimeException" ),
			DefaultValueFactory._mf_msg,
			"abc"
		);			
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void test_Etch_RuntimeException_msg_badValue() throws Exception
	{
		testTypeField(
			vf.getType( "_Etch_RuntimeException" ),
			DefaultValueFactory._mf_msg,
			123
		);
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void test_Etch_RuntimeException_badField() throws Exception
	{
		testTypeField(
			vf.getType( "_Etch_RuntimeException" ),
			MyValueFactory._mf_a,
			"abc"
		);	
	}
	
	private void testTypeField( Type type, Field field, Object value )
	{
		Message msg = new Message( type, vf );
		msg.put( field, value );
	}
	
	private void testTypeFields( Type type, Field ... fields )
	{
		Set<Field> aFields = type.getFields();
		Set<Field> eFields = mkFieldSet( fields );
		assertEquals( eFields, aFields );
	}
	
	/** @throws Exception */
	@Test
	public void test_Etch_AuthException_fields() throws Exception
	{
		testTypeFields(
			vf.getType( "_Etch_AuthException" ),
			DefaultValueFactory._mf_msg
		);
		
		testTypeField(
			vf.getType( "_Etch_AuthException" ),
			DefaultValueFactory._mf_msg,
			"abc"
		);
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void test_Etch_AuthException_msg_badValue() throws Exception
	{
		testTypeField(
			vf.getType( "_Etch_AuthException" ),
			DefaultValueFactory._mf_msg,
			123
		);
	}
	
	/** @throws Exception */
	@Test
	public void test_exception_fields() throws Exception
	{
		testTypeFields(
			vf.getType( "_exception" ),
			DefaultValueFactory._mf_result,
			DefaultValueFactory._mf__messageId,
			DefaultValueFactory._mf__inReplyTo
		);
		
		testTypeField(
			vf.getType( "_exception" ),
			DefaultValueFactory._mf_result,
			new RuntimeException()
		);
		
		testTypeField(
			vf.getType( "_exception" ),
			DefaultValueFactory._mf__messageId,
			123L
		);
		
		testTypeField(
			vf.getType( "_exception" ),
			DefaultValueFactory._mf__inReplyTo,
			123L
		);
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void test_exception_result_badValue() throws Exception
	{
		testTypeField(
			vf.getType( "_exception" ),
			DefaultValueFactory._mf_result,
			123
		);
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void test_exception_messageId_badValue() throws Exception
	{
		testTypeField(
			vf.getType( "_exception" ),
			DefaultValueFactory._mf__messageId,
			true
		);
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void test_exception_inReplyTo_badValue() throws Exception
	{
		testTypeField(
			vf.getType( "_exception" ),
			DefaultValueFactory._mf__inReplyTo,
			"abc"
		);
	}

	/** @throws Exception */
	@Test
	public void getType_locked() throws Exception
	{
		assertNotNull( vf.getType( "x" ) );
		assertNotNull( vf.getType( "y" ) );
		assertNull( vf.getType( "a" ) );
		assertNull( vf.getType( "b" ) );
	}

	/** @throws Exception */
	@Test
	public void getField_locked() throws Exception
	{
		Type type = new Type( "blah" );
		assertNotNull( type.getField( "a" ) );
		assertNotNull( type.getField( "b" ) );
		type.lock();
		assertNotNull( type.getField( "a" ) );
		assertNotNull( type.getField( "b" ) );
		assertNull( type.getField( "x" ) );
		assertNull( type.getField( "y" ) );
	}
	
	/** @throws Exception */
	@Test
	public void getField_id_messageId() throws Exception
	{
		assertEquals( "_messageId", DefaultValueFactory._mf__messageId.getName() );
		assertEquals( 1661383784, DefaultValueFactory._mf__messageId.getId() );
	}

	/** @throws Exception */
	@Test
	public void getField_id_inReplyTo() throws Exception
	{
		assertEquals( "_inReplyTo", DefaultValueFactory._mf__inReplyTo.getName() );
		assertEquals( -307705434, DefaultValueFactory._mf__inReplyTo.getId() );
	}

	/////////////////////
	// STRING ENCODING //
	/////////////////////
	
	/** @throws Exception */
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
	
	/** @throws Exception */
	@Test
	public void messageId() throws Exception
	{
		Type mt_foo = new Type( "foo" );
		mt_foo.putValidator( DefaultValueFactory._mf__messageId, Validator_long.get( 0 ) );
		
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
	
	/** @throws Exception */
	@Test
	public void inReplyTo() throws Exception
	{
		Type mt_foo = new Type( "foo" );
		mt_foo.putValidator( DefaultValueFactory._mf__inReplyTo, Validator_long.get( 0 ) );
		
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
	
	/** @throws Exception */
	@Test
	public void exportCustomValue_RuntimeException() throws Exception
	{
		Exception value = new NullPointerException();
		StructValue sv = vf.exportCustomValue( value );
		sv.checkType( vf.getType( "_Etch_RuntimeException" ) );
		assertEquals( 1, sv.size() );
		assertEquals( "java.lang.NullPointerException",
			sv.get( DefaultValueFactory._mf_msg ) );
	}
	
	/** @throws Exception */
	@Test
	public void exportCustomValue_RuntimeException_msg() throws Exception
	{
		Exception value = new NullPointerException( "foo != null" );
		StructValue sv = vf.exportCustomValue( value );
		sv.checkType( vf.getType( "_Etch_RuntimeException" ) );
		assertEquals( 1, sv.size() );
		assertEquals( "java.lang.NullPointerException: foo != null",
			sv.get( DefaultValueFactory._mf_msg ) );
	}
	
	/** @throws Exception */
	@Test
	public void exportCustomValue_Object() throws Exception
	{
		Object value = new Object();
		assertNull( vf.exportCustomValue( value ) );
		// cannot export unsupported type
	}
	
	/** @throws Exception */
	@Test
	public void importCustomValue_Etch_RuntimeException() throws Exception
	{
		StructValue sv = new StructValue( vf.getType( "_Etch_RuntimeException" ) );
		_Etch_RuntimeException e = (_Etch_RuntimeException) vf.importCustomValue( sv );
		assertNotNull( e );
		assertNull( e.msg );
	}
	
	/** @throws Exception */
	@Test
	public void importCustomValue_Etch_RuntimeException_msg() throws Exception
	{
		StructValue sv = new StructValue( vf.getType( "_Etch_RuntimeException" ) );
		sv.put( DefaultValueFactory._mf_msg, "foo" );
		_Etch_RuntimeException e = (_Etch_RuntimeException) vf.importCustomValue( sv );
		assertNotNull( e );
		assertEquals( "foo", e.msg );
	}
	
	/** @throws Exception */
	@Test
	public void importCustomValue_Unknown() throws Exception
	{
		StructValue sv = new StructValue( vf.getType( "x" ) );
		assertNull( vf.importCustomValue( sv ) );
		// cannot import unsupported type
	}
	
	/**
	 * A fake implementation of DefaultValueFactory for testing.
	 */
	public static class MyValueFactory extends DefaultValueFactory
	{
		private final static TypeMap types = new TypeMap();
		
		private final static Class2TypeMap class2type = new Class2TypeMap();

		static
		{
			DefaultValueFactory.init( types, class2type );
		}
		
		// define our types and fields here:
		
		/** type x */
		public final static Type _mt_x = types.get( "x" );
		
		/** type y */
		public final static Type _mt_y = types.get( "y" );
		
		/** field a */
		public final static Field _mf_a = new Field( "a" );
		
		/** field b */
		public final static Field _mf_b = new Field( "b" );
		
		static
		{
			types.lock();
			class2type.lock();
		}

		/**
		 * Constructs the ValueFactoryFake.
		 */
		public MyValueFactory()
		{
			super( types, class2type );
		}
	}
}
