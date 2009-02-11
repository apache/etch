// $Id$
// 
// Licensed to the Apache Software Foundation (ASF) under one
// or more contributor license agreements. See the NOTICE file
// distributed with this work for additional information
// regarding copyright ownership. The ASF licenses this file
// to you under the Apache License, Version 2.0 (the
// "License"); you may not use this file except in compliance
// with the License. You may obtain a copy of the License at
// 
//   http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing,
// software distributed under the License is distributed on an
// "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
// KIND, either express or implied. See the License for the
// specific language governing permissions and limitations
// under the License.
// 
namespace Org.Apache.Etch.Bindings.Csharp.Transport.Fmt
{
    /// <summary>
    /// Values denoting types of encoded values. There are two value ranges:
    /// 
    ///  -128 to -65: codes for defined types.
    /// -64 to 127: tiny integer values.
    ///            
    /// Tiny integers are completely encoded within the type byte.
    /// </summary>
    public class TypeCode
    {
        /// <summary>
        /// A code denoting a null value.
        /// </summary>
        public const sbyte NULL = -128;

        /// <summary>
        /// A code denoting no value. 
        /// For example, an array is a sequence of values (some of which may
	    /// be NULL), terminated by a NONE.
        /// </summary>
        public const sbyte NONE = -127;


        /// <summary>
        /// A code denoting a false boolean value.
        /// </summary>
        public const sbyte BOOLEAN_FALSE = -126;

        /// <summary>
        /// A code denoting a true boolean value.
        /// </summary>
        public const sbyte BOOLEAN_TRUE = -125;

        /// <summary>
        /// A code denoting a signed const sbyte.
        /// </summary>
        public const sbyte BYTE = -124;

        /// <summary>
        /// A code denoting a single const sbyte signed short.
        /// </summary>
        public const sbyte SHORT = -123;

        /// <summary>
        /// A code denoting a four sbyte signed integer.
        /// </summary>
        public const sbyte INT = -122;
             
        /// <summary>
        /// A code denoting an eight const sbyte signed long, lsb first.
        /// </summary>
        public const sbyte LONG = -121;

        /// <summary>
        /// A code denoting a four const sbyte ieee floating format number.
        /// </summary>
        public const sbyte FLOAT = -120;

        /// <summary>
        /// A code denoting an eight const sbyte ieee floating format number.
        /// </summary>
        public const sbyte DOUBLE = -119;

        //// Arrays /////

        /// <summary>
        /// A code denoting an array of booleans.
        /// </summary>
        //public const sbyte BOOLS = -118;

        /// <summary>
        /// A code denoting an array of bytes.
        /// </summary>
        public const sbyte BYTES = -117;

        /// <summary>
        /// A code denoting an array of shorts.
        /// </summary>
        //public const sbyte SHORTS = -116;

        /// <summary>
        /// A code denoting an array of ints.
        /// </summary>
        //public const sbyte INTS = -115;

        /// <summary>
        /// A code denoting an array of longs.
        /// </summary>
        //public const sbyte LONGS = -114;

        /// <summary>
        /// A code denoting an array of singles.
        /// </summary>
        //public const sbyte FLOATS = -113;

        /// <summary>
        /// A code denoting an array of doubles.
        /// </summary>
        //public const sbyte DOUBLES = -112;

        /// <summary>
        /// A code denoting a sequence of values.
        /// </summary>
        public const sbyte ARRAY = -111;


        //// strings /////
        
        /// <summary>
        /// A code denoting an empty string.
        /// </summary>
        public const sbyte EMPTY_STRING = -110;

        /// <summary>
        /// A code denoting a utf-8 encoded string.
        /// </summary>
        public const sbyte STRING = -109;

        /// <summary>
        /// A code denoting a custom value (struct, exception, enum, extern)
	    /// from a value factory. An associated value identifies the specific
	    /// type. The format on the wire of STRUCT and CUSTOM are the same.
	    /// Emit CUSTOM instead of STRUCT, and accept STRUCT as if it were
	    /// CUSTOM.
        /// </summary>
        public const sbyte STRUCT = -108;



        /// <summary>
        /// A code denoting a custom value (struct, exception, enum, extern)
        /// from a value factory. An associated value identifies the specific
        /// type. The format on the wire of STRUCT and CUSTOM are the same.
        /// Emit CUSTOM instead of STRUCT, and accept STRUCT as if it were
        /// CUSTOM.
        /// </summary>
        public const sbyte CUSTOM = -107;


        /// <summary>
        /// A code denoting that any value is ok (as long as we know how
        /// to serialize it). Dynamic methods should be applied to determine
        /// the type. This type never appears on the wire.
        /// </summary>
        public const sbyte ANY = -106;

        ////////////////////
        // tiny integers //
        ////////////////////

        /// <summary>
        /// Minimum "small" integer.
        /// </summary>
        public const sbyte MIN_TINY_INT = -64;

        /// <summary>
        /// Maximum "small" integer. Small integers are encoded asis
        /// (with embedded type code)
        /// </summary>
        public const sbyte MAX_TINY_INT = 127;

        
    }
}
