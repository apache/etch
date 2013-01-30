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

import java.util.List;
import java.util.Map;

import org.apache.etch.interoptester.InteropTestIntf;
import org.apache.etch.interoptester.Param;
import org.apache.etch.interoptester.Prog;
import org.apache.etch.interoptester.Program;
import org.apache.etch.interoptester.TestIntf;
import org.apache.etch.util.AssertionException;
import org.apache.etch.util.core.xml.XmlParser;
import org.apache.etch.util.core.xml.XmlParser.TagElement;

import static org.junit.Assert.*;


/** Test of Prog */
public class TestProg
{
	private final TestIntf test = new MyTest();
	
	private TagElement parseXml( String s ) throws Exception
	{
		return new XmlParser().parseOne( s );
	}
	
	/** @throws Exception */
	@org.junit.Test( expected = AssertionException.class )
	public void parse1() throws Exception
	{
		// tag is prog
		Prog.parse( test, parseXml( "<blah/>" ) );
	}
	
	/** @throws Exception */
	@org.junit.Test( expected = IllegalArgumentException.class )
	public void parse2() throws Exception
	{
		// prog tag has no name attribute
		Prog.parse( test, parseXml( "<prog/>" ) );
	}
	
	/** @throws Exception */
	@org.junit.Test( expected = IllegalArgumentException.class )
	public void parse3() throws Exception
	{
		// prog tag has no name attribute
		Prog.parse( test, parseXml( "<prog name=\"\"/>" ) );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void parse4() throws Exception
	{
		Prog p = Prog.parse( test, parseXml( "<prog name=\"foo\"/>" ) );
		assertSame( test, p.test() );
		assertEquals( "foo", p.name() );
		assertEquals( 0, p.args().size() );
	}
	
	/** @throws Exception */
	@org.junit.Test( expected = IllegalArgumentException.class )
	public void parse5() throws Exception
	{
		Prog.parse( test, parseXml( "<prog name=\"foo\"><arg name=\"bar\"/></prog>" ) );
	}
	
	/** @throws Exception */
	@org.junit.Test( expected = IllegalArgumentException.class )
	public void parse6() throws Exception
	{
		// duplicate name arg bar
		Prog.parse( test, parseXml( "<prog name=\"foo\"><arg name=\"bar\"/><arg name=\"bar\"/></prog>" ) );
	}
	
	/** @throws Exception */
	@org.junit.Test( expected = IllegalArgumentException.class )
	public void parse7() throws Exception
	{
		Prog.parse( test, parseXml( "<prog name=\"foo\"><arg name=\"bar\"/><arg name=\"baz\"/></prog>" ) );
	}
	
	private static class MyTest implements TestIntf
	{
		public List<Prog> cleanup()
		{
			// ignore
			return null;
		}

		public InteropTestIntf itest()
		{
			// ignore
			return null;
		}

		public Prog jig()
		{
			// ignore
			return null;
		}

		public String name()
		{
			// ignore
			return null;
		}

		public Map<String, Param> params()
		{
			// ignore
			return null;
		}

		public void run( Map<String, String> args )
		{
			// ignore
		}

		public List<Prog> setup()
		{
			// ignore
			return null;
		}

		public List<Prog> support()
		{
			// ignore
			return null;
		}

		public Program getProgram( String name )
		{
			// ignore
			return null;
		}
	}
}
