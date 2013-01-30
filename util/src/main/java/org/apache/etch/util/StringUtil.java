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

package org.apache.etch.util;

/**
 * Miscellaneous string utility functions.
 */
public class StringUtil
{
	/**
	 * Makes a random string using the default characters, which
	 * are B-Z (minus E,I,O,U), b-z (minus e,i,o,u), and 2-9. Thus
	 * we have no vowel like characters in the result so that strings
	 * like 'fool', 'idiot', and 'doofus' will not ever appear.
	 * 
	 * Because there are 50 choices for each character in the resulting
	 * string, each character contributes 5.64 bits of randomness. so, if
	 * you want at least 64 bits of randomness, you'll need at least 12
	 * characters.
	 * 
	 * @param n the length of the string.
	 * 
	 * @return a random string of the specified length.
	 */
	public static String rndString( int n )
	{
		return rndString( n, DEFAULT_CHARS );
	}
	
	/**
	 * Makes a random string using the specified characters.
	 * @param n the length of the string.
	 * @param chars the characters used to construct the random string.
	 * @return a random string of the specified length.
	 */
	public static String rndString( int n, char[] chars )
	{
		if (n < 1)
			throw new IllegalArgumentException( "n < 1" );
		
		int k = chars.length;
		if (k < 2)
			throw new IllegalArgumentException( "chars.length < 2" );
		
		StringBuffer sb = new StringBuffer( n );
		while (n-- > 0)
			sb.append( chars[(int) (Math.random() * k)] );
		
		return sb.toString();
	}
	
	private final static char[] DEFAULT_CHARS = (
		"BCDFGHJKLMNPQRSTVWXYZ" +
		"bcdfghjklmnpqrstvwxyz" +
		"23456789"
		).toCharArray();

	/**
	 * @param s
	 * @param c
	 * @return an array x of two elements containing s split in two by the
	 * leftmost c. x[0] is the chars before c, x[1] is the chars after c.
	 * If c is not in s, returns null.
	 */
	public static String[] leftSplit( String s, char c )
	{
		if (s == null)
			throw new NullPointerException( "s == null" );
		
		int i = s.indexOf( c );
		if (i < 0)
			return null;
		
		return new String[] { s.substring( 0, i ), s.substring( i+1 ) };
	}
	
	/**
	 * @param s
	 * @param c
	 * @return an array x of two elements containing s split in two by the
	 * rightmost c. x[0] is the chars before c, x[1] is the chars after c.
	 * If c is not in s, returns null.
	 */
	public static String[] rightSplit( String s, char c )
	{
		if (s == null)
			throw new NullPointerException( "s == null" );
		
		int i = s.lastIndexOf( c );
		if (i < 0)
			return null;
		
		return new String[] { s.substring( 0, i ), s.substring( i+1 ) };
	}

	/**
	 * @param a
	 * @param b
	 * @return true if a equals b, false otherwise. takes into account nulls.
	 */
	public static boolean equals( String a, String b )
	{
		if (a == b)
			return true;
		
		if (a == null || b == null)
			return false;
		
		return a.equals( b );
	}

	/**
	 * @param a
	 * @return matches behavior of equals, above.
	 */
	public static int hashCode( String a )
	{
		if (a == null)
			return 0;
		return a.hashCode();
	}

	/**
	 * @param a
	 * @param b
	 * @return true if a equals b, false otherwise. takes into account nulls.
	 */
	public static boolean equalsIgnoreCase( String a, String b )
	{
		if (a == b)
			return true;
		
		if (a == null || b == null)
			return false;
		
		return a.equalsIgnoreCase( b );
	}

	/**
	 * @param a
	 * @return matches behavior of equalsIgnoreCase, above.
	 */
	public static int hashCodeIgnoreCase( String a )
	{
		if (a == null)
			return 0;
		return a.toLowerCase().hashCode();
	}
	
	/**
	 * Translates a hex char into an int.
	 * @param c a hex char (0-9, A-F, a-f).
	 * @return an int (0-15).
	 */
	public static int fromHex( char c )
	{
		if (c >= '0' && c <= '9')
			return c - '0';
		if (c >= 'A' && c <= 'F')
			return c - 'a' + 10;
		if (c >= 'a' && c <= 'f')
			return c - 'a' + 10;
		throw new IllegalArgumentException( "bad hex char "+c );
	}
	
	/**
	 * Translates an int into a hex char.
	 * @param i an int (0-15)
	 * @return a hex char (0-9, a-f).
	 */
	public static char toHex( int i )
	{
		if (i >= 0 && i <= 9)
			return (char) ('0' + i);
		if (i >= 10 && i <= 15)
			return (char) ('a' + i - 10);
		throw new IllegalArgumentException( "bad hex digit selector "+i );
	}
	
	/**
	 * @param digest
	 * @return string version of digest.
	 */
	public static String fingerprint( byte[] digest )
	{
		StringBuffer sb = new StringBuffer( digest.length * 2 );
		for (byte b: digest)
		{
			sb.append( toHex( b >>> 4 & 15 ) );
			sb.append( toHex( b & 15 ) );
		}
		return sb.toString();
	}
}
