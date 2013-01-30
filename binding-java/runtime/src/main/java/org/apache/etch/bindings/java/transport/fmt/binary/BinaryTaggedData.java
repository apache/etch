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

package org.apache.etch.bindings.java.transport.fmt.binary;

import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.transport.fmt.TaggedData;
import org.apache.etch.bindings.java.transport.fmt.TypeCode;

/**
 * Common elements for binary tagged data input and output.
 */
public class BinaryTaggedData extends TaggedData
{
	/**
	 * Sentinel which marks the end of a struct or array item list
	 * in a binary input or output stream.
	 */
	public static final Object NONE = new Object()
	{
		@Override
		public String toString()
		{
			return "NONE";
		}
	};
	
	/**
	 * This is the current version of the protocol.
	 */
	public final static byte VERSION = 3;

	/**
	 * Constructs the BinaryTaggedData.
	 *
	 * @param vf
	 */
	public BinaryTaggedData( ValueFactory vf )
	{
		super( vf );
	}

	@Override
	public byte checkValue( final Object value )
	{
		if (value == null)
			return TypeCode.NULL;
		
		if (value == NONE)
			return TypeCode.NONE;
		
		final Class<?> c = value.getClass();
		
		if (c == Boolean.class)
			return (Boolean) value ? TypeCode.BOOLEAN_TRUE : TypeCode.BOOLEAN_FALSE;
		
		if (value instanceof Number)
		{
			if (c == Byte.class)
				return checkByte( (Byte) value );
			
			if (c == Short.class)
				return checkShort( (Short) value );
				
			if (c == Integer.class)
				return checkInteger( (Integer) value );
			
			if (c == Long.class)
				return checkLong( (Long) value );
			
			if (c == Float.class)
				return TypeCode.FLOAT;
			
			if (c == Double.class)
				return TypeCode.DOUBLE;
			
			return TypeCode.CUSTOM;
		}
		
		if (c == String.class)
		{
			String s = (String) value;
			
			if (s.length() == 0)
				return TypeCode.EMPTY_STRING;
			
			return TypeCode.STRING;
		}
		
		if (c == StructValue.class)
			return TypeCode.CUSTOM;
		
		if (c.isArray())
		{
//			if (c == boolean[].class) return TypeCode.BOOLS;
			if (c == byte[].class) return TypeCode.BYTES;
//			if (c == short[].class) return TypeCode.SHORTS;
//			if (c == int[].class) return TypeCode.INTS;
//			if (c == long[].class) return TypeCode.LONGS;
//			if (c == float[].class) return TypeCode.FLOATS;
//			if (c == double[].class) return TypeCode.DOUBLES;
			return TypeCode.ARRAY;
		}
		
		return TypeCode.CUSTOM;
	}

	@Override
	public byte getNativeTypeCode( Class<?> c )
	{
		if (c == boolean.class)
			return TypeCode.BOOLEAN_TRUE;
		
		if (c == byte.class)
			return TypeCode.BYTE;
		
		if (c == short.class)
			return TypeCode.SHORT;
			
		if (c == int.class)
			return TypeCode.INT;
		
		if (c == long.class)
			return TypeCode.LONG;
		
		if (c == float.class)
			return TypeCode.FLOAT;
		
		if (c == double.class)
			return TypeCode.DOUBLE;
		
		if (c == String.class)
			return TypeCode.STRING;
		
		if (c == Object.class)
			return TypeCode.ANY;
		
		return TypeCode.CUSTOM;
	}

	@Override
	public Type getCustomStructType( Class<?> c )
	{
		return vf.getCustomStructType( c );
	}

	@Override
	public Class<?> getNativeType( byte type )
	{
		switch (type)
		{
			case TypeCode.BOOLEAN_TRUE:
			case TypeCode.BOOLEAN_FALSE:
				return boolean.class;
			
			case TypeCode.BYTE:
				return byte.class;
			
			case TypeCode.SHORT:
				return short.class;
			
			case TypeCode.INT:
				return int.class;
			
			case TypeCode.LONG:
				return long.class;
			
			case TypeCode.FLOAT:
				return float.class;
			
			case TypeCode.DOUBLE:
				return double.class;
			
			case TypeCode.EMPTY_STRING:
			case TypeCode.STRING:
				return String.class;
				
			case TypeCode.ANY:
				return Object.class;
			
			default:
				throw new IllegalArgumentException( "unsupported native type "+type );
		}
	}

	private static byte checkByte( byte v )
	{
		if (v >= TypeCode.MIN_TINY_INT && v <= TypeCode.MAX_TINY_INT)
			return v;
		
		return TypeCode.BYTE;
	}
	
	private static byte checkShort( short v )
	{
		if (v >= Byte.MIN_VALUE && v <= Byte.MAX_VALUE)
			return checkByte( (byte) v );
		
		return TypeCode.SHORT;
	}

	private static byte checkInteger( int v )
	{
		if (v >= Short.MIN_VALUE && v <= Short.MAX_VALUE)
			return checkShort( (short) v );
		
		return TypeCode.INT;
	}

	private static byte checkLong( long v )
	{
		if (v >= Integer.MIN_VALUE && v <= Integer.MAX_VALUE)
			return checkInteger( (int) v );
		
		return TypeCode.LONG;
	}
}
