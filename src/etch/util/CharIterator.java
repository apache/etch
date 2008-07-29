/* $Id: CharIterator.java 21902 2006-04-12 13:28:38Z wert $
 *
 * Created by wert on Feb 16, 2005
 *
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package etch.util;

/**
 * @author wert
 */
public class CharIterator
{
	/**
	 * @param s
	 */
	public CharIterator( String s )
	{
		this.s = s;
		this.n = s.length();
	}
	
	private final String s;
	
	private final int n;
	
	private int i;

	/**
	 * @return true if there are more chars.
	 */
	public boolean hasNext()
	{
		return i < n;
	}

	/**
	 * @return the next character in sequence.
	 */
	public char next()
	{
		return s.charAt( i++ );
	}
}
