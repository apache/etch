/* $Id$
 *
 * Created by sccomer on Jan 17, 2007.
 *
 * Copyright (c) 2007 Cisco Systems, Inc. All rights reserved.
 */

package etch.bindings.java.transport.fmt;

/**
 * @author wert
 *
 */
/**
 * Values denoting types of encoded values. Small integers are encoded
 * within the type byte. There are three formats:
 * 
 * 0xxyyyyy - small positive integer. xx is the code for the type. yyyyy
 *            encodes the value, 0-31.
 * 10xxzzzz - small negative integer. xx is the code for the type. zzzz
 *            encodes the negative of (value plus 1): -1 to -16.
 * 11kkkkkk - code for the various other types, -1 to -64
 */
public interface TypeCode
{
	/**
	 * A code denoting a null value.
	 */
	public byte NULL = -1;
	/**
	 * A code denoting a false boolean value.
	 */
	public byte BOOLEAN_FALSE = -2;
	/**
	 * A code denoting a true boolean value.
	 */
	public byte BOOLEAN_TRUE = -3;
	/**
	 * A code denoting a signed byte.
	 */
	public byte BYTE1 = -4;
	/**
	 * A code denoting a single byte signed short.
	 */
	public byte SHORT1 = -5;
	/**
	 * A code denoting a two byte signed short, msb first.
	 */
	public byte SHORT2 = -6;
	/**
	 * A code denoting a single byte signed integer.
	 */
	public byte INT1 = -7;
	/**
	 * A code denoting a two byte signed integer, msb first.
	 */
	public byte INT2 = -8;
	/**
	 * A code denoting a four byte signed integer, msb first.
	 */
	public byte INT4 = -9;
	/**
	 * A code denoting a single byte signed long.
	 */
	public byte LONG1 = -10;
	/**
	 * A code denoting a two byte signed long, msb first.
	 */
	public byte LONG2 = -11;
	/**
	 * A code denoting a four byte signed long, msb first.
	 */
	public byte LONG4 = -12;
	/**
	 * A code denoting an eight byte signed long, msb first.
	 */
	public byte LONG8 = -13;
	/**
	 * A code denoting a four byte ieee floating format number.
	 */
	public byte FLOAT4 = -14;
	/**
	 * A code denoting an eight byte ieee floating format number.
	 */
	public byte FLOAT8 = -15;
	/**
	 * A code denoting an array of bytes.
	 */
	public byte BYTES = -16;
	/**
	 * A code denoting an empty string.
	 */
	public byte EMPTY_STRING = -17;
	/**
	 * A code denoting a utf-8 encoded string.
	 */
	public byte STRING = -18;
	/**
	 * A code denoting a sequence of key / value pairs.
	 */
	public byte STRUCT = -19;
	/**
	 * A code denoting a sequence of values.
	 */
	public byte ARRAY = -20;
	/**
	 * A code denoting a custom value from a value factory. An integer
	 * value follows which identifies the specific type.
	 */
	public byte CUSTOM = -21;
	/**
	 * A code denoting no value, which is different than NULL. For
	 * example, an array is a sequence of values (some of which may
	 * be NULL), terminated by a NONE.
	 */
	public byte NONE = -22;
	/**
	 * A code denoting that any value is ok (as long as we know how
	 * to serialize it). Dynamic methods should be applied to determine
	 * the type. This type never appears on the wire.
	 */
	public byte ANY = -23;

	////////////////////
	// SMALL INTEGERS //
	////////////////////
	
	/**
	 * Minimum "small" integer.
	 */
	public byte MIN_SMALL_INT = -16;
	/**
	 * Maximum "small" integer. Small integers are encoded asis
	 * (with embedded type code)
	 */
	public byte MAX_SMALL_INT = 31;
	/**
	 * Positive sentinal mask.
	 */
	public int PSMASK = 0x80;
	/**
	 * Positive sentinal value.
	 */
	public int PSVALUE = 0x00;
	/**
	 * Shift for positive data type value.
	 */
	public int PDTSHIFT = 5;
	/**
	 * Mask for positive value.
	 */
	public int PVMASK = 31;
	/**
	 * Negative sentinal mask.
	 */
	public int NSMASK = 0xC0;
	/**
	 * Negative sentinal value.
	 */
	public int NSVALUE = 0x80;
	/**
	 * Shift for negative data type value.
	 */
	public int NDTSHIFT = 4;
	/**
	 * Mask for negative value.
	 */
	public int NVMASK = 15;
	/**
	 * Mask for data type value.
	 */
	public int DTMASK = 0x03;
	/**
	 * Data type value for small integer from byte.
	 */
	public byte BYTE_DT = 0;
	/**
	 * Data type value for small integer from short.
	 */
	public byte SHORT_DT = 1;
	/**
	 * Data type value for small integer from int.
	 */
	public byte INT_DT = 2;
	/**
	 * Data type value for small integer from long.
	 */
	public byte LONG_DT = 3;
	/**
	 * Base value of byte type positive small ints.
	 */
	public byte POSITIVE_BYTE_BASE = 0;
	/**
	 * Base value of short type positive small ints.
	 */
	public byte POSITIVE_SHORT_BASE = 32;
	/**
	 * Base value of int type positive small ints.
	 */
	public byte POSITIVE_INT_BASE = 64;
	/**
	 * Base value of long type positive small ints.
	 */
	public byte POSITIVE_LONG_BASE = 96;
	/**
	 * Base value of byte type negative small ints.
	 */
	public byte NEGATIVE_BYTE_BASE = -128;
	/**
	 * Base value of short type negative small ints.
	 */
	public byte NEGATIVE_SHORT_BASE = -112;
	/**
	 * Base value of int type negative small ints.
	 */
	public byte NEGATIVE_INT_BASE = -96;
	/**
	 * Base value of long type negative small ints.
	 */
	public byte NEGATIVE_LONG_BASE = -80;
}