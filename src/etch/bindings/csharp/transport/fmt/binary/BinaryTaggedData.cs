using System;
using System.Text;
using System.Collections.Generic;
using Etch.Msg;

namespace Etch.Transport.Fmt.Binary
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
        public static readonly sbyte VERSION = 1;

        public BinaryTaggedData()
        {
            // do nothing
        }

        /// <summary>
        /// Constructs the BinaryTaggedData.
        /// </summary>
        /// <param name="vf"></param>
        public BinaryTaggedData( ValueFactory vf ) : base (vf)
        {
        }

        public sbyte CheckValue( Object value, Validator v )
        {
            if ( value == null )
                return TypeCode.NULL;

            if ( value == NONE )
                return TypeCode.NONE;

            sbyte type = (sbyte) v.CheckValue( value );

            type = AdjSmallValue( type, value );

            return type;

        }

        private sbyte AdjSmallValue( sbyte type, Object value )
        {
            if ( type == TypeCode.BYTE1 || type == TypeCode.SHORT1 || 
                type == TypeCode.INT1 || type == TypeCode.LONG1 )
            {
                sbyte v = Convert.ToSByte( value );
                if ( v >= TypeCode.MIN_SMALL_INT && v <= TypeCode.MAX_SMALL_INT )
                {
                    if ( v >= 0 )
                    {
                        if ( type == TypeCode.BYTE1 )
                            return ( sbyte ) ( v + TypeCode.POSITIVE_BYTE_BASE );
                        else if ( type == TypeCode.SHORT1 )
                            return ( sbyte ) ( v + TypeCode.POSITIVE_SHORT_BASE );
                        else if ( type == TypeCode.INT1 )
                            return ( sbyte ) ( v + TypeCode.POSITIVE_INT_BASE );
                        else
                            return ( sbyte ) ( v + TypeCode.POSITIVE_LONG_BASE );
                    }
                    if ( type == TypeCode.BYTE1 )
                        return ( sbyte ) ( -( v + 1 ) + TypeCode.NEGATIVE_BYTE_BASE );
                    else if ( type == TypeCode.SHORT1 )
                        return ( sbyte ) ( -( v + 1 ) + TypeCode.NEGATIVE_SHORT_BASE );
                    else if ( type == TypeCode.INT1 )
                        return ( sbyte ) ( -( v + 1 ) + TypeCode.NEGATIVE_INT_BASE );
                    else
                        return ( sbyte ) ( -( v + 1 ) + TypeCode.NEGATIVE_LONG_BASE );
                }
            }
            return type;
        }

        public override sbyte CheckValue( object value )
        {
            if ( value == null )
                return TypeCode.NULL;
            else if ( value == NONE )
                return TypeCode.NONE;

            Type type = value.GetType();

            if ( value is long )
                return CheckLong( ( long ) value, TypeCode.LONG8, TypeCode.LONG_DT );

            else if ( value is Int32 )
                return CheckInteger( ( Int32 ) value, TypeCode.INT4, TypeCode.INT_DT );

            else if ( value is short )
                return CheckShort( ( short ) value, TypeCode.SHORT2, TypeCode.SHORT_DT );

            else if ( value is SByte )
                return CheckByte( ( sbyte ) value, TypeCode.BYTE1, TypeCode.BYTE_DT );

            else if ( value is Double )
                return ( sbyte ) TypeCode.FLOAT8;

            else if ( value is float )
                return ( sbyte ) TypeCode.FLOAT4;

            else if ( value is String )
            {
                String s  = ( String ) value;
                if ( s.Length == 0 )
                    return TypeCode.EMPTY_STRING;

                return TypeCode.STRING;
            }

            else if ( value is Boolean )
            {
                if ( ( Boolean ) value )
                    return TypeCode.BOOLEAN_TRUE;

                return TypeCode.BOOLEAN_FALSE;
            }

            else if ( value is StructValue )
                return TypeCode.STRUCT;

            else if ( value is ArrayValue )
                return TypeCode.ARRAY;

            else if ( value is sbyte[] )
                return TypeCode.BYTES;

            else if ( value is Array )
                return TypeCode.ARRAY;

            return TypeCode.CUSTOM;

        }

        public override sbyte GetNativeTypeCode( Type c )
        {
            if ( c == typeof( Boolean ) )
                return TypeCode.BOOLEAN_TRUE;

            if ( c == typeof( SByte ) )
                return TypeCode.BYTE1;

            if ( c == typeof( short ) )
                return TypeCode.SHORT2;

            if ( c == typeof( int ) )
                return TypeCode.INT4;

            if ( c == typeof( long ) )
                return TypeCode.LONG8;

            if ( c == typeof( float ) )
                return TypeCode.FLOAT4;

            if ( c == typeof( double ) )
                return TypeCode.FLOAT8;

            if ( c == typeof( string ) )
                return TypeCode.STRING;

            if ( c == typeof( Object ) )
                return TypeCode.ANY;

            return TypeCode.CUSTOM;
        }

        public override XType GetCustomStructType( Type c )
        {
            return vf.GetCustomStructType( c );
        }

        public override Type GetCustomType( XType type )
        {
            return vf.GetCustomType( type );
        }

        public override Type GetNativeType( sbyte type )
        {
            switch ( type )
            {
                case TypeCode.BOOLEAN_TRUE:
                case TypeCode.BOOLEAN_FALSE:
                    return typeof( Boolean );

                case TypeCode.BYTE1:
                    return typeof( sbyte );

                case TypeCode.SHORT1:
			    case TypeCode.SHORT2:
                    return typeof( short );

                case TypeCode.INT1:
			    case TypeCode.INT2:
			    case TypeCode.INT4:
                    return typeof( int );

                case TypeCode.LONG1:
			    case TypeCode.LONG2:
			    case TypeCode.LONG4:
			    case TypeCode.LONG8:
                    return typeof( long );

                case TypeCode.FLOAT4:
                    return typeof( float );

                case TypeCode.FLOAT8:
                    return typeof( double );

                case TypeCode.EMPTY_STRING:
			    case TypeCode.STRING:
                    return typeof( string );

                case TypeCode.ANY:
                    return typeof( Object );

                default:
                    throw new Exception( "unsupported native type " + type );
            }
        }

        private static sbyte CheckByte( sbyte v, sbyte is1, sbyte dt )
        {
            if ( v >= TypeCode.MIN_SMALL_INT && v <= TypeCode.MAX_SMALL_INT )
            {
                if ( v >= 0 )
                    return ( sbyte ) ( TypeCode.PSVALUE | ( dt << TypeCode.PDTSHIFT ) | v );

                return ( sbyte ) ( TypeCode.NSVALUE | ( dt << TypeCode.NDTSHIFT ) | -( v+1 ) );
            }    


            return is1;
        }

        private static sbyte CheckShort( short v, sbyte is2, sbyte dt )
        {
            if ( v >= SByte.MinValue&& v <= SByte.MaxValue )
                return CheckByte( ( sbyte ) v, ( sbyte ) ( is2+1 ), dt );

            return is2;
        }

        private static sbyte CheckInteger( int v, sbyte is4, sbyte dt )
        {
            if ( v >= short.MinValue && v <= short.MaxValue )
                return CheckShort( ( short ) v, ( sbyte ) ( is4+1 ), dt );

            return is4;
        }

        private static sbyte CheckLong( long v, sbyte is8, sbyte dt )
        {
            if ( v >= int.MinValue && v <= int.MaxValue )
                return CheckInteger( ( int ) v, ( sbyte ) ( is8+1 ), dt );

            return is8;
        }

    }
}
