using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for Runtime Exceptions
    /// </summary>
    public class Validator_RuntimeException : Validator
    {
        /// <summary></summary>
        /// <returns>an instance of the validator</returns>
        public static Validator_RuntimeException Get()
        {
            if ( validator == null )
                validator = new Validator_RuntimeException();

            return validator;
        }

        private static Validator_RuntimeException validator;

        public Validator_RuntimeException()
        {
            // nothing to do
        }

        public override Validator ElementValidator()
        {
            throw new Exception( "not an array" );
        }

        public override bool Validate( object value )
        {
            return ( value is Exception );
        }

        public override object ValidateValue(object value)
        {
            if (Validate(value))
                return value;

            throw new Exception("value not appropriate for RuntimeException: " + value);
        }      
    }
}
