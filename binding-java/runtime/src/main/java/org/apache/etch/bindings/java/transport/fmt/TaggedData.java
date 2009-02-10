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

package org.apache.etch.bindings.java.transport.fmt;

import java.lang.reflect.Array;

import org.apache.etch.bindings.java.msg.ComboValidator;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.Validator;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.Validator_StructValue;
import org.apache.etch.bindings.java.transport.ArrayValue;


/**
 * Common interface for various types of tagged data input and output.
 */
abstract public class TaggedData
{
	/**
	 * Constructs the TaggedData.
	 *
	 * @param vf
	 */
	public TaggedData( ValueFactory vf )
	{
		this.vf = vf;
	}
	
	/**
	 * The value factory to use for tagged input and output.
	 */
	protected final ValueFactory vf;

	/**
	 * @return the value factory to use for tagged input and output.
	 */
	public ValueFactory getValueFactory()
	{
		return vf;
	}

	/**
	 * @param value
	 * @param v
	 * @return an ArrayValue which models the given array.
	 */
	@SuppressWarnings("deprecation")
	protected ArrayValue toArrayValue( Object value, Validator v )
	{
		Class<?> c = value.getClass();
//		System.out.printf( "toArrayValue: class is %s\n", c );
		int dim = 0;
		while (c.isArray())
		{
			dim++;
			c = c.getComponentType();
		}
//		System.out.printf( "toArrayValue: base class is %s dim %d\n", c, dim );
		
		// now we want the type code for c, and if the type code is custom,
		// we'll also want the struct type code.
		
		byte typeCode = getNativeTypeCode( c );
		
		Type customStructType;
		if (typeCode == TypeCode.CUSTOM || typeCode == TypeCode.STRUCT)
		{
			customStructType = getCustomStructType( c );
			
			if (customStructType == null && c == StructValue.class)
			{
				Validator_StructValue x = findStructValueValidator( v );
				if (x != null)
					customStructType = x.getType();
			}
			
			if (customStructType == null )
				throw new IllegalArgumentException( "customStructType == null" );
		}
		else
			customStructType = null;

//		System.out.printf( "toArrayValue: type %d customStructType %d dim %d\n", type, customStructType, dim );
		return new ArrayValue( value, typeCode, customStructType, dim );
	}
	
	private Validator_StructValue findStructValueValidator( Validator v )
	{
		if (v instanceof Validator_StructValue)
			return (Validator_StructValue) v;
		
		if (v instanceof ComboValidator)
		{
			ComboValidator x = (ComboValidator) v;
			Validator_StructValue y = findStructValueValidator( x.a() );
			
			if (y != null)
				return y;
			
			return findStructValueValidator( x.b() );
		}
		
		return null;
	}

	/**
	 * @param av
	 * @return a native array converted from an array value.
	 */
	protected Object fromArrayValue( ArrayValue av )
	{
		return av.getArray();
	}
	
	/**
	 * @param typeCode
	 * @param customStructType
	 * @param dim
	 * @param length
	 * @return the allocated array value.
	 */
	protected Object allocArrayValue( byte typeCode, Type customStructType,
		int dim, int length )
	{
		Class<?> clss = getComponentType( typeCode, customStructType, dim-1 );
		
		if (clss == null)
			throw new IllegalArgumentException( String.format(
				"could not get array for %s, %s, %d",
				typeCode, customStructType, dim-1 ) );

		return Array.newInstance( clss, length );
	}
	
	@SuppressWarnings("deprecation")
	private Class<?> getComponentType( byte typeCode, Type customStructType,
		int dim )
	{
		Class<?> c;
		if (typeCode == TypeCode.CUSTOM || typeCode == TypeCode.STRUCT)
		{
			c = customStructType.getComponentType();
			if (c == null)
				c = StructValue.class;
		}
		else
		{
			c = getNativeType( typeCode );
		}
		
//		System.out.printf( "getComponentType class %s\n", c );
		
		if (c == null)
			return null;
		
		if (dim == 0)
			return c;
		
		// TODO is there a better way to get the class of a multi-dimensional array?
		int[] dims = new int[dim];
		Object o = Array.newInstance( c, dims );
//		System.out.printf( "object = %s\n", o );
		c = o.getClass();
//		System.out.printf( "class = %s\n", c );
		return c;
	}

	/**
	 * @param value
	 * @return return a type code for the specified value.
	 */
	abstract public byte checkValue( Object value );
	
	/**
	 * Returns the type code for the specified class. This
	 * is needed when we have an array and we have determine
	 * the base type and now we're fixing to serialize it.
	 * We use the base type code to reconstitute it on the
	 * other side. So we don't return a perfect code the way
	 * checkValue does, but rather just something that let's
	 * us recreate the appropriate array type on import.
	 * @param c
	 * @return a type code for the specified class.
	 */
	abstract public byte getNativeTypeCode( Class<?> c );
	
	/**
	 * @param c
	 * @return a struct type for the specified custom class.
	 */
	abstract public Type getCustomStructType( Class<?> c );

	/**
	 * @param type
	 * @return the class for a specified (native) type code.
	 */
	abstract public Class<?> getNativeType( byte type );
}
