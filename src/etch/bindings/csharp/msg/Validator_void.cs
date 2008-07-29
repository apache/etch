using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for void
    /// </summary>
    public class Validator_void : Validator
    {
        /// <summary></summary>
        /// <param name="nDims">the number of dimensions. 0 for scalar</param>
        /// <returns>an instance of the validator</returns>
        public static Validator_void Get( int nDims )
        {
            if ( nDims != 0 )
                throw new ArgumentException( "nDims != 0" );

            if ( validator == null )
                validator = new Validator_void();

            return validator;
        }

        private static Validator_void validator;

        private Validator_void()
        {
            // nothing to do
        }

        public override Validator ElementValidator()
        {
            return null;
        }

        public override bool Validate( object value )
        {
            return value == null;
        }

        public override object ValidateValue(object value)
        {
            if (Validate(value))
                return value;

            throw new Exception("value not appropriate for void: " + value);
        }      
    }
}
