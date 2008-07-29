using System;
using System.Collections.Generic;
using System.Text;
using Etch.Transport;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for ArrayValue
    /// </summary>
    public class Validator_ArrayValue : TypeValidator
    {
        public static Validator_ArrayValue Get()
        {
            if ( validator == null )
                validator = new Validator_ArrayValue();

            return validator;
        }

        private static Validator_ArrayValue validator;

        private Validator_ArrayValue() : base( typeof( ArrayValue ), typeof( ArrayValue ), 0, "ArrayValue" )
        {
        }

        public override Validator ElementValidator()
        {
            return null;
        }      
    }
}
