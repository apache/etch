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

import java.io.EOFException;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.net.DatagramPacket;
import java.net.DatagramSocket;

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
final public class BigEndianFlexBuffer implements FlexBuffer
{
	/**
	 * Constructs a FlexBuffer with internal buffer and index and length of 0.
	 */
	public BigEndianFlexBuffer()
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
	public BigEndianFlexBuffer( byte[] buffer )
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
	public BigEndianFlexBuffer( byte[] buffer, int length )
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
	public BigEndianFlexBuffer( byte[] buffer, int index, int length )
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
	
	public void ensureSpace( int len ) throws IOException
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

	public int length()
	{
		return length;
	}

	public void setLength( int length, boolean put ) throws IllegalArgumentException, IOException
	{
		if (length < 0)
			throw new IllegalArgumentException( "length < 0" );
		
		ensureSpace( length );
		
		this.length = length;
		
		if (index > length)
			index = length;
	}

	private int length;
	
	public int index()
	{
		return index;
	}
	
	public void setIndex( int index ) throws IllegalArgumentException
	{
		if (index < 0 || index > length)
			throw new IllegalArgumentException( "index < 0 || index > length" );
		
		this.index = index;
	}
	
	private int index;

	public int avail()
	{
		return length - index;
	}

	public void reset()
	{
		index = 0;
		length = 0;
		
		if (buffer.length > TRIM_BUFFER_LEN)
			buffer = new byte[TRIM_BUFFER_LEN];
	}

	public void compact()
	{
		if (index == 0)
			return;
		
		int n = avail();
		if (n == 0)
		{
			reset();
			return;
		}
		
		System.arraycopy( buffer, index, buffer, 0, n );
		
		index = 0;
		length = n;
	}
	
	public int get() throws IOException
	{
		checkAvail( 1 );
		
		return buffer[index++] & 255;
	}
	
	public int get( byte[] buf ) throws IOException
	{
		return get( buf, 0, buf.length );
	}
	
	public int get( byte[] buf, int off, int len ) throws IOException
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

	public void getFully( byte[] b ) throws IOException
    {
	    checkAvail( b.length );
	    int n = get( b, 0, b.length );
	    assert n == b.length;
    }
	
	public void put( int b ) throws IOException
	{
		ensureSpace( index+1 );
		buffer[index++] = (byte) b;
		fixLength();
	}
	
	public void put( byte[] buf ) throws IOException
	{
		put( buf, 0, buf.length );
	}
	
	public void put( byte[] buf, int off, int len ) throws IOException
	{
		checkBuf( buf, off, len );
		
		if (len == 0)
			return;
		
		ensureSpace( index+len );
		System.arraycopy( buf, off, buffer, index, len );
		index += len;
		fixLength();
	}

	public void put( DataInput buf ) throws IOException
	{
		put0( buf, buf.avail() );
	}
	
	public void put( DataInput buf, int n ) throws IOException
	{
		if (n < 0)
			throw new IllegalArgumentException( "n < 0" );
		
		if (n > buf.avail())
			throw new IOException( "n > buf.avail()" );
		
		put0( buf, n );
	}
	
	private void put0( DataInput buf, int n ) throws IOException
	{
		ensureSpace( index+n );
		buf.get( buffer, index, n );
		index += n;
		fixLength();
	}

	public void skip( int len, boolean put ) throws IOException
	{
		if (len < 0)
			throw new IllegalArgumentException( "len < 0" );
		
		if (len == 0)
			return;
		
		if (put)
		{
			ensureSpace( index+len );
			index += len;
			fixLength();
			return;
		}
		
		checkAvail( len );
		
		index += len;
	}

	private void fixLength()
	{
		if (index > length)
			length = index;
	}

	private void checkBuf( byte[] buf, int off, int len )
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

	public byte[] getAvailBytes() throws IOException
	{
		byte[] buf = new byte[avail()];
		get( buf );
		return buf;
	}

	public byte[] getBytes( int len ) throws IOException
	{
		if (len < 0)
			throw new IllegalArgumentException( "len < 0" );
		
		if (len > avail())
			throw new IOException( "len > avail()" );
		
		byte[] buf = new byte[len];
		get( buf );
		return buf;
	}
	
	public void checkAvail( int len ) throws IOException
	{
		if (len > avail())
			throw new EOFException( "len > avail()" );
	}

	public int read( InputStream is ) throws IOException
	{
		reset();
		
		int n = is.read( buffer );
		if (n < 0)
			return -1;
		
		length = n;
		return n;
	}

	public void write( OutputStream os ) throws IOException
	{
		int n = avail();
		os.write( buffer, index, n );
		skip( n, false );
	}

	public void receive( DatagramSocket s, DatagramPacket p )
	{
		// TODO Auto-generated method stub
	}

	public void send( DatagramSocket s, DatagramPacket p )
	{
		// TODO Auto-generated method stub
	}

	public DataInput dataInput()
	{
		if (befdi == null)
			befdi = new BigEndianFlexDataInput( this );
		return befdi;
	}
	
	private DataInput befdi;

	public DataOutput dataOutput()
	{
		if (befdo == null)
			befdo = new BigEndianFlexDataOutput( this );
		return befdo;
	}
	
	private DataOutput befdo;
}