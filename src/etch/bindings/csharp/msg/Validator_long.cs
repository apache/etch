using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for long
    /// </summary>
    public class Validator_long : TypeValidator
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="nDims">number of dimensions. 0 for a scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator_long Get( int nDims )
        {
            CheckDims( nDims );

            if ( nDims >= validators.Length )
                return new Validator_long( nDims );

            Validator_long v = validators[ nDims ];

            if ( v == null )
                v = validators[ nDims ] = new Validator_long( nDims );

            return v;
        }

        private static Validator_long[] validators = new Validator_long[ MAX_CACHED ];

        /// <summary>
        /// Constructs the validator.
        /// </summary>
        /// <param name="nDims"></param>
        private Validator_long( int nDims )
            : base( typeof( long ), typeof( long ), nDims, "long["+nDims+"]" )
        { 
        }

        public override Validator ElementValidator()
        {
            return Get( nDims - 1 );
        }

        public override sbyte? CheckValue( object value )
        {
            if ( !Validate( value ) )
                return null;

            if ( nDims > 0 )
                return Etch.Transport.Fmt.TypeCode.ARRAY;

            long v = ( long ) value;

            if ( v >= SByte.MinValue && v <= SByte.MaxValue )
                return Etch.Transport.Fmt.TypeCode.LONG1;

            if ( v >= short.MinValue && v <= short.MaxValue )
                return Etch.Transport.Fmt.TypeCode.LONG2;

            if ( v >= int.MinValue && v <= int.MaxValue )
                return Etch.Transport.Fmt.TypeCode.LONG4;

            return Etch.Transport.Fmt.TypeCode.LONG8;
        }
    }
}
