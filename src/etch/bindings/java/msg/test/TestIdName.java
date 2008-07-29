/* $Id$
 *
 * Created by sccomer on Feb 4, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg.test;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import org.junit.Test;

import etch.bindings.java.msg.IdName;

/** */
public class TestIdName
{
	/** */
	@Test
	public void getId()
	{
		assertEquals( 1, new IdName( 1, "foo" ).getId() );
		assertEquals( 1, new IdName( 1, "bar" ).getId() );
		assertEquals( 2, new IdName( 2, "foo" ).getId() );
		assertEquals( 2, new IdName( 2, "bar" ).getId() );
	}

	/** */
	@Test
	public void getName()
	{
		assertEquals( "foo", new IdName( 1, "foo" ).getName() );
		assertEquals( "bar", new IdName( 1, "bar" ).getName() );
		assertEquals( "foo", new IdName( 2, "foo" ).getName() );
		assertEquals( "bar", new IdName( 2, "bar" ).getName() );
	}

	/** */
	@Test
	public void xtoString()
	{
		assertEquals( "foo", new IdName( 1, "foo" ).toString() );
		assertEquals( "bar", new IdName( 1, "bar" ).toString() );
		assertEquals( "foo", new IdName( 2, "foo" ).toString() );
		assertEquals( "bar", new IdName( 2, "bar" ).toString() );
	}

	/** */
	@Test
	public void equalsObject()
	{
		assertTrue( new IdName( 1, "foo" ).equals( new IdName( 1, "foo" ) ) );
		assertTrue( new IdName( 1, "foo" ).equals( new IdName( 1, "bar" ) ) );
		assertTrue( new IdName( 2, "foo" ).equals( new IdName( 2, "foo" ) ) );
		assertTrue( new IdName( 2, "foo" ).equals( new IdName( 2, "bar" ) ) );
		
		assertFalse( new IdName( 1, "foo" ).equals( new IdName( 2, "foo" ) ) );
		assertFalse( new IdName( 1, "foo" ).equals( new IdName( 2, "bar" ) ) );
		assertFalse( new IdName( 2, "foo" ).equals( new IdName( 1, "foo" ) ) );
		assertFalse( new IdName( 2, "foo" ).equals( new IdName( 1, "bar" ) ) );
		
		assertFalse( new IdName( 1, "foo" ).equals( null ) );
		assertFalse( new IdName( 1, "foo" ).equals( 1 ) );
		assertFalse( new IdName( 1, "foo" ).equals( true ) );
	}

	/** */
	@Test
	public void hashCodeObject()
	{
		// you cannot really predict when hashCode will not be the same. the
		// rule is that it must be the same when the objects would compare equal.
		assertEquals( new IdName( 1, "foo" ).hashCode(), new IdName( 1, "foo" ).hashCode() );
		assertEquals( new IdName( 1, "foo" ).hashCode(), new IdName( 1, "bar" ).hashCode() );
		assertEquals( new IdName( 2, "foo" ).hashCode(), new IdName( 2, "foo" ).hashCode() );
		assertEquals( new IdName( 2, "foo" ).hashCode(), new IdName( 2, "bar" ).hashCode() );
	}

	/** */
	@Test
	public void hash()
	{
		assertEquals( 5381, IdName.hash( "" ) );
		assertEquals( 352988316, IdName.hash( "a" ) );
		assertEquals( 1511848646, IdName.hash( "ab" ) );
		assertEquals( 669497117, IdName.hash( "abc" ) );
		assertEquals( -1994190713, IdName.hash( "abcd" ) );
		assertEquals( -802680418, IdName.hash( "abcde" ) );
		assertEquals( 1266308680, IdName.hash( "abcdef" ) );
		assertEquals( -379372513, IdName.hash( "abcdefg" ) );
		assertEquals( -1416967159, IdName.hash( "abcdefgh" ) );
		assertEquals( 53556896, IdName.hash( "abcdefghi" ) );
		assertEquals( -4427318, IdName.hash( "abcdefghij" ) );
	}
}
