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
import java.nio.BufferOverflowException;


/**
 * Description of PlainByteBuffer.
 */
public class PlainByteBuffer extends ByteBuffer
{
	/**
	 * Constructs the PlainByteBuffer.
	 *
	 * @param size
	 */
	public PlainByteBuffer( int size )
	{
		this.size = size;
		buffer = new byte[size];
	}
	
	private final int size;
	
	private final byte[] buffer;
	
	private int nextGet;
	
	private int nextPut;
	
	@Override
	public int size()
	{
		return size;
	}
	
	/**
	 * @return the byte array backing this buffer.
	 */
	public byte[] getBuf()
	{
		return buffer;
	}
	
	@Override
	public int length()
	{
		return nextPut-nextGet;
	}
	
	@Override
	public void clear()
	{
		nextGet = 0;
		nextPut = 0;
	}

	@Override
	public byte get() throws EOFException
	{
		if (nextGet >= nextPut)
			throw new EOFException();
		
		return buffer[nextGet++];
	}
	
	@Override
	public int get( byte[] buf, int off, int len )
	{
		if (off < 0)
			throw new IllegalArgumentException( "off < 0" );
		
		if (len < 0)
			throw new IllegalArgumentException( "len < 0" );
		
		if (off+len > buf.length)
			throw new IllegalArgumentException( "off+len > buf.length" );
		
		if (len == 0)
			return 0;
		
		int count = Math.min( len, nextPut-nextGet );
		
		if (count > 0)
		{
			System.arraycopy( buffer, nextGet, buf, off, count );
			nextGet += count;
		}
		
		return count;
	}

	@Override
	public void put( byte b ) throws BufferOverflowException
	{
		if (nextPut >= size)
			throw new BufferOverflowException();
		buffer[nextPut++] = b;
	}
	
	@Override
	public int put( byte[] buf, int off, int len )
	{
		if (off < 0)
			throw new IllegalArgumentException( "off < 0" );
		
		if (len < 0)
			throw new IllegalArgumentException( "len < 0" );
		
		if (off+len > buf.length)
			throw new IllegalArgumentException( "off+len > buf.length" );
		
		if (len == 0)
			return 0;
		
		int count = Math.min( len, size-nextPut );
		
		if (count > 0)
		{
			System.arraycopy( buf, off, buffer, nextPut, count );
			nextPut += count;
		}
		
		return count;
	}
}