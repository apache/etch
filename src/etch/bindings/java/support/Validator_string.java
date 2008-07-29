package etch.bindings.java.support;

import etch.bindings.java.msg.Validator;

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
		checkDims( nDims );
		
		if (nDims >= validators.length)
			return new Validator_string( nDims );
		
		Validator_string v = validators[nDims];
		
		if (v == null)
			v = validators[nDims] = new Validator_string( nDims );
		
		return v;
	}
	
	private static Validator_string[] validators = new Validator_string[MAX_CACHED];
	
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
}
