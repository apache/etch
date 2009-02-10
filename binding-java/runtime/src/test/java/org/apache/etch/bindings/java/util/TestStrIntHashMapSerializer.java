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

package org.apache.etch.bindings.java.util;

import static org.junit.Assert.assertEquals;

import org.apache.etch.bindings.java.msg.ImportExportHelper;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.Class2TypeMap;
import org.apache.etch.bindings.java.support.DummyValueFactory;
import org.junit.Test;


/** Tests StrIntHashMapSerializer */
public class TestStrIntHashMapSerializer
{
	private final ValueFactory vf = new DummyValueFactory();
	
	/** @throws Exception */
	@Test
	public void test0() throws Exception
	{
		StrIntHashMap map = new StrIntHashMap();
		testMap( map );
	}

	/** @throws Exception */
	@Test
	public void test1() throws Exception
	{
		StrIntHashMap map = new StrIntHashMap();
		map.put( "a", 1 );
		testMap( map );
	}

	/** @throws Exception */
	@Test
	public void test2() throws Exception
	{
		StrIntHashMap map = new StrIntHashMap();
		map.put( "a", 1 );
		map.put( "b", 2 );
		testMap( map );
	}

	/** @throws Exception */
	@Test
	public void test3() throws Exception
	{
		StrIntHashMap map = new StrIntHashMap();
		map.put( "a", 1 );
		map.put( "b", 2 );
		map.put( "c", 3 );
		testMap( map );
	}

	/** @throws Exception */
	@Test
	public void testNullValue() throws Exception
	{
		StrIntHashMap map = new StrIntHashMap();
		map.put( "a", 1 );
		map.put( "b", null );
		map.put( "c", 3 );
		testMap( map );
	}

	/** @throws Exception */
	@Test
	public void testNullKey() throws Exception
	{
		StrIntHashMap map = new StrIntHashMap();
		map.put( "a", 1 );
		map.put( null, 2 );
		map.put( "c", 3 );
		testMap( map );
	}

	/** @throws Exception */
	@Test
	public void testNullKeyValue() throws Exception
	{
		StrIntHashMap map = new StrIntHashMap();
		map.put( "a", 1 );
		map.put( null, null );
		map.put( "c", 3 );
		testMap( map );
	}
	
	private void testMap( StrIntHashMap map )
	{
//		System.out.println( "---------------------------------------------" );
//		System.out.println( "map = "+map );
		
		Type type = new Type( "map" );
		Class2TypeMap class2type = new Class2TypeMap();
		StrIntHashMapSerializer.init( type, class2type );
		ImportExportHelper helper = type.getImportExportHelper();
		
		StructValue sv = helper.exportValue( vf, map );
//		System.out.println( "sv = "+sv );
		
		assertEquals( sv.type(), type );
		
		StrIntHashMap map2 = (StrIntHashMap) helper.importValue( sv );
//		System.out.println( "map2 = "+map2 );
		
		assertEquals( map, map2 );
	}
}
