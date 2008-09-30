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
 * Implementation of DataInput using a FlexBuffer.
 */
abstract public class FlexDataInput implements DataInput
{
	/**
	 * Constructs a FlexDataInput with the specified FlexBuffer.
	 * 
	 * @param buffer
	 */
	public FlexDataInput( FlexBuffer buffer )
	{
		this.buffer = buffer;
	}

	protected final FlexBuffer buffer;
	
	public FlexBuffer buffer()
	{
		return buffer;
	}

	final public int index()
	{
		return buffer.index();
	}

	final public void setIndex( int index ) throws IllegalArgumentException
	{
		buffer.setIndex( index );
	}

	final public int length()
	{
		return buffer.length();
	}

	final public void setLength( int length ) throws IllegalArgumentException,
		IOException
	{
		buffer.setLength( length, false );
	}

	final public int avail()
	{
		return buffer.avail();
	}

	final public void checkAvail( int length ) throws IOException
	{
		buffer.checkAvail( length );
	}

	final public void reset()
	{
		buffer.reset();
	}

	final public void compact()
	{
		buffer.compact();
	}

	final public void skip( int length ) throws IOException
	{
		buffer.skip( length, false );
	}

	final public int get() throws IOException
	{
		return buffer.get();
	}

	final public int get( byte[] buf ) throws IOException
	{
		return buffer.get( buf );
	}

	final public int get( byte[] buf, int off, int len ) throws IOException
	{
		return buffer.get( buf, off, len );
	}

	final public void getFully( byte[] buf ) throws IOException
	{
		buffer.getFully( buf );
	}

	final public byte[] getBytes( int length ) throws IOException
	{
		return buffer.getBytes( length );
	}

	final public byte[] getAvailBytes() throws IOException
	{
		return buffer.getAvailBytes();
	}

	final public byte getByte() throws IOException
	{
		return (byte) buffer.get();
	}

	final public float getFloat() throws IOException
	{
		return Float.intBitsToFloat( getInt() );
	}

	final public double getDouble() throws IOException
	{
		return Double.longBitsToDouble( getLong() );
	}
}
