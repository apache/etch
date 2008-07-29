package etch.bindings.java.msg;

import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Validator for long.
 */
public class Validator_long extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_long get( int nDims )
    {
		if (nDims < 0)
			return null;
		
		if (nDims >= validators.length)
			return new Validator_long( nDims );
		
		if (validators[nDims] == null)
			validators[nDims] = new Validator_long( nDims );
		
		return validators[nDims];
    }
	
	private static Validator_long[] validators = new Validator_long[4];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_long( int nDims )
	{
		super( Long.class, long.class, nDims, "long["+nDims+"]" );
	}

	@Override
    public Validator elementValidator()
    {
	    return get( nDims-1 );
    }

	@Override
	public Byte checkValue( Object value )
	{
		if (!validate( value))
			return null;
		
		if (nDims > 0)
			return TypeCode.ARRAY;
		
		long v = (Long) value;
		
		if (v >= Byte.MIN_VALUE && v <= Byte.MAX_VALUE)
			return TypeCode.LONG1;
		
		if (v >= Short.MIN_VALUE && v <= Short.MAX_VALUE)
			return TypeCode.LONG2;
		
		if (v >= Integer.MIN_VALUE && v <= Integer.MAX_VALUE)
			return TypeCode.LONG4;

		return TypeCode.LONG8;
	}
}
