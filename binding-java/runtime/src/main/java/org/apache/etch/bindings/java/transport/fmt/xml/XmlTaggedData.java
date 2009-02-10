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

package org.apache.etch.bindings.java.transport.fmt.xml;

import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.transport.ArrayValue;
import org.apache.etch.bindings.java.transport.fmt.TaggedData;
import org.apache.etch.bindings.java.transport.fmt.TypeCode;

/**
 * XML tagged data implementation.
 */
public class XmlTaggedData extends TaggedData
{
	/**
	 * @param vf
	 */
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
				return TypeCode.BYTE;
			
			if (clss == Short.class)
				return TypeCode.SHORT;
				
			if (clss == Integer.class)
				return TypeCode.INT;
			
			if (clss == Long.class)
				return TypeCode.LONG;
			
			if (clss == Float.class)
				return TypeCode.FLOAT;
				
			if (clss == Double.class)
				return TypeCode.DOUBLE;
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
			return TypeCode.CUSTOM;
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
		// TODO implement getCustomStructType function
		return null;
	}

	@Override
	public Class<?> getNativeType( byte type )
	{
		// TODO implement getNativeType function
		return null;
	}

	@Override
	public byte getNativeTypeCode( Class<?> c )
	{
		// TODO implement getNativeTypeCode function
		return 0;
	}
}
