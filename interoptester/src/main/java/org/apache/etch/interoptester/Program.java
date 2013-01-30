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

import java.io.BufferedReader;
import java.io.IOException;
import java.io.InputStream;
import java.io.InputStreamReader;
import java.io.PrintStream;
import java.util.ArrayList;
import java.util.HashMap;
import java.util.List;
import java.util.Map;

import org.apache.etch.util.Assertion;
import org.apache.etch.util.core.xml.XmlParser.TagElement;


/**
 * Program models a program to be run as part of a test. The program has
 * positional parameters, environment, stdin, stdout, stderr, and timeout.
 */
public class Program
{
	/**
	 * @param itest
	 * @param r
	 * @return the parsed Program.
	 */
	public static Program parse( InteropTestIntf itest, TagElement r )
	{
		Assertion.check( r.matches( null, "program" ), "tag is program" );
		
		String name = r.getAttr( null, "name" );
		if (name == null || name.length() == 0)
			throw new IllegalArgumentException( "program tag has no name" );
		
		Program p = new Program( itest, name );
		p.parseBody( r );
		return p;
	}
	
	/**
	 * @param itest
	 * @param name
	 */
	private Program( InteropTestIntf itest, String name )
	{
		this.itest = itest;
		this.name = name;
	}

	private final InteropTestIntf itest;

	private final String name;
	
	/**
	 * @return the containing InteropTestIntf.
	 */
	public InteropTestIntf itest()
	{
		return itest;
	}
	
