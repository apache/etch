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

import org.apache.etch.interoptester.Token;
import org.apache.etch.util.AssertionException;
import org.apache.etch.util.core.xml.XmlParser;
import org.apache.etch.util.core.xml.XmlParser.TagElement;
import org.junit.Test;
import static org.junit.Assert.*;


/** Test of Token */
public class TestToken
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
		// tag is token
		Token.parse( parseXml( "<blah/>" ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse2() throws Exception
	{
		Token t = Token.parse( parseXml( "<token/>" ) );
		assertNull( t.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse3() throws Exception
	{
		Token t = Token.parse( parseXml( "<token></token>" ) );
		assertNull( t.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse4() throws Exception
	{
		Token t = Token.parse( parseXml( "<token>abc</token>" ) );
		assertEquals( "abc", t.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse5() throws Exception
	{
		Token t = Token.parse( parseXml( "<token value=\"\"/>" ) );
		assertEquals( "", t.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse6() throws Exception
	{
		Token t = Token.parse( parseXml( "<token value=\"abc\"/>" ) );
		assertEquals( "abc", t.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse7() throws Exception
	{
		Token t = Token.parse( parseXml( "<token>{bar}</token>" ) );
		assertEquals( "123", t.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse8() throws Exception
	{
		Token t = Token.parse( parseXml( "<token value=\"{bar}\"/>" ) );
		assertEquals( "123", t.value( args ) );
	}
}
