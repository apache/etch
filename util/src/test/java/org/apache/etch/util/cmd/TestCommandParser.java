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

package org.apache.etch.util.cmd;

import static org.junit.Assert.assertEquals;
import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertNull;
import static org.junit.Assert.assertTrue;

import java.util.ArrayList;
import java.util.List;

import org.junit.Test;

/**
 * Test of CommandParser.
 */
public class TestCommandParser extends org.apache.etch.util.cmd.Program
{
	/**
	 * @param args
	 */
	public static void main( String[] args )
	{
		try
		{
			prog = new TestCommandParser();
			main( prog, args );
		}
		catch ( Throwable e )
		{
			cmdParseError = true;
			e.printStackTrace();
		}
	}
	
	private static TestCommandParser prog;
	
	private static boolean cmdParseError;

	@Override
	protected void defineOptionsAndParameters( CommandParser cp )
		throws Exception
	{
		cp.defineNullOption( "-a|--alpha", "setAlpha",
			"set the alpha option",
			Option.NONE );

		cp.defineNullOption( "-b|--beta", "setBeta",
			"set the beta option",
			Option.NONE );

		cp.defineStringOption( "-f|--foo", "value",
			"setFooValue",
			"specifies the foo value",
			Option.NONE, null, null );

		cp.defineStringOption( "-B|--bar", "value",
			"setBarValue",
			"specifies the bar value",
			Option.NONE, null, null );
		
		cp.defineStringParameter( "one",
			"one",
			"first param",
			false, false, null );
		
		cp.defineStringParameter( "two",
			"two",
			"second param",
			false, false, null );
		
		cp.defineStringParameter( "rest",
			"rest",
			"args",
			false, true, null );
	}

	/**
	 * Command parser method to set the alpha flag.
	 * 
	 * @param cp
	 * @param option
	 * @param token
	 */
	public void setAlpha( CommandParser cp, Option option, String token )
	{
		alpha = true;
	}

	private boolean alpha;

	/**
	 * Command parser method to set the beta flag.
	 * 
	 * @param cp
	 * @param option
	 * @param token
	 */
	public void setBeta( CommandParser cp, Option option, String token )
	{
		beta = true;
	}

	private boolean beta;

	/**
	 * Command parser method to set the foo value.
	 * 
	 * @param cp
	 * @param option
	 * @param token
	 * @param value
	 */
	public void setFooValue( CommandParser cp, Option option, String token,
		String value )
	{
		fooValue = value;
	}

	private String fooValue;

	/**
	 * Command parser method to set the bar value.
	 * 
	 * @param cp
	 * @param option
	 * @param token
	 * @param value
	 */
	public void setBarValue( CommandParser cp, Option option, String token,
		String value )
	{
		barValue = value;
	}

	private String barValue;
	
	/**
	 * @param cp
	 * @param param
	 * @param value
	 */
	public void one( CommandParser cp, Parameter param, String value )
	{
		one = value;
	}
	
	private String one;
	
	/**
	 * @param cp
	 * @param param
	 * @param value
	 */
	public void two( CommandParser cp, Parameter param, String value )
	{
		two = value;
	}
	
	private String two;
	
	/**
	 * @param cp
	 * @param param
	 * @param value
	 */
	public void rest( CommandParser cp, Parameter param, String value )
	{
		rest.add( value );
	}
	
	private List<String> rest = new ArrayList<String>();
	
	@Override
	public void exit( int code, String source, String reason, boolean emergency )
	{
		exitCode = code;
	}
	
	private int exitCode;

	@Override
	protected void run() throws Exception
	{
		run = true;
	}

	private boolean run;
	
