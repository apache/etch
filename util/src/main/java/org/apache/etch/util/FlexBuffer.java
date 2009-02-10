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

package org.apache.etch.util;

import java.io.EOFException;
import java.io.IOException;

/**
 * A FlexBuffer wraps a byte array and manages the active region of
 * it (0..length). It also supports dynamically extending the buffer
 * as needed.
 * 
 * A FlexBuffer also has an index (read or write cursor). The various
 * get and put operations always occur at the current index, with the
 * index adjusted appropriately afterward. Get will not move index past
 * length. If put needs to move index past length, length is also
 * adjusted. This may cause the byte array to be re-allocated to a
 * larger size.
 */
public final class FlexBuffer
{
	/**
	 * Constructs a FlexBuffer with internal buffer and index and length of 0.
	 */
	public FlexBuffer()
	{
		this( new byte[INIT_BUFFER_LEN], 0, 0 );
	}

	/**
	 * Constructs the FlexBuffer out of an existing buffer, ready to
	 * read, with the index set to 0 and length set to buffer.length.
	 * The buffer is adopted, not copied. If you want to copy, use
	 * one of the put methods instead.
	 * 
	 * Note: this is the same as FlexBuffer( buffer, 0, buffer.length ).
	 *
	 * @param buffer the buffer to adopt.
	 * 
	 * @see #put(byte[])
	 */
	public FlexBuffer( byte[] buffer )
	{
		this( buffer, 0, buffer.length );
	}
	
	/**
	 * Constructs the FlexBuffer out of an existing buffer, ready to
	 * read, with the index set to 0 and the specified length. The
	 * buffer is adopted, not copied. If you want to copy, use one
	 * of the put methods instead.
	 * 
	 * Note: this is the same as FlexBuffer( buffer, 0, length ).
	 *
	 * @param buffer the buffer to adopt.
	 * 
	 * @param length the length of the data in the buffer (index is
	 * presumed to be 0).
	 * 
	 * @see #put(byte[], int, int)
	 */
	public FlexBuffer( byte[] buffer, int length )
	{
		this( buffer, 0, length );
	}
	
	/**
	 * Constructs the FlexBuffer out of an existing buffer, ready to
	 * read, with the specified index and length. The buffer is adopted,
	 * not copied. If you want to copy, use one of the put methods
	 * instead.
	 * 
	 * Note: if you want to start off writing to the end of the buffer
	 * and not reading it, specify index as the place to start writing,
	 * and length as the amount of data that is valid after index (which
	 * might reasonably be 0).
	 *
	 * @param buffer the buffer to adopt.
	 * 
	 * @param index the place to start reading or writing.
	 * 
	 * @param length the length of the valid data in the buffer,
	 * starting at index.
	 * 
	 * @see #put(byte[], int, int)
	 */
	public FlexBuffer( byte[] buffer, int index, int length )
	{
		if (buffer == null)
			throw new NullPointerException( "buffer == null" );
		
		if (index < 0)
			throw new IllegalArgumentException( "index < 0" );
		
		if (length < 0)
			throw new IllegalArgumentException( "length < 0" );
		
		if (index+length > buffer.length)
			throw new IllegalArgumentException( "index+length > buffer.length" );
		
		this.buffer = buffer;
		this.index = index;
		this.length = index+length;
	}

	/**
	 * @return the current byte array. Might change if any operation
	 * needs to extend length past the end of the array.
	 */
	final public byte[] getBuf()
	{
		return buffer;
	}
	
	final private void ensureLength( int len ) throws IOException
	{
		int n = buffer.length;
		if (len <= n)
			return;
		
		// the buffer is not big enough, we have to expand it.
		
		int k = n;
		if (k < INIT_BUFFER_LEN)
			k = INIT_BUFFER_LEN;
		
		while (len > k && k < MAX_BUFFER_LEN)
			k = k << 1;
		
		if (len > k)
			throw new IOException( "buffer overflow" );
		
		byte[] b = new byte[k];
		System.arraycopy( buffer, 0, b, 0, n );
		buffer = b;
	}
	
