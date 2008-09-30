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
 * Implementation of FlexDataOutput which writes 2, 4, and 8 byte integers using
 * big-endian semantics (high-order byte is first).
 */
public class LittleEndianFlexDataOutput extends FlexDataOutput
{
	/**
	 * Constructs the BigEndianFlexDataOutput.
	 * 
	 * @param buffer
	 */
	public LittleEndianFlexDataOutput( FlexBuffer buffer )
	{
		super( buffer );
	}

	public void putShort( short value ) throws IOException
	{
		buffer.ensureSpace( 2 );
		buffer.put( value );
		buffer.put( value >> 8 );
	}

	public void putInt( int value ) throws IOException
	{
		buffer.ensureSpace( 4 );
		buffer.put( value );
		buffer.put( value >> 8 );
		buffer.put( value >> 16 );
		buffer.put( value >> 24 );
	}

	public void putLong( long value ) throws IOException
	{
		buffer.ensureSpace( 8 );
		buffer.put( (int) value );
		buffer.put( (int) (value >> 8) );
		buffer.put( (int) (value >> 16) );
		buffer.put( (int) (value >> 24) );
		buffer.put( (int) (value >> 32) );
		buffer.put( (int) (value >> 40) );
		buffer.put( (int) (value >> 48) );
		buffer.put( (int) (value >> 56) );
	}
}
