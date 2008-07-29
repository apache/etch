package etch.bindings.java.msg;

import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Validator for boolean.
 */
public class Validator_boolean extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_boolean get( int nDims )
    {
		if (nDims < 0)
			return null;
		
		if (nDims >= validators.length)
			return new Validator_boolean( nDims );
		
		if (validators[nDims] == null)
			validators[nDims] = new Validator_boolean( nDims );
		
		return validators[nDims];
    }
	
	private static Validator_boolean[] validators = new Validator_boolean[4];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_boolean( int nDims )
	{
		super( Boolean.class, boolean.class, nDims, "boolean["+nDims+"]" );
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
		
		Boolean v = (Boolean) value;
		
		return v ? TypeCode.BOOLEAN_TRUE : TypeCode.BOOLEAN_FALSE;
	}
}
