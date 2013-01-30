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

import org.apache.etch.bindings.java.msg.ComboValidator;
import org.apache.etch.bindings.java.msg.Field;
import org.apache.etch.bindings.java.msg.Message;
import org.apache.etch.bindings.java.msg.StructValue;
import org.apache.etch.bindings.java.msg.Type;
import org.apache.etch.bindings.java.msg.Validator;
import org.apache.etch.bindings.java.msg.ValueFactory;
import org.apache.etch.bindings.java.support.Validator_int;
import org.apache.etch.bindings.java.support.Validator_object;
import org.apache.etch.bindings.java.support.Validator_string;
import org.apache.etch.bindings.java.transport.ArrayValue;
import org.apache.etch.bindings.java.transport.TaggedDataInput;
import org.apache.etch.bindings.java.transport.fmt.TypeCode;
import org.apache.etch.util.Assertion;
import org.apache.etch.util.FlexBuffer;


/**
 * BinaryTaggedDataInput has methods to support reading tagged
 * values from an input buffer.
 */
final public class BinaryTaggedDataInput extends BinaryTaggedData
	implements TaggedDataInput
{
	/**
	 * Constructs the BinaryTaggedDataInput with a null buffer.
	 * 
	 * @param vf the value factory for the service.
	 * @param uri the uri used to construct the transport stack.
	 */
	public BinaryTaggedDataInput( ValueFactory vf, String uri )
	{
		super( vf );
		// don't have anything to do with uri yet.
	}

	private FlexBuffer buf;
	
	private int lengthBudget;

	/////////////////////////////
	// TaggedDataInput methods //
	/////////////////////////////
	
	public Message readMessage( FlexBuffer buf ) throws IOException
	{
		this.buf = buf;
		
		// lengthBudget is how many array elements total are reasonable to
		// allocate while parsing this message. the largest value comes as each
		// byte of the incoming message turned into an array element. the total
		// will always be lower than this as often it takes multiple bytes to
		// make a single array element. so, each time we make an array, we
		// deduct the specified length from lengthBudget. if the result is
		// negative, then either the incoming message is misformed or someone is
		// trying to spoof us.
		
		lengthBudget = buf.avail();
		
		try
		{
			Message msg = startMessage();
			readKeysAndValues( msg );
			endMessage( msg );
			return msg;
		}
		finally
		{
			this.buf = null;
			lengthBudget = 0;
		}
	}

	private StructValue readStruct() throws IOException
	{
		StructValue sv = startStruct();
		readKeysAndValues( sv );
		endStruct( sv );
		return sv;
	}
	
	private ArrayValue readArray( Validator v ) throws IOException
	{
		ArrayValue av = startArray();
		readValues( av, v );
		endArray( av );
		return av;
	}

	private void readKeysAndValues( StructValue sv )
		throws IOException
	{
		Type t = sv.type();
		while (true)
		{
			Field key = readField( t );
			if (key == null)
				break;
			
			Validator v = t.getValidator( key );
			if (v != null)
			{
				sv.put( key, readValue( v ) );
			}
			else
			{
				// read the value but ignore it.
				Object obj = readValue( Validator_object.get( 0 ) );
				if (false)
					sv.put( key, obj );
			}
		}
	}

	private void readValues( ArrayValue av, Validator v ) throws IOException
	{
		Validator ev = v.elementValidator();
		while (true)
		{
			Object value = readValue( ev, true );
			if (value == NONE)
				break;
			
			av.add( value );
		}
	}

	////////////////////////
	// Main input methods //
	////////////////////////

	private Message startMessage() throws IOException
	{
		byte version = buf.getByte();
		
		if (version != VERSION)
			throw new IOException(
				String.format(
					"binary tagged data version mismatch: got %d expected %d",
					version, VERSION ) );
		
		Type t = readType();
		int length = readLength();
		
		return new Message( t, vf, length );
	}

	private void endMessage( Message msg )
	{
		// nothing to do, readKeysAndValues swallowed the NONE.
	}

	private StructValue startStruct() throws IOException
	{
		Type t = readType();
		int length = readLength();
		
		return new StructValue( t, vf, length );
	}

	private void endStruct( StructValue struct )
	{
		// nothing to do, readKeysAndValues swallowed the NONE.
	}

	@SuppressWarnings("deprecation")
	private ArrayValue startArray() throws IOException
	{
		byte type = buf.getByte();
		
		Type customStructType;
		if (type == TypeCode.CUSTOM || type == TypeCode.STRUCT)
			customStructType = readType();
		else
			customStructType = null;
		
		int dim = readIntegerValue();
		if (dim <= 0 || dim > Validator.MAX_NDIMS)
			throw new IllegalArgumentException(
				"dim <= 0 || dim > Validator.MAX_NDIMS" );
		
		int length = readLength();
		
		Object array = allocArrayValue( type, customStructType, dim, length );
		
		return new ArrayValue( array, type, customStructType, dim );
	}

	private void endArray( ArrayValue array )
	{
		array.compact();
	}
	
	private Type readType() throws IOException
	{
		Object obj = readValue( intOrStrValidator, false );
		
		if (obj instanceof Integer)
		{
			Integer id = (Integer) obj;
			Type type = vf.getType( id );
			
			if (type == null)
				type = new Type( id, id.toString() );
			
			return type;
		}
		
		Assertion.check( obj instanceof String, "obj instanceof String" );
		String name = (String) obj;
		Type type = vf.getType( name );
		
		if (type == null)
			type = new Type( name );
		
		return type;
	}

	private Field readField( Type type ) throws IOException
	{
		Object obj = readValue( intOrStrValidator, true );
		
		if (obj == NONE)
			return null;
		
		if (obj instanceof Integer)
		{
			Integer id = (Integer) obj;
			Field field = type.getField( id );
			
			if (field == null)
				field = new Field( id, id.toString() );
			
			return field;
		}
		
		Assertion.check( obj instanceof String, "obj instanceof String" );
		String name = (String) obj;
		Field field = type.getField( name );
		
		if (field == null)
			field = new Field( name );
		
		return field;
	}
	
	private final Validator intOrStrValidator =
		new ComboValidator( Validator_int.get( 0 ), Validator_string.get( 0 ) );
	
	private int readLength() throws IOException
	{
		int length = readIntegerValue();
		
		if (length < 0 || length > lengthBudget)
			throw new IllegalArgumentException(
				"length < 0 || length > lengthBudget" );
		
		lengthBudget -= length;
		
		return length;
	}
	
	private Integer readIntegerValue() throws IOException
	{
		return (Integer) readValue( intValidator );
	}
	
	private final Validator intValidator = Validator_int.get( 0 );

	///////////////////////////
	// LOCAL UTILITY METHODS //
	///////////////////////////
	
	private Object validateValue( Validator v, Object value )
	{
		// v == null more or less implies that a field is not known
		// for a type. thus we don't care about the field value as
		// we are going to ignore it. therefore, return null.
		if (v == null)
			return null;
		
		if (value == null)
			return null;
		
		return v.validateValue( value );
	}
	
	private Object validateValue( Validator v, boolean noneOk, Object value )
	{
		if (noneOk && value == NONE)
			return value;
		
		return validateValue( v, value );
	}
	
	private Object readValue( Validator v ) throws IOException
	{
		return readValue( v, false );
	}
	
	@SuppressWarnings("deprecation")
	private Object readValue( Validator v, boolean noneOk ) throws IOException
	{
		byte type = buf.getByte();
		
		switch (type)
		{
			case TypeCode.NULL:
				return validateValue( v, null );
			
			case TypeCode.NONE:
				return validateValue( v, noneOk, NONE );
			
			case TypeCode.BOOLEAN_FALSE:
				return validateValue( v, Boolean.FALSE );
			
			case TypeCode.BOOLEAN_TRUE:
				return validateValue( v, Boolean.TRUE );
			
			case TypeCode.BYTE:
				return validateValue( v, buf.getByte() );
			
			case TypeCode.SHORT:
				return validateValue( v, buf.getShort() );
			
			case TypeCode.INT:
				return validateValue( v, buf.getInt() );
			
			case TypeCode.LONG:
				return validateValue( v, buf.getLong() );
			
			case TypeCode.FLOAT:
				return validateValue( v, buf.getFloat() );
			
			case TypeCode.DOUBLE:
				return validateValue( v, buf.getDouble() );
				
			case TypeCode.BYTES:
				return validateValue( v, readBytes() );

//			reserved for future use:
//			case TypeCode.BOOLS:
//			case TypeCode.SHORTS:
//			case TypeCode.INTS:
//			case TypeCode.LONGS:
//			case TypeCode.FLOATS:
//			case TypeCode.DOUBLES:
//				throw new UnsupportedOperationException( "unsupported type code "+type );
				
			case TypeCode.ARRAY:
				return validateValue( v, fromArrayValue( readArray( v ) ) );
			
			case TypeCode.EMPTY_STRING:
				return validateValue( v, "" );
			
			case TypeCode.STRING:
				return validateValue( v, new String( readBytes(), vf.getStringEncoding() ) );
			
			case TypeCode.STRUCT:
			case TypeCode.CUSTOM:
				return validateValue( v, vf.importCustomValue( readStruct() ) );
			
			default:
				if (type >= TypeCode.MIN_TINY_INT && type <= TypeCode.MAX_TINY_INT)
					return validateValue( v, type );
			
				throw new UnsupportedOperationException( "unsupported type code "+type );
		}
	}
	
	private byte[] readBytes() throws IOException
	{
		int length = readLength();
		byte[] b = new byte[length];
		buf.getFully( b );
		return b;
	}
}
