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

import java.util.Stack;

import org.apache.etch.util.Assertion;



/**
 * Abstract implementation of LogHandler. Subclasser must implement
 * final disposition of the message.
 */
abstract public class AbstractLogHandler implements LogHandler
{
	protected AbstractLogHandler()
	{
		this( null );
	}
	
	protected AbstractLogHandler( String initialPhase )
	{
		setPhase( initialPhase );
	}
	
	private String phase;

	private int errorCount;

	private int warningCount;

	private String source;

	private int logLevel = LEVEL_INFO;

	private Stack<LogHandler.History> history = new Stack<LogHandler.History>();

	final public int getLevel()
	{
		return logLevel;
	}

	final public void setLevel( int level )
	{
		checkLevel( level );
		logLevel = level;
	}

	final public String getPhase()
	{
		return phase;
	}

	final public void setPhase( String phase )
	{
		this.phase = phase;
	}

	final public boolean hasError()
	{
		return errorCount > 0;
	}

	final public int errorCount()
	{
		return errorCount;
	}

	final public boolean hasWarning()
	{
		return warningCount > 0;
	}

	final public int warningCount()
	{
		return warningCount;
	}

	final public void report( int level, String fmt, Object ... params )
	{
		report( level, null, fmt, params );
	}

	final public void report( int level, Integer lineNumber, String fmt,
		Object ... params )
	{
		report( level, lineNumber, String.format( fmt, params ) );
	}

	final public void logMessage( int level, Token token, String msg )
	{
		report( level, token.beginLine, msg );
	}

	final public void report( int level, Integer lineNumber, String msg )
	{
		checkLevel( level );
		
		if (level == LEVEL_ERROR)
			errorCount++;
		
		if (level == LEVEL_WARNING)
			warningCount++;
		
		if (isInteresting( level ))
			log( new Message( level, phase, source, lineNumber, msg.trim(),
				getHistory() ) );
	}

	/**
	 * Logs a message which has already been deemed interesting based on
	 * its level. Subclasser might print the message or put it into a list.
	 * @param message
	 */
	abstract protected void log( Message message );

	private LogHandler.History[] getHistory()
	{
		if (history.isEmpty())
			return null;
		
		return history.toArray( new LogHandler.History[history.size()] );
	}

	private void checkLevel( int level )
	{
		Assertion.check( LEVEL_IMPORTANT < LEVEL_INFO,
			"LEVEL_IMPORTANT < LEVEL_INFO" );
		
		if (level < LEVEL_IMPORTANT || level > LEVEL_INFO)
			throw new IllegalArgumentException(
				"level < LEVEL_IMPORTANT || level > LEVEL_INFO" );
	}

	final public boolean isInteresting( int level )
	{
		Assertion.check( level >= LEVEL_IMPORTANT && level <= LEVEL_INFO,
			"level >= LEVEL_IMPORTANT && level <= LEVEL_INFO" );
		return level <= logLevel;
	}

	final public void pop( String oldSource )
	{
		if (oldSource == null || oldSource.length() == 0)
			throw new IllegalArgumentException(
				"oldSource is null or empty" );
		
		if (!oldSource.equals( source ))
			throw new IllegalArgumentException(
				"oldSource does not match last push" );
		
		if (!history.isEmpty())
		{
			LogHandler.History h = history.pop();
			source = h.source;
		}
		else
		{
			source = null;
		}
	}

	final public void push( String newSource, Integer lineNumberInOldSource )
	{
		if (newSource == null || newSource.length() == 0)
			throw new IllegalArgumentException(
				"newSource is null or empty" );
		
		if (source == null && lineNumberInOldSource != null)
			throw new IllegalArgumentException(
				"line number must be null for initial source" );
		
		if (source != null && lineNumberInOldSource == null)
			throw new IllegalArgumentException(
				"line number cannot be null for nested source" );
		
		if (lineNumberInOldSource != null)
			history.push( new LogHandler.History( source, lineNumberInOldSource ) );
		
		source = newSource;
	}
}