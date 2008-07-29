using System;
using System.Collections.Generic;
using System.Text;

namespace Etch.Msg
{
    /// <summary>
    /// Validator for byte
    /// </summary>
    public class Validator_byte : TypeValidator
    {
        /// <summary>
        /// 
        /// </summary>
        /// <param name="nDims">number of dimensions. 0 for scalar.</param>
        /// <returns>an instance of the validator</returns>
        public static Validator_byte Get( int nDims )
        {
            CheckDims( nDims );

            if ( nDims >= validators.Length )
                return new Validator_byte( nDims );

            Validator_byte v = validators[ nDims ];

            if ( v == null )
                v = validators[ nDims ] = new Validator_byte( nDims );

            return v;
        }

        private static Validator_byte[] validators = new Validator_byte[ MAX_CACHED ];

        /// <summary>
        /// Constructs the validator.
        /// </summary>
        /// <param name="nDims"></param>
        private Validator_byte( int nDims )
            : base( typeof( sbyte ), typeof( sbyte ), nDims, "byte["+nDims+"]" )
        {
        }

        public override Validator ElementValidator()
        {
            return Get( nDims - 1 );
        }
      	    
	    public override bool Validate( object value )
	    {
		    if (nDims > 0)
			   return base.Validate(value);
		
		    if (value == null)
			    return false;

            if (value.GetType() == typeof(sbyte))
			    return true;
		
		    if (value.GetType() == typeof(short))
		    {
			    short v = (short) value;
			    return v >= SByte.MinValue && v <= SByte.MaxValue;
               
		    }
		
		   if (value.GetType() == typeof(Int32))
		    {
			    int v = (Int32) value;
			    return v >= SByte.MinValue && v <= SByte.MaxValue;
		    }
		
		   if (value.GetType() == typeof(long))
		   {
			    long v = (long) value;
			    return v >= SByte.MinValue && v <= SByte.MaxValue;
		    }
		
		    return false;
	    }
		
	    public override object ValidateValue( object value )
	    {
		    value = base.ValidateValue(value);
		
		    if ((value.GetType() == typeof(sbyte)) || nDims > 0)
			    return value;

            return Convert.ToSByte(value);
	    }      
    }
}
