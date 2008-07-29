/* $Id$
 *
 * Created by sccomer on Jan 17, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport.fmt.xml;

import etch.bindings.java.msg.ArrayValue;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.transport.fmt.TaggedData;
import etch.bindings.java.transport.fmt.TypeCode;

public class XmlTaggedData extends TaggedData
{
	public XmlTaggedData( ValueFactory vf )
	{
		super( vf );
	}
	
	@Override
	public byte checkValue( Object value )
	{
		if (value == null)
			return TypeCode.NULL;
		
		Class<?> clss = value.getClass();
		
		if (value instanceof Number)
		{
			if (clss == Byte.class)
				return TypeCode.BYTE1;
			
			if (clss == Short.class)
				return TypeCode.SHORT2;
				
			if (clss == Integer.class)
				return TypeCode.INT4;
			
			if (clss == Long.class)
				return TypeCode.LONG8;
			
			if (clss == Float.class)
				return TypeCode.FLOAT4;
				
			if (clss == Double.class)
				return TypeCode.FLOAT8;
		}
		else if (clss == String.class)
		{
			String s = (String) value;
			
			if (s.length() == 0)
				return TypeCode.EMPTY_STRING;
			
			return TypeCode.STRING;
		}
		else if (clss == Boolean.class)
		{
			if ((Boolean) value)
				return TypeCode.BOOLEAN_TRUE;
			
			return TypeCode.BOOLEAN_FALSE;
		}
		else if (clss == StructValue.class)
		{
			return TypeCode.STRUCT;
		}
		else if (clss == ArrayValue.class)
		{
			return TypeCode.ARRAY;
		}
		else if (clss == byte[].class)
		{
			return TypeCode.BYTES;
		}
		else if (clss.isArray())
		{
			return TypeCode.ARRAY;
		}
		
		return TypeCode.CUSTOM;
	}

	@Override
	public Type getCustomStructType( Class<?> c )
	{
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Class<?> getCustomType( Type type )
	{
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public Class<?> getNativeType( byte type )
	{
		// TODO Auto-generated method stub
		return null;
	}

	@Override
	public byte getNativeTypeCode( Class<?> c )
	{
		// TODO Auto-generated method stub
		return 0;
	}
}
