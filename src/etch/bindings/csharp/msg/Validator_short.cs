using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for short
    /// </summary>
    public class Validator_short : TypeValidator
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="nDims">number of dimensions. 0 for a scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator_short Get( int nDims )
        {
            CheckDims( nDims );

            if ( nDims >= validators.Length )
                return new Validator_short( nDims );

            Validator_short v = validators[ nDims ];

            if ( v == null )
                v = validators[ nDims ] = new Validator_short( nDims );

            return v;
        }

        private static Validator_short[] validators = new Validator_short[ MAX_CACHED ];

        /// <summary>
        /// Constructs the validator.
        /// </summary>
        /// <param name="nDims"></param>
        private Validator_short( int nDims )
            : base( typeof( short ), typeof( short ), nDims, "short["+nDims+"]" )
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

            short v = ( short ) value;

            if ( v >= SByte.MinValue && v <= SByte.MaxValue )
                return Etch.Transport.Fmt.TypeCode.SHORT1;

            return Etch.Transport.Fmt.TypeCode.SHORT2;
        }
    }
}
