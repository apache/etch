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

import org.apache.etch.util.Assertion;
import org.apache.etch.util.core.xml.XmlParser.TagElement;


/**
 * Test models a test that can be run. A Test has a name (used by a Run to
 * specify the test), includes some parameter values and some programs to be
 * run for setup, support, the test jig itself, and then cleanup.
 */
public class Test implements TestIntf
{
	/**
	 * @param itest
	 * @param r
	 * @return the parsed Test.
	 */
	public static Test parse( InteropTestIntf itest, TagElement r )
	{
		Assertion.check( r.matches( null, "test" ), "tag is test" );
		
		String name = r.getAttr( null, "name" );
		if (name == null || name.length() == 0)
			throw new IllegalArgumentException( "test tag has no name attribute" );
		
		Test t = new Test( itest, name );
		t.parseBody( r );
		return t;
	}

	/**
	 * @param itest
	 * @param name
	 */
	private Test( InteropTestIntf itest, String name )
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
	
	private void parseBody( TagElement r )
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
				
				if (te.matches( null, "setup" ))
				{
					parseSetup( te );
					return true;
				}
				
				if (te.matches( null, "support" ))
				{
					parseSupport( te );
					return true;
				}
				
				if (te.matches( null, "jig" ))
				{
					parseJig( te );
					return true;
				}
				
				if (te.matches( null, "cleanup" ))
				{
					parseCleanup( te );
					return true;
				}
				
				return false;
			}
		}.walk( r );
		
		if (jig == null)
			throw new IllegalArgumentException( "no jig defined for test: "+name );
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

	private void parseParam( TagElement r )
	{
		Param p = Param.parse( r );
		params.put( p.name(), p );
	}
	
	private final Map<String, Param> params = new HashMap<String, Param>();

	private void parseSetup( TagElement r )
	{
		parseProgList( setup, r );
	}
	
	private final List<Prog> setup = new ArrayList<Prog>();

	private void parseSupport( TagElement r )
	{
		parseProgList( support, r );
	}
	
	private final List<Prog> support = new ArrayList<Prog>();

	private void parseJig( TagElement r )
	{
		List<Prog> jigs = new ArrayList<Prog>();
		parseProgList( jigs, r );
		if (jigs.size() != 1)
			throw new IllegalArgumentException( "only one test jig per test" );
		jig = jigs.get( 0 );
	}
	
	private Prog jig;

	private void parseCleanup( TagElement r )
	{
		parseProgList( cleanup, r );
	}

	private final List<Prog> cleanup = new ArrayList<Prog>();
	
	private void parseProgList( final List<Prog> list, TagElement r )
	{
		new ChildWalker()
		{
			public boolean tag( TagElement te )
			{
				if (te.matches( null, "prog" ))
				{
					parseProg( list, te );
					return true;
				}
				
				return false;
			}
		}.walk( r );
	}

	private void parseProg( List<Prog> list, TagElement r )
	{
		list.add( Prog.parse( this, r ) );
	}

	public void run( Map<String, String> overrides ) throws Exception
	{
		//System.out.println( "Test.run: "+name+", overrides = "+overrides );
		
		Map<String, String> substs = Param.processOverrides( "test", params,
			overrides );
		
		//System.out.println( "Test.run: "+name+", substs = "+substs );
		
		runProgs( setup, substs );
		try
		{
			List<RunningProg> runningProgs = startProgs( support, substs );
			try
			{
				runProg( jig, substs );
			}
			finally
			{
				stopProgs( runningProgs );
			}
		}
		finally
		{
			runProgs( cleanup, substs );
		}
	}

	private void runProgs( List<Prog> progs, Map<String, String> substs ) throws Exception
	{
		for (Prog prog: progs)
			runProg( prog, substs );
	}

	private List<RunningProg> startProgs( List<Prog> progs,
		Map<String, String> substs ) throws Exception
	{
		List<RunningProg> runningProgs = new ArrayList<RunningProg>();
		for (Prog prog: progs)
			runningProgs.add( startProg( prog, substs ) );
		return runningProgs;
	}

	private void stopProgs( List<RunningProg> runningProgs ) throws Exception
	{
		for (RunningProg rp: runningProgs)
			stopProg( rp );
	}

	private void runProg( Prog prog, Map<String, String> substs ) throws Exception
	{
		prog.run( substs );
	}

	private RunningProg startProg( Prog prog, Map<String, String> substs ) throws Exception
	{
		return prog.start( substs );
	}

	private void stopProg( RunningProg rp ) throws Exception
	{
		rp.stop();
	}

	public Prog jig()
	{
		return jig;
	}

	public List<Prog> cleanup()
	{
		return cleanup;
	}

	public List<Prog> setup()
	{
		return setup;
	}

	public List<Prog> support()
	{
		return support;
	}

	public Map<String, Param> params()
	{
		return params;
	}

	public Program getProgram( String name )
	{
		return itest.getProgram( name );
	}
}