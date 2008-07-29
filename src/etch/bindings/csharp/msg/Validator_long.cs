using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for long
    /// </summary>
    public class Validator_long : TypeValidator
    {
        /// <summary></summary>
        /// <param name="nDims">number of dimensions. 0 for a scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator_long Get( int nDims )
        {
            CheckDims( nDims );

            if ( nDims >= validators.Length )
                return new Validator_long( nDims );

            Validator_long v = validators[ nDims ];

            if ( v == null )
                v = validators[ nDims ] = new Validator_long( nDims );

            return v;
        }

        private static Validator_long[] validators = new Validator_long[ MAX_CACHED ];

        /// <summary>
        /// Constructs the validator.
        /// </summary>
        /// <param name="nDims"></param>
        private Validator_long( int nDims )
            : base( typeof( long ), typeof( long ), nDims, "long["+nDims+"]" )
        { 
        }

        public override Validator ElementValidator()
        {
            return Get( nDims - 1 );
        }

        
	    public override bool Validate( object value )
	    {
		    if (nDims > 0)
			    return base.Validate( value );
		
		    if (value == null)
			    return false;
		
		    Type type = value.GetType();
		
		    return type == typeof(long) || type == typeof(Int32) ||
			type == typeof(short) || type == typeof(sbyte);
	    }
	
	   
	    public override object ValidateValue( object value )
	    {
		    value = base.ValidateValue( value );
		
		    if (value.GetType() == typeof(long) || nDims > 0)
			    return value;

            return Convert.ToInt64(value);
	    }

      
    }
}
