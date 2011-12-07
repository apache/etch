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

package org.apache.etch.compiler;

import java.io.PrintStream;

/**
 * This class implements the LogHandler Interface for Compiler
 * @author gsandhir
 *
 */
public class ConsoleLogHandler extends AbstractLogHandler
{
	/**
	 * Constructs the CompilerLogHandler and sets the initial phase.
	 * @param phase
	 */
	public ConsoleLogHandler( String phase )
	{
		super( phase );
	}
	
	/**
	 * Constructs the CompilerLogHandler.
	 */
	public ConsoleLogHandler()
	{
		super();
	}

	@Override
	protected void log( Message msg )
	{
		PrintStream ps = selectPrintStream( msg.level );
		
		// Etch : Level : Phase : Source [line Line] : msg
		
		ps.print( "Etch" );		
		formatLevel( ps, msg.level );
		formatPhase( ps, msg.phase );
		if (msg.source != null) {
			formatSource( ps, msg.source, msg.lineNumber );
		}
		formatMsg( ps, msg.msg );
		ps.println();
		
		formatHistory( msg.history );
	}

	private final static String SEP = " : ";

	private void formatLevel( PrintStream ps, int level )
	{
		ps.print( SEP );
		switch (level)
		{
			case LEVEL_INFO:
				ps.print( "I" );
				break;
			case LEVEL_WARNING:
				ps.print( "W" );
				break;
			case LEVEL_ERROR:
				ps.print( "E" );
				break;
			default:
				ps.print( "?" );
				break;
		}
	}

	private void formatPhase( PrintStream ps, String phase )
	{
		ps.print( SEP );
		if (phase != null && phase.length() > 0)
			ps.print( phase );
	}

	private void formatSource( PrintStream ps, String source, Integer lineNumber )
	{
		if (source == null)
			throw new NullPointerException( "source == null" );
		
		ps.print( SEP );
		if (lineNumber != null && lineNumber != 0)
			ps.printf( "%s line %d", source, lineNumber.intValue() );
		else
			ps.print( source );
	}
	
	private void formatMsg( PrintStream ps, String msg )
	{
		ps.print( SEP );
		ps.print( msg );
	}

	private void formatHistory( History[] history )
	{
		// TODO implement formatHistory
	}

	private PrintStream selectPrintStream( int level )
	{
		switch (level)
		{
			case LEVEL_ERROR:
				return System.err;
			default:
				return System.out;
		}
	}
}
