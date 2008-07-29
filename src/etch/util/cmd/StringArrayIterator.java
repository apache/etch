/* $Id: StringArrayIterator.java 21902 2006-04-12 13:28:38Z wert $
 *
 * Created by wert on Mar 27, 2005.
 * 
 * Copyright (c) 2006 Metreos, Inc. All rights reserved.
 */

package etch.util.cmd;

import java.util.Iterator;
import java.util.NoSuchElementException;

/**
 * Simply an Iterator for String[].
 */
public class StringArrayIterator implements Iterator<String>
{
	/**
	 * @param array
	 */
	public StringArrayIterator( String[] array )
	{
		this.array = array;
	}
	
	private final String[] array;
	
	private int index;
	
	/* (non-Javadoc)
	 * @see java.util.Iterator#hasNext()
	 */
	public boolean hasNext()
	{
		return index < array.length;
	}

	/* (non-Javadoc)
	 * @see java.util.Iterator#next()
	 */
	public String next()
	{
		if (index >= array.length)
			throw new NoSuchElementException();
		
		return array[index++];
	}

	/* (non-Javadoc)
	 * @see java.util.Iterator#remove()
	 */
	public void remove()
	{
		throw new UnsupportedOperationException();
	}
}
