/* $Id: CircularStringBuf.java 25575 2006-06-29 21:49:27Z wert $
 *
 * Created by wert on Apr 27, 2005.
 *
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package metreos.core.xml;

import metreos.util.Assertion;

/**
 * A StringBuf implemented as a circular fixed-size list of characters.
 */
final public class CircularStringBuf implements StringBuf
{
	/**
	 * Constructs the CircularStringBuf.
	 * @param descr a description of this buffer
	 *
	 * @param maxLen the maximum number of characters to keep
	 * in the buf.
	 */
	public CircularStringBuf( String descr, int maxLen )
	{
		this.descr = descr;
		this.maxLen = maxLen;
		chars = new char[maxLen];
	}
	
	private final String descr;
	
	private final int maxLen;
	
	private final char[] chars;
	
	private int index;
	
	private int length;

	/* (non-Javadoc)
	 * @see metreos.core.xml.StringBuf#append(char)
	 */
	public void append( char c )
	{
		fixupIndex();
		
		chars[index++] = c;
		
		if (length < maxLen)
			length++;
	}
	
	private void fixupIndex()
	{
		while (index >= maxLen)
			index -= maxLen;
	}

	/* (non-Javadoc)
	 * @see metreos.core.xml.StringBuf#append(java.lang.String)
	 */
	public void append( String s )
	{
		// if the length of s is larger than or equal to maxLen,
		// then we only need consider the last maxLen characters
		// and put them directly into the buffer. otherwise, we
		// must deposit the characters in two segments:
		// [index..maxLen) and [0..index).
		
		int n = s.length();
		if (n == 0)
			return;

		// offset in s (if negative, then n < maxLen)
		int i = n - maxLen;
		if (i < 0)
			i = 0;
		
		// how many characters from s we actually want.
		n = n - i;
		
		Assertion.check( i >= 0 && i < s.length(), "i >= 0 && i < s.length()" );
		Assertion.check( n > 0 && n <= maxLen, "n > 0 && n <= maxLen" );
		Assertion.check( i+n == s.length(), "i+n == s.length()" );
		
		// here is an optimization: if we're getting maxLen chars
		// from s, just get the chars directly into the buffer in
		// one try and reset index and length.
		
		if (n == maxLen)
		{
			s.getChars( i, i+n, chars, 0 );
			index = n;
			length = n;
			return;
		}

		Assertion.check( n < maxLen, "n < maxLen" );

		fixupIndex();
		
		int seg1len = maxLen - index;
		if (seg1len > n)
			seg1len = n;
		n -= seg1len;
		
		int seg2len = index;
		if (seg2len > n)
			seg2len = n;
		n -= seg2len;
		
		Assertion.check( n == 0, "n == 0" );
		Assertion.check( seg1len > 0, "seg1len > 0" );
		Assertion.check( seg2len >= 0, "seg2len >= 0" );
		Assertion.check( (seg1len + seg2len) < maxLen, "(seg1len + seg2len) < maxLen" );
		
		// handle segment 1:
		
		s.getChars( i, i+seg1len, chars, index );
		i += seg1len;
		index += seg1len;
		length += seg1len;
		
		// handle segment 2:
		
		if (seg2len > 0)
		{
			s.getChars( i, i+seg2len, chars, 0 );
			index = seg2len;
			length += seg2len;
		}
		
		if (length > maxLen)
			length = maxLen;
	}

	/* (non-Javadoc)
	 * @see metreos.core.xml.StringBuf#length()
	 */
	public int length()
	{
		return length;
	}

	@Override
	public String toString()
	{
		fixupIndex();
		
		if (index == length)
		{
			// chars are in one segment: [0..index)
			return new String( chars, 0, index );
		}
		
		// chars are in two segments: [index..maxLen) and [0..index)
		
		StringBuffer sb = new StringBuffer( maxLen );
		sb.append( chars, index, maxLen - index );
		sb.append( chars, 0, index );
		return sb.toString();
	}
	
	public void clear()
	{
		index = 0;
		length = 0;
	}

	public String getDescr()
	{
		return descr;
	}
}
