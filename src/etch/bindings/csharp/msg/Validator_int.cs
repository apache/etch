using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for int
    /// </summary>
    public class Validator_int : TypeValidator
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="nDims">number of dimensions. 0 for a scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator_int Get( int nDims )
        {
            CheckDims( nDims );

            if ( nDims >= validators.Length )
                return new Validator_int( nDims );

            Validator_int v = validators[ nDims ];

            if ( v == null )
                v = validators[ nDims ] = new Validator_int( nDims );

            return v;
        }

        private static Validator_int[] validators = new Validator_int[ MAX_CACHED ];

        /// <summary>
        /// Constructs the validator.
        /// </summary>
        /// <param name="nDims"></param>
        private Validator_int( int nDims )
            : base( typeof( int ), typeof( int ), nDims, "int["+nDims+"]" )
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

            int v = ( Int32 ) value;

            if ( v >= SByte.MinValue && v <= SByte.MaxValue )
                return Etch.Transport.Fmt.TypeCode.INT1;

            if ( v >= short.MinValue && v <= short.MaxValue )
                return Etch.Transport.Fmt.TypeCode.INT2;

            return Etch.Transport.Fmt.TypeCode.INT4;
        }
    }
}
