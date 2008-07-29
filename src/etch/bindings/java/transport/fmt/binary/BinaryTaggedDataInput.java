/* $Id$
 *
 * Created by sccomer on Jan 17, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport.fmt.binary;

import java.io.IOException;

import metreos.util.FlexBuffer;
import metreos.util.URL;
import etch.bindings.java.msg.ArrayValue;
import etch.bindings.java.msg.Message;
import etch.bindings.java.msg.StructValue;
import etch.bindings.java.msg.TaggedDataInput;
import etch.bindings.java.msg.Type;
import etch.bindings.java.msg.Validator;
import etch.bindings.java.msg.Validator_int;
import etch.bindings.java.msg.ValueFactory;
import etch.bindings.java.support.MessageSource;
import etch.bindings.java.transport.fmt.TypeCode;

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
	 * @param buf the input buffer.
	 */
	public BinaryTaggedDataInput( ValueFactory vf, FlexBuffer buf )
	{
		super( vf );
		this.buf = buf;
	}
	
	/**
	 * Constructs the BinaryTaggedDataInput with a null buffer.
	 * 
	 * @param vf the value factory for the service.
	 * @param uri the uri used to construct the transport stack.
	 * 
	 * @see #setBuffer(FlexBuffer)
	 */
	public BinaryTaggedDataInput( ValueFactory vf, URL uri )
	{
		super( vf );
		// don't have anything to do with uri yet.
	}

	private FlexBuffer buf;

	/**
	 * @param buf
	 */
	public void setBuffer( FlexBuffer buf )
	{
		this.buf = buf;
	}

	public Message startMessage() throws IOException
	{
		byte version = buf.getByte();
		
		if (version != VERSION)
			throw new IOException(
				String.format(
					"binary tagged data version mismatch: got %d expected %d",
					version, VERSION ) );
		
		Type type = readStructType();
		return new Message( type, vf );
	}

	public void endMessage( Message msg )
	{
		// nothing to do.
	}

	public StructValue startStruct() throws IOException
	{
		Type type = readStructType();
		return new StructValue( type );
	}
	
	private Type readStructType() throws IOException
	{
		Integer id = (Integer) readValue( intValidator );
		return vf.getType( id );
	}

	public boolean readStructElement( StructElement se ) throws IOException
	{
		Object obj = readValueOrNone( intValidator );
		if (obj == NONE)
			return false;
		
		Integer id = (Integer) obj;
		se.key = vf.getField( id );
		
		Validator v = currentType != null ? currentType.getValidator( se.key ) : null;
		se.value = readValue( v );
		return true;
	}
	
	private Type currentType;

	public void endStruct( StructValue struct )
	{
		// nothing to do.
	}

	public ArrayValue startArray()
		throws IOException
	{
		byte type = buf.getByte();
		
		Type customStructType;
		if (type == TypeCode.CUSTOM)
			customStructType = readStructType();
		else
			customStructType = null;
		
		int dim = (Integer) readValue( intValidator );
		
//		System.out.printf( "startArray (input) %d %s %d\n", type, customStructType, dim );
		
		return new ArrayValue( type, customStructType, dim );
	}

	public boolean readArrayElement( ArrayElement ae )
		throws IOException
	{
		ae.value = readValueOrNone( arrayElementValidator );
		if (ae.value == NONE)
			return false;
		return true;
	}
	
	private Validator arrayElementValidator;

	public void endArray( ArrayValue array )
	{
		// nothing to do.
	}

	/**
	 * Reads a Message from the data buffer.
	 * @param ms the message source.
	 * @param vf the value factor for the message.
	 * @param buf the data buffer.
	 * @return the message read.
	 * @throws IOException if there is a problem reading the message.
	 */
	public static Message readMessage( ValueFactory vf, byte[] buf,
		MessageSource ms ) throws IOException
	{
		return readMessage( vf, new FlexBuffer( buf ), ms );
	}
	
	/**
	 * Reads a Message from the data buffer.
	 * @param ms the message source.
	 * @param vf the value factor for the message.
	 * @param buf the data buffer.
	 * @param off offset to start of message.
	 * @param len length of message.
	 * @return the message read.
	 * @throws IOException if there is a problem reading the message.
	 */
	public static Message readMessage( ValueFactory vf, byte[] buf, int off,
		int len, MessageSource ms ) throws IOException
	{
		return readMessage( vf, new FlexBuffer( buf, off, len ), ms );
	}
	
	/**
	 * @param ms
	 * @param vf
	 * @param buf
	 * @return the message read.
	 * @throws IOException
	 */
	public static Message readMessage( ValueFactory vf, FlexBuffer buf,
		MessageSource ms ) throws IOException
	{
		BinaryTaggedDataInput tdi = new BinaryTaggedDataInput( vf, buf );
		return tdi.readMessage();
	}
	
	public Message readMessage() throws IOException
	{
		Message msg = startMessage();
		readKeysAndValues( msg );
		endMessage( msg );
		return msg;
	}
	