	private byte[] buffer;
	
	private final static int INIT_BUFFER_LEN = 32;
	
	private final static int TRIM_BUFFER_LEN = 16*1024;
	
	private final static int MAX_BUFFER_LEN = 4*1024*1024;

	/**
	 * @return the current value of length. This is the last
	 * index of valid data in the buffer.
	 */
	final public int length()
	{
		return length;
	}

	/**
	 * @param length the new value of length. Length must be >= 0.
	 * If length < index, index is also set to length. If length
	 * is larger than the current buffer, the buffer is expanded.
	 * @return this flex buffer object.
	 * @throws IllegalArgumentException if length < 0.
	 * @throws IOException if the buffer overflows its max length.
	 */
	final public FlexBuffer setLength( int length ) throws IllegalArgumentException, IOException
	{
		if (length < 0)
			throw new IllegalArgumentException( "length < 0" );
		
		ensureLength( length );
		
		this.length = length;
		
		if (index > length)
			index = length;
		
		return this;
	}

	private int length;
	
	/**
	 * @return the current value of index.
	 */
	final public int index()
	{
		return index;
	}
	
	/**
	 * @param index the new value of index. Index must be >= 0.
	 * @return this flex buffer object.
	 * @throws IllegalArgumentException if index < 0 or if index > length.
	 */
	final public FlexBuffer setIndex( int index ) throws IllegalArgumentException
	{
		if (index < 0 || index > length)
			throw new IllegalArgumentException( "index < 0 || index > length" );
		
		this.index = index;
		
		return this;
	}
	
	private int index;

	/**
	 * @return length() - index(). Result is always >= 0. This is the amount
	 * of data that could be read using the various forms of get. It doesn't
	 * really mean anything in relation to put.
	 */
	final public int avail()
	{
		return length - index;
	}

	/**
	 * Sets both index and length to 0. Trims the buffer length if it is past
	 * the recommended limit.
	 * 
	 * Shorthand for setLength( 0 ).
	 * 
	 * @return this flex buffer object.
	 */
	final public FlexBuffer reset()
	{
		index = 0;
		length = 0;
		
		if (buffer.length > TRIM_BUFFER_LEN)
			buffer = new byte[TRIM_BUFFER_LEN];
		
		return this;
	}

	/**
	 * Compacts the buffer by moving remaining data (from index to length)
	 * to the front of the buffer. Sets index to 0, and sets length to
	 * avail (before index was changed).
	 * 
	 * @return this flex buffer object.
	 */
	final public FlexBuffer compact()
	{
		if (index == 0)
			return this;
		
		int n = avail();
		if (n == 0)
		{
			reset();
			return this;
		}
		
		System.arraycopy( buffer, index, buffer, 0, n );
		
		index = 0;
		length = n;
		
		return this;
	}
	
	/**
	 * @return the unsigned byte value at index, and adjusts index
	 * by adding one.
	 * @throws IOException if avail() < 1.
	 */
	final public int get() throws IOException
	{
		checkAvail( 1 );
		
		return buffer[index++] & 255;
	}
	
	/**
	 * Copies data from the byte array to buf as if by repeated
	 * calls to get().
	 * @param buf a buffer to receive the data. At most
	 * min( buf.length, avail() ) bytes are transferred.
	 * @return the amount of data transferred.
	 * @throws IOException if avail() < 1.
	 */
	final public int get( byte[] buf ) throws IOException
	{
		return get( buf, 0, buf.length );
	}
	
