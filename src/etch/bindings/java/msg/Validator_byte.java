package etch.bindings.java.msg;

import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Validator for byte.
 */
public class Validator_byte extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_byte get( int nDims )
    {
		if (nDims < 0)
			return null;
		
		if (nDims >= validators.length)
			return new Validator_byte( nDims );
		
		if (validators[nDims] == null)
			validators[nDims] = new Validator_byte( nDims );
		
		return validators[nDims];
    }
	
	private static Validator_byte[] validators = new Validator_byte[4];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_byte( int nDims )
	{
		super( Byte.class, byte.class, nDims, "byte["+nDims+"]" );
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
			return nDims == 1 ? TypeCode.BYTES : TypeCode.ARRAY;
		
		return TypeCode.BYTE1;
	}
}