//	/**
//	 * @return a signed byte (-128..127) from the input stream.
//	 * @throws IOException if there is a problem with the input stream.
//	 */
//	private byte readByte() throws IOException
//	{
//		int b = is.read();
//		// is.read() returns 0-255 value or -1 if eof
//		if (b < 0)
//			throw new EOFException();
//		return (byte) b;
//	}

//	/**
//	 * @return an unsigned byte (0..255) from the input stream.
//	 * @throws IOException if there is a problem with the input stream.
//	 */
//	private short readUByte() throws IOException
//	{
//		int b = is.read();
//		// is.read() returns 0-255 value or -1 if eof
//		if (b < 0)
//			throw new EOFException();
//		return (short) b;
//	}
	
//	/**
//	 * @return a signed short from the input stream.
//	 * @throws IOException if there is a problem with the input stream.
//	 */
//	private short readShort() throws IOException
//	{
//		short a = readUByte();
//		short b = readUByte();
//		return (short) (a | (b << 8));
//	}
	
//	/**
//	 * @return an unsigned short from the input stream.
//	 * @throws IOException if there is a problem with the input stream.
//	 */
//	private int readUShort() throws IOException
//	{
//		int a = readUByte();
//		int b = readUByte();
//		return a | (b << 8);
//	}

//	/**
//	 * @return a signed int from the input stream.
//	 * @throws IOException
//	 */
//	public int readInt() throws IOException
//	{
//		int a = readUShort();
//		int b = readUShort();
//		return a | (b << 16);
//	}
	
//	/**
//	 * @return an unsigned int from the input stream.
//	 * @throws IOException if there is a problem with the input stream.
//	 */
//	private long readUInt() throws IOException
//	{
//		long a = readUShort();
//		long b = readUShort();
//		return a | (b << 16);
//	}
	
//	/**
//	 * @return a signed long from the input stream.
//	 * @throws IOException if there is a problem with the input stream.
//	 */
//	private long readLong() throws IOException
//	{
//		long a = readUInt();
//		long b = readUInt();
//		return a | (b << 32);
//	}
	
//	/**
//	 * @return an IEEE format single precision floating point number from
//	 * the input stream.
//	 * @throws IOException if there is a problem with the input stream.
//	 */
//	private float readFloat() throws IOException
//	{
//		return Float.intBitsToFloat( readInt() );
//	}
	
//	/**
//	 * @return an IEEE format double precision floating point number from
//	 * the input stream.
//	 * @throws IOException if there is a problem with the input stream.
//	 */
//	private double readDouble() throws IOException
//	{
//		return Double.longBitsToDouble( readLong() );
//	}
	
//	/**
//	 * Read all the bytes from the input stream.
//	 * @param b a buffer for the bytes that are read.
//	 * @throws IOException if there is a problem with the input stream. 
//	 */
//	private void readBytes( byte[] b ) throws IOException
//	{
//		int n = b.length;
//		for (int i = 0; i < n; i++)
//			b[i] = readByte();
//	}

