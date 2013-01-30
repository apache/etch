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


/**
 * This Interface is used for Logging messages. It helps provide a consistent
 * way of printing and reporting Error, Info and Warning messages.
 * @author gsandhir
 *
 */
public interface LogHandler
{
	/**
	 * A message which is always printed, but doesn't affect compilation.
	 * Also usually formatted without level, phase, source, line number, or
	 * history information.
	 */
	public static final int LEVEL_IMPORTANT = 0;
	
	/**
	 * An error level message. Also causes compiler to exit with an error code.
	 */
	public static final int LEVEL_ERROR = 1;
	
	/**
	 * A warning level message.
	 */
	public static final int LEVEL_WARNING = 2;
	
	/**
	 * An information level message.
	 */
	public static final int LEVEL_INFO = 3;
	
	/**
	 * Formats and reports a message without a line number.
	 * @param level
	 * @param fmt
	 * @param params
	 */
	public void report( int level, String fmt, Object ... params );
	
	/**
	 * Formats and reports a message with perhaps a line number.
	 * @param level
	 * @param lineNumber
	 * @param fmt
	 * @param params
	 */
	public void report( int level, Integer lineNumber, String fmt,
		Object ... params );
	
	/**
	 * This method is used to either store or print messages
	 * @param level indicates ERROR, WARNING or INFO
	 * @param token indicates the token at which the error occurred
	 * @param msg String message
	 * @deprecated use {@link #report(int, Integer, String)} instead.
	 */
	@Deprecated
	public void logMessage( int level, Token token, String msg );
	
	/**
	 * Reports a pre-formatted message perhaps with a line number.
	 * @param level
	 * @param lineNumber
	 * @param msg
	 */
	public void report( int level, Integer lineNumber, String msg );
	
	/**
	 * Determines if a message is interesting given its level.
	 * @param level
	 * @return true if a message is interesting given its level.
	 */
	public boolean isInteresting( int level );
	
	/**
	 * Pushes a new source on the stack.
	 * @param newSource new source name being read.
	 * @param lineNumberInOldSource line number in old source which caused the new
	 * source to be read. This should be null for the top level source.
	 */
	public void push( String newSource, Integer lineNumberInOldSource );
	
	/**
	 * Removes the source that was added by the last push method.
	 * It behaves exactly like the pop method in Stacks
	 * @param oldSource the source name we just finished reading. This was
	 * previously passed as the argument to the last push.
	 */
	public void pop( String oldSource );
	
	/**
	 * @return true if an error has been reported.
	 */
	public boolean hasError();
	
	/**
	 * @return count of errors that have been reported.
	 */
	public int errorCount();
	
	/**
	 * @return true if a warning has been reported.
	 */
	public boolean hasWarning();
	
	/**
	 * @return count of warnings that have been reported.
	 */
	public int warningCount();

	/**
	 * Gets the level of logging.
	 * @return the current level of logging. Only messages with level less than
	 * or equal to this value will be logged.
	 */
	public int getLevel();
	
	/**
	 * Sets the level of logging. Only messages with level less than
	 * or equal to this value will be logged.
	 * @param level the level of logging
	 */
	public void setLevel( int level );
	
	/**
	 * @return the current phase of processing. The initial value is null.
	 */
	public String getPhase();
	
	/**
	 * Sets the current phase of processing.
	 * @param phase may be null.
	 */
	public void setPhase( String phase );
	
	/**
	 * Final condensation of the information of a report.
	 */
	public static class Message
	{
		/**
		 * @param level
		 * @param phase
		 * @param source
		 * @param lineNumber
		 * @param msg
		 * @param history
		 */
		public Message( int level, String phase, String source,
			Integer lineNumber, String msg, History[] history )
		{
			this.level = level;
			this.phase = phase;
			this.source = source;
			this.lineNumber = lineNumber;
			this.msg = msg;
			this.history = history;
		}
		
		/** the level of the report */
		public final int level;
		
		/** the phase of processing when the report was made */
		public final String phase;
		
		/** the current input source */
		public final String source;
		
		/** the line number of the source which caused the report (if any) */
		public final Integer lineNumber;
		
		/** the text of the report */
		public final String msg;
		
		/** the history of sources previous to source / lineNumber above */
		public final History[] history;
	}

	/**
	 * Record of the input streams previous to the last one pushed.
	 */
	public static class History
	{
		/**
		 * @param source
		 * @param lineNumber
		 */
		public History( String source, int lineNumber )
		{
			this.source = source;
			lineNum = lineNumber;
		}
		
		/**
		 * The source of the input stream (file, stdin, etc.)
		 */
		public final String source;
		
		/**
		 * The line number of the source which caused a new source to be pushed.
		 */
		public final int lineNum;
	}
}
