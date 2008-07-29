/* $Id$
 *
 * Created by wert on Apr 26, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport.fmt.binary;

import etch.bindings.java.msg.ArrayValue;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.transport.fmt.TaggedData;
import etch.bindings.java.transport.fmt.TypeCode;

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
	public final static byte VERSION = 1;

	/**
	 * Constructs the BinaryTaggedData.
	 *
	 * @param vf
	 */
	public BinaryTaggedData( ValueFactory vf )
	{
		super( vf );
	}
	
	/**
	 * @param value
	 * @param v the validator for values of this type.
	 * @return return a type code for the specified value.
	 */
	public byte checkValue( final Object value, Validator v )
	{
		if (value == null)
			return TypeCode.NULL;
		
		if (value == NONE)
			return TypeCode.NONE;
		
		Byte type = v.checkValue( value );
		if (type == null)
			throw new UnsupportedOperationException( "type == null" );
		
		type = adjSmallValue( type, value );
		
		return type;
	}
	
	private byte adjSmallValue( byte type, Object value )
	{
		if (type == TypeCode.BYTE1 || type == TypeCode.SHORT1 ||
			type == TypeCode.INT1 || type == TypeCode.LONG1)
		{
			byte v = ((Number) value).byteValue();
			if (v >= TypeCode.MIN_SMALL_INT && v <= TypeCode.MAX_SMALL_INT)
			{
				if (v >= 0)
				{
					if (type == TypeCode.BYTE1)
						return (byte) (v + TypeCode.POSITIVE_BYTE_BASE);
					else if (type == TypeCode.SHORT1)
						return (byte) (v + TypeCode.POSITIVE_SHORT_BASE);
					else if (type == TypeCode.INT1)
						return (byte) (v + TypeCode.POSITIVE_INT_BASE);
					else
						return (byte) (v + TypeCode.POSITIVE_LONG_BASE);
				}
				
				if (type == TypeCode.BYTE1)
					return (byte) (-(v + 1) + TypeCode.NEGATIVE_BYTE_BASE);
				else if (type == TypeCode.SHORT1)
					return (byte) (-(v + 1) + TypeCode.NEGATIVE_SHORT_BASE);
				else if (type == TypeCode.INT1)
					return (byte) (-(v + 1) + TypeCode.NEGATIVE_INT_BASE);
				else
					return (byte) (-(v + 1) + TypeCode.NEGATIVE_LONG_BASE);
			}
		}
		return type;
	}

	@Override
	public byte checkValue( final Object value )
	{
		if (value == null)
			return TypeCode.NULL;
		
		if (value == NONE)
			return TypeCode.NONE;
		
		final Class<?> c = value.getClass();
		
		if (value instanceof Number)
		{
			if (c == Integer.class)
				return checkInteger( (Integer) value, TypeCode.INT4, TypeCode.INT_DT );
			
			if (c == Long.class)
				return checkLong( (Long) value, TypeCode.LONG8, TypeCode.LONG_DT );
			
			if (c == Short.class)
				return checkShort( (Short) value, TypeCode.SHORT2, TypeCode.SHORT_DT );
				
			if (c == Byte.class)
				return checkByte( (Byte) value, TypeCode.BYTE1, TypeCode.BYTE_DT );
			
			if (c == Double.class)
				return TypeCode.FLOAT8;
			
			if (c == Float.class)
				return TypeCode.FLOAT4;
			
			return TypeCode.CUSTOM;
		}
		
		if (c == String.class)
		{
			String s = (String) value;
			
			if (s.length() == 0)
				return TypeCode.EMPTY_STRING;
			
			return TypeCode.STRING;
		}
		
		if (c == Boolean.class)
			return ((Boolean) value) ? TypeCode.BOOLEAN_TRUE : TypeCode.BOOLEAN_FALSE;
		
		if (c == StructValue.class)
			return TypeCode.STRUCT;
		
		if (c == ArrayValue.class)
			return TypeCode.ARRAY;
		
		if (c == byte[].class)
			return TypeCode.BYTES;
		
		if (c.isArray())
			return TypeCode.ARRAY;
		
		return TypeCode.CUSTOM;
	}

	@Override
	public byte getNativeTypeCode( Class<?> c )
	{
		if (c == boolean.class)
			return TypeCode.BOOLEAN_TRUE;
		
		if (c == byte.class)
			return TypeCode.BYTE1;
		
		if (c == short.class)
			return TypeCode.SHORT2;
			
		if (c == int.class)
			return TypeCode.INT4;
		
		if (c == long.class)
			return TypeCode.LONG8;
		
		if (c == float.class)
			return TypeCode.FLOAT4;
		
		if (c == double.class)
			return TypeCode.FLOAT8;
		
		if (c == String.class)
			return TypeCode.STRING;
		
		return TypeCode.CUSTOM;
	}

	@Override
	public Type getCustomStructType( Class<?> c )
	{
		return vf.getCustomStructType( c );
	}

	@Override
	public Class<?> getCustomType( Type type )
	{
		return vf.getCustomType( type );
	}

	@Override
	public Class<?> getNativeType( byte type )
	{
		switch (type)
		{
			case TypeCode.BOOLEAN_TRUE:
			case TypeCode.BOOLEAN_FALSE:
				return boolean.class;
			
			case TypeCode.BYTE1:
				return byte.class;
			
			case TypeCode.SHORT1:
			case TypeCode.SHORT2:
				return short.class;
			
			case TypeCode.INT1:
			case TypeCode.INT2:
			case TypeCode.INT4:
				return int.class;
			
			case TypeCode.LONG1:
			case TypeCode.LONG2:
			case TypeCode.LONG4:
			case TypeCode.LONG8:
				return long.class;
			
			case TypeCode.FLOAT4:
				return float.class;
			
			case TypeCode.FLOAT8:
				return double.class;
			
			case TypeCode.EMPTY_STRING:
			case TypeCode.STRING:
				return String.class;
			
			default:
				throw new IllegalArgumentException( "unsupported native type "+type );
		}
	}

	private static byte checkByte( byte v, byte is1, byte dt )
	{
		if (v >= TypeCode.MIN_SMALL_INT && v <= TypeCode.MAX_SMALL_INT)
		{
			if (v >= 0)
				return (byte) (TypeCode.PSVALUE | (dt << TypeCode.PDTSHIFT) | v);
			
			return (byte) (TypeCode.NSVALUE | (dt << TypeCode.NDTSHIFT) | -(v+1));
		}
		
		return is1;
	}
	
	private static byte checkShort( short v, byte is2, byte dt )
	{
		if (v >= Byte.MIN_VALUE && v <= Byte.MAX_VALUE)
			return checkByte( (byte) v, (byte) (is2+1), dt );
		
		return is2;
	}

	private static byte checkInteger( int v, byte is4, byte dt )
	{
		if (v >= Short.MIN_VALUE && v <= Short.MAX_VALUE)
			return checkShort( (short) v, (byte) (is4+1), dt );
		
		return is4;
	}

	private static byte checkLong( long v, byte is8, byte dt )
	{
		if (v >= Integer.MIN_VALUE && v <= Integer.MAX_VALUE)
			return checkInteger( (int) v, (byte) (is8+1), dt );
		
		return is8;
	}
}
