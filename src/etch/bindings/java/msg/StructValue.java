/* $Id$
 *
 * Created by sccomer on Jan 17, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

import java.io.IOException;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;
import java.util.Set;

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
	
	/**
	 * Writes a struct to the tagged data output.
	 * @param tdo the tagged data output to write to.
	 * @throws IOException if there is a problem with the tagged data output.
	 */
	public void writeStruct( TaggedDataOutput tdo )
		throws IOException
	{
		tdo.startStruct( this );
		writeKeysAndValues( tdo );
		tdo.endStruct( this );
	}
	
	/**
	 * Writes the key / value pairs.
	 * @param tdo the tagged data output to write to.
	 * @throws IOException if there are problems with the tagged data output.
	 */
	public final void writeKeysAndValues( TaggedDataOutput tdo ) throws IOException
	{
		for (Map.Entry<Field, Object> me: this)
			tdo.writeStructElement( me.getKey(), me.getValue() );
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

	/**
	 * @param obj an object to compare to this struct value.
	 * @param helper implementation to help with comparing
	 * comformant but not identical objects (e.g., 23
	 * represented as Byte vs. Integer).
	 * @return true if the object is a struct value and
	 * compares equal with this one (relative to helper).
	 */
	public boolean equals( Object obj, EqualsHelper helper )
	{
		if (obj == this)
			return true;
		
		if (obj == null)
			return false;
		
		if (getClass() != obj.getClass())
			return false;
		
		StructValue other = (StructValue) obj;
		
		if (!type.equals( other.type ))
			return false;
		
		Set<Field> keySet = keySet();
		Set<Field> otherKeySet = other.keySet();
		
		if (!keySet.equals( otherKeySet ))
			return false;
		
		for (Field mf: keySet)
		{
			Object a = get( mf );
			Object b = other.get( mf );
			if (helper != null)
			{
				if (!helper.equals( a, b ))
					return false;
			}
			else if (!objEquals( a, b ))
			{
				return false;
			}
		}
		
		return true;
	}

	/**
	 * Compares two objects for equality taking into account the fact
	 * that one or both might be null.
	 * @param a an object to compare.
	 * @param b an object to compare.
	 * @return true if both are null or if both are not null and a.equals( b ).
	 */
	private static boolean objEquals( Object a, Object b )
	{
		if (a == null && b == null)
			return true;
		
		if (a == null || b == null)
			return false;
		
		return a.equals( b );
	}
}