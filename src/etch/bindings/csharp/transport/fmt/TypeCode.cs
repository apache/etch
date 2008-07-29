using System;
using System.Text;
using System.Collections.Generic;

namespace Etch.Transport.Fmt
{
    /// <summary>
    /// Values denoting types of encoded values. Small integers are encoded
    /// within the type const sbyte. There are three formats:
    /// 
    /// 0xxyyyyy - small positive integer. xx is the code for the type. yyyyy
    ///            encodes the value, 0-31.
    /// 10xxzzzz - small negative integer. xx is the code for the type. zzzz
    ///            encodes the negative of (value plus 1): -1 to -16. 
    /// 11kkkkkk - code for the various other types, -1 to -64
    /// </summary>
    public class TypeCode
    {
        /// <summary>
        /// A code denoting a null value.
        /// </summary>
        public const sbyte NULL = -1;

        /// <summary>
        /// A code denoting a false boolean value.
        /// </summary>
        public const sbyte BOOLEAN_FALSE = -2;

        /// <summary>
        /// A code denoting a true boolean value.
        /// </summary>
        public const sbyte BOOLEAN_TRUE = -3;

        /// <summary>
        /// A code denoting a signed const sbyte.
        /// </summary>
        public const sbyte BYTE1 = -4;

        /// <summary>
        /// A code denoting a single const sbyte signed short.
        /// </summary>
        public const sbyte SHORT1 = -5;

        /// <summary>
        /// A code denoting a two const sbyte signed short, lsb first.
        /// </summary>
        public const sbyte SHORT2 = -6;

        /// <summary>
        /// A code denoting a single const sbyte signed integer.
        /// </summary>
        public const sbyte INT1 = -7;
        
        /// <summary>
        /// A code denoting a two const sbyte signed integer, lsb first.
        /// </summary>
        public const sbyte INT2 = -8;

        /// <summary>
        /// A code denoting a four const sbyte signed integer, lsb first.
        /// </summary>
        public const sbyte INT4 = -9;

        /// <summary>
        /// A code denoting a single const sbyte signed long.
        /// </summary>
        public const sbyte LONG1 = -10;

        /// <summary>
        /// A code denoting a two const sbyte signed long, lsb first.
        /// </summary>
        public const sbyte LONG2 = -11;

        /// <summary>
        /// A code denoting a four const sbyte signed long, lsb first.
        /// </summary>
        public const sbyte LONG4 = -12;

        /// <summary>
        /// A code denoting an eight const sbyte signed long, lsb first.
        /// </summary>
        public const sbyte LONG8 = -13;

        /// <summary>
        /// A code denoting a four const sbyte ieee floating format number.
        /// </summary>
        public const sbyte FLOAT4 = -14;

        /// <summary>
        /// A code denoting an eight const sbyte ieee floating format number.
        /// </summary>
        public const sbyte FLOAT8 = -15;

        /// <summary>
        /// A code denoting an array of const sbytes.
        /// </summary>
        public const sbyte BYTES = -16;

        /// <summary>
        /// A code denoting an empty string.
        /// </summary>
        public const sbyte EMPTY_STRING = -17;

        /// <summary>
        /// A code denoting a utf-8 encoded string.
        /// </summary>
        public const sbyte STRING = -18;

        /// <summary>
        /// A code denoting a sequence of key / value pairs.
        /// </summary>
        public const sbyte STRUCT = -19;

        /// <summary>
        /// A code denoting a sequence of values.
        /// </summary>
        public const sbyte ARRAY = -20;

        /// <summary>
        /// A code denoting a custom value from a value factory. An integer
        /// value follows which identifies the specific type.
        /// </summary>
        public const sbyte CUSTOM = -21;

        /// <summary>
        /// A code denoting no value, which is different than NULL. For
        /// example, an array is a sequence of values (some of which may
        /// be NULL), terminated by a NONE.
        /// </summary>
        public const sbyte NONE = -22;

        /// <summary>
        /// A code denoting that any value is ok (as long as we know how
        /// to serialize it). Dynamic methods should be applied to determine
        /// the type. This type never appears on the wire.
        /// </summary>
        public const sbyte ANY = -23;

        ////////////////////
        // SMALL INTEGERS //
        ////////////////////

        /// <summary>
        /// Minimum "small" integer.
        /// </summary>
        public const sbyte MIN_SMALL_INT = -16;

        /// <summary>
        /// Maximum "small" integer. Small integers are encoded asis
        /// (with embedded type code)
        /// </summary>
        public const sbyte MAX_SMALL_INT = 31;

        /// <summary>
        /// Positive sentinal mask.
        /// </summary>
        public const int PSMASK = 0x80;

        /// <summary>
        /// Positive sentinal value.
        /// </summary>
        public const int PSVALUE = 0x00;

        /// <summary>
        /// Shift for positive data type value.
        /// </summary>
        public const int PDTSHIFT = 5;

        /// <summary>
        /// Mask for positive value.
        /// </summary>
        public const int PVMASK = 31;

        /// <summary>
        /// Negative sentinal mask.
        /// </summary>
        public const int NSMASK = 0xC0;

        /// <summary>
        /// Negative sentinal value.
        /// </summary>
        public const int NSVALUE = 0x80;

        /// <summary>
        /// Shift for negative data type value.
        /// </summary>
        public const int NDTSHIFT = 4;

        /// <summary>
        /// Mask for negative value.
        /// </summary>
        public const int NVMASK = 15;

        /// <summary>
        /// Mask of data type value
        /// </summary>
        public const int DTMASK = 0x03;

        /// <summary>
        /// Data type value for small integer from const sbyte.
        /// </summary>
        public const sbyte BYTE_DT = 0;

        /// <summary>
        /// Data type value for small integer from short.
        /// </summary>
        public const sbyte SHORT_DT = 1;

        /// <summary>
        /// Data type value for small integer from int.
        /// </summary>
        public const sbyte INT_DT = 2;

        /// <summary>
        /// Data type value for small integer from long.
        /// </summary>
        public const sbyte LONG_DT = 3;

        /// <summary>
        /// Base value of byte type positive small ints.
        /// </summary>
        public const sbyte POSITIVE_BYTE_BASE = 0;

        /// <summary>
        /// Base value of short type positive small ints.
        /// </summary>
        public const sbyte POSITIVE_SHORT_BASE = 32;

        /// <summary>
        /// Base value of int type positive small ints.
        /// </summary>
        public const sbyte POSITIVE_INT_BASE = 64;

        /// <summary>
        /// Base value of long type positive small ints.
        /// </summary>
        public const sbyte POSITIVE_LONG_BASE = 96;

        /// <summary>
        /// Base value of byte type negative small ints.
        /// </summary>
        public const sbyte NEGATIVE_BYTE_BASE = -128;

        /// <summary>
        /// Base value of short type negative small ints.
        /// </summary>
        public const sbyte NEGATIVE_SHORT_BASE = -112;

        /// <summary>
        /// Base value of int type negative small ints.
        /// </summary>
        public const sbyte NEGATIVE_INT_BASE = -96;

        /// <summary>
        /// Base value of long type negative small ints.
        /// </summary>
        public const sbyte NEGATIVE_LONG_BASE = -80;
    }
}
