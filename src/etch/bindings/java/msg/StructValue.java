/* $Id$
 *
 * Created by sccomer on Jan 17, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/**
 * A typed map of key/value pairs, where the type is a Type,
 * each key is a Field, and each value is of arbitrary type
 * chosen from the basic java types boolean, byte, short, int, long,
 * float, double, String, an array of those, the extended types
 * ArrayValue and StructValue, and specific types supported by
 * ValueFactory.
 * 
 * StructValue is not protected against concurrent access.
 */
public class StructValue extends HashMap<Field, Object>
	implements Iterable<Map.Entry<Field, Object>>
{
	/**
	 * A bogus serial version uid.
	 */
	private static final long serialVersionUID = 1L;

	/**
	 * Constructs the StructValue.
	 * @param type the type of the struct (or the action
	 * or event if this is a message).
	 */
	public StructValue( Type type )
	{
		if (type == null)
			throw new IllegalArgumentException( "type == null" );
		
		this.type = type;
	}
	
	/**
	 * Constructs the StructValue with expected length.
	 * @param type the type of the struct (or the action
	 * or event if this is a message).
	 * @param length the expected number of name/value pairs.
	 */
	public StructValue( Type type, int length )
	{
		super( length );
		
		if (type == null)
			throw new IllegalArgumentException( "type == null" );
		
		this.type = type;
	}

	private final Type type;
	
	/**
	 * @return the struct type.
	 */
	public Type type()
	{
		return type;
	}

	/**
	 * Compares the type of this struct to another type.
	 * @param otherType the type to compare this type to.
	 * @return true if this struct is of the specified type.
	 */
	public boolean isType( Type otherType )
	{
		return type.equals( otherType );
	}

	/**
	 * Checks a struct for having the expected type.
	 * @param expectedType the expected type of this struct.
	 * @throws IllegalArgumentException if the type is not as expected.
	 */
	public void checkType( Type expectedType )
	{
		if (!isType( expectedType ))
			throw new IllegalArgumentException();
	}
	
	/**
	 * @return an iterator over the entry set (key/value pairs) of this struct.
	 * @see java.util.Map.Entry
	 */
	public final Iterator<Map.Entry<Field, Object>> iterator()
	{
		return entrySet().iterator();
	}
	
	@Override
	final public Object put( Field key, Object value )
	{
		if (value == null)
			return remove( key );
		
		// TODO disable validate on put, we don't need it except as an
		// extra safety net during development.
		if (true)
		{
		    Validator v = type.getValidator( key );
		    if (v == null || !v.validate( value ))
		    	throw new IllegalArgumentException( String.format(
		    		"validator %s failed for type %s field %s: value %s", v, type, key, value ) );
		}
		
	    return super.put( key, value );
	}
	
	@Override
	public String toString()
	{
		return String.format( "%s: %s", type, super.toString() );
	}
}