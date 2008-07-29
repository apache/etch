using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validates values using one of two validators
    /// </summary>
    public class ComboValidator : Validator
    {
        /// <summary>
        /// Constructs the combo validator
        /// </summary>
        /// <param name="a"></param>
        /// <param name="b"></param>
        public ComboValidator( Validator a, Validator b )
        {
            this.a = a;
            this.b = b;
        }

        private readonly Validator a;
        private readonly Validator b;

        public override string ToString()
        {
            return String.Format( "({0} OR {1})", a, b );
        }

        public override Validator ElementValidator()
        {
            Validator na;

            try
            {
                na = a.ElementValidator();
            }
            catch ( Exception )
            {
                na = null;
            }

            Validator nb;
            try
            {
                nb = b.ElementValidator();
            }
            catch ( Exception )
            {
                nb = null;
            }

            if ( na == null && nb == null )
                throw new ArgumentException( "na == null and nb == null" );

            if ( na == null )
                return nb;

            if ( nb == null )
                return na;

            return new ComboValidator( na, nb );

        }

        public override bool Validate( object value )
        {
            return a.Validate( value ) || b.Validate( value );
        }

        public override sbyte?  CheckValue(object value)
        {
            sbyte? type = a.CheckValue( value );
            if ( type == null )
                type = b.CheckValue( value );
            return type;
        }
    }
}
