using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    public class Validator_float : TypeValidator
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="nDims">number of dimensions. 0 for a scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator_float Get( int nDims )
        {
            CheckDims( nDims );

            if ( nDims >= validators.Length )
                return new Validator_float( nDims );

            Validator_float v = validators[ nDims ];

            if ( v == null )
                v = validators[ nDims ] = new Validator_float( nDims );

            return v;
        }

        private static Validator_float[] validators = new Validator_float[ MAX_CACHED ];

        /// <summary>
        /// Constructs the validator.
        /// </summary>
        /// <param name="nDims"></param>
        private Validator_float( int nDims )
            : base( typeof( float ), typeof( float ), nDims, "float["+nDims+"]" )
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

            return Etch.Transport.Fmt.TypeCode.FLOAT4;
        }
    }
}
