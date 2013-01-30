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

package org.apache.etch.interoptester;

import java.util.HashMap;
import java.util.Map;


import org.apache.etch.interoptester.Param;
import org.apache.etch.util.AssertionException;
import org.apache.etch.util.core.xml.XmlParser;
import org.apache.etch.util.core.xml.XmlParser.TagElement;
import org.junit.Test;

import static org.junit.Assert.*;


/** */
public class TestParam
{
	private final Map<String, String> args = new HashMap<String, String>();
	{
		args.put( "bar", "123" );
		args.put( "baz", "456" );
	}
	
	private TagElement parseXml( String s ) throws Exception
	{
		return new XmlParser().parseOne( s );
	}
	
	/** @throws Exception */
	@Test( expected = AssertionException.class )
	public void parse1() throws Exception
	{
		// tag is param
		Param.parse( parseXml( "<foo/>" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void parse2() throws Exception
	{
		// param tag has no name attribute
		Param.parse( parseXml( "<param/>" ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse3() throws Exception
	{
		Param p = Param.parse( parseXml( "<param name=\"foo\"/>" ) );
		assertEquals( "foo", p.name() );
		assertNull( p.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse4() throws Exception
	{
		Param p = Param.parse( parseXml( "<param name=\"foo\"></param>" ) );
		assertEquals( "foo", p.name() );
		assertNull( p.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse5() throws Exception
	{
		Param p = Param.parse( parseXml( "<param name=\"foo\">789</param>" ) );
		assertEquals( "foo", p.name() );
		assertEquals( "789", p.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse6() throws Exception
	{
		Param p = Param.parse( parseXml( "<param name=\"foo\" default=\"\"/>" ) );
		assertEquals( "foo", p.name() );
		assertEquals( "", p.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse7() throws Exception
	{
		Param p = Param.parse( parseXml( "<param name=\"foo\" default=\"789\"/>" ) );
		assertEquals( "foo", p.name() );
		assertEquals( "789", p.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse8() throws Exception
	{
		Param p = Param.parse( parseXml( "<param name=\"bar\"/>" ) );
		assertEquals( "bar", p.name() );
		assertEquals( "123", p.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse9() throws Exception
	{
		Param p = Param.parse( parseXml( "<param name=\"baz\" default=\"234\"/>" ) );
		assertEquals( "baz", p.name() );
		assertEquals( "456", p.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void toString1() throws Exception
	{
		Param p = Param.parse( parseXml( "<param name=\"baz\" default=\"234\"/>" ) );
		assertEquals( "Param( baz, 234 )", p.toString() );
	}
}
