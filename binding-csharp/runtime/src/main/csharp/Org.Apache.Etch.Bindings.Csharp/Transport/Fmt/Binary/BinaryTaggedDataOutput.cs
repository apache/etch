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
using System.Collections.Generic;
using System.Text;
using Org.Apache.Etch.Bindings.Csharp.Msg;
using Org.Apache.Etch.Bindings.Csharp.Support;
using Org.Apache.Etch.Bindings.Csharp.Util;

namespace Org.Apache.Etch.Bindings.Csharp.Transport.Fmt.Binary
{
    /// <summary>
    /// An implementation of TaggedDataOutput which uses a binary encoding.
    /// </summary>
    public sealed class BinaryTaggedDataOutput : BinaryTaggedData, TaggedDataOutput
    {
        /// <summary>
        /// Name of uri parameter which controls whether we write ints or strings
        /// for types and fields.
        /// </summary>
        public const String STRING_TYPE_AND_FIELD =
		    "BinaryTaggedDataOutput.stringTypeAndField";

        /// <summary>
        /// Constructs the BinaryTaggedDataOutput with a null buffer.
        /// </summary>
        /// <param name="vf">the value factory for the service.</param>
        /// <param name="uri">the uri used to construct the transport stack.</param>
        public BinaryTaggedDataOutput(ValueFactory vf, string uri)
            : base(vf)
        {
            this.level = vf.GetLevel();
            URL u = new URL(uri);
            stringTypeAndField = u.GetBooleanTerm(STRING_TYPE_AND_FIELD, false);
        }

        private readonly Validator.Level level;

        private readonly bool stringTypeAndField;

        //////////////////////////////
        // TaggedDataOutput methods //
        //////////////////////////////

        #region TaggedDataOutput methods

        public void WriteMessage(Message msg, FlexBuffer buf)
        {
            try
            {
                this.buf = buf;
                StartMessage(msg);
                WriteKeysAndValues(msg);
                EndMessage(msg);
            }
            finally
            {
                this.buf = null;
            }
        }

        private FlexBuffer buf;

        private void WriteStruct(StructValue sv)
        {
            StartStruct(sv);
            WriteKeysAndValues(sv);
            EndStruct(sv);
        }

        private void WriteArray(ArrayValue av, Validator v)
        {
            StartArray(av);
            WriteValues(av, v);
            EndArray(av);
        }

        private void WriteKeysAndValues(StructValue sv)
        {
            XType t = sv.GetXType;
            foreach (KeyValuePair<Field, Object> me in sv)
            {
                Field f = me.Key;
                WriteField(f);
                WriteValue(t.GetValidator(f), me.Value);
            }
        }

        private void WriteValues(ArrayValue av, Validator v)
        {
            Validator ev = v.ElementValidator();
            foreach (Object value in av)
                WriteValue(ev, value);
        }


        /////////////////////////
        // Main output methods //
        /////////////////////////

