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

import org.apache.etch.util.Assertion;
import org.apache.etch.util.core.xml.XmlParser.TagElement;


/**
 * Model of a test run. Run identifies the Test to be run and the arguments
 * (name / value pairs) which will be supplied to the Test when it is run.
 */
public class Run
{
	/**
	 * @param itest 
	 * @param r the xml tag which defines this Run.
	 * @return the parsed Run object.
	 */
	public static Run parse( InteropTestIntf itest, TagElement r )
	{
		Assertion.check( r.matches( null, "run" ), "tag is run" );
		
		String name = r.getAttr( null, "name" );
		if (name == null || name.length() == 0)
			name = "run-"+itest.nextRunId();
		
		String test = r.getAttr( null, "test" );
		if (test == null || test.length() == 0)
			throw new IllegalArgumentException( "run tag has no test attribute" );
		
		Run t = new Run( itest, name, test );
		t.parseBody( r );
		return t;
	}

	/**
	 * @param itest
	 * @param test
	 */
	private Run( InteropTestIntf itest, String name, String test )
	{
		this.itest = itest;
		this.name = name;
		this.test = test;
	}

	private final InteropTestIntf itest;
	
	private final String name;
	
	private final String test;
	
	/**
	 * @return the containing InteropTestIntf.
	 */
	public InteropTestIntf itest()
	{
		return itest;
	}
	
	/**
	 * @return the name of this run.
	 */
	public String name()
	{
		return name;
	}

	/**
	 * @return the test name.
	 */
	public String test()
	{
		return test;
	}
	
	@Override
	public String toString()
	{
		return "Run ("+name+")";
	}
	
	private void parseBody( TagElement r )
	{
		new ChildWalker()
		{
			public boolean tag( TagElement te )
			{
				if (te.matches( null, "arg" ))
				{
					Arg a = Arg.parse( te );
					if (args.containsKey( a.name() ))
						throw new IllegalArgumentException( "duplicate name arg "+a.name() );
					args.put( a.name(), a );
					return true;
				}
				
				if (te.matches( null, "disable" ))
				{
					if (te.countChildren() > 0)
						disable = Boolean.valueOf( te.getCdataValue() );
					else
						disable = true;
					//System.out.println( "run "+name+" test "+test+", disable = "+disable );
					return true;
				}
				
				return false;
			}
		}.walk( r );
	}
	
	private boolean disable;
	
	private final Map<String, Arg> args = new HashMap<String, Arg>();

	/**
	 * Runs the test specified by this Run.
	 * @param substs params from interoptest to use for substitutions into this
	 * Run's args. The args are then used to override the params of the Run's
	 * test.
	 * @throws Exception 
	 */
	public void run( Map<String, String> substs ) throws Exception
	{
		try
		{
			TestIntf t = itest.getTest( test );
			if (t == null)
				throw new IllegalArgumentException( "test "+test+" not found" );
			
			System.out.println( "---------------------------------------------------" );
			
			if (disable)
			{
				System.out.println( "Run.run: name "+name+" test "+test+", disabled" );
				return;
			}
			
			System.out.println( "Run.run: name "+name+" test "+test+", substs = "+substs );
			
			Map<String, String> overrides = Arg.processSubst( "run", args, substs );
			
			System.out.println( "Run.run: name "+name+" test "+test+", overrides = "+overrides );
			
			t.run( overrides );
			
			System.out.println( "Run.run: name "+name+" test "+test+", done" );
		}
		catch ( RuntimeException e )
		{
			throw new RuntimeException( "Run.run: name "+name+" test "+test+", caught exception", e );
		}
	}

	/**
	 * @return the map of args for this test run.
	 */
	public Map<String, Arg> args()
	{
		return args;
	}
}