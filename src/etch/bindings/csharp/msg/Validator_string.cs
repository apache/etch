using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for strings
    /// </summary>
    public class Validator_string : TypeValidator
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="nDims">number of dimensions. 0 for a scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator_string Get( int nDims )
        {
            CheckDims( nDims );

            if ( nDims >= validators.Length )
                return new Validator_string( nDims );

            Validator_string v = validators[ nDims ];

            if ( v == null )
                v = validators[ nDims ] = new Validator_string( nDims );

            return v;
        }

        private static Validator_string[] validators = new Validator_string[ MAX_CACHED ];

        /// <summary>
        /// Constructs the validator.
        /// </summary>
        /// <param name="nDims"></param>
        private Validator_string( int nDims )
            : base( typeof( string ), typeof( string ), nDims, "string["+nDims+"]" )
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

            string v = ( string ) value;

            return v.Length > 0 ? Etch.Transport.Fmt.TypeCode.STRING : Etch.Transport.Fmt.TypeCode.EMPTY_STRING;
        }
    }
}
