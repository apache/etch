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

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertSame;

import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.etch.interoptester.InteropTestIntf;
import org.apache.etch.interoptester.Param;
import org.apache.etch.interoptester.Prog;
import org.apache.etch.interoptester.Program;
import org.apache.etch.interoptester.Run;
import org.apache.etch.interoptester.TestIntf;
import org.apache.etch.util.AssertionException;
import org.apache.etch.util.core.xml.XmlParser;
import org.apache.etch.util.core.xml.XmlParser.TagElement;
import org.junit.Test;


/** */
public class TestRun
{
	private InteropTestIntf itest = new MyInteropTestIntf();
	
	private TagElement parseXml( String s ) throws Exception
	{
		return new XmlParser().parseOne( s );
	}
	
	/** @throws Exception */
	@Test( expected=AssertionException.class )
	public void parse1() throws Exception
	{
		Run.parse( itest, parseXml( "<blah/>" ) );
	}
	
	/** @throws Exception */
	@Test( expected=IllegalArgumentException.class )
	public void parse2() throws Exception
	{
		Run.parse( itest, parseXml( "<run/>" ) );
	}
	
	/** @throws Exception */
	@Test( expected=IllegalArgumentException.class )
	public void parse3() throws Exception
	{
		Run.parse( itest, parseXml( "<run test=\"\"/>" ) );
	}
	
	/** @throws Exception */
	@Test
	public void parse4() throws Exception
	{
		Run r = Run.parse( itest, parseXml( "<run test=\"foo\"/>" ) );
		assertSame( itest, r.itest() );
		assertEquals( "foo", r.test() );
		assertEquals( 0, r.args().size() );
	}
	
	/** @throws Exception */
	@Test
	public void parse5() throws Exception
	{
		Run r = Run.parse( itest, parseXml( "<run test=\"bar\"></run>" ) );
		assertSame( itest, r.itest() );
		assertEquals( "bar", r.test() );
		assertEquals( 0, r.args().size() );
	}
	
	/** @throws Exception */
	@Test( expected=IllegalArgumentException.class )
	public void parse6() throws Exception
	{
		// unknown tag blah in run.
		Run.parse( itest, parseXml( "<run test=\"foo\"><blah/></run>" ) );
	}
	
	/** @throws Exception */
	@Test( expected=IllegalArgumentException.class )
	public void parse7() throws Exception
	{
		// arg tag has no name.
		Run.parse( itest, parseXml( "<run test=\"foo\"><arg/></run>" ) );
	}
	
