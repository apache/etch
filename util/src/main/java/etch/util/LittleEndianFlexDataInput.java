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
 * Implementation of FlexDataInput which reads 2, 4, and 8
 * byte integers using little-endian semantics (low-order byte
 * is first).
 */
final public class LittleEndianFlexDataInput extends FlexDataInput
{
	/**
	 * Constructs the LittleEndianFlexDataInput.
	 * @param buffer
	 */
	public LittleEndianFlexDataInput( FlexBuffer buffer )
	{
		super( buffer );
	}

	public short getShort() throws IOException
    {
		buffer.checkAvail( 2 );
		int value = buffer.get();
		return (short) (value + (buffer.get() << 8));
    }
	
	public int getInt() throws IOException
	{
		buffer.checkAvail( 4 );
		int value = buffer.get();
		value += (buffer.get() << 8);
		value += (buffer.get() << 16);
		return value + (buffer.get() << 24);
	}

	public long getLong() throws IOException
    {
		buffer.checkAvail( 8 );
		long value = buffer.get();
		value += (((long)buffer.get()) << 8);
		value += (((long)buffer.get()) << 16);
		value += (((long)buffer.get()) << 24);
		value += (((long)buffer.get()) << 32);
		value += (((long)buffer.get()) << 40);
		value += (((long)buffer.get()) << 48);
		return value + (((long)buffer.get()) << 56);
    }
}
