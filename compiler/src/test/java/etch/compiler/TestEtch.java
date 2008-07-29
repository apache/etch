/*
 * $Id$
 * 
 * Copyright 2007-2008 Cisco Systems Inc.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

package etch.compiler;

import static org.junit.Assert.assertFalse;
import static org.junit.Assert.assertTrue;

import java.io.ByteArrayInputStream;
import java.io.InputStream;
import java.util.ArrayList;
import java.util.HashSet;
import java.util.List;

import org.junit.Before;
import org.junit.Test;

/**
 * JUnit Test to test Compiler API.
 * 
 */
public class TestEtch
{
	private CmdLineOptions cmdLineOption;

	private String correctEtch = "module foo service  bar {  void hello( int x, int y ) }";

	private String wrongEtch1 = "module foo serviced bar {  void hello( int x, int y ) }";

	private String wrongEtch2 = "module foo service  bar    void hello( int x, int y ) }";

	private String wrongEtch3 = "module foo service  bar {  void hello( int x, in  y ) }";

	/**
	 * This method sets up the CmdLineOptions
	 * 
	 */

	@Before
	public void setup()
	{
		cmdLineOption = new CmdLineOptions();
		cmdLineOption.binding = Backend.BINDING_JAVA;
		HashSet<String> set = new HashSet<String>();
		set.add( Backend.WHAT_ALL );
		cmdLineOption.what = set;
	}

	/**
	 * This test case checks the successful generation of code from a given Etch
	 * String.
	 */
	@Test
	public void compileCorrectEtch()
	{
		TestEtchLogHandler logHandler = doCompile( correctEtch, cmdLineOption );
		assertTrue( logHandler.errorList.isEmpty() );
	}
	
	private static TestEtchLogHandler doCompile( String source, CmdLineOptions options )
	{
		TestEtchLogHandler logHandler = new TestEtch.TestEtchLogHandler();
		logHandler.push( source, null );
		InputStream is1 = new ByteArrayInputStream( source.getBytes() );
		Etch2.doCompile( options, is1, logHandler );
		logHandler.pop();
		return logHandler;
	}

	/**
	 * This test case checks the successful generation of code from a given Etch
	 * String
	 */
	@Test
	public void compileWrongEtch1()
	{
		TestEtchLogHandler logHandler = doCompile( wrongEtch1, cmdLineOption );
		List<String> errorList = logHandler.getErrorList();
		assertFalse( errorList.isEmpty() );
		String errorMsg = errorList.get( 0 );
		assertTrue( errorMsg.contains( "Encountered \"serviced\"" ) );
	}

	/**
	 * This test case checks the successful generation of code from a given Etch
	 * String
	 * 
	 */
	@Test
	public void compileWrongEtch2()
	{
		TestEtchLogHandler logHandler = doCompile( wrongEtch2, cmdLineOption );
		List<String> errorList = logHandler.getErrorList();
		assertFalse( errorList.isEmpty() );
		String errorMsg = errorList.get( 0 );
		assertTrue( errorMsg.contains( "Encountered \"void\"" ) );
	}

	/**
	 * This test case checks the successful generation of code from a given Etch
	 * String
	 * 
	 */

	@Test
	public void compileWrongEtch3()
	{
		TestEtchLogHandler logHandler = doCompile( wrongEtch3, cmdLineOption );
		List<String> errorList = logHandler.getErrorList();
		assertFalse( errorList.isEmpty() );
		String errorMsg = errorList.get( 0 );
		// System.out.println( "errorMsg = "+errorMsg );
		assertTrue( errorMsg.contains( "undefined" ) );
	}

	private static class TestEtchLogHandler extends CompilerLogHandler
	{
		private List<String> errorList = new ArrayList<String>();

		@Override
		public void logMessage( int level, Token token, String msg )
		{
			if (level == LogHandler.LEVEL_ERROR)
			{
				String fmt = "Parsing " + printStack() + ": " + msg;
				errorList.add( String.format( errHeader, fmt ) );
				// System.err.printf(errHeader,fmt);
			}
		}

		/**
		 * Returns the error List
		 * 
		 * @return ArrayList
		 */
		public List<String> getErrorList()
		{
			return errorList;
		}
	}
}
