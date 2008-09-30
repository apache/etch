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
 * Implementation of DataOutput using a FlexBuffer.
 */
abstract public class FlexDataOutput implements DataOutput
{
	/**
	 * Constructs the FlexDataOutput with the specified FlexBuffer.
	 * 
	 * @param buffer
	 */
	public FlexDataOutput( FlexBuffer buffer )
	{
		this.buffer = buffer;
	}

	protected final FlexBuffer buffer;
	
	public FlexBuffer buffer()
	{
		return buffer;
	}

	public int index()
	{
		return buffer.index();
	}

	public void setIndex( int index ) throws IllegalArgumentException
	{
		buffer.setIndex( index );
	}

	public int length()
	{
		return buffer.length();
	}

	public void setLength( int length ) throws IllegalArgumentException,
		IOException
	{
		buffer.setLength( length, true );
	}
	
	public int avail()
	{
		return buffer.avail();
	}

	public void reset()
	{
		buffer.reset();
	}

	public void compact()
	{
		buffer.compact();
	}

	public void skip( int length ) throws IOException
	{
		buffer.skip( length, true );
	}

	public void put( int b ) throws IOException
	{
		buffer.put( b );
	}

	public void put( byte[] buf ) throws IOException
	{
		buffer.put( buf );
	}

	public void put( byte[] buf, int off, int len ) throws IOException
	{
		buffer.put( buf, off, len );
	}

	public void put( DataInput buf ) throws IOException
	{
		buffer.put( buf );
	}

	public void put( DataInput buf, int len ) throws IOException
	{
		buffer.put( buf, len );
	}

	public void putByte( byte value ) throws IOException
	{
		buffer.put( value );
	}

	public void putFloat( float value ) throws IOException
	{
		putInt( Float.floatToIntBits( value ) );
	}

	public void putDouble( double value ) throws IOException
	{
		putLong( Double.doubleToLongBits( value ) );
	}
}
