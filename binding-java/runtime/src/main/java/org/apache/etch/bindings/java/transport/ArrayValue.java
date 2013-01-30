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

package org.apache.etch.bindings.java.transport;

import java.lang.reflect.Array;
import java.util.Iterator;

import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.util.ArrayIterator;


/**
 * An array of values, where each value is of arbitrary type
 * chosen from the basic java types boolean, byte, short, int,
 * long, float, double, String, an array of those, the extended
 * types ArrayValue and StructValue, and specific types supported
 * by ValueFactory.
 * 
 * ArrayValue is not protected against concurrent access.
 */
public final class ArrayValue implements Iterable<Object>
{
	/**
	 * Constructs the ArrayValue.
	 * @param array
	 * @param dim
	 * @param customStructType
	 * @param typeCode
	 */
	public ArrayValue( Object array, byte typeCode, Type customStructType, int dim )
	{
		if (array == null)
			throw new NullPointerException( "array == null" );
		
		if (!array.getClass().isArray())
			throw new IllegalArgumentException( "!array.getClass().isArray()" );
		
		this.array = array;
		this.typeCode = typeCode;
		this.customStructType = customStructType;
		this.dim = dim;
	}
	
	/**
	 * Constructs an array value with no type info. This is used perhaps
	 * by extern struct serializers.
	 * @param array
	 */
	public ArrayValue( Object array )
	{
		this( array, (byte) 0, null, 0 );
	}
	
	private Object array;
	
	private final byte typeCode;
	
	private final Type customStructType;
	
	private final int dim;
	
	/**
	 * @return the TypeCode for this array value.
	 * For example, if the array is int[][], then
	 * the type would be TypeCode.INT4.
	 */
	public byte typeCode()
	{
		return typeCode;
	}
	
	/**
	 * @return a struct type if a custom type code.
	 */
	public Type customStructType()
	{
		return customStructType;
	}
	
	/**
	 * @return the dimsionality of the array.
	 * For example, if the array is int[][], the
	 * dimensionality would be 2.
	 */
	public int dim()
	{
		return dim;
	}

	public Iterator<Object> iterator()
	{
		return new ArrayIterator( array );
	}

	/**
	 * @return the number of elements in the array.
	 */
	public int size()
	{
		return Array.getLength( array );
	}

	/**
	 * @param index
	 * @return the element at the specified index.
	 */
	public Object get( int index )
	{
		return Array.get( array, index );
	}

	/**
	 * Reallocates the array so that it is only as long as needed.
	 */
	public void compact()
	{
		if (addIndex == size())
			return;
		
		Object narray = Array.newInstance( array.getClass().getComponentType(), addIndex );
		System.arraycopy( array, 0, narray, 0, addIndex );
		array = narray;
	}

	/**
	 * Adds the value to the end of the array, making more space
	 * available if needed.
	 * @param value
	 */
	public void add( Object value )
	{
		int n = size();
		if (addIndex >= n)
		{
			if (n == 0)
				n = 8;
			else
				n *= 2;
			Object narray = Array.newInstance( array.getClass().getComponentType(), n );
			System.arraycopy( array, 0, narray, 0, addIndex );
			array = narray;
		}
		Array.set( array, addIndex++, value );
	}
	
	private int addIndex;

	/**
	 * @return the array value.
	 */
	public Object getArray()
	{
		return array;
	}
}