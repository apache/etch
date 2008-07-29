/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package etch.util;

import java.lang.reflect.Array;
import java.util.Iterator;
import java.util.NoSuchElementException;

/**
 * Iterator over a generic array.
 */
public class ArrayIterator implements Iterator<Object>
{
	/**
	 * Constructs the ArrayIterator.
	 * @param array
	 */
	public ArrayIterator( Object array )
	{
		if (array == null)
			throw new NullPointerException( "array == null" );
		
		if (!array.getClass().isArray())
			throw new IllegalArgumentException( "!array.getClass().isArray()" );
		
		this.array = array;
		length = Array.getLength( array );
	}
	
	private final Object array;
	
	private final int length;
	
	private int index = 0;
	
	public boolean hasNext()
	{
		return index < length;
	}

	public Object next()
	{
		if (!hasNext())
			throw new NoSuchElementException();
		
		return Array.get( array, index++ );
	}

	public void remove()
	{
		throw new UnsupportedOperationException();
	}
}
