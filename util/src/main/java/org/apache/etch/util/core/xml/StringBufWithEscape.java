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

import java.util.Map;

/**
 * Description of StringBufWithEscape.
 */
public class StringBufWithEscape extends PlainStringBuf
{
	/**
	 * Constructs the StringBufWithEscape.
	 *
	 * @param descr
	 * @param maxLen
	 * @param maxEscapeLen
	 * @param escapes
	 */
	public StringBufWithEscape( String descr, int maxLen,
		int maxEscapeLen, Map<String,Character> escapes )
	{
		super( descr, maxLen );
		this.maxEscapeLen = maxEscapeLen;
		this.escapes = escapes;
	}
	
	private final int maxEscapeLen;
	
	private final Map<String,Character> escapes;
	
	@Override
	public void append( char c )
	{
		if (c == '&')
		{
			if (escapeBuffer != null)
				throw new UnsupportedOperationException( "bad char '"+c+"' in escape" );
			escapeBuffer = new PlainStringBuf( "escape", maxEscapeLen );
		}
		else if (escapeBuffer != null)
		{
			if (c == ';')
			{
				String s = escapeBuffer.toString();
				escapeBuffer = null;
				super.append( parseEscape( s ) );
			}
			else
			{
				escapeBuffer.append( c );
			}
		}
		else
		{
			super.append( c );
		}
	}
	
	private char parseEscape( String s )
	{
		if (s.length() == 0)
			throw new UnsupportedOperationException( "empty escape" );
		
		if (s.startsWith( "#" ))
		{
			try
			{
				int k = Integer.parseInt( s.substring( 1 ) );
				
				if (k < 0 || k > 65535)
					throw new UnsupportedOperationException( "numeric escape out of range '"+s+"'" );
				
				return (char) k;
			}
			catch ( NumberFormatException e )
			{
				throw new UnsupportedOperationException( "bad numeric escape '"+s+"'" );
			}
		}
		
		Character c = escapes.get( s );
		if (c != null)
			return c.charValue();
		
		c = escapes.get( s.toLowerCase() );
		if (c != null)
			return c.charValue();
		
		throw new UnsupportedOperationException( "unknown entity escape '"+s+"'" );
	}
	
	@Override
	public int length()
	{
		int n = super.length();
		
		if (escapeBuffer != null)
			return n+1;
		
		return n;
	}

	@Override
	public String toString()
	{
		if (escapeBuffer != null)
			throw new UnsupportedOperationException( "unfinished escape" );
		
		return super.toString();
	}

	private PlainStringBuf escapeBuffer;
}
