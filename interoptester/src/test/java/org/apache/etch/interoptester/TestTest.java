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

import static org.junit.Assert.*;


import java.util.HashMap;
import java.util.Map;

import org.apache.etch.interoptester.InteropTestIntf;
import org.apache.etch.interoptester.Program;
import org.apache.etch.interoptester.Test;
import org.apache.etch.interoptester.TestIntf;
import org.apache.etch.util.AssertionException;
import org.apache.etch.util.core.xml.XmlParser;
import org.apache.etch.util.core.xml.XmlParser.TagElement;


/** Test of Test */
public class TestTest
{
	private final InteropTestIntf itest = new MyInteropTest();
	
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
	@org.junit.Test( expected = AssertionException.class )
	public void parse1() throws Exception
	{
		// tag is test
		Test.parse( itest, parseXml( "<blah/>" ) );
	}
	
	/** @throws Exception */
	@org.junit.Test( expected = IllegalArgumentException.class )
	public void parse2() throws Exception
	{
		// test tag has not name attribute
		Test.parse( itest, parseXml( "<test/>" ) );
	}
	
	/** @throws Exception */
	@org.junit.Test( expected = IllegalArgumentException.class )
	public void parse3() throws Exception
	{
		// no jig defined for test: foo
		Test.parse( itest, parseXml( "<test name=\"foo\"/>" ) );
	}
	
	/** @throws Exception */
	@org.junit.Test( expected = IllegalArgumentException.class )
	public void parse4() throws Exception
	{
		// no jig defined for test: foo
		Test.parse( itest, parseXml( "<test name=\"foo\"><jig></jig></test>" ) );
	}
	
	/** @throws Exception */
	@org.junit.Test( expected = IllegalArgumentException.class )
	public void parse5() throws Exception
	{
		// only one test jig per test
		Test.parse( itest, parseXml(
			"<test name=\"foo\"><jig><prog name=\"bar\"/><prog name=\"baz\"/></jig></test>" ) );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void parse6() throws Exception
	{
		Test t = Test.parse( itest, parseXml(
			"<test name=\"foo\"><jig><prog name=\"bar\"/></jig></test>" ) );
		assertSame( itest, t.itest() );
		assertEquals( "foo", t.name() );
		assertEquals( 0, t.params().size() );
		assertEquals( 0, t.setup().size() );
		assertEquals( 0, t.support().size() );
		assertEquals( "bar", t.jig().name() );
		assertEquals( 0, t.cleanup().size() );
	}
	
	/** @throws Exception */
	@org.junit.Test
	public void parse7() throws Exception
	{
		Test t = Test.parse( itest, parseXml(
			"<test name=\"bar\"><params><param name=\"param1\"/><param name=\"param2\"/></params><setup><prog name=\"setup1\"/><prog name=\"setup2\"/></setup><support><prog name=\"support1\"/><prog name=\"support2\"/></support><jig><prog name=\"jig1\"/></jig><cleanup><prog name=\"cleanup1\"/><prog name=\"cleanup2\"/></cleanup></test>" ) );
		assertSame( itest, t.itest() );
		assertEquals( "bar", t.name() );
		assertEquals( 2, t.params().size() );
		assertTrue( t.params().containsKey( "param1" ) );
		assertTrue( t.params().containsKey( "param2" ) );
		assertEquals( 2, t.setup().size() );
		assertEquals( "setup1", t.setup().get( 0 ).name() );
		assertEquals( "setup2", t.setup().get( 1 ).name() );
		assertEquals( 2, t.support().size() );
		assertEquals( "support1", t.support().get( 0 ).name() );
		assertEquals( "support2", t.support().get( 1 ).name() );
		assertEquals( "jig1", t.jig().name() );
		assertEquals( 2, t.cleanup().size() );
		assertEquals( "cleanup1", t.cleanup().get( 0 ).name() );
		assertEquals( "cleanup2", t.cleanup().get( 1 ).name() );
	}
	
	private static class MyInteropTest implements InteropTestIntf
	{
		public TestIntf getTest( String test )
		{
			// ignore
			return null;
		}

		public void run( Map<String, String> args )
		{
			// ignore
		}

		public Program getProgram( String name )
		{
			// ignore
			return null;
		}

		public int nextRunId()
		{
			return ++runId;
		}
		
		private int runId;
	}
}
