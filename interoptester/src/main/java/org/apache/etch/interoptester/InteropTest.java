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

import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.etch.util.core.xml.XmlParser.TagElement;


/** */
public class InteropTest implements InteropTestIntf
{
	static InteropTestIntf parse( TagElement r )
	{
		if (!r.matches( null, "interoptest" ))
			return null;
		
		InteropTest it = new InteropTest();
		it.parseBody( r );
		return it;
	}
	
	/**
	 * @param r
	 */
	public void parseBody( TagElement r )
	{
		new ChildWalker()
		{
			public boolean tag( TagElement te )
			{
				if (te.matches( null, "params" ))
				{
					parseParams( te );
					return true;
				}
				
				if (te.matches( null, "programs" ))
				{
					parsePrograms( te );
					return true;
				}
				
				if (te.matches( null, "tests" ))
				{
					parseTests( te );
					return true;
				}
				
				if (te.matches( null, "run" ))
				{
					parseRun( te );
					return true;
				}
				
				return false;
			}
		}.walk( r );
	}
	
	private void parseParams( TagElement r )
	{
		new ChildWalker()
		{
			public boolean tag( TagElement te )
			{
				if (te.matches( null, "param" ))
				{
					parseParam( te );
					return true;
				}
				
				return false;
			}
		}.walk( r );
	}
	
	private void parsePrograms( TagElement r )
	{
		new ChildWalker()
		{
			public boolean tag( TagElement te )
			{
				if (te.matches( null, "program" ))
				{
					parseProgram( te );
					return true;
				}
				
				return false;
			}
		}.walk( r );
	}

	private void parseTests( TagElement r )
	{
		new ChildWalker()
		{
			public boolean tag( TagElement te )
			{
				if (te.matches( null, "test" ))
				{
					parseTest( te );
					return true;
				}
				
				return false;
			}
		}.walk( r );
	}
	
	private void parseParam( TagElement r )
	{
		Param p = Param.parse( r );
		params.put( p.name(), p );
	}
	
	private final Map<String, Param> params = new HashMap<String, Param>();

	private void parseProgram( TagElement r )
	{
		Program p = Program.parse( this, r );
		programs.put( p.name(), p );
	}
	
	private final Map<String, Program> programs = new HashMap<String, Program>();

	private void parseTest( TagElement r )
	{
		Test t = Test.parse( this, r );
		tests.put( t.name(), t );
	}
	
	final Map<String, Test> tests = new HashMap<String, Test>();

	private void parseRun( TagElement r )
	{
		runs.add( Run.parse( this, r ) );
	}
	
	private final List<Run> runs = new ArrayList<Run>();

	public void run( Map<String, String> overrides ) throws Exception
	{
		//System.out.println( "InteropTest.run: overrides = "+overrides );
		
		// construct substs out of params + overrides
		
		Map<String, String> substs = Param.processOverrides( "interoptest",
			params, overrides );
		
		//System.out.println( "InteropTest.run: substs = "+substs );
		
		// run each "run" defined in interoptest
		
		for (Run r: runs)
			r.run( substs );
	}

	public TestIntf getTest( String test )
	{
		return tests.get( test );
	}

	public Program getProgram( String name )
	{
		return programs.get( name );
	}

	public int nextRunId()
	{
		return ++runId;
	}
	
	private int runId;
}