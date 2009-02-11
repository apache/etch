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
using System;
using Org.Apache.Etch.Bindings.Csharp.Msg;

namespace Org.Apache.Etch.Bindings.Csharp.Transport.Fmt.Binary
{
    /// <summary>
    /// Common elements for binary tagged data input and output.
    /// </summary>
    public class BinaryTaggedData : TaggedData
    {
        /// <summary>
        /// Sentinel which marks the end of a struct or array item list
        /// in a binary input or output stream.
        /// </summary>

        public class SentinelObject
        {
            public override string ToString()
            {
                return "NONE";
            }
        }
        public static readonly SentinelObject NONE = new SentinelObject();


        /// <summary>
        /// This is the current version of the protocol.
        /// </summary>
        public const sbyte VERSION = 3;

        public BinaryTaggedData()
        {
            // do nothing
        }

        /// <summary>
        /// Constructs the BinaryTaggedData.
        /// </summary>
        /// <param name="vf"></param>
        public BinaryTaggedData(ValueFactory vf)
            : base(vf)
        {
        }


        public override sbyte CheckValue(object value)
        {
            if (value == null)
                return TypeCode.NULL;
            else if (value == NONE)
                return TypeCode.NONE;

            Type type = value.GetType();

            if (value is long)
                return CheckLong((long)value);

            else if (value is Int32)
                return CheckInteger((Int32)value);

            else if (value is short)
                return CheckShort((short)value);

            else if (value is SByte)
                return CheckByte((sbyte)value);

            else if (value is Double)
                return (sbyte)TypeCode.DOUBLE;

            else if (value is float)
                return (sbyte)TypeCode.FLOAT;

            else if (value is String)
            {
                String s = (String)value;
                if (s.Length == 0)
                    return TypeCode.EMPTY_STRING;

                return TypeCode.STRING;
            }

            else if (value is Boolean)
            {
                if ((Boolean)value)
                    return TypeCode.BOOLEAN_TRUE;

                return TypeCode.BOOLEAN_FALSE;
            }

            else if (value is StructValue)
                return TypeCode.CUSTOM;


            else if (value is Array)
            {
                Array a = (Array)value;
                //if (a.GetType() == typeof(Int32[]))
                //    return TypeCode.INTS;
                //if (a.GetType() == typeof(Int16[]))
                //    return TypeCode.SHORTS;
                //if (a.GetType() == typeof(Boolean[]))
                //    return TypeCode.BOOLS;
                if (a.GetType() == typeof(SByte[]))
                    return TypeCode.BYTES;
                //if (a.GetType() == typeof(Int64[]))
                //    return TypeCode.LONGS;
                //if (a.GetType() == typeof(Single[]))
                //    return TypeCode.FLOATS;
                //if (a.GetType() == typeof(Double[]))
                //    return TypeCode.DOUBLES;
                return TypeCode.ARRAY;

            }

            return TypeCode.CUSTOM;

        }

        public override sbyte GetNativeTypeCode(Type c)
        {
            if (c == typeof(Boolean))
                return TypeCode.BOOLEAN_TRUE;

            if (c == typeof(SByte))
                return TypeCode.BYTE;

            if (c == typeof(short))
                return TypeCode.SHORT;

            if (c == typeof(int))
                return TypeCode.INT;

            if (c == typeof(long))
                return TypeCode.LONG;

            if (c == typeof(float))
                return TypeCode.FLOAT;

            if (c == typeof(double))
                return TypeCode.DOUBLE;

            if (c == typeof(string))
                return TypeCode.STRING;

            if (c == typeof(Object))
                return TypeCode.ANY;

            return TypeCode.CUSTOM;
        }

        public override XType GetCustomStructType(Type c)
        {
            return vf.GetCustomStructType(c);
        }



        public override Type GetNativeType(sbyte type)
        {
            switch (type)
            {
                case TypeCode.BOOLEAN_TRUE:
                case TypeCode.BOOLEAN_FALSE:
                    return typeof(Boolean);

                case TypeCode.BYTE:
                    return typeof(sbyte);

                case TypeCode.SHORT:
                    return typeof(short);

                case TypeCode.INT:
                    return typeof(int);

                case TypeCode.LONG:
                    return typeof(long);

                case TypeCode.FLOAT:
                    return typeof(float);

                case TypeCode.DOUBLE:
                    return typeof(double);

                case TypeCode.EMPTY_STRING:
                case TypeCode.STRING:
                    return typeof(string);

                case TypeCode.ANY:
                    return typeof(Object);

                default:
                    throw new Exception("unsupported native type " + type);
            }
        }

        private static sbyte CheckByte(sbyte v)
        {
            if (v >= TypeCode.MIN_TINY_INT && v <= TypeCode.MAX_TINY_INT)
                return v;
            return TypeCode.BYTE;
        }

        private static sbyte CheckShort(short v)
        {
            if (v >= SByte.MinValue && v <= SByte.MaxValue)
                return CheckByte((sbyte)v);

            return TypeCode.SHORT;
        }

        private static sbyte CheckInteger(int v)
        {
            if (v >= short.MinValue && v <= short.MaxValue)
                return CheckShort((short)v);

            return TypeCode.INT;
        }

        private static sbyte CheckLong(long v)
        {
            if (v >= int.MinValue && v <= int.MaxValue)
                return CheckInteger((int)v);

            return TypeCode.LONG;
        }

    }
}
