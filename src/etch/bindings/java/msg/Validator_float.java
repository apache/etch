package etch.bindings.java.msg;

import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Validator for float.
 */
public class Validator_float extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_float get( int nDims )
    {
		if (nDims < 0)
			return null;
		
		if (nDims >= validators.length)
			return new Validator_float( nDims );
		
		if (validators[nDims] == null)
			validators[nDims] = new Validator_float( nDims );
		
		return validators[nDims];
    }
	
	private static Validator_float[] validators = new Validator_float[4];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_float( int nDims )
	{
		super( Float.class, float.class, nDims, "float["+nDims+"]" );
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
		
		return TypeCode.FLOAT4;
	}
}
