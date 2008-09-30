/* $Id$
 *
 * Copyright 2008 Cisco Systems Inc.
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

package etch.util;

import java.io.IOException;

/**
 * Interface for getting various data types from some input.
 */
public interface DataInput
{
	/**
	 * @return the underlying buffer.
	 */
	public FlexBuffer buffer();
	
	/**
	 * @return the current value of index. This is the offset of the next byte
	 * read.
	 */
	public int index();

	/**
	 * Sets the index, the offset of the next byte read.
	 * 
	 * @param index the new value of index. Index must be >= 0.
	 * 
	 * @throws IllegalArgumentException if index < 0 or if index > length.
	 */
	public void setIndex( int index ) throws IllegalArgumentException;

	/**
	 * @return the current value of length. This is the last index of valid data
	 * in the buffer.
	 */
	public int length();

	/**
	 * @param length the new value of length. Length must be >= 0. If length <
	 * index, index is also set to length.
	 * 
	 * @throws IllegalArgumentException if length < 0.
	 * 
	 * @throws IOException if length > actual buffer length.
	 */
	public void setLength( int length ) throws IllegalArgumentException,
		IOException;

	/**
	 * @return length() - index(). Result is always >= 0. This is the amount of
	 * data that could be read using the various forms of get.
	 */
	public int avail();

	/**
	 * Checks that enough data is available to complete an operation.
	 * 
	 * @param length the required amount of data.
	 * 
	 * @throws IOException if length > avail().
	 */
	public void checkAvail( int length ) throws IOException;

	/**
	 * Calls buffer.reset(). This has the effect of clearing the output buffer
	 * and resetting index and length to 0. It might free buffer resources or
	 * otherwise destroy any data remaining in the buffer.
	 */
	public void reset();

	/**
	 * Calls buffer.compact(). Compacts the buffer by moving remaining data
	 * (from index to length) to the front of the buffer. Sets index to 0, and
	 * sets length to the value of length - index (before index was changed).
	 * 
	 * For example, if index = 5 and length = 8, compact() copies the 3 bytes
	 * starting at position 5 to 0, sets index to 0 and length to 3.
	 */
	public void compact();

	/**
	 * Adjusts index as if by a get but without transferring any data. This
	 * could be used to skip over a data item in the buffer. It is an error if
	 * the adjusted index would be past the current length of the buffer.
	 * 
	 * @param length the number of bytes to skip over. Length must be >= 0. It
	 * is an error if length > avail().
	 * 
	 * @throws IOException if length > avail().
	 */
	public void skip( int length ) throws IOException;

	/**
	 * @return the unsigned byte value (0-255) at index, and adjusts index by
	 * adding one.
	 * 
	 * @throws IOException if avail() < 1.
	 */
	public int get() throws IOException;

	/**
	 * Copies data from the byte array to buf as if by repeated calls to get()
	 * while avail() > 0.
	 * 
	 * @param buf a buffer to receive the data. At most min( buf.length, avail()
	 * ) bytes are transferred.
	 * 
	 * @return the amount of data transferred.
	 * 
	 * @throws IOException if avail() < 1.
	 */
	public int get( byte[] buf ) throws IOException;

	/**
	 * Copies data from this to buf as if by repeated calls to get().
	 * 
	 * @param buf a buffer to receive the data. At most min( len, avail() )
	 * bytes are transferred, starting at off.
	 * 
	 * @param off the index in buf to receive the data. Off must be >= 0 && <=
	 * buf.length.
	 * 
	 * @param len the max amount of data to transfer. Len must be >= 0 and <=
	 * buf.length - off.
	 * 
	 * @return the amount of data transferred.
	 * 
	 * @throws IOException if avail() < 1
	 */
	public int get( byte[] buf, int off, int len ) throws IOException;

	/**
	 * Fills a buffer from the next b.length bytes.
	 * 
	 * @param buf the buffer to fill.
	 * @throws IOException if avail() < b.length.
	 */
	public void getFully( byte[] buf ) throws IOException;

	/**
	 * @param length the number of bytes to get.
	 * 
	 * @return a newly allocated byte array with the specified number of bytes
	 * gotten.
	 * 
	 * @throws IOException if length < avail().
	 */
	public byte[] getBytes( int length ) throws IOException;

	/**
	 * @return a newly allocated by array with all the available bytes as if by
	 * getBytes( avail() ).
	 * 
	 * @throws IOException
	 */
	public byte[] getAvailBytes() throws IOException;

	/////////////////////////////////////////
	// HIGH-LEVEL DATA TRANSFER OPERATIONS //
	/////////////////////////////////////////

	/**
	 * @return the next available signed byte.
	 * 
	 * @throws IOException if avail() < 1.
	 */
	public byte getByte() throws IOException;

	// ////////////////////////////////////////////////////////////
	// The following operations are sensitive to byte-ordering. //
	// ////////////////////////////////////////////////////////////

	/**
	 * @return a 16-bit signed int composed from the next 2 bytes.
	 * 
	 * @throws IOException if avail() < 2.
	 */
	public short getShort() throws IOException;

	/**
	 * @return a 32-bit signed int composed from the next 4 bytes.
	 * 
	 * @throws IOException if avail() < 4.
	 */
	public int getInt() throws IOException;

	/**
	 * @return a 64-bit signed long composed from the next 8 bytes.
	 * 
	 * @throws IOException if avail() < 8.
	 */
	public long getLong() throws IOException;

	/**
	 * @return a 32-bit float from the next 4 bytes (as ieee single format).
	 * 
	 * @throws IOException if avail() < 4.
	 */
	public float getFloat() throws IOException;

	/**
	 * @return a 64-bit float from the next 8 bytes (as ieee double format).
	 * 
	 * @throws IOException if avail() < 8.
	 */
	public double getDouble() throws IOException;
}