	/** @throws Exception */
	@Test
	public void nothing() throws Exception
	{
		main( new String[] {} );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertFalse( prog.alpha );
		assertFalse( prog.beta );
		assertNull( prog.fooValue );
		assertNull( prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void short1() throws Exception
	{
		main( new String[] { "-a" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertTrue( prog.alpha );
		assertFalse( prog.beta );
		assertNull( prog.fooValue );
		assertNull( prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void short2() throws Exception
	{
		main( new String[] { "-b" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertFalse( prog.alpha );
		assertTrue( prog.beta );
		assertNull( prog.fooValue );
		assertNull( prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void short3a() throws Exception
	{
		main( new String[] { "-f", "abc" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertFalse( prog.alpha );
		assertFalse( prog.beta );
		assertEquals( "abc", prog.fooValue );
		assertNull( prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void short3b() throws Exception
	{
		main( new String[] { "-fabc" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertFalse( prog.alpha );
		assertFalse( prog.beta );
		assertEquals( "abc", prog.fooValue );
		assertNull( prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void short4a() throws Exception
	{
		main( new String[] { "-B", "def" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertFalse( prog.alpha );
		assertFalse( prog.beta );
		assertNull( prog.fooValue );
		assertEquals( "def", prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void short4b() throws Exception
	{
		main( new String[] { "-Bdef" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertFalse( prog.alpha );
		assertFalse( prog.beta );
		assertNull( prog.fooValue );
		assertEquals( "def", prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void allshort1() throws Exception
	{
		main( new String[] { "-abf", "abc" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertTrue( prog.alpha );
		assertTrue( prog.beta );
		assertEquals( "abc", prog.fooValue );
		assertNull( prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void allshort2() throws Exception
	{
		main( new String[] { "-abfabc" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertTrue( prog.alpha );
		assertTrue( prog.beta );
		assertEquals( "abc", prog.fooValue );
		assertNull( prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void long1() throws Exception
	{
		main( new String[] { "--alpha" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertTrue( prog.alpha );
		assertFalse( prog.beta );
		assertNull( prog.fooValue );
		assertNull( prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void long2() throws Exception
	{
		main( new String[] { "--beta" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertFalse( prog.alpha );
		assertTrue( prog.beta );
		assertNull( prog.fooValue );
		assertNull( prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void long3a() throws Exception
	{
		main( new String[] { "--foo", "abc" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertFalse( prog.alpha );
		assertFalse( prog.beta );
		assertEquals( "abc", prog.fooValue );
		assertNull( prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void long3b() throws Exception
	{
		main( new String[] { "--foo", "" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertFalse( prog.alpha );
		assertFalse( prog.beta );
		assertEquals( "", prog.fooValue );
		assertNull( prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void long3c() throws Exception
	{
		main( new String[] { "--foo=abc" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertFalse( prog.alpha );
		assertFalse( prog.beta );
		assertEquals( "abc", prog.fooValue );
		assertNull( prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void long3d() throws Exception
	{
		main( new String[] { "--foo=" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertFalse( prog.alpha );
		assertFalse( prog.beta );
		assertEquals( "", prog.fooValue );
		assertNull( prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void long4a() throws Exception
	{
		main( new String[] { "--bar", "def" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertFalse( prog.alpha );
		assertFalse( prog.beta );
		assertNull( prog.fooValue );
		assertEquals( "def", prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void long4b() throws Exception
	{
		main( new String[] { "--bar=def" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertFalse( prog.alpha );
		assertFalse( prog.beta );
		assertNull( prog.fooValue );
		assertEquals( "def", prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void alllong1() throws Exception
	{
		main( new String[] { "--alpha", "--beta", "--foo", "abc", "--bar", "def" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertTrue( prog.alpha );
		assertTrue( prog.beta );
		assertEquals( "abc", prog.fooValue );
		assertEquals( "def", prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void alllong2() throws Exception
	{
		main( new String[] { "--alpha", "--beta", "--foo=abc", "--bar=def" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertTrue( prog.alpha );
		assertTrue( prog.beta );
		assertEquals( "abc", prog.fooValue );
		assertEquals( "def", prog.barValue );
	}
	
	/** @throws Exception */
	@Test
	public void param1() throws Exception
	{
		main( new String[] { "abc" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertEquals( "abc", prog.one );
		assertEquals( null, prog.two );
		assertEquals( 0, prog.rest.size() );
	}
	
	/** @throws Exception */
	@Test
	public void param2() throws Exception
	{
		main( new String[] { "abc", "def" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertEquals( "abc", prog.one );
		assertEquals( "def", prog.two );
		assertEquals( 0, prog.rest.size() );
	}
	
	/** @throws Exception */
	@Test
	public void param3() throws Exception
	{
		main( new String[] { "abc", "def", "ghi" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertEquals( "abc", prog.one );
		assertEquals( "def", prog.two );
		assertEquals( 1, prog.rest.size() );
		assertEquals( "ghi", prog.rest.get( 0 ) );
	}
	
	/** @throws Exception */
	@Test
	public void param4() throws Exception
	{
		main( new String[] { "abc", "def", "ghi", "jkl" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertEquals( "abc", prog.one );
		assertEquals( "def", prog.two );
		assertEquals( 2, prog.rest.size() );
		assertEquals( "ghi", prog.rest.get( 0 ) );
		assertEquals( "jkl", prog.rest.get( 1 ) );
	}
	
	/** @throws Exception */
	@Test
	public void mixed1() throws Exception
	{
		main( new String[] { "abc", "-a", "def", "ghi", "-f", "123", "jkl" } );
		assertTrue( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 0, prog.exitCode );
		assertTrue( prog.alpha );
		assertFalse( prog.beta );
		assertEquals( "123", prog.fooValue );
		assertEquals( null, prog.barValue );
		assertEquals( "abc", prog.one );
		assertEquals( "def", prog.two );
		assertEquals( 2, prog.rest.size() );
		assertEquals( "ghi", prog.rest.get( 0 ) );
		assertEquals( "jkl", prog.rest.get( 1 ) );
	}
	
	/** @throws Exception */
	@Test
	public void error1() throws Exception
	{
		// unknown short option
		main( new String[] { "-x" } );
		assertFalse( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 1, prog.exitCode );
	}
	
	/** @throws Exception */
	@Test
	public void error2() throws Exception
	{
		// unknown long option
		main( new String[] { "--x" } );
		assertFalse( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 1, prog.exitCode );
	}
	
	/** @throws Exception */
	@Test
	public void error3() throws Exception
	{
		// missing value for short option
		main( new String[] { "-f" } );
		assertFalse( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 1, prog.exitCode );
	}
	
	/** @throws Exception */
	@Test
	public void error4() throws Exception
	{
		// missing value for long option
		main( new String[] { "--foo" } );
		assertFalse( prog.run );
		assertFalse( cmdParseError );
		assertEquals( 1, prog.exitCode );
	}
}
