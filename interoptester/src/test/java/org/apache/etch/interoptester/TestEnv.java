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

import org.apache.etch.interoptester.Env;
import org.apache.etch.util.AssertionException;
import org.apache.etch.util.core.xml.XmlParser;
import org.apache.etch.util.core.xml.XmlParser.TagElement;
import org.junit.Test;

import static org.junit.Assert.*;


/** Test of Env */
public class TestEnv
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
		// tag is env
		Env.parse( parseXml( "<blah/>" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void parse2() throws Exception
	{
		// env tag has no name
		Env.parse( parseXml( "<env/>" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void parse3() throws Exception
	{
		// env tag has no name
		Env.parse( parseXml( "<env name=\"\"/>" ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse4() throws Exception
	{
		Env e = Env.parse( parseXml( "<env name=\"foo\"/>" ) );
		assertEquals( "foo", e.name() );
		assertNull( e.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse5() throws Exception
	{
		Env e = Env.parse( parseXml( "<env name=\"foo\"></env>" ) );
		assertEquals( "foo", e.name() );
		assertNull( e.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse6() throws Exception
	{
		Env e = Env.parse( parseXml( "<env name=\"bar\">abc</env>" ) );
		assertEquals( "bar", e.name() );
		assertEquals( "abc", e.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse7() throws Exception
	{
		Env e = Env.parse( parseXml( "<env name=\"foo\" value=\"\"/>" ) );
		assertEquals( "foo", e.name() );
		assertEquals( "", e.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse8() throws Exception
	{
		Env e = Env.parse( parseXml( "<env name=\"foo\" value=\"abc\"/>" ) );
		assertEquals( "foo", e.name() );
		assertEquals( "abc", e.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse9() throws Exception
	{
		Env e = Env.parse( parseXml( "<env name=\"bar\">{blah}</env>" ) );
		assertEquals( "bar", e.name() );
		assertEquals( "{blah}", e.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse10() throws Exception
	{
		Env e = Env.parse( parseXml( "<env name=\"foo\" value=\"{blah}\"/>" ) );
		assertEquals( "foo", e.name() );
		assertEquals( "{blah}", e.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse11() throws Exception
	{
		Env e = Env.parse( parseXml( "<env name=\"bar\">s{blag}t</env>" ) );
		assertEquals( "bar", e.name() );
		assertEquals( "s{blag}t", e.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse12() throws Exception
	{
		Env e = Env.parse( parseXml( "<env name=\"foo\" value=\"s{blag}t\"/>" ) );
		assertEquals( "foo", e.name() );
		assertEquals( "s{blag}t", e.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse13() throws Exception
	{
		Env e = Env.parse( parseXml( "<env name=\"bar\">{bar}</env>" ) );
		assertEquals( "bar", e.name() );
		assertEquals( "123", e.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse14() throws Exception
	{
		Env e = Env.parse( parseXml( "<env name=\"foo\" value=\"{bar}\"/>" ) );
		assertEquals( "foo", e.name() );
		assertEquals( "123", e.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse15() throws Exception
	{
		Env e = Env.parse( parseXml( "<env name=\"bar\">x{baz}y</env>" ) );
		assertEquals( "bar", e.name() );
		assertEquals( "x456y", e.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse16() throws Exception
	{
		Env e = Env.parse( parseXml( "<env name=\"foo\" value=\"x{baz}y\"/>" ) );
		assertEquals( "foo", e.name() );
		assertEquals( "x456y", e.value( args ) );
	}
}
