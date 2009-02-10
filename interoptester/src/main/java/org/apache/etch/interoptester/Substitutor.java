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

import java.util.Map;

import org.apache.etch.util.Assertion;
import org.apache.etch.util.CharIterator;


/**
 * A class to perform substitutions into a string from a map,
 * so that with input value="abc{foo}def" and map has key "foo"
 * with value "123", the result is "abc123def".
 */
public class Substitutor
{
	private Substitutor()
	{
		// don't instantiate
	}
	
	/**
	 * @param value
	 * @param map
	 * @return value with substitutions from map
	 */
	public static String subst( String value, Map<String, String> map )
	{
		StringBuffer sb = new StringBuffer();
		StringBuffer save = new StringBuffer();
		CharIterator i = new CharIterator( value );
		int state = 0;
		while (state != 4)
		{
			char c;
			int cc;
			if (i.hasNext())
			{
				c = i.next();
				cc = charClass( c );
			}
			else
			{
				c = 0;
				cc = EOF;
			}
			
			int nstate = nstates[state][cc];
			int action = actions[state][cc];
			
			switch (action)
			{
				case 1:
					sb.append( c );
					break;
				case 2:
					// ignore
					break;
				case 3:
					save.append( c );
					break;
				case 4:
					sb.append( save );
					save.setLength( 0 );
					sb.append( c );
					break;
				case 5:
					save.append( c );
					sb.append( subst0( save.toString(), map ) );
					save.setLength( 0 );
					break;
				case 6:
					throw new IllegalArgumentException( "malformed escape" );
				case 7:
					sb.append( save );
					save.setLength( 0 );
					break;
				default:
					throw new IllegalArgumentException( "action "+action+" unknown" );
			}
			
			state = nstate;
		}
		return sb.toString();
	}
	
	private static Object subst0( String s, Map<String, String> map )
	{
		Assertion.check( s.startsWith( "{" ), "starts with {" );
		Assertion.check( s.endsWith( "}" ), "ends with }" );
		String n = s.substring( 1 ).substring( 0, s.length()-2 );
		if (map.containsKey( n ))
			return map.get( n );
		return s;
	}

	private static int charClass( char c )
	{
		if (c == '\\') return BSL;
		if (c == '{') return LBR;
		if (c == '}') return RBR;
		if (c >= 'a' && c <= 'z') return ALN;
		if (c >= 'A' && c <= 'Z') return ALN;
		if (c >= '0' && c <= '9') return ALN;
		return OTH;
	}
	
	private final static int BSL = 0;
	
	private final static int LBR = 1;
	
	private final static int RBR = 2;
	
	private final static int ALN = 3;
	
	private final static int OTH = 4;
	
	private final static int EOF = 5;
	
	private static final byte[][] nstates =
	{
		// first index is current state
		// second index is char class: BSL, LBR, RBR, ALN, OTH, EOF
		{ 1, 2, 0, 0, 0, 4 },
		{ 0, 0, 0, 0, 0, 4 },
		{ 0, 2, 0, 3, 0, 4 },
		{ 0, 0, 0, 3, 0, 4 }
	};
	
	private static final byte[][] actions =
	{
		// first index is current state
		// second index is char class: BSL, LBR, RBR, ALN, OTH, EOF
		{ 2, 3, 1, 1, 1, 2 },
		{ 1, 1, 1, 1, 1, 6 },
		{ 4, 1, 4, 3, 4, 7 },
		{ 4, 4, 5, 3, 4, 7 }
	};
}
