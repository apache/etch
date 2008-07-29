using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for custom type
    /// </summary>
    public class Validator_custom : TypeValidator
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="clss">Class of the custom type</param>
        /// <param name="nDims">number of dimensions. 0 for scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator Get( Type clss, int nDims )
        {
            CheckDims( nDims );

            Validator[] validators = GetValidators( clss );

            if ( nDims >= validators.Length )
                return new Validator_custom( clss, nDims );

            Validator v = validators[ nDims ];

            if ( v == null )
                v = validators[ nDims ] = new Validator_custom( clss, nDims );

            return v;
        }

        private static Validator[] GetValidators( Type clss )
        {
            Validator_custom[] validators;

            try
            {
                validators = validatorsByClass[ clss ];
            }
            catch ( KeyNotFoundException )
            {
                validators = null;
            }

            if ( validators == null )
            {
                validators = new Validator_custom[ MAX_CACHED ];
                validatorsByClass.Add( clss, validators );
            }
            return validators;
        }

        private static Dictionary<Type, Validator_custom[]> validatorsByClass = 
            new Dictionary<Type, Validator_custom[]>();

        /// <summary>
        /// Constructs the validator
        /// </summary>
        /// <param name="clss">the class of the custom type</param>
        /// <param name="nDims">the number of dimensions. 0 for scalar. </param>
        private Validator_custom( Type clss, int nDims )
            : base( clss, clss, nDims, clss.ToString()+"["+nDims+"]" )
        {
            this.clss = clss;
        }

        private Type clss;

        public override Validator ElementValidator()
        {
            return Get( clss, nDims - 1 );
        }

        public override sbyte? CheckValue( object value )
        {
            if ( !Validate( value ) )
                return null;

            if ( nDims > 0 )
                return Etch.Transport.Fmt.TypeCode.ARRAY;

            return Etch.Transport.Fmt.TypeCode.CUSTOM;
        }
    }
}
