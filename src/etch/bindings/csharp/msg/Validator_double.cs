using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for double
    /// </summary>
    public class Validator_double : TypeValidator
    {
        /// <summary></summary>
        /// <param name="nDims">number of dimensions. 0 for a scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator_double Get( int nDims )
        {
            CheckDims( nDims );

            if ( nDims >= validators.Length )
                return new Validator_double( nDims );

            Validator_double v = validators[ nDims ];

            if ( v == null )
                v = validators[ nDims ] = new Validator_double( nDims );

            return v;
        }

        private static Validator_double[] validators = new Validator_double[ MAX_CACHED ];

        /// <summary>
        /// Constructs the validator.
        /// </summary>
        /// <param name="nDims"></param>
        private Validator_double( int nDims )
            : base( typeof( Double ), typeof( double ), nDims, "double["+nDims+"]" )
        { 
        }

        public override Validator ElementValidator()
        {
            return Get( nDims - 1 );
        }
       
    }
}
