package etch.bindings.java.msg;

import etch.bindings.java.transport.fmt.TypeCode;

/**
 * Validator for string.
 */
public class Validator_string extends TypeValidator
{
	/**
	 * @param nDims the number of dimensions. 0 for a scalar.
	 * @return an instance of the validator.
	 */
	public static Validator_string get( int nDims )
    {
		if (nDims < 0)
			return null;
		
		if (nDims >= validators.length)
			return new Validator_string( nDims );
		
		if (validators[nDims] == null)
			validators[nDims] = new Validator_string( nDims );
		
		return validators[nDims];
    }
	
	private static Validator_string[] validators = new Validator_string[4];
	
	/**
	 * Constructs the Validator.
	 *
	 * @param nDims the number of dimensions. 0 for a scalar.
	 */
	private Validator_string( int nDims )
	{
		super( String.class, String.class, nDims, "string["+nDims+"]" );
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
		
		String v = (String) value;
		
		return v.length() > 0 ? TypeCode.STRING : TypeCode.EMPTY_STRING;
	}
}
