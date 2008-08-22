/* $Id$
 *
 * Copyright 2007-2008 Cisco Systems Inc.
 *
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy
 * of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations
 * under the License.
 */

package etch.bindings.java.transport.fmt.xml;

import java.io.IOException;
import java.io.OutputStream;
import java.io.OutputStreamWriter;
import java.io.StringWriter;
import java.io.Writer;
import java.util.Map;
import java.util.Stack;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.msg.Validator.Level;
import etch.bindings.java.transport.ArrayValue;
import etch.bindings.java.transport.TaggedDataOutput;
import etch.bindings.java.transport.fmt.TypeCode;
import etch.util.FlexBuffer;
import etch.util.core.xml.XmlParser.DefaultTagElement;
import etch.util.core.xml.XmlParser.TagElement;

/**
 * Description of TaggedDataOutputStream.
 */
@SuppressWarnings("all")
final public class XmlTaggedDataOutput extends XmlTaggedData
	implements TaggedDataOutput, XmlTags
{
	public class FlexBufferOutputStream extends OutputStream
	{
		public FlexBufferOutputStream( FlexBuffer buf )
		{
			this.buf = buf;
		}
		
		private final FlexBuffer buf;

		@Override
		public void write( int b ) throws IOException
		{
			buf.put( b );
		}
	}

	private static final byte VERSION = 1;

	/**
	 * Constructs the XmlTaggedDataOutput.
	 * 
	 * @param vf the value factory for the service.
	 * @param uri the uri used to construct the transport stack.
	 */
	public XmlTaggedDataOutput( ValueFactory vf, String uri )
	{
		super( vf );
		level = vf.getLevel();
	}

	private final Level level;
	
	private final Stack<TagElement> stack = new Stack<TagElement>();
	
	public void writeMessage( Message msg, FlexBuffer buf ) throws IOException
	{
		try
		{
			startMessage( msg );
			writeKeysAndValues( msg );
			endMessage( msg, buf );
		}
		finally
		{
			stack.clear();
		}
	}

	private void writeStruct( StructValue sv ) throws IOException
	{
		startStruct( sv );
		writeKeysAndValues( sv );
		endStruct( sv );
	}

	private void writeArray( ArrayValue av, Validator v ) throws IOException
	{
		startArray( av );
		writeValues( av, v );
		endArray( av );
	}

	private void writeKeysAndValues( StructValue sv ) throws IOException
	{
		Type t = sv.type();
		for (Map.Entry<Field, Object> me: sv)
		{
			Field f = me.getKey();
			TagElement te = writeValue( t.getValidator( f ), me.getValue() );
			setFieldAttr( te, f );
		}
	}

	private void writeValues( ArrayValue av, Validator v )
		throws IOException
	{
		Validator ev = v.elementValidator();
		for (Object value: av)
			writeValue( ev, value );
	}
	
	/////////////////////////
	// Main output methods //
	/////////////////////////

	private void startMessage( Message msg )
	{
		TagElement te = new DefaultTagElement( MESSAGE_TAG );
		te.setAttr( null, VERSION_ATTR, VERSION );
		setTypeAttr( te, msg.type() );
		setSizeAttr( te, msg.size() );
		stack.push( te );
	}

	private void endMessage( Message msg, FlexBuffer buf ) throws IOException
	{
		TagElement te = stack.pop();
		if (!stack.isEmpty())
			throw new IOException( "end of message stack sequence error: "+stack );
		te.toString( new OutputStreamWriter( new FlexBufferOutputStream( buf ) ) );
	}

	private void startStruct( StructValue struct )
	{
		// the caller has already created an element tag for us.
		TagElement te = stack.peek();
		setTypeAttr( te, struct.type() );
		setSizeAttr( te, struct.size() );
	}

	private void endStruct( StructValue struct )
	{
		// the caller will pop this element off the stack
	}

	private void startArray( ArrayValue array )
	{
		// the caller has already created an element tag for us.
		TagElement te = stack.peek();
		byte type = array.typeCode();
		setTypeCodeAttr( te, type );
		if (type == TypeCode.CUSTOM)
			setTypeAttr( te, array.customStructType() );
		setDimAttr( te, array.dim() );
		setSizeAttr( te, array.size() );
	}

	private void endArray( ArrayValue array )
	{
		// the caller will pop this element off the stack
	}

	private void setTypeAttr( TagElement te, Type type )
	{
		te.setAttr( null, TYPE_ATTR, type.getName() );
	}

	private void setSizeAttr( TagElement te, int size )
	{
		te.setAttr( null, SIZE_ATTR, size );
	}

	private void setDimAttr( TagElement te, int dim )
	{
		te.setAttr( null, DIM_ATTR, dim );
	}

	private void setTypeCodeAttr( TagElement te, byte tc )
	{
		te.setAttr( null, TYPE_CODE_ATTR, tc );
	}

	private void setFieldAttr( TagElement te, Field field )
	{
		te.setAttr( null, FIELD_ATTR, field.getName() );
	}
	
	//////////////////////////////////////////////

	private TagElement writeValue( Validator validator, Object value )
	{
		// TODO Auto-generated method stub
		return null;
	}

	private void writeStructElement( Field key, Object value ) throws IOException
	{
		TagElement te = stack.peek().addTag( ELEMENT_TAG );
		te.setAttr( null, KEY_ATTR, key.getId() );
		stack.push( te );
		writeValue( value );
		stack.pop();
	}

	private void writeArrayElement( Object value ) throws IOException
	{
		TagElement te = stack.peek().addTag( ELEMENT_TAG );
		stack.push( te );
		writeValue( value );
		stack.pop();
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
}
