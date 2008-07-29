/* $Id$
 *
 * Created by sccomer on Jan 17, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport.fmt.binary;

import java.io.IOException;

import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.support.Validator_int;
import etch.bindings.java.transport.ArrayValue;
import etch.bindings.java.transport.TaggedDataInput;
import etch.bindings.java.transport.fmt.TypeCode;
import etch.util.FlexBuffer;

/**
 * BinaryTaggedDataInput has methods to support reading tagged
 * values from an input buffer.
 */
final public class BinaryTaggedDataInput extends BinaryTaggedData
	implements TaggedDataInput
{
	/**
	 * Constructs the BinaryTaggedDataInput.
	 * 
	 * @param vf the value factory for the service.
	 */
	private BinaryTaggedDataInput( ValueFactory vf )
	{
		super( vf );
	}
	
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

	/////////////////////////////
	// TaggedDataInput methods //
	/////////////////////////////
	
	public Message readMessage( FlexBuffer buf ) throws IOException
	{
		this.buf = buf;
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
			Object obj = readValue( intValidator, true );
			if (obj == NONE)
				break;
			
			Integer id = (Integer) obj;
			Field key = t.getField( id );
			
			Validator v = t.getValidator( key );
			Object value = readValue( v );
			
			sv.put( key, value );
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
		int length = readIntegerValue();
		
		return new Message( t, length, vf );
	}

	private void endMessage( Message msg )
	{
		// nothing to do, readKeysAndValues swallowed the NONE.
	}

	private StructValue startStruct() throws IOException
	{
		Type t = readType();
		int length = readIntegerValue();
		return new StructValue( t, length );
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
		
		int length = readIntegerValue();
		
		Object array = allocArrayValue( type, customStructType, dim, length );
		
		return new ArrayValue( array, type, customStructType, dim );
	}

	private void endArray( ArrayValue array )
	{
		array.compact();
	}
	
	private Type readType() throws IOException
	{
		Integer id = readIntegerValue();
		Type t = vf.getType( id );
		if (t == null)
			throw new IllegalArgumentException( "unknown type "+id );
		return t;
	}
	
	private Integer readIntegerValue() throws IOException
	{
		return (Integer) readValue( intValidator );
	}
	
	private final Validator intValidator = Validator_int.get( 0 );

	///////////////////////////
	// LOCAL UTILITY METHODS //
	///////////////////////////
	
	private Object validateValue( Validator v, boolean noneOk, Object value )
		throws IOException
	{
		// v == null more or less implies that a field is not known
		// for a type. thus we don't care about the field value as
		// we are going to ignore it. therefore, return null.
		if (v == null)
			return null;
//			throw new IOException( "no validator" );
		
		if (value == null)
			return null;
		
		if (noneOk && value == NONE)
			return value;
		
		return v.validateValue( value );
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
				return validateValue( v, false, null );
			
			case TypeCode.NONE:
				return validateValue( v, noneOk, NONE );
			
			case TypeCode.BOOLEAN_FALSE:
				return validateValue( v, false, Boolean.FALSE );
			
			case TypeCode.BOOLEAN_TRUE:
				return validateValue( v, false, Boolean.TRUE );
			
			case TypeCode.BYTE:
				return validateValue( v, false, buf.getByte() );
			
			case TypeCode.SHORT:
				return validateValue( v, false, buf.getShort() );
			
			case TypeCode.INT:
				return validateValue( v, false, buf.getInt() );
			
			case TypeCode.LONG:
				return validateValue( v, false, buf.getLong() );
			
			case TypeCode.FLOAT:
				return validateValue( v, false, buf.getFloat() );
			
			case TypeCode.DOUBLE:
				return validateValue( v, false, buf.getDouble() );
				
			case TypeCode.BYTES:
				return validateValue( v, false, readBytes() );

//			reserved for future use:
//			case TypeCode.BOOLS:
//			case TypeCode.SHORTS:
//			case TypeCode.INTS:
//			case TypeCode.LONGS:
//			case TypeCode.FLOATS:
//			case TypeCode.DOUBLES:
//				throw new UnsupportedOperationException( "unsupported type code "+type );
				
			case TypeCode.ARRAY:
				return validateValue( v, false, fromArrayValue( readArray( v ) ) );
			
			case TypeCode.EMPTY_STRING:
				return validateValue( v, false, "" );
			
			case TypeCode.STRING:
				return validateValue( v, false, new String( readBytes(), vf.getStringEncoding() ) );
			
			case TypeCode.STRUCT:
			case TypeCode.CUSTOM:
				return validateValue( v, false, vf.importCustomValue( readStruct() ) );
			
			default:
				if (type >= TypeCode.MIN_TINY_INT && type <= TypeCode.MAX_TINY_INT)
					return validateValue( v, false, type );
			
				throw new UnsupportedOperationException( "unsupported type code "+type );
		}
	}
	
	private byte[] readBytes() throws IOException
	{
		int n = readIntegerValue();
		byte[] b = new byte[n];
		buf.getFully( b );
		return b;
	}

	/**
	 * Reads a Message from the data buffer.
	 * @param vf the value factor for the message.
	 * @param buf the data buffer.
	 * @return the message read.
	 * @throws IOException if there is a problem reading the message.
	 */
	public static Message readMessage( ValueFactory vf, byte[] buf )
		throws IOException
	{
		return readMessage( vf, new FlexBuffer( buf ) );
	}
	
	/**
	 * Reads a Message from the data buffer.
	 * @param vf the value factor for the message.
	 * @param buf the data buffer.
	 * @param off offset to start of message.
	 * @param len length of message.
	 * @return the message read.
	 * @throws IOException if there is a problem reading the message.
	 */
	public static Message readMessage( ValueFactory vf, byte[] buf, int off,
		int len ) throws IOException
	{
		return readMessage( vf, new FlexBuffer( buf, off, len ) );
	}
	
	/**
	 * Reads a Message from the flex buffer.
	 * @param vf
	 * @param buf
	 * @return the message read.
	 * @throws IOException
	 */
	public static Message readMessage( ValueFactory vf, FlexBuffer buf )
		throws IOException
	{
		BinaryTaggedDataInput tdi = new BinaryTaggedDataInput( vf );
		return tdi.readMessage( buf );
	}
}
