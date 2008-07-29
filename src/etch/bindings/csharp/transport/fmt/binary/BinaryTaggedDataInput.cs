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
        /// <param name="buf">the input buffer</param>
        public BinaryTaggedDataInput(ValueFactory vf, FlexBuffer buf) : base(vf)
        {
            
            this.buf = buf;
        }

        /// <summary>
        /// Constructs the BinaryTaggedDataInput with a null buffer.
        /// </summary>
        /// <param name="vf">the value factory for the service.</param>
        /// <param name="uri">the uri used to construct the transport stack.</param>
        public BinaryTaggedDataInput( ValueFactory vf, URL uri ) : base( vf )
        {}

        private FlexBuffer buf;

        /////////////////////////////
        // TaggedDataInput methods //
        /////////////////////////////

        public void SetBuffer( FlexBuffer buf )
	    {
            this.buf = buf;
	    }


        public Message ReadMessage()
        {
            Message msg = StartMessage();
            ReadKeysAndValues( msg );
            EndMessage( msg );
            return msg;
        }

        public StructValue ReadStruct()
        {
            StructValue sv = StartStruct();
            ReadKeysAndValues( sv );
            EndStruct( sv );
            return sv;

        }

        public ArrayValue ReadArray( Validator v )
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
                Field key = vf.GetField( id );

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

            return new Message( ReadType(), vf );
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
            return new StructValue( ReadType() );
	    }

        private void EndStruct( StructValue sv )
        {
            // nothing to do, readKeysAndValues swallowed the NONE.
        }

        private ArrayValue StartArray()
        {
            sbyte type = ( sbyte ) buf.GetByte();

            XType customStructType;
            if ( type == TypeCode.CUSTOM )
                customStructType = ReadType();
            else
                customStructType = null;

            int dim = ( int ) ReadValue( intValidator );

            int length = 8;

            Object array = AllocateArrayValue( type, customStructType, dim, length );

            return new ArrayValue( array, type, customStructType, dim );
        }

        public void EndArray( ArrayValue array )
        {
            array.Compact();
        }

        private XType ReadType()
        {
            int? id = ( int? ) ReadValue( intValidator );
            return vf.GetType( id );
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

            if ( value == NONE && noneOk )
                return value;

            if ( !v.Validate( value ) )
                throw new Exception( "validator " + v + " failed for value " + value );

            return value;
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
                    return ValidateValue( v, noneOk, null );

                case TypeCode.BOOLEAN_FALSE:
                    return ValidateValue( v, noneOk, false );

                case TypeCode.BOOLEAN_TRUE:
                    return ValidateValue( v, noneOk, true );

                case TypeCode.BYTE1:
                    return ( sbyte ) ValidateValue( v, noneOk, buf.GetByte() );

                case TypeCode.SHORT1:
                    return ( short ) ValidateValue( v, noneOk, ( short ) buf.GetByte() );

                case TypeCode.INT1:
                    return ( int ) ValidateValue( v, noneOk, ( int ) buf.GetByte() );

                case TypeCode.LONG1:
                    return ValidateValue( v, noneOk, ( long ) buf.GetByte() );

                case TypeCode.SHORT2:
                    return ValidateValue( v, noneOk, buf.GetShort() );

                case TypeCode.INT2:
                    return ( int ) ValidateValue( v, noneOk, ( int ) buf.GetShort() );

                case TypeCode.LONG2:
                    return ( long ) ValidateValue( v, noneOk, ( long ) buf.GetShort() );

                case TypeCode.INT4:
                    return ValidateValue( v, noneOk, buf.GetInt() );

                case TypeCode.LONG4:
                    return ( long ) ValidateValue( v, noneOk, ( long ) buf.GetInt() );

                case TypeCode.LONG8:
                    return ValidateValue( v, noneOk, buf.GetLong() );

                case TypeCode.FLOAT4:
                    return ValidateValue( v, noneOk, buf.GetFloat() );

                case TypeCode.FLOAT8:
                    return ValidateValue( v, noneOk, buf.GetDouble() );

                case TypeCode.BYTES:
                    return ValidateValue( v, noneOk, ReadSbytes() );

                case TypeCode.EMPTY_STRING:
                    return ValidateValue( v, noneOk, "" );

                case TypeCode.STRING:
                    //return new String( sArray, 0, sArray.Length, Encoding.UTF8 );
                    //return Encoding.UTF8.GetString( ReadBytes() );
                    return ValidateValue( v, noneOk, Encoding.UTF8.GetString( ReadBytes() ) );

                case TypeCode.ARRAY:
                    return ValidateValue( v, noneOk, FromArrayValue( ReadArray( v ) ) );

                case TypeCode.STRUCT:
                    return ValidateValue( v, noneOk, ReadStruct() );

                case TypeCode.CUSTOM:
                    return ValidateValue( v, noneOk, vf.ImportCustomValue( ReadStruct() ) );

                case TypeCode.NONE:
                    return ValidateValue( v, noneOk, NONE );

                default:
                    if ( ( type & TypeCode.PSMASK ) == TypeCode.PSVALUE )
                    {
                        // positive small integers
                        int value = type & TypeCode.PVMASK;
                        int dt = ( type >> TypeCode.PDTSHIFT ) & TypeCode.DTMASK;
                        if ( dt == TypeCode.BYTE_DT )
                            return ValidateValue( v, noneOk, ( sbyte ) value );
                        if ( dt == TypeCode.SHORT_DT )
                            return ValidateValue( v, noneOk, ( short ) value );
                        if ( dt == TypeCode.INT_DT )
                            return ValidateValue( v, noneOk, value );
                        return ValidateValue( v, noneOk, ( long ) value );
                    }
                    if ( ( type & TypeCode.NSMASK ) == TypeCode.NSVALUE )
                    {
                        // negative small integers
                        int value = -( type & TypeCode.NVMASK )-1;
                        int dt = ( type >> TypeCode.NDTSHIFT ) & TypeCode.DTMASK;
                        //System.out.printf( "type %x value %d dt %d\n", type, value, dt );
                        if ( dt == TypeCode.BYTE_DT )
                            return ValidateValue( v, noneOk, ( sbyte ) value );
                        if ( dt == TypeCode.SHORT_DT )
                            return ValidateValue( v, noneOk, ( short ) value );
                        if ( dt == TypeCode.INT_DT )
                            return ValidateValue( v, noneOk, value );
                        return ValidateValue( v, noneOk, ( long ) value );
                    }
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
            int n = ( int ) ReadValue( intValidator );
            byte[] b = new byte[ n ];
            buf.GetFully( b );
            return b;
        }

        /// <summary>
        /// Reads a Message from the data buffer.
        /// </summary>
        /// <param name="vf">the value factory.</param>
        /// <param name="buf">the data buffer.</param>
        /// <param name="ms">the message source</param>
        /// <returns>the message read.</returns>
        /// Exception:
        ///     throws Exception
        public static Message ReadMessage( ValueFactory vf, byte[] buf, MessageSource ms )
        {
            return ReadMessage( vf, new FlexBuffer( buf ), ms );
        }


        /// <summary>
        /// Reads a Message from the data buffer.
        /// </summary>
        /// <param name="vf">the value factory for the message</param>
        /// <param name="buf">the data buffer</param>
        /// <param name="off">the offset to start of message</param>
        /// <param name="len">the length of the message</param>
        /// <param name="ms">the message source</param>
        /// <returns>the message read</returns>
        /// Exception:
        ///     throws Exception
        public static Message ReadMessage( ValueFactory vf, byte[] buf, int off,
            int len, MessageSource ms )
        {
            return ReadMessage( vf, new FlexBuffer( buf, off, len ), ms );
        }

        /// <summary>
        /// 
        /// </summary>
        /// <param name="vf"></param>
        /// <param name="iStream"></param>
        /// <param name="ms"></param>
        /// <returns>the message read</returns>
        /// Exception:
        ///     throws Exception
        public static Message ReadMessage( ValueFactory vf, FlexBuffer buf, MessageSource ms )
        {
            BinaryTaggedDataInput tdi = new BinaryTaggedDataInput( vf, buf );
            return tdi.ReadMessage();
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
            byte[] b = new byte[n];

            buf.GetFully( b );
            
            sbyte[] sb = new sbyte[ n ];
            Buffer.BlockCopy( b, 0, sb, 0, Buffer.ByteLength( b ) );

            return sb;
        }

    }
}
