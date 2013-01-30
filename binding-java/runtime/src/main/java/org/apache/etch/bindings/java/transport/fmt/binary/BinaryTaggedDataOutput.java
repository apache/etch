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

import java.io.IOException;
import java.util.Map;

import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.Validator;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.msg.Validator.Level;
import org.apache.etch.bindings.java.support.Validator_int;
import org.apache.etch.bindings.java.support.Validator_string;
import org.apache.etch.bindings.java.transport.ArrayValue;
import org.apache.etch.bindings.java.transport.TaggedDataOutput;
import org.apache.etch.bindings.java.transport.fmt.TypeCode;
import org.apache.etch.util.FlexBuffer;
import org.apache.etch.util.URL;


/**
 * An implementation of TaggedDataOutput which uses a binary encoding.
 */
final public class BinaryTaggedDataOutput extends BinaryTaggedData
	implements TaggedDataOutput
{
	/**
	 * Name of uri parameter which controls whether we write ints or strings
	 * for types and fields.
	 */
	public static final String STRING_TYPE_AND_FIELD =
		"BinaryTaggedDataOutput.stringTypeAndField";

	/**
	 * Constructs the BinaryTaggedDataOutput.
	 * 
	 * @param vf the value factory for the service.
	 * @param uri the uri used to construct the transport stack.
	 */
	public BinaryTaggedDataOutput( ValueFactory vf, String uri )
	{
		super( vf );
		level = vf.getLevel();
		URL u = new URL( uri );
		stringTypeAndField = u.getBooleanTerm( STRING_TYPE_AND_FIELD, false );
	}

	private final Level level;
	
	private final boolean stringTypeAndField;

	//////////////////////////////
	// TaggedDataOutput methods //
	//////////////////////////////

	public void writeMessage( Message msg, FlexBuffer buf ) throws IOException
	{
		try
		{
			this.buf = buf;
			startMessage( msg );
			writeKeysAndValues( msg );
			endMessage( msg );
		}
		finally
		{
			this.buf = null;
		}
	}

	private FlexBuffer buf;

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
			writeField( f );
			writeValue( t.getValidator( f ), me.getValue() );
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

	private void startMessage( Message msg ) throws IOException
	{
		// since message is top-level, no type code is written to
		// indicate a message is starting. we do write a version
		// number to indicate this version of the binary tagged
		// data output format.
		buf.putByte( VERSION );
		startStruct( msg );
	}

	private void endMessage( Message msg ) throws IOException
	{
		endStruct( msg );
	}

	private void startStruct( StructValue struct ) throws IOException
	{
		// the caller has already written a type code to indicate a
		// struct is starting. this code is shared by startMessage().
		writeType( struct.type() );
		writeIntValue( struct.size() );
	}

	private void endStruct( StructValue struct ) throws IOException
	{
		// this code is shared by endMessage.
		writeNoneValue();
	}

	private void startArray( ArrayValue array ) throws IOException
	{
		// the caller has already written a type code to indicate an
		// array is starting.
		byte type = array.typeCode();
		buf.putByte( type );
		if (type == TypeCode.CUSTOM)
			writeType( array.customStructType() );
		writeIntValue( array.dim() );
		writeIntValue( array.size() );
	}

	private void endArray( ArrayValue array ) throws IOException
	{
		writeNoneValue();
	}
	
	private void writeType( Type type ) throws IOException
	{
		if (stringTypeAndField)
			writeStringValue( type.getName() );
		else
			writeIntValue( type.getId() );
	}
	
	private void writeField( Field field ) throws IOException
	{
		if (stringTypeAndField)
			writeStringValue( field.getName() );
		else
			writeIntValue( field.getId() );
	}

	private void writeIntValue( Integer value ) throws IOException
	{
		writeValue( intValidator, value );
	}
	
	private final Validator intValidator = Validator_int.get( 0 );
	
	private void writeStringValue( String value ) throws IOException
	{
		writeValue( stringValidator, value );
	}
	
	private final Validator stringValidator = Validator_string.get( 0 );
	
	private void writeNoneValue() throws IOException
	{
		writeValue( noneValidator, NONE );
	}
	
	private final Validator noneValidator = Validator_none.get();

	///////////////////////////
	// LOCAL UTILITY METHODS //
	///////////////////////////

	/**
	 * @param v the validator for this value, or none if there
	 * or none if there isn't one.
	 * @param value
	 * @throws IOException
	 */
	private void writeValue( Validator v, Object value ) throws IOException
	{
//		System.out.printf( "writeValue %s: %s\n", value, v );
		
		byte typeCode = checkValue( value );
		
		if (level != Level.NONE && value != null)
		{
		    if (level == Level.FULL && v == null)
		    	throw new IllegalArgumentException(
		    		"validator missing" );
				
			if (v != null && !v.validate( value ))
				throw new IllegalArgumentException( String.format(
					"validator %s failed for value %s",
		    		v, value ) );
		}
		
		buf.putByte( typeCode );

		switch (typeCode)
		{
			case TypeCode.NULL:
			case TypeCode.NONE:
			case TypeCode.BOOLEAN_FALSE:
			case TypeCode.BOOLEAN_TRUE:
			case TypeCode.EMPTY_STRING:
				return;
			
			case TypeCode.BYTE:
				buf.putByte( ((Number) value).byteValue() );
				return;
			
			case TypeCode.SHORT:
				buf.putShort( ((Number) value).shortValue() );
				return;
			
			case TypeCode.INT:
				buf.putInt( ((Number) value).intValue() );
				return;
			
			case TypeCode.LONG:
				buf.putLong( ((Number) value).longValue() );
				return;
			
			case TypeCode.FLOAT:
				buf.putFloat( ((Number) value).floatValue() );
				return;
			
			case TypeCode.DOUBLE:
				buf.putDouble( ((Number) value).doubleValue() );
				return;
			
			case TypeCode.BYTES:
				writeBytes( (byte[]) value );
				return;

//			reserved for future use:
//			case TypeCode.BOOLS:
//			case TypeCode.SHORTS:
//			case TypeCode.INTS:
//			case TypeCode.LONGS:
//			case TypeCode.FLOATS:
//			case TypeCode.DOUBLES:
//				throw new UnsupportedOperationException( "unsupported type code "+type );
			
			case TypeCode.ARRAY:
				writeArray( toArrayValue( value, v ), v );
				return;
			
			case TypeCode.STRING:
				writeBytes( ((String) value).getBytes( vf.getStringEncoding() ) );
				return;
			
			case TypeCode.CUSTOM:
			{
				StructValue struct = vf.exportCustomValue( value );
				
				if (struct == null)
					throw new UnsupportedOperationException( "unsupported type "+value.getClass() );
				
				writeStruct( struct );
				return;
			}
			
			default:
				// type is either "tiny" integer or unused...
				
				if (typeCode >= TypeCode.MIN_TINY_INT && typeCode <= TypeCode.MAX_TINY_INT)
					return;
			
				throw new UnsupportedOperationException( "unsupported type code "+typeCode );
		}
	}

	/**
	 * Writes a value (signed or unsigned) to the output stream.
	 * @param value the value to write to the output stream.
	 * @throws IOException if there is a problem with the output stream.
	 */
	private void writeBytes( byte[] value ) throws IOException
	{
		int n = value.length;
		writeIntValue( n );
		buf.put( value, 0, n );
	}
}