	/**
	 * @return the name of this Program.
	 */
	public String name()
	{
		return name;
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
				
				if (te.matches( null, "tokens" ))
				{
					parseTokens( te );
					return true;
				}
				
				if (te.matches( null, "environment" ))
				{
					parseEnvironment( te );
					return true;
				}
				
				if (te.matches( null, "stdin" ))
				{
					parseStdin( te );
					return true;
				}
				
				if (te.matches( null, "stdout" ))
				{
					parseStdout( te );
					return true;
				}
				
				if (te.matches( null, "stderr" ))
				{
					parseStderr( te );
					return true;
				}
				
				if (te.matches( null, "stdouttag" ))
				{
					parseStdoutTag( te );
					return true;
				}
				
				if (te.matches( null, "stderrtag" ))
				{
					parseStderrTag( te );
					return true;
				}
				
				if (te.matches( null, "timeout" ))
				{
					parseTimeout( te );
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

	private void parseParam( TagElement te )
	{
		Param p = Param.parse( te );
		params.put( p.name(), p );
	}
	
	private final Map<String, Param> params = new HashMap<String, Param>();

	private void parseTokens( TagElement r )
	{
		new ChildWalker()
		{
			public boolean tag( TagElement te )
			{
				if (te.matches( null, "token" ))
				{
					parseToken( te );
					return true;
				}

				return false;
			}
		}.walk( r );
	}

	private void parseToken( TagElement r )
	{
		tokens.add( Token.parse( r ) );
	}
	
	private final List<Token> tokens = new ArrayList<Token>();

	private void parseEnvironment( TagElement r )
	{
		new ChildWalker()
		{
			public boolean tag( TagElement te )
			{
				if (te.matches( null, "env" ))
				{
					parseEnv( te );
					return true;
				}

				return false;
			}
		}.walk( r );
	}

	private void parseEnv( TagElement r )
	{
		Env e = Env.parse( r );
		envs.put( e.name(), e );
	}
	
	private final Map<String, Env> envs = new HashMap<String, Env>();

	private void parseStdin( TagElement r )
	{
		// TODO Auto-generated method stub
		throw new UnsupportedOperationException( "parseStdin" );
	}

	private void parseStdout( TagElement r )
	{
		// TODO Auto-generated method stub
		throw new UnsupportedOperationException( "parseStdout" );
	}

	private void parseStderr( TagElement r )
	{
		// TODO Auto-generated method stub
		throw new UnsupportedOperationException( "parseStderr" );
	}
	
	private void parseStdoutTag( TagElement r )
	{
		stdoutTag = r.getCdataValue();
	}
	
	private String stdoutTag = "out";
	
	private void parseStderrTag( TagElement r )
	{
		stderrTag = r.getCdataValue();
	}
	
	private String stderrTag = "err";

	private void parseTimeout( TagElement r )
	{
		// TODO Auto-generated method stub
		throw new UnsupportedOperationException( "parseTimeout" );
	}

	/**
	 * @param overrides
	 * @return a RunningProg which allows the running program to be controlled.
	 * @throws Exception 
	 */
	public RunningProg start( Map<String, String> overrides ) throws Exception
	{
		Map<String, String> substs = processOverrides( "start", overrides );
		List<String> tkns = processTokens( substs );
		Map<String, String> nvs = processEnvs( substs );
		//System.out.printf( "Program.start: tokens %s envs %s\n", tkns, nvs );
		DefaultRunningProg rp = new DefaultRunningProg( tkns, nvs, stdoutTag, stderrTag );
		try
		{
			rp.start();
			return rp;
		}
		catch ( Exception e )
		{
			e.printStackTrace();
			try
			{
				rp.stop();
			}
			catch ( InterruptedException e1 )
			{
				e1.printStackTrace();
			}
			throw new Exception( "process did not start" );
		}
	}

	/**
	 * @param overrides
	 * @throws Exception 
	 */
	public void run( Map<String, String> overrides ) throws Exception
	{
		Map<String, String> substs = processOverrides( "run", overrides );
		List<String> tkns = processTokens( substs );
		Map<String, String> nvs = processEnvs( substs );
		//System.out.printf( "Program.run: tokens %s envs %s\n", tkns, nvs );
		
		DefaultRunningProg rp = new DefaultRunningProg( tkns, nvs, stdoutTag, stderrTag );
		try
		{
			rp.start();
		}
		catch ( Exception e )
		{
			e.printStackTrace();
			try
			{
				rp.stop();
			}
			catch ( InterruptedException e1 )
			{
				e1.printStackTrace();
			}
			throw new Exception( "process did not start" );
		}
		
		try
		{
			Integer sts = rp.join();
			if (sts == null || sts != 0)
				throw new Exception( "process died with bad status: "+sts );
		}
		catch ( InterruptedException e )
		{
			e.printStackTrace();
		}
	}

	private Map<String, String> processOverrides( String who,
		Map<String, String> overrides )
	{
		//System.out.println( "Program."+who+": name "+name+", overrides = "+overrides );
		
		Map<String, String> substs = Param.processOverrides( "program", params,
			overrides );
		
		//System.out.println( "Program."+who+": name "+name+", substs = "+substs );
		
		return substs;
	}

	private List<String> processTokens( Map<String, String> substs )
	{
		List<String> ntokens = new ArrayList<String>();
		for (Token t: tokens)
			ntokens.add( t.value( substs ) );
		return ntokens;
	}

	private Map<String, String> processEnvs( Map<String, String> substs )
	{
		Map<String, String> nenvs = new HashMap<String, String>();
		for (Env e: envs.values())
			nenvs.put( e.name(), e.value( substs ) );
		return nenvs;
	}
	
	/**
	 * @author wert
	 *
	 */
	public static class ProcessStreamReader extends Thread
	{
		/**
		 * @param is input stream to read until eof.
		 * @param tag 
		 * @param ps 
		 * @throws Exception 
		 */
		public ProcessStreamReader( InputStream is, PrintStream ps, String tag ) throws Exception
		{
			rdr = new BufferedReader( new InputStreamReader( is ) );
			this.ps = ps;
			this.tag = tag;
		}
		
		private final BufferedReader rdr;
		
		private final PrintStream ps;
		
		private final String tag;
		
		@Override
		public void run()
		{
			try
			{
				String s;
				while ((s = rdr.readLine()) != null)
				{
					synchronized (ps)
					{
						ps.printf( "[%s] %s\n", tag, s );
					}
				}
			}
			catch ( Exception e )
			{
				e.printStackTrace();
			}
			finally
			{
				try
				{
					rdr.close();
				}
				catch ( IOException e )
				{
					e.printStackTrace();
				}
			}
		}
	}
}