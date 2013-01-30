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

/**
 * Description of Program
 */
abstract public class Program
{
	/**
	 * @param program
	 * @param args
	 * @throws Exception
	 */
	protected static void main( final Program program, String[] args )
		throws Exception
	{
		CommandParser cp = new CommandParser( program );
		
		cp.defineNullOption( program.getHelpTokens(), "doHelp",
			"print help message and exit.",
			Option.NONE );
		
		program.defineOptionsAndParameters( cp );
		
		if (!cp.parse( args ))
		{
			program.exit( 1, program.toString(), "cmdLineParseFailure", true );
			return;
		}
		
		Runtime.getRuntime().addShutdownHook(
			new Thread()
			{
				@Override
				public void run()
				{
					program.shutdown( program.shutdownSource,
						program.shutdownReason, program.shutdownEmergency );
				}
			});
		
		program.run();
	}

	/**
	 * @param code the exit code.
	 * @param source the source of the request.
	 * @param reason the reason for the request.
	 * @param emergency perform minimal shutdown if true.
	 */
	public void exit( int code, String source, String reason,
		boolean emergency )
	{
		shutdownSource = source;
		shutdownReason = reason;
		shutdownEmergency = emergency;
		System.exit( code );
	}
	
	/**
	 * The source of the request.
	 */
	String shutdownSource = "runtime";
	
	/**
	 * The reason for the request.
	 */
	String shutdownReason = "unknown";
	
	/**
	 * Full shutdown (false) or minimal (true)?
	 */
	boolean shutdownEmergency = false;

	/**
	 * @return the list of tokens the user might specify to get help.
	 * The default list of tokens is "-h|--help".
	 */
	protected String getHelpTokens()
	{
		return "-h|--help";
	}

	/**
	 * Prints the help message for the program.
	 * @param cp
	 * @param option
	 * @param token
	 * @return false indicating the program should exit.
	 */
	public boolean doHelp( CommandParser cp, Option option, String token )
	{
		cp.showHelp( false );
		return false;
	}

	@Override
	public String toString()
	{
		return getClass().getName().toString();
	}
	
	/**
	 * Gives the program a chance to define its options and parameters.
	 * @param cp
	 * @throws Exception
	 */
	abstract protected void defineOptionsAndParameters( CommandParser cp )
		throws Exception;
	
	/**
	 * Runs the program after successful command line parsing.
	 * @throws Exception
	 */
	abstract protected void run() throws Exception;
	
	/**
	 * Shuts down the program in an orderly way when System.exit is called,
	 * or when the program is killed by external command (^C) or unix kill.
	 * @param source the source of the request
	 * @param reason the reason for the shutdown
	 * @param emergency only a minimal amount of shutdown should be performed
	 */
	protected void shutdown( String source, String reason, boolean emergency )
	{
		// nothing to do.
	}
}
