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

package org.apache.etch.bindings.java.support;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNotNull;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;

import java.util.HashSet;
import java.util.Set;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.TypeMap;
import org.junit.Test;


/** Test DefaultValueFactory */
public class TestDefaultValueFactory
{
	private MyValueFactory vf = new MyValueFactory( "tcp:" );
	
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
		assertNotNull( vf.getType( "a" ) );
		vf.lockDynamicTypes();
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
		StructValue sv = new StructValue( vf.getType( "_Etch_RuntimeException" ), vf );
		_Etch_RuntimeException e = (_Etch_RuntimeException) vf.importCustomValue( sv );
		assertNotNull( e );
		assertNull( e.msg );
	}
	
	/** @throws Exception */
	@Test
	public void importCustomValue_Etch_RuntimeException_msg() throws Exception
	{
		StructValue sv = new StructValue( vf.getType( "_Etch_RuntimeException" ), vf );
		sv.put( DefaultValueFactory._mf_msg, "foo" );
		_Etch_RuntimeException e = (_Etch_RuntimeException) vf.importCustomValue( sv );
		assertNotNull( e );
		assertEquals( "foo", e.msg );
	}
	
	/** @throws Exception */
	@Test
	public void importCustomValue_Unknown() throws Exception
	{
		StructValue sv = new StructValue( vf.getType( "x" ), vf );
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
		 * @param uri 
		 */
		public MyValueFactory( String uri )
		{
			super( uri, types, class2type );
		}
	}
}
