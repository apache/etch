package etch.bindings.java.msg;

import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Validator for short.
 */
public class Validator_short extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_short get( int nDims )
    {
		if (nDims < 0)
			return null;
		
		if (nDims >= validators.length)
			return new Validator_short( nDims );
		
		if (validators[nDims] == null)
			validators[nDims] = new Validator_short( nDims );
		
		return validators[nDims];
    }
	
	private static Validator_short[] validators = new Validator_short[4];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_short( int nDims )
	{
		super( Short.class, short.class, nDims, "short["+nDims+"]" );
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
		
		short v = (Short) value;
		
		if (v >= Byte.MIN_VALUE && v <= Byte.MAX_VALUE)
			return TypeCode.SHORT1;
		
		return TypeCode.SHORT2;
	}
}
