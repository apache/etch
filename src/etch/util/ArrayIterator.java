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