//	private byte readType() throws IOException
//	{
//		return readByte();
//	}
	
	/**
	 * @param v the validator to use to check the value format.
	 * @return the object read.
	 * @throws IOException
	 */
	private Object readValue( Validator v )
		throws IOException
	{
		Object value = readValue0();
		
		if (value == null)
			return null;
		
		return validateValue( value, v );
	}

	private Object readValueOrNone( Validator v )
		throws IOException
	{
		Object value = readValue0();
		
		if (value == null)
			return null;
		
		if (value == NONE)
			return value;
		
		return validateValue( value, v );
	}
	
	private Object validateValue( Object value, Validator v )
	{
		// TODO replace with if (v == null || !v.validate( value ))
		if (v != null && !v.validate( value ))
		{
			// issue a warning?
			System.out.printf( "value %s does not validate using %s\n", value, v );
			return null;
		}
		
		return value;
	}
	
	/**
	 * @return the object read.
	 * @throws IOException
	 */
	private Object readValue0()
		throws IOException
	{
		byte type = buf.getByte();
		
		switch (type)
		{
			case TypeCode.NULL:
				return null;
			
			case TypeCode.BOOLEAN_FALSE:
				return Boolean.FALSE;
			
			case TypeCode.BOOLEAN_TRUE:
				return Boolean.TRUE;
			
			case TypeCode.BYTE1:
				return buf.getByte();
			
			case TypeCode.SHORT1:
				return (short) buf.getByte();
			
			case TypeCode.INT1:
				return (int) buf.getByte();
			
			case TypeCode.LONG1:
				return (long) buf.getByte();
			
			case TypeCode.SHORT2:
				return buf.getShort();
			
			case TypeCode.INT2:
				return (int) buf.getShort();
			
			case TypeCode.LONG2:
				return (long) buf.getShort();
			
			case TypeCode.INT4:
				return buf.getInt();
			
			case TypeCode.LONG4:
				return (long) buf.getInt();
			
			case TypeCode.LONG8:
				return buf.getLong();
			
			case TypeCode.FLOAT4:
				return buf.getFloat();
			
			case TypeCode.FLOAT8:
				return buf.getDouble();
				
			case TypeCode.BYTES:
				return readBytes();
			
			case TypeCode.EMPTY_STRING:
				return "";
			
			case TypeCode.STRING:
				return new String( readBytes(), vf.getStringEncoding() );
			
			case TypeCode.ARRAY:
				return fromArrayValue( readArrayValue() );
			
			case TypeCode.STRUCT:
				return readStructValue();
			
			case TypeCode.CUSTOM:
				return vf.importCustomValue( readStructValue() );
			
			case TypeCode.NONE:
				return NONE;
			
			default:
				if ((type & TypeCode.PSMASK) == TypeCode.PSVALUE)
				{
					// positive small integers
					int value = type & TypeCode.PVMASK;
					int dt = (type >> TypeCode.PDTSHIFT) & TypeCode.DTMASK;
					if (dt == TypeCode.BYTE_DT) return (byte) value;
					if (dt == TypeCode.SHORT_DT) return (short) value;
					if (dt == TypeCode.INT_DT) return value;
					return (long) value;
				}
				if ((type & TypeCode.NSMASK) == TypeCode.NSVALUE)
				{
					// negative small integers
					int value = -(type & TypeCode.NVMASK)-1;
					int dt = (type >> TypeCode.NDTSHIFT) & TypeCode.DTMASK;
					if (dt == TypeCode.BYTE_DT) return (byte) value;
					if (dt == TypeCode.SHORT_DT) return (short) value;
					if (dt == TypeCode.INT_DT) return value;
					return (long) value;
				}
				throw new UnsupportedOperationException( "unsupported type code "+type );
		}
	}
	
	public ArrayValue readArrayValue() throws IOException
	{
		ArrayValue array = startArray();
		readValues( array );
		endArray( array );
		return array;
	}

	private void readValues( ArrayValue array ) throws IOException
	{
		ArrayElement ae = new ArrayElement();
		while (readArrayElement( ae ))
			array.add( ae.value );
	}

	public StructValue readStructValue() throws IOException
	{
		StructValue sv = startStruct();
		readKeysAndValues( sv );
		endStruct( sv );
		return sv;
	}

	/**
	 * Reads the key / value pairs and puts them in the struct value.
	 * @param sv
	 * @throws IOException if there are problems with the tagged data input.
	 */
	private void readKeysAndValues( StructValue sv )
		throws IOException
	{
		StructElement se = new StructElement();
		while (readStructElement( se ))
			sv.put( se.key, se.value );
	}
	
	private byte[] readBytes() throws IOException
	{
		int n = (Integer) readValue( intValidator );
		byte[] b = new byte[n];
		buf.getFully( b );
		return b;
	}
	
	private final Validator intValidator = Validator_int.get( 0 );
}
