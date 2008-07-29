using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for short
    /// </summary>
    public class Validator_short : TypeValidator
    {
        /// <summary></summary>
        /// <param name="nDims">number of dimensions. 0 for a scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator_short Get( int nDims )
        {
            CheckDims( nDims );

            if ( nDims >= validators.Length )
                return new Validator_short( nDims );

            Validator_short v = validators[ nDims ];

            if ( v == null )
                v = validators[ nDims ] = new Validator_short( nDims );

            return v;
        }

        private static Validator_short[] validators = new Validator_short[ MAX_CACHED ];

        /// <summary>
        /// Constructs the validator.
        /// </summary>
        /// <param name="nDims"></param>
        private Validator_short( int nDims )
            : base( typeof( short ), typeof( short ), nDims, "short["+nDims+"]" )
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
    		
		    if ( type == typeof(short) || type == typeof(sbyte))
			    return true;
    		
		    if (type == typeof(Int32))
		    {
			    int v = (Int32) value;
			    return v >= short.MinValue&& v <= short.MaxValue;
		    }
    		
		    if (type == typeof(long))
		    {
			    long v = (long) value;
			    return v >= short.MinValue && v <= short.MaxValue;
		    }
    		
		    return false;
	    }
	
	
	    public override object ValidateValue( object value )
	    {
		    value = base.ValidateValue( value );
    		
		    if (value.GetType() == typeof(short) || nDims > 0)
			    return value;

            return Convert.ToInt16(value);
	    }

       
    }
}
