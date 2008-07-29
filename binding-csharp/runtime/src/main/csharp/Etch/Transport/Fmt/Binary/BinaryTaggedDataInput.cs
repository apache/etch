// $Id$
// 
// Copyright 2007-2008 Cisco Systems Inc.
// 
// Licensed under the Apache License, Version 2.0 (the "License"); you may not
// use this file except in compliance with the License. You may obtain a copy
// of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS, WITHOUT
// WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied. See the
// License for the specific language governing permissions and limitations
// under the License.

using System;
using System.IO;
using System.Text;
using Etch.Msg;
using Etch.Support;
using Etch.Util;

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
        /// <param name="vf">the value factory for the service.</param>
        /// <param name="uri">the uri used to construct the transport stack.</param>
        public BinaryTaggedDataInput( ValueFactory vf, string uri )
            : base( vf )
        {
            // do nothing.
        }

        /////////////////////////////
        // TaggedDataInput methods //
        /////////////////////////////


        public Message ReadMessage(FlexBuffer buf)
        {
            this.buf = buf;
            
            // lengthBudget is how many array elements total are reasonable to
            // allocate while parsing this message. the largest value comes as each
            // byte of the incoming message turned into an array element. the total
            // will always be lower than this as often it takes multiple bytes to
            // make a single array element. so, each time we make an array, we
            // deduct the specified length from lengthBudget. if the result is
            // negative, then either the incoming message is misformed or someone is
            // trying to spoof us.
            
            lengthBudget = buf.Avail();

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
                lengthBudget = 0;
            }
        }

        private FlexBuffer buf;

        private int lengthBudget;

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

                int id = ( int ) obj;
                Field key = t.GetField( id );
                if (key == null)
                    key = new Field(id, id.ToString());

                Validator v = t.GetValidator( key );
                if (v == null)
                {
                    // read the value but ignore it.
                    ReadValue(Validator_object.Get(0));
                }
                else
                {
                    sv.Add(key, ReadValue(v));
                }
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

        private Message StartMessage()
        {
            sbyte version = ( sbyte ) buf.GetByte();
			
			if (version != VERSION)
				throw new IOException(
					"binary tagged data version mismatch: got "+version+" expected "+VERSION );

            XType t = ReadType();
            int length = ReadLength();

            return new Message(t, vf, length);
        }

        private void EndMessage( Message msg )
        {
            // nothing to do., readKeysAndValues swallowed the NONE.
        }

        private StructValue StartStruct() 
	    {
            XType t = ReadType();
            int length = ReadLength();

            return new StructValue(t, vf, length);
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

            int dim = ReadIntegerValue();
            if (dim <= 0 || dim > Validator.MAX_NDIMS)
                throw new ArgumentException("dim <= 0 || dim > Validator.MAX_NDIMS");

            int length = ReadLength();

            Object array = AllocateArrayValue( type, customStructType, dim, length );

            return new ArrayValue( array, type, customStructType, dim );
        }

        private void EndArray(ArrayValue array)
        {
            array.Compact();
        }

        private XType ReadType()
        {
            int id = ReadIntegerValue();
            XType t = vf.GetType(id);
            if (t == null)
                t = new XType(id, id.ToString());
            return t;
        }

        private int ReadLength()
        {
            int length = ReadIntegerValue();

            if (length < 0 || length > lengthBudget)
                throw new ArgumentException(
                    "length < 0 || length > lengthBudget");
            
            lengthBudget -= length;

            return length;
        }

        private int ReadIntegerValue() 
	    {
            return (int)ReadValue(intValidator);
	    }

        private readonly Validator intValidator = Validator_int.Get( 0 );

        ///////////////////////////
        // LOCAL UTILITY METHODS //
        ///////////////////////////

        private Object ValidateValue(Validator v, Object value)
        {
            // v == null more or less implies that a field is not known
            // for a type. thus we don't care about the field value as
            // we are going to ignore it. therefore, return null.

            if (v == null)
                return null;

            if (value == null)
                return null;

            return v.ValidateValue(value);
        }

        private Object ValidateValue( Validator v, Boolean noneOk, Object value )
        {
            if (noneOk && value == NONE)
                return value;

            return ValidateValue(v, value);
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
                    return ValidateValue( v, null );

                case TypeCode.NONE:
                    return ValidateValue(v, noneOk, NONE);

                case TypeCode.BOOLEAN_FALSE:
                    return ValidateValue( v, false );

                case TypeCode.BOOLEAN_TRUE:
                    return ValidateValue( v, true );

                case TypeCode.BYTE:
                    return  ValidateValue( v, buf.GetByte() );

                case TypeCode.SHORT:
                    return  ValidateValue( v, buf.GetShort() );

                case TypeCode.INT:
                    return  ValidateValue( v, buf.GetInt() );

                case TypeCode.LONG:
                    return ValidateValue( v, buf.GetLong() );

                case TypeCode.FLOAT:
                    return ValidateValue( v, buf.GetFloat() );

                case TypeCode.DOUBLE:
                    return ValidateValue( v, buf.GetDouble() );

                case TypeCode.BYTES:
                    return ValidateValue( v, ReadSbytes() );

                case TypeCode.EMPTY_STRING:
                    return ValidateValue( v, "" );

                case TypeCode.STRING:
                    return ValidateValue( v, Encoding.UTF8.GetString( ReadBytes() ) );

                //case TypeCode.BOOLS:
                //case TypeCode.SHORTS:
                //case TypeCode.INTS:
                //case TypeCode.LONGS:
                //case TypeCode.FLOATS:
                //case TypeCode.DOUBLES:
                case TypeCode.ARRAY:
                    return ValidateValue( v, FromArrayValue( ReadArray( v ) ) );

                case TypeCode.STRUCT:
                case TypeCode.CUSTOM:
                    return ValidateValue( v, vf.ImportCustomValue( ReadStruct() ) );

                default:
                    if (type >= TypeCode.MIN_TINY_INT && type <= TypeCode.MAX_TINY_INT)
                        return ValidateValue(v, type);
                    throw new Exception( "unsupported type code "+type );
            }
        }

        private byte[] ReadBytes()
        {
            int length = ReadLength();
            byte[] b = new byte[ length ];
            buf.GetFully( b );
            return b;
        }

        private sbyte[] ReadSbytes()
        {
            int length = ReadLength();
            sbyte[] b = new sbyte[length];
            buf.GetFully((byte[])(Array)b);
            return b;
        }
    }
}
