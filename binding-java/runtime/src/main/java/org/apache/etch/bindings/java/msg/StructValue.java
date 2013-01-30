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

package org.apache.etch.bindings.java.msg;

import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

import org.apache.etch.bindings.java.msg.Validator.Level;


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

	// magic number 6 results in super( 8 ) in HashMap constructor.
	private final static int DEFAULT_SIZE = 8;
	
	/**
	 * Constructs the StructValue with default length.
	 * 
	 * @param type the type of the struct.
	 * @param vf the value factory.
	 */
	public StructValue( Type type, ValueFactory vf )
	{
		this( type, vf, 0 );
	}
	
	/**
	 * Constructs the StructValue.
	 * 
	 * @param type the type of the struct.
	 * @param vf the value factory.
	 * @param length the number of name / value pairs expected. 0 means use
	 * default.
	 */
	public StructValue( Type type, ValueFactory vf, int length )
	{
		super( length > 0 ? (length*4+2)/3 : DEFAULT_SIZE );
		
		if (type == null)
			throw new IllegalArgumentException( "type == null" );
		
		this.type = type;
		level = vf.getLevel();
	}

	private final Type type;
	
	private final Level level;
	
	/**
	 * @return the struct type.
	 */
	public Type type()
	{
		return type;
	}
	
	/**
	 * @return validation level.
	 */
	public Level level()
	{
		return level;
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
		
		if (level != Level.NONE)
		{
		    Validator v = type.getValidator( key );
		    
		    if (level == Level.FULL && v == null)
		    	throw new IllegalArgumentException( String.format(
		    		"validator missing for type %s field %s", type, key ) );
		    
		    if (v != null && !v.validate( value ))
		    	throw new IllegalArgumentException( String.format(
		    		"validator %s failed for type %s field %s: value %s",
		    		v, type, key, value ) );
		}
		
	    return super.put( key, value );
	}
	
	@Override
	public String toString()
	{
		return String.format( "%s: %s", type, super.toString() );
	}
}