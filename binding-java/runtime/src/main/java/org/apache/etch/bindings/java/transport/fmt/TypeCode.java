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

package org.apache.etch.bindings.java.transport.fmt;

/**
 * Values denoting types of encoded values. There are two value ranges:
 * 
 * -128 to -65: codes for defined types.
 * -64 to 127: tiny integer values.
 * 
 * Tiny integers are completely encoded within the type byte.
 */
public interface TypeCode
{
	/**
	 * A code denoting a null value.
	 */
	public byte NULL = -128;
	
	/**
	 * A code denoting no value, which is different than NULL. For
	 * example, an array is a sequence of values (some of which may
	 * be NULL), terminated by a NONE.
	 */
	public byte NONE = -127;

	//////////////////////////////////////////////////
	// scalar values (but, also see tiny int below) //
	//////////////////////////////////////////////////
	
	/**
	 * A code denoting a false boolean value.
	 */
	public byte BOOLEAN_FALSE = -126;
	
	/**
	 * A code denoting a true boolean value.
	 */
	public byte BOOLEAN_TRUE = -125;
	
	/**
	 * A code denoting a signed byte.
	 */
	public byte BYTE = -124;
	
	/**
	 * A code denoting a two byte signed short, msb first.
	 */
	public byte SHORT = -123;
	
	/**
	 * A code denoting a four byte signed integer, msb first.
	 */
	public byte INT = -122;
	
	/**
	 * A code denoting an eight byte signed long, msb first.
	 */
	public byte LONG = -121;
	
	/**
	 * A code denoting a four byte ieee floating format number (single,
	 * written as an INT).
	 */
	public byte FLOAT = -120;
	
	/**
	 * A code denoting an eight byte ieee floating format number (double,
	 * written as a LONG).
	 */
	public byte DOUBLE = -119;

	////////////
	// arrays //
	////////////
	
	// BOOLS, SHORTS, INTS, LONGS, FLOATS, and DOUBLES are
	// reserved for future use.

//	/**
//	 * A code denoting an array of booleans.
//	 */
//	public byte BOOLS = -118;
	
	/**
	 * A code denoting an array of bytes.
	 */
	public byte BYTES = -117;

//	/**
//	 * A code denoting an array of shorts.
//	 */
//	public byte SHORTS = -116;

//	/**
//	 * A code denoting an array of ints.
//	 */
//	public byte INTS = -115;

//	/**
//	 * A code denoting an array of longs.
//	 */
//	public byte LONGS = -114;

//	/**
//	 * A code denoting an array of ieee format singles.
//	 */
//	public byte FLOATS = -113;

//	/**
//	 * A code denoting an array of ieee format doubles.
//	 */
//	public byte DOUBLES = -112;
	
	/**
	 * A code denoting a sequence of values.
	 */
	public byte ARRAY = -111;
	
	/////////////
	// strings //
	/////////////
	
	/**
	 * A code denoting an empty string.
	 */
	public byte EMPTY_STRING = -110;
	
	/**
	 * A code denoting a utf-8 encoded string.
	 */
	public byte STRING = -109;

	/////////////////////
	// structured data //
	/////////////////////
	
	/**
	 * A code denoting a custom value (struct, exception, enum, extern)
	 * from a value factory. An associated value identifies the specific
	 * type. The format on the wire of STRUCT and CUSTOM are the same.
	 * Emit CUSTOM instead of STRUCT, and accept STRUCT as if it were
	 * CUSTOM.
	 */
	@Deprecated
	public byte STRUCT = -108;
	
	/**
	 * A code denoting a custom value (struct, exception, enum, extern)
	 * from a value factory. An associated value identifies the specific
	 * type. The format on the wire of STRUCT and CUSTOM are the same.
	 * Emit CUSTOM instead of STRUCT, and accept STRUCT as if it were
	 * CUSTOM.
	 */
	public byte CUSTOM = -107;

	//////////
	// misc //
	//////////
	
	/**
	 * A code denoting that any value is ok (as long as we know how
	 * to serialize it). Dynamic methods should be applied to determine
	 * the type. This type never appears on the wire.
	 */
	public byte ANY = -106;

	///////////////////
	// tiny integers //
	///////////////////
	
	/**
	 * Minimum "tiny" integer.
	 */
	public byte MIN_TINY_INT = -64;
	
	/**
	 * Maximum "tiny" integer.
	 */
	public byte MAX_TINY_INT = 127;
}