using System;
using System.Collections.Generic;
using System.Text;
using Etch.Support;
using Etch.Msg;

namespace Etch.Transport.Fmt.Binary
{
    /// <summary>
    /// A validator for the special end of list marker,
    /// </summary>
    public class Validator_none : Validator
    {
        public static Validator_none Get()
        {
            if ( validator == null )
                validator = new Validator_none();
            return validator;
        }

        private static Validator_none validator;

        /// <summary>
        /// constructs the validator
        /// </summary>
        public Validator_none()
        {
            //nothing to do
        }

        public override Validator ElementValidator()
        {
        
            throw new ArgumentException("not an array");
        }

        public override bool Validate( Object value )
        {
            return value == BinaryTaggedData.NONE;
        }

      

        public override object ValidateValue(object value)
        {
            if (Validate(value))
                return value;

            throw new Exception("value not appropriate for none: " + value);
        }
    }
}
