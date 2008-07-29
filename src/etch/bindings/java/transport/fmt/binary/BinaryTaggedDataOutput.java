/* $Id$
 *
 * Created by sccomer on Jan 17, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport.fmt.binary;

import java.io.IOException;
import java.util.ArrayList;
import java.util.List;

import metreos.util.FlexBuffer;
import metreos.util.URL;
import etch.bindings.java.msg.ArrayValue;
import etch.bindings.java.msg.Field;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.TaggedDataOutput;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator;
import etch.bindings.java.msg.Validator_int;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Description of TaggedDataOutputStream.
 */
final public class BinaryTaggedDataOutput extends BinaryTaggedData
	implements TaggedDataOutput
{
	/**
	 * Constructs the BinaryTaggedDataOutput.
	 *
	 * @param buf the output buffer.
	 * @param vf the value factory for the service. 
	 */
	public BinaryTaggedDataOutput( FlexBuffer buf, ValueFactory vf )
	{
		super( vf );
		this.buf = buf;
	}
	
	/**
	 * Constructs the BinaryTaggedDataOutput with a null buffer.
	 * 
	 * @param vf the value factory for the service.
	 * @param uri the uri used to construct the transport stack.
	 * 
	 * @see #setBuffer(FlexBuffer)
	 */
	public BinaryTaggedDataOutput( ValueFactory vf, URL uri )
	{
		super( vf );
		// don't have anything to do with uri yet.
	}

	private FlexBuffer buf;

	//////////////////////////////
	// TaggedDataOutput methods //
	//////////////////////////////

	public void startMessage( Message msg ) throws IOException
	{
		// since message is top-level, no type code is written to
		// indicate a message is starting. we do write a version
		// number to indicate this version of the binary tagged
		// data output.
		structs.clear();
		buf.putByte( VERSION );
		writeStructType( msg.type() );
		pushStruct( msg );
	}

	public void endMessage( Message msg ) throws IOException
	{
		writeValue( noneValidator, NONE );
		popStruct( msg );
		assert structs.isEmpty();
	}

	public void startStruct( StructValue struct ) throws IOException
	{
		// the caller has already written a type code to indicate a
		// struct is starting.
		writeStructType( struct.type() );
		pushStruct( struct );
	}

	public void writeStructElement( Field key, Object value ) throws IOException
	{
		Validator v = currentType.getValidator( key );
		writeValue( intValidator, key.getId() );
		writeValue( v, value );
	}

	public void endStruct( StructValue struct ) throws IOException
	{
		writeValue( noneValidator, NONE );
		popStruct( struct );
	}
	
	private void pushStruct( StructValue struct )
	{
		structs.add( currentStruct );
		currentStruct = struct;
		currentType = currentStruct.type();
	}
	
	private void popStruct( StructValue struct )
	{
		assert currentStruct == struct;
		currentStruct = structs.remove( structs.size()-1 );
		currentType = currentStruct != null ? currentStruct.type() : null;
	}
	
	private final List<StructValue> structs = new ArrayList<StructValue>();
	
	private StructValue currentStruct;
	
	private Type currentType;

	public void startArray( ArrayValue array ) throws IOException
	{
		// the caller has already written a type code to indicate an
		// array is starting.
		byte type = array.typeCode();
		buf.putByte( type );
		if (type == TypeCode.CUSTOM)
			writeStructType( array.customStructType() );
		writeValue( intValidator, array.dim() );
	}
	
	private void writeStructType( Type type ) throws IOException
	{
		writeValue( intValidator, type.getId() );
	}

	public void writeArrayElement( Object value ) throws IOException
	{
		writeValue( arrayElementValidator, value );
	}
	
	private Validator arrayElementValidator;

	public void endArray( ArrayValue array ) throws IOException
	{
		writeValue( noneValidator, NONE );
	}
	
	private final Validator intValidator = Validator_int.get( 0 );
	
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
	public void writeValue( Validator v, Object value ) throws IOException
	{
//		System.out.printf( "writeValue %s: %s\n", value, v );
		
		byte type = checkValue( value, v );
		
		if (type == TypeCode.ANY)
			type = checkValue( value );
		
		// xTODO disable this, we don't need it except as an
		// extra safety net during development.
		if (false)
		{
			if (v == null)
				throw new IOException( String.format(
					"value %s (class %s) has no validator",
					value, value.getClass() ) );
			
			if (value != null && !v.validate( value ))
				throw new IllegalArgumentException( String.format(
						"value %s (class %s) not valid for validator %s",
						value, value.getClass(), v ) );
			
			byte oldType = checkValue( value );
			if (type != oldType)
				throw new IOException( String.format(
					"value %s (class %s) type code %d, old type code %d",
					value, value != null ? value.getClass() : null, type, oldType ) );
		}
		
		buf.putByte( type );

		switch (type)
		{
			case TypeCode.NULL:
			case TypeCode.BOOLEAN_FALSE:
			case TypeCode.BOOLEAN_TRUE:
			case TypeCode.EMPTY_STRING:
			case TypeCode.NONE:
				return;
			
			case TypeCode.BYTE1:
			case TypeCode.SHORT1:
			case TypeCode.INT1:
			case TypeCode.LONG1:
				buf.putByte( ((Number) value).byteValue() );
				return;
			
			case TypeCode.SHORT2:
			case TypeCode.INT2:
			case TypeCode.LONG2:
				buf.putShort( ((Number) value).shortValue() );
				return;
			
			case TypeCode.INT4:
			case TypeCode.LONG4:
				buf.putInt( ((Number) value).intValue() );
				return;
			
			case TypeCode.LONG8:
				buf.putLong( ((Number) value).longValue() );
				return;
			
			case TypeCode.FLOAT4:
				buf.putFloat( ((Number) value).floatValue() );
				return;
			
			case TypeCode.FLOAT8:
				buf.putDouble( ((Number) value).doubleValue() );
				return;
			
			case TypeCode.BYTES:
				writeBytes( (byte[]) value );
				return;
			
			case TypeCode.STRING:
				writeBytes( ((String) value).getBytes( vf.getStringEncoding() ) );
				return;
			
			case TypeCode.STRUCT:
				((StructValue) value).writeStruct( this );
				return;
			
			case TypeCode.ARRAY:
			{
				Validator oldArrayElementValidator = arrayElementValidator;
				arrayElementValidator = v != null ? v.elementValidator() : null;
				
				if (value instanceof ArrayValue)
					((ArrayValue) value).write( this );
				else
					toArrayValue( value ).write( this );
				
				arrayElementValidator = oldArrayElementValidator;
				return;
			}
				
			case TypeCode.CUSTOM:
			{
				StructValue struct = vf.exportCustomValue( value );
				
				if (struct == null)
					throw new UnsupportedOperationException( "unsupported type "+value.getClass() );
				
				struct.writeStruct( this );
				return;
			}
			
			default:
				// type is either "small" integer or unused...
				return;
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
		writeValue( intValidator, n );
		buf.put( value, 0, n );
	}

	/**
	 * @param msg
	 * @param vf the value factory for the service. 
	 * @return the bytes of the message
	 * @throws IOException
	 */
	public static byte[] getBytes( Message msg, ValueFactory vf ) throws IOException
	{
		FlexBuffer buf = new FlexBuffer();
		BinaryTaggedDataOutput tdo = new BinaryTaggedDataOutput( buf, vf );
		msg.writeMessage( tdo );
		return buf.getAvailBytes();
	}

	/**
	 * @param buf
	 */
	public void setBuffer( FlexBuffer buf )
	{
		this.buf = buf;
	}
}
