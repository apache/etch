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
 * Interface for putting various data types to some output.
 */
public interface DataOutput
{
	/**
	 * @return the underlying buffer.
	 */
	public FlexBuffer buffer();
	
	/**
	 * @return the current value of index. This is the offset of the next byte
	 * written.
	 */
	public int index();

	/**
	 * Sets the index, the offset of the next byte written.
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
	 * index, index is also set to length. If length is larger than the current
	 * buffer, the buffer is expanded.
	 * 
	 * @throws IllegalArgumentException if length < 0.
	 * 
	 * @throws IOException if length > max allowed buffer length.
	 */
	public void setLength( int length ) throws IllegalArgumentException,
		IOException;
	
	/**
	 * @return length() - index(). Result is always >= 0.
	 */
	public int avail();

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
	 * Adjusts index as if by a put but without transferring any data. This
	 * could be used to skip over a data item in the buffer. The buffer is
	 * extended if the adjusted index would be past the current length of the
	 * buffer.
	 * 
	 * @param length the number of bytes to skip over. Length must be >= 0.
	 * 
	 * @throws IOException if the buffer cannot be expanded.
	 */
	public void skip( int length ) throws IOException;

	/**
	 * Puts a byte into the buffer at the current index, then adjusts index
	 * by one. Adjusts the length as necessary so that length >= index. The
	 * buffer is expanded if needed.
	 * 
	 * @param b a signed or unsigned byte value (-128 to 255).
	 * 
	 * @throws IOException if the buffer overflows its max length.
	 */
	public void put( int b ) throws IOException;

	/**
	 * Copies all bytes from buf into the buffer at the current index, then
	 * adjusts index by the number of bytes copied. Adjusts the length as
	 * necessary so that length >= index. The buffer is expanded if needed.
	 * 
	 * @param buf the source of the bytes to copy.
	 * 
	 * @throws IOException if the buffer overflows its max length (in which case
	 * no bytes are copied).
	 */
	public void put( byte[] buf ) throws IOException;

	/**
	 * Copies some bytes from buf into the buffer at the current index, then
	 * adjusts index by the number of bytes copied. Adjusts the length as
	 * necessary so that length >= index. The buffer is expanded if needed.
	 * 
	 * @param buf the source of the bytes to copy.
	 * 
	 * @param off the index in buf of the first byte to copy. Must be >= 0 and
	 * <= buf.length.
	 * 
	 * @param len the number of bytes to copy. Must be >= 0 and <= buf.length -
	 * off.
	 * 
	 * @throws IOException if the buffer overflows its max length (in which case
	 * no bytes are copied).
	 */
	public void put( byte[] buf, int off, int len ) throws IOException;

	/**
	 * Copies available bytes from buf into the buffer at the current index,
	 * then adjusts index by the number of bytes copied. Adjusts the length as
	 * necessary so that length >= index. The buffer is expanded if needed.
	 * 
	 * @param buf the source of the bytes to copy.
	 * 
	 * @throws IOException if the buffer overflows its max length (in which case
	 * no bytes are copied).
	 */
	public void put( DataInput buf ) throws IOException;

	/**
	 * Copies some bytes from buf into the buffer at the current index, then
	 * adjusts index by the number of bytes copied. Adjusts the length as
	 * necessary so that length >= index. The buffer is expanded if needed.
	 * 
	 * @param buf the source of the bytes to copy.
	 * 
	 * @param len the number of bytes to copy. Must be >= 0 && <= buf.avail().
	 * 
	 * @throws IOException if the buffer overflows its max length (in which case
	 * no bytes are copied).
	 */
	public void put( DataInput buf, int len ) throws IOException;

	/////////////////////////////////////////
	// HIGH-LEVEL DATA TRANSFER OPERATIONS //
	/////////////////////////////////////////

	/**
	 * Puts the byte into the buffer.
	 * 
	 * @param value
	 * 
	 * @throws IOException
	 */
	public void putByte( byte value ) throws IOException;

	// ////////////////////////////////////////////////////////////
	// The following operations are sensitive to byte-ordering. //
	// ////////////////////////////////////////////////////////////

	/**
	 * Puts the 16-bit signed int into the buffer as the next 2 bytes.
	 * 
	 * @param value
	 * 
	 * @throws IOException
	 */
	public void putShort( short value ) throws IOException;

	/**
	 * Puts the 32-bit signed int into the buffer as the next 4 bytes.
	 * 
	 * @param value
	 * 
	 * @throws IOException
	 */
	public void putInt( int value ) throws IOException;

	/**
	 * Puts the 64-bit signed int into the buffer as the next 8 bytes.
	 * 
	 * @param value
	 * 
	 * @throws IOException
	 */
	public void putLong( long value ) throws IOException;

	/**
	 * Puts the 32-bit float into the buffer as the next 4 bytes (as ieee single
	 * format).
	 * 
	 * @param value
	 * 
	 * @throws IOException
	 */
	public void putFloat( float value ) throws IOException;

	/**
	 * Puts the 64-bit float into the buffer as the next 8 bytes (as ieee double
	 * format).
	 * 
	 * @param value
	 * 
	 * @throws IOException
	 */
	public void putDouble( double value ) throws IOException;
}
