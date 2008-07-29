/* $Id$
 *
 * Created by sccomer on Feb 4, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg.test;

import static org.junit.Assert.assertEquals;

import org.junit.Test;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.IdName;

public class TestField
{
	@Test
	public void fieldIntegerString()
	{
		testMf( 1, "one" );
		testMf( 2, "two" );
		testMf( 3, "three" );
	}

	@Test
	public void fieldString()
	{
		testMf( "one" );
		testMf( "two" );
		testMf( "three" );
	}

	private void testMf( int id, String name )
	{
		Field mf = new Field( id, name );
		assertEquals( id, mf.getId() );
		assertEquals( name, mf.getName() );
	}

	private void testMf( String name )
	{
		Field mf = new Field( name );
		assertEquals( IdName.hash( name ), mf.getId() );
		assertEquals( name, mf.getName() );
	}
}
