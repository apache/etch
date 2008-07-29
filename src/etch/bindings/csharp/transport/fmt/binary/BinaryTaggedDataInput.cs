using System;
using System.IO;
using System.Text;
using System.Collections.Generic;
using Etch.Msg;
using Etch.Util;
using Etch.Support;

namespace Etch.Transport.Fmt.Binary
{
    /// <summary>
    /// TaggedDataInputStream has methods to support reading little-endian integer
    /// and floating point data types as well as tagged values from an input
    /// stream.
    /// </summary>
    public class BinaryTaggedDataInput : BinaryTaggedData, TaggedDataInput
    {
        /// <summary>
        /// Constructs the BinaryTaggedDataInput.
        /// </summary>
        /// <param name="vf">the value factory for the service</param>
        /// 
        public BinaryTaggedDataInput(ValueFactory vf) : base(vf)
        {
            
          
        }

        /// <summary>
        /// Constructs the BinaryTaggedDataInput with a null buffer.
        /// </summary>
        /// <param name="vf">the value factory for the service.</param>
        /// <param name="uri">the uri used to construct the transport stack.</param>
        public BinaryTaggedDataInput( ValueFactory vf, string uri ) : base( vf )
        {}

        private FlexBuffer buf;

        /////////////////////////////
        // TaggedDataInput methods //
        /////////////////////////////


        public Message ReadMessage(FlexBuffer buf)
        {
            this.buf = buf;
            try
            {
                Message msg = StartMessage();
                ReadKeysAndValues(msg);
                EndMessage(msg);
                return msg;
            }
            finally
            {
                this.buf = null;
            }

        }

        private StructValue ReadStruct()
        {
            StructValue sv = StartStruct();
            ReadKeysAndValues( sv );
            EndStruct( sv );
            return sv;

        }

        private ArrayValue ReadArray( Validator v )
        {
            ArrayValue av = StartArray();
            ReadValues( av, v );
            EndArray( av );
            return av;
        }


        private void ReadKeysAndValues( StructValue sv )
        {
            XType t = sv.GetXType;

            while ( true )
            {
                Object obj = ReadValue( intValidator, true );
                if ( obj == NONE )
                    break;

                int? id = ( int? ) obj;
                Field key = t.GetField( id );

                Validator v = t.GetValidator( key );
                Object value = ReadValue( v );

                sv.Add( key, value );
            }
        }

        private void ReadValues( ArrayValue av, Validator v )
        {
            Validator ev = v.ElementValidator();
            while ( true )
            {
                Object value = ReadValue( ev, true );
                if ( value == NONE )
                    break;

                av.Add( value );
            }
        }

        ////////////////////////
        // Main input methods //
        ////////////////////////

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        /// Exception:
        ///     throws Exception
        private Message StartMessage()
        {
            sbyte version = ( sbyte ) buf.GetByte();
			
			if (version != VERSION)
				throw new IOException(
					"binary tagged data version mismatch: got "+version+" expected "+VERSION );

            XType t = ReadType();
            int length = ReadIntegerValue().Value;

      //      return new Message( ReadType(), vf );
            return new Message(t,length, vf);
        }

        private void EndMessage( Message msg )
        {
            // nothing to do., readKeysAndValues swallowed the NONE.
        }

        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        /// Exception:
        ///     throws Exception
        private StructValue StartStruct() 
	    {
            XType t = ReadType();
            int length = ReadIntegerValue().Value;
            return new StructValue( t,length);
	    }

        private void EndStruct( StructValue sv )
        {
            // nothing to do, readKeysAndValues swallowed the NONE.
        }

        private ArrayValue StartArray()
        {
            sbyte type = ( sbyte ) buf.GetByte();

            XType customStructType;
            if(type == TypeCode.CUSTOM || type == TypeCode.STRUCT)
                customStructType = ReadType();
            else
                customStructType = null;

      //      int dim = ( int ) ReadValue( intValidator );
            int dim = ReadIntegerValue().Value;

            int length = ReadIntegerValue().Value;

            Object array = AllocateArrayValue( type, customStructType, dim, length );

            return new ArrayValue( array, type, customStructType, dim );
        }

        public void EndArray( ArrayValue array )
        {
            array.Compact();
        }

        private XType ReadType()
        {
       //     int? id = ( int? ) ReadValue( intValidator );
            int? id = ReadIntegerValue();
            XType t = vf.GetType(id);
            if (t == null)
                throw new ArgumentException("unknown type " + id);
            return t;
        }

        private int? ReadIntegerValue() 
	    {
		//    return (int?)ReadValue( intValidator );
           return Convert.ToInt32(ReadValue(intValidator));
	    }

        private readonly Validator intValidator = Validator_int.Get( 0 );

        ///////////////////////////
        // LOCAL UTILITY METHODS //
        ///////////////////////////