	/**
	 * Copies data from the byte array to buf as if by repeated
	 * calls to get().
	 * @param buf a buffer to receive the data. At most
	 * min( len, avail() ) bytes are transferred, starting
	 * at off.
	 * @param off the index in buf to receive the data.
	 * Off must be >= 0 && <= buf.length.
	 * @param len the max amount of data to transfer. Len
	 * must be >= 0 and <= buf.length - off.
	 * @return the amount of data transferred.
	 * @throws IOException if avail() < 1
	 */
	final public int get( byte[] buf, int off, int len ) throws IOException
	{
		checkBuf( buf, off, len );
		
		if (len == 0)
			return 0;
		
		checkAvail( 1 );
		
		int n = Math.min( len, avail() );
		
		System.arraycopy( buffer, index, buf, off, n );
		index += n;
		
		return n;
	}

	/**
	 * @return the next available byte.
	 * @throws IOException if avail() < 1.
	 */
	final public byte getByte() throws IOException
    {
		checkAvail( 1 );
		
		return buffer[index++];
	}
	
	private final static boolean littleEndian = false;

	/**
	 * @return a short composed from the next 2 bytes. Little-endian.
	 * @throws IOException if avail() < 2.
	 */
	final public short getShort() throws IOException
    {
		checkAvail( 2 );

		if (littleEndian)
		{
			// little-endian
			int value = buffer[index++] & 255;
			return (short) (value + ((buffer[index++] & 255) << 8));
		}
		
		// big-endian
		int value = buffer[index++];
		return (short) ((value << 8) + (buffer[index++] & 255));
    }
	
	/**
	 * @return an int composed from the next 4 bytes. Little-endian.
	 * @throws IOException if avail() < 4.
	 */
	final public int getInt() throws IOException
	{
		checkAvail( 4 );

		if (littleEndian)
		{
			// little-endian
			int value = buffer[index++] & 255;
			value += (buffer[index++] & 255) << 8;
			value += (buffer[index++] & 255) << 16;
			return value + ((buffer[index++] & 255) << 24);
		}
		
		// big-endian
		int value = buffer[index++];
		value = (value << 8) + (buffer[index++] & 255);
		value = (value << 8) + (buffer[index++] & 255);
		return (value << 8) + (buffer[index++] & 255);
	}

	/**
	 * @return a long composed from the next 8 bytes. Little-endian.
	 * @throws IOException if avail() < 8.
	 */
	final public long getLong() throws IOException
    {
		checkAvail( 8 );

		if (littleEndian)
		{
			// little-endian
			long value = buffer[index++] & 255;
			value += (long)(buffer[index++] & 255) << 8;
			value += (long)(buffer[index++] & 255) << 16;
			value += (long)(buffer[index++] & 255) << 24;
			value += (long)(buffer[index++] & 255) << 32;
			value += (long)(buffer[index++] & 255) << 40;
			value += (long)(buffer[index++] & 255) << 48;
			return value + ((long)(buffer[index++] & 255) << 56);
		}
		
		// big-endian
		long value = buffer[index++];
		value = (value << 8) + (buffer[index++] & 255);
		value = (value << 8) + (buffer[index++] & 255);
		value = (value << 8) + (buffer[index++] & 255);
		value = (value << 8) + (buffer[index++] & 255);
		value = (value << 8) + (buffer[index++] & 255);
		value = (value << 8) + (buffer[index++] & 255);
		return (value << 8) + (buffer[index++] & 255);
    }

	/**
	 * @return a float from the next available bytes.
	 * @throws IOException if avail() < 4.
	 */
	final public float getFloat() throws IOException
    {
		return Float.intBitsToFloat( getInt() );
    }

	/**
	 * @return a double from the next available bytes.
	 * @throws IOException if avail() < 8.
	 */
	final public double getDouble() throws IOException
    {
		return Double.longBitsToDouble( getLong() );
    }

	/**
	 * Fills a buffer fully from the next available bytes.
	 * @param b
	 * @throws IOException if avail() < b.length.
	 */
	final public void getFully( byte[] b ) throws IOException
    {
	    checkAvail( b.length );
	    int n = get( b, 0, b.length );
	    assert n == b.length;
    }
	