        /// <summary>
        /// 
        /// </summary>
        /// <param name="msg"></param>
        /// Exception:
        ///     throws Exception
        private void StartMessage(Message msg)
        {
            // since message is top-level, no type code is written to
            // indicate a message is starting. we do write a version number
            // to indicate this version of the binary tagged data output
            buf.PutByte(VERSION);
            StartStruct(msg);
            //       WriteType( msg.GetXType );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="msg"></param>
        /// Exception:
        ///     throws Exception
        private void EndMessage(Message msg)
        {
            EndStruct(msg);

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sv"></param>
        /// Exception:
        ///     throws Exception
        private void StartStruct(StructValue sv)
        {
            // the caller has already written a type code to indicate a
            // struct is starting.
            WriteType(sv.GetXType);
            WriteIntValue(sv.Count);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sv"></param>
        /// Exception:
        ///     throws Exception
        private void EndStruct(StructValue sv)
        {
            WriteNoneValue();
        }

        private readonly IList<StructValue> structs = new List<StructValue>();

        private void StartArray(ArrayValue array)
        {
            // the caller has already written a type code to indicate an
            // array is starting.
            sbyte type = array.Type;
            buf.PutByte(type);
            if (type == TypeCode.CUSTOM)
                WriteType(array.CustomStructType);
            WriteIntValue(array.Dim);
            WriteIntValue(array.Size());

        }

        private void WriteType(XType type)
        {
            if (stringTypeAndField)
                WriteStringValue(type.Name);
            else
                WriteIntValue(type.Id);
        }

        private void WriteField(Field field)
        {
            if (stringTypeAndField)
                WriteStringValue(field.Name);
            else
                WriteIntValue(field.Id);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="array"></param>
        /// Exception:
        ///     throws Exception
        private void EndArray(ArrayValue array)
        {
            WriteNoneValue();
        }

        private void WriteIntValue(int value)
        {
            WriteValue(intValidator, value);
        }

        private readonly Validator intValidator = Validator_int.Get(0);

        private void WriteStringValue(String value)
        {
            WriteValue(stringValidator, value);
        }

        private readonly Validator stringValidator = Validator_string.Get(0);

        private void WriteNoneValue()
        {
            WriteValue(noneValidator, NONE);
        }

        private readonly Validator noneValidator = Validator_none.Get();

        #endregion

        ///////////////////////////
        // LOCAL UTILITY METHODS //
        ///////////////////////////

        #region Local Utility Methods

        /// <summary>
        /// 
        /// </summary>
        /// <param name="v">the validator for this value, or none if there
        /// isn't one</param>
        /// <param name="value"></param>
        /// Exception:
        ///     throws Exception
        private void WriteValue(Validator v, Object value)
        {
            sbyte typeCode = CheckValue(value);

            if (level != Validator.Level.NONE && value != null)
            {
                if (level == Validator.Level.FULL && v == null)
                    throw new ArgumentException("validator missing");
                
                if (v != null && !v.Validate(value))
                    throw new ArgumentException( String.Format(
                        "validator {0} failed for value {1}", v, value));
            }

            buf.PutByte(typeCode);

            switch (typeCode)
            {
                case TypeCode.NULL:
                case TypeCode.BOOLEAN_FALSE:
                case TypeCode.BOOLEAN_TRUE:
                case TypeCode.EMPTY_STRING:
                case TypeCode.NONE:
                    return;

                case TypeCode.BYTE:
                    buf.PutByte((Convert.ToSByte(value)));
                    return;
                case TypeCode.SHORT:
                    buf.PutShort(Convert.ToInt16(value));
                    return;

                case TypeCode.INT:
                    buf.PutInt(Convert.ToInt32(value));
                    return;

                case TypeCode.LONG:
                    buf.PutLong(Convert.ToInt64(value));
                    return;

                case TypeCode.FLOAT:
                    buf.PutFloat(Convert.ToSingle(value));
                    return;

                case TypeCode.DOUBLE:
                    buf.PutDouble(Convert.ToDouble(value));
                    return;

                case TypeCode.BYTES:
                    WriteBytes((sbyte[])value);
                    return;

                // reserved for future use
                //case TypeCode.BOOLS:
                //case TypeCode.SHORTS:
                //case TypeCode.INTS:
                //case TypeCode.LONGS:
                //case TypeCode.FLOATS:
                //case TypeCode.DOUBLES:
                case TypeCode.ARRAY:
                    WriteArray(ToArrayValue(value, v), v);
                    return;

                case TypeCode.STRING:
                    WriteBytes(Encoding.UTF8.GetBytes((String)value));
                    return;

                case TypeCode.CUSTOM:
                    {
                        StructValue sv = vf.ExportCustomValue(value);
                        if (sv == null)
                            throw new Exception("unsupported type " + value.GetType());

                        WriteStruct(sv);
                        return;
                    }

                default:
                    // type is either "tiny" integer or unused...
                    if (typeCode >= TypeCode.MIN_TINY_INT && typeCode <= TypeCode.MAX_TINY_INT)
                        return;

                    throw new Exception("unsupported typecode " + typeCode);
            }
        }

        /// <summary>
        /// Writes a value (signed or unsigned) to the output stream.
        /// </summary>
        /// <param name="value">the value to write to the output stream.</param>
        /// Exception:
        ///     throws Exception if there's a problem with the output stream
        private void WriteBytes(sbyte[] value)
        {
            int n = value.Length;
            WriteIntValue(n);
            buf.Put((byte[])(Array)value, 0, n);
        }

        /// <summary>
        /// Writes a value (signed or unsigned) to the output stream.
        /// </summary>
        /// <param name="value">the value to write to the output stream.</param>
        /// Exception:
        ///     throws Exception if there's a problem with the output stream
        private void WriteBytes(byte[] value)
        {
            int n = value.Length;
            WriteIntValue(n);
            buf.Put(value, 0, n);
        }

        #endregion Local utility methods
    }
}