	/** @throws Exception */
	@Test( expected=IllegalArgumentException.class )
	public void parse8() throws Exception
	{
		// arg tag has no name.
		Run.parse( itest, parseXml( "<run test=\"foo\"><arg name=\"\"/></run>" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void parse9() throws Exception
	{
		// arg a1 has no value
		Run.parse( itest, parseXml( "<run test=\"foo\"><arg name=\"a1\"/></run>" ) );
	}
	
	/** @throws Exception */
	@Test( expected=IllegalArgumentException.class )
	public void parse10() throws Exception
	{
		// duplicate name arg a1
		Run.parse( itest, parseXml( "<run test=\"foo\"><arg name=\"a1\"/><arg name=\"a1\"/></run>" ) );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void parse11() throws Exception
	{
		// arg a1 has no value
		Run.parse( itest, parseXml( "<run test=\"foo\"><arg name=\"a1\"/><arg name=\"a2\"/></run>" ) );
	}
	
	/** @throws Exception */
	@Test( expected=RuntimeException.class )
	public void run1() throws Exception
	{
		// test bad not found
		Run r = Run.parse( itest, parseXml( "<run test=\"bad\"/>" ) );
		r.run( new HashMap<String, String>() );
	}
	
	/** @throws Exception */
	@Test
	public void run2() throws Exception
	{
		Run r = Run.parse( itest, parseXml( "<run test=\"foo\"/>" ) );
		assertNull( test );
		r.run( new HashMap<String, String>() );
		assertEquals( "foo", test.name() );
		assertEquals( 0, test.args.size() );
	}
	
	/** @throws Exception */
	@Test( expected = IllegalArgumentException.class )
	public void run3() throws Exception
	{
		// arg 'abc' has no value
		Run.parse( itest, parseXml( "<run test=\"foo\"><arg name=\"abc\"/></run>" ) );
	}
	
	/** @throws Exception */
	@Test
	public void run4() throws Exception
	{
		Run r = Run.parse( itest, parseXml( "<run test=\"foo\"><arg name=\"abc\" value=\"234\"/></run>" ) );
		assertNull( test );
		r.run( new HashMap<String, String>() );
		assertEquals( "foo", test.name() );
		assertEquals( 1, test.args.size() );
		assertEquals( "234", test.args.get( "abc" ) );
	}
	
	/** @throws Exception */
	@Test
	public void run5() throws Exception
	{
		Run r = Run.parse( itest, parseXml( "<run test=\"foo\"><arg name=\"abc\">345</arg></run>" ) );
		assertNull( test );
		r.run( new HashMap<String, String>() );
		assertEquals( "foo", test.name() );
		assertEquals( 1, test.args.size() );
		assertEquals( "345", test.args.get( "abc" ) );
	}
	
	/** @throws Exception */
	@Test
	public void run6() throws Exception
	{
		Run r = Run.parse( itest, parseXml( "<run test=\"bar\"/>" ) );
		assertNull( test );
		Map<String, String> args = new HashMap<String, String>();
		args.put( "abc", "123" );
		r.run( args );
		assertEquals( "bar", test.name() );
		assertEquals( 0, test.args.size() );
	}
	
	/** @throws Exception */
	@Test
	public void run7() throws Exception
	{
		Run r = Run.parse( itest, parseXml( "<run test=\"bar\"><arg name=\"abc\" value=\"xy\"/></run>" ) );
		assertNull( test );
		Map<String, String> args = new HashMap<String, String>();
		args.put( "p1", "123" );
		r.run( args );
		assertEquals( "bar", test.name() );
		assertEquals( 1, test.args.size() );
		assertEquals( "xy", test.args.get( "abc" ) );
	}
	
	/** @throws Exception */
	@Test
	public void run8() throws Exception
	{
		Run r = Run.parse( itest, parseXml( "<run test=\"bar\"><arg name=\"abc\" value=\"x{p2}y\"/></run>" ) );
		assertNull( test );
		Map<String, String> args = new HashMap<String, String>();
		args.put( "p1", "123" );
		r.run( args );
		assertEquals( "bar", test.name() );
		assertEquals( 1, test.args.size() );
		assertEquals( "x{p2}y", test.args.get( "abc" ) );
	}
	
	/** @throws Exception */
	@Test
	public void run9() throws Exception
	{
		Run r = Run.parse( itest, parseXml( "<run test=\"bar\"><arg name=\"abc\" value=\"x{p1}y\"/></run>" ) );
		assertNull( test );
		Map<String, String> args = new HashMap<String, String>();
		args.put( "p1", "123" );
		r.run( args );
		assertEquals( "bar", test.name() );
		assertEquals( 1, test.args.size() );
		assertEquals( "x123y", test.args.get( "abc" ) );
	}
	
	/** @throws Exception */
	@Test
	public void run10() throws Exception
	{
		Run r = Run.parse( itest, parseXml( "<run test=\"bar\"><arg name=\"abc\" value=\"x\\{p1}y\"/></run>" ) );
		assertNull( test );
		Map<String, String> args = new HashMap<String, String>();
		args.put( "p1", "123" );
		r.run( args );
		assertEquals( "bar", test.name() );
		assertEquals( 1, test.args.size() );
		assertEquals( "x{p1}y", test.args.get( "abc" ) );
	}
	
	/** @throws Exception */
	@Test
	public void run11() throws Exception
	{
		Run r = Run.parse( itest, parseXml( "<run test=\"bar\"><arg name=\"abc\" value=\"x\\\\y\"/></run>" ) );
		assertNull( test );
		Map<String, String> args = new HashMap<String, String>();
		args.put( "p1", "123" );
		r.run( args );
		assertEquals( "bar", test.name() );
		assertEquals( 1, test.args.size() );
		assertEquals( "x\\y", test.args.get( "abc" ) );
	}
	
	private class MyInteropTestIntf implements InteropTestIntf
	{
		public TestIntf getTest( String test )
		{
			if (test.equals( "bad" ))
				return null;
			
			return new MyTest( this, test );
		}

		public void run( Map<String, String> args )
		{
			// ignore.
		}

		public Program getProgram( String name )
		{
			// ignore.
			return null;
		}

		public int nextRunId()
		{
			return ++runId;
		}
		
		private int runId;
	}
	
	private class MyTest implements TestIntf
	{
		public MyTest( InteropTestIntf itest, String name )
		{
			this.itest = itest;
			this.name = name;
		}
		
		private final InteropTestIntf itest;
		
		private final String name;
		
		public InteropTestIntf itest()
		{
			return itest;
		}
		
		public String name()
		{
			return name;
		}

		public void run( Map<String, String> args )
		{
			TestRun.this.test = this;
			this.args = args;
		}
		
		private Map<String, String> args;

		public List<Prog> cleanup()
		{
			// ignore
			return null;
		}

		public Prog jig()
		{
			// ignore
			return null;
		}

		public Map<String, Param> params()
		{
			// ignore
			return null;
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
	
	private MyTest test;
}
