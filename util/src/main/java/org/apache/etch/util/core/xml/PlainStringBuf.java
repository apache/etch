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
 * Description of PlainStringBuf.
 */
public class PlainStringBuf implements StringBuf
{
	/**
	 * Constructs the PlainStringBuf.
	 *
	 * @param descr
	 * @param maxLen
	 */
	public PlainStringBuf( String descr, int maxLen )
	{
		this.descr = descr;
		this.maxLen = maxLen;
	}
	
	private final String descr;
	
	private final int maxLen;

	/**
	 * @return the descr of this string buf.
	 */
	public String getDescr()
	{
		return descr;
	}

	public void append( char c )
	{
		if (sb.length() >= maxLen)
			throw new UnsupportedOperationException(
				"maximum length of "+descr+" exceeded: "+maxLen );

		sb.append( c );
	}

	public void append( String s )
	{
		// this must process a character at a time so that subclassers
		// will work properly.
		int n = s.length();
		for (int i = 0; i < n; i++)
			append( s.charAt( i )  );
	}

	public int length()
	{
		return sb.length();
	}

	@Override
	public String toString()
	{
		return sb.toString();
	}
	
	public void clear()
	{
		sb.setLength( 0 );
	}
	
	private final StringBuffer sb = new StringBuffer();
}