	/**
	 * Puts a byte into the buffer at the current index,
	 * then adjusts the index by one. Adjusts the length
	 * as necessary. The buffer is expanded if needed.
	 * @param b
	 * @return this flex buffer object.
	 * @throws IOException if the buffer overflows its max length.
	 */
	final public FlexBuffer put( int b ) throws IOException
	{
		ensureLength( index+1 );
		buffer[index++] = (byte) b;
		fixLength();
		
		return this;
	}
	
	/**
	 * Puts some bytes into the buffer as if by repeated
	 * calls to put().
	 * @param buf the source of the bytes to put. The entire
	 * array of bytes is put.
	 * @return this flex buffer object.
	 * @throws IOException if the buffer overflows its max length.
	 */
	final public FlexBuffer put( byte[] buf ) throws IOException
	{
		return put( buf, 0, buf.length );
	}
	
	/**
	 * Puts some bytes into the buffer as if by repeated
	 * calls to put().
	 * @param buf the source of the bytes to put.
	 * @param off the index to the first byte to put.
	 * @param len the number of bytes to put.
	 * @return this flex buffer object.
	 * @throws IOException if the buffer overflows its max length.
	 */
	final public FlexBuffer put( byte[] buf, int off, int len ) throws IOException
	{
		checkBuf( buf, off, len );
		
		if (len == 0)
			return this;
		
		ensureLength( index+len );
		System.arraycopy( buf, off, buffer, index, len );
		index += len;
		fixLength();
		
		return this;
	}

	/**
	 * Copies the available bytes from buf into buffer as if by
	 * repeated execution of put( buf.get() ).
	 * @param buf the source of the bytes to put. All available
	 * bytes are copied.
	 * @return this flex buffer object.
	 * @throws IOException if the buffer overflows its max length.
	 */
	final public FlexBuffer put( FlexBuffer buf ) throws IOException
	{
		int n = buf.avail();
		put( buf.buffer, buf.index, n );
		buf.skip( n, false );
		return this;
	}

	/**
	 * Copies the specified number of bytes from buf into buffer
	 * as if by repeated execution of put( buf.get() ).
	 * @param buf the source of the bytes to put.
	 * @param len the number of bytes to put. Len must be <=
	 * buf.avail().
	 * @return this flex buffer object.
	 * @throws IOException if the buffer overflows its max length.
	 */
	final public FlexBuffer put( FlexBuffer buf, int len ) throws IOException
	{
		if (len > buf.avail())
			throw new IllegalArgumentException( "len > buf.avail()" );
		
		put( buf.buffer, buf.index, len );
		buf.skip( len, false );
		return this;
	}

	/**
	 * Puts the byte.
	 * @param value
	 * @throws IOException
	 */
	final public void putByte( byte value ) throws IOException
    {
		ensureLength( index+1 );
		
		buffer[index++] = value;
		
		fixLength();
    }

	/**
	 * Puts the short as the next 2 bytes. Little-endian.
	 * @param value
	 * @throws IOException
	 */
	final public void putShort( short value ) throws IOException
    {
		ensureLength( index+2 );
		
		if (littleEndian)
		{
			buffer[index++] = (byte) value;
			buffer[index++] = (byte) (value >> 8);
		}
		else
		{
			buffer[index++] = (byte) (value >>> 8);
			buffer[index++] = (byte) value;
		}
		
		fixLength();
    }

	/**
	 * Puts the int as the next 4 bytes. Little-endian.
	 * @param value
	 * @throws IOException
	 */
	final public void putInt( int value ) throws IOException
    {
		ensureLength( index+4 );
		
		if (littleEndian)
		{
			buffer[index++] = (byte) value;
			buffer[index++] = (byte) (value >> 8);
			buffer[index++] = (byte) (value >> 16);
			buffer[index++] = (byte) (value >> 24);
		}
		else
		{
			buffer[index++] = (byte) (value >>> 24);
			buffer[index++] = (byte) (value >>> 16);
			buffer[index++] = (byte) (value >>> 8);
			buffer[index++] = (byte) value;
		}
		
		fixLength();
    }

