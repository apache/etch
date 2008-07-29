/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package etch.compiler;


import java.util.Stack;

/**
 * This class implements the LogHandler Interface for Compiler
 * @author gsandhir
 *
 */

public class CompilerLogHandler implements LogHandler
{
	/**
	 * Constructs the CompilerLogHandler and sets the initial phase.
	 * @param phase
	 */
	public CompilerLogHandler( String phase )
	{
		this.phase = phase;
	}
	
	private String phase;
	
	public void setPhase( String phase )
	{
		this.phase = phase;
	}
	
	public String getPhase()
	{
		return phase;
	}
	
	/**
	 * Etch Error Header
	 */
	public String errHeader = "ETCH ERROR : %s";
	/**
	 * Etch Info Header
	 */
	public String infoHeader = "ETCH INFO : %s";
	/**
	 * Etch Warning Header
	 */
	public String warnHeader = "ETCH WARNING : %s";
	
	/**
	 * Error Flag to indicate whether there errors
	 */
	public boolean errorFlag = false;

	private Stack<Source> stack = new Stack<Source>();
	
	private boolean isQuiet = false;
	
	/**
	 * Constructs the CompilerLogHandler.
	 */
	public CompilerLogHandler()
	{
		this( "" );
	}
	
	public void logMessage( int level, Token token, String msg )
	{
		synchronized (this)
		{
			if (level == LogHandler.LEVEL_ERROR)
			{
				errorFlag = true;
				String fmt = phase + " " + printStack() + ": " + msg;
				System.err.printf( errHeader, fmt );
			}
			else if (level == LogHandler.LEVEL_INFO)
			{
				if (!isQuiet)
				{
					String fmt = printStack() + ": " + msg;
					System.out.printf( infoHeader, fmt );
				}
			}
			else if (level == LogHandler.LEVEL_WARNING)
			{
				if (!isQuiet)
				{
					String fmt = phase + " " + printStack() + ": " + msg;
					System.out.printf( warnHeader, fmt );
				}
			}
			else
			{
				System.err.println( msg );
			}
		}
	}
	
	public void setQuiet( boolean quiet )
	{
		isQuiet = quiet;
	}
	
	public boolean getQuiet()
	{
		return isQuiet;
	}

	public void push( String value, Integer num )
	{
		Source source = new Source( value, num );
		stack.push( source );
	}

	public void pop()
	{
		stack.pop();
	}

	public boolean hasError()
	{
		return errorFlag;
	}
	/**
	 * Prints the Source of the log message
	 * @return String containing the complete source
	 */
	public String printStack()
	{
		String temp = "";
		if (stack.isEmpty())
			return temp;
		int firstElementNum = stack.size() - 1;

		Source firstElement = stack.get( firstElementNum );
		temp = temp + firstElement.getSourceName();
		/*
		 * if (firstElement.getLineNumber() != null) temp = temp + ", Line " +
		 * firstElement.getLineNumber().toString();
		 */
		for (int i = firstElementNum - 1; i >= 0; i--)
		{
			Source src = stack.get( i );
			temp = temp + " [ Included From " + src.getSourceName();
			// if ((i + 1) <= firstElementNum)
			// {
			Source prevSource = stack.get( i + 1 );
			if (prevSource.getLineNumber() != null)
				temp = temp + ", Line " + prevSource.getLineNumber().toString()
					+ " ] ";
			else
				temp = temp + " ] ";
			/*
			 * } else temp = temp + " ] ";
			 */
		}

		return temp;
	}

	private class Source
	{
		String str;

		Integer lineNum;
		/**
		 * Constructor
		 * @param s
		 * @param in
		 */

		public Source( String s, Integer in )
		{
			str = s;
			lineNum = in;
		}
		
		/**
		 * Get the name of the file or string from where the error or info message
		 * originated
		 * @return Source Name
		 */

		public String getSourceName()
		{
			return str;
		}
		
		/**
		 * Returns the line number from where the error or info message originated
		 * @return line number
		 */

		public Integer getLineNumber()
		{
			return lineNum;
		}

	}
	
	/**
	 * Test Driver for CompilerLogHandler
	 * @param argv
	 */

	public static void main( String[] argv )
	{
		CompilerLogHandler cl = new CompilerLogHandler();
		cl.push( "Example", null );
		cl.push( "def", new Integer( 10 ) );
		String st = cl.printStack();
		System.out.println( st );
	
	}

}
