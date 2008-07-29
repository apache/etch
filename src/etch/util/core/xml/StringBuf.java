/* $Id: StringBuf.java 21902 2006-04-12 13:28:38Z wert $
 *
 * Created by wert on Apr 27, 2005.
 *
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package etch.util.core.xml;

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
