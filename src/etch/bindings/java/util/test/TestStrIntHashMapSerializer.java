/* $Id$
 *
 * Created by wert on Jun 25, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.util.test;

import static org.junit.Assert.assertEquals;

import org.junit.Test;

import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.support.DefaultValueFactory;
import etch.bindings.java.util.StrIntHashMap;
import etch.bindings.java.util.StrIntHashMapSerializer;

public class TestStrIntHashMapSerializer
{
	private final DefaultValueFactory vf = new DefaultValueFactory();
	
	@Test
	public void testImport0() throws Exception
	{
		StrIntHashMap map = new StrIntHashMap();
		testMap( map );
	}
	
	@Test
	public void testImport1() throws Exception
	{
		StrIntHashMap map = new StrIntHashMap();
		map.put( "a", 1 );
		testMap( map );
	}
	
	@Test
	public void testImport2() throws Exception
	{
		StrIntHashMap map = new StrIntHashMap();
		map.put( "a", 1 );
		map.put( "b", 2 );
		testMap( map );
	}
	
	@Test
	public void testImport3() throws Exception
	{
		StrIntHashMap map = new StrIntHashMap();
		map.put( "a", 1 );
		map.put( "b", 2 );
		map.put( "c", 3 );
		testMap( map );
	}
	
	@Test
	public void testImportNull1() throws Exception
	{
		StrIntHashMap map = new StrIntHashMap();
		map.put( "a", null );
		testMap( map );
	}
	
	@Test
	public void testImportNull2() throws Exception
	{
		StrIntHashMap map = new StrIntHashMap();
		map.put( null, 2 );
		testMap( map );
	}
	
	@Test
	public void testImportNull3() throws Exception
	{
		StrIntHashMap map = new StrIntHashMap();
		map.put( null, null );
		testMap( map );
	}
	
	public void testMap( StrIntHashMap map )
	{
		Type type = vf.getType( "blah" );
		StrIntHashMapSerializer.init( vf, type );
		
		StructValue sv = new StructValue( type );
		StrIntHashMapSerializer.exportValue( vf, map, sv );
//		System.out.println( "sv = "+sv );
		StrIntHashMap map2 = StrIntHashMapSerializer.importValue( vf, sv );
		assertEquals( map, map2 );
	}
}
