/*
 * $Id$
 * 
 * Copyright 2007-2008 Cisco Systems Inc.
 * 
 * Licensed under the Apache License, Version 2.0 (the "License"); you may not
 * use this file except in compliance with the License. You may obtain a copy of
 * the License at
 * 
 * http://www.apache.org/licenses/LICENSE-2.0
 * 
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
 * License for the specific language governing permissions and limitations under
 * the License.
 */

package etch.util;

import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;

/**
 * A FlexBuffer models a byte array and manages the active region of it
 * (0..length). It also supports dynamically extending the buffer as needed.
 * 
 * A FlexBuffer also has an index (read or write cursor). The various get and
 * put operations always occur at the current index, with the index adjusted
 * appropriately afterward. Get will not move index past length. If put needs to
 * move index past length, length is also adjusted. This may cause the byte
 * array to be re-allocated to a larger size.
 */
public interface FlexBuffer
{
	/**
	 * @return a new or previously allocated DataInput implementation which
	 * uses this FlexBuffer as a backing store.
	 */
	public DataInput dataInput();
	
	/**
	 * @return a new or previously allocated DataOutput implementation which
	 * uses this FlexBuffer as a backing store.
	 */
	public DataOutput dataOutput();
	
	/**
	 * @return the current value of index.
	 */
	public int index();

	/**
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
	 * buffer and put is true, the buffer is expanded.
	 * 
	 * @param put true if the buffer should be expanded if not long enough.
	 * 
	 * @throws IllegalArgumentException if length < 0.
	 * 
	 * @throws IOException if length > max.
	 */
	public void setLength( int length, boolean put )
		throws IllegalArgumentException, IOException;

	/**
	 * @return length() - index(). Result is always >= 0. This is the amount of
	 * data that could be read using the various forms of get. It doesn't really
	 * mean anything in relation to put.
	 */
	public int avail();

	/**
	 * Checks that len <= avail().
	 * @param len the amount of data needed.
	 * 
	 * @throws IOException if len > avail().
	 */
	public void checkAvail( int len ) throws IOException;

	/**
	 * Ensures that the buffer has enough space to write len bytes at the
	 * current index().
	 * 
	 * @param len the amount of space needed.
	 * 
	 * @throws IOException if there is not enough space and the buffer cannot
	 * be made larger.
	 */
	public void ensureSpace( int len ) throws IOException;

	/**
	 * Sets both index and length to 0. Trims the buffer length if it is past
	 * the recommended limit.
	 * 
	 * Shorthand for setLength( 0 ).
	 */
	public void reset();

	/**
	 * Compacts the buffer by moving remaining data (from index to length) to
	 * the front of the buffer. Sets index to 0, and sets length to avail
	 * (before index was changed). Trims the buffer length if it is past
	 * the recommended limit.
	 */
	public void compact();

	////////////////////
	// GET OPERATIONS //
	////////////////////

	/**
	 * @return the unsigned byte value at index, and adjusts index by adding
	 * one.
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
	 * Copies data from the byte array to buf as if by repeated calls to get().
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
	 * @param b the buffer to fill.
	 * 
	 * @throws IOException if b.length > avail().
	 */
	public void getFully( byte[] b ) throws IOException;

	/**
	 * @param len the number of bytes to get.
	 * 
	 * @return a newly allocated byte array filled with len bytes.
	 * 
	 * @throws IOException if len > avail().
	 */
	public byte[] getBytes( int len ) throws IOException;

	/**
	 * @return the currently available bytes. This is avail() bytes starting
	 * from index(). Same as getBytes( avail() ).
	 * 
	 * @throws IOException
	 */
	public byte[] getAvailBytes() throws IOException;

	/**
	 * Adjusts index as if by a get or put but without transferring any data.
	 * This could be used to skip over a data item in an input or output buffer.
	 * 
	 * @param len the number of bytes to skip over. Len must be >= 0. If put is
	 * false, it is an error if len > avail(). If put is true, the buffer may be
	 * extended (and the buffer length adjusted).
	 * 
	 * @param put if true it is ok to extend the length of the buffer.
	 * 
	 * @throws EOFException if put is false and len > avail().
	 * 
	 * @throws IOException if the max buffer size is exceeded.
	 */
	public void skip( int len, boolean put ) throws IOException;

	////////////////////
	// PUT OPERATIONS //
	////////////////////

	/**
	 * Puts a byte into the buffer at the current index, then adjusts the index
	 * by one. Adjusts the length as necessary. The buffer is expanded if
	 * needed.
	 * 
	 * @param b
	 * 
	 * @throws IOException if the buffer overflows its max length.
	 */
	public void put( int b ) throws IOException;

	/**
	 * Puts some bytes into the buffer as if by repeated calls to put().
	 * 
	 * @param buf the source of the bytes to put. The entire array of bytes is
	 * put.
	 * 
	 * @throws IOException if the buffer overflows its max length.
	 */
	public void put( byte[] buf ) throws IOException;

	/**
	 * Puts some bytes into the buffer as if by repeated calls to put().
	 * 
	 * @param buf the source of the bytes to put.
	 * 
	 * @param off the index to the first byte to put.
	 * 
	 * @param len the number of bytes to put.
	 * 
	 * @throws IOException if the buffer overflows its max length.
	 */
	public void put( byte[] buf, int off, int len ) throws IOException;

	/**
	 * Copies the available bytes from buf into buffer as if by repeated
	 * execution of put( buf.get() ) while buf.avail() > 0.
	 * 
	 * @param buf the source of the bytes to put. All available bytes are
	 * copied.
	 * 
	 * @throws IOException if the buffer overflows its max length.
	 */
	public void put( DataInput buf ) throws IOException;

	/**
	 * Copies the specified number of bytes from buf into buffer as if by
	 * repeated execution of put( buf.get() ).
	 * 
	 * @param buf the source of the bytes to put.
	 * 
	 * @param len the number of bytes to put. Len must be <= buf.avail().
	 * 
	 * @throws IOException if the buffer overflows its max length.
	 */
	public void put( DataInput buf, int len ) throws IOException;

	///////////////////////
	// STREAM OPERATIONS //
	///////////////////////

	/**
	 * Resets the buffer and reads data into it. Sets length to the amount of
	 * data read. The amount of data read depends upon buffer implementation,
	 * geometry, etc.
	 * 
	 * @param is the InputStream to use.
	 * 
	 * @return the count of bytes read.
	 * 
	 * @throws IOException
	 */
	public int read( InputStream is ) throws IOException;

	/**
	 * Writes data from index() for avail() bytes.
	 * 
	 * @param os the OutputStream to use.
	 * 
	 * @throws IOException
	 */
	public void write( OutputStream os ) throws IOException;

	/////////////////////////
	// DATAGRAM OPERATIONS //
	/////////////////////////

	/**
	 * Resets the buffer and reads a datagram into it. Sets length to the amount
	 * of data read.
	 * 
	 * @param s the DatagramSocket to use.
	 * 
	 * @param p the DatagramPacket to use.
	 */
	public void receive( DatagramSocket s, DatagramPacket p );

	/**
	 * Writes the datagram from index() for avail() bytes.
	 * 
	 * @param s the DatagramSocket to use.
	 * 
	 * @param p the DatagramPacket to use.
	 */
	public void send( DatagramSocket s, DatagramPacket p );
}