	/**
	 * Puts the long as the next 8 bytes. Little-endian.
	 * @param value
	 * @throws IOException
	 */
	final public void putLong( long value ) throws IOException
    {
		ensureLength( index+8 );
		
		if (littleEndian)
		{
			buffer[index++] = (byte) value;
			buffer[index++] = (byte) (value >> 8);
			buffer[index++] = (byte) (value >> 16);
			buffer[index++] = (byte) (value >> 24);
			buffer[index++] = (byte) (value >> 32);
			buffer[index++] = (byte) (value >> 40);
			buffer[index++] = (byte) (value >> 48);
			buffer[index++] = (byte) (value >> 56);
		}
		else
		{
			buffer[index++] = (byte) (value >>> 56);
			buffer[index++] = (byte) (value >>> 48);
			buffer[index++] = (byte) (value >>> 40);
			buffer[index++] = (byte) (value >>> 32);
			buffer[index++] = (byte) (value >>> 24);
			buffer[index++] = (byte) (value >>> 16);
			buffer[index++] = (byte) (value >>> 8);
			buffer[index++] = (byte) value;
		}
		
		fixLength();
    }

	/**
	 * Puts the float as the next 4 bytes. Little-endian.
	 * @param value
	 * @throws IOException
	 */
	final public void putFloat( float value ) throws IOException
    {
		putInt( Float.floatToIntBits( value ) );
    }

	/**
	 * Puts the double as the next 8 bytes. Little-endian.
	 * @param value
	 * @throws IOException
	 */
	final public void putDouble( double value ) throws IOException
    {
		putLong( Double.doubleToLongBits( value ) );
    }

	/**
	 * Adjusts index as if by a get or put but without transferring
	 * any data. This could be used to skip over a data item in an
	 * input or output buffer.
	 * 
	 * @param len the number of bytes to skip over. Len must be
	 * >= 0. If put is false, it is an error if len > avail().
	 * If put is true, the buffer may be extended (and the buffer
	 * length adjusted).
	 * 
	 * @param put if true it is ok to extend the length of the
	 * buffer.
	 * 
	 * @return this flex buffer object.
	 * 
	 * @throws EOFException if put is false and len > avail().
	 * 
	 * @throws IOException if the max buffer size is exceeded.
	 */
	final public FlexBuffer skip( int len, boolean put ) throws IOException
	{
		if (len < 0)
			throw new IllegalArgumentException( "len < 0" );
		
		if (len == 0)
			return this;
		
		if (put)
		{
			ensureLength( index+len );
			index += len;
			fixLength();
			return this;
		}
		
		checkAvail( len );
		
		index += len;
		
		return this;
	}

	/**
	 * If index has moved past length during a put, then adjust length
	 * to track index.
	 */
	final private void fixLength()
	{
		if (index > length)
			length = index;
	}

	/**
	 * Checks buf, off, and len for being reasonable.
	 * @param buf
	 * @param off
	 * @param len
	 */
	final private void checkBuf( byte[] buf, int off, int len )
	{
		if (buf == null)
			throw new NullPointerException( "buf == null" );
		
		if (off < 0 || off > buf.length)
			throw new IllegalArgumentException( "off < 0 || off > buf.length" );
		
		if (len < 0)
			throw new IllegalArgumentException( "len < 0" );
		
		if (off+len > buf.length)
			throw new IllegalArgumentException( "off+len > buf.length" );
	}

	/**
	 * @return the currently available bytes.
	 * @throws IOException
	 */
	final public byte[] getAvailBytes() throws IOException
	{
		byte[] buf = new byte[avail()];
		get( buf );
		return buf;
	}
	
	/**
	 * Checks that there are enough bytes to for a read.
	 * @param len the length required by a read operation.
	 * @throws EOFException if len > avail().
	 */
	final private void checkAvail( int len ) throws IOException
	{
		if (len > avail())
			throw new EOFException( "len > avail()" );
	}
}