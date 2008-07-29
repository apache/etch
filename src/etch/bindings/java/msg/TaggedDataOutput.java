/* $Id$
 *
 * Created by sccomer on Jan 17, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.msg;

import java.io.IOException;

import metreos.util.FlexBuffer;

/**
 * A TaggedDataOutputStream writes type tagged data values to an output stream.
 */
public interface TaggedDataOutput
{
	/**
	 * Writes the beginning of message data. A message is also a struct,
	 * and so the type of the struct is written. The fields and
	 * values of the struct should be written using
	 * {@link #writeStructElement(Field, Object)}.
	 * @param msg the message being written out.
	 * @throws IOException if there is a problem with the stream.
	 * @see #endMessage(Message)
	 */
	public void startMessage( Message msg ) throws IOException;

	/**
	 * Writes the end of message data.
	 * @param msg the message that was written.
	 * @throws IOException if there is a problem with the stream.
	 */
	public void endMessage( Message msg ) throws IOException;
	
	/**
	 * Writes the beginning of struct data. The type of the struct is
	 * written. The fields and values of the struct are written
	 * using {@link #writeStructElement(Field, Object)}.
	 * @param struct the struct being written out.
	 * @throws IOException if there is a problem with the stream.
	 * @see #endStruct(StructValue)
	 */
	public void startStruct( StructValue struct ) throws IOException;

	/**
	 * Writes a struct element with the specified key and value.
	 * @param key
	 * @param value
	 * @throws IOException if there is a problem with the stream.
	 */
	public void writeStructElement( Field key, Object value )
		throws IOException;

	/**
	 * Writes the end of struct data.
	 * @param struct the struct that was written.
	 * @throws IOException if there is a problem with the stream.
	 */
	public void endStruct( StructValue struct ) throws IOException;

	/**
	 * Writes the beginning of array data. The values of the array
	 * should be written using {@link #writeArrayElement(Object)}.
	 * @param array the array to be written.
	 * @throws IOException if there is a problem with the stream.
	 * @see #endArray(ArrayValue)
	 */
	public void startArray( ArrayValue array ) throws IOException;

	/**
	 * Writes an array element with the specified value.
	 * @param value
	 * @throws IOException if there is a problem with the stream.
	 */
	public void writeArrayElement( Object value ) throws IOException;

	/**
	 * Writes the end of array data.
	 * @param array the array that was written.
	 * @throws IOException if there is a problem with the stream.
	 */
	public void endArray( ArrayValue array ) throws IOException;

	public void setBuffer( FlexBuffer msgBuf );
}
