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

package org.apache.etch.util.core.xml;

/**
 * A simple interface to a string buffer.
 */
public interface StringBuf
{
	/**
	 * Append a character to the string buffer.
	 * @param c the character to append.
	 */
	public void append( char c );
	
	/**
	 * Append a string to the string buffer.
	 * @param s the string of chars to append.
	 */
	public void append( String s );

	/**
	 * @return the number of characters in the string buffer.
	 */
	public int length();

	/**
	 * @return the current contents as a string.
	 */
	public String toString();

	/**
	 * Reset the string buffer to having no content.
	 */
	public void clear();

	/**
	 * @return a description of the string buffer.
	 */
	public String getDescr();
}
