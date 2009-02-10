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

import org.apache.etch.interoptester.Arg;
import org.apache.etch.util.AssertionException;
import org.apache.etch.util.core.xml.XmlParser;
import org.apache.etch.util.core.xml.XmlParser.TagElement;
import org.junit.Test;
import static org.junit.Assert.*;


/**
 * Test of Arg.
 */
public class TestArg
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
		// tag is arg
		Arg.parse( parseXml( "<foo/>" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void parse2() throws Exception
	{
		// arg tag has no name
		Arg.parse( parseXml( "<arg/>" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void parse3() throws Exception
	{
		// arg tag has no name
		Arg.parse( parseXml( "<arg name=\"\"/>" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void parse4() throws Exception
	{
		// arg 'foo' has no value
		Arg.parse( parseXml( "<arg name=\"foo\"/>" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void parse5() throws Exception
	{
		// arg 'foo' has no value
		Arg.parse( parseXml( "<arg name=\"foo\"></arg>" ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse6() throws Exception
	{
		Arg a = Arg.parse( parseXml( "<arg name=\"foo\">abc</arg>" ) );
		assertEquals( "foo", a.name() );
		assertEquals( "abc", a.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse7() throws Exception
	{
		Arg a = Arg.parse( parseXml( "<arg name=\"foo\" value=\"\"/>" ) );
		assertEquals( "foo", a.name() );
		assertEquals( "", a.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse8() throws Exception
	{
		Arg a = Arg.parse( parseXml( "<arg name=\"foo\" value=\"abc\"/>" ) );
		assertEquals( "foo", a.name() );
		assertEquals( "abc", a.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse9() throws Exception
	{
		Arg a = Arg.parse( parseXml( "<arg name=\"fu\">{bar}</arg>" ) );
		assertEquals( "fu", a.name() );
		assertEquals( "123", a.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse10() throws Exception
	{
		Arg a = Arg.parse( parseXml( "<arg name=\"fu\" value=\"{bar}\"/>" ) );
		assertEquals( "fu", a.name() );
		assertEquals( "123", a.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse11() throws Exception
	{
		Arg a = Arg.parse( parseXml( "<arg name=\"tofu\">x{baz}y</arg>" ) );
		assertEquals( "tofu", a.name() );
		assertEquals( "x456y", a.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse12() throws Exception
	{
		Arg a = Arg.parse( parseXml( "<arg name=\"tofu\" value=\"x{baz}y\"/>" ) );
		assertEquals( "tofu", a.name() );
		assertEquals( "x456y", a.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse13() throws Exception
	{
		Arg a = Arg.parse( parseXml( "<arg name=\"snafu\">x{barf}y</arg>" ) );
		assertEquals( "snafu", a.name() );
		assertEquals( "x{barf}y", a.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse14() throws Exception
	{
		Arg a = Arg.parse( parseXml( "<arg name=\"snafu\" value=\"x{barf}y\"/>" ) );
		assertEquals( "snafu", a.name() );
		assertEquals( "x{barf}y", a.value( args ) );
	}
	
	/** @throws Exception */
	@Test
	public void toString1() throws Exception
	{
		Arg a = Arg.parse( parseXml( "<arg name=\"tofu\" value=\"x{baz}y\"/>" ) );
		assertEquals( "Arg( tofu, x{baz}y )", a.toString() );
	}
}
