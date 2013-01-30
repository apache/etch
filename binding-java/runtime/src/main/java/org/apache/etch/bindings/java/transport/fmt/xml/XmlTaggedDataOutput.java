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

import java.io.IOException;
import java.io.StringWriter;
import java.io.Writer;
import java.util.Stack;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Validator;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.transport.ArrayValue;
import org.apache.etch.bindings.java.transport.TaggedDataOutput;
import org.apache.etch.bindings.java.transport.fmt.TypeCode;
import org.apache.etch.util.FlexBuffer;
import org.apache.etch.util.core.xml.XmlParser.DefaultTagElement;
import org.apache.etch.util.core.xml.XmlParser.TagElement;


/**
 * Description of TaggedDataOutputStream.
 */
@SuppressWarnings("all")
final public class XmlTaggedDataOutput extends XmlTaggedData
	implements TaggedDataOutput, XmlTags
{
	/**
	 * Constructs the TaggedDataOutputStream.
	 *
	 * @param wtr
	 * @param vf
	 */
	public XmlTaggedDataOutput( Writer wtr, ValueFactory vf )
	{
		super( vf );
		this.wtr = wtr;
	}
	
	private final Writer wtr;
	
	private final Stack<TagElement> stack = new Stack<TagElement>();

	public void startMessage( Message msg )
	{
		stack.clear();
		TagElement te = new DefaultTagElement( MESSAGE_TAG );
		te.setAttr( null, STRUCT_TYPE_ATTR, msg.type().getId() );
		stack.push( te );
	}

	public void endMessage( Message msg ) throws IOException
	{
		TagElement te = stack.pop();
		if (!stack.isEmpty())
			throw new IOException( "stack sequence error: "+stack );
		te.toString( wtr );
	}

	public void startStruct( StructValue struct )
	{
		// the caller has already created an element tag for us.
		TagElement te = stack.peek();
		te.setAttr( null, STRUCT_TYPE_ATTR, struct.type().getId() );
	}

	public void endStruct( StructValue struct )
	{
		// stack.pop();
	}

	public void startArray( ArrayValue array )
	{
		// the caller has already created an element tag for us.
		// TagElement te = stack.peek();
	}

	public void endArray( ArrayValue array )
	{
		// stack.pop();
	}

	public void writeStructElement( Field key, Object value ) throws IOException
	{
		TagElement te = stack.peek().addTag( ELEMENT_TAG );
		te.setAttr( null, KEY_ATTR, key.getId() );
		stack.push( te );
		writeValue( value );
		stack.pop();
	}

	public void writeArrayElement( Object value ) throws IOException
	{
		TagElement te = stack.peek().addTag( ELEMENT_TAG );
		stack.push( te );
		writeValue( value );
		stack.pop();
	}
	
	public void close() throws IOException
	{
		wtr.close();
	}
	
	@SuppressWarnings("deprecation")
	private void writeValue( Object value ) throws IOException
	{
		// the caller has already created an element for us.
		
		TagElement te = stack.peek();
		
		int type = checkValue( value );
		
		switch (type)
		{
			case TypeCode.NULL:
			{
				te.setAttr( null, ELEMENT_TYPE_ATTR, NULL_TYPE );
				break;
			}
			
			case TypeCode.BOOLEAN_FALSE:
			case TypeCode.BOOLEAN_TRUE:
			{
				te.setAttr( null, ELEMENT_TYPE_ATTR, BOOLEAN_TYPE );
				te.addCdata( value.toString() );
				break;
			}
			
			case TypeCode.BYTE:
			case TypeCode.SHORT:
			case TypeCode.INT:
			{
				te.setAttr( null, ELEMENT_TYPE_ATTR, INTEGER_TYPE );
				te.addCdata( value.toString() );
				break;
			}
			
			case TypeCode.LONG:
			{
				te.setAttr( null, ELEMENT_TYPE_ATTR, LONG_TYPE );
				te.addCdata( value.toString() );
				break;
			}
			
			case TypeCode.FLOAT:
			{
				te.setAttr( null, ELEMENT_TYPE_ATTR, FLOAT_TYPE );
				te.addCdata( value.toString() );
				break;
			}
			
			case TypeCode.DOUBLE:
			{
				te.setAttr( null, ELEMENT_TYPE_ATTR, DOUBLE_TYPE );
				te.addCdata( value.toString() );
				break;
			}
			
			case TypeCode.BYTES:
			{
				byte[] b = (byte[]) value;
				te.setAttr( null, ELEMENT_TYPE_ATTR, BYTES_TYPE );
				te.addCdata( bytes2string( b ) );
				break;
			}
			
			case TypeCode.EMPTY_STRING:
			case TypeCode.STRING:
			{
				String s = (String) value;
				te.setAttr( null, ELEMENT_TYPE_ATTR, STRING_TYPE );
				te.addCdata( s );
				break;
			}
			
			case TypeCode.ARRAY:
				te.setAttr( null, ELEMENT_TYPE_ATTR, ARRAY_TYPE );
				if (value instanceof ArrayValue)
					writeArray( (ArrayValue) value, null );
				else
					writeArray( toArrayValue( value, null ), null );
				break;
				
			case TypeCode.STRUCT:
			case TypeCode.CUSTOM:
			{
				StructValue struct = vf.exportCustomValue( value );
				
				if (struct == null)
					throw new UnsupportedOperationException( "unsupported type "+value.getClass() );
				
				te.setAttr( null, ELEMENT_TYPE_ATTR, CUSTOM_TYPE );
				writeStruct( struct );
				break;
			}
			
			default:
				throw new IllegalArgumentException( "unsupported type code "+type );
		}
	}

	private String bytes2string( byte[] buf )
	{
		StringBuffer sb = new StringBuffer();
		for (byte b: buf)
		{
			if (sb.length() > 0)
				sb.append( ' ' );
			sb.append( Integer.toString( b, 16 ) );
		}
		return sb.toString();
	}

	/**
	 * @param msg
	 * @param vf
	 * @return the xml of the specified message.
	 * @throws IOException
	 */
	public static String getXml( Message msg, ValueFactory vf ) throws IOException
	{
		StringWriter sw = new StringWriter();
		TaggedDataOutput tdo = new XmlTaggedDataOutput( sw, vf );
		tdo.writeMessage( msg, null );
		return sw.toString();
	}

	public void setBuffer( FlexBuffer msgBuf )
	{
		// TODO implement setBuffer function
		
	}

	public void writeMessage( Message msg, FlexBuffer buf ) throws IOException
	{
		// TODO implement writeMessage function
		
	}

	private void writeStruct( StructValue sv ) throws IOException
	{
		// TODO implement writeStruct function
		
	}

	private void writeArray( ArrayValue av, Validator v ) throws IOException
	{
		// TODO implement writeArray function
		
	}
}
