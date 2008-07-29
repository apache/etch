using System;
using System.Text;
using System.Collections.Generic;
using System.Diagnostics;
using Etch.Msg;
using Etch.Util;

namespace Etch.Transport.Fmt.Binary
{
    /// <summary>
    /// Description of TaggedDataOutputStream.
    /// </summary>
    public sealed class BinaryTaggedDataOutput : BinaryTaggedData, TaggedDataOutput
    {
        /// <summary>
        /// Constructs the BinaryTaggedDataOutput.
        /// </summary>

        /// <param name="vf"></param>
        public BinaryTaggedDataOutput(ValueFactory vf)
            : base(vf)
        {

        }

        /// <summary>
        /// Constructs the BinaryTaggedDataOutput with a null buffer.
        /// </summary>
        /// <param name="vf">the value factory for the service.</param>
        /// <param name="uri">the uri used to construct the transport stack.</param>
        public BinaryTaggedDataOutput(ValueFactory vf, string uri)
            : base(vf)
        { }

        private FlexBuffer buf;

        //////////////////////////////
        // TaggedDataOutput methods //
        //////////////////////////////

        #region TaggedDataOutput methods

        public void SetBuffer(FlexBuffer buf)
        {
            this.buf = buf;
        }

        public void WriteMessage(Message msg, FlexBuffer buf)
        {
            try
            {
                SetBuffer(buf);
                StartMessage(msg);
                WriteKeysAndValues(msg);
                EndMessage(msg);
            }
            finally
            {
                SetBuffer(null);
            }
        }

        public void WriteStruct(StructValue sv)
        {
            StartStruct(sv);
            WriteKeysAndValues(sv);
            EndStruct(sv);
        }

        public void WriteArray(ArrayValue av, Validator v)
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
                WriteIntValue(f.Id);
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
        public void StartMessage(Message msg)
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
        public void EndMessage(Message msg)
        {
            //WriteValue( noneValidator, NONE );
            EndStruct(msg);

        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="sv"></param>
        /// Exception:
        ///     throws Exception
        public void StartStruct(StructValue sv)
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
        public void EndStruct(StructValue sv)
        {
            //    WriteValue( noneValidator, NONE );
            WriteNoneValue();
        }

        private readonly IList<StructValue> structs = new List<StructValue>();

        public void StartArray(ArrayValue array)
        {
            // the caller has already written a type code to indicate an
            // array is starting.
            sbyte type = array.Type;
            buf.PutByte(type);
            if (type == TypeCode.CUSTOM)
                WriteType(array.CustomStructType);
            //        WriteValue( intValidator, array.Dim );
            WriteIntValue(array.Dim);
            WriteIntValue(array.Size());

        }

        private void WriteType(XType type)
        {
            WriteIntValue(type.Id);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="array"></param>
        /// Exception:
        ///     throws Exception
        public void EndArray(ArrayValue array)
        {
            WriteNoneValue();
        }

        private void WriteIntValue(int? value)
        {
            WriteValue(intValidator, value);
        }

        private readonly Validator intValidator = Validator_int.Get(0);

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
        public void WriteValue(Validator v, Object value)
        {
            sbyte type = CheckValue(value);



            buf.PutByte(type);

            switch (type)
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
                    //            WriteBytes(Encoding.UTF8.GetBytes(Convert.ToString(value)));
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
                    if (type >= TypeCode.MIN_TINY_INT && type <= TypeCode.MAX_TINY_INT)
                        return;

                    throw new Exception("unsupported typecode " + type);
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
            //     WriteValue( intValidator, n );

      /*     byte[] bvalue = new byte[Buffer.ByteLength(value)];
            Buffer.BlockCopy(value, 0, bvalue, 0, Buffer.ByteLength(value)); */

            byte[] bvalue = (byte[])(Array)value;
           

            buf.Put(bvalue, 0, n);
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
            WriteValue(intValidator, n);
            buf.Put(value, 0, n);
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="msg"></param>
        /// <param name="vf"></param>
        /// <returns></returns>
        /// Exception:
        ///     throws Exception
        public static byte[] GetBytes(Message msg, ValueFactory vf)
        {
            FlexBuffer buf = new FlexBuffer();
            BinaryTaggedDataOutput tdo = new BinaryTaggedDataOutput(vf);
            tdo.WriteMessage(msg, buf);
            return buf.GetAvailBytes();
        }

        #endregion Local utility methods
    }
}
