package etch.bindings.java.msg;

import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Validator for int.
 */
public class Validator_int extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_int get( int nDims )
	{
		if (nDims < 0)
			return null;
		
		if (nDims >= validators.length)
			return new Validator_int( nDims );
		
		if (validators[nDims] == null)
			validators[nDims] = new Validator_int( nDims );
		
		return validators[nDims];
	}
	
	private static Validator_int[] validators = new Validator_int[4];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_int( int nDims )
	{
		super( Integer.class, int.class, nDims, "int["+nDims+"]" );
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
		
		int v = (Integer) value;
		
		if (v >= Byte.MIN_VALUE && v <= Byte.MAX_VALUE)
			return TypeCode.INT1;
		
		if (v >= Short.MIN_VALUE && v <= Short.MAX_VALUE)
			return TypeCode.INT2;

		return TypeCode.INT4;
	}
}
