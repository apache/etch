using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for Boolean
    /// </summary>
    public class Validator_boolean : TypeValidator
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="nDims">number of dimensions. 0 for a scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator_boolean Get( int nDims )
        {
            CheckDims( nDims );

            if ( nDims >= validators.Length )
                return new Validator_boolean( nDims );

            Validator_boolean v = validators[ nDims ];

            if ( v == null )
                v = validators[ nDims ] = new Validator_boolean( nDims );

            return v;
        }

        private static Validator_boolean[] validators = new Validator_boolean[ MAX_CACHED ];

        /// <summary>
        /// Constructs the validator.
        /// </summary>
        /// <param name="nDims"></param>
        private Validator_boolean( int nDims )
            : base( typeof( bool ), typeof( bool ), nDims, "boolean["+nDims+"]" )
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

            Boolean v = ( Boolean ) value;
            return v ? Etch.Transport.Fmt.TypeCode.BOOLEAN_TRUE : Etch.Transport.Fmt.TypeCode.BOOLEAN_FALSE;
        }
    }
}
