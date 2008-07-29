using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    public class Validator_object : Validator
    {
        public override sbyte? CheckValue( object value )
        {
            return Etch.Transport.Fmt.TypeCode.ANY;
        }

        public override Validator ElementValidator()
        {
            return this;
        }

        public override bool Validate( object value )
        {
            return true;
        }

        public static Validator Get( int nDims )
        {
            if ( inst == null )
                inst = new Validator_object();
            return inst;
        }

        private static Validator_object inst;
    }
}
