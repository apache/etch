using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for structvalue
    /// </summary>
    public class Validator_StructValue : Validator
    {
        public static Validator_StructValue Get()
        {
            if ( validator == null )
                validator = new Validator_StructValue();

            return validator;
        }

        private static Validator_StructValue validator;

        private Validator_StructValue()
            //: base( typeof( StructValue ), typeof( StructValue ), 0, "StructValue" )
        { 
            // nothing to do
        }

        public override Validator ElementValidator()
        {
            throw new ArgumentException( "not an array" );
        }

        public override bool Validate( object value )
        {
            return value.GetType() == typeof( StructValue );
        }

        public override sbyte? CheckValue( object value )
        {
            if ( !Validate( value ) )
                return null;

            return Etch.Transport.Fmt.TypeCode.STRUCT;
        }
    }
}
