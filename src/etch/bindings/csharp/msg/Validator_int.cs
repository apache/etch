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
        /// <summary></summary>
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

       
	    public override bool Validate( object value )
	    {
		    if (nDims > 0)
			return base.Validate( value );
		
		    if (value == null)
			    return false;
		
		    Type type = value.GetType();
		
		    if ( type == typeof(Int32) || type == typeof(Int16) || type == typeof(sbyte) )
			    return true;
		
		    if (type == typeof(long))
		    {
			    long v = (long) value;
			    return v >= Int32.MinValue && v <= Int32.MaxValue;
		    }
		
		    return false;
	    }
	
	
	    public override object  ValidateValue( object value )
	    {
		    value = base.ValidateValue( value );
		
		    if ( value.GetType() == typeof(Int32) || nDims > 0)
			 return value;
			   
            return Convert.ToInt32(value);
    	}
       
    }
}