        private Object ValidateValue( Validator v, Boolean noneOk, Object value )
        {
            if ( v == null )
                throw new Exception( "No validator" );

            if ( value == null )
                return null;

            if (noneOk && value == NONE)
                return value;

            return v.ValidateValue(value);
        }

        private Object ReadValue( Validator v )
        {
            return ReadValue( v, false );
        }

        private Object ReadValue( Validator v, Boolean noneOk )
        {
            sbyte type = ( sbyte ) buf.GetByte();

            switch ( type )
            {
                case TypeCode.NULL:
                    return ValidateValue( v, false, null );

                case TypeCode.NONE:
                    return ValidateValue(v, noneOk, NONE);

                case TypeCode.BOOLEAN_FALSE:
                    return ValidateValue( v, false, false );

                case TypeCode.BOOLEAN_TRUE:
                    return ValidateValue( v, false, true );

                case TypeCode.BYTE:
                    return  ValidateValue( v, false, buf.GetByte() );

                case TypeCode.SHORT:
                    return  ValidateValue( v, false,  buf.GetShort() );

                case TypeCode.INT:
                    return  ValidateValue( v, false, buf.GetInt() );

              

                case TypeCode.LONG:
                    return ValidateValue( v, false, buf.GetLong() );

                case TypeCode.FLOAT:
                    return ValidateValue( v, false, buf.GetFloat() );

                case TypeCode.DOUBLE:
                    return ValidateValue( v, false, buf.GetDouble() );

                case TypeCode.BYTES:
                    return ValidateValue( v, false, ReadSbytes() );

                case TypeCode.EMPTY_STRING:
                    return ValidateValue( v, false, "" );

                case TypeCode.STRING:
                    //return new String( sArray, 0, sArray.Length, Encoding.UTF8 );
                    //return Encoding.UTF8.GetString( ReadBytes() );
                    return ValidateValue( v, false, Encoding.UTF8.GetString( ReadBytes() ) );

                //case TypeCode.BOOLS:
                //case TypeCode.SHORTS:
                //case TypeCode.INTS:
                //case TypeCode.LONGS:
                //case TypeCode.FLOATS:
                //case TypeCode.DOUBLES:
                case TypeCode.ARRAY:
                    return ValidateValue( v, false, FromArrayValue( ReadArray( v ) ) );

                case TypeCode.STRUCT:
                case TypeCode.CUSTOM:
                    return ValidateValue( v, false, vf.ImportCustomValue( ReadStruct() ) );

                default:
                    if (type >= TypeCode.MIN_TINY_INT && type <= TypeCode.MAX_TINY_INT)
                        return ValidateValue(v, false, type);
                    throw new Exception( "unsupported type code "+type );

            }

        }



        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        /// Exception:
        ///     throws Exception
        private byte[] ReadBytes()
        {
            int n = ReadIntegerValue().Value;
            byte[] b = new byte[ n ];
            buf.GetFully( b );
            return b;
        }

        /// <summary>
        /// Reads a Message from the data buffer.
        /// </summary>
        /// <param name="vf">the value factory.</param>
        /// <param name="buf">the data buffer.</param>
        /// <returns>the message read.</returns>
        /// Exception:
        ///     throws Exception
        public static Message ReadMessage( ValueFactory vf, byte[] buf )
        {
            return ReadMessage( vf, new FlexBuffer( buf ));
        }


        /// <summary>
        /// Reads a Message from the data buffer.
        /// </summary>
        /// <param name="vf">the value factory for the message</param>
        /// <param name="buf">the data buffer</param>
        /// <param name="off">the offset to start of message</param>
        /// <param name="len">the length of the message</param>
        /// <returns>the message read</returns>
        /// Exception:
        ///     throws Exception
        public static Message ReadMessage( ValueFactory vf, byte[] buf, int off,
            int len )
        {
            return ReadMessage( vf, new FlexBuffer( buf, off, len ));
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="vf"></param>
        /// <returns>the message read</returns>
        /// Exception:
        ///     throws Exception
        public static Message ReadMessage( ValueFactory vf, FlexBuffer buf)
        {
            BinaryTaggedDataInput tdi = new BinaryTaggedDataInput( vf);
            return tdi.ReadMessage(buf);
        }


        /// <summary>
        /// 
        /// </summary>
        /// <returns></returns>
        /// Exception:
        ///     throws Exception
        private sbyte[] ReadSbytes()
        {
            int n = ( int ) ReadValue( intValidator );
            sbyte[] b = new sbyte[n];

            buf.GetFully( (byte[])(Array)b );

            
            
        /*   sbyte[] sb = new sbyte[ n ];
            Buffer.BlockCopy( b, 0, sb, 0, Buffer.ByteLength( b ) ); */
    //      sbyte[] sb = (sbyte[])(Array)b;


            return b;
        }

    }
}
