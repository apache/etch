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
 * A TaggedDataInputStream reads type tagged values from an input stream.
 */
public interface TaggedDataInput
{
	/**
	 * Reads a message from the stream.
	 * @return a message read from the stream.
	 * @throws IOException 
	 */
	public Message readMessage() throws IOException;

	/**
	 * Reads a struct from the stream.
	 * @return the struct read from the stream.
	 * @throws IOException 
	 */
	public StructValue readStructValue() throws IOException;
	
	/**
	 * Reads an array from the stream.
	 * @return the array read from the stream.
	 * @throws IOException
	 */
	public ArrayValue readArrayValue() throws IOException;
	
//	/**
//	 * Starts reading a message from the stream.
//	 * @return the message that we are reading.
//	 * @throws IOException if there is a problem with the stream.
//	 */
//	public Message startMessage() throws IOException;

//	/**
//	 * Ends a message that we are reading.
//	 * @param msg the message that has been read.
//	 * @throws IOException if there is a problem with the stream.
//	 */
//	public void endMessage( Message msg ) throws IOException;

//	/**
//	 * Starts reading a struct from the stream.
//	 * @return the struct that we are reading.
//	 * @throws IOException if there is a problem with the stream.
//	 */
//	public StructValue startStruct() throws IOException;

//	/**
//	 * Reads the next key and value in the sequence of struct
//	 * elements by stuffing the fields of the passed in StructElement.
//	 * @param se place to store the read key and value.
//	 * @return true if a key and value were read, false if there aren't any more
//	 * key / value pairs.
//	 * @throws IOException if there is a problem with the stream.
//	 */
//	public boolean readStructElement( StructElement se ) throws IOException;

//	/**
//	 * Ends a struct that we are reading.
//	 * @param struct the struct that we read.
//	 * @throws IOException if there is a problem with the stream.
//	 */
//	public void endStruct( StructValue struct ) throws IOException;

//	/**
//	 * Starts reading an array from the stream.
//	 * @return the array that we are reading.
//	 * @throws IOException if there is a problem with the stream.
//	 */
//	public ArrayValue startArray() throws IOException;

//	/**
//	 * Reads the next value in the sequence of array elements by
//	 * stuffing the fields of the passed in ArrayElement.
//	 * @param ae place to store the read value.
//	 * @return true if a value was read, false if there aren't any more.
//	 * @throws IOException if there is a problem with the stream.
//	 */
//	public boolean readArrayElement( ArrayElement ae ) throws IOException;

//	/**
//	 * Ends an array that we are reading.
//	 * @param array the array that we read.
//	 * @throws IOException if there is a problem with the stream.
//	 */
//	public void endArray( ArrayValue array ) throws IOException;
	
	/**
	 * Holder for a key / value pair from a struct element sequence.
	 */
	public static class StructElement
	{
		/**
		 * Description of key.
		 */
		public Field key;
		
		/**
		 * Description of value.
		 */
		public Object value;
	}
	
	/**
	 * Holder for a value from an array element sequence.
	 */
	public static class ArrayElement
	{
		/**
		 * The array element value.
		 */
		public Object value;
	}

	/**
	 * @param buf
	 */
	public void setBuffer( FlexBuffer buf );
}
