/* $Id$
 *
 * Created by sccomer on Feb 4, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.IdName;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator;
import etch.bindings.java.support.ComboValidator;
import etch.bindings.java.support.Validator_int;

/** */
public class TestType
{
	/** */
	@Test
	public void typeIntegerString()
	{
		testMt( 1, "one" );
		testMt( 2, "two" );
		testMt( 3, "three" );
	}

	/** */
	@Test
	public void typeString()
	{
		testMt( "one" );
		testMt( "two" );
		testMt( "three" );
	}

	/** @throws Exception */
	@Test
	public void testRunValidators() throws Exception
	{
		boolean b = Type.getRunValidators();

		Type.setRunValidators( true );
		assertTrue( Type.getRunValidators() );

		Type.setRunValidators( false );
		assertFalse( Type.getRunValidators() );
		
		Type.setRunValidators( b );
	}

	/** @throws Exception */
	@Test
	public void testGetValidator() throws Exception
	{
		Type t = new Type( "t" );
		Field f = new Field( "f" );
		Field g = new Field( "g" );
		Validator v = Validator_int.get( 0 );
		
		t.putValidator( f, v );
		
		Type.setRunValidators( false );
		assertNull( t.getValidator( f ) );
		
		Type.setRunValidators( true );
		assertSame( v, t.getValidator( f ) );
		
		Type.setRunValidators( false );
		assertNull( t.getValidator( g ) );
		
		Type.setRunValidators( true );
		assertNull( t.getValidator( g ) );
	}

	/** @throws Exception */
	@Test
	public void testPutValidatorNull() throws Exception
	{
		Type t = new Type( "t" );
		Field f = new Field( "f" );
		Validator v = Validator_int.get( 0 );

		Type.setRunValidators( true );
		assertNull( t.getValidator( f ) );
		
		t.putValidator( f, null );
		assertNull( t.getValidator( f ) );
		
		t.putValidator( f, v );
		assertSame( v, t.getValidator( f ) );
		
		t.putValidator( f, v );
		assertSame( ComboValidator.class, t.getValidator( f ).getClass() );
		
		t.clearValidator( f );
		assertNull( t.getValidator( f ) );
	}

	private void testMt( int id, String name )
	{
		Type mt = new Type( id, name );
		assertEquals( id, mt.getId() );
		assertEquals( name, mt.getName() );
	}

	private void testMt( String name )
	{
		Type mt = new Type( name );
		assertEquals( IdName.hash( name ), mt.getId() );
		assertEquals( name, mt.getName() );
	}
}
