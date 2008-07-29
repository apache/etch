package etch.bindings.java.msg;

import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Validator for double.
 */
public class Validator_double extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_double get( int nDims )
    {
		if (nDims < 0)
			return null;
		
		if (nDims >= validators.length)
			return new Validator_double( nDims );
		
		if (validators[nDims] == null)
			validators[nDims] = new Validator_double( nDims );
		
		return validators[nDims];
    }
	
	private static Validator_double[] validators = new Validator_double[4];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_double( int nDims )
	{
		super( Double.class, double.class, nDims, "double["+nDims+"]" );
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
		
		return TypeCode.FLOAT8;
	}
}